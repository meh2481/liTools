SHELL=C:/Windows/System32/cmd.exe
objects = wordPackDict.o sndmanifest.o itemmanifest.o combodb.o residmap.o zpipe.o ogg.o image.o font.o loctex.o parse.o matrix.o tinyxml2.o
decompressobjects = $(objects) liDecompress.o threadDecompress.o
comressobjects = $(objects) liCompress.o threadCompress.o
HEADERPATH = -I./include
LIBPATH = -L./libs
LIB = -lpng -lzlib -lttvfs -lvorbis -logg -static-libgcc -static-libstdc++

all : liDecompress.exe liCompress.exe recalcSoundManifest.exe strip.exe modManage.exe util/pullpakfiles.exe util/removeresc.exe util/repack.exe util/hash.exe util/WinResource.exe
 
liDecompress.exe : $(decompressobjects)
	g++ -Wall -O2 -o $@ $(decompressobjects) $(LIBPATH) $(LIB)
liCompress.exe : $(comressobjects)
	g++ -Wall -O2 -o $@ $(comressobjects) $(LIBPATH) $(LIB)
recalcSoundManifest.exe : recalcSoundManifest.o
	g++ -Wall -O2 -o $@ $< ogg.o tinyxml2.o $(LIBPATH) $(LIB)
strip.exe : strip.o
	g++ -Wall -O2 -o $@ $<  -static-libgcc -static-libstdc++
modManage.exe : modManage.o
	g++ -Wall -O2 -o $@ $<  -static-libgcc -static-libstdc++
util/pullpakfiles.exe : util/pullpakfiles.o
	g++ -Wall -O2 -o $@ $<  -static-libgcc -static-libstdc++
util/removeresc.exe : util/removeresc.o
	g++ -Wall -O2 -o $@ $<  -static-libgcc -static-libstdc++
util/repack.exe : util/repack.o
	g++ -Wall -O2 -o $@ $< $(LIBPATH) $(LIB)
util/WinResource.exe : util/WinResource.o
	g++ -Wall -O2 -o $@ $<  -static-libgcc -static-libstdc++
util/hash.exe : util/hash.o
	g++ -Wall -O2 -o $@ $< residmap.o tinyxml2.o -static-libgcc -static-libstdc++
	
%.o: %.cpp
	g++ -O2 -c -MMD -o $@ $< $(HEADERPATH)

-include $(objects:.o=.d)
-include $(decompressobjects:.o=.d)
-include $(compressobjects:.o=.d)

.PHONY : clean
clean :
	rm -rf liDecompress.exe liCompress.exe recalcSoundManifest.exe strip.exe modManage.exe util/pullpakfiles.exe util/removeresc.exe util/repack.exe util/hash.exe util/WinResource.exe *.o *.d
