from flask import Flask,request, jsonify
import firebase_admin
from firebase_admin import credentials,firestore

cred = credentials.Certificate("E:/Work/activity_tracker_cube/Code/API/activity-tracker-cube-firebase-adminsdk-ugbiu-85cfbc3bbe.json")
firebase_admin.initialize_app(cred)
db=firestore.client()




app=Flask(__name__)

@app.route("/",methods =['POST'])

def data_ingestion():
    request_data=request.get_json()
    
    return 'Hi'
    


# # TODO: endpoint for receiving data in the format : {'user':'Mathew','activity':'exercise','starttime':10:30,'endtime':12:20,'duration':110}
# # arrange data as username->data->...
# @app.route('/data_ingestion')
# def data_ingestion:
#     data=request.json()
    

# # TODO: endpoint for sending activities in the format : {'user':'Mathew','activity':['swimming','sleeping','dancing','learning']}
# # arrange data as username -> activities -> ...
# @app.route('/broadcast_activities')


# # TODO: endpoint for sending activities in the format : {'user':'Mathew','activity':['swimming','sleeping','dancing','learning']}
# # arrange data as username -> activities -> ...
# @app.route('/get_activities')

app.run(port=5000,  debug=True)