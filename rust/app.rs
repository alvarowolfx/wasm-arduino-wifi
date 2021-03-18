#![no_std]

mod arduino_api;
use arduino_api::*;

const BLINK_INTERVAL: u32 = 1000;
const SSID : &str = "YOUR_SSID"; 
const PASSWORD : &str = "YOUR_PASS";

struct App {
  led_pin: u32,
  last_millis: u32,
  led_state: bool,
}

impl App {

  fn new() -> Self {
    let led_pin = getPinLED();
    let led_state = false;
    let last_millis = millis();

    pinMode(led_pin, OUTPUT);
    digitalWrite(led_pin, LOW );

    serialPrintln("Hello from Rust 🦀");
    Self { led_pin, led_state, last_millis }
  }

  fn connect(&self) {
    if wifiStatus() == WL_CONNECTED {
      return;
    }

    wifiConnect(SSID, PASSWORD);
    serialPrintln("Connecting");
    let mut attempts = 0;
    while wifiStatus() != WL_CONNECTED {
      delay(500);
      serialPrint(".");
      attempts = attempts + 1;
      if attempts >= 10 {
        serialPrintln("Failed to connect!");
        return;
      }
    }
    serialPrintln("Connected!");
    printWifiLocalIp();
    serialPrintln("");
  }

  fn run(&mut self) {
    self.connect();
    let current_millis: u32 = millis();
    if current_millis - self.last_millis >= BLINK_INTERVAL {
      let connected: bool = wifiStatus() == WL_CONNECTED;

      serialPrint("[");
      serialPrintInt(current_millis);
      serialPrint("][connected : ");
      serialPrint(if connected { "true" } else { "false" });
      serialPrint("][");
      printWifiLocalIp();
      serialPrintln("] Rust 🦀");

      self.led_state = !self.led_state;
      digitalWrite(self.led_pin, if self.led_state { HIGH } else { LOW });

      self.last_millis = millis();
    }
    delay(10);
  }
}

/*
 * Entry point
 */

#[no_mangle]
pub extern fn _start() {
    let mut app = App::new();
    loop {
        app.run();
    }
}

#[panic_handler]
fn handle_panic(_: &core::panic::PanicInfo) -> ! {
    loop {}
}