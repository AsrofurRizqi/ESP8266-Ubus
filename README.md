## Openwrt Ubus Monitoring ESP8266
    Simple resource monitoring for openwrt os via ubus API

#### Ubus Documentation
[Openwrt Ubus](https://openwrt.org/docs/techref/ubus)

#### Usage
##### Setup Oled Connection :
- VCC oled &rarr; 3v pins
- GND oled &rarr; GND pins
- SCL oled &rarr; D1 pins
- SDA oled &rarr; D2 pins

##### Create Ubus Read Only User :

- Touch lesssuperuser.json on ```/usr/share/rpcd/acl.d/```
```
{
        "lesssuperuser": {
                "description": "read only user",
                "read": {
                        "ubus": {
                                "*": [ "*" ]
                        },
                        "uci": [ "*" ],
                },
        }
}
```
- Add rpcd user on ```/etc/config/rpcd```
```
config login
        option username 'username'
        option password 'generatedpassword'
        list read lesssuperuser
```
- Generate password using uhttpd ```uhttpd -m yourpassword``` example : ```$1$$2o.aphMT3BTmkPqUbYh7u1```
- Change default user on code with created user.
- Change openwrt ip on code.
- Change default wifi SSID and password on code.
- Time Based on NTP Client
#### Test

#### References
- [Adafruit SSD1306](https://github.com/adafruit/Adafruit_SSD1306)
- [ArduinoJson](https://github.com/bblanchon/ArduinoJson)
- [NTPClient](https://github.com/arduino-libraries/NTPClient)
- [Time](https://github.com/PaulStoffregen/Time)