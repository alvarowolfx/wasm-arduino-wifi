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
* First search for the MDNS service using `dns-sd -B _arduino .`. Example output:
```
Browsing for _arduino._tcp
DATE: ---Wed 22 Jan 2020---
9:51:36.357  ...STARTING...
Timestamp     A/R    Flags  if Domain               Service Type         Instance Name 
9:51:36.358  Add        2   8 local.               _arduino._tcp.       esp32-30aea438c8e0
```
* Them run the same pio command to uploadfs, but with the service name:
```
pio run --target uploadfs --upload-port esp32-30aea438c8e0.local
```

