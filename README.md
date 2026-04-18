# bmi270-components

ESPHome components for the Bosch BMI270

Configured with 2G

## esphome Configuration
You will need to create a `secrets.yaml` file with the following content, to be able to use esphome CLI:
```
wifi_ssid:     <your wifi ssid>
wifi_password: <your wifi password>

wifi_ap_name:     <your ap name>
wifi_ap_password: <your ap password>

homeassistant_api_key: <HomeAssistant generated API key>

ota_password: <your OTA password>
```