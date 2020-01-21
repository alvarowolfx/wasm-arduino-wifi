#![no_std]

mod arduino_api;
use arduino_api::*;

static LED: u32 = 5;

fn setup() {
    pin_mode(LED, OUTPUT);

    wifi_connect("ssid", "pass");
    delay(500);
    while wifi_status() != WL_CONNECTED {
      delay(500);
      serial_log(".");
    }

    serial_log("WiFi Connected");    
}

fn run() {
    serial_log("Hello from Rust");    
    digital_write(LED, HIGH);
    delay(1000);
    digital_write(LED, LOW);
    delay(1000);
}

/*
 * Entry point
 */

#[no_mangle]
pub extern fn _start() {
    setup();
    loop {
        run();
    }
}

#[panic_handler]
fn handle_panic(_: &core::panic::PanicInfo) -> ! {
    loop {}
}