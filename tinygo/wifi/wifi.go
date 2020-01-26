package wifi

//go:wasm-module wifi
//go:export wifiStatus
func Status() uint

//go:wasm-module wifi
//go:export wifiConnect
func Connect(ssid string, password string)

//go:wasm-module wifi
//go:export wifiLocalIp
func _getLocalIp(buf *byte)

func LocalIp() string {
	var buf = make([]byte, 16)
	_getLocalIp(&buf[0])
	// Find '\0'
	n := -1
	for i, b := range buf {
		if b == 0 {
			break
		}
		n = i
	}
	return string(buf[:n+1])
}

const (
	WL_NO_SHIELD       = 0x255
	WL_IDLE_STATUS     = 0x0
	WL_NO_SSID_AVAIL   = 0x1
	WL_SCAN_COMPLETED  = 0x2
	WL_CONNECTED       = 0x3
	WL_CONNECT_FAILED  = 0x4
	WL_CONNECTION_LOST = 0x5
	WL_DISCONNECTED    = 0x6
)
