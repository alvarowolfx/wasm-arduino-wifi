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

let LED: u32 = -1;

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

function setup(): void {
  LED = getPinLED();
  pinMode(LED, OUTPUT);
  Serial.println('Hello from AssemblyScript 😊')
}

function run(): void {
  connect()
  const t: u32 = millis();
  const connected: bool = WiFi.status() === WiFi.WL_CONNECTED;
  const localIp: string = WiFi.localIp();

  Serial.println('[' + t.toString() + ']' + '[connected : ' + connected.toString() + '] [' + localIp + '] Running.');

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