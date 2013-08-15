SHELL=C:/Windows/System32/cmd.exe
objects = wordPackDict.o sndmanifest.o itemmanifest.o combodb.o residmap.o zpipe.o ogg.o image.o font.o loctex.o parse.o matrix.o
decompressobjects = $(objects) liDecompress.o threadDecompress.o
comressobjects = $(objects) liCompress.o threadCompress.o
HEADER = -I./include
LIB = -L./libs -lpng -lzlib -lttvfs -lvorbis -logg -ltinyxml2

all : liDecompress.exe liCompress.exe recalcSoundManifest.exe strip.exe modManage.exe util/pullpakfiles.exe util/removeresc.exe util/repack.exe util/test.exe util/WinResource.exe
 
liDecompress.exe : $(decompressobjects)
	g++ -Wall -O2 -o $@ $(decompressobjects) $(LIB)
liCompress.exe : $(comressobjects)
	g++ -Wall -O2 -o $@ $(comressobjects) $(LIB)
recalcSoundManifest.exe : recalcSoundManifest.o
	g++ -Wall -O2 -o $@ $< ogg.o $(LIB)
strip.exe : strip.o
	g++ -Wall -O2 -o $@ $<
modManage.exe : modManage.o
	g++ -Wall -O2 -o $@ $<
util/pullpakfiles.exe : util/pullpakfiles.o
	g++ -Wall -O2 -o $@ $<
util/removeresc.exe : util/removeresc.o
	g++ -Wall -O2 -o $@ $<
util/repack.exe : util/repack.o
	g++ -Wall -O2 -o $@ $< $(LIB)
util/test.exe : util/test.o
	g++ -Wall -O2 -o $@ $< $(LIB)
util/WinResource.exe : util/WinResource.o
	g++ -Wall -O2 -o $@ $<
	
%.o: %.cpp
	g++ -O2 -c -MMD -o $@ $< $(HEADER)

-include $(objects:.o=.d)
-include $(decompressobjects:.o=.d)
-include $(compressobjects:.o=.d)

.PHONY : clean
clean :
	rm -rf *.exe *.o util/*.exe *.d
