objects = wordPackDict.o sndmanifest.o itemmanifest.o residmap.o zpipe.o ogg.o image.o font.o loctex.o
HEADER = -I./include
LIB = -L./libs

all : liDecompress.exe liCompress.exe recalcSoundManifest.exe strip.exe modManage.exe util/pullpakfiles.exe util/removeresc.exe util/repack.exe util/test.exe util/WinResource.exe
 
liDecompress.exe : $(objects) liDecompress.o threadDecompress.o
	g++ -Wall -O2 -o liDecompress.exe $(objects) liDecompress.o threadDecompress.o $(LIB) -lpng -lzlib -lttvfs -lvorbis -logg -ltinyxml2
liCompress.exe : $(objects) liCompress.o threadCompress.o
	g++ -Wall -O2 -o liCompress.exe $(objects) liCompress.o threadCompress.o $(LIB) -lpng -lzlib -lttvfs -lvorbis -logg -ltinyxml2
recalcSoundManifest.exe : recalcSoundManifest.o
	g++ -Wall -O2 -o recalcSoundManifest.exe recalcSoundManifest.o ogg.o $(LIB) -lvorbis -logg -ltinyxml2
strip.exe : strip.o
	g++ -Wall -O2 -o strip.exe strip.o
modManage.exe : modManage.o
	g++ -Wall -O2 -o modManage.exe modManage.o
util/pullpakfiles.exe : pullpakfiles.o
	g++ -Wall -O2 -o util/pullpakfiles.exe pullpakfiles.o
util/removeresc.exe : removeresc.o
	g++ -Wall -O2 -o util/removeresc.exe removeresc.o
util/repack.exe : repack.o
	g++ -Wall -O2 -o util/repack.exe repack.o $(LIB) -lttvfs
util/test.exe : test.o
	g++ -Wall -O2 -o util/test.exe test.o $(LIB) -lttvfs
util/WinResource.exe : WinResource.o
	g++ -Wall -O2 -o util/WinResource.exe WinResource.o
	
threadDecompress.o : threadDecompress.cpp pakDataTypes.h
	g++ -Wall -O2 -c threadDecompress.cpp $(HEADER)
threadCompress.o : threadCompress.cpp pakDataTypes.h
	g++ -Wall -O2 -c threadCompress.cpp $(HEADER)
wordPackDict.o : wordPackDict.cpp pakDataTypes.h
	g++ -Wall -O2 -c wordPackDict.cpp $(HEADER)
sndmanifest.o : sndmanifest.cpp pakDataTypes.h sndmanifest.h
	g++ -Wall -O2 -c sndmanifest.cpp $(HEADER)
residmap.o : residmap.cpp pakDataTypes.h residmap.h
	g++ -Wall -O2 -c residmap.cpp $(HEADER)
itemmanifest.o : itemmanifest.cpp pakDataTypes.h
	g++ -Wall -O2 -c itemmanifest.cpp $(HEADER)
zpipe.o : zpipe.cpp pakDataTypes.h
	g++ -Wall -O2 -c zpipe.cpp $(HEADER)
ogg.o : ogg.cpp pakDataTypes.h
	g++ -Wall -O2 -c ogg.cpp $(HEADER)
image.o : image.cpp pakDataTypes.h
	g++ -Wall -O2 -c image.cpp $(HEADER)
liDecompress.o : liDecompress.cpp pakDataTypes.h
	g++ -Wall -O2 -c liDecompress.cpp $(HEADER)
liCompress.o : liCompress.cpp pakDataTypes.h
	g++ -Wall -O2 -c liCompress.cpp $(HEADER)
recalcSoundManifest.o : recalcSoundManifest.cpp pakDataTypes.h
	g++ -Wall -O2 -c recalcSoundManifest.cpp $(HEADER)
strip.o : strip.cpp
	g++ -Wall -O2 -c strip.cpp $(HEADER)
modManage.o : modManage.cpp pakDataTypes.h
	g++ -Wall -O2 -c modManage.cpp $(HEADER)
pullpakfiles.o : util/pullpakfiles.cpp
	g++ -Wall -O2 -c util/pullpakfiles.cpp $(HEADER)
removeresc.o : util/removeresc.cpp
	g++ -Wall -O2 -c util/removeresc.cpp $(HEADER)
repack.o : util/repack.cpp
	g++ -Wall -O2 -c util/repack.cpp $(HEADER)
test.o : util/test.cpp
	g++ -Wall -O2 -c util/test.cpp $(HEADER)
WinResource.o : util/WinResource.cpp
	g++ -Wall -O2 -c util/WinResource.cpp $(HEADER)
font.o : font.cpp pakDataTypes.h
	g++ -Wall -O2 -c font.cpp $(HEADER)
loctex.o : loctex.cpp pakDataTypes.h
	g++ -Wall -O2 -c loctex.cpp $(HEADER)


.PHONY : clean
clean :
	rm -rf liDecompress.exe liCompress.exe recalcSoundManifest.exe strip.exe modManage.exe util/pullpakfiles.exe util/removeresc.exe util/repack.exe util/test.exe util/WinResource.exe threadDecompress.o threadCompress.o liDecompress.o liCompress.o recalcSoundManifest.o strip.o modManage.o pullpakfiles.o removeresc.o repack.o test.o WinResource.o $(objects)
