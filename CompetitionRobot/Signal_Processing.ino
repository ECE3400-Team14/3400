/**
 * FFT analysis code for Audio and IR signal analysis
 * Based off example sketch fft_adc_serial provided by the FFT library
 * Written by: David Burgstahler (dfb93), Gregory Kaiser (ghk48)
 */

#define LOG_OUT 1 // use the log output function
#define FFT_N 128 // set to 256 point fft

#include <FFT.h> // include the library

bool IR_initilized = false;
const byte IR_threshold = 75; //67 (a little more range)
//Audio FFT Variables
const byte threshold = 110; //the monitored signal bin amplitude threshold for detection (100)
byte start_count = 0;       // counter for keeping track of back-to-back detections
const byte count_max = 20;  // the number of back-to-back detections for a confirmed detection

int OLD_ADCSRA;

//The setup FFT function
void fft_setup()
{
  ADMUX = 0x40; // use adc0
  DIDR0 = 0x01; // turn off the digital input for adc0
}

//Performs a full FFT analysis. ~ 256 samples/sample rate (~26 ms Audio, ~3 ms IR)
//return true for valid detection. false for no detection
void fft_analyze()
{
  fft_setup();
  OLD_ADCSRA = ADCSRA;
  cli(); // UDRE interrupt slows this way down on arduino1.0
  for (int i = 0; i < 256; i += 2)
  { // save 256 samples
    while (!(ADCSRA & 0x10))
      ; // wait for adc to be ready

    if (!has_started)
      ADCSRA = 0xf7; // restart adc for audio (128 prescale factor) [9600 Hz sample rate]
    else
    {
      ADCSRA = 0xe4;
      ADCSRA = 0xf4; //IR prescale factor (16) [~76900 Sample Rate]
    }

    byte m = ADCL; // fetch adc data
    byte j = ADCH;
    int k = (j << 8) | m; // form into an int
    k -= 0x0200;          // form into a signed int
    k <<= 6;              // form into a 16b signed int

    //int k = analogRead(A0);
    fft_input[i] = k;     // put real data into even bins
    fft_input[i + 1] = 0; // set odd bins to 0
  }
  fft_window();  // window the data for better frequency response
  fft_reorder(); // reorder the data before doing the fft
  fft_run();     // process the data in the fft
  fft_mag_log(); // take the output of the fft
  sei();
  //660 hz Audio Analysis
  if (!has_started)
  {
    //Serial.println(fft_log_out[17]);
    if (fft_log_out[9] > threshold)
    {
      if (debug)
        Serial.println("660Hz detected!");
      //Serial.println(fft_log_out[17]);
      start_count++;
      if (start_count == count_max)
      {
        if (debug)
        {
          Serial.println("Go!!!!!!");
          Serial.println("Disabling Audio Detection");
          Serial.println("Enabling IR Detection");
        }
        has_started = true; //start the Robot and IR analysis
        //digitalWrite(mux_pin, HIGH);//Set mux to IR input
        start_count = 0;
        fft_detect = true;
      }
      else
        fft_detect = false;
    }
    else
    {
      start_count = (start_count != 0) ? start_count - 1 : 0;
      fft_detect = false;
    }
  }
  //IR Analysis
  else
  {
    if (IR_initilized == false)
    { //ADCSRA = 0xe4;
      IR_initilized = true;
    }
    else
    {
      if (fft_log_out[11] > IR_threshold /*55*/)
      {
        if (debug)
          Serial.print("Robot Detected!!!");
        fft_detect = true;
      }
      else
      {
        if (debug)
          Serial.print("No Robot Detected");
        start_count = (start_count != 0) ? start_count - 1 : 0;
        fft_detect = false;
      }
    }
  }

  ADCSRA = OLD_ADCSRA;
}
