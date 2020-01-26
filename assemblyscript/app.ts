import * as WiFi from './wifi';
import {
  HIGH,
  LOW,
  OUTPUT,
  millis,
  delay,
  pinMode,
  digitalWrite,
  getPinLED
} from './arduino';
import * as Serial from './serial';

let ledPin: u32 = -1;

const ssid = "YOUR_SSID"
const password = "YOUR_PASSWORD"

function connect(): void {
  if (WiFi.status() === WiFi.WL_CONNECTED) {
    return;
  }

  WiFi.connect(ssid, password)
  Serial.println("Connecting")
  let attempts = 0
  while (WiFi.status() != WiFi.WL_CONNECTED) {
    delay(500);
    Serial.print(".")
    attempts++
    if (attempts >= 10) {
      Serial.println("Failed to connect!")
      return;
    }
  }
  Serial.println("Connected!")
  Serial.println(WiFi.localIp())
}

const blinkInterval: u32 = 1000;
let lastMillis: u32 = 0;
let ledState: bool = false;

function setup(): void {
  ledPin = getPinLED();
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledState ? HIGH : LOW);
  lastMillis = millis();
  Serial.println('Hello from AssemblyScript 😊')
}

function run(): void {
  connect();
  const currentMillis: u32 = millis();
  if (currentMillis - lastMillis >= blinkInterval) {
    const connected: bool = WiFi.status() === WiFi.WL_CONNECTED;
    const localIp: string = WiFi.localIp();
    Serial.println('[' + currentMillis.toString() + ']' + '[connected : ' + connected.toString() + '] [' + localIp + '] AssemblyScript 😊');
    ledState = !ledState;
    digitalWrite(ledPin, ledState ? HIGH : LOW);
    lastMillis = millis();
  }
  delay(10);
}

/*
 * Entry point
 */
export function _start(): void {
  setup();
  while (1) run();
}