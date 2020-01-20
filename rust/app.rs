#![no_std]

mod arduino_api;
use arduino_api::*;

static LED: u32 = 5;

fn setup() {
    pin_mode(LED, OUTPUT);
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