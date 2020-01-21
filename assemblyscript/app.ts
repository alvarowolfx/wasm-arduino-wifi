@external("arduino", "millis")
declare function millis(): u32;

@external("arduino", "delay")
declare function delay(ms: u32): void;

@external("arduino", "pinMode")
declare function pinMode(pin: u32, mode: u32): void;

@external("arduino", "digitalWrite")
declare function digitalWrite(pin: u32, value: u32): void;

@external("arduino", "getPinLED")
declare function getPinLED(): u32;

@external("arduino", "log")
declare function println(ptr: usize, len: i32): void;

@external("wifi", "wifiStatus")
declare function wifiStatus(): u32;

@external("wifi", "wifiConnect")
declare function wifiConnect(ssid: usize, ssid_len: i32, pass: usize, pass_len: i32): void;

@external("wifi", "printWifiLocalIp")
declare function printWifiLocalIp(): void;

function connect(ssid: string, pass: string): void {
  wifiConnect(
    changetype<usize>(String.UTF8.encode(ssid)), String.UTF8.byteLength(ssid),
    changetype<usize>(String.UTF8.encode(pass)), String.UTF8.byteLength(pass)
  )
}

function log(str: string): void {
  println(changetype<usize>(String.UTF8.encode(str)), String.UTF8.byteLength(str));
}

const LOW: u32 = 0;
const HIGH: u32 = 1;

const INPUT: u32 = 0x0;
const OUTPUT: u32 = 0x1;
const INPUT_PULLUP: u32 = 0x2;

const WL_NO_SHIELD: u32 = 0x255;
const WL_IDLE_STATUS: u32 = 0x0;
const WL_NO_SSID_AVAIL: u32 = 0x1;
const WL_SCAN_COMPLETED: u32 = 0x2;
const WL_CONNECTED: u32 = 0x3;
const WL_CONNECT_FAILED: u32 = 0x4;
const WL_CONNECTION_LOST: u32 = 0x5;
const WL_DISCONNECTED: u32 = 0x6;

let LED: u32 = -1;

const ssid = "YOUR_SSID"
const password = "YOUR_PASSWORD"

function setup(): void {
  LED = getPinLED();
  pinMode(LED, OUTPUT);
  log('Hello from AssemblyScript 😊\n')

  connect(ssid, password)
  log("Connecting to wifi \n")
  while (wifiStatus() != WL_CONNECTED) {
    delay(500);
    log(".")
  }
  log("Connected!\n")
  printWifiLocalIp()
}

function run(): void {
  const t: u32 = millis();
  log('[' + t.toString() + '] ' + 'Running. \n');

  digitalWrite(LED, HIGH);
  delay(1000);
  digitalWrite(LED, LOW);
  delay(1000);
}

/*
 * Entry point
 */
export function _start(): void {
  setup();
  while (1) run();
}