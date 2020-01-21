//
//  Wasm3 - high performance WebAssembly interpreter written in C.
//
//  Copyright © 2019 Steven Massey, Volodymyr Shymanskyy.
//  All rights reserved.
//

#include "Arduino.h"

#include <wasm3.h>
#include "m3_arduino_api.h"

#define WASM_STACK_SLOTS 2048
#define NATIVE_STACK_SIZE 32 * 1024

// AssemblyScript app
#include "../assemblyscript/app.wasm.h"

// Rust app
//#include "../rust/app.wasm.h"

#define FATAL(func, msg)              \
  {                                   \
    Serial.print("Fatal: " func " "); \
    Serial.println(msg);              \
    return;                           \
  }

void wasm_task(void *)
{
  M3Result result = m3Err_none;

  IM3Environment env = m3_NewEnvironment();
  if (!env)
    FATAL("NewEnvironment", "failed");

  IM3Runtime runtime = m3_NewRuntime(env, WASM_STACK_SLOTS, NULL);
  if (!runtime)
    FATAL("NewRuntime", "failed");

  IM3Module module;
  result = m3_ParseModule(env, &module, app_wasm, app_wasm_len - 1);
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

void setup()
{
  Serial.begin(115200);
  delay(100);

  Serial.print("\nWasm3 v" M3_VERSION ", build " __DATE__ " " __TIME__ "\n");

#ifdef ESP32
  // On ESP32, we can launch in a separate thread
  xTaskCreate(&wasm_task, "wasm3", NATIVE_STACK_SIZE, NULL, 5, NULL);
#else
  wasm_task(NULL);
#endif
}

void loop()
{
  delay(100);
}