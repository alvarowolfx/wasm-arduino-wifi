
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
  m3ApiGetArgMem(const char *, out)
      //m3ApiGetArg(uint32_t, out_len)

      //char output[out_len + 1];
      //memcpy(output, out, out_len);
      //printf(out);
      Serial.println(out);
  //Serial.println(output);
  //Serial.println(strlen(out));
  //Serial.println(sizeof(out));
  //Serial.println(String(out));

  m3ApiSuccess();
}

m3ApiRawFunction(m3_arduino_wifi_status)
{
  m3ApiReturnType(uint32_t)

      m3ApiReturn(WiFi.status());
}

m3ApiRawFunction(m3_arduino_wifi_connect)
{
  m3ApiGetArgMem(const char *, ssid)
      m3ApiGetArgMem(const char *, password);

  uint32_t len_ssid = strlen(ssid) + 1;
  char local_ssid[len_ssid];
  uint32_t len_password = strlen(password) + 1;
  char local_password[len_password];

  memcpy(local_ssid, ssid, len_ssid);
  memcpy(local_password, password, len_password);
  Serial.println("Ssid");
  Serial.println(local_ssid);
  Serial.println("Password");
  Serial.println(local_password);

  WiFi.begin(local_ssid, local_password);
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
  m3_LinkRawFunction(module, arduino, "serialLog", "v(*)", &m3_arduino_log);
  //m3_LinkRawFunction(module, arduino, "serialLog", "v(*i)", &m3_arduino_log);

  m3_LinkRawFunction(module, arduino, "getPinLED", "i()", &m3_arduino_getPinLED);

  /* Wifi */
  m3_LinkRawFunction(module, wifi, "wifiStatus", "i()", &m3_arduino_wifi_status);
  m3_LinkRawFunction(module, wifi, "wifiConnect", "v(**)", &m3_arduino_wifi_connect);

  m3_LinkRawFunction(module, "env", "io_get_stdout", "i()", &m3_dummy);

  return m3Err_none;
}
