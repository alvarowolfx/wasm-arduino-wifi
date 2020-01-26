//
//  Wasm3 - high performance WebAssembly interpreter written in C.
//
//  Copyright © 2019 Steven Massey, Volodymyr Shymanskyy.
//  All rights reserved.
//

#include "Arduino.h"
#ifdef ESP32
#include <FS.h>
#include <SPIFFS.h>
#include <ESPmDNS.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESP8266mDNS.h>
#endif
#include <ESPAsyncWebServer.h>
#include <wasm3.h>
#include "m3_arduino_api.h"

#define WASM_STACK_SLOTS 2 * 1024
#define NATIVE_STACK_SIZE 32 * 1024

AsyncWebServer server(80);
// AssemblyScript app
// #include "../assemblyscript/app.wasm.h"

// Rust app
//#include "../rust/app.wasm.h"

#define FATAL_MSG(func, msg)          \
  {                                   \
    Serial.print("Fatal: " func " "); \
    Serial.println(msg);              \
    delay(1000);                      \
  }

#define FATAL(func, msg) \
  {                      \
    FATAL_MSG(func, msg) \
    continue;            \
  }

size_t readWasmFileSize(const char *path)
{
  Serial.printf("Reading file: %s\n", path);

  if (!SPIFFS.exists(path))
  {
    Serial.println("File not found");
    return 0;
  }

  File file = SPIFFS.open(path);
  if (!file)
  {
    Serial.println("Failed to open file for reading");
    return 0;
  }
  size_t size = file.size();
  file.close();
  return size;
}

size_t readWasmFile(const char *path, uint8_t *buf)
{
  Serial.printf("Reading file: %s\n", path);

  if (!SPIFFS.exists(path))
  {
    Serial.println("File not found");
    return 0;
  }

  File file = SPIFFS.open(path);
  if (!file)
  {
    Serial.println("Failed to open file for reading");
    return 0;
  }

  Serial.print("Read from file: ");
  size_t i = 0;
  while (file.available())
  {
    buf[i] = file.read();
    i++;
  }
  file.close();

  return i;
}

void wasm_task(void *)
{
  IM3Environment env = m3_NewEnvironment();
  if (!env)
  {
    FATAL_MSG("NewEnvironment", "failed");
    return;
  }

  IM3Runtime runtime = m3_NewRuntime(env, WASM_STACK_SLOTS, NULL);
  if (!runtime)
  {
    FATAL_MSG("NewRuntime", "failed");
    return;
  }

  while (1)
  {
    M3Result result = m3Err_none;

    size_t app_wasm_size = readWasmFileSize("/app.wasm");
    if (app_wasm_size == 0)
      FATAL("ReadWasm", "File not found")

    uint8_t buffer[app_wasm_size];
    size_t read_bytes = readWasmFile("/app.wasm", buffer);
    if (read_bytes == 0)
      FATAL("ReadWasm", "File not found")

    IM3Module module;
    result = m3_ParseModule(env, &module, buffer, app_wasm_size - 1);
    if (result)
      FATAL("ParseModule", result);

    result = m3_LoadModule(runtime, module);
    if (result)
      FATAL("LoadModule", result);

    result = m3_LinkArduino(runtime);
    if (result)
      FATAL("LinkArduino", result);

    IM3Function f;
    // TinyGo workaround
    result = m3_FindFunction(&f, runtime, "cwa_main");
    if (result)
    {
      result = m3_FindFunction(&f, runtime, "_start");
    }
    if (result)
      FATAL("FindFunction", result);

    printf("Running WebAssembly...\n");

    const char *i_argv[1] = {NULL};
    result = m3_CallWithArgs(f, 0, i_argv);

    // Should not arrive here

    if (result)
    {
      M3ErrorInfo info;
      m3_GetErrorInfo(runtime, &info);
      Serial.print("Error: ");
      Serial.print(result);
      Serial.print(" (");
      Serial.print(info.message);
      Serial.println(")");
    }
  }
}

void handleUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
{
  if (!filename.endsWith(".wasm"))
  {
    request->send(400, "text/plain", "Only wasm files accepted");
    return;
  }
  if (!index)
  {
    Serial.printf("UploadStart: %s\n", filename.c_str());
    request->_tempFile = SPIFFS.open("/app.wasm", "w");
  }
  if (len)
  {
    request->_tempFile.write(data, len);
  }
  //Serial.printf("%s", (const char *)data);
  if (final)
  {
    request->_tempFile.close();
    Serial.printf("UploadEnd: %s (%u)\n", filename.c_str(), index + len);
    request->send(200, "text/plain", "Uploaded");
  }
}

void setup()
{
  Serial.begin(115200);
  delay(100);

  SPIFFS.begin();
  delay(100);

  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP("WasmOTA");

  uint64_t chipid = ESP.getEfuseMac();
  char hostname[20];
  sprintf(hostname, "esp-%08X", (uint32_t)chipid);

  MDNS.begin(hostname);
  server.on("/upload", HTTP_POST, [](AsyncWebServerRequest *request) {}, handleUpload);
  server.begin();
  MDNS.addService("ota", "tcp", 80);

  Serial.print("\nWasm3 v" M3_VERSION ", build " __DATE__ " " __TIME__ "\n");
  Serial.print(hostname);

#ifdef ESP32
  // On ESP32, we can launch in a separate thread
  // xTaskCreate(&wasm_task, "wasm3", NATIVE_STACK_SIZE, NULL, 5, NULL);
  xTaskCreatePinnedToCore(&wasm_task, "wasm3", NATIVE_STACK_SIZE, NULL, 5, NULL, 1);
#else
  wasm_task(NULL);
#endif
}

void loop()
{
  delay(100);
}