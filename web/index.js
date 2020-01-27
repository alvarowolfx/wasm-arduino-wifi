import "babel-polyfill"

let source = 'as.wasm'
let ipAddr = '127.0.0.1'
let status = 'offline'

let worker = null

const serialOutput = document.getElementById('serial')
const ssidOutput = document.getElementById('ssid')
const ledOutput = document.getElementById('led')     

const sourceSelect = document.getElementById('source')
const statusSelect = document.getElementById('wifiStatus')
const ipAddrInput = document.getElementById('ipaddr')   

sourceSelect.onchange = function(event){
  console.log('Source', event.target.value)
  source = event.target.value
  start()
}

statusSelect.onchange = function(event){
  console.log('Status', event.target.value)
  status = event.target.value
  worker.postMessage({ type : 'setStatus', payload : status })  
}

ipAddrInput.onblur = function(event){
  console.log('ipAddr', event.target.value)
  ipAddr = event.target.value
  worker.postMessage({ type : 'setIpAddr', payload : ipAddr })
}

async function start(){
  if(worker){
    worker.terminate()
    ssidOutput.value = ''
    statusSelect.value = 'offline'
  }

  worker = new Worker('./worker.js')
  
  const wasmModule = await createWebAssembly(source);
  worker.postMessage({ type : 'init', payload : { module : wasmModule, ipAddr, status } })
  worker.postMessage({ type : 'setStatus', payload : status })
  worker.postMessage({ type : 'setIpAddr', payload : ipAddr })

  worker.onmessage = (msg) => {
    //console.log('main on msg', msg);
    switch(msg.data.type) {
      case 'digitalWrite': {
        const { pin , value } = msg.data.payload
        ledOutput.textContent = value ? 'ON' : 'OFF'
        //ledOutput.style['color'] = value ? 'black' : 'white'
        ledOutput.style['background-color'] = value ? 'green' : 'black' 
        break;
      }  
      case 'print' : {
        const { output } = msg.data.payload
        const finalOutput = output.replace("\n", "<br/>")
        serialOutput.innerHTML += finalOutput
        serialOutput.scrollTo(0,serialOutput.scrollHeight);
        break;
      }
      case 'wifiConnect' : {
        const { ssid } = msg.data.payload        
        ssidOutput.value = ssid
        statusSelect.value = 'online'
        break;
      }
    }
  }  
}

async function createWebAssembly(path) {    
  const result = await window.fetch(path);
  const bytes = await result.arrayBuffer();
  //const txt = await result.text();
  //console.log(txt)
  return WebAssembly.compile(bytes);
}

start()