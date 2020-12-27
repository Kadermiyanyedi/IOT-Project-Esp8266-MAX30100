from flask import *
from iot_firebase import iot_firebase_esp8266

app = Flask(__name__)


@app.route("/", methods=["GET"])
def home():
    results = iot_firebase_esp8266.get_data()
    return render_template("index.html", data=results, last_data=results[-1])


if __name__ == "__main__":
    app.run(debug=True, port=5252)