/*
	Project  : LaLiMat project (https://www.youtube.com/playlist?list=PLJBKmE2nNweRXOebZjydkMEiq2pHtBMOS in Chinese)
 	file     : logo.h
	Author   : ykchau
 	youtube  : youtube.com/ykchau888
  	Licenese : GPL-3.0
   	Please let me know if you use it commercial project.
*/

/****************************
    1. A [8][8] array for each logo
    2. Each point store 1 ~ 3 color data
    3. Color store at seperate variable

    example : youtube logo

    No Color = 0
    Base Color = 1 (store at [0])
    Second Color = 2
    Third Color = 3

    0 0 0 0 0 0 0 0
    0 0 1 1 1 1 0 0
    0 1 3 2 3 3 1 0
    0 1 3 2 2 3 1 0
    0 1 3 2 3 3 1 0
    0 0 1 1 1 1 0 0
    0 0 0 0 0 0 0 0
    0 0 0 0 0 0 0 0

 ***************************/


const byte Logo_Youtube[8][8] PROGMEM = {
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 1, 1, 1, 1, 0, 0,
  0, 1, 3, 2, 3, 3, 1, 0,
  0, 1, 3, 2, 2, 3, 1, 0,
  0, 1, 3, 2, 3, 3, 1, 0,
  0, 0, 1, 1, 1, 1, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0
};

const byte Color_Youtube[3][3] PROGMEM = {
  {255, 0, 0},
  {255, 255, 255},
  {200, 0, 0}
};

const byte Logo_Instagram[8][8] PROGMEM = {
  2, 2, 2, 3, 3, 3, 3, 3,
  3, 1, 1, 1, 1, 1, 1, 3,
  3, 1, 2, 3, 3, 1, 1, 3,
  5, 1, 5, 1, 1, 4, 1, 4,
  4, 1, 6, 1, 1, 4, 1, 4,
  4, 1, 7, 4, 4, 4, 1, 4,
  7, 1, 1, 1, 1, 1, 1, 4,
  8, 8, 7, 7, 4, 4, 4, 4
};

const byte Color_Instagram[8][3] PROGMEM = {
  {255, 255, 255},
  {143, 50, 248},
  {247, 42, 239},
  {234, 18, 47},
  {250, 43, 187},
  {248, 30, 93},
  {253, 128, 82},
  {249, 236, 106}
};

const byte Logo_Facebook[8][8] PROGMEM = {
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 1, 1, 1, 1, 0, 0,
  0, 1, 3, 3, 2, 2, 1, 0,
  0, 1, 3, 3, 2, 3, 1, 0,
  0, 1, 3, 2, 2, 2, 1, 0,
  0, 1, 3, 3, 2, 3, 1, 0,
  0, 0, 1, 1, 1, 1, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0
};

const byte Color_Facebook[3][3] PROGMEM = {
  {0, 0, 100},
  {255, 255, 255},
  {0, 0, 255}
};

const byte Logo_Twitter[8][8] PROGMEM = {
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 1, 0, 0, 0, 1, 1, 0,
  0, 0, 1, 0, 1, 2, 1, 1,
  0, 1, 2, 1, 2, 2, 1, 0,
  0, 0, 1, 2, 2, 2, 1, 0,
  0, 0, 0, 1, 2, 1, 0, 0,
  0, 0, 1, 1, 1, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0
};

const byte Color_Twitter[3][3] PROGMEM = {
  {0, 100, 255},
  {0, 100, 200},
  {0, 0, 0}
};

const byte Logo_Twitch[8][8] PROGMEM = {
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 1, 1, 1, 1, 1, 1, 1,
  0, 1, 0, 0, 0, 0, 0, 1,
  0, 1, 0, 1, 0, 1, 0, 1,
  0, 1, 0, 0, 0, 0, 0, 1,
  0, 1, 1, 0, 1, 1, 1, 0,
  0, 0, 0, 1, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0
};

const byte Color_Twitch[3][3] PROGMEM = {
  {50, 0, 200},
  {0, 0, 0},
  {0, 0, 0}
};

const byte Logo_Pinterest[8][8] PROGMEM = {
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 1, 1, 1, 1, 0, 0,
  0, 1, 3, 3, 2, 3, 1, 0,
  0, 1, 3, 2, 3, 2, 1, 0,
  0, 1, 3, 2, 2, 3, 1, 0,
  0, 1, 3, 2, 3, 3, 1, 0,
  0, 0, 1, 1, 1, 1, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0
};

const byte Color_Pinterest[3][3] PROGMEM = {
  {255, 0, 0},
  {255, 255, 255},
  {200, 0, 0}
};
const byte Logo_Snapchat[8][8] PROGMEM = {
  0, 1, 1, 1, 1, 1, 1, 0,
  1, 1, 1, 2, 2, 1, 1, 1,
  1, 1, 2, 3, 3, 2, 1, 1,
  1, 1, 2, 3, 3, 2, 1, 1,
  1, 2, 3, 3, 3, 3, 2, 1,
  1, 1, 2, 3, 3, 2, 1, 1,
  1, 2, 1, 2, 2, 1, 2, 1,
  0, 1, 1, 1, 1, 1, 1, 0
};

const byte Color_Snapchat[3][3] PROGMEM = {
  {255, 255, 0},
  {255, 255, 255},
  {255, 255, 255}
};

const byte Logo_Tictok[8][8] PROGMEM = {
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 1, 0, 0, 0,
  0, 0, 0, 0, 1, 1, 0, 0,
  0, 0, 1, 2, 1, 2, 1, 0,
  0, 1, 2, 0, 1, 2, 0, 0,
  0, 1, 2, 0, 1, 2, 0, 0,
  0, 0, 1, 1, 2, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0
};

const byte Color_Tictok[3][3] PROGMEM = {
  {255, 255, 255},
  {255, 100, 100},
  {0, 0, 0}
};

#define LOGO_YOUTUBE   0
#define LOGO_INSTAGRAM 1
#define LOGO_FACEBOOK  2
#define LOGO_TWITTER   3
#define LOGO_TWITCH    4
#define LOGO_PINTEREST 5
#define LOGO_SNAPCHAT  6
#define LOGO_TICTOK    7
#define LOGO_CLOCK     8



byte (*getLogo(byte logo))[8] {
  switch (logo) {
    case 0: return Logo_Youtube; break;
    case 1: return Logo_Instagram; break;
    case 2: return Logo_Facebook; break;
    case 3: return Logo_Twitter; break;
    case 4: return Logo_Twitch; break;
    case 5: return Logo_Pinterest; break;
    case 6: return Logo_Snapchat; break;
    case 7: return Logo_Tictok; break;
    default:
      return Logo_Youtube;
  }
}

byte (*getLogoColor(byte logo))[3] {
  switch (logo) {
    case 0: return Color_Youtube; break;
    case 1: return Color_Instagram; break;
    case 2: return Color_Facebook; break;
    case 3: return Color_Twitter; break;
    case 4: return Color_Twitch; break;
    case 5: return Color_Pinterest; break;
    case 6: return Color_Snapchat; break;
    case 7: return Color_Tictok; break;
    default:
      return Color_Youtube;
  }
}
