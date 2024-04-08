/*
	Project  : LaLiMat project (https://www.youtube.com/playlist?list=PLJBKmE2nNweRXOebZjydkMEiq2pHtBMOS in Chinese)
 	file     : Slave.ino
	Author   : ykchau
 	youtube  : youtube.com/ykchau888
  	Licenese : GPL-3.0
   	Please let me know if you use it commercial project.
*/

// Board initialize
#include "colorduinoboard.h"
#include <EEPROM.h>
#include <Wire.h>

/*----- I2C Protocol -----*/
#define START_BYTE      0x00
String payload;

/*----- Colorduino Commands -----*/
#define CMD_SHOWLETTER  0x10
#define CMD_SHOWLOGO    0x11
#define CMD_SHOWDOUBLE  0x12
#define CMD_ANI_MSG     0x13


byte I2C_Incoming = 0;
byte doingCommand = 0;

ColorRGB *cc;



/*
  Serial functions for brightness setting, it will be replaced by button control later
*/
void serialFlush() {
    while (Serial.available() > 0) {
        char t = Serial.read();
    }
}

void readSerial(int *data) {
    do {
        if (Serial.available() > 0) {
            *data = Serial.parseInt();
            Serial.print("Number read = ");
            Serial.println(*data);
            serialFlush();
        }
        //Serial.println(*data);
    } while (*data == 0);
}

/*
  I2C Reading Function
*/
void I2CreceiveEvent(int action) {
    byte t0 = millis();
    
    // Clear payload before read
    payload = "";
    byte cur = 0;
    I2C_Incoming = 1;   // got incoming I2C

    while (1 < Wire.available()) {
        while ( doingCommand ) {
            // wait
        }
        char c = Wire.read();
        if (c == START_BYTE) {
            // read the payload
            byte byteToRead = Wire.read();

            for (byte i = 0; i < byteToRead; i++) {
                c = Wire.read();
                payload += c;
            }
            
            // check checksum
            char checksumInPayload = payload.charAt(payload.length() - 1);
            payload.remove(payload.length() - 1);  // drop the checksum

            char checksumValidate = Checksum(&payload);

            if (checksumInPayload == checksumValidate) {
                // if size correct, do command
                doingCommand = 1;
                doCommand(&payload);
                doingCommand = 0;
            }
        }
    }
    I2C_Incoming = 0;   // incoming I2C extracted
    Serial.println(String("Time Used :") + (millis() - t0));
}

/*
  Do Command rec'd from I2C
*/
void doCommand(String *payload) {
    // extract command and data packet
    byte command = payload->charAt(0);
	byte logo = 0;
    ColorRGB *cc = colorduino.getLetterColor();

    switch (command) {
        case CMD_SHOWLETTER:
            colorduino.showLetter(payload->charAt(1), 0, 2, cc, SINGLE_LETTER);
            break;
        case CMD_SHOWDOUBLE:
            colorduino.showLetter(payload->charAt(1), 0, 2, cc, DOUBLE_FIRST);
            colorduino.showLetter(payload->charAt(2), 4, 2, cc, DOUBLE_SECOND);
            break;
        case CMD_SHOWLOGO:
            logo = payload->charAt(1);
            colorduino.showLogo(payload->charAt(1));
            break;
        case CMD_ANI_MSG:
            char col = payload->charAt(1);
            char row = payload->charAt(2);
            char letter0 = payload->charAt(3);
            char letter1 = payload->charAt(4);
            char letter2 = payload->charAt(5);
            char direction = -1; // move from right to left

            if ( col % 4 == 0 ) {  // 2 letter on screen
                colorduino.showLetter( ( col < 4 ) ? letter0 : letter1, (direction * col), row, cc, DOUBLE_FIRST);
                colorduino.showLetter( ( col < 4 ) ? letter1 : letter2, (direction * col) + 4, row, cc, DOUBLE_SECOND);
            } else {  // 3 letter on screen
                colorduino.showLetter(letter0, (direction * col) + 0, row, cc, DOUBLE_FIRST);
                colorduino.showLetter(letter1, (direction * col) + 4, row, cc, DOUBLE_MIDDLE);
                colorduino.showLetter(letter2, (direction * col) + 8, row, cc, DOUBLE_SECOND);
            }
            break;
        default:
            break;
    }
}

/*
  Calculate the checksum of I2C packet
  checksum = XOR all command, payload
*/
byte Checksum(String *ptr) {
    byte chk = 0;
    for (int i = 0; i < ptr->length(); i++) {
        chk ^= ptr->charAt(i);
    }
    return chk;
}




void setup() {
    Serial.begin(115200);

    /*
   * This is use for hard code the I2C address to the EEPROM So we don't have to change address at the code everytime 
   * when upload the program.
   * 
   * After finish hard coding, you should comment out the EEPROM write statement.
   */
    //EEPROM.write(0, 0x70); // This command need to do ONE time only. Unless you need to change the address.
    byte deviceAddress = EEPROM.read(0);
    byte deviceNum = deviceAddress - 0x70;

    Wire.begin(deviceAddress);
    Wire.onReceive(I2CreceiveEvent);
    Serial.println(String("") + "Colorduino No." + deviceNum + ", address : 0x" + deviceAddress + " online.");

    // Tested Setting
    //  Square RGB
    //  10, 6,  6
    //  20, 15, 14
    //  30, 16, 18
    //  40, 20, 25
    //  50, 28, 40
    //  60, 35, 55

    //  Circle RGB
    //  6,  10, 8
    //  9,  20, 15
    //  14, 30, 24
    //  19, 40, 31
    //  26, 50, 37
    //  34, 60, 50
    //colorduino.setBrightness(60, 35, 55);
    colorduino.setBrightness(6, 10, 8);  // it will become flickering when low value set

    // Show device number on screen
    cc = colorduino.getLetterColor();
    colorduino.showLetter(char(deviceNum + 0x30), 0, 0, cc, SINGLE_LETTER);
}

void loop() {
    delay(100);
}
