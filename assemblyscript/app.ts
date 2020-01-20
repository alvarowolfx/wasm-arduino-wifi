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

@external("arduino", "serialLog")
declare function println(output: string): void;
//declare function println(output: usize, len: i32): void;
//declare function println(output: string, len: i32): void;

function log(out: string): void {
  //println(out, out.length)  // <- Only first letter arrives, but lenght is correct
  println(out);
  //println(out); // <- Only first letter arrives
  //String.UTF16.encode(out) // <- Just referencing this crashes
  //println(changetype<usize>(String.UTF8.encode(out, true)), String.UTF8.byteLength(out, true)); // <- Crash
  //println(String.UTF8.encode(out, true), String.UTF8.byteLength(out, true)); // <- Crash
}

const LOW: u32 = 0;
const HIGH: u32 = 1;

const INPUT: u32 = 0x0;
const OUTPUT: u32 = 0x1;
const INPUT_PULLUP: u32 = 0x2;

let LED: u32 = -1;

function setup(): void {
  LED = getPinLED();
  pinMode(LED, OUTPUT);
}

function run(): void {
  log('From Web Assembly')
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