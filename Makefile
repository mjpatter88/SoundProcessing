#put the compiler names in a variable for easy switching
CC=gcc
CXX=g++
JAVA=javac

#Basic flags used by all builds
BASE=-pedantic -Wall

#Flag to include math libraries
MATH_FLAG=-lm

CFLAGS=${BASE}

#Makes every part of the assignment
all : gensine info split combine static mix reverb merge FHighLow SoundProcessor gendtmf gendtmf2 dtmf

#Part A: Gensine
gensine : gensine.o sine_lib.o util.o
	$(CC) -o  gensine gensine.o sine_lib.o util.o $(MATH_FLAG)
	
gensine.o : gensine.c sine_lib.h util.h
	$(CC) -c $(CFLAGS) gensine.c 
	
sine_lib.o : sine_lib.c sine_lib.h util.h
	$(CC) -c $(CFLAGS) sine_lib.c
	
util.o : util.c util.h
	$(CC) -c $(CFLAGS) -fPIC util.c

#Part B: Info
info : info.o input_lib.o util.o
	$(CC) -o info info.o input_lib.o util.o

info.o : info.c input_lib.h
	$(CC) -c $(CFLAGS) info.c
	
input_lib.o : input_lib.c input_lib.h util.h
	$(CC) -c $(CFLAGS) -fPIC input_lib.c 

#Part C: Split
split : split.o input_lib.o util.o
	$(CC) -o split split.o input_lib.o util.o

split.o : split.c input_lib.h
	$(CC) -c $(CFLAGS) split.c

#Part C: combine
combine : combine.o input_lib.o util.o
	$(CC) -o combine combine.o input_lib.o util.o

combine.o : combine.c input_lib.h
	$(CC) -c $(CFLAGS) combine.c

#Part D: static
static : static.o util.o
	$(CC) -o static static.o util.o $(MATH_FLAG)

static.o : static.c util.h
	$(CC) -c $(CFLAGS) static.c

#Part E: mix
mix : mix.o input_lib.o util.o
	$(CC) -o mix mix.o input_lib.o util.o $(MATH_FLAG)

mix.o : mix.c input_lib.h util.h
	$(CC) -c $(CFLAGS) mix.c

gendtmf : gendtmf.sh
	chmod +x gendtmf.sh

gendtmf2 : gendtmf2.sh
	chmod +x gendtmf2.sh

#Part G: Merge (used with gendtmf2)
merge : merge.o input_lib.o util.o
	$(CC) -o merge merge.o input_lib.o util.o
	
merge.o : merge.c input_lib.h util.h
	$(CC) -c $(CFLAGS) merge.c

#Part H FHighLow (Java)
FHighLow : FHighLow.java fourier_lib.so
	$(JAVA) FHighLow.java
	javah FHighLow

# c stuff for this part
#This seems to work for pyrite also, but if not then use the following line
#for pyrite: gcc -fPIC -shared -I/usr/java/jdk/include/ -I/usr/java/jdk/include/linux/
fourier_lib.so : fourier_lib.o util.o input_lib.o fourier.o
	gcc -shared -fPIC -I/usr/lib64/jvm/java-6-sun-1.6.0.15/include \
	-I/usr/lib64/jvm/java-6-sun-1.6.0.15/include/linux fourier_lib.o \
	util.o fourier.o input_lib.o -o fourier_lib.so $(MATH_FLAG)

fourier_lib.o : fourier_lib.c util.h input_lib.h fourier.h
	gcc -shared -fPIC -I/usr/lib64/jvm/java-6-sun-1.6.0.15/include \
	-I/usr/lib64/jvm/java-6-sun-1.6.0.15/include/linux -c $(CFLAGS) fourier_lib.c

fourier.o : fourier.c fourier.h util.h input_lib.h
	$(CC) -c $(CFLAGS) -fPIC fourier.c

#Part I SoundProcessor (Java)
SoundProcessor : SoundProcessor.java SoundInfo GraphDisplay
	$(JAVA) SoundProcessor.java

SoundInfo : SoundInfo.java fourier_lib.so
	$(JAVA) SoundInfo.java
	javah SoundInfo

GraphDisplay : GraphDisplay.java
	$(JAVA) GraphDisplay.java

#Part K: reverb
reverb : reverb.o input_lib.o util.o
	$(CC) -o reverb reverb.o input_lib.o util.o $(MATH_FLAG)
	
reverb.o : reverb.c input_lib.h util.h
	$(CC) -c $(CFLAGS) reverb.c

#Part J: dtmf
dtmf : dtmf.o input_lib.o util.o fourier.o
	$(CC) -o dtmf dtmf.o input_lib.o util.o fourier.o $(MATH_FLAG)
	
dtmf.o : dtmf.c input_lib.h util.h fourier.h
	$(CC) -c $(CFLAGS) dtmf.c

clean :
	rm -f *.o
	
distclean :
	rm -f *.o
	rm -f gensine
	rm -f info
	rm -f split
	rm -f combine
	rm -f static
	rm -f mix
	rm -f reverb
	rm -f merge
	rm -f FHighLow.class
	rm -f SoundProcessor.class
	rm -f dtmf