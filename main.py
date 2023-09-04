from flask import Flask, render_template, request
import mariadb
import json
import sys

app = Flask(__name__)

# database connection
conn = mariadb.connect(
        user="root",
        password="",
        host="localhost",
        port=3306,
        database="project8")

cur = conn.cursor()

sys.setrecursionlimit(50000)

#home page
@app.route("/")
def home():
    return render_template('index.html')

#page to handle the update request
@app.route("/insert", methods=['GET', 'POST'])
def insert():

    if(request.method == 'GET'):
        Appliance = request.args.get('appliance')
        Status = request.args.get('status')
        Value = request.args.get('value')
        Power = request.args.get('power')

        cur.execute("insert control(appliance, status, value, power) values (%s, %s, %s, %s)", (Appliance, Status, Value, Power))
        conn.commit()

        return "Insertion successful!"  # You can return a response here
    
    return "Something went wrong"

#page to update the database
@app.route("/update", methods=['GET', 'POST'])
def update():

    if(request.method == 'GET'):
        Appliance = request.args.get('appliance')
        Status = request.args.get('status')
        Value = request.args.get('value')
        Power = request.args.get('power')

        cur.execute("update control set status=%s, value=%s, power=%s where appliance=%s", (Status, Value, Power, Appliance))
        conn.commit()

        return "Updation successful!"
    
    return "Something went wrong"

#page to handle the fetching data from database
@app.route("/fetch", methods=['GET', 'POST'])
def fetch():

    if(request.method == 'GET'):
        Appliance = request.args.get('appliance')

        cur.execute("select * from control where appliance=%s", (Appliance,))
        responce = cur.fetchone()

        return json.dumps(responce)
    return "Something went wrong"

@app.route("/fetchpower", methods=['GET','POST'])
def fetchPower():
    if(request.method == 'GET'):
        Appliance = request.args.get('appliance')
        if(Appliance == 'AC' or Appliance == 'geyser'):
            cur.execute("select * from power where name=%s", (Appliance,))
            responce = cur.fetchone()
        else:
            return "no power data"
        return json.dumps(responce)
    return "Something went wrong"


if __name__ == "__main__":
    app.run(host='0.0.0.0', port = 80)
