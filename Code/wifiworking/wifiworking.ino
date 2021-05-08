#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>

ESP8266WiFiMulti WiFiMulti;
char user[]="Mathew";
char get_activity_api_url[] = "http://activitytracker.mak3r.space/get_activities/Mathew";
char post_data_api_url[]="http://activitytracker.mak3r.space/post_data";
// char authkey[] = "401D82B1BD4B47D6817DBD5C8A6AD297";// find authkey in octoprint application keys in settings

  char ssid[] = "man";//your wifi
  char password[] = "dodododo"; //your password
 
void setupWifi(){
  
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
        // Serial.println(payload);    
        DeserializationError error = deserializeJson(doc, payload);

        if (error) {
          Serial.print(F("deserializeJson() failed: "));
          Serial.println(error.f_str());
          return;
        }

        JsonArray activities = doc["activities"];
        const char* activities_0 = activities[0]; // "swimming"
        const char* activities_1 = activities[1]; // "sleeping"
        const char* activities_2 = activities[2]; // "dancing"
        const char* activities_3 = activities[3]; // "learning"


      http.end();
      Serial.println(activities_0);
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
  String json;
  // StaticJsonDocument<128> doc;
  DynamicJsonDocument doc(2048);
  doc["user"] = "Mathew";
  doc["activity"] = "exercise";
  doc["starttime"] = "10:30";
  doc["endtime"] = "12:20";
  doc["duration"] = "110";

  serializeJson(doc, json);
  // Serial.println(json);

  if ((WiFiMulti.run() == WL_CONNECTED)) {
    WiFiClient client;
    HTTPClient http;


    Serial.print("[HTTP] begin...\n");
    // http.begin(client, get_activity_api_url);
    http.addHeader("Content-Type", "application/json");
    Serial.println(json);
    // json="{\"user\":\"Mathew\",\"activity\":\"exercise\",\"starttime\":\"10:30\",\"endtime\":\"12:20\",\"duration\":\"110\"}";
    // Serial.println(json);
    
    http.begin(post_data_api_url);
    int httpResponseCode =http.POST(json);
    if(httpResponseCode>0){

      //  TODO: Rectify the error in the post request
      // String response = http.getString();                       
       
      Serial.println(httpResponseCode);   
      // Serial.println(response);
     
    }
    else {
     
      Serial.printf("Error occurred while sending HTTP POST: %s\n");
       
    }
    http.end();
  }
  delay(1000);
  
}

void setup() {
  /*
  Initial setup
  */
  Serial.begin(115200);
  setupWifi(); 
}

void loop() {
  /*
  This is the main function
  */
  post_data();
}