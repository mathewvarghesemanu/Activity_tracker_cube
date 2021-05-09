#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <math.h>
#include "gyro.h"

ESP8266WiFiMulti WiFiMulti;

char user[]="Mathew";

struct user_activities_struct {
  /*
  Structure to store user activities Received from the get request
  */
  const char* activity_0;
  const char* activity_1;
  const char* activity_2;
  const char* activity_3;
};
user_activities_struct user_activities;

struct current_struct{
  /*
  Structure to store the current state of the activity tracker
  */
  int cube_current_state;
  const char* current_activity;  
  bool no_activity;
};
current_struct current;

char get_activity_api_url[] = "http://activitytracker.mak3r.space/get_activities/Mathew";
char post_data_api_url[]="http://activitytracker.mak3r.space/post_data";
// char authkey[] = "401D82B1BD4B47D6817DBD5C8A6AD297";// find authkey in octoprint application keys in settings

  char ssid[] = "NETGEAR87";//your wifi
  char password[] = "quaintsocks941"; //your password
 
void setupWifi(){
  /*
  Function to initially set up Wi-Fi for connecting to Internet. It should be included in Setup function
  */
  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(ssid, password);
  Serial.printf("Wifi configured");
}

void get_activity_list(){
  /*
  This is used to get the list of activities that user will configure using the mobile app. The user can choose 4 activities at once. 
  Corresponding to the four activities the user can choose, we have four modes depending upon the four sides of the cube.
  */
  if ((WiFiMulti.run() == WL_CONNECTED)) {
    WiFiClient client;
    HTTPClient http;
    StaticJsonDocument<192> doc;

    // Serial.print("[HTTP] begin...\n");
    http.begin(client, get_activity_api_url);
    http.addHeader("Content-Type", "application/json");
    // Serial.print("[HTTP] GET...\n");
    int httpCode = http.GET();
    if (httpCode > 0) {
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.println(payload);    
        DeserializationError error = deserializeJson(doc, payload);

        if (error) {
          Serial.print(F("deserializeJson() failed: "));
          Serial.println(error.f_str());
          return;
        }
        else{
          JsonArray activities = doc["activities"];
          user_activities.activity_0 = activities[0]; // "swimming"
          user_activities.activity_1 = activities[1]; // "sleeping"
          user_activities.activity_2 = activities[2]; // "dancing"
          user_activities.activity_3 = activities[3]; // "learning"
          Serial.println(user_activities.activity_0);
          // strcpy(user_activities.activity_0,activities[0]);
          // user_activities.activity_1=activities[1];
          // user_activities.activity_2=activities[2];
          // user_activities.activity_3=activities[3];
          
        }
      http.end();
      
      }   

    delay(500);
    }
  }
}


void post_data(){
  /*
  Past data function is used to send the data of the ongoing activity to the API. 
  The user is recognised by the first field of the data.
  */
  if (current.no_activity==false){
    String json;
    DynamicJsonDocument doc(2048);
    //TODO: assign The respective values to the variables
    //TODO: Figure out a way to send only when there is a state change
    doc["user"] = "Mathew";
    doc["activity"] = current.current_activity;
    doc["starttime"] = "10:30";
    doc["endtime"] = "12:20";
    doc["duration"] = "110";

    serializeJson(doc, json);
    
    // // Serial.println(json);

    if ((WiFiMulti.run() == WL_CONNECTED)) {
      
    
      Serial.println(json);
      // json="{\"user\":\"Mathew\",\"activity\":\"exercise\",\"starttime\":\"10:30\",\"endtime\":\"12:20\",\"duration\":\"110\"}";
      // Serial.println(json);
      WiFiClient client;
      HTTPClient http;
      Serial.print("[HTTP] begin...\n");
      http.begin(post_data_api_url);
      http.addHeader("Content-Type", "application/json");
      int httpResponseCode =http.POST(json);
      if(httpResponseCode>0){
        Serial.println(httpResponseCode);   
      }
      else {
        Serial.printf("Error occurred while sending HTTP POST: %s\n");
      }
      http.end();
    }
  }
  delay(1000);
}


void map_activity(){
  /*
  Function to map the current state of the cube to the corresponding activities receeived from the get request
  */
  switch(current.cube_current_state){
    case 0:
    current.no_activity=true;    
    break;
    case 1:
    current.current_activity=user_activities.activity_0;
    current.no_activity=false;
    break;    
    case 2:
    current.current_activity=user_activities.activity_1;
    current.no_activity=false;
    break;
    case 3:
    current.current_activity=user_activities.activity_2;
    current.no_activity=false;
    break;
    case 4:
    current.current_activity=user_activities.activity_3;
    current.no_activity=false;
    break;
  }
}



void find_activity(){
  /*
  Function to find activity based on the gyro readings corresponding to the face of the cube
  */
  if (gyro_readings.roll>-40 && gyro_readings.roll<40 && gyro_readings.pitch>-40 && gyro_readings.pitch<40){
    current.cube_current_state=0;
  }
  else if (gyro_readings.roll>-40 && gyro_readings.roll<40 && gyro_readings.pitch<-40){
    current.cube_current_state=1;
  }
  else if (gyro_readings.roll>40 && gyro_readings.pitch>-40 && gyro_readings.pitch<40){
    current.cube_current_state=2;
  }
  else if (gyro_readings.roll>-40 && gyro_readings.roll<40 && gyro_readings.pitch>40) {
    current.cube_current_state=3;
  }
  else if (gyro_readings.roll<-40 && gyro_readings.pitch>-40 && gyro_readings.pitch<40){
    current.cube_current_state=4;
  }
  map_activity();
}
void setup() {
  /*
  Initial setup
  */
  Serial.begin(115200);
  setupWifi(); 
  initialize_gyro();
}

void loop() {
  /*
  This is the main function
  */
  // get_activity_list();
  // Serial.print(no);
  read_gyro();
  Serial.print(gyro_readings.roll);
  Serial.print("/");
  Serial.println(gyro_readings.pitch);
  find_activity();
  Serial.println(current.cube_current_state);
}