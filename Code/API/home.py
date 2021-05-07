from flask import Flask,request, jsonify
import firebase_admin
from firebase_admin import credentials,firestore
import os
cwd=os.getcwd()
complete_path=os.path.join(cwd,"activity-tracker-cube-firebase-adminsdk-ugbiu-85cfbc3bbe.json")
print(complete_path)
cred = credentials.Certificate(complete_path)
firebase_admin.initialize_app(cred)
db=firestore.client()

activities=['playing','sleeping','studying','working']


app=Flask(__name__)

@app.route("/",methods =["POST"])

def data_ingestion():
    request_data=request.get_json()
    activity_data=jsonify(request_data).get_json()
    activity_data.pop("user")
    db.collection("activity_data").document(request_data["user"]).collection("data").add(activity_data)
    return "Saved to firestore"
    


# TODO: endpoint for receiving data in the format : {"user":"Mathew","activity":"exercise","starttime":10:30,"endtime":12:20,"duration":110}
# arrange data as username->data->...
@app.route("/data_ingestion",methods =["POST"])
def data_ingestion():
    request_data=request.get_json()
    activity_data=jsonify(request_data).get_json()
    activity_data.pop("user")
    db.collection("activity_data").document(request_data["user"]).collection("data").add(activity_data)
    return "Saved to firestore"
    

# TODO: endpoint for sending activities in the format : {
"user":"Mathew","activity":["swimming","sleeping","dancing","learning"]}
# arrange data as username -> activities -> ...
@app.route("/broadcast_activities")
def get_activities:
    return {'activities':activities}


# TODO: endpoint for sending activities in the format : {"user":"Mathew","activity":["swimming","sleeping","dancing","learning"]}
# arrange data as username -> activities -> ...
@app.route("/get_activities")
def set_activities():
    request_data=request.get_json()
    activity_list=jsonify(request_data).get_json()
    activity_data.pop("user")
    db.collection("activity_data").document(request_data["user"]).document("activity_list").add(activity_data)
    return "Activities Saved to firestore"


app.run(port=5000,  debug=True)