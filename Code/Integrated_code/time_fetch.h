#include <NTPClient.h>
#include <WiFiUdp.h>
// #include <ESP8266WiFiMulti.h>

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");


struct start_time_struct{
  /*
  Structure to store the starting time of the activity
  */
  unsigned long start_stopwatch;
  int currentHour;
  int currentMinute;
  int monthDay;
  int currentMonth;
  int currentYear;
  String formattedTime;
  String currentDate;
};
start_time_struct start_time;

struct end_time_struct{
  /*
  Structure to store the ending time of the activity
  */
  unsigned long end_stopwatch;
  int currentHour;
  int currentMinute;
  int monthDay;
  int currentMonth;
  int currentYear;
  String formattedTime;
  String currentDate;
};
end_time_struct end_time;
unsigned long duration=0;

void initialize_time(){
  /*
  Initialising the time client to account for time zone differences
  */
  // TODO: Put the correct time Zone
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  timeClient.setTimeOffset(0);
}

void get_duration(){
  /*
  Getting the time for one activity
  */
  duration=end_time.end_stopwatch-start_time.start_stopwatch;
}

void get_start_time(){
  /*
  Function for getting the starting time of the activity
  */  
unsigned long epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime ((time_t *)&epochTime); 
timeClient.update();
  start_time.start_stopwatch=millis()/1000;
  start_time.currentHour = timeClient.getHours(); 
  start_time.currentMinute = timeClient.getMinutes();  
  String formattedTime = timeClient.getFormattedTime();
  start_time.monthDay = ptm->tm_mday;
  start_time.currentMonth = ptm->tm_mon+1;
  start_time.currentYear = ptm->tm_year+1900;
  start_time.formattedTime=timeClient.getFormattedTime();
  start_time.currentDate=String(start_time.currentYear) + "-" + String(start_time.currentMonth) + "-" + String(start_time.monthDay);
}

void get_end_time(){
  /*
  Function for getting the ending time of the activity
  The duration variable is automatically updated Upon calling this function
  */ 
  unsigned long epochTime = timeClient.getEpochTime();  
  struct tm *ptm = gmtime ((time_t *)&epochTime); 
  timeClient.update();
  end_time.end_stopwatch=millis()/1000;
  get_duration();
  end_time.currentHour = timeClient.getHours(); 
  end_time.currentMinute = timeClient.getMinutes();  
  end_time.monthDay = ptm->tm_mday;
  end_time.currentMonth = ptm->tm_mon+1;
  end_time.currentYear = ptm->tm_year+1900;
  end_time.formattedTime=timeClient.getFormattedTime();
  end_time.currentDate=String(start_time.currentYear) + "-" + String(start_time.currentMonth) + "-" + String(start_time.monthDay);

  get_duration();
}

