/*
 Copyright (C) 2011 J. Coliz <maniacbug@ymail.com>
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

/**
 * Example for Getting Started with nRF24L01+ radios.
 *
 * This is an example of how to use the RF24 class.  Write this sketch to two
 * different nodes.  Put one of the nodes into 'transmit' mode by connecting
 * with the serial monitor and sending a 'T'.  The ping node sends the current
 * time to the pong node, which responds by sending the value back.  The ping
 * node can then see how long the whole cycle took.
 */

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

//
// Hardware configuration
//

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10

RF24 radio(9,10);

//
// Topology
//

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0x0000000028LL, 0x0000000029LL };

//
// Role management
//
// Set up role.  This sketch uses the same software for all the nodes
// in this system.  Doing so greatly simplifies testing.
//

// The various roles supported by this sketch
typedef enum { role_ping_out = 1, role_pong_back } role_e;

// The debug-friendly names of those roles
const char* role_friendly_name[] = { "invalid", "Ping out", "Pong back"};

// The role of the current running sketch
role_e role = role_pong_back;

//random number to begin with
unsigned long data = 0x982fc65b;

void setup(void)
{
  //
  // Print preamble
  //

  Serial.begin(9600);
  printf_begin();
  printf("\n\rRF24/examples/GettingStarted/\n\r");
  printf("ROLE: %s\n\r",role_friendly_name[role]);
  printf("*** PRESS 'T' to begin transmitting to the other node\n\r");

  //
  // Setup and configure rf radio
  //

  radio.begin();

  // optionally, increase the delay between retries & # of retries
  radio.setRetries(15,15);
  radio.setAutoAck(true);
  // set the channel
  radio.setChannel(0x50);
  // set the power
  // RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_MED=-6dBM, and RF24_PA_HIGH=0dBm.
  radio.setPALevel(RF24_PA_MIN);
  //RF24_250KBPS for 250kbs, RF24_1MBPS for 1Mbps, or RF24_2MBPS for 2Mbps
  radio.setDataRate(RF24_250KBPS);

  // optionally, reduce the payload size.  seems to
  // improve reliability
  //radio.setPayloadSize(8);

  //
  // Open pipes to other nodes for communication
  //

  // This simple sketch opens two pipes for these two nodes to communicate
  // back and forth.
  // Open 'our' pipe for writing
  // Open the 'other' pipe for reading, in position #1 (we can have up to 5 pipes open for reading)

  if ( role == role_ping_out )
  {
    radio.openWritingPipe(pipes[0]);
    radio.openReadingPipe(1,pipes[1]);
  }
  else
  {
    radio.openWritingPipe(pipes[1]);
    radio.openReadingPipe(1,pipes[0]);
  }

  //
  // Start listening
  //

  radio.startListening();

  //
  // Dump the configuration of the rf unit for debugging
  //

  radio.printDetails();
}

void loop(void)
{

  //
  // Pong back role.  Receive each packet, dump it out, and send it back
  //

  if ( role == role_pong_back )
  {
    // if there is data ready
    if ( radio.available() )
    {
      // Dump the payloads until we've gotten everything
      unsigned long data;
      bool done = false;
      while (!done)
      {
        // Fetch the payload, and see if this was the last one.
        done = radio.read( &data, sizeof(unsigned long) );
        // Spew it
        int x = data & 0xf;
        int y = (data & 0xf0) >> 4;
        int n = (data & (1<<8)) >> 8;
        int e = (data & (1<<9)) >> 9;
        int s = (data & (1<<10)) >> 10;
        int w = (data & (1<<11)) >> 11;
        int color = (data & (1<<12)) >> 12;
        int shape = (data & (3<<13)) >> 13;
        int explored = (data & (1<<15)) >> 15;
        printf("%d",x);
        printf(",%d", y);
        if(n==0) Serial.print(",north=false");
        if(e==0) Serial.print(",east=false");
        if(s==0) Serial.print(",south=false");
        if(w==0) Serial.print(",west=false");
        if (n) Serial.print(",north=true");
        if (e) Serial.print(",east=true"); 
        if (s) Serial.print(",south=true"); 
        if (w) Serial.print(",west=true");
        
        if (shape != 0){
          if (shape == 2){
            Serial.print(",tshape=triangle");
          } else if (shape == 3){
            Serial.print(",tshape=square");
          } else if(shape == 1) {
            Serial.print(",tshape=circle");
          }
          if (color) Serial.print(",tcolor=blue");
          else Serial.print(",tcolor=red");
        }
        Serial.print("\n");
        //Serial.println("1,1,north=true");
        //printf("x = %d, y = %d, north = %d, east = %d, south = %d, west = %d, color = %d, shape = %d, explored = %d\n",x,y,n,e,s,w,color,shape,explored);
        //printf("x = 0, y = 0, north = true, east = false, south = false, west = false, color = red, shape = triangle, explored = true\n");


        // Delay just a little bit to let the other unit
        // make the transition to receiver
        delay(20);

      }

      // First, stop listening so we can talk
      radio.stopListening();

      // Send the final one back.
      radio.write( &data, sizeof(unsigned long) );
      //printf("Sent response.\n\r");

      // Now, resume listening so we catch the next packets.
      radio.startListening();
    }
  }

  //
  // Change roles
  //

//  if ( Serial.available() )
//  {
//    char c = toupper(Serial.read());
//    if ( c == 'T' && role == role_pong_back )
//    {
//      printf("*** CHANGING TO TRANSMIT ROLE -- PRESS 'R' TO SWITCH BACK\n\r");
//
//      // Become the primary transmitter (ping out)
//      role = role_ping_out;
//      radio.openWritingPipe(pipes[0]);
//      radio.openReadingPipe(1,pipes[1]);
//    }
//    else if ( c == 'R' && role == role_ping_out )
//    {
//      printf("*** CHANGING TO RECEIVE ROLE -- PRESS 'T' TO SWITCH BACK\n\r");
//
//      // Become the primary receiver (pong back)
//      role = role_pong_back;
//      radio.openWritingPipe(pipes[1]);
//      radio.openReadingPipe(1,pipes[0]);
//    }
//  }
}
// vim:cin:ai:sts=2 sw=2 ft=cpp
