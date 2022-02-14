//records 10 seconds of audio and stores it to the SD file//

#include <SPI.h>
#include <SD.h>
#include <SD_t3.h>
#include <SerialFlash.h>
#include <Bounce2.h>
#include <math.h>
#include <Audio.h>
#include <Wire.h>

//write wav
unsigned long ChunkSize = 0L;
unsigned long Subchunk1Size = 16;
unsigned int AudioFormat = 1;
unsigned int numChannels = 1;
unsigned long sampleRate = 44100;
unsigned int bitsPerSample = 16;
unsigned long byteRate = sampleRate*numChannels*(bitsPerSample/8);// samplerate x channels x (bitspersample / 8)
unsigned int blockAlign = numChannels*bitsPerSample/8;
unsigned long Subchunk2Size = 0L;
unsigned long recByteSaved = 0L;
unsigned long NumSamples = 0L;
byte byte1, byte2, byte3, byte4;

const int myInput = AUDIO_INPUT_LINEIN;
//const int myInput = AUDIO_INPUT_MIC;

AudioPlaySdWav           audioSD;
AudioInputI2S            audioInput;
AudioOutputI2S           audioOutput;
AudioRecordQueue         queue1;

//recod from mic
AudioConnection          patchCord1(audioInput, 0, queue1, 0);
AudioConnection          patchCord2(audioSD, 0, audioOutput, 0);
AudioConnection          patchCord3(audioSD, 0, audioOutput, 1);

AudioControlSGTL5000     audioShield;

//to be added to control recording and playback on button press
#define Buttonpin1 36
Bounce2::Button button1 = Bounce2::Button();
int mode = 0;  // 0=stopped, 1=recording, 2=playing
int prev_mode = 0;

File frec;

elapsedMillis  msecs;
int t = 0;

// Define SD card pins (using the SD card on teensy as it faster than the audio shield slot)
#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  11
#define SDCARD_SCK_PIN   13

void setup() {

  button1.attach(Buttonpin1, INPUT);
  button1.interval(15);
  
  Serial.begin(9600); //debug comms
  AudioMemory(60);

  audioShield.enable();
  audioShield.inputSelect(myInput);
  audioShield.volume(0.5);  //0-1

  SD.begin(BUILTIN_SDCARD);
  if (!(SD.begin(BUILTIN_SDCARD))) {
    // stop here, but print a message repetitively
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }

  startRecording(); //start recording at end of setup
  
  mode=1; //set mode to 1 to keep recording 
}


void loop() {
//very crude loop for recording 10 seconds and start to playback to output afterwards
  if (t > 10000 && t < 12000){
    stopRecording();
    delay (2000);
    startPlaying();
  }
  if (t <= 10000) {
    t= msecs;
    continueRecording();
  }
  
}

void startRecording() {
  Serial.println("startRecording");
  if (SD.exists("RECORD1.WAV")) { //would be good to add a method to iterate recordings instead of overwritting
    SD.remove("RECORD1.WAV");
  }
  frec = SD.open("RECORD1.WAV", FILE_WRITE);
  if (frec) {
    queue1.begin();
    recByteSaved = 0L;
  }
}

void continueRecording() {
  
  if (queue1.available() >= 2) {
    byte buffer[512];
    memcpy(buffer, queue1.readBuffer(), 256);
    queue1.freeBuffer();
    memcpy(buffer + 256, queue1.readBuffer(), 256);
    queue1.freeBuffer();
    // write all 512 bytes to the SD card
    frec.write(buffer, 512);
    recByteSaved += 512;
  }
}

void stopRecording() {
  Serial.println("stopRecording");
  queue1.end();{
    
  while (queue1.available() > 0) {
    frec.write((byte*)queue1.readBuffer(), 256);
    queue1.freeBuffer();
    recByteSaved += 256;
    }
  writeOutHeader();
  frec.close();
  }
}


void startPlaying() {
  Serial.println("startPlaying");
  audioSD.play("RECORD.WAV");


}


void stopPlaying() {
  Serial.println("stopPlaying");
  audioSD.stop();
}

void writeOutHeader() { 
// writes header file to turn raw file to wav
// update WAV header with final filesize/datasize
//  NumSamples = (recByteSaved*8)/bitsPerSample/numChannels;
//  Subchunk2Size = NumSamples*numChannels*bitsPerSample/8; // number of samples x number of channels x number of bytes per sample
  Subchunk2Size = recByteSaved;
  ChunkSize = Subchunk2Size + 36;
  frec.seek(0);
  frec.write("RIFF");
  byte1 = ChunkSize & 0xff;
  byte2 = (ChunkSize >> 8) & 0xff;
  byte3 = (ChunkSize >> 16) & 0xff;
  byte4 = (ChunkSize >> 24) & 0xff;  
  frec.write(byte1);  frec.write(byte2);  frec.write(byte3);  frec.write(byte4);
  frec.write("WAVE");
  frec.write("fmt ");
  byte1 = Subchunk1Size & 0xff;
  byte2 = (Subchunk1Size >> 8) & 0xff;
  byte3 = (Subchunk1Size >> 16) & 0xff;
  byte4 = (Subchunk1Size >> 24) & 0xff;  
  frec.write(byte1);  frec.write(byte2);  frec.write(byte3);  frec.write(byte4);
  byte1 = AudioFormat & 0xff;
  byte2 = (AudioFormat >> 8) & 0xff;
  frec.write(byte1);  frec.write(byte2); 
  byte1 = numChannels & 0xff;
  byte2 = (numChannels >> 8) & 0xff;
  frec.write(byte1);  frec.write(byte2); 
  byte1 = sampleRate & 0xff;
  byte2 = (sampleRate >> 8) & 0xff;
  byte3 = (sampleRate >> 16) & 0xff;
  byte4 = (sampleRate >> 24) & 0xff;  
  frec.write(byte1);  frec.write(byte2);  frec.write(byte3);  frec.write(byte4);
  byte1 = byteRate & 0xff;
  byte2 = (byteRate >> 8) & 0xff;
  byte3 = (byteRate >> 16) & 0xff;
  byte4 = (byteRate >> 24) & 0xff;  
  frec.write(byte1);  frec.write(byte2);  frec.write(byte3);  frec.write(byte4);
  byte1 = blockAlign & 0xff;
  byte2 = (blockAlign >> 8) & 0xff;
  frec.write(byte1);  frec.write(byte2); 
  byte1 = bitsPerSample & 0xff;
  byte2 = (bitsPerSample >> 8) & 0xff;
  frec.write(byte1);  frec.write(byte2); 
  frec.write("data");
  byte1 = Subchunk2Size & 0xff;
  byte2 = (Subchunk2Size >> 8) & 0xff;
  byte3 = (Subchunk2Size >> 16) & 0xff;
  byte4 = (Subchunk2Size >> 24) & 0xff;  
  frec.write(byte1);  frec.write(byte2);  frec.write(byte3);  frec.write(byte4);
  frec.close();
  Serial.println("header written"); 
  Serial.print("Subchunk2: "); 
  Serial.println(Subchunk2Size); 
}


void ButtonUpdate(){
    //update states of buttons (non working so far)
    button1.update();
    
    if (button1.pressed()){
      Serial.print("button pressed 1 \n");
      prev_mode= mode;
      mode++;
      if (mode >2){
      mode = 0;
      }        
    }
}