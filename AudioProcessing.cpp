#include "RtAudio.h"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include "AudioProcessing.h"

MY_TYPE *Buffer;
double *newBuffer;

int record( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
         double streamTime, RtAudioStreamStatus status, void *userData )
{
  if ( status )
    std::cout << "Stream overflow detected!" << std::endl;
  // Do something with the data in the "inputBuffer" buffer. 
//  std::cout << "WTF" << std::endl;

  Buffer = (signed short *)inputBuffer;

  for(unsigned int i = 0; i < FRAMES; i++) {
    newBuffer[i] = abs(Buffer[i]);
    newBuffer[i]/=MAX_VAL;
  //  std::cout << "newbuf: " << newBuffer[i] << std::endl;
  }

  return 0;
}

int audioFn() {
  if ( adc.getDeviceCount() < 1 ) {
    std::cout << "\nNo audio devices found!\n";
    exit( 0 );
  }
  RtAudio::StreamParameters parameters;
  parameters.deviceId = adc.getDefaultInputDevice();
  parameters.nChannels = 1;
  parameters.firstChannel = 0;
  unsigned int sampleRate = SAMPLE_RATE;
  unsigned int bufferFrames = FRAMES;

  newBuffer = (double *)malloc(sizeof(double) * FRAMES);

  try {
    adc.openStream( NULL, &parameters, RTAUDIO_SINT16,
                    sampleRate, &bufferFrames, &record);
    adc.startStream();
  }
  catch ( RtError& e ) {
    e.printMessage();
    exit( 0 );
  }
  
  std::cout << "\nAnalyzing...\n";
  return 0;
}

int exitFn()
{
  try {
    // Stop the stream
    adc.stopStream();
  }
  catch (RtError& e) {
    e.printMessage();
  }
  if ( adc.isStreamOpen() ) adc.closeStream();
  return 0;
}