from flask import Flask, render_template
import firebase_admin
from firebase_admin import credentials, db

# Point Flask to look inside "templates" for HTML files
app = Flask(__name__)

# Firebase setup
cred = credentials.Certificate("anticheat.json")
firebase_admin.initialize_app(cred, {
    "databaseURL": "https://anticheat-21e70-default-rtdb.asia-southeast1.firebasedatabase.app/"
})

@app.route("/")
def index():
    ref = db.reference("/")   # Root of database
    data = ref.get()          # Fetch all data
    return render_template("test.html", data=data)

if __name__ == "__main__":
    app.run(debug=True)
