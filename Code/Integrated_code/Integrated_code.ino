
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <math.h>
#include "gyro.h"
#include "time_fetch.h"
#include "constants.h"

ESP8266WiFiMulti WiFiMulti;




void setupWifi() {
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


String set_date() {
  //  char dates[20];
  String dates_string;
  if ((WiFiMulti.run() == WL_CONNECTED)) {
    WiFiClient client;
    HTTPClient http;

    // Serial.print("[HTTP] begin...\n");
    http.begin(client, get_date_api_url);
    http.addHeader("Content-Type", "application/json");
    // Serial.print("[HTTP] GET...\n");
    StaticJsonDocument<192> doc;

    int httpCode = http.GET();
    if (httpCode > 0) {
      Serial.printf("[HTTP] GET <date> code: %d\n", httpCode);
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
//        Serial.println(payload);
        DeserializationError error = deserializeJson(doc, payload);

        if (error) {
          Serial.print(F("deserializeJson() failed: "));
          Serial.println(error.f_str());
          return "error";
        }
        else {
          const char * date = doc["date"];
          //                      dates=doc["Date"].as<String>();
          //                    Serial.println(date);
          dates_string = String(date);
          //          my_strcpy2(dates, date);
          //          Serial.print(String(dates));
        }
        http.end();
        return dates_string;

      }

      delay(500);
    }
  }
  else {
    Serial.println("Not Connected");
    return String("no");
  }
}


void get_activity_list() {
  /*
    This is used to get the list of activities that user will configure using the mobile app. The user can choose 4 activities at once.
    Corresponding to the four activities the user can choose, we have four modes depending upon the four sides of the cube.
  */
  if ((WiFiMulti.run() == WL_CONNECTED)) {
    WiFiClient client;
    HTTPClient http;

    // Serial.print("[HTTP] begin...\n");
    http.begin(client, get_activity_api_url);
    http.addHeader("Content-Type", "application/json");
    // Serial.print("[HTTP] GET...\n");
    StaticJsonDocument<192> doc;

    int httpCode = http.GET();
    if (httpCode > 0) {
      Serial.printf("[HTTP] GET... <activity> code: %d\n", httpCode);
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.println(payload);
        DeserializationError error = deserializeJson(doc, payload);

        if (error) {
          Serial.print(F("deserializeJson() failed: "));
          Serial.println(error.f_str());
          return;
        }
        else {
          JsonArray activities = doc["activities"];


          user_activities.activity_0 = String(activities[0]); // "swimming"
          user_activities.activity_1 = String(activities[1]); // "sleeping"
          user_activities.activity_2 = String(activities[2]); // "dancing"
          user_activities.activity_3 = String(activities[3]); // "learning"
        }
        http.end();
      }
      delay(500);
    }
  }
  else {
    Serial.println("Not Connected");
  }
}


void post_data() {
  /*
    Past data function is used to send the data of the ongoing activity to the API.
    The user is recognised by the first field of the data.
  */
  if (current.no_activity == false) {
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


      //      Serial.println(json);
      // json="{\"user\":\"Mathew\",\"activity\":\"exercise\",\"starttime\":\"10:30\",\"endtime\":\"12:20\",\"duration\":\"110\"}";
      // Serial.println(json);
      WiFiClient client;
      HTTPClient http;
      
      http.begin(client, post_data_api_url);
      http.addHeader("Content-Type", "application/json");
      int httpResponseCode = http.POST(json);
      if (httpResponseCode > 0) {
        Serial.print("[HTTP] <post activity> code: ");
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


void get_activity_transition(int state) {
  /*
    Function to get the transition from one activity to another and trigger required functions
  */

  if (current.cube_current_state != state) {
    tone(buzzer_pin, 1500, 300);
    get_end_time();
    // TODO: Send Previous activity, start time, endtime, duration as a post request
    Serial.println("***********************************");
    Serial.print("Transition :");
    Serial.print(current.cube_current_state);
    Serial.print("-->");
    Serial.println(state);
    Serial.println("***********************************");

    post_data();
    current.cube_current_state = state;
    get_start_time();

  }
}

void map_activity() {
  /*
    Function to map the current state of the cube to the corresponding activities receeived from the get request
  */
  switch (current.cube_current_state) {
    case 0:
      current.no_activity = true;
      //      my_strcpy2( current.current_activity, "None");
      current.current_activity = "None";
      break;
    case 1:
      //      my_strcpy2( current.current_activity, user_activities.activity_0);
      current.current_activity = user_activities.activity_0;
      current.no_activity = false;
      break;
    case 2:
      //      my_strcpy2( current.current_activity, user_activities.activity_1);

      current.current_activity = user_activities.activity_1;
      current.no_activity = false;
      break;
    case 3:
      //      my_strcpy2( current.current_activity, user_activities.activity_2);

      current.current_activity = user_activities.activity_2;
      current.no_activity = false;
      break;
    case 4:
      //      my_strcpy2( current.current_activity, user_activities.activity_3);

      current.current_activity = user_activities.activity_3;
      current.no_activity = false;
      break;
  }
}


void find_activity() {
  /*
    Function to find activity based on the gyro readings corresponding to the face of the cube
    This function also take care of the transition between activities
  */
  if (gyro_readings.roll > -40 && gyro_readings.roll < 40 && gyro_readings.pitch > -40 && gyro_readings.pitch < 40) {
    get_activity_transition(0);
  }
  else if (gyro_readings.roll > -40 && gyro_readings.roll < 40 && gyro_readings.pitch < -40) {
    get_activity_transition(1);
  }
  else if (gyro_readings.roll > 40 && gyro_readings.pitch > -40 && gyro_readings.pitch < 40) {
    get_activity_transition(2);
  }
  else if (gyro_readings.roll > -40 && gyro_readings.roll < 40 && gyro_readings.pitch > 40) {
    get_activity_transition(3);
  }
  else if (gyro_readings.roll < -40 && gyro_readings.pitch > -40 && gyro_readings.pitch < 40) {
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
  delay(3000);
  get_start_time();
  get_activity_list();
  pinMode(buzzer_pin, OUTPUT);
  digitalWrite(buzzer_pin, LOW);
}




void loop() {
  /*
    This is the main function
  */
  read_gyro();
  // Serial.print(gyro_readings.roll);
  // Serial.print("/");
  // Serial.println(gyro_readings.pitch);
  find_activity();
  // Serial.println(current.cube_current_state);


  // Serial.println(duration);
  // delay(2000);
  // }
  //  get_start_time();
  // Serial.println(current.current_activity);
  // Serial.println(gyro_readings.pitch);
  delay(1000);
  Serial.println(current.current_activity);
}
