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

/**
 * Modifications made to receive maze data from our robot
 * Modified by: Gregory Kaiser (ghk48), Andrew Lin (yl656), David Burgstahler (dfb93)
 */

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

//
// Hardware configuration
//

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10

RF24 radio(9, 10);
//
// Topology
//

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = {0x0000000028LL, 0x0000000029LL};
//
// Role management
//
// Set up role.  This sketch uses the same software for all the nodes
// in this system.  Doing so greatly simplifies testing.
//

unsigned int data; //data to be sent by radio

// The various roles supported by this sketch
typedef enum
{
  role_ping_out = 1,
  role_pong_back
} role_e;

// The debug-friendly names of those roles
const char *role_friendly_name[] = {"invalid", "Ping out", "Pong back"};

// The role of the current running sketch
role_e role = role_ping_out;

//random number to begin with

void radioSetup(void)
{
  //
  // Setup and configure rf radio
  //
  if (debug)
    Serial.println("Beginning Radio");
  radio.begin();

  // optionally, increase the delay between retries & # of retries
  radio.setRetries(15, 15);
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

  radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1, pipes[1]);

  //
  // Start listening
  //

  radio.startListening();
}

void sendMaze(int x, int y)
{
  // First, stop listening so we can talk.
  radio.stopListening();
  // Take the time, and send it.  This will block until complete
  //Serial.println(mazeData[x+rowLength*y]);
  data = (mazeData[x + rowLength * y] << 8) | (y << 4) | x; //construct data
  //bool ok = radio.write( mazeData+x+rowLength*y, sizeof(unsigned int) );
  bool ok = radio.write(&data, sizeof(unsigned int));
  if (ok)
    if (debug)
      Serial.println("ok...");
    else if (debug)
      Serial.print("failed.\n\r");

  // Now, continue listening
  radio.startListening();

  // Wait here until we get a response, or timeout (250ms)
  unsigned long started_waiting_at = millis();
  bool timeout = false;
  while (!radio.available() && !timeout)
    if (millis() - started_waiting_at > 100)
      timeout = true;

  // if timed out, resend
  if (timeout)
  {
    if (debug)
      Serial.print("Failed, response timed out.\n\r");
    //sendMaze();
  }
}
