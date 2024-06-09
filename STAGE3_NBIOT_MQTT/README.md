# ESP32 Microcontroller Code

This code is designed for an ESP32 microcontroller to handle various tasks including network registration, MQTT communication, scheduling, and handling device information. The code utilizes the ArduinoJson library for JSON parsing and EEPROM for non-volatile memory storage.

## Features

- Connecting to MQTT Broker with NB-IoT Modem
- Subscribe to MQTT Topics
- Decode Received MQTT Messages

## MQTT Topics

- `MQTT_TOPIC_1` - "v1/devices/me/rpc/request/1"
- `MQTT_TOPIC_2` - "v1/devices/me/rpc/request/2"
- `MQTT_TOPIC_3` - "v1/devices/me/attributes"

## MQTT Message Handling

### Request to Reboot the Device

**Topic**: `v1/devices/me/rpc/request/1`  
**Payload**: `{"method": "reboot"}`

### Request for Schedule Configuration

A message to the below topic with the appropriate payload should be sent to the device. It acts as a request for schedule information for a specific date.

**Topic**: `v1/devices/me/rpc/request/2`  
**Payload**: `{"AC": DAY_OF_THE_WEEK}`

| Value | Day       | Special Day  |
|-------|-----------|--------------|
| 1     | MONDAY    | 8 = DAY 1    |
| 2     | TUESDAY   | 9 = DAY 2    |
| 3     | WEDNESDAY | 10 = DAY 3   |
| 4     | THURSDAY  | 11 = DAY 4   |
| 5     | FRIDAY    | 12 = DAY 5   |
| 6     | SATURDAY  |              |
| 7     | SUNDAY    |              |

### Request to Update Existing Schedule on the Device

**Topic**: `v1/devices/me/attributes`  
**Payload**: `{"AC": 1,"MOON": MOON, "MOOF": MOOF}`

- **MOON**: ON TIME (FORMAT HH:MM)
- **MOOF**: OFF TIME (FORMAT HH:MM)
