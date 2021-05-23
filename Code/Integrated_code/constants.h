

const int buzzer_pin = D5;



char get_activity_api_url[] = "http://activitytracker.mak3r.space/get_activities/Mathew";
char post_data_api_url[]="http://activitytracker.mak3r.space/post_data";
char get_date_api_url[]="http://activitytracker.mak3r.space/Date";

// char authkey[] = "401D82B1BD4B47D6817DBD5C8A6AD297";// find authkey in octoprint application keys in settings

char ssid[] = "NETGEAR87";//your wifi
char password[] = "quaintsocks941"; //your password


char user[]="Mathew";

struct user_activities_struct {
  /*
    Structure to store user activities Received from the get request
  */
  String activity_0 = "work";
  String activity_1 = "study";
  String activity_2 = "exercise";
  String activity_3 = "read";
};
user_activities_struct user_activities;

struct current_struct {
  /*
    Structure to store the current state of the activity tracker
  */
  int cube_current_state = 0;
  String current_activity;
  bool no_activity;
};
current_struct current;
