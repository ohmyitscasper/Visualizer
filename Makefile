CC=g++
CFLAGS= -Wall -g -I./includes
LDFLAGS= -lGL -lglut -lGLU -lpthread -lasound -lfftw3 -lm
DEFS     =   -DHAVE_GETTIMEOFDAY -D__LINUX_ALSA__
OBJECT_PATH = objects

.PHONY:default
default: all

Visualizer: Animation.o AudioProcessing.o
	$(CC) $(CFLAGS) -o Visualizer Animation.o AudioProcessing.o $(OBJECT_PATH)/RtAudio.o $(LDFLAGS)

Animation.o: Animation.cpp
	$(CC) $(CFLAGS) -c Animation.cpp -o Animation.o $(LDFLAGS)		

AudioProcessing.o: AudioProcessing.cpp
	$(CC) $(CFLAGS) $(DEFS) -c AudioProcessing.cpp -o AudioProcessing.o

#AudioProcessing: AudioProcessing.cpp
#	$(CC) $(CFLAGS) $(DEFS) -o AudioProcessing AudioProcessing.cpp $(OBJECT_PATH)/RtAudio.o $(LDFLAGS)
	
#playsaw: playsaw.cpp 
#	$(CC) $(CFLAGS) $(DEFS) -o playsaw playsaw.cpp $(OBJECT_PATH)/RtAudio.o $(LDFLAGS)
#
#record: record.cpp
#	$(CC) $(CFLAGS) $(DEFS) -o record record.cpp $(OBJECT_PATH)/RtAudio.o $(LDFLAGS)

.PHONY:all
all: clean Visualizer

.PHONY:clean
clean:
	rm -rf AudioProcessing.o Animation.o Visualizer
