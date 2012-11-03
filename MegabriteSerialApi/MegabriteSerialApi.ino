
/********* Octo API stuff *****************/
#define clockpin 13 // CI
#define enablepin 10 // EI
#define latchpin 9 // LI
#define datapin 11 // DI
 
#define NumLEDs 2
 
int LEDChannels[NumLEDs][3] = {0};
int SB_CommandMode;
int SB_RedCommand;
int SB_GreenCommand;
int SB_BlueCommand;

/********* touch detection stuff ************/
#define TOUT 8
#define TIN 9
#define TGUARD 10

int  i;
unsigned int x, y;
float accum, fout, fval = .07;    // these are variables for a simple low-pass (smoothing) filter - fval of 1 = no filter - .001 = max filter






 
void setup() {
   /*** OCTOAPI ***/
   pinMode(datapin, OUTPUT);
   pinMode(latchpin, OUTPUT);
   pinMode(enablepin, OUTPUT);
   pinMode(clockpin, OUTPUT);
   SPCR = (1<<SPE)|(1<<MSTR)|(0<<SPR1)|(0<<SPR0);
   digitalWrite(latchpin, LOW);
   digitalWrite(enablepin, LOW);
 
   Serial.begin(115200);
   for (int i = 1; i < NumLEDs; i++)
    {
      setLED(i, 1023, 1023, 0);
    }
    WriteLEDArray();
    /*** TOUCH ***/
    pinMode(TOUT, OUTPUT);     // output pin
    pinMode(TIN, INPUT);      // input pin
    pinMode(TGUARD, OUTPUT);    // guard pin
    digitalWrite(TGUARD, LOW);  //could also be HIGH - don't use this pin for changing output though

}
 
void SB_SendPacket() {
 
    if (SB_CommandMode == B01) {
     SB_RedCommand = 120;
     SB_GreenCommand = 100;
     SB_BlueCommand = 100;
    }
 
    SPDR = SB_CommandMode << 6 | SB_BlueCommand>>4;
    while(!(SPSR & (1<<SPIF)));
    SPDR = SB_BlueCommand<<4 | SB_RedCommand>>6;
    while(!(SPSR & (1<<SPIF)));
    SPDR = SB_RedCommand << 2 | SB_GreenCommand>>8;
    while(!(SPSR & (1<<SPIF)));
    SPDR = SB_GreenCommand;
    while(!(SPSR & (1<<SPIF)));
 
}
 
void WriteLEDArray() {
 
    SB_CommandMode = B00; // Write to PWM control registers
    for (int h = 0;h<NumLEDs;h++) {
	  SB_RedCommand = LEDChannels[h][0];
	  SB_GreenCommand = LEDChannels[h][1];
	  SB_BlueCommand = LEDChannels[h][2];
	  SB_SendPacket();
    }
 
    delayMicroseconds(15);
    digitalWrite(latchpin,HIGH); // latch data into registers
    delayMicroseconds(15);
    digitalWrite(latchpin,LOW);
 
    SB_CommandMode = B01; // Write to current control registers
    for (int z = 0; z < NumLEDs; z++) SB_SendPacket();
    delayMicroseconds(15);
    digitalWrite(latchpin,HIGH); // latch data into registers
    delayMicroseconds(15);
    digitalWrite(latchpin,LOW);
 
}

void setLED(byte LED, int red, int green, int blue)
{
  LEDChannels[LED][0] = red;
  LEDChannels[LED][1] = green;
  LEDChannels[LED][2] = blue;

}

void clearLEDs() {
   for (int i = 1; i < NumLEDs; i++)
  {
    setLED(i, 0, 0, 0);
  }
  WriteLEDArray();
}
 
byte inByte;
int in[4] = {0,0,0,0}; 
int pos = 0;
void loop() {

   if (Serial.available() > 0) {

    inByte = Serial.read();
    if ((inByte >= 48) && (inByte <= 57)) { // if ASCII numeric '0' - '9'
      in[pos] = in[pos] * 10 + (inByte - 48);
    }
    else if (inByte == ' ') {
      pos++;
      if (pos >= 4) pos = 0;
    }
    else if (inByte == ',' || inByte == 'n') {
      setLED(in[0], in[1], in[2], in[3]);
      /*
      Serial.print('[', BYTE);
      Serial.print(in[0], DEC);
      Serial.print(',', BYTE);
      Serial.print(in[1], DEC);
      Serial.print(',', BYTE);
      Serial.print(in[2], DEC);
      Serial.print(',', BYTE);
      Serial.print(in[3], DEC);
      Serial.print(']', BYTE);
      */
      pos = 0;
      in[0] = 0;
      in[1] = 0;
      in[2] = 0;
      in[3] = 0;
    }
    else if (inByte == 'W') {
      WriteLEDArray();
      //Serial.print('!', BYTE);
      pos = 0;
    }
    else if (inByte == 'C') {
      clearLEDs();
      pos = 0;
    }
    //Serial.print(inByte, BYTE);
    
    
   }
   touch();
   
   
   /*
  int i;
  static int x, y, z;
  
  for (i = 1; i < 8; i++)
  {
    setLED(i, 0, 0, 0);
  }
  
  setLED(x, 1023, 1023, 1023);
  x++;
  if (x > 7)
    x = 1;
  WriteLEDArray();
  delay(500);
 
 */
}


void touch() {
    y = 0;        // clear out variables
  x = 0;

  for (i=0; i < 4 ; i++ ){       // do it four times to build up an average - not really neccessary but takes out some jitter

    // LOW-to-HIGH transition
    //PORTD = PORTD | 1;                    // Same as line below -  shows programmer chops but doesn't really buy any more speed
    digitalWrite(TOUT, HIGH);    
    // output pin is PortD2 (Arduino , sensor pin is PortD3 (Arduinio )                                   

    //while ((PIND2 & D5) != D5 ) {        // while the sense pin is not high
    while (digitalRead(TIN) != 1) {    // same as above port manipulation above - only 20 times slower!                
      x++;
    }
    delay(1);

    //  HIGH-to-LOW transition
    //PORTD = PORTD & 0xFE;                // Same as line below - these shows programmer chops but doesn't really buy any more speed
    digitalWrite(TOUT, LOW);              
    //while((PIND2 & D5) != 0 ){            // while pin is not low  -- same as below only 20 times faster
    while(digitalRead(TIN) != 0 ) {    // same as above port manipulation - only 20 times slower!
      y++;  
    }

    delay(1);
  }

  fout =  (fval * (float)x) + ((1-fval) * accum);  // Easy smoothing filter "fval" determines amount of new data in fout
  accum = fout;   

  Serial.print((long)x, DEC);    // raw data - Low to High
  Serial.print( "   ");
  Serial.print((long)y, DEC);    // raw data - High to Low
  Serial.print( "   ");
  Serial.println( (long)fout, DEC); // Smoothed Low to High
}

