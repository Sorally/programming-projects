/* 
 *  Arduino analog input 5 - I2C SCL
 *  Arduino analog input 4 - I2C SDA
 *  Arduino digital output 3 - IR LED
 *  Arduino digital output 8 - i2c master alerter
 *  Arduino digital output 10 - OneWire dallas thermometer
 *  Arduino digital output 11 - IR Receiver
 *
 */

#define SANYO_HDR_SPACE        3360
#define SANYO_ONE_SPACE        1160          
#define SANYO_BIT_MARK	        480
#define SANYO_ZERO_SPACE	320

#include "TimerOne.h"         // querying temperature at set intervals
#include <Wire.h>             // i2c communications
#include <OneWire.h>          // dallas 1wire communications 
#include <IRremote.h>         // recieving & transmitting IR

int irmsg;
char current_temp[4];
unsigned char checksum;
char msg;
int IRledPin;
int get_temperature;
int HighByte, LowByte, TReading, SignBit, Tc_100, Whole, Fract;
OneWire ds(10);
IRsend irsend;
int RECV_PIN = 11;
IRrecv irrecv(RECV_PIN);
int I2C_MASTER_ALERTER;
decode_results results;


/****************************************************************************
 * setup()                                
 *
 *
 ****************************************************************************/
void setup() {
  
  Serial.begin(9600);
  I2C_MASTER_ALERTER = 12;     // pin 8
  checksum = 0;
  IRledPin =  3;    // LED connected to digital pin 3
  irmsg = 0;
  get_temperature = 10;
  int tmp = 0;

  irrecv.enableIRIn(); // Start the receiver
  Timer1.initialize(10000000); // 10 seconds
  Timer1.attachInterrupt(temperature_callback);

  pinMode(IRledPin, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(I2C_MASTER_ALERTER, OUTPUT);
  Wire.begin(0x52);  // 0101 0010  

  Wire.onReceive(receiveData);  // register event
  Wire.onRequest(requestEvent); // register event
  Serial.println("Ready");
}


/****************************************************************************
 * requestEvent()                                
 *
 * called when the master requests data
 ****************************************************************************/
void requestEvent() {

    Wire.send(Whole);  // send current temperature as Whole number
}


/****************************************************************************
 * receiveData()                                
 *
 * called when data arrives from the master
 ****************************************************************************/
void receiveData(int numBytes) {
  
  int i = 0;
  checksum = 0;
  int test = 0;
  
  if (Wire.available()) {
    msg = Wire.receive();
    char msg2 = Wire.receive();
    while (msg2) {
      msg2 = Wire.receive();
      test++;
    }

    switch (msg) {
      case 0x02:
        //get_temp();
        break;
      
      case 0x03:  // switch aircon ON
        switchAircon(1);
        break;
     
      case 0x04:  // switch aircon OFF
        switchAircon(0);
        break;
  
      case 0x05: // toggle light (on/off)
        toggle_light();
        delay(300);
        toggle_light();
        break;

      case 0x06: // switch light (in steps)
        toggle_light();
        break;
       
    }

    //Serial.print("received Data:");
    Serial.print(msg, HEX);
    Serial.print(" ");
    Serial.println(test, HEX);
  }
}


/****************************************************************************
 * pulseIR()                                
 *
 * This procedure sends a 38KHz pulse to the IRledPin for a certain # of 
 * microseconds. We'll use this whenever we need to send codes
 ****************************************************************************/
void pulseIR(long microsecs) {
  // we'll count down from the number of microseconds we are told to wait
  //Serial.println("pulseIR()");
  cli();  // this turns off any background interrupts
 
  while (microsecs > 0) {
    // 38 kHz is about 13 microseconds high and 13 microseconds low
   digitalWrite(IRledPin, HIGH);  // this takes about 3 microseconds to happen
   delayMicroseconds(10);         // hang out for 10 microseconds
   digitalWrite(IRledPin, LOW);   // this also takes about 3 microseconds
   delayMicroseconds(10);         // hang out for 10 microseconds
 
   // so 26 microseconds altogether
   microsecs -= 26;
  }
 
  sei();  // this turns them back on
}


/****************************************************************************
 * switchAircon()                                
 *
 *
 ****************************************************************************/
void switchAircon(int OnOff) {

  unsigned char currentPulse;
  unsigned char pwrOn[17] =  {0x81, 0x00, 0x14, 0x00, 0xe1, 0x20, 0x0a, 0xbb, 0xe2, 0x00, 0x0b, 0x00, 0x00, 0x40, 0x00, 0x00, 0x15};
  unsigned char pwrOff[17] = {0x81, 0x00, 0x14, 0x00, 0xe1, 0x40, 0x0a, 0xbb, 0xe0, 0x00, 0x0b, 0x00, 0x00, 0x40, 0x02, 0x00, 0x65};
  
  pulseIR(SANYO_HDR_SPACE);

  for (int i = 0; i < 17; i++) {
  
    currentPulse = OnOff?pwrOn[i]:pwrOff[i];
  
    for (int j = 7; j >= 0; j--) {
      if ((currentPulse >> j) & 1) {
        delayMicroseconds(SANYO_ONE_SPACE);
      } else {
        delayMicroseconds(SANYO_ZERO_SPACE);
      }  
        pulseIR(SANYO_BIT_MARK);
    }
  }
    
  delayMicroseconds(SANYO_ZERO_SPACE);
  pulseIR(SANYO_BIT_MARK);
  
}


/****************************************************************************
 * toggle_light()                                
 *
 *
 ****************************************************************************/
void toggle_light(void) {
  //Serial.println("toggle_light()");
  irsend.enableIROut(38);
  irsend.sendNEC(0x41B658A7, 32); // Living room power
  irrecv.enableIRIn();
}


/****************************************************************************
 * flash_master()                                
 *
 *
 ****************************************************************************/
void flash_master(int val) {
  
  //Serial.print("flash master ");
  //Serial.println(irmsg);

  digitalWrite(13, HIGH);
  delay(50);

  irmsg = val;
  digitalWrite(I2C_MASTER_ALERTER, HIGH);   // set the LED on
  delay(1);
  digitalWrite(I2C_MASTER_ALERTER, LOW);    // set the LED off

}


/****************************************************************************
 * loop()                                
 *
 *
 ****************************************************************************/
void loop() {

  if (irrecv.decode(&results)) {
    if (results.value == 0x2FD58A7) {           // volume up
      flash_master(1);
    } else if (results.value == 0x2FD7887) {    // volume down
      flash_master(2);
    } else if (results.value == 0x2FD08F7) {    // volume mute
      flash_master(3);
    } else if (results.value == 0x27DB44B) {    // play
      flash_master(4);
    } else if (results.value == 0x2FD0AF5) {    // pause
      flash_master(5);
    } else if (results.value == 0x27D748B) {    // next
      flash_master(6);
    } else if (results.value == 0x27D34CB) {    // previous
      flash_master(7);
    } else if (results.value == 0x27DD42B) {    // stop
      flash_master(8);
    }
    
    irrecv.resume(); // Receive the next value
  }
  digitalWrite(13, LOW);

}


/****************************************************************************
 * get_temp()                                
 *
 *
 ****************************************************************************/
void get_temp() {
  
  byte i;
  byte present = 0;
  byte data[12];
  byte addr[8];
  char temp[3];
  
  if ( !ds.search(addr)) {
      ds.reset_search();
      return;
  }
 
  if ( OneWire::crc8( addr, 7) != addr[7]) {
      //Serial.print("CRC is not valid!\n");
      return;
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44,1);         // start conversion, with parasite power on at the end

  delay(1000);     // maybe 750ms is enough, maybe not

  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad

  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
  }

  LowByte = data[0];
  HighByte = data[1];
  TReading = (HighByte << 8) + LowByte;
  SignBit = TReading & 0x8000;  // test most sig bit
  if (SignBit) { // negative
    TReading = (TReading ^ 0xffff) + 1; // 2's comp
  }
  Tc_100 = (6 * TReading) + TReading / 4;    // multiply by (100 * 0.0625) or 6.25
  Whole = Tc_100 / 100;  // separate off the whole and fractional portions
  Fract = Tc_100 % 100;

  int temp_ptr = 0;
  memset(temp, 0, 3);

  Whole *= SignBit?-1:1;

  

  //itoa (Whole, temp + strlen(temp), 10);
  //temp[strlen(temp)] = '.';
  
  //if (Fract < 10) {
  //  temp[strlen(temp)] = '0'; 
  //}

  //itoa(Fract, temp + strlen(temp), 10);

  //strcpy(current_temp, temp);
  
}


/****************************************************************************
 * temperature_callback()                                
 *
 *
 ****************************************************************************/
void temperature_callback(void) {
  
  get_temperature++;

  if (get_temperature > 1) {
    get_temperature = 0;
    cli();
    get_temp();
    sei();
  }
}


