# Norvi_Agent_1
Industrial IoT Node

Firmware development for NORVI AGENT1 CST-BT9-E

Development Stage 1
- Hosting webserver on ESP32
- Retrive data from webpage and save on ESP32 Flash memory
- Retrive data from ESP32 Flash memory and push to webpage

Development Stage 2
- Read date and time information from DS3231 RTC
- Set alarms on DS3231 RTC for Special dates and week day information 
- Turn output ON and OFF based on current date and time and schedule

Development Stage 3
- Connect to MQTT broker over NB-IoT
- Publish start up data
- Publish current consumption variable on a set interval
- Subscribe to topics for schedule updates
- Publish schedule on MQTT requests

Development Stage 4
- Merge Stage 2 with Stage 3