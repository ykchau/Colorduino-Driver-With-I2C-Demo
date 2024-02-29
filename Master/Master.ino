#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <Wire.h>
#include <time.h>

/*----- WiFi -----*/
const char *wifi_ssid = "YOURWIFI";
const char *wifi_password = "PASSWORD";
int wifiStatus = WL_IDLE_STATUS;
byte wifiGiveupCount = 0;

/*----- NTP -----*/
#define TIMEZONE        28800  // GMT8, 8 * 60 * 60
#define NTP_SERVER_1    "pool.ntp.org"
#define NTP_SERVER_2    "time.nist.gov"
time_t rawtime;

/*----- I2C Protocol -----*/
#define START_BYTE      0x00

/*----- Colorduino Panels -----*/
#define PANEL_0         0x70
#define PANEL_1         0x71
#define PANEL_2         0x72
#define PANEL_3         0x73
#define PANEL_4         0x74

/*----- Colorduino Commands -----*/
#define CMD_SHOWLETTER  0x10
#define CMD_SHOWLOGO    0x11
#define CMD_SHOWDOUBLE  0x12
#define CMD_ANI_MSG     0x13


/*----- Modes -----*/
#define MODE_WIFI_CONNECTION    0
#define MODE_CLOCK_12           1
#define MODE_CLOCK_24           2
#define MODE_DATE               3
#define MODE_ANI_MSG            4

/*----- Clock Mode -----*/
#define CLOCK_12                0
#define CLOCK_24                1


/*----- I2C Packet Command & Payload buffer-----*/
byte command_p0;
byte command_p1;
byte command_p2;
byte command_p3;
byte command_p4;
byte payload_p0[2];
byte payload_p1[2];
byte payload_p2[2];
byte payload_p3[2];
byte payload_p4[2];
bool commandsReady;
Ticker ticker_Command;
Ticker ticker_mode;

/* for message animation */
String AniMsg_payload;
bool AniMsg = false;
bool AniMsgReady;
int AniMsgSpeed_MS = 200; // Recommend speed is > 10ms, because you can't read the word
byte AniMsgCurLetterPos;
byte AniMsg_Col;        // pixel col
byte AniMsg_Row = 2;    // pixel row


void setCommands(byte p0 = CMD_SHOWDOUBLE, byte p1 = CMD_SHOWDOUBLE, byte p2 = CMD_SHOWDOUBLE, byte p3 = CMD_SHOWDOUBLE, byte p4 = CMD_SHOWDOUBLE) {
    command_p0 = p0;
    command_p1 = p1;
    command_p2 = p2;
    command_p3 = p3;
    command_p4 = p4;
}

/****************************
 * 
 * Functions
 * 
 ***************************/
void display(byte mode) {
    ticker_mode.detach();  // detach previous mode before any other action.

    switch (mode) {
        case MODE_WIFI_CONNECTION:
            update_wifi_connection();
            break;
        case MODE_CLOCK_12:
            ticker_mode.attach(1, update_clock, CLOCK_12);  // Update clock every sec, mainly for colon blinking
            break;
        case MODE_CLOCK_24:
            ticker_mode.attach(1, update_clock, CLOCK_24);  // Update clock every sec, and colon blinking
            break;
        case MODE_DATE:
            ticker_mode.attach(1, update_date);  // Update date every sec, for date change
            break;
        case MODE_ANI_MSG:
            update_AniMsg(); // Initialize Animate Message
            break;
        default:
            break;
    }
}



void update_wifi_connection() {
    commandsReady = false;
    emptyPayloads();

    //Serial.print("Attempting to connect to ...");
    //Serial.println(wifi_ssid);

    WiFi.mode(WIFI_STA);

    configTime(TIMEZONE, 0, NTP_SERVER_1, NTP_SERVER_2);

    WiFi.begin(wifi_ssid, wifi_password);

    while (WiFi.status() != WL_CONNECTED && wifiGiveupCount != 10) {
        commandsReady = false;
        Serial.println("Couldn't get a wifi connection");
        Serial.print(".");

        setCommands(CMD_SHOWDOUBLE, CMD_SHOWDOUBLE, CMD_SHOWDOUBLE, CMD_SHOWDOUBLE, CMD_SHOWDOUBLE);

        setStringPayload(String("Connecting"));

        // give up after
        wifiGiveupCount++;
        delay(1000);
    }

    if (WiFi.status() == WL_CONNECTED) {
        setStringPayload(String("Connect OK"));

        //Serial.print("Connected to wifi, IP");
        //Serial.println(WiFi.localIP());
        wifiGiveupCount = 0;
    } else {
        //No IP!
        setStringPayload(String("no network"));
        //Serial.println("Failed connect to Wifi!");
    }

    delay(1000);
}

void update_clock(int clockMode) {
    commandsReady = false;
    emptyPayloads();

    // get current time
    time(&rawtime);
    struct tm *info = localtime(&rawtime);
    /*
    // struct tm *info = gmtime(&rawtime);

    // Serial.println("Current clock:");
    // Serial.println(String("GMTIME :") + info->tm_yday + " " + info->tm_mday + " " + (info->tm_year + 1900) + " " + info->tm_mon + " " + info->tm_mday + " " + info->tm_hour + ":" + info->tm_min + ":" + info->tm_sec);

    // info = localtime(&rawtime);

    // Serial.println("Current clock:");
    // Serial.println(String("LOCALTIME:") + info->tm_yday + " " + info->tm_mday + " " + (info->tm_year + 1900) + " " + info->tm_mon + " " + info->tm_mday + " " + info->tm_hour + ":" + info->tm_min + ":" + info->tm_sec);
    */

    byte hour = info->tm_hour;
    byte min = info->tm_min;

    byte hour_H = info->tm_hour / 10 + 48;  // add 48 convert to ASCII
    byte hour_L = info->tm_hour % 10 + 48;
    byte minute_H = info->tm_min / 10 + 48;
    byte minute_L = info->tm_min % 10 + 48;
    byte colon = info->tm_sec % 2 ? ':' : ' ';

    /* 
        Parameters for clock display
        default parameters is for CLOCK 12
        Panel 0 is for weather logo in Clock 12 mode
        no logo will be shown on Clock 24 mode
    */

    if (clockMode == CLOCK_24) {
        setCommands();

        payload_p0[0] = ' ';
        payload_p0[1] = hour_H;

        payload_p1[0] = hour_L;
        payload_p1[1] = colon;

        payload_p2[0] = minute_H;
        payload_p2[1] = minute_L;

        payload_p3[0] = colon;
        payload_p3[1] = (info->tm_sec / 10 + 48);

        payload_p4[0] = (info->tm_sec % 10 + 48);
        payload_p4[1] = ' ';
    } else {  // CLOCK_12
        setCommands(CMD_SHOWLOGO);

        payload_p0[0] = (info->tm_sec % 8);
        payload_p0[1] = ' ';

        payload_p1[0] = ' ';
        payload_p1[1] = hour_H;

        payload_p2[0] = hour_L;
        payload_p2[1] = colon;

        payload_p3[0] = minute_H;
        payload_p3[1] = minute_L;

        payload_p4[0] = (hour > 11) ? 'P' : 'A';
        payload_p4[1] = 'M';
    }

    commandsReady = true;
}

void update_date() {
    commandsReady = false;
    emptyPayloads();

    // get current time
    time(&rawtime);
    struct tm *info = localtime(&rawtime);

    byte day = info->tm_mday;
    byte month = info->tm_mon;
    byte year = info->tm_year;

    byte day_H = day / 10 + 48;
    byte day_L = day % 10 + 48;

    String monthName[12] = {"JAN", "FEB", "MAR", "ARP", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};

    setCommands(CMD_SHOWLOGO);

    // Assign payload
    payload_p0[0] = (info->tm_wday);  // show week logo at panel 0
    payload_p0[1] = 0x00;

    payload_p1[0] = ' ';
    payload_p1[1] = day_H;

    payload_p2[0] = day_L;
    payload_p2[1] = ' ';

    payload_p3[0] = monthName[month].charAt(0);
    payload_p3[1] = monthName[month].charAt(1);

    payload_p4[0] = monthName[month].charAt(2);
    payload_p4[1] = ' ';

    commandsReady = true;
}

void update_AniMsg() {
    /*
        This function will keep pass the message to colorduino,
        until interrupt or message all sent

        There have 2 reasons that we send message separately, 
        1. Each board may have cycle delay, for short message it should looks nice, however,
        if the message is longer, some non-syncing glitch may occur
        2. Because the limit sending buffer, and we don't want to modified the TWI.h or other
        system document.

        The buffer size of payload requires at least 3 byte, because we need 1 more screen 
        for waiting to shift in to fit the current screen, we also need col and row variable
        for where the letter place.

        We are preset 2 sec for screen shifting per panel, that means each dot moving require
        2 sec / 8 = 0.25, so, a variable for calculation using 1 sec based, which is

            default speed = 4 means 1 sec / Scaler = 0.25s per moving 1 dot.
    */
    commandsReady = false;

    if (!AniMsgReady) {
        AniMsg_payload = "Please Subscribe My Channel : YK Chau";
        setCommands(CMD_ANI_MSG, CMD_ANI_MSG, CMD_ANI_MSG, CMD_ANI_MSG, CMD_ANI_MSG);
        //setCommands(CMD_SHOWLOGO, CMD_ANI_MSG, CMD_ANI_MSG, CMD_ANI_MSG, CMD_ANI_MSG);
        AniMsgReady = true;
        AniMsg = true;
    }

    commandsReady = true;
}

void setAniMsgCommand() {
    /*
        payload buffer with 5 byte size,
        Data sequence = [0] Col, [1] Row, [2] Letter0, [3] Letter1, [4] Letter2
        we current move from to left only, some of the col value should in negative
        form while it moved out of screen, the value of this col is how many pixel
        to be moved (or deducted).

        Because sentence is from left to right, so only direction right to left will be implemented.
    */
    byte AniMsgBufferSize = 5;
    byte p0[5];
    byte p1[5];
    byte p2[5];
    byte p3[5];
    byte p4[5];

    if ( command_p0 == CMD_SHOWLOGO ) {
        p0[1] = AniMsgCurLetterPos % 2;
        setAniMsgPayload(p1, AniMsgCurLetterPos + 0, AniMsg_Col, AniMsg_Row);
        setAniMsgPayload(p2, AniMsgCurLetterPos + 2, AniMsg_Col, AniMsg_Row);
        setAniMsgPayload(p3, AniMsgCurLetterPos + 4, AniMsg_Col, AniMsg_Row);
        setAniMsgPayload(p4, AniMsgCurLetterPos + 6, AniMsg_Col, AniMsg_Row);
    } else {
        setAniMsgPayload(p0, AniMsgCurLetterPos + 0, AniMsg_Col, AniMsg_Row);
        setAniMsgPayload(p1, AniMsgCurLetterPos + 2, AniMsg_Col, AniMsg_Row);
        setAniMsgPayload(p2, AniMsgCurLetterPos + 4, AniMsg_Col, AniMsg_Row);
        setAniMsgPayload(p3, AniMsgCurLetterPos + 6, AniMsg_Col, AniMsg_Row);
        setAniMsgPayload(p4, AniMsgCurLetterPos + 8, AniMsg_Col, AniMsg_Row);
    }
    

    // update next position
    if (AniMsg_Col < 8) {
        AniMsg_Col++;
    }

    // if a letter moved 4 pixel, than tell the program shift 1 letter next time
    if (AniMsg_Col % 4 == 0) {
        AniMsgCurLetterPos++;
        AniMsg_Col = 0;
    }

    if ((AniMsg_payload.length() - AniMsgCurLetterPos) > 0) {
        sendI2CPacket(PANEL_0, command_p0, p0, AniMsgBufferSize);
        sendI2CPacket(PANEL_1, command_p1, p1, AniMsgBufferSize);
        sendI2CPacket(PANEL_2, command_p2, p2, AniMsgBufferSize);
        sendI2CPacket(PANEL_3, command_p3, p3, AniMsgBufferSize);
        sendI2CPacket(PANEL_4, command_p4, p4, AniMsgBufferSize);
    }
}

/*
    Payloads and Commands
*/
void emptyPayloads() {
    memset(payload_p0, 0, sizeof(payload_p0));
    memset(payload_p1, 0, sizeof(payload_p1));
    memset(payload_p2, 0, sizeof(payload_p2));
    memset(payload_p3, 0, sizeof(payload_p3));
    memset(payload_p4, 0, sizeof(payload_p4));
}

void setStringPayload(String payloads) {
    commandsReady = false;
    emptyPayloads();

    payload_p0[0] = payloads.charAt(0);
    payload_p0[1] = payloads.charAt(1);

    payload_p1[0] = payloads.charAt(2);
    payload_p1[1] = payloads.charAt(3);

    payload_p2[0] = payloads.charAt(4);
    payload_p2[1] = payloads.charAt(5);

    payload_p3[0] = payloads.charAt(6);
    payload_p3[1] = payloads.charAt(7);

    payload_p4[0] = payloads.charAt(8);
    payload_p4[1] = payloads.charAt(9);

    commandsReady = true;
}

void setAniMsgPayload(byte *panel, byte msgStartLetter, byte col, byte row) {
    panel[0] = col;
    panel[1] = row;
    /* Data sequence = [0] Col, [1] Row, [2] Letter0, [3] Letter1, [4] Letter2 */
    for (int i = 0; i < 3; i++) {
        if ((msgStartLetter + i) < AniMsg_payload.length()) {
            panel[i + 2] = AniMsg_payload.charAt(msgStartLetter + i);
        } else {
            panel[i + 2] = ' ';  // if out of array, send blank.
        }
    }
}

/*
    I2C Functions
*/
void sendCommands() {
    if (commandsReady) {
        // Do Normal display
            if (AniMsg) {
                /*
                Because the update speed and payload size diff.
                we will temporarily speed up the ticker_command,
                and reset to default after message finish. 
            */
                setAniMsgCommand();

                if (AniMsgCurLetterPos < AniMsg_payload.length()) {
                    ticker_Command.detach();
                    ticker_Command.attach_ms(AniMsgSpeed_MS, sendCommands);
                } else {
                    ticker_Command.detach();
                    ticker_Command.attach_ms(500, sendCommands);
                    AniMsgCurLetterPos = 0;
                    AniMsgReady = false;
                    AniMsg = false;

                    // If the delay is too long in the loop, it will go next mode while the delay complete
                }
            } else {
                sendI2CPacket(PANEL_0, command_p0, payload_p0, 2);
                sendI2CPacket(PANEL_1, command_p1, payload_p1, 2);
                sendI2CPacket(PANEL_2, command_p2, payload_p2, 2);
                sendI2CPacket(PANEL_3, command_p3, payload_p3, 2);
                sendI2CPacket(PANEL_4, command_p4, payload_p4, 2);
            }
        
    }
}

/*
    The function send message to other device by I2C
    It contains 4 parts :
    START_BYTE = 0x00
    byteToSend = 1 command byte + # of payload byte + 1 checksum byte
    payload = the content for use of command, size depends on mode
    checksum = XOR command and payload
*/
void sendI2CPacket(byte targetDevice, byte command, byte *payload, byte payloadSize) {
    byte byteToSend = payloadSize + 1 + 1;  // plus command & checksum

    Wire.beginTransmission(targetDevice);
    Wire.write(START_BYTE);
    Wire.write(byteToSend);
    Wire.write(command);
    for (int i = 0; i < payloadSize; i++) {
        Wire.write(payload[i]);
    }
    Wire.write(checksum(command, payload, payloadSize));
    Wire.endTransmission();
}

byte checksum(byte command, byte *ptr, byte ptrSize) {
    byte chk = 0 ^ command;

    for (int i = 0; i < ptrSize; i++) {
        chk ^= *ptr++;
    }

    return chk;
}

void findI2CDevice(byte address) {
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();

    if (error == 0) {
        if (address > 0x70 && address < 0x75)
            Serial.print("Colorduino found at address : 0x");
        Serial.println(address, HEX);
    } else if (error == 4) {
        Serial.print("Unknown error at address 0x");
        Serial.println(address, HEX);
    }
}

/****************************
 * 
 * Main
 * 
 ***************************/

void setup() {
    /* Serial */
    Serial.begin(115200);

    /* I2C */
    Wire.begin();
    // Scan the known address I2C device
    // Colorduino 0x70 ~ 0x74
    findI2CDevice(PANEL_0);
    findI2CDevice(PANEL_1);
    findI2CDevice(PANEL_2);
    findI2CDevice(PANEL_3);
    findI2CDevice(PANEL_4);
    delay(2000);  // let user see the panel is online

    // enable colorduino message update
    ticker_Command.attach(0.5, sendCommands);

    /* WiFi */
    display(MODE_WIFI_CONNECTION);
}

void loop() {
    /* I2C Command/Mode */
    //display(MODE_CLOCK_12);
    //delay(10000);
    display(MODE_CLOCK_24);
    delay(5000);
    // display(MODE_DATE);
    // delay(3000);
    display(MODE_ANI_MSG);
    delay(10000);
    // stop animsg
    // AniMsgCurLetterPos = 0; // reset position if required
    AniMsg = false;
    AniMsgReady = false;

    // if (WiFi.status()) {
    //     Serial.print("Online in Network : ");
    //     Serial.println(WiFi.localIP().toString());
    // }

}
