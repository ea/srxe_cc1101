#include "src/cc1101/ELECHOUSE_CC1101.h"
#include <SmartResponseXE.h>
#include "src/pocsag/pocsag.h"

#define MSG_MAX 512
byte buffer[MSG_MAX] = {0};
char buffer_hex[MSG_MAX*2] = {0};
byte msgbuff[60] = {0};
int msgidx = 0;
int line=60; 



void setup()
{

  //having serial enabled interfers with 5th keyboard row, till we find a workaround...
  Serial.begin(9600);

  SRXEInit(0xe7, 0xd6, 0xa2); // initialize display
  Serial.println("Initialized display.");
  SRXEWriteString(0,0,"To send:", FONT_LARGE, 3, 0); 
  SRXEWriteString(0,40,"Received:", FONT_LARGE, 3, 0); 
  
  ELECHOUSE_cc1101.Init(F_DAP);
  delay(100);
  ELECHOUSE_cc1101.SetReceive();
}



// NOTE: doesn't send a valid POCSAG message, just plain ASCII , but with all the same radio config, for testing.
void sendmsg(){
  ELECHOUSE_cc1101.Init(F_DAP);
  delay(100);
  ELECHOUSE_cc1101.SendData(msgbuff, strlen((char*)msgbuff));
  ELECHOUSE_cc1101.Init(F_DAP);
  ELECHOUSE_cc1101.SetReceive();
}
  

void tohex(byte *in, char *out, unsigned int len)
{
        unsigned int i;
        memset(out,'\0',MSG_MAX*2);
        if(in == NULL || len == 0 ) return;
        if(len > MSG_MAX) len = MSG_MAX-1;

        for (i=0; i<len; i++) {
                out[i*2]   = "0123456789ABCDEF"[in[i] >> 4];
                out[i*2+1] = "0123456789ABCDEF"[in[i] & 0x0F];
        }
        out[len*2] = '\0';
}



void loop()
{
 
  byte ch;
  if(ch = SRXEGetKey()){
    msgbuff[msgidx] = ch;
    msgidx +=1;
    if(ch == 5){ //send on arrow down
        sendmsg();
        memset(msgbuff,0,60);
        msgidx = 0;
      } 
    if(msgidx > 20){ // or if we are past 20 bytes ...
        sendmsg();
        memset(msgbuff,0,60);
        msgidx = 0;
      }
      
      SRXEWriteString(0,20,"                                        ", FONT_LARGE, 3, 0);
      SRXEWriteString(0,20,(char *) msgbuff, FONT_LARGE, 3, 0);
   }

  if (ELECHOUSE_cc1101.CheckReceiveFlag()){

    int len = ELECHOUSE_cc1101.ReceiveData(buffer);

    if(len) {
      buffer[len] = '\0';
      tohex(buffer,buffer_hex,len);
      Serial.println((char *) buffer_hex);
      
      uint32_t *words;
      words=(uint32_t*) (buffer+3);
      pocsag_newbatch();
        for(int i=0;i<15;i++){
          pocsag_handleword(__builtin_bswap32(words[i])^0xFFFFFFFF);
        }
    
      if(pocsag_msg_type == 3){ // alphanum
        SRXEWriteString(0,line,"                                        ", FONT_LARGE, 3, 0);
        memset(buffer,0,MSG_MAX);
        ultoa(pocsag_lastid,(char *)buffer,10);
        strcat((char *)buffer, ": ");
        strcat((char *)buffer,pocsag_buffer);
        Serial.println((char*)buffer);
        SRXEWriteString(0,line,(char *) buffer, FONT_LARGE, 3, 0); 
        line+=20;
        if(line > 100) line = 60;
      }
    }
    ELECHOUSE_cc1101.SetReceive();
  }
} 
