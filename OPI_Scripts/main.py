#!/home/atolyekapi/atolyekapi/.venv/bin/python3
import paho.mqtt.client as mqtt
from model import Users, Log
import time


def on_connect(mqttc, obj, flags, rc):
    print("rc: " + str(rc))


def on_message(mqttc, obj, msg):
    print(msg.topic + " " + str(msg.qos) + " " + str(msg.payload.decode('UTF-8')))

    if(msg.topic == "inside"):
        if(Users.is_authorized(msg.payload)):
            Log.log(msg.payload, "EXIT")
        mqttc.publish("I/O", "1")

    if(msg.topic == "outside"):
        if(Users.is_authorized(msg.payload)):
            mqttc.publish("I/O", "1")
            Log.log(msg.payload, "ENTRY")
    
    print("Local time:")

def on_publish(mqttc, obj, mid):
    print("mid: " + str(mid))
    pass


def on_subscribe(mqttc, obj, mid, granted_qos):
    print("Subscribed: " + str(mid) + " " + str(granted_qos))

def on_log(client, userdata, level, buf):
    print("log: ",buf)

mqtt_client = mqtt.Client()
mqtt_client.on_message = on_message
mqtt_client.on_connect = on_connect
mqtt_client.on_publish = on_publish
mqtt_client.on_log=on_log

mqtt_client.connect("localhost", 1883, 60)

mqtt_client.subscribe("inside", 0)
mqtt_client.subscribe("outside", 0)


mqtt_client.loop_forever()

