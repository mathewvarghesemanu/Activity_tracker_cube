#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <math.h>
#include "gyro.h"
#include "time_fetch.h"
#include "constants.h"
ESP8266WiFiMulti WiFiMulti;
const int buzzer_pin=D5;

void my_strcpy2(char dest[], const char* src)
{
  int i=0;
    while ('\0' != *src){
        dest[i] = *src++;
        i=i+1;
    }
    dest[i] = '\0';
    
}

struct user_activities_struct {
  /*
  Structure to store user activities Received from the get request
  */
  const char* activity_0="work";
  const char* activity_1="study";
  const char* activity_2="exercise";
  const char* activity_3="read";
};
user_activities_struct user_activities;

struct current_struct{
  /*
  Structure to store the current state of the activity tracker
  */
  int cube_current_state=0;
  char current_activity[20];  
  bool no_activity;
};
current_struct current;


 
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

          
          // user_activities.activity_0 = activities[0]; // "swimming"
          // user_activities.activity_1 = activities[1]; // "sleeping"
          // user_activities.activity_2 = activities[2]; // "dancing"
          // user_activities.activity_3 = activities[3]; // "learning"
          // Serial.println(user_activities.activity_0);
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
  else{
    Serial.println("Not Connected");
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
    doc["user"] = user;
    doc["activity"] = current.current_activity;
    doc["starttime"] = start_time.formattedTime;
    doc["startdate"] = start_time.currentDate;
    doc["endtime"] = end_time.formattedTime;
    //TODO: Include end date
    doc["duration"] = duration; 

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
    my_strcpy2( current.current_activity,"None");
    break;
    case 1:
    my_strcpy2( current.current_activity,user_activities.activity_0);
    // current.current_activity=user_activities.activity_0;
    current.no_activity=false;
    break;    
    case 2:
    my_strcpy2( current.current_activity,user_activities.activity_1);

    // current.current_activity=user_activities.activity_1;
    current.no_activity=false;
    break;
    case 3:
    my_strcpy2( current.current_activity,user_activities.activity_2);

    // current.current_activity=user_activities.activity_2;
    current.no_activity=false;
    break;
    case 4:
    my_strcpy2( current.current_activity,user_activities.activity_3);

    // current.current_activity=user_activities.activity_3;
    current.no_activity=false;
    break;
  }
}


void get_activity_transition(int state){
  /*
  Function to get the transition from one activity to another and trigger required functions
  */
  
  if (current.cube_current_state!=state){
      tone(buzzer_pin, 1500, 300); 
      get_end_time();
      // TODO: Send Previous activity, start time, endtime, duration as a post request
      Serial.print("Transition: ");
      Serial.println(current.cube_current_state);

      Serial.println(current.current_activity);
      post_data();      
      current.cube_current_state=state;
      get_start_time();
      
    }
}

void find_activity(){
  /*
  Function to find activity based on the gyro readings corresponding to the face of the cube
  This function also take care of the transition between activities
  */
  if (gyro_readings.roll>-40 && gyro_readings.roll<40 && gyro_readings.pitch>-40 && gyro_readings.pitch<40){
    get_activity_transition(0);
  }
  else if (gyro_readings.roll>-40 && gyro_readings.roll<40 && gyro_readings.pitch<-40){
    get_activity_transition(1);
  }
  else if (gyro_readings.roll>40 && gyro_readings.pitch>-40 && gyro_readings.pitch<40){
   get_activity_transition(2);
  }
  else if (gyro_readings.roll>-40 && gyro_readings.roll<40 && gyro_readings.pitch>40) {
    get_activity_transition(3);
  }
  else if (gyro_readings.roll<-40 && gyro_readings.pitch>-40 && gyro_readings.pitch<40){
    get_activity_transition(4);
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
  initialize_time();
  get_start_time();
  delay(3000);
  get_activity_list();
  pinMode(buzzer_pin,OUTPUT);
  digitalWrite(buzzer_pin, LOW);
}




void loop() {
  /*
  This is the main function
  */
  // get_activity_list();
  // get_activity_list();

// if (user_activities.activity_0!=""){
  read_gyro();
  // Serial.print(gyro_readings.roll);
  // Serial.print("/");
  // Serial.println(gyro_readings.pitch);
  find_activity();
  // Serial.println(current.cube_current_state);

    
  // Serial.println(duration);
  // delay(2000);
// }  
Serial.println(current.current_activity);
Serial.println(gyro_readings.pitch);

delay(1000);  
}