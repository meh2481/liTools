// code to convert little inferno sound resources to ogg files
// the version of libogg that you use here must be version 1.1.4
// dec 12, 2012 - allan blomquist
 
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <vorbis/codec.h>
#include <vector>
#include <cstring>
#include "pakDataTypes.h"
using namespace std;
 
// MUST use libogg-1.1.4 - not any other version!
#include <ogg/ogg.h>

const int kStdWorkingSetSizeBytes = 108204;
 
struct SoundHeader
{
        int totalSampleCountPerChannel;
        int oggTotalDataSizeBytes;
        int vorbisMarkersSizeBytes;
        int vorbisWorkingSetSizeBytes;
        int vorbisPacketsSizeBytes;
        int samplesPerSec;
        int channels;
};
 
struct StreamMarker
{
        int packetStreamByteOffset;
        int pcmSampleOffset;
        int packetIdx;
};
 
struct VorbisPacket
{
        int sizeBytes;
        unsigned char *pData;
};
 
int binaryToOgg( const char* in, const char* out )
{
        FILE *fSrc = fopen( in, "rb" );
        FILE *fDest = fopen( out, "wb" );
 
        // read header
        SoundHeader hdr;
        fread( &hdr, 1, sizeof(hdr), fSrc );
 
        // read stream markers
        int numMarkers = hdr.vorbisMarkersSizeBytes / sizeof(StreamMarker);
        StreamMarker *pMarkers = new StreamMarker[ numMarkers ];
        fread( pMarkers, sizeof(StreamMarker), numMarkers, fSrc );
 
        // setup for making an ogg stream - serial number isn't going to matter
        ogg_stream_state os;
        const int kOggStreamSerialNo = 0x00001234;
        ogg_stream_init( &os, kOggStreamSerialNo );
 
        int doneBytes = 0;
        int donePackets = 0;
 
        // always skip the first marker
        int doneMarkers = 1;
 
        // convert raw vorbis packets to ogg stream
        while( doneBytes < hdr.vorbisPacketsSizeBytes )
        {
                // packet size comes first
                short packetSizeBytes;
                fread( &packetSizeBytes, 1, sizeof(packetSizeBytes), fSrc );
 
                // followed by packet data
                unsigned char *pPacketData = new unsigned char[ packetSizeBytes ];
                fread( pPacketData, 1, packetSizeBytes, fSrc );
 
                // set up an ogg packet with the data we just read
                ogg_packet op;
                op.packet = pPacketData;
                op.bytes = packetSizeBytes;
 
                // granule pos mostly maps 1:1 with markers but there are a few special cases for the first few packets
                if( (donePackets == 0) || (donePackets == 2) )
                {
                        op.granulepos = 0;
                }
                else
                {
                        op.granulepos = -1;
 
                        StreamMarker *pNextMarker = pMarkers + doneMarkers;
                        if( (donePackets >= 4) && (pNextMarker->packetStreamByteOffset == doneBytes) )
                        {
                                op.granulepos = pNextMarker->pcmSampleOffset;
                                ++doneMarkers;
                        }
                }
 
                int packetStreamAdvanceBytes = sizeof(packetSizeBytes) + packetSizeBytes;
                bool lastPacket = (doneBytes + packetStreamAdvanceBytes) >= hdr.vorbisPacketsSizeBytes;
 
                // these flag values are what come out of libogg when decoding the original files
                op.b_o_s = (donePackets == 0) ? 256 : 0;
                op.e_o_s = (lastPacket) ? 512 : 0;
 
                op.packetno = donePackets;
 
                // give packet to ogg stream
                ogg_stream_packetin( &os, &op );
                delete [] pPacketData;
 
                doneBytes += packetStreamAdvanceBytes;
                ++donePackets;
 
                // flush ogg stream after first 3 packets (as required by spec) and at end of stream
                if( (donePackets == 3) || lastPacket )
                {
                        while( true )
                        {
                                ogg_page og;
                                int result = ogg_stream_flush( &os, &og );
                                if( result == 0 ) break;
                                fwrite( og.header, 1, og.header_len, fDest );
                                fwrite( og.body, 1, og.body_len, fDest );
                        }
                }
        }
 
        // cleanup
        ogg_stream_clear(&os);
 
        delete [] pMarkers;
 
        fclose( fDest );
        fclose( fSrc );
 
        return 0;
}

// code to convert ogg files to little inferno sound resources
// dec 13, 2012 - allan blomquist
 
int oggToBinary( const char* in, const char* out )
{
        FILE *fSrc = fopen( in, "rb" );
 
        // get size of ogg src file
        fseek( fSrc, 0, SEEK_END );
        int oggSizeBytes = ftell( fSrc );
        fseek( fSrc, 0, SEEK_SET );
 
        ogg_sync_state oy;
        ogg_sync_init( &oy );
 
        // read in the entire ogg file and give it to the ogg sync state
        char *pBuffer = ogg_sync_buffer( &oy, oggSizeBytes );
        fread( pBuffer, 1, oggSizeBytes, fSrc );
        ogg_sync_wrote( &oy, oggSizeBytes );
        fclose( fSrc );
 
        // get the first ogg page so we can see what the serial number we need to use is
        ogg_page og;
        ogg_sync_pageout( &oy, &og );
        int serialNumber = ogg_page_serialno( &og );
 
        // set up an ogg stream and load it up with all of the pages we have
        ogg_stream_state os;
        ogg_stream_init( &os, serialNumber );
 
        do
        {
                ogg_stream_pagein( &os, &og );
        }
        while( ogg_sync_pageout( &oy, &og ) > 0 );
 
        // vorbis init
        vorbis_info vi;
        vorbis_info_init( &vi );
 
        vorbis_comment vc;
        vorbis_comment_init( &vc );
 
        vorbis_block vb;
        vorbis_dsp_state vd;
 
        // process all packets
        ogg_packet op;
        int sampleCounter = 0;
        int packetStreamBytes = 0;
        std::vector< StreamMarker > markers;
        std::vector< VorbisPacket > packets;
        while( ogg_stream_packetout( &os, &op ) > 0 )
        {
                // use granulepos from ogg packet to set up stream markers
                // markers are almost 1:1 with ogg but with a slight change on the first audio packet
                int granulePos = -1;
                if( packets.size() == 3 )
                {
                        granulePos = 0;
                }
                else if( packets.size() > 3 )
                {
                        granulePos = (int)op.granulepos;
                }
 
                if( granulePos >= 0 )
                {
                        StreamMarker marker;
                        marker.packetStreamByteOffset = packetStreamBytes;
                        marker.pcmSampleOffset = granulePos;
                        marker.packetIdx = packets.size();
                        markers.push_back( marker );
                }
 
                // feed packets to vorbis so we can ask about stream meta data later
                if( packets.size() < 3 )
                {
                        vorbis_synthesis_headerin( &vi, &vc, &op );
                        if( packets.size() == 2 )
                        {
                                vorbis_synthesis_init( &vd, &vi );
                                vorbis_block_init( &vd, &vb );
                        }
                }
                else
                {
                        vorbis_synthesis( &vb, &op );
                        vorbis_synthesis_blockin( &vd, &vb );
 
                        float **pcm;
                        int samples = vorbis_synthesis_pcmout( &vd, &pcm );
                        vorbis_synthesis_read( &vd, samples );
                        sampleCounter += samples;
                }
 
                // remember packet data to use later for output
                VorbisPacket packet;
                packet.sizeBytes = op.bytes;
                packet.pData = new unsigned char[ op.bytes ];
                memcpy( packet.pData, op.packet, op.bytes );
                packets.push_back( packet );
 
                packetStreamBytes += sizeof(short) + op.bytes;
        }
 
        // cleanup
        vorbis_block_clear( &vb );
        vorbis_dsp_clear( &vd );       
        vorbis_comment_clear( &vc );
 
        ogg_stream_clear( &os );
        ogg_sync_clear( &oy );
 
        // now we can write the game sound resource output
        FILE *fDest = fopen( out, "wb" );
 
        //const int kStdWorkingSetSizeBytes = 108204;
 
        // write header
        SoundHeader hdr;
        hdr.totalSampleCountPerChannel = sampleCounter;
        hdr.vorbisWorkingSetSizeBytes = kStdWorkingSetSizeBytes;
        hdr.vorbisMarkersSizeBytes = markers.size() * sizeof(StreamMarker);
        hdr.vorbisPacketsSizeBytes = packetStreamBytes;
        hdr.oggTotalDataSizeBytes = hdr.vorbisMarkersSizeBytes + hdr.vorbisPacketsSizeBytes;
        hdr.samplesPerSec = vi.rate;
        hdr.channels = vi.channels;
        fwrite( &hdr, 1, sizeof(hdr), fDest );
 
        // write markers
        fwrite( &markers[0], 1, hdr.vorbisMarkersSizeBytes, fDest );
 
        // write packets
        for( unsigned int i=0; i < packets.size(); ++i )
        {
                short packetSizeBytes = packets[i].sizeBytes;
                fwrite( &packetSizeBytes, 1, sizeof(packetSizeBytes), fDest );
                fwrite( packets[i].pData, 1, packets[i].sizeBytes, fDest );
        }
 
        fclose( fDest );
 
        // cleanup
        for( unsigned int i=0; i < packets.size(); ++i )
        {
                delete [] packets[i].pData;
        }
 
        vorbis_info_clear( &vi );
 
        return 0;
}

takeRecord getOggData( const char* cFile )
{
        FILE *fSrc = fopen( cFile, "rb" );
		if(fSrc == NULL)
		{
			//std::cout << "Could not open file " << cFile << " for reading. Skipping..." << std::endl;
			takeRecord tr;
			tr.resId = 0;
			return tr;
		}
 
        // get size of ogg src file
        fseek( fSrc, 0, SEEK_END );
        int oggSizeBytes = ftell( fSrc );
        fseek( fSrc, 0, SEEK_SET );
 
        ogg_sync_state oy;
        ogg_sync_init( &oy );
 
        // read in the entire ogg file and give it to the ogg sync state
        char *pBuffer = ogg_sync_buffer( &oy, oggSizeBytes );
        fread( pBuffer, 1, oggSizeBytes, fSrc );
        ogg_sync_wrote( &oy, oggSizeBytes );
        fclose( fSrc );
 
        // get the first ogg page so we can see what the serial number we need to use is
        ogg_page og;
        ogg_sync_pageout( &oy, &og );
        int serialNumber = ogg_page_serialno( &og );
 
        // set up an ogg stream and load it up with all of the pages we have
        ogg_stream_state os;
        ogg_stream_init( &os, serialNumber );
 
        do
        {
                ogg_stream_pagein( &os, &og );
        }
        while( ogg_sync_pageout( &oy, &og ) > 0 );
 
        // vorbis init
        vorbis_info vi;
        vorbis_info_init( &vi );
 
        vorbis_comment vc;
        vorbis_comment_init( &vc );
 
        vorbis_block vb;
        vorbis_dsp_state vd;
 
        // process all packets
        ogg_packet op;
        int sampleCounter = 0;
        int packetStreamBytes = 0;
        std::vector< StreamMarker > markers;
        std::vector< VorbisPacket > packets;
        while( ogg_stream_packetout( &os, &op ) > 0 )
        {
                // use granulepos from ogg packet to set up stream markers
                // markers are almost 1:1 with ogg but with a slight change on the first audio packet
                int granulePos = -1;
                if( packets.size() == 3 )
                {
                        granulePos = 0;
                }
                else if( packets.size() > 3 )
                {
                        granulePos = (int)op.granulepos;
                }
 
                if( granulePos >= 0 )
                {
                        StreamMarker marker;
                        marker.packetStreamByteOffset = packetStreamBytes;
                        marker.pcmSampleOffset = granulePos;
                        marker.packetIdx = packets.size();
                        markers.push_back( marker );
                }
 
                // feed packets to vorbis so we can ask about stream meta data later
                if( packets.size() < 3 )
                {
                        vorbis_synthesis_headerin( &vi, &vc, &op );
                        if( packets.size() == 2 )
                        {
                                vorbis_synthesis_init( &vd, &vi );
                                vorbis_block_init( &vd, &vb );
                        }
                }
                else
                {
                        vorbis_synthesis( &vb, &op );
                        vorbis_synthesis_blockin( &vd, &vb );
 
                        float **pcm;
                        int samples = vorbis_synthesis_pcmout( &vd, &pcm );
                        vorbis_synthesis_read( &vd, samples );
                        sampleCounter += samples;
                }
 
                // remember packet data to use later for output
                VorbisPacket packet;
                packet.sizeBytes = op.bytes;
				//DXR: Ignore packet data, since we're only concerned with size here
                packets.push_back( packet );
 
                packetStreamBytes += sizeof(short) + op.bytes;
        }
 
        // cleanup
        vorbis_block_clear( &vb );
        vorbis_dsp_clear( &vd );       
        vorbis_comment_clear( &vc );
 
        ogg_stream_clear( &os );
        ogg_sync_clear( &oy );
 
        // now we can write the game sound resource output
 
        
 
        // Figure out header
		takeRecord tr;
		tr.resId = 1;
	    tr.channels = vi.channels;
		tr.samplesPerSec = vi.rate;
		tr.sampleCountPerChannel = sampleCounter;
		tr.vorbisWorkingSetSizeBytes = kStdWorkingSetSizeBytes;
		tr.vorbisMarkersSizeBytes = markers.size() * sizeof(StreamMarker);
		tr.vorbisPacketsSizeBytes = packetStreamBytes;
 
        vorbis_info_clear( &vi );
 
        return tr;
}
