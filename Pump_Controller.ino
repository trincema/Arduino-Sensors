#include <VirtualWire.h>

unsigned char DEVICE_ID = 1;
unsigned char FLOW_SENSOR = PD2; // Flow Sensor Input
unsigned char PUMP_SENSOR = PD1; // Water Pump Sensor Input
unsigned int PULSES_PER_LITER = 450;
volatile int flow_frequency = 0;    // Measures flow sensor pulses
volatile int pulses = 0;            // Measures flow sensor pulses
float vol = 0.0;
float l_min;
unsigned long currentTime;
unsigned long cloopTime;
bool water_running = true;

byte message[VW_MAX_MESSAGE_LEN]; // a buffer to store the incoming messages
byte messageLength = VW_MAX_MESSAGE_LEN; // the size of the message

void setup() {
  Serial.begin(9600);
  currentTime = millis();
  cloopTime = currentTime;
  pinMode(PUMP_SENSOR, OUTPUT);
  digitalWrite(PUMP_SENSOR, LOW);
  // We declare the Arduino pin to which the signal pin of the flow sensor is connected,
  // as an input pin. We create a pull-up on the pin by setting it “HIGH” and set up a “Rising” edge
  // interrupt on it with the flow() function we created earlier as the callback.
  pinMode(FLOW_SENSOR, INPUT);
  digitalWrite(FLOW_SENSOR, HIGH); // Optional Internal Pull-Up
  attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR), flow, RISING); // Setup Interrupt

  vw_set_rx_pin(PB4); // pin
  vw_setup(2000); // bps
  vw_rx_start();
}

void loop() {
  if (water_running) {
    currentTime = millis();
    // Every 5 seconds, calculate and print litres/hour
    if(currentTime >= (cloopTime + 5000)) {
       cloopTime = currentTime; // Updates cloopTime
       l_min = (flow_frequency / 7.5); // (Pulse frequency x 60 min) / 7.5Q = flowrate in L/hour
       Serial.print("Rate: ");
       Serial.print(l_min);
       Serial.println(" L/M");
       vol = (float) pulses / PULSES_PER_LITER;
       Serial.print("Volume: ");
       Serial.print(vol);
       Serial.println(" L");
       flow_frequency = 0; // Reset Counter
    }
  }

  // Receive
  if (vw_get_message(message, &messageLength)) { // non-blocking
    for (int i = 0; i < messageLength; i++) {
      Serial.print(message[i]);
    }
    Serial.println();
  }
}

// Interrupt function
void flow () {
   flow_frequency++;
   pulses++;
}

void startPump() {
    digitalWrite(PUMP_SENSOR, HIGH);
}

void stopPump() {
    digitalWrite(PUMP_SENSOR, LOW);
}
