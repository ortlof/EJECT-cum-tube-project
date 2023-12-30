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
#define CUMCONN     20
#define CUMTOGGLE   21
#define CUMTIME     22
#define CUMSIZE     23
#define CUMACCEL    24
#define CUMREVERSE  25

#define M5_ID 99
#define EJECT_ID 69

#define HEARTBEAT 99

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

float cum_time = 1.0;
float cum_size = 1.0;
float cum_accel = 1.0;
bool continuous = false;
bool ejecting = false;
bool reverse = false;

float steps_per_second = 800;

// max rpm = 500
// 6400 steps per revolution
float MAX_STEPS_PER_SECOND = 500 * 6400 / 60;

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
  int esp_target;
} struct_message;

bool esp_connect = false;

struct_message outgoingcontrol;
struct_message incomingcontrol;

esp_now_peer_info_t peerInfo;

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status == ESP_NOW_SEND_SUCCESS){
    success = "Delivery Success :)";
  }
  else{
    success = "Delivery Fail :(";
  }
}

// Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&incomingcontrol, incomingData, sizeof(incomingcontrol));

  switch(incomingcontrol.esp_command)
  {
    case CUMTOGGLE:
    {
      if (incomingcontrol.esp_value == 0) {
        stepper.setTargetPositionToStop();
        ejecting = false;
      } else {
        ejecting = true;
      }
      LogDebug("ejecting: " + String(ejecting));
    }
    break;
    case CUMTIME:
    {
      cum_time = incomingcontrol.esp_value;
      steps_per_second = min(MAX_STEPS_PER_SECOND, cum_size / cum_time);
      LogDebug("cum_time: " + String(cum_time));
    }
    break;
    case CUMSIZE:
    {
      cum_size = incomingcontrol.esp_value * 800; // 800 steps per 1/8 revolution
      if (cum_size / cum_time > MAX_STEPS_PER_SECOND) {
        cum_size = MAX_STEPS_PER_SECOND * cum_time;
      }
      steps_per_second = min(MAX_STEPS_PER_SECOND, cum_size / cum_time);
      LogDebug("cum_size: " + String(cum_size));
    }
    break;
    case CUMACCEL:
    {
      cum_accel = incomingcontrol.esp_value * 800;
      LogDebug("cum_accel: " + String(cum_accel));
    }
    break;
    case CUMREVERSE:
    {
      reverse = incomingcontrol.esp_value == 1;
      LogDebug("reverse: " + String(reverse));
    }
    break;
    case HEARTBEAT:
    {
      LogDebug("Heartbeat");
    }
    break;
  }
}

bool SendCommand(int Command, float Value){
  outgoingcontrol.esp_connected = true;
  outgoingcontrol.esp_command = Command;
  outgoingcontrol.esp_value = Value;
  outgoingcontrol.esp_target = M5_ID;
  esp_err_t result = esp_now_send(M5_Remote_Address, (uint8_t *) &outgoingcontrol, sizeof(outgoingcontrol));

  if (result == ESP_OK) {
    return true;
  } 
  else {
    delay(20);
    esp_err_t result = esp_now_send(M5_Remote_Address, (uint8_t *) &outgoingcontrol, sizeof(outgoingcontrol));
    return false;
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

  digitalWrite(MOTOR_ENA_PIN, LOW);
  stepper.connectToPins(MOTOR_STEP_PIN, MOTOR_DIRECTION_PIN);

  SendCommand(CUMCONN, EJECT_ID);
}

void loop()
{
  stepper.setSpeedInStepsPerSecond(steps_per_second);
  stepper.setAccelerationInStepsPerSecondPerSecond(cum_accel);
  stepper.setDecelerationInStepsPerSecondPerSecond(continuous ? 0 : cum_accel);

  if (cum_size > 0 && ejecting) {
    digitalWrite(MOTOR_ENA_PIN, LOW);
    stepper.moveRelativeInSteps(reverse ? -cum_size : cum_size);

    if (!continuous) {
      SendCommand(CUMTOGGLE, EJECT_ID);
      ejecting = false;
    }
  } else {
    digitalWrite(MOTOR_ENA_PIN, HIGH);
  }

  delay((continuous && ejecting) ? 100 : 500);
}
