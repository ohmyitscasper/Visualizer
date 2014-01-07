#ifndef __AUDIOPROCESSING_H__
#define __AUDIOPROCESSING_H__

#define FRAMES 2048
#define SAMPLE_RATE 44100
#define MAX_VAL 8231

typedef signed short MY_TYPE;
extern MY_TYPE *Buffer;
extern double *newBuffer;
extern RtAudio adc;

int audioFn();
int exitFn();


#endif