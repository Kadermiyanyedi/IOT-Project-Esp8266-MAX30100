import firebase_admin
from firebase_admin import credentials
from firebase_admin import db

# Fetch the service account key JSON file contents
cred = credentials.Certificate("firebase_adminsdk.json")

# Initialize the app with a service account, granting admin privileges
firebase_admin.initialize_app(
    cred,
    {"databaseURL": "dataBaseUrl"},
)

data = db.reference("sensor")
result = []


def get_data():
    result.append(data.get())
    return result