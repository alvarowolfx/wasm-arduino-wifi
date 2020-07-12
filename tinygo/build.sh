# Prepare
#export PATH=/usr/local/tinygo/bin:$PATH
#export GOROOT=/opt/go

# Compile
tinygo  build -target wasm --no-debug               \
        -panic trap -wasm-abi generic     \
        -ldflags="-z stack-size=1024 --max-memory=65536" \
        -heap-size 1024 \
        -o app.wasm app.go

# Optimize (optional)
#wasm-opt -O3 app.wasm -o app.wasm
#wasm-strip app.wasm

# Convert to WAT
#wasm2wat app.wasm -o app.wat

# Convert to C header
xxd -i app.wasm > app.wasm.h

cp app.wasm ../data/app.wasm