
#include "m3_arduino_api.h"

#include <m3_api_defs.h>
#include <m3_env.h>

#include <Arduino.h>
#include <WiFi.h>
/*
 * Note: each RawFunction should complete with one of these calls:
 *   m3ApiReturn(val)   - Returns a value
 *   m3ApiSuccess()     - Returns void (and no traps)
 *   m3ApiTrap(trap)    - Returns a trap
 */

m3ApiRawFunction(m3_arduino_millis)
{
  m3ApiReturnType(uint32_t)

      m3ApiReturn(millis());
}

m3ApiRawFunction(m3_arduino_delay)
{
  m3ApiGetArg(uint32_t, ms)

      //printf("api: delay %d\n", ms); // you can also trace API calls

      delay(ms);

  m3ApiSuccess();
}

// This maps pin modes from arduino_wasm_api.h
// to actual platform-specific values
uint8_t mapPinMode(uint8_t mode)
{
  switch (mode)
  {
  case (0):
    return INPUT;
  case (1):
    return OUTPUT;
  case (2):
    return INPUT_PULLUP;
  }
  return INPUT;
}

m3ApiRawFunction(m3_arduino_pinMode)
{
  m3ApiGetArg(uint32_t, pin)
      m3ApiGetArg(uint32_t, mode)

          pinMode(pin, mapPinMode(mode));

  m3ApiSuccess();
}

m3ApiRawFunction(m3_arduino_digitalWrite)
{
  m3ApiGetArg(uint32_t, pin)
      m3ApiGetArg(uint32_t, value)

          digitalWrite(pin, value);

  m3ApiSuccess();
}

// This is a convenience function
m3ApiRawFunction(m3_arduino_getPinLED)
{
  m3ApiReturnType(uint32_t)

      m3ApiReturn(LED_BUILTIN);
}

m3ApiRawFunction(m3_arduino_log)
{
  m3ApiGetArgMem(const byte *, out);
  m3ApiGetArg(int32_t, out_len);

  //printf("api: log %p\n", out);

  byte buff[out_len + 1];
  memcpy(buff, out, out_len);
  buff[out_len] = '\0';

  Serial.print((char *)buff);

  m3ApiSuccess();
}

m3ApiRawFunction(m3_arduino_wifi_status)
{
  m3ApiReturnType(uint32_t)

      m3ApiReturn(WiFi.status());
}

m3ApiRawFunction(m3_arduino_wifi_connect)
{
  m3ApiGetArgMem(const byte *, ssid);
  m3ApiGetArg(int32_t, ssid_len);
  m3ApiGetArgMem(const byte *, pass);
  m3ApiGetArg(int32_t, pass_len);

  byte local_ssid[ssid_len + 1];
  byte local_password[pass_len + 1];

  memcpy(local_ssid, ssid, ssid_len);
  local_ssid[ssid_len] = '\0';
  memcpy(local_password, pass, pass_len);
  local_password[pass_len] = '\0';

  WiFi.begin((char *)local_ssid, (char *)local_password);

  m3ApiSuccess();
}

m3ApiRawFunction(m3_arduino_wifi_local_ip)
{
  m3ApiReturnType(const byte *);

  const char *ip = WiFi.localIP().toString().c_str();
  uint32_t len = strlen(ip);
  byte local_ip[len + 1];
  memcpy(local_ip, ip, len);
  local_ip[len] = '\0';
  Serial.println(ip);

  m3ApiReturn(local_ip);
}

m3ApiRawFunction(m3_arduino_wifi_print_local_ip)
{
  Serial.println(WiFi.localIP());

  m3ApiSuccess();
}

// Dummy, for TinyGO
m3ApiRawFunction(m3_dummy)
{
  m3ApiSuccess();
}

M3Result m3_LinkArduino(IM3Runtime runtime)
{
  IM3Module module = runtime->modules;
  const char *arduino = "arduino";
  const char *wifi = "wifi";

  m3_LinkRawFunction(module, arduino, "millis", "i()", &m3_arduino_millis);
  m3_LinkRawFunction(module, arduino, "delay", "v(i)", &m3_arduino_delay);
  m3_LinkRawFunction(module, arduino, "pinMode", "v(ii)", &m3_arduino_pinMode);
  m3_LinkRawFunction(module, arduino, "digitalWrite", "v(ii)", &m3_arduino_digitalWrite);
  m3_LinkRawFunction(module, arduino, "log", "v(*i)", &m3_arduino_log);

  m3_LinkRawFunction(module, arduino, "getPinLED", "i()", &m3_arduino_getPinLED);

  /* Wifi */
  m3_LinkRawFunction(module, wifi, "wifiStatus", "i()", &m3_arduino_wifi_status);
  m3_LinkRawFunction(module, wifi, "wifiConnect", "v(*i*i)", &m3_arduino_wifi_connect);
  m3_LinkRawFunction(module, wifi, "wifiLocalIp", "*()", &m3_arduino_wifi_local_ip);
  m3_LinkRawFunction(module, wifi, "printWifiLocalIp", "v()", &m3_arduino_wifi_print_local_ip);

  m3_LinkRawFunction(module, "env", "io_get_stdout", "i()", &m3_dummy);

  return m3Err_none;
}
