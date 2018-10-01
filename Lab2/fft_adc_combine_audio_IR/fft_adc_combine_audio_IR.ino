/*
fft_adc_serial.pde
guest openmusiclabs.com 7.7.14
example sketch for testing the fft library.
it takes in data on ADC0 (Analog0) and processes them
with the fft. the data is sent out over the serial
port at 115.2kb.
*/

#define LOG_OUT 1 // use the log output function
//#define OCTAVE 1 // use the oct output function
//#define OCT_NORM 1
#define FFT_N 256 // set to 256 point fft

#include <FFT.h> // include the library

bool has_started = false;//false: Audio Analysis Stage, true: IR Analysis Stage
bool IR_initilized = false;
//Audio FFT Variables
int threshold = 70;//the monitored signal bin amplitude threshold for detection
int start_count = 0;// counter for keeping track of back-to-back detections
int count_max = 5;// the number of back-to-back detections for a confirmed detection

//TODO: add any IR local variables here

int mux_pin = 0; //TODO: change this port to the desired mux pin. LOW = Audio, HIGH = IR

void setup() {
  Serial.begin(9600); // use the serial port
  
  TIMSK0 = 0; // turn off timer0 for lower jitter
  ADCSRA = 0xe7; // set the adc to free running mode (change to 0xe7 for default resolution?)
  ADMUX = 0x41; // use adc0
  DIDR0 = 0x01; // turn off the digital input for adc0

  pinMode(mux_pin, OUTPUT);
  digitalWrite(mux_pin, LOW); 
}

void loop() {
  while(1) { // reduces jitter
    cli();  // UDRE interrupt slows this way down on arduino1.0
    for (int i = 0 ; i < 512 ; i += 2) { // save 256 samples
      while(!(ADCSRA & 0x10)); // wait for adc to be ready
      
      if (!has_started) ADCSRA = 0xf7; // restart adc for audio
      else  ADCSRA = 0xf4; //TODO: set for IR prescale factor
      
      byte m = ADCL; // fetch adc data
      byte j = ADCH;
      int k = (j << 8) | m; // form into an int
      k -= 0x0200; // form into a signed int
      k <<= 6; // form into a 16b signed int
      
      //int k = analogRead(A0);
      fft_input[i] = k; // put real data into even bins
      fft_input[i+1] = 0; // set odd bins to 0
    }
    fft_window(); // window the data for better frequency response
    fft_reorder(); // reorder the data before doing the fft
    fft_run(); // process the data in the fft
    fft_mag_log(); // take the output of the fft
    sei();
    if(!has_started) {
      if(fft_log_out[17] > threshold) {
        Serial.println("660Hz detected!");
        Serial.println(fft_log_out[17]);
        start_count++;
        if(start_count == 5) {
          Serial.println("Go!!!!!!");
          Serial.println("Disabling Audio Detection");
          has_started = true;//start the Robot and IR analysis
          digitalWrite(mux_pin, HIGH);//Set mux to IR input
          start_count = 0;
       }
      }
      else { start_count = (start_count != 0) ? start_count - 1 : 0;}
    }
    else {
      if (!IR_initilized){
        ADCSRA = 0xe4; // set the adc to free running mode
        IR_initilized = 1;
      }else{
        if (fft_log_out[21] > 60){
          Serial.println("Robot Detected!!!");
        }
      }
    }
    /*
    Serial.println("start");
    for (byte i = 0 ; i < FFT_N/2 ; i++) { 
      Serial.println(fft_log_out[i]); // send out the data
    }*/
    
  }
}
