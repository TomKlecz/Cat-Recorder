/*!Teensy Audio Interface with channel and gain select through bluetooth*/
#include <SD.h>
#include <SerialFlash.h>
#include <math.h>
#include <Audio.h>
#include <Wire.h>

//the teensy Audio shield declarations
const int myInput = AUDIO_INPUT_LINEIN;
AudioInputI2S            i2s1;           
AudioOutputI2S           i2s2;           
AudioOutputUSB           usb1;           
AudioConnection          patchCord1(i2s1, 0, i2s2, 0);
AudioConnection          patchCord2(i2s1, 0, usb1, 0);
AudioConnection          patchCord3(i2s1, 1, i2s2, 1);
AudioConnection          patchCord4(i2s1, 1, usb1, 1);
AudioControlSGTL5000     audioShield;    


//Define Digital Out pins (alternative to SPI pins) 
#define GAIN_A_1 15
#define GAIN_A_2 14
#define GAIN_A_3 16
#define GAIN_A_4 17

#define GAIN_B_1 26
#define GAIN_B_2 24
#define GAIN_B_3 25
#define GAIN_B_4 27

#define SEN_CTRL_1 38
#define SEN_CTRL_2 40
#define SEN_CTRL_3 41
#define SEN_CTRL_5 36
#define SEN_CTRL_6 33
#define SEN_CTRL_7 30
#define SEN_CTRL_8 35

#define HWSERIAL Serial1

bool setupbool = false;

void setup() {
    setupbool=true;
    
    //DigitalOut GAIN & SEL pins are active low. Setup as HIGH
    //GAIN_A_1
    pinMode(GAIN_A_1, OUTPUT);
    digitalWrite(GAIN_A_1, 1);

    //GAIN_A_2
    pinMode(GAIN_A_2, OUTPUT);
    digitalWrite(GAIN_A_2, 1);

    //GAIN_A_3
    pinMode(GAIN_A_3, OUTPUT);
    digitalWrite(GAIN_A_3, 1);

    //GAIN_A_4
    pinMode(GAIN_A_4, OUTPUT);
    digitalWrite(GAIN_A_4, 1);

    //GAIN_B_1
    pinMode(GAIN_B_1, OUTPUT);
    digitalWrite(GAIN_B_1, 1);

    //GAIN_B_2
    pinMode(GAIN_B_2, OUTPUT);
    digitalWrite(GAIN_B_2, 1);

    //GAIN_B_3
    pinMode(GAIN_B_3, OUTPUT);
    digitalWrite(GAIN_B_3, 1);

    //GAIN_B_4
    pinMode(GAIN_B_4, OUTPUT);
    digitalWrite(GAIN_B_4, 1);

    //SEN_CTRL_1
    pinMode(SEN_CTRL_1, OUTPUT);
    digitalWrite(SEN_CTRL_1, 1);

    //SEN_CTRL_2
    pinMode(SEN_CTRL_2, OUTPUT);
    digitalWrite(SEN_CTRL_2, 1);

    //SEN_CTRL_3
    pinMode(SEN_CTRL_3, OUTPUT);
    digitalWrite(SEN_CTRL_3, 1);

    //SEN_CTRL_4
    pinMode(SEN_CTRL_4, OUTPUT);
    digitalWrite(SEN_CTRL_4, 1);

    //SEN_CTRL_5
    pinMode(SEN_CTRL_5, OUTPUT);
    digitalWrite(SEN_CTRL_5, 1);

    //SEN_CTRL_6
    pinMode(SEN_CTRL_6, OUTPUT);
    digitalWrite(SEN_CTRL_6, 1);

    //SEN_CTRL_7
    pinMode(SEN_CTRL_7, OUTPUT);
    digitalWrite(SEN_CTRL_7, 1);

    //SEN_CTRL_8
    pinMode(SEN_CTRL_8, OUTPUT);
    digitalWrite(SEN_CTRL_8, 1);
    
    Serial.begin(9600); //debug comms
    HWSERIAL.begin(9600); //bluetooth serial  
    //setup the audio shield for recording
    AudioMemory(240);
    audioShield.enable();
    audioShield.inputSelect(myInput);
    audioShield.volume(0.5); //set headphone amp volume for monitoring recording
    audioShield.lineInLevel(0); //set codec input dynamic range
    audioShield.lineOutLevel(13); //set codec output dynamic range

    setupbool=bluetoothsetup;

}

void loop() {

      if (HWSERIAL.available() > 0) { //check turned recording off
        byte incomingByte = HWSERIAL.read();
        Serial.print("UART received: ");
        Serial.println(incomingByte, DEC);
        HWSERIAL.print("UART received:");
        HWSERIAL.println(incomingByte, DEC);
        if (incomingByte == 0x00110001){
            setupbool = true;
            setupbool=bluetoothsetup();
        }
    }        
}

bool bluetoothsetup() {
    HWSERIAL.print("SETUP_MODE ");
    HWSERIAL.print("MIC_SELECT ");
    bool mic_selected = false;
    while (mic_selected == false) {
        if (HWSERIAL.available() > 0) {
            byte incomingByte = HWSERIAL.read();
            
            incomingByte = incomingByte & 15;
            //Bitwise AND with 1111 binary to keep only the 4 LSBs
            
            int Byte_error;
            if (incomingByte == 0 || incomingByte == 9) {
                Byte_error = false;
                HWSERIAL.print("ERROR. Value needs to be 1 to 8. Press again");
            }
            while (Byte_error == false) {
                if (HWSERIAL.available() > 0) {
                    incomingByte = HWSERIAL.read();
                    if (!(incomingByte == 0 || incomingByte == 9)) {
                        Byte_error == true;
                    }
                }
              
            }
            //Ensure the byte (number sent by bluetooth) is 
                //if the byte is not between 1 and 8 give error
                    //Ask for a new byte
                    //Repeat until byte is within range

            mic_select(incomingByte);
            //Call mic_select function with mic number as argument

            Serial.print("UART received: ");
            Serial.println(incomingByte, DEC);
            HWSERIAL.print("UART received:");
            HWSERIAL.println(incomingByte, DEC);

            //Break while loop
            mic_selected = true;
        }
    }
    bool mic_gain = false;

    while (mic_gain == false) {
        if (HWSERIAL.available() > 0) {
            byte incomingByte = HWSERIAL.read();

            incomingByte = incomingByte & 15;
            //Bitwise AND with 1111 binary to keep only the 4 LSBs
            
            int Byte_error;
            if ((incomingByte==0) || (incomingByte > 4)) {
                Byte_error = false;
                HWSERIAL.print("ERROR. Value needs to be 1 to 4. Press again");
            }
            while (Byte_error == false) {
                if (HWSERIAL.available() > 0) {
                    incomingByte = HWSERIAL.read();
                    if (!(incomingByte == 0 || incomingByte > 4)) {
                        Byte_error == true;
                    }
                }

            }
            //Ensure the byte (number sent by bluetooth) is 
                //if the byte is not between 1 and 4 give error
                    //Ask for a new byte
                    //Repeat until byte is within range

            Serial.print("UART received: ");
            Serial.println(incomingByte, DEC);
            HWSERIAL.print("UART received:");
            HWSERIAL.println(incomingByte, DEC);

            //Assign Byte to throat gain
            byte gain = incomingByte;

            //Break while loop
            mic_gain = true;
        }
    }

    //call gain_setting function with the throat_gain and variable_gain as arguments
    gain_select(gain);

    //put the 3 bools to false 
    mic_selected = false;
    mic_gain = false;
    return false;
}

void mic_select(byte mic_number){

     //set all SEN_CTRL pins HIGH. 
    //in case one was set low at some point
    digitalWrite(SEN_CTRL_1, 1);
    digitalWrite(SEN_CTRL_2, 1);
    digitalWrite(SEN_CTRL_3, 1);
    digitalWrite(SEN_CTRL_4, 1);
    digitalWrite(SEN_CTRL_5, 1);
    digitalWrite(SEN_CTRL_6, 1);
    digitalWrite(SEN_CTRL_7, 1);
    digitalWrite(SEN_CTRL_8, 1);

    //According to the 1-8 number received by bluetooth
    //Pull the corresponding pin LOW
    switch (mic_number) {
        case 1:
            digitalWrite(SEN_CTRL_1, 0);
        case 2: 
            digitalWrite(SEN_CTRL_2, 0);
        case 3:
            digitalWrite(SEN_CTRL_3, 0);
        case 4:
            digitalWrite(SEN_CTRL_4, 0);
        case 5:
            digitalWrite(SEN_CTRL_5, 0);
        case 6:
            digitalWrite(SEN_CTRL_6, 0);
        case 7:
            digitalWrite(SEN_CTRL_7, 0);
        case 8:
            digitalWrite(SEN_CTRL_8, 0);
    }
    
}

void gain_select(byte gain){
    
    //set all GAIN pins HIGH. 
    //in case one was set low at some point
    digitalWrite(GAIN_A_1, 1);
    digitalWrite(GAIN_A_2, 1);
    digitalWrite(GAIN_A_3, 1);
    digitalWrite(GAIN_A_4, 1);
    digitalWrite(GAIN_B_1, 1);
    digitalWrite(GAIN_B_2, 1);
    digitalWrite(GAIN_B_3, 1);
    digitalWrite(GAIN_B_4, 1);

    //For each of the two mic gains
    
    //throat_gain
    //According to the 1-4 number received by bluetooth
    //Pull the corresponding pin LOW
    switch (gain) {
    case 1:
        digitalWrite(GAIN_A_1, 0);
        digitalWrite(GAIN_B_1, 0);
    case 2:
        digitalWrite(GAIN_A_2, 0);
        digitalWrite(GAIN_B_2, 0);
    case 3:
        digitalWrite(GAIN_A_3, 0);
        digitalWrite(GAIN_B_3, 0);
    case 4:
        digitalWrite(GAIN_A_4, 0);
        digitalWrite(GAIN_B_4, 0);
    }
}
