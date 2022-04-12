/*!not so naive teensy recorder*/

#include <SPI.h>
#include <SD.h>
#include <SD_t3.h>
#include <SerialFlash.h>
#include <Bounce2.h>
#include <math.h>
#include <Audio.h>
#include <Wire.h>
#include <string>


//write wav variables
unsigned long ChunkSize = 0L;
unsigned long Subchunk1Size = 16;
unsigned int AudioFormat = 1;
unsigned int numChannels = 1;
unsigned long sampleRate = 44100;
unsigned int bitsPerSample = 16;
unsigned long byteRate = sampleRate*numChannels*(bitsPerSample/8);// samplerate x channels x (bitspersample / 8)
unsigned int blockAlign = numChannels*bitsPerSample/8;
unsigned long Subchunk2Size = 0L;
unsigned long NumSamples = 0L;
byte byte1, byte2, byte3, byte4;
unsigned long recByteSaved_throat = 0L;
unsigned long recByteSaved_variable = 0L;


//the teensy Audio shield declarations
const int myInput = AUDIO_INPUT_LINEIN;
AudioInputI2S            i2s1;           
AudioRecordQueue         queue1;         
AudioRecordQueue         queue2;         
AudioConnection          patchCord1(i2s1, 0, queue1, 0);
AudioConnection          patchCord2(i2s1, 1, queue2, 0);
AudioControlSGTL5000     audioShield;    
AudioControlSGTL5000     audioShield;

// Define SD card pins (using the SD card on teensy as it faster than the audio shield slot)
#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  11
#define SDCARD_SCK_PIN   13

//int to store which mode we're in
bool recording = false;  // false=stopped, true=recording

//the files we use to write to the sd card
File throat_channel;
File variable_channel;

// variables which increment at each new recording so that the file names are incremented;
int first_name_iterator = 0; 
int second_name_iterator = 0;

#define HWSERIAL Serial1
bool setupbool = false;

void setup() {
    setupbool=true;
  
    Serial.begin(9600); //debug comms
    HWSERIAL.begin(9600); //bluetooth serial  
    //setup the audio shield for recording
    AudioMemory(120);
    audioShield.enable();
    audioShield.inputSelect(myInput);
    audioShield.volume(0.5); //set headphone amp volume
    audioShield.lineInLevel(0);

    //check the SD card is here
    SD.begin(BUILTIN_SDCARD);
    if (!(SD.begin(BUILTIN_SDCARD))) {
    // stop here, but print a message repetitively, probably a better way to this with bluetooth
        while (1) {
            Serial.println("Unable to access the SD card");
            delay(500);
        }
    }

    // add set up code to receive from bluettoth here :: aka set the mic the record, the mic gain, the name of the files to record to on the SD card 
    while (setupbool==true){
        setupbool=bluetoothsetup;
    }
    //start recording at end of setup, we should probably put this in a while loop that waits to receive the start record trigger from bluetooth to start recording
    startRecording("first_testfile","second_testfile"); 
}

void loop() {
    //very crude loop for recording
    //in the end, the loop should just wait for bluetooth info the check when to end the recording (through the mode variable)
    //and call continueRecording while mode == 1 . we could also add something which goes re sets up the device if mode == 0
    //so that we don't have to power the device off to start a new recording 

    while(recording==true){
      if (HWSERIAL.available() > 0) { //check turned recording off
        incomingByte = HWSERIAL.read();
        Serial.print("UART received: ");
        Serial.println(incomingByte, DEC);
        HWSERIAL.print("UART received:");
        HWSERIAL.println(incomingByte, DEC);
        if (incomingByte == 0x00000000){
          recording=false;
        }
      }        
      //ButtonUpdate();
      continueRecording();
        //check if bluetooth turned recording to false
    } 

    if (recording == false){
        stopRecording();
        setupbool == true;
        while (setupbool == true){ //enter setup mode so that we can start a new recording, the last line in this if statement should be startRecording(); 
          bluetoothsetup(setupbool);
        }
        startRecording();  
    }
}

void startRecording(char myfirst_file_name, char mysecond_file_name) {

    while (SD.exists(myfirst_file_name)) { //if the file name already exist then increment the number at the end of the name
        unsigned int file_name_length = myfirst_file_name.length();
        while (myfirst_file_name[file_name_length-1]==static_cast<string>(first_name_iterator)) {
            first_name_iterator++;
        }
        myfirst_file_name[file_name_length-1]=static_cast<string>(first_name_iterator);   
        first_name_iterator++;    
    }
  
    throat_channel = SD.open(myfirst_file_name, FILE_WRITE); //open the file in write mode

    while (SD.exists(mysecond_file_name)) { //if the file name already exist then increment the number at the end of the name
        unsigned int file_name_length2 = mysecond_file_name.length();
        while (mysecond_file_name[file_name_length2-1]==static_cast<string>(second_name_iterator)) {
            second_name_iterator++;
        }
        mysecond_file_name[file_name_length2-1]=static_cast<string>(second_name_iterator);   
        second_name_iterator++;    
    }
  
    variable_channel = SD.open(mysecond_file_name, FILE_WRITE); //open the file in write mode

    if (throat_channel) {
        queue1.begin();
        recByteSaved_throat = 0L;
    }

    if (variable_channel) {
        queue2.begin();
        recByteSaved_variable = 0L;
    }
    recording=true;
    //Serial.println("startRecording"); this could be where we send a string back to the app through bluetooth to confirm we started recording 
}

void continueRecording() {
  
    if (queue1.available() >= 2) {
        byte buffer[512];
        memcpy(buffer, queue1.readBuffer(), 256);
        queue1.freeBuffer();
        memcpy(buffer + 256, queue1.readBuffer(), 256);
        queue1.freeBuffer();
        // write all 512 bytes to the SD card
        throat_channel.write(buffer, 512);
        recByteSaved_throat += 512;
    }

    if (queue2.available() >= 2) {
        byte buffer[512];
        memcpy(buffer, queue2.readBuffer(), 256);
        queue2.freeBuffer();
        memcpy(buffer + 256, queue2.readBuffer(), 256);
        queue2.freeBuffer();
        // write all 512 bytes to the SD card
        variable_channel.write(buffer, 512);
        recByteSaved_variable += 512;
    }
}

void stopRecording() {
    HWSERIAL.println("stopRecording");
  
    queue1.end();{ 
        while(queue1.available() > 0) {
            throat_channel.write((byte*)queue1.readBuffer(), 256);
            queue1.freeBuffer();
            recByteSaved_throat += 256;
        }
        writeOutHeader(throat_channel,recByteSaved_throat); //add throat arguments to write header
        throat_channel.close();
    }

    queue2.end();{ 
        while (queue2.available() > 0) {
        variable_channel.write((byte*)queue2.readBuffer(), 256);
        queue2.freeBuffer();
        recByteSaved_variable += 256;
        }
        writeOutHeader(variable_channel,recByteSaved_variable); //add variable channel arguments to write header
        variable_channel.close();
    }
}

void writeOutHeader(File myfile, int recBytes) { 
// writes header file to turn raw file to wav
// update WAV header with final filesize/datasize
//  NumSamples = (recByteSaved*8)/bitsPerSample/numChannels;
//  Subchunk2Size = NumSamples*numChannels*bitsPerSample/8; // number of samples x number of channels x number of bytes per sample
    Subchunk2Size = recBytes;
    ChunkSize = Subchunk2Size + 36;
    myfile.seek(0);
    myfile.write("RIFF");
    byte1 = ChunkSize & 0xff;
    byte2 = (ChunkSize >> 8) & 0xff;
    byte3 = (ChunkSize >> 16) & 0xff;
    byte4 = (ChunkSize >> 24) & 0xff;  
    myfile.write(byte1);  myfile.write(byte2);  myfile.write(byte3);  myfile.write(byte4);
    myfile.write("WAVE");
    myfile.write("fmt ");
    byte1 = Subchunk1Size & 0xff;
    byte2 = (Subchunk1Size >> 8) & 0xff;
    byte3 = (Subchunk1Size >> 16) & 0xff;
    byte4 = (Subchunk1Size >> 24) & 0xff;  
    myfile.write(byte1);  myfile.write(byte2);  myfile.write(byte3);  myfile.write(byte4);
    byte1 = AudioFormat & 0xff;
    byte2 = (AudioFormat >> 8) & 0xff;
    myfile.write(byte1);  myfile.write(byte2); 
    byte1 = numChannels & 0xff;
    byte2 = (numChannels >> 8) & 0xff;
    myfile.write(byte1); myfile.write(byte2); 
    byte1 = sampleRate & 0xff;
    byte2 = (sampleRate >> 8) & 0xff;
    byte3 = (sampleRate >> 16) & 0xff;
    byte4 = (sampleRate >> 24) & 0xff;  
    myfile.write(byte1);  myfile.write(byte2);  myfile.write(byte3);  myfile.write(byte4);
    byte1 = byteRate & 0xff;
    byte2 = (byteRate >> 8) & 0xff;
    byte3 = (byteRate >> 16) & 0xff;
    byte4 = (byteRate >> 24) & 0xff;  
    myfile.write(byte1);  myfile.write(byte2);  myfile.write(byte3);  myfile.write(byte4);
    byte1 = blockAlign & 0xff;
    byte2 = (blockAlign >> 8) & 0xff;
    myfile.write(byte1);  myfile.write(byte2); 
    byte1 = bitsPerSample & 0xff;
    byte2 = (bitsPerSample >> 8) & 0xff;
    myfile.write(byte1);  myfile.write(byte2); 
    myfile.write("data");
    byte1 = Subchunk2Size & 0xff;
    byte2 = (Subchunk2Size >> 8) & 0xff;
    byte3 = (Subchunk2Size >> 16) & 0xff;
    byte4 = (Subchunk2Size >> 24) & 0xff;  
    myfile.write(byte1);  myfile.write(byte2);  myfile.write(byte3);  myfile.write(byte4);
    myfile.close();
    //Serial.println("header written"); 
    //Serial.print("Subchunk2: "); 
    //Serial.println(Subchunk2Size); 
}

bool bluetoothsetup(){
    HWSERIAL.print("SETUP_MODE ");
    HWSERIAL.print("MIC_SELECT ");
    bool mic_selected=false;
    while (mic_selected==false){
       if (HWSERIAL.available() > 0) {
          incomingByte = HWSERIAL.read();
          Serial.print("UART received: ");
          Serial.println(incomingByte, DEC);
          HWSERIAL.print("UART received:");
          HWSERIAL.println(incomingByte, DEC);
          mic_selected=true;
        }
    }

      
      //if all the setup variables are received then turn setupbool to false
}