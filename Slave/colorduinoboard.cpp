/*
	Project  : LaLiMat project (https://www.youtube.com/playlist?list=PLJBKmE2nNweRXOebZjydkMEiq2pHtBMOS in Chinese)
 	file     : colorduinoboard.cpp
	Author   : ykchau
 	youtube  : youtube.com/ykchau888
  	Licenese : GPL-3.0
   	Please let me know if you use it commercial project.
*/

#include "colorduinoBoard.h"

#include "font.h"
#include "logo.h"

ColorduinoBoard colorduino;
/*******************************
	Reference
********************************/
/*
  Port summary of colorduino
  --- Port B ---
  PB0	C0	M54564FP
  PB1	C1	M54564FP
  PB2	C2	M54564FP
  PB3	C3	M54564FP
  PB4	C4	M54564FP
  PB5	C5	M54564FP
  PB6	XTAL1
  PB7	XTAL2

  --- Port C ---
  PC0	LED_SELBNK	DM163
  PC1	LED_LAT_BAR	DM163
  PC2	LED_RST_BAR	DM163
  PC3	N/A
  PC4	SDA	SDA_Pin
  PC5	SCL	SCL_Pin
  PC6	RESET_BAR	ResetButton
  PC7	N/A

  --- Port D ---
  PD0	RXD	RXD_Pin
  PD1	TXD	TXD_Pin
  PD2	N/A
  PD3	C6	M54564FP
  PD4	C7	M54564FP
  PD5	N/A
  PD6	LED_SDA	DM163
  PD7	LED_SCL	DM163

*/

/*******************************
	Private
********************************/
// ATMega328P init functions
void ColorduinoBoard::ATMega328P_IO_Init() {
    // https://www.arduino.cc/en/Reference/PortManipulation
    // High/1 as input; Low/0 as output

    // Set Data Direction Register of the corresponding Port to work as output
    DDRB = 0xff;
    DDRC = 0xff;
    DDRD = 0xff;

    // Set Port Register of the corresponding Port to work as input
    PORTB = 0x00;
    PORTC = 0x00;
    PORTD = 0x00;
}

void ColorduinoBoard::ATMega328P_DM163_LED_Init() {
    DM163_reset();
}

void ColorduinoBoard::ATMega328P_Timer_Init_COMPA() {
    DENY_INTERRUPTS;

    // |= setbit to 1
    // &= ~ setbit to 0

    // Init WGM to CTC mode
    // WGM22 = 0, WGM21 = 1, WGM20 = 0
    TCCR2B &= ~(1 << WGM22);  // Actually it's not required, because initial Value is 0, it's for easy reading
    TCCR2A |= (1 << WGM21);
    TCCR2A &= ~(1 << WGM20);

    // Init Prescaler as 64 (128 is ok) by set Clock Select bit
    // CS22 = 1, CS21 = 0, CS20 = 0
    TCCR2B |= (1 << CS22);
    TCCR2B |= (1 << CS21);
    TCCR2B |= (1 << CS20);

    // Init ASSR, set AS2 = 1, i.e. Timer/Counter2 is clocked from external clock
    ASSR |= 1 << AS2;

    // Init TIMSK2
    TIMSK2 |= 1 << OCIE2A;  // Enable Timer/Counter2 Compare match A interrupt

    // Init TimerCounter 2
    /*
    Tick time for 1024 Prescaler
       16Mhz / 1024 = 15625
       1/15625 = 64us

    Time of each DCK (Data Cycle) of DM163
       T_Rise(20ns) + T_Fall(20ns) + [(1/16Mhz * x) * 2] (where x is instructions time require for upload to DM163)

    Assume 10 cycle require for instruction for upload data to DM163
       20 + 20 + [(62.5 * 10) * 2] = 1290ns

    Data Cycle require for 1 row
       8 pixels * 3 colors * ( 6 + 8 ) bit = 336 DCK

    Speed of DM163 data upload to register is about 336 DCK * time of DCK
       336 * 1290ns = 433.44 us

    8 Rows
       8 * 433.44 = 3467.52 us

    Minimun ticks require per row
       433.44 us / 64 = 6.77 ticks

    2ms update rate = 2000/64 ~= 31.25
    1ms update rate = 1000/64 ~= 15.63
    0.5ms update rate = 500/64 ~= 7.81

    So, it requires minimum 7 ticks (6.77) for update a row, for easy calculation, we use 500us, that is ~8 ticks (7.81),
    that means we require 0.5ms * 8 rows = 4ms (250Hz) to update the whole LED matrix.

    Therefore, if we set 2.5ms update rate, 40 ticks, that is 2.5ms * 8 = 20ms (50Hz), we will notice glitch from the panel
    because of not enough refresh rate.

    So, 8 ~ 32 ticks is ok for use.

  */
    OCR2A = 16;  // 8 ~ 32 is ok, 8 may experience flicker, 16 recommended
    TCNT2 = 0;   // Start Counter at 0

    ALLOW_INTERRUPTS;
}

void ColorduinoBoard::ATMega328P_Timer_Init_OVF() {
    DENY_INTERRUPTS;

    // |= setbit to 1
    // &= ~ setbit to 0

    // Init WGM to Normal mode
    // WGM22 = 0, WGM21 = 0, WGM20 = 0
    TCCR2B &= ~(1 << WGM22);  // Actually it's not required, because initial Value is 0, it's for easy reading
    TCCR2A &= ~(1 << WGM21);
    TCCR2A &= ~(1 << WGM20);

    // Init Prescaler as 128 by set Clock Select bit
    // CS22 = 1, CS21 = 0, CS20 = 1
    TCCR2B |= (1 << CS22);
    TCCR2B &= ~(1 << CS21);
    TCCR2B |= (1 << CS20);

    // Init ASSR, set AS2 = 1, i.e. Timer/Counter2 is clocked from external clock
    ASSR |= 1 << AS2;

    // Init TIMSK2
    TIMSK2 |= 0 << OCIE2B;  // Disable Timer/Counter2 Compare match A interrupt
    TIMSK2 |= 1 << TOIE2;   // Enable Timer2 overflow interrupt

    // Init TimerCounter 2
    TCNT2 = 99;  // Start Counter at 0

    ALLOW_INTERRUPTS;
}

ISR(TIMER2_COMPA_vect) {
    DENY_INTERRUPTS;

    colorduino.updateDisplayRow();
    TCNT2 = 0;
    ALLOW_INTERRUPTS;
}

//ISR(TIMER2_OVF_vect) {
//  DENY_INTERRUPTS;
//
//  TCNT2 = 99;
//
//  colorduino.updateDisplayRow();
//
//  ALLOW_INTERRUPTS;
//}

void ColorduinoBoard::init() {
    ATMega328P_IO_Init();
    ATMega328P_DM163_LED_Init();
    ATMega328P_Timer_Init_COMPA();
    //ATMega328P_Timer_Init_OVF();
}

/***************************************************
  DM163 LED Driver control function (Low Level Command)

  Refer to DM163 manual page 12,13

  PINs connection of DM163 to RGB LED Matrix
  DM163 PINs            LED Matrix
  IOUT 3,6,9,14,17,20,25,28   RED 0..7
  IOUT 4,7,12,15,18,21,26,29    GREEN 0..7
  IOUT 5,8,13,16,19,24,27,30    BLUE 0..7

  Sequence:
  Ch23 to Ch0
  BGR of each channel
  MSB to LSB

  Example:
  Ch23_Blue_[MSB to LSB]
  Ch23_Green_[MSB to LSB]
  Ch23_Red_[MSB to LSB]

  Ch22_Blue_[MSB to LSB]
  Ch22_Green_[MSB to LSB]
  Ch22_Red_[MSB to LSB]
  ...and so on

***************************************************/
void ColorduinoBoard::DM163_Mode(byte mode) {
    DM163_LAT_CLR;
    if (mode == CALIBRATION_MODE) {
        DM163_SEL_BANK_6BIT;
    } else {  // IMAGE_MODE
        DM163_SEL_BANK_8BIT;
    }
}

void ColorduinoBoard::DM163_SendBit() {
    // Send Bit Data to DM163 shift Register by flipping SCL
    DM163_SCL_CLR;
    DM163_SCL_SET;
}

void ColorduinoBoard::DM163_UpdateRegister() {
    DM163_LAT_SET;
    DM163_LAT_CLR;
}

void ColorduinoBoard::DM163_reset() {
    /*
    The IC is initialize when RST = 0 (low). The IC must be reset before first use,
    if each channel is assigned to drive multiple LEDs, IC should be reset before
    each LED data latch to prevent from flashing.
  */
    DM163_RST_SET;
    DM163_Delay(1);
    DM163_RST_CLR;
    DM163_Delay(1);
    DM163_RST_SET;
    DM163_Delay(1);
}

// other functions
void ColorduinoBoard::DM163_Delay(byte i) {
    unsigned int y;
    y = i * 10;
    while (y--)
        ;
}

/***************************************************
  LED Functions

  Workflow of DM163:
  1. Select mode for assign diff bank (0, 6bit or 1 8bit) for use
  2. Prepare data and write it to LEDDataBuffer
  3. Load the prepared data to DM163
  4. Update data by switch the latch

***************************************************/

// DM163 Data Loading command
void ColorduinoBoard::DM163_Popbit(byte byteData, byte bitMode = IMAGE_MODE) {
    // Send Data bit by bit, MSB to LSB
    byte temp = byteData;

    if (bitMode == CALIBRATION_MODE) {
        temp = byteData << 2;
    }

    for (byte bitPos = 0; bitPos < bitMode; bitPos++) {
        // B10000000, Send in reverse order, so MSB first
        // check MSB of data byte, shift next to MSB and check until complete.

        temp &B10000000 ? DM163_SDA_SET : DM163_SDA_CLR;
        temp <<= 1;

        // Send Bit Data to DM163 buffer by flipping SCL
        DM163_SendBit();
    }
}

// update calibration data to DM163 6-bit shift register
void ColorduinoBoard::DM163_LoadCalibrationData(byte red_ch, byte green_ch, byte blue_ch) {
    DM163_Mode(CALIBRATION_MODE);

    for (byte row = 0; row < LED_MATRIX_HEIGHT; row++) {
        DM163_Popbit(blue_ch, CALIBRATION_MODE);
        DM163_Popbit(green_ch, CALIBRATION_MODE);
        DM163_Popbit(red_ch, CALIBRATION_MODE);
    }
    //DM163_SEL_BANK_8BIT;
    DM163_UpdateRegister();
}

// update row data to DM163 8-bit shift register
void ColorduinoBoard::DM163_UpdateRowData(byte curRow) {
    for (byte curPixel = 0; curPixel < 8; curPixel++) {
        DM163_Popbit(displayFramePointer[curPixel][curRow].b, IMAGE_MODE);
        DM163_Popbit(displayFramePointer[curPixel][curRow].g, IMAGE_MODE);
        DM163_Popbit(displayFramePointer[curPixel][curRow].r, IMAGE_MODE);
    }
    DM163_UpdateRegister();
}

// Frame buffer control functions
/* Display Frame */

// Swap buffer to display
void ColorduinoBoard::FB_SwapFrame() {
    DENY_INTERRUPTS;

    ColorRGB(*temp)[LED_MATRIX_WIDTH] = displayFramePointer;
    memcpy(displayFramePointer, bufferFramePointer, sizeof(frameBuffer0));
    memcpy(bufferFramePointer, temp, sizeof(frameBuffer0));

    ALLOW_INTERRUPTS;
}

/* Buffer Frame */

// Copy current display data to buffer frame
void ColorduinoBoard::FB_CopyDisplayFrame() {
    DENY_INTERRUPTS;
    memcpy(bufferFramePointer, displayFramePointer, sizeof(frameBuffer0));  // 8*8*3
    ALLOW_INTERRUPTS;
}

// fill buffer frame with ZERO
void ColorduinoBoard::FB_ClearBufferFrame() {
    DENY_INTERRUPTS;
    memset(bufferFramePointer, 0x00, sizeof(frameBuffer0));
    ALLOW_INTERRUPTS;
}

// fill buffer  with ZERO
void ColorduinoBoard::FB_ClearBuffer(ColorRGB *bf) {
    DENY_INTERRUPTS;
    memset(bf, 0x00, sizeof(frameBuffer0));
    ALLOW_INTERRUPTS;
}

// fill the buffer frame with same data
void ColorduinoBoard::FB_FillData(byte red_ch, byte green_ch, byte blue_ch) {
    ColorRGB *tempPtr = &bufferFramePointer[0][0];
    for (byte col = 0; col < LED_MATRIX_WIDTH; col++) {
        for (byte row = 0; row < LED_MATRIX_HEIGHT; row++) {
            //bufferFramePointer[col][row] = {red_ch, green_ch, blue_ch};
            tempPtr->r = red_ch;
            tempPtr->g = green_ch;
            tempPtr->b = blue_ch;
            tempPtr++;
        }
    }
}

// 90, 180, 270
void ColorduinoBoard::FB_Rotate(int deg) {
    //ColorRGB tempBuffer[LED_MATRIX_WIDTH][LED_MATRIX_HEIGHT];
    FB_ClearBuffer(*tempBuffer);
    memcpy(tempBuffer, displayFramePointer, sizeof(frameBuffer0));  // 8*8*3

    if (deg == 90) {
        for (byte row = 0; row < 8; row++) {
            for (byte col = 0; col < 8; col++) {
                bufferFramePointer[row][7 - col] = tempBuffer[col][row];
            }
        }
    } else if (deg == 180) {
        for (byte row = 0; row < 8; row++) {
            for (byte col = 0; col < 8; col++) {
                bufferFramePointer[7 - col][7 - row] = tempBuffer[col][row];
            }
        }
    } else if (deg == 270) {
        for (byte row = 0; row < 8; row++) {
            for (byte col = 0; col < 8; col++) {
                bufferFramePointer[7 - row][col] = tempBuffer[col][row];
            }
        }
    }
}

// 0 = up/down,  1 =left/right
void ColorduinoBoard::FB_Flip(byte dir) {
    //ColorRGB tempBuffer[LED_MATRIX_WIDTH][LED_MATRIX_HEIGHT];
    FB_ClearBuffer(*tempBuffer);
    memcpy(tempBuffer, displayFramePointer, sizeof(frameBuffer0));  // 8*8*3

    if (dir == 0) {
        for (byte row = 0; row < 8; row++) {
            for (byte col = 0; col < 8; col++) {
                bufferFramePointer[col][7 - row] = tempBuffer[col][row];
            }
        }
    } else if (dir == 1) {
        for (byte row = 0; row < 8; row++) {
            for (byte col = 0; col < 8; col++) {
                bufferFramePointer[7 - col][row] = tempBuffer[col][row];
            }
        }
    }
}

// Move the display frame with the origin bottom left
// if off screen shift TRUE, then shift to opposite pos
// range of col/row, -7 ~ +7
void ColorduinoBoard::FB_Move(char offsetCol, char offsetRow, bool offScreenShift) {
    //ColorRGB tempBuffer[LED_MATRIX_WIDTH][LED_MATRIX_HEIGHT];
    FB_ClearBuffer(*tempBuffer);
    memcpy(tempBuffer, displayFramePointer, sizeof(frameBuffer0));  // 8*8*3

    FB_ClearBufferFrame();
    if (!offScreenShift) {
        // ignore the pixel shift off screen
        for (byte row = 0; row < 8; row++) {
            for (byte col = 0; col < 8; col++) {
                byte tempCol = col + offsetCol;
                byte tempRow = row + offsetRow;
                if ((tempCol >= 0 && tempCol < 8) && (tempRow >= 0 && tempRow < 8)) {
                    bufferFramePointer[tempCol][tempRow] = tempBuffer[col][row];
                }
            }
        }
    } else {
        // move the pixel back to opposite position of the buffer
        for (byte row = 0; row < 8; row++) {
            for (byte col = 0; col < 8; col++) {
                char tempCol = col + offsetCol;
                char tempRow = row + offsetRow;

                if (tempCol >= 0 && tempCol < 8) {
                    if (tempRow >= 0 && tempRow < 8) {
                        // not off screen data
                        bufferFramePointer[tempCol][tempRow] = tempBuffer[col][row];
                    } else if (tempRow > 7) {
                        // Top to Bottom
                        bufferFramePointer[tempCol][tempRow - 8] = tempBuffer[col][row];
                    } else if (tempRow < 0) {
                        // Bottom to Top
                        bufferFramePointer[tempCol][tempRow + 8] = tempBuffer[col][row];
                    }
                } else if (tempCol > 7) {
                    if (tempRow >= 0 && tempRow < 8) {
                        // Right to Left
                        bufferFramePointer[tempCol - 8][tempRow] = tempBuffer[col][row];
                    } else if (tempRow > 7) {
                        // Top Right to Bottom Left
                        bufferFramePointer[tempCol - 8][tempRow - 8] = tempBuffer[col][row];
                    } else if (tempRow < 0) {
                        // Bottom Right to Top Left
                        bufferFramePointer[tempCol - 8][tempRow + 8] = tempBuffer[col][row];
                    }
                } else if (tempCol < 0) {
                    if (tempRow >= 0 && tempRow < 8) {
                        // Left to Right
                        bufferFramePointer[tempCol + 8][tempRow] = tempBuffer[col][row];
                    } else if (tempRow > 7) {
                        // Top Left to Bottom Right
                        bufferFramePointer[tempCol + 8][tempRow - 8] = tempBuffer[col][row];
                    } else if (tempRow < 0) {
                        // Bottom Left to Top Right
                        bufferFramePointer[tempCol + 8][tempRow + 8] = tempBuffer[col][row];
                    }
                }
            }
        }
    }
}

void ColorduinoBoard::move(char offsetCol, char offsetRow, bool offScreenShift = false) {
    FB_Move(offsetCol, offsetRow, offScreenShift);
    FB_SwapFrame();
}

/*******************************
	Public
********************************/

/*
  Due to the difference of each LED matrix, you may want to calibrate
  each matrix to make all look same, to use this function, you should
  use fillColor() function to fill color, and then adjust the matrix,
  the result should use for calibration.

  Since the data bank for calibration is 6-bit, therefore the data
  range = 0..63

  Column from 0..7, bottom to top
  Row from 0..7, left to right

  Board orientation:
  x = column/horizontal
  y = row/vertical
  board PINs on the bottom right when facing the LED Matrix

  x\y
      0,7 1,7 2,7 3,7 4,7 5,7 6,7 7,7
      0,6 1,6 2,6 3,6 4,6 5,6 6,6 7,6
      0,5 1,5 2,5 3,5 4,5 5,5 6,5 7,5
      0,4 1,4 2,4 3,4 4,4 5,4 6,4 7,4
      0,3 1,3 2,3 3,3 4,3 5,3 6,3 7,3 --PINs
      0,2 1,2 2,2 3,2 4,2 5,2 6,2 7,2 --PINs
      0,1 1,1 2,1 3,1 4,1 5,1 6,1 7,1 --PINs
      0,0 1,0 2,0 3,0 4,0 5,0 6,0 7,0

*/
// M54564FP row select
void ColorduinoBoard::ROW_Enable(byte rowNum) {
    switch (rowNum) {
        // Open the Pin of Specific port pin for send ready
        // PIN & PORT refer to CX at the above summary
        case 0:
            PORTB = B00000001;
            break;
        case 1:
            PORTB = B00000010;
            break;
        case 2:
            PORTB = B00000100;
            break;
        case 3:
            PORTB = B00001000;
            break;
        case 4:
            PORTB = B00010000;
            break;
        case 5:
            PORTB = B00100000;
            break;
        case 6:
            PORTD = B00001000;
            break;
        case 7:
            PORTD = B00010000;
            break;
        default:
            PORTB = 0x00;
            PORTD = 0x00;
    }
}

// Turn of all power for all rows
void ColorduinoBoard::ROW_DisableAll() {
    PORTB = 0x00;
    PORTD = 0x00;
}

// ISR update display function
void ColorduinoBoard::updateDisplayRow() {
    if (updatingRow > 7) {
        updatingRow = 0;
    }

    // Because of the circuit complexity, the RGB Matrix share same anode
    // so we enable a row for each interrupts, to avoid all column LED
    // active at same time, the fast refresh rate can trick human eyes
    // https://www.sparkfun.com/news/2650

    ROW_DisableAll();

    DM163_Mode(IMAGE_MODE);
    DM163_UpdateRowData(updatingRow);

    ROW_Enable(updatingRow);
    updatingRow++;
}

// LED Display control functions
void ColorduinoBoard::clearDisplay() {
    FB_ClearBufferFrame();
    FB_SwapFrame();
}

// set the brightness for correction of the color
void ColorduinoBoard::setBrightness(byte red_level, byte green_level, byte blue_level) {
    DENY_INTERRUPTS;
    // this function will fill data to 6-bit register as correction data
    DM163_LoadCalibrationData(red_level, green_level, blue_level);

    ALLOW_INTERRUPTS;
}

// fill display with single color
void ColorduinoBoard::fillColor(byte red, byte green, byte blue) {
    FB_FillData(red, green, blue);
    FB_SwapFrame();
}

// set pixel with specific data on the display
void ColorduinoBoard::setPixel(byte col, byte row, byte red, byte green, byte blue) {
    FB_ClearBufferFrame();
    bufferFramePointer[col][row] = {red, green, blue};
    FB_SwapFrame();
}

// add pixel based on the existing display
void ColorduinoBoard::addPixel(byte col, byte row, byte red, byte green, byte blue) {
    FB_CopyDisplayFrame();
    bufferFramePointer[col][row] = {red, green, blue};
    FB_SwapFrame();
}

void ColorduinoBoard::showLetter(byte letter, char startCol, char startRow, ColorRGB *color, byte mode = SINGLE_LETTER) {
    byte letterAddress = getLetter(letter);

    if (mode == DOUBLE_FIRST || mode == SINGLE_LETTER) {
        // clear frame buffer
        FB_ClearBufferFrame();
    }

    // fill data to buffer
    for (byte row = 0; row < 8; row++) {
        for (byte letterbit = 0; letterbit < 8; letterbit++) {
            char tempCol = row + startCol;
            char tempRow = 7 - letterbit + startRow;

            if ((tempCol >= 0 && tempCol < 8) && (tempRow >= 0 && tempRow < 8)) {  // map to the position if within display
                byte c = pgm_read_byte(&letters[letterAddress][row]);
                if (c & (1 << letterbit)) {
                    bufferFramePointer[tempCol][tempRow] = *color;
                }
            }
        }
    }

    if (mode == DOUBLE_SECOND || mode == SINGLE_LETTER) {  // DISPLAY_OVERWRITE
        // display it
        FB_SwapFrame();
    }
}

void ColorduinoBoard::showLogo(byte logo) {
    FB_ClearBufferFrame();

    byte(*logoPtr)[8] = getLogo(logo);
    byte(*logocolorPtr)[3] = getLogoColor(logo);

    for (byte row = 0; row < 8; row++) {
        for (byte col = 0; col < 8; col++) {
            byte c = pgm_read_byte(&logoPtr[7 - row][col]);
            if (c != 0) {
                bufferFramePointer[col][row] = {
                    pgm_read_byte(&logocolorPtr[c - 1][0]),
                    pgm_read_byte(&logocolorPtr[c - 1][1]),
                    pgm_read_byte(&logocolorPtr[c - 1][2])};
            } else {
                bufferFramePointer[col][row] = {0, 0, 0};
            }
        }
    }

    FB_SwapFrame();
}

void ColorduinoBoard::rotate(int deg) {
    FB_Rotate(deg);
    FB_SwapFrame();
}

void ColorduinoBoard::flip(byte dir) {
    FB_Flip(dir);
    FB_SwapFrame();
}

/*******************************
  Protected
********************************/
