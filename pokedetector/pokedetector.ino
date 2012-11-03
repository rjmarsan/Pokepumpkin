#define TOUT 8
#define TIN 9
#define TGUARD 10

int  i;
unsigned int x, y;
float accum, fout, fval = .07;    // these are variables for a simple low-pass (smoothing) filter - fval of 1 = no filter - .001 = max filter

void setup() {
  Serial.begin(115200);

  DDRB=B101;     // DDR is the pin direction register - governs inputs and outputs- 1's are outputs
  // Arduino pin 8 output, pin 9 input, pin 10 output for "guard pin"
  //  preceding line is equivalent to three lines below
  pinMode(TOUT, OUTPUT);     // output pin
  pinMode(TIN, INPUT);      // input pin
  pinMode(TGUARD, OUTPUT);    // guard pin
  digitalWrite(TGUARD, LOW);  //could also be HIGH - don't use this pin for changing output though
}

void loop() {
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
  delay(50);
}

