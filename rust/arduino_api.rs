#![allow(dead_code)]
#![allow(non_snake_case)]

#[link(wasm_import_module = "arduino")]
extern {
  #[link_name = "millis"]         fn unsafe_millis() -> u32;
  #[link_name = "delay"]          fn unsafe_delay(ms: u32);
  #[link_name = "pinMode"]        fn unsafe_pinMode(pin:u32, mode:u32);
  #[link_name = "digitalWrite"]   fn unsafe_digitalWrite(pin:u32, value:u32);
  #[link_name = "getPinLED"]      fn unsafe_getPinLED() -> u32;
  #[link_name = "getChipID"] fn unsafe_getChipID(buf : *mut u8);
}

#[link(wasm_import_module = "serial")]
extern {
  #[link_name = "print"] fn unsafe_print(ptr: *const u8, size: usize);
  #[link_name = "printInt"] fn unsafe_print_int(out: u32);
}

#[link(wasm_import_module = "wifi")]
extern {
  #[link_name = "wifiConnect"] fn unsafe_wifi_connect(ssid: *const u8, ssid_len : usize, password: *const u8, password_len : usize);
  #[link_name = "wifiStatus"] fn unsafe_wifi_status() -> u32;
  #[link_name = "wifiLocalIp"] fn unsafe_wifi_local_ip(buf : *mut u8);
  #[link_name = "printWifiLocalIp"] fn unsafe_print_wifi_local_ip();
}

pub static LOW:u32  = 0;
pub static HIGH:u32 = 1;

pub static INPUT:u32          = 0x0;
pub static OUTPUT:u32         = 0x1;
pub static INPUT_PULLUP:u32   = 0x2;

/* Wifi Status */
pub static WL_NO_SHIELD: u32        = 255;
pub static WL_IDLE_STATUS: u32      = 0;
pub static WL_NO_SSID_AVAIL: u32    = 1;
pub static WL_SCAN_COMPLETED: u32   = 2;
pub static WL_CONNECTED: u32        = 0x03;
pub static WL_CONNECT_FAILED: u32   = 4;
pub static WL_CONNECTION_LOST: u32  = 5;
pub static WL_DISCONNECTED: u32     = 6;

pub fn millis         () -> u32              { unsafe { unsafe_millis() } }
pub fn delay          (ms: u32)              { unsafe { unsafe_delay(ms); } }
pub fn pinMode       (pin:u32, mode:u32)    { unsafe { unsafe_pinMode(pin, mode) } }
pub fn digitalWrite  (pin:u32, value:u32)   { unsafe { unsafe_digitalWrite(pin, value) } }
pub fn serialPrintInt(out: u32) { unsafe { unsafe_print_int(out); } }
pub fn serialPrint     (out: &str)       {
  unsafe {
    unsafe_print(out.as_bytes().as_ptr() as *const u8, out.len());
  }
}

pub fn serialPrintln     (out: &str)       {
  serialPrint(out);
  serialPrint("\n");
}

pub fn getPinLED    () -> u32 { unsafe { unsafe_getPinLED() } }

pub fn wifiConnect (ssid :&str, password : &str){
  unsafe {
    unsafe_wifi_connect(
      ssid.as_bytes().as_ptr() as *const u8, ssid.len(),
      password.as_bytes().as_ptr() as *const u8, password.len()
    );
  }
}
pub fn wifiStatus    () -> u32 { unsafe { unsafe_wifi_status() } }
pub fn printWifiLocalIp () { unsafe { unsafe_print_wifi_local_ip() } }