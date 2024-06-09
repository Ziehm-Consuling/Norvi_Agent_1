Code for an ESP32 microcontroller to handle various tasks including network registration, MQTT communication, scheduling, and handling device information. The code utilizes the ArduinoJson library for JSON parsing and EEPROM for non-volatile memory storage.- Connecting to MQTT Broker with NB-IoT Modem

- Subscribe to MQTT Topics
  MQTT_TOPIC_1 "v1/devices/me/rpc/request/1"
  MQTT_TOPIC_2 "v1/devices/me/rpc/request/2"
  MQTT_TOPIC_3 "v1/devices/me/attributes"

- Decode Received MQTT Messages
  Request to Reboot the device
  topic : v1/devices/me/rpc/request/1
  payload : {"method": "reboot"}

  Request for schedue configuration / A message to below topic with appropriate payload should be sent to device. It acts as a request for schedule information for a specific date.  
   topic: v1/devices/me/rpc/request/2
   payload: {"AC":<DAY OF THE WEEK>}
  1 = MONDAY            8 = DAY 1
  2 = TUESDAY           9 = DAY 2
  3 = WEDNESDAY         10 = DAY 3
  4 = THURSDAY          11 = DAY 4
  5 = FRIDAY            12 = DAY 5
  6 = SATURDAY
  7 = SUNDAY

  Request to update existing schedule on the device
  topic: 'v1/devices/me/attributes'
  payload : {"AC": 1,"MOON": MOON, "MOOF": MOOF,}
  MOON = ON TIME / FORMAT HH:MM
  MOOF = OFF TIME / FORMAT HH:MM
