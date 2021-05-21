from flask import Flask,request, jsonify
import firebase_admin
from firebase_admin import credentials,firestore
import os
import datetime

cwd=os.getcwd()
complete_path=os.path.join(cwd,"activity-tracker-cube-firebase-adminsdk-ugbiu-85cfbc3bbe.json")
print(complete_path)
cred = credentials.Certificate(complete_path)
firebase_admin.initialize_app(cred)
db=firestore.client()

activities=["work","study","exercise","read"]


app=Flask(__name__)

@app.route("/")
def test():
    return 'Activity tracker API'
# TODO: endpoint for receiving data in the format : {"user":"Mathew","activity":"exercise","starttime":10:30,"endtime":12:20,"duration":110}
# arrange data as username->data->...
@app.route("/post_data",methods =["POST"])
def data_ingestion():
    request_data=request.get_json()
    activity_data=jsonify(request_data).get_json()
    activity_data.pop("user")
    db.collection("activity_data").document(request_data["user"]).collection("data").add(activity_data)
    return "Saved to firestore"
    

# TODO: endpoint for sending activities in the format : {
# {"activities":["swimming","sleeping","dancing","learning"]}
@app.route("/get_activities",methods=["GET"])
def get_activities_helper():
    return "Enter name in URL to get activities"


@app.route("/get_activities/<string:person_name>",methods =["GET"])
def get_activities(person_name):
    activity_list = []
    print(person_name)
    received_activities = db.collection("activity_data").document(person_name).get().to_dict()
    print(received_activities)
    return received_activities


# TODO: endpoint for sending activities in the format : {"user":"Mathew","activity":["swimming","sleeping","dancing","learning"]}
# arrange data as username -> activities -> ...
@app.route("/post_activities")
def set_activities():
    request_data=request.get_json()
    activity_list=jsonify(request_data).get_json()
    activity_list.pop("user")
    db.collection("activity_data").document(request_data["user"]).set(activity_list)
    return "Activities Saved to firestore"

@app.route("/get_activities/<string:person_name>/aggregate_time",methods =["GET"])
def get_aggregare_time(person_name):
    activity_set = set(activities)
    activity_duration= dict()
    received_activities = db.collection("activity_data").document("Mathew").collection("data").get()

    # for activity in received_activities:
    #     activity_dict=activity.to_dict()
    #     activity_set.add(activity_dict['activity'])
    for distict_activity in activity_set:
        activity_duration[distict_activity]=0
    for activity in received_activities:
        activity_dict=activity.to_dict()
        for distict_activity in activity_set:
            if activity_dict["activity"]==distict_activity:
                activity_duration[distict_activity]=activity_duration[distict_activity]+activity_dict["duration"]
    return(activity_duration)

@app.route("/get_activities/<string:person_name>/aggregate_time_today",methods =["GET"])
def get_aggregate_time_today(person_name):
    activity_set = set(activities)
    activity_duration= dict()
    received_activities = db.collection("activity_data").document("Mathew").collection("data").get()
    
    date=str(datetime.datetime.now())[:10]
    
    # for activity in received_activities:
    #     activity_dict=activity.to_dict()
    #     activity_set.add(activity_dict['activity'])
    date=str(datetime.datetime.now())[:10]
    for distict_activity in activity_set:
        activity_duration[distict_activity]=0
    for activity in received_activities:
        activity_dict=activity.to_dict()
        for distict_activity in activity_set:
            if (activity_dict["activity"]==distict_activity) and (activity_dict.get("startdate")==date):
                activity_duration[distict_activity]=activity_duration[distict_activity]+activity_dict["duration"]
    
    return(activity_duration)

@app.route("/Date")
def get_date():
    date=str(datetime.datetime.now())[:10]
    return {"date":date}
    
# app.run(port=5000,  debug=True)