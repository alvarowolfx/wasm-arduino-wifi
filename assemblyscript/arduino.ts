@external("arduino", "millis")
export declare function millis(): u32;

@external("arduino", "delay")
export declare function delay(ms: u32): void;

@external("arduino", "pinMode")
export declare function pinMode(pin: u32, mode: u32): void;

@external("arduino", "digitalWrite")
export declare function digitalWrite(pin: u32, value: u32): void;

@external("arduino", "getPinLED")
export declare function getPinLED(): u32;

export const LOW: u32 = 0;
export const HIGH: u32 = 1;

export const INPUT: u32 = 0x0;
export const OUTPUT: u32 = 0x1;
export const INPUT_PULLUP: u32 = 0x2;