//
//  Wasm3 - high performance WebAssembly interpreter written in C.
//
//  Copyright © 2019 Steven Massey, Volodymyr Shymanskyy.
//  All rights reserved.
//

#include "Arduino.h"
#include <ArduinoOTA.h>
#include <FS.h>
#include <SPIFFS.h>
#include <wasm3.h>
#include "m3_arduino_api.h"

#define WASM_STACK_SLOTS 2048
#define NATIVE_STACK_SIZE 32 * 1024

// AssemblyScript app
// #include "../assemblyscript/app.wasm.h"

// Rust app
//#include "../rust/app.wasm.h"

#define FATAL(func, msg)              \
  {                                   \
    Serial.print("Fatal: " func " "); \
    Serial.println(msg);              \
    return;                           \
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

  return file.size();
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
  while (1)
  {
    M3Result result = m3Err_none;

    IM3Environment env = m3_NewEnvironment();
    if (!env)
      FATAL("NewEnvironment", "failed");

    IM3Runtime runtime = m3_NewRuntime(env, WASM_STACK_SLOTS, NULL);
    if (!runtime)
      FATAL("NewRuntime", "failed");

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
    result = m3_FindFunction(&f, runtime, "_start");
    if (result)
      FATAL("FindFunction", result);

    printf("Running WebAssembly...\n");

    const char *i_argv[1] = {NULL};
    result = m3_CallWithArgs(f, 0, i_argv);

    if (result)
      FATAL("CallWithArgs", result);

    // Should not arrive here
  }
}

bool otaStarted = false;
void startOta()
{

  if (otaStarted)
  {
    return;
  }

  // Port defaults to 3232
  ArduinoOTA.setPort(3232);

  // Hostname defaults to esp3232-[MAC]
  // ArduinoOTA.setHostname("myesp32");
  ArduinoOTA.setMdnsEnabled(true);

  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA
      .onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH)
          type = "sketch";
        else // U_SPIFFS
          type = "filesystem";

        // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
        Serial.println("Start updating " + type);
      })
      .onEnd([]() {
        Serial.println("\nEnd");
      })
      .onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
      })
      .onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR)
          Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR)
          Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR)
          Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR)
          Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR)
          Serial.println("End Failed");
      });

  ArduinoOTA.begin();
  otaStarted = true;
}

void handleOTA()
{
  ArduinoOTA.handle();
  if (WiFi.status() == WL_CONNECTED)
  {
    startOta();
  }
  else
  {
    otaStarted = false;
    ArduinoOTA.end();
  }
}

void setup()
{
  Serial.begin(115200);
  delay(100);

  SPIFFS.begin();
  delay(100);

  Serial.print("\nWasm3 v" M3_VERSION ", build " __DATE__ " " __TIME__ "\n");

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
  handleOTA();
  delay(100);
}