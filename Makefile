objects = wordPackDict.o sndmanifest.o itemmanifest.o residmap.o zpipe.o ogg.o convertpng.o

all : liDecompress.exe liCompress.exe recalcSoundManifest.exe strip.exe modManage.exe util/pullpakfiles.exe util/removeresc.exe util/repack.exe util/test.exe util/WinResource.exe
 
liDecompress.exe : $(objects) liDecompress.o threadDecompress.o
	g++ -Wall -O2 -o liDecompress.exe $(objects) liDecompress.o threadDecompress.o -lpng -lzlib -lttvfs -lvorbis -logg -ltinyxml2
liCompress.exe : $(objects) liCompress.o threadCompress.o
	g++ -Wall -O2 -o liCompress.exe $(objects) liCompress.o threadCompress.o -lpng -lzlib -lttvfs -lvorbis -logg -ltinyxml2
recalcSoundManifest.exe : recalcSoundManifest.o
	g++ -Wall -O2 -o recalcSoundManifest.exe recalcSoundManifest.o ogg.o -lvorbis -logg -ltinyxml2
strip.exe : strip.o
	g++ -Wall -O2 -o strip.exe strip.o
modManage.exe : modManage.o
	g++ -Wall -O2 -o modManage.exe modManage.o -lttvfs
util/pullpakfiles.exe : pullpakfiles.o
	g++ -Wall -O2 -o util/pullpakfiles.exe pullpakfiles.o
util/removeresc.exe : removeresc.o
	g++ -Wall -O2 -o util/removeresc.exe removeresc.o
util/repack.exe : repack.o
	g++ -Wall -O2 -o util/repack.exe repack.o -lttvfs
util/test.exe : test.o
	g++ -Wall -O2 -o util/test.exe test.o -lttvfs
util/WinResource.exe : WinResource.o
	g++ -Wall -O2 -o util/WinResource.exe WinResource.o
	
threadDecompress.o : threadDecompress.cpp pakDataTypes.h
	g++ -Wall -O2 -c threadDecompress.cpp
threadCompress.o : threadCompress.cpp pakDataTypes.h
	g++ -Wall -O2 -c threadCompress.cpp
wordPackDict.o : wordPackDict.cpp pakDataTypes.h
	g++ -Wall -O2 -c wordPackDict.cpp
sndmanifest.o : sndmanifest.cpp pakDataTypes.h sndmanifest.h
	g++ -Wall -O2 -c sndmanifest.cpp
residmap.o : residmap.cpp pakDataTypes.h residmap.h
	g++ -Wall -O2 -c residmap.cpp
itemmanifest.o : itemmanifest.cpp pakDataTypes.h
	g++ -Wall -O2 -c itemmanifest.cpp
zpipe.o : zpipe.c
	g++ -Wall -O2 -c zpipe.c
ogg.o : ogg.cpp pakDataTypes.h
	g++ -Wall -O2 -c ogg.cpp
convertpng.o : convertpng.cpp pakDataTypes.h
	g++ -Wall -O2 -c convertpng.cpp
liDecompress.o : liDecompress.cpp pakDataTypes.h
	g++ -Wall -O2 -c liDecompress.cpp
liCompress.o : liCompress.cpp pakDataTypes.h
	g++ -Wall -O2 -c liCompress.cpp
recalcSoundManifest.o : recalcSoundManifest.cpp pakDataTypes.h
	g++ -Wall -O2 -c recalcSoundManifest.cpp
strip.o : strip.cpp
	g++ -Wall -O2 -c strip.cpp
modManage.o : modManage.cpp pakDataTypes.h
	g++ -Wall -O2 -c modManage.cpp
pullpakfiles.o : util/pullpakfiles.cpp
	g++ -Wall -O2 -c util/pullpakfiles.cpp
removeresc.o : util/removeresc.cpp
	g++ -Wall -O2 -c util/removeresc.cpp
repack.o : util/repack.cpp
	g++ -Wall -O2 -c util/repack.cpp
test.o : util/test.cpp
	g++ -Wall -O2 -c util/test.cpp
WinResource.o : util/WinResource.cpp
	g++ -Wall -O2 -c util/WinResource.cpp


.PHONY : clean
clean :
	rm -rf liDecompress.exe liCompress.exe recalcSoundManifest.exe strip.exe modManage.exe util/pullpakfiles.exe util/removeresc.exe util/repack.exe util/test.exe util/WinResource.exe threadDecompress.o threadCompress.o liDecompress.o liCompress.o recalcSoundManifest.o strip.o modManage.o pullpakfiles.o removeresc.o repack.o test.o WinResource.o $(objects)