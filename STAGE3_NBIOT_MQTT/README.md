MQTT Topics
MQTT_TOPIC_1 - "v1/devices/me/rpc/request/1"
MQTT_TOPIC_2 - "v1/devices/me/rpc/request/2"
MQTT_TOPIC_3 - "v1/devices/me/attributes"
MQTT Message Handling
Request to Reboot the Device
Topic: v1/devices/me/rpc/request/1
Payload: {"method": "reboot"}
Request for Schedule Configuration
A message to the below topic with the appropriate payload should be sent to the device. It acts as a request for schedule information for a specific date.

Topic: v1/devices/me/rpc/request/2
Payload: {"AC": DAY_OF_THE_WEEK}
Value	Day	Special Day
1	MONDAY	8 = DAY 1
2	TUESDAY	9 = DAY 2
3	WEDNESDAY	10 = DAY 3
4	THURSDAY	11 = DAY 4
5	FRIDAY	12 = DAY 5
6	SATURDAY	
7	SUNDAY	
Request to Update Existing Schedule on the Device
Topic: v1/devices/me/attributes

Payload: {"AC": 1,"MOON": MOON, "MOOF": MOOF,}

MOON: ON TIME (FORMAT HH
)
MOOF: OFF TIME (FORMAT HH
)
Example Commands
Reboot Command:

json
Copy code
{"method":"reboot"}
Schedule Request Commands:

json
Copy code
{"AC": 1}  // For Monday
{"AC": 2}  // For Tuesday
// ...and so on
Schedule Configuration Commands:

json
Copy code
{"AC": 1, "MOON": "14:23", "MOOF": "18:51"}
// ...and similar commands for other days and special dates.
