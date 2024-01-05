/*
  ObserverOpto v3.0 by Ichiro Aoki
  v3.1 --> 3.2
  Ch_10 can be used.
    
  v3.0 --> 3.1
  Specifically designed for continuous illumination (of 590 nm LED) for Chrimson optogenetics.
  Delay command and unnecessary parameters were removed.
  Ch_10 is supposed not to be used.
  
  v2.0 --> 3.0
  Trigger of blue illumination was synchronized to each shutter opening.

  v2.0
  This is to control two LEDs (such as PriorLED) through TTL in response to a camera shutter.
  Connect "#2 Camera Exposing Output" of Evolve delta camera to port #2 of Arduino.
  Connect TTL ports of yellow and blue LED to port #12 and 13 of Arduino.
  Run this program while Arduino being connecte to PC and a Serial monitor on.
  Arduino gets ready for camera signal after a serial signal is sent.
  Illumination cycles start in response to the first exposure.
  Ch10 is on while the camera shutter is open.
  (The camera shutter and z-position are controlled from MicroManager in the PC.)
*/

const int TTLin = 2;
const int TTLout = 10;
const int TTLout2 = 11;
int mode = 0;
//Set 4 parameters below
const int num_Z = 2; //Number of Z planes
const int init_delay = num_Z * 30;    // Number of Ch10 pulses before Ch11 signal.
const int pulse_max = num_Z * 30;     // Number of additional Ch10 pulses after which Ch11 signal stops.
//const int pulse_dur = 10000;       // yellow pulse length in ms
//const int interval = 0; // Time between end of Ch10 pulse and start of Ch11 signal.
const int num_10 = num_Z * 90;  //Number of total Ch10 pulses
//const int delay_2 = 60;    // Number of blue pulses until second yellow illumination.

void setup()
{
  pinMode(TTLin, INPUT);
  pinMode(TTLout, OUTPUT);
  pinMode(TTLout2, OUTPUT);
  Serial.begin(57600);
}

void loop()
{
  Serial.println("Send serial signal when ready!");
  while (mode == 0) {
    mode = 1;
    digitalWrite(TTLout, LOW);
    digitalWrite(TTLout2, LOW);
    int pulse_started = 0;
    int pulse_ended = 0;
    int Ch_11 = 0;
    while (mode == 1) {
      if (Serial.read() > 0) {
        mode = 2;
        Serial.print(pulse_started);
        Serial.println("Ready");
        while (mode == 2) {
          if (digitalRead(2)) {
            mode = 3;
            Serial.println("Running. Send signal to abort.");
            while (mode == 3 && pulse_ended < num_10) {
              if (digitalRead(2) && (pulse_started == pulse_ended)) {
                digitalWrite(TTLout, HIGH);
                pulse_started++;
              }
              if (!digitalRead(2) && (pulse_started > pulse_ended)) {
                digitalWrite(TTLout, LOW);
                pulse_ended++;
              }
              if ((pulse_ended >= init_delay) && (Ch_11 == 0)) {
                digitalWrite(TTLout2, HIGH);
                Ch_11++;
              }
              if ((pulse_ended >= init_delay + pulse_max) && (Ch_11 == 1)) {
                digitalWrite(TTLout2, LOW);
                Ch_11++;
              }
              
//              else{
//                digitalWrite(TTLout2, LOW);
//              }
              if (Serial.read() > 0) {
                mode = 4;
                Serial.print(pulse_started);
                Serial.println("Aborted");
              }
            }
          }
          if (Serial.read() > 0) {
            mode = 4;
            Serial.print(pulse_started);
            Serial.println("Aborted");
          }
        }
      }
    }
  }
  mode = 0;
}
