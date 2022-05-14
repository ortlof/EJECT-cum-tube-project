#include <Arduino.h>          // Basic Needs

// Include the Stepper library:
#include <ESP_FlexyStepper.h>
#include <esp_now.h>
#include <WiFi.h>

// IO pin assignments
const int MOTOR_STEP_PIN = 4;
const int MOTOR_DIRECTION_PIN = 5;
const int MOTOR_ENA_PIN = 6;

uint8_t M5_Remote_Address[] = {0x08, 0x3A, 0xF2, 0x68, 0x1E, 0x74};

// create the stepper motor object
ESP_FlexyStepper stepper;

// Command States

#define CUMSPEED  20
#define CUMTIME   21
#define CUMSIZE   22
#define CUMACCEL  23


unsigned long previousMillis = 0;
long interval = 1000;

///////////////////////////////////////////
////
////  To Debug or not to Debug
////
///////////////////////////////////////////

// Uncomment the following line if you wish to print DEBUG info
#define DEBUG 

#ifdef DEBUG
#define LogDebug(...) Serial.println(__VA_ARGS__)
#define LogDebugFormatted(...) Serial.printf(__VA_ARGS__)
#else
#define LogDebug(...) ((void)0)
#define LogDebugFormatted(...) ((void)0)
#endif

float cum_time = 0.0;
float cum_speed = 1000.0;
float cum_size = 0.0;
float cum_accel = 0.0;

// Variable to store if sending data was successful
String success;

int out_esp_command;
float out_esp_value;

int incoming_esp_command;
float incoming_esp_value;

typedef struct struct_message {
  float esp_speed;
  float esp_depth;
  float esp_stroke;
  float esp_sensation;
  float esp_pattern;
  bool esp_rstate;
  bool esp_connected;
  int esp_command;
  float esp_value;
} struct_message;

bool esp_connect = false;

struct_message outgoingcontrol;
struct_message incomingcontrol;

esp_now_peer_info_t peerInfo;

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status ==0){
    success = "Delivery Success :)";
  }
  else{
    success = "Delivery Fail :(";
  }
}

// Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&incomingcontrol, incomingData, sizeof(incomingcontrol));

  LogDebug(incomingcontrol.esp_command);
  LogDebug(incomingcontrol.esp_value);

  switch(incomingcontrol.esp_command)
  {
    case CUMSPEED:
    {
     cum_speed = incomingcontrol.esp_value;
    }
    break;
    case CUMTIME:
    {
     interval = incomingcontrol.esp_value * 1000;
    }
    break;
    case CUMSIZE:
    {
     cum_size = incomingcontrol.esp_value * 1500;
    }
    break;
    case CUMACCEL:
    {
    cum_accel = incomingcontrol.esp_value * 10000;
    }
    break;
  }
}


void setup()
{  
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    LogDebug(WiFi.macAddress());
    
    // Init ESP-NOW
    if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
    }
    // Once ESPNow is successfully Init, we will register for Send CB to
    // get the status of Trasnmitted packet
    esp_now_register_send_cb(OnDataSent);

    // Register peer
    memcpy(peerInfo.peer_addr, M5_Remote_Address, 6);
    peerInfo.channel = 0;  
    peerInfo.encrypt = false;
  
      // Add peer        
    if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
    }
    // Register for a callback function that will be called when data is received
    esp_now_register_recv_cb(OnDataRecv);


    // connect and configure the stepper motor to its IO pins
    pinMode(MOTOR_ENA_PIN, OUTPUT);
    digitalWrite(MOTOR_ENA_PIN, LOW);
    stepper.connectToPins(MOTOR_STEP_PIN, MOTOR_DIRECTION_PIN);
}

void loop()
{
  stepper.setSpeedInStepsPerSecond(cum_speed);
  stepper.setAccelerationInStepsPerSecondPerSecond(cum_accel);
  stepper.setDecelerationInStepsPerSecondPerSecond(cum_accel);

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    digitalWrite(MOTOR_ENA_PIN, LOW);
    previousMillis = currentMillis;
    stepper.moveRelativeInSteps(cum_size);
  }
  if (stepper.getDistanceToTargetSigned() == 0){
     digitalWrite(MOTOR_ENA_PIN, HIGH);
  }
  
}