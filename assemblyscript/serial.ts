
@external("serial", "print")
declare function _print(ptr: usize, len: i32): void;

export function print(str: string): void {
  let out = String.UTF8.encode(str);
  _print(changetype<usize>(out), out.byteLength);
}

export function println(str: string): void {
  print(str + '\n')
}