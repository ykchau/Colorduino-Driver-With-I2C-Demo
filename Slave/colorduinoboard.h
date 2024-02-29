#ifndef COLORDUINOBOARD_H_
#define COLORDUINOBOARD_H_

#include <Arduino.h>

/*----- DM163 LED Driver Pins -----*/
#define DM163_SEL_BANK_8BIT PORTC   |= B00000001
#define DM163_SEL_BANK_6BIT PORTC   &= ~B00000001

#define DM163_LAT_SET PORTC         |= B00000010
#define DM163_RST_SET PORTC         |= B00000100
#define DM163_SCL_SET PORTD         |= B01000000
#define DM163_SDA_SET PORTD         |= B10000000

#define DM163_LAT_CLR PORTC         &= ~B00000010
#define DM163_RST_CLR PORTC         &= ~B00000100
#define DM163_SCL_CLR PORTD         &= ~B01000000
#define DM163_SDA_CLR PORTD         &= ~B10000000

/*----- DM163 Mode, 6-bit for correction data, 8-bit for image data -----*/
#define CALIBRATION_MODE        6
#define IMAGE_MODE              8

/*----- LED Matrix -----*/
#define LED_MATRIX_WIDTH        8
#define LED_MATRIX_HEIGHT       8

/*----- Redefine the name of exist function -----*/
#define DENY_INTERRUPTS         cli();
#define ALLOW_INTERRUPTS        sei();

/*----- Show Letter -----*/
#define DOUBLE_FIRST            0
#define DOUBLE_SECOND           1
#define SINGLE_LETTER           2
#define DOUBLE_MIDDLE          3



typedef struct RGB {
    byte r;
    byte g;
    byte b;
} ColorRGB;

class ColorduinoBoard {
   private:
    ColorRGB frameBuffer0[LED_MATRIX_WIDTH][LED_MATRIX_HEIGHT];
    ColorRGB frameBuffer1[LED_MATRIX_WIDTH][LED_MATRIX_HEIGHT];
    ColorRGB tempBuffer[LED_MATRIX_WIDTH][LED_MATRIX_HEIGHT];

    ColorRGB (*displayFramePointer)[LED_MATRIX_WIDTH];
    ColorRGB (*bufferFramePointer)[LED_MATRIX_WIDTH];
    byte updatingRow;

    // ATMega328P init functions
    void ATMega328P_IO_Init();
    void ATMega328P_DM163_LED_Init();
    void ATMega328P_Timer_Init_COMPA();
    void ATMega328P_Timer_Init_OVF();
    void init();

    // DM163 Low level commands
    void DM163_Mode(byte mode);
    void DM163_SendBit();
    void DM163_UpdateRegister();
    void DM163_reset();

    // other functions
    void DM163_Delay(byte i);

    // DM163 Data Loading command
    void DM163_Popbit(byte byteData, byte bitMode = IMAGE_MODE);
    void DM163_LoadCalibrationData(byte red_ch, byte green_ch, byte blue_ch);
    void DM163_UpdateRowData(byte curRow);

    // Frame buffer control functions
    /* Display Frame */
    void FB_SwapFrame();

    /* Buffer Frame */
    void FB_CopyDisplayFrame();
    void FB_ClearBufferFrame();
    void FB_ClearBuffer(ColorRGB *bf);
    void FB_FillData(byte red_ch, byte green_ch, byte blue_ch);
    void FB_Rotate(int deg);                                            // 90, 180, 270
    void FB_Flip(byte dir);                                             // up/down, left/right
    void FB_Move(char offsetCol, char offsetRow, bool offScreenShift);  // if off screen hidden false, then shift to ZERO pos

    ColorRGB ColorLetter = {255, 255, 255};

   public:
    ColorduinoBoard() {
        updatingRow = 0;
        displayFramePointer = frameBuffer0;
        bufferFramePointer = frameBuffer1;

        init();
    }

    // ISR update display function
    void updateDisplayRow();

    // M54564FP row select
    void ROW_Enable(byte rowNum);
    void ROW_DisableAll();

    // LED Display control functions
    void clearDisplay();
    void setBrightness(byte red_level, byte green_level, byte blue_level);
    void fillColor(byte red, byte green, byte blue);
    void setPixel(byte col, byte row, byte red, byte green, byte blue);
    void addPixel(byte col, byte row, byte red, byte green, byte blue);
    void showLetter(byte letter, char startCol, char startRow, ColorRGB *color, byte mode = SINGLE_LETTER);
    void showLogo(byte logo);

    void rotate(int deg);
    void flip(byte dir);
    void move(char offsetCol, char offsetRow, bool offScreenShift = false);

    // variable manipulate
    ColorRGB *getLetterColor() {
        return &ColorLetter;
    }
    void setLetterColor(byte color[3]) {
        ColorLetter.r = color[0];
        ColorLetter.g = color[1];
        ColorLetter.b = color[2];
    }

   protected:
};

extern ColorduinoBoard colorduino;
#endif  // COLORDUINOBOARD_H_
