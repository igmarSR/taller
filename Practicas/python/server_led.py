'''
    Raspberry Pi GPIO Status and Control
'''
import RPi.GPIO as GPIO
from flask import Flask, render_template, request
app = Flask(__name__)
GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)
#define actuators GPIOs
ledRed = 21

#initialize GPIO status variables
ledRedSts = 0

# Define led pins as output
GPIO.setup(ledRed, GPIO.OUT)   
# turn leds OFF 
GPIO.output(ledRed, GPIO.LOW)
    
@app.route("/")
def index():
    # Read Sensors Status
    ledRedSts = GPIO.input(ledRed)
    templateData = {
              'title' : 'GPIO output Status!',
              'ledRed'  : ledRedSts,
        }
    return render_template('index_led.html', **templateData)
    
@app.route("/<deviceName>/<action>")
def action(deviceName, action):
    if deviceName == 'ledRed':
        actuator = ledRed   
    if action == "on":
        GPIO.output(actuator, GPIO.HIGH)
    if action == "off":
        GPIO.output(actuator, GPIO.LOW)
             
    ledRedSts = GPIO.input(ledRed)
   
    templateData = {
              'ledRed'  : ledRedSts,
    }
    return render_template('index_led.html', **templateData)
if __name__ == "__main__":
   print("INICIANDO...............")
   app.run(host='0.0.0.0', port=80, debug=True)