#include <ADCInput.h>
//Change the following lines based on your pin GPIO numbers
#define W_CLK 8
#define FQ_UD 9
#define DATA 10
#define RESET 11
#define CKT_OUTPUT 27
#define CKT_INPUT 26

// Pin 8 - connect to AD9850 module word load clock pin (CLK)
// Pin 9 - connect to freq update pin (FQ)
// Pin 10 - connect to serial data load pin (DATA)
// Pin 11 - connect to reset pin (RST).
#define pulseHigh(pin) \
  { \
    digitalWrite(pin, HIGH); \
    digitalWrite(pin, LOW); \
  }
// transfers a byte, a bit at a time, LSB first to the 9850 via serial DATA line
int max_volt;
ADCInput adc(CKT_INPUT);
void tfr_byte(byte data) {
  for (int i = 0; i < 8; i++, data >>= 1) {
    digitalWrite(DATA, data & 0x01);
    pulseHigh(W_CLK);
    //after each bit sent, CLK is pulsed high
  }
}
// frequency calc from datasheet page 8 = <sys clock> * <frequency tuning word>/2^32
void sendFrequency(double frequency) {
  int32_t freq = frequency * 4294967295 / 125000000;  // note 125 MHz clock on 9850
  for (int b = 0; b < 4; b++, freq >>= 8) {
    tfr_byte(freq & 0xFF);
  }
  tfr_byte(0x000);
  // Final control byte, all 0 for 9850 chip
  pulseHigh(FQ_UD);  // Done! Should see output
}

// void SetFrequency(unsigned long frequency)
// {
// unsigned long tuning_word = (frequency * pow(2, 32)) / DDS_CLOCK;
// digitalWrite (LOAD, LOW);
// shiftOut(DATA, CLOCK, LSBFIRST, tuning_word);
// shiftOut(DATA, CLOCK, LSBFIRST, tuning_word &gt;&gt; 8);
// shiftOut(DATA, CLOCK, LSBFIRST, tuning_word &gt;&gt; 16);
// shiftOut(DATA, CLOCK, LSBFIRST, tuning_word &gt;&gt; 24);
// shiftOut(DATA, CLOCK, LSBFIRST, 0x0);
// digitalWrite (LOAD, HIGH);
// }

void setup() {
  // configure arduino data pins for output
  pinMode(FQ_UD, OUTPUT);
  pinMode(W_CLK, OUTPUT);
  pinMode(DATA, OUTPUT);
  pinMode(RESET, OUTPUT);
  pulseHigh(RESET);
  pulseHigh(W_CLK);
  pulseHigh(FQ_UD);
  max_volt = 0;
  adc.setBuffers(4,32);
  adc.begin(500000);
  // this pulse enables serial mode - Datasheet page 12 figure 10
}
void loop() {
  // int arr[250];
  // int count = 0;
  int i=50000;
  String line = "";
  sendFrequency(i);
  while (1) {
    sendFrequency(i);
    int max_volt = 0;
    for(int j=0;j<=50;j++){
      int reading = adc.read();
      if(reading>max_volt){
        max_volt=reading;
      }
    }
    line = line+String(max_volt)+",";
    i=(i+1000)%300000;
    if(i==0){
      i=50000;
      line=line+"\n";
      Serial.println(line);
      line="";
    }
  }
  // while (1) {
  //   int reading = analogRead(ANALOG);
  //   if (reading > max_volt) {
  //     max_volt = reading;
  //   }
  //   Serial.print(reading);
  //   Serial.print(",");
  //   Serial.print(max_volt);
  //   Serial.println();
  //   asin(double);
  //Serial.println(sin(PI/2));
  //}
}
