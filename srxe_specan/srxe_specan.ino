#include <SmartResponseXE.h>
#include <CC1101.h>
#include "src/specan.h"
#include <stdint.h>

void poll_keyboard() {
  u8 vstep;
  u8 hstep;

  vstep = (height == TALL) ? TALL_STEP : SHORT_STEP;

  switch (width) {
  case NARROW:
    hstep = NARROW_STEP;
    break;
  case ULTRAWIDE:
    hstep = ULTRAWIDE_STEP;
    break;
  default:
    hstep = WIDE_STEP;
    break;
  }

  switch (SRXEGetKey()) {
  case 0:
    return;
  case '9':
    SRXEWriteString(0,20,"900MHz band", FONT_LARGE,3,0);
    user_freq = CEN_900;
    break;
  case '4':
    SRXEWriteString(0,20,"400MHz band", FONT_LARGE,3,0);
    user_freq = CEN_400;
    break;
  case '3':
    SRXEWriteString(0,20,"300MHz band", FONT_LARGE,3,0);
    user_freq = CEN_300;
    break;
  case 'w':
    SRXEWriteString(0,20,"WIDE", FONT_LARGE,3,0);          
    set_width(WIDE);
    break;
  case 'n':
    SRXEWriteString(0,20,"NARROW", FONT_LARGE,3,0);        
    set_width(NARROW);
    break;
  case 'u':
    SRXEWriteString(0,20,"ULTRAWIDE", FONT_LARGE,3,0);      
    set_width(ULTRAWIDE);
    break;
  case ' ':
    switch (width) {
    case WIDE:
      SRXEWriteString(0,20,"NARROW", FONT_LARGE,3,0);    
      set_width(NARROW);
      break;
    case NARROW:
      SRXEWriteString(0,20,"ULTRAWIDE", FONT_LARGE,3,0);    
      set_width(ULTRAWIDE);
      break;
    default:
      SRXEWriteString(0,20,"WIDE", FONT_LARGE,3,0);
      set_width(WIDE);
      break;
    }
    break;
  case 't':
    SRXEWriteString(0,20,"Heigth TALL", FONT_LARGE,3,0);
    height = TALL;
    break;
  case 'l':
    SRXEWriteString(0,20,"Heigth SHORT", FONT_LARGE,3,0);
    height = SHORT;
    break;
  case 8: // DEL
    height = !height;
    break;
  case 3: // > 
    user_freq += hstep;
    break;
  case 2: // < 
    user_freq -= hstep;
    break;
  case 4: // ^
  case 'q': 
     SRXEWriteString(0,20,"vscroll++", FONT_LARGE,3,0);
    vscroll = MAX(vscroll - vstep, MIN_VSCROLL);
    break;
  case 5:
  case 'a': // down arrow
    SRXEWriteString(0,20,"vscroll--", FONT_LARGE,3,0);
    vscroll = MIN(vscroll + vstep, MAX_VSCROLL);
    break;
  case 'm':
    SRXEWriteString(0,20,"Max hold", FONT_LARGE,max_hold? 3:0, max_hold? 0:3);
    max_hold = !max_hold;
    delay(100);
    break;
  case 's':
    /* s for stop pause */
    while (SRXEGetKey() == 's');
    while (SRXEGetKey() != 's')
      delay(200);
    break;
  case 'p':
      SRXEWriteString(0,20,"Persistence", FONT_LARGE,persistence? 3:0, persistence? 0:3);
    if (persistence == 1)
      persistence = PERSIST;
    else
      persistence = 1;
    set_center_freq(center_freq);
    sweep %= persistence;
    break;
  case 0xf0:
      SRXEWriteString(0,20,"Sleeping", FONT_LARGE,3, 0);
      SRXESleep();


  default:
    break;
  }
}


void setup() {
 // Serial.begin(2000000);

  SRXEInit(0xe7, 0xd6, 0xa2); // initialize display
  Serial.println("Initialized display.");
  cc1101.Init();
  delay(100);
  Serial.println("cc1101 init!");
  init_all();
  delay(100);
  Serial.println("Initialized radio!");

}



void draw_ruller(){
  
  SRXEHorizontalLine(0,108,127,3,2);

  for(int i = 1 ; i < 128; i++){
    if(((i+12) % 5) == 0)   SRXEVerticalLine(i,108,6,3);// low notches
    if(((i+12) % 25) == 0)  SRXEVerticalLine(i,108,12,3);// high notches
  }
}

void draw_frequency(){
  char ai[5];
  switch (width) {
    case NARROW:
      itoa(user_freq,ai,10);
      SRXEWriteString(168,121,ai, FONT_MEDIUM, 3, 0); 
      itoa(user_freq-2,ai,10);
      SRXEWriteString(21,121,ai, FONT_MEDIUM, 3, 0);
      itoa(user_freq+2,ai,10);
      SRXEWriteString(318,121,ai, FONT_MEDIUM, 3, 0);
      break;
    case ULTRAWIDE:
      itoa(user_freq,ai,10);
      SRXEWriteString(168,121,ai, FONT_MEDIUM, 3, 0); 
      itoa(user_freq-40,ai,10);
      SRXEWriteString(21,121,ai, FONT_MEDIUM, 3, 0);
      itoa(user_freq+40,ai,10);
      SRXEWriteString(318,121,ai, FONT_MEDIUM, 3, 0);
      break;
    default:
      itoa(user_freq,ai,10);
      SRXEWriteString(168,121,ai, FONT_MEDIUM, 3, 0); 
      itoa(user_freq-10,ai,10);
      SRXEWriteString(21,121,ai, FONT_MEDIUM, 3, 0);
      itoa(user_freq+10,ai,10);
      SRXEWriteString(318,121,ai, FONT_MEDIUM, 3, 0);
  }
}

void plotch(uint8_t ch){

  uint8_t ss = 0;
  uint8_t s;
  uint8_t m;

  for(int i = 0; i < persistence; i++ ){
    ss = MAX(chan_table[ch].ss[i],ss);
  }
  
  if(ss == chan_table[ch].last_drawn) { //skip redrawing if no change 
    return;
  }
  chan_table[ch].last_drawn = ss;

  if(height == TALL){
    s = MAX((ss - vscroll)>>1,0);
    m = MAX((chan_table[ch].max - vscroll)>>1,0);
  }else{
    s = MAX((ss - vscroll)>>2,0);
    m = MAX((chan_table[ch].max - vscroll)>>2,0);
  }

  SRXEVerticalLine(ch-4,0,106,0); //clear bar
 
  uint8_t y =  (106 - s); // bar height
  uint8_t ym = (106 - m);

  // we start from 4th channel, but want it in first column
  if(max_hold)SRXEVerticalLine(ch-4,ym, m ,2); //draw max with gray
  SRXEVerticalLine(ch-4,y,s,3); // draw current with black

}
  int t = 0;
void loop() {

  uint8_t ch = 0; 

  if (user_freq != center_freq){
    user_freq = set_center_freq(user_freq);
  }
  
  draw_ruller();
  
  draw_frequency();

  // the screen can only fit 128 channels (3px per channel)
  // so we skip first 4 , rather than redoing the tuning
  for (ch = 4; ch < NUM_CHANNELS-1; ch++) { 
    tune(ch);
    poll_keyboard(); 

    cc1101.SpiStrobe(CC1101_SRX);
    plotch(ch); 
    if (width == NARROW)    for (int i = 350; i-- ;); // copied , check if necessary, drawing to the screen is pretty slow        
    
    chan_table[ch].ss[sweep] = cc1101.SpiReadStatus(CC1101_RSSI) ^ 0x80;
    
    if (max_hold){
      chan_table[ch].max = MAX(chan_table[ch].ss[sweep],chan_table[ch].max);
    }else{
      chan_table[ch].max = 0;         
    }
    
    cc1101.SpiStrobe(CC1101_SIDLE);
  }  
  ++sweep;
  sweep %= persistence;
  
}
