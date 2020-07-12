
@external("wifi", "wifiStatus")
declare function _wifiStatus(): u32;

@external("wifi", "wifiConnect")
declare function _wifiConnect(ssid: usize, ssid_len: i32, pass: usize, pass_len: i32): void;

@external("wifi", "wifiLocalIp")
declare function _wifiLocalIp(ptr: usize): void;

export function connect(ssid: string, pass: string): void {
  let outSsid = String.UTF8.encode(ssid);
  let outPass = String.UTF8.encode(pass);
  _wifiConnect(
    changetype<usize>(outSsid), outSsid.byteLength,
    changetype<usize>(outPass), outPass.byteLength
  )
}

export function status(): u32 {
  return _wifiStatus();
}

export function localIp(): string {
  const ptr = memory.data(16);
  _wifiLocalIp(ptr);
  return String.UTF8.decodeUnsafe(ptr, 16, true);
}

export const WL_NO_SHIELD: u32 = 0x255;
export const WL_IDLE_STATUS: u32 = 0x0;
export const WL_NO_SSID_AVAIL: u32 = 0x1;
export const WL_SCAN_COMPLETED: u32 = 0x2;
export const WL_CONNECTED: u32 = 0x3;
export const WL_CONNECT_FAILED: u32 = 0x4;
export const WL_CONNECTION_LOST: u32 = 0x5;
export const WL_DISCONNECTED: u32 = 0x6;
