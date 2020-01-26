package serial

//go:wasm-module serial
//go:export print
func Print(s string)

func Println(s string) {
	Print(s)
	Print("\n")
}
