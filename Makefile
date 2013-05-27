objects = wordPackDict.o sndmanifest.o itemmanifest.o residmap.o zpipe.o ogg.o image.o font.o loctex.o parse.o
decompressobjects = $(objects) liDecompress.o threadDecompress.o
comressobjects = $(objects) liCompress.o threadCompress.o
HEADER = -I./include
LIB = -L./libs -lpng -lzlib -lttvfs -lvorbis -logg -ltinyxml2

all : liDecompress.exe liCompress.exe recalcSoundManifest.exe strip.exe modManage.exe util/pullpakfiles.exe util/removeresc.exe util/repack.exe util/test.exe util/WinResource.exe
 
liDecompress.exe : $(decompressobjects)
	g++ -Wall -O2 -o %@ $(decompressobjects) $(LIB)
liCompress.exe : $(comressobjects)
	g++ -Wall -O2 -o %@ $(comressobjects) $(LIB)
recalcSoundManifest.exe : recalcSoundManifest.o
	g++ -Wall -O2 -o %@ $< ogg.o $(LIB)
strip.exe : strip.o
	g++ -Wall -O2 -o %@ $<
modManage.exe : modManage.o
	g++ -Wall -O2 -o %@ $<
util/pullpakfiles.exe : pullpakfiles.o
	g++ -Wall -O2 -o %@ $<
util/removeresc.exe : removeresc.o
	g++ -Wall -O2 -o %@ $<
util/repack.exe : repack.o
	g++ -Wall -O2 -o %@ $< $(LIB)
util/test.exe : test.o
	g++ -Wall -O2 -o %@ $< $(LIB)
util/WinResource.exe : WinResource.o
	g++ -Wall -O2 -o %@ $<
	
%.o: %.cpp
	g++ -c -MMD -o $@ $< $(HEADER)
	
#threadDecompress.o : threadDecompress.cpp pakDataTypes.h
#	g++ -Wall -O2 -c threadDecompress.cpp $(HEADER)
#threadCompress.o : threadCompress.cpp pakDataTypes.h
#	g++ -Wall -O2 -c threadCompress.cpp $(HEADER)
#wordPackDict.o : wordPackDict.cpp pakDataTypes.h
#	g++ -Wall -O2 -c wordPackDict.cpp $(HEADER)
#sndmanifest.o : sndmanifest.cpp pakDataTypes.h sndmanifest.h
#	g++ -Wall -O2 -c sndmanifest.cpp $(HEADER)
#residmap.o : residmap.cpp pakDataTypes.h residmap.h
#	g++ -Wall -O2 -c residmap.cpp $(HEADER)
#itemmanifest.o : itemmanifest.cpp pakDataTypes.h
#	g++ -Wall -O2 -c itemmanifest.cpp $(HEADER)
#zpipe.o : zpipe.cpp pakDataTypes.h
#	g++ -Wall -O2 -c zpipe.cpp $(HEADER)
#ogg.o : ogg.cpp pakDataTypes.h
#	g++ -Wall -O2 -c ogg.cpp $(HEADER)
#image.o : image.cpp pakDataTypes.h
#	g++ -Wall -O2 -c image.cpp $(HEADER)
#liDecompress.o : liDecompress.cpp pakDataTypes.h
#	g++ -Wall -O2 -c liDecompress.cpp $(HEADER)
#liCompress.o : liCompress.cpp pakDataTypes.h
#	g++ -Wall -O2 -c liCompress.cpp $(HEADER)
#recalcSoundManifest.o : recalcSoundManifest.cpp pakDataTypes.h
#	g++ -Wall -O2 -c recalcSoundManifest.cpp $(HEADER)
#strip.o : strip.cpp
#	g++ -Wall -O2 -c strip.cpp $(HEADER)
#modManage.o : modManage.cpp pakDataTypes.h
#	g++ -Wall -O2 -c modManage.cpp $(HEADER)
#pullpakfiles.o : util/pullpakfiles.cpp
#	g++ -Wall -O2 -c util/pullpakfiles.cpp $(HEADER)
#removeresc.o : util/removeresc.cpp
#	g++ -Wall -O2 -c util/removeresc.cpp $(HEADER)
#repack.o : util/repack.cpp
#	g++ -Wall -O2 -c util/repack.cpp $(HEADER)
#test.o : util/test.cpp
#	g++ -Wall -O2 -c util/test.cpp $(HEADER)
#WinResource.o : util/WinResource.cpp
#	g++ -Wall -O2 -c util/WinResource.cpp $(HEADER)
#font.o : font.cpp pakDataTypes.h
#	g++ -Wall -O2 -c font.cpp $(HEADER)
#loctex.o : loctex.cpp pakDataTypes.h
#	g++ -Wall -O2 -c loctex.cpp $(HEADER)

-include $(objects:.o=.d)
-include $(decompressobjects:.o=.d)
-include $(compressobjects:.o=.d)

.PHONY : clean
clean :
	rm -rf *.exe *.o util/*.exe *.d
