
@external("serial", "print")
declare function _print(ptr: usize, len: i32): void;

export function print(str: string): void {
  _print(changetype<usize>(String.UTF8.encode(str)), String.UTF8.byteLength(str));
}

export function println(str: string): void {
  print(str + '\n')
}