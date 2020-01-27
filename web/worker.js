
const memory = new WebAssembly.Memory({initial: 256, maximum: 2048});
let exports = null;
let linearMemory = null;

let defaultStatus = 'offline'
let defaultIpAddr = '127.0.0.1'

function init(wasmModule, opts) {
  ipAddr = (opts && opts.ipAddr) || defaultIpAddr
  status = defaultStatus
  const env = {
    'abortStackOverflow': _ => { throw new Error('overflow'); },
    'table': new WebAssembly.Table({initial: 0, maximum: 0, element: 'anyfunc'}),
    'tableBase': 0,
    'memory': memory,
    'memoryBase': 1024,
    'STACKTOP': 0,
    'STACK_MAX': memory.buffer.byteLength,
    'io_get_stdout' : () => {}
  }; 

  const arduino = {
    digitalWrite : (pin, value ) => {      
      postMessage({ type : 'digitalWrite', payload : { pin, value }})      
    },
    millis: () => {
      return Date.now() % 10000000
    },
    delay: (ms) => {      
      //postMessage({ type : 'delay', payload : { ms }})      
    },
    pinMode: (pin, mode) => {      
      postMessage({ type : 'pinMode', payload : { pin, mode }})
    },
    getPinLED: () => {
      return 42  
    },
  }
    
  const utf8decoder = new TextDecoder()
  const utf8encoder = new TextEncoder()

  const serial = {
    print: (index, len) => {                     
      const stringBuffer = new Uint8Array(linearMemory.buffer, index,len);      
      const output = utf8decoder.decode(stringBuffer);      
      postMessage({ type : 'print', payload : { output }})      
    },
    printInt: (value) => {
      postMessage({ type : 'print', payload : { output: String(value) }})
    }
  }

  const wifi = {
    wifiStatus: () => {      
      return status === 'online' ? 
        0x3 : // WL_CONNECTED 
        0x6 // WL_DISCONNECTED
    },
    wifiConnect: (ssid_index, ssid_len, password_index, pass_len) => {
      //console.log('Wifi Connect', ssid, pass)
      const ssidBuffer = new Uint8Array(linearMemory.buffer, ssid_index,ssid_len);
      const passBuffer = new Uint8Array(linearMemory.buffer, password_index,pass_len);
      const ssid = utf8decoder.decode(ssidBuffer);
      const password = utf8decoder.decode(passBuffer);
      status = 'online'
      postMessage({ type : 'wifiConnect', payload : { ssid, password }})
    },
    printWifiLocalIp : () => {
      const output = ipAddr
      postMessage({ type : 'print', payload : { output }})
    },
    wifiLocalIp: (index) => {
      const stringBuffer = new Uint8Array(linearMemory.buffer, index, 16);      
      utf8encoder.encodeInto(ipAddr, stringBuffer)      
    }
  }
  
  const importObject = { env, arduino, serial, wifi, memory };
  WebAssembly.instantiate(wasmModule, importObject)
    .then( wasmInstance => {
      console.log(wasmInstance)      
      exports = wasmInstance.exports
      linearMemory = exports.memory;
      console.info('got exports', exports, wasmInstance);
      if(exports.cwa_main){
        exports.cwa_main();
      }else{
        exports._start();
      }    
    }) 
}

console.log('init worker')

setStatus = function(newStatus){
  status = newStatus
}

onmessage = function(msg){
  console.log('on msg worker', msg)
  if(msg.data.type === 'init'){
    init(msg.data.payload.module, msg.data.payload)
  }
  if(msg.data.type === 'setIpAddr'){
    ipAddr = msg.data.payload
  }
  if(msg.data.type === 'setStatus'){
    status = msg.data.payload
  }
}