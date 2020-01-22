
@external("wifi", "wifiStatus")
declare function _wifiStatus(): u32;

@external("wifi", "wifiConnect")
declare function _wifiConnect(ssid: usize, ssid_len: i32, pass: usize, pass_len: i32): void;

@external("wifi", "wifiLocalIp")
declare function _wifiLocalIp(ptr: usize): void;

export function connect(ssid: string, pass: string): void {
  _wifiConnect(
    changetype<usize>(String.UTF8.encode(ssid)), String.UTF8.byteLength(ssid),
    changetype<usize>(String.UTF8.encode(pass)), String.UTF8.byteLength(pass)
  )
}

export function status(): u32 {
  return _wifiStatus();
}

export function localIp(): string {
  const arr = new ArrayBuffer(16);
  _wifiLocalIp(changetype<usize>(arr));
  return String.UTF8.decode(arr, true)
}

export const WL_NO_SHIELD: u32 = 0x255;
export const WL_IDLE_STATUS: u32 = 0x0;
export const WL_NO_SSID_AVAIL: u32 = 0x1;
export const WL_SCAN_COMPLETED: u32 = 0x2;
export const WL_CONNECTED: u32 = 0x3;
export const WL_CONNECT_FAILED: u32 = 0x4;
export const WL_CONNECTION_LOST: u32 = 0x5;
export const WL_DISCONNECTED: u32 = 0x6;
