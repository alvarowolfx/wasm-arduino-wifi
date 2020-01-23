# Running WASM on Arduino 

Testing WebAssembly on IoT devices. 

This is a Work in Progress 

### OTA Update WASM file

The file is being saved on SPIFFS and the Arduino is loading from it to run on WASM3 VM. So we can update the WASM code without changing the host code.

Steps to update WASM code:

* `cd` into `assemblyscript` folder and run `npm run build`.
  * This will compile the AS code to Wasm and also copy the file to the `data` folder

#### To update locally via cable
* Run the command `pio run --target uploadfs`

#### To update via WiFi
* Your device needs to sucessfully connect to the network. Then follow the steps:
* First search for the MDNS service using `dns-sd -B _ota .`. Example output:
```
Browsing for _ota._tcp
DATE: ---Thu 23 Jan 2020---
 0:04:57.636  ...STARTING...
Timestamp     A/R    Flags  if Domain               Service Type         Instance Name
 0:07:10.852  Add        2   8 local.               _ota._tcp.           esp-38A4AE30
```
* Them send an Wasm file on the /upload endpoint on the device:
```
curl -X POST -F "app.wasm=@./app.wasm" http://esp-38A4AE30.local/upload
```

