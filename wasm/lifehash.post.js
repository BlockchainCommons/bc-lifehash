/// <reference types="emscripten" />

Module['onRuntimeInitialized'] = function () {
    Module['free'] = cwrap('free', null, ['number']);
    Module['malloc'] = cwrap('malloc', 'number', ['number']);
    // Module['intSqrt'] = cwrap('int_sqrt', 'number', ['number']);
    // Module['add'] = cwrap('add', 'number', ['number', 'number']);
    // Module['printU8'] = cwrap('print_u8', null, ['number']);
    // Module['incrementU8'] = cwrap('increment_u8', 'number', ['number']);
    // Module['printU16'] = cwrap('print_u16', null, ['number']);
    // Module['printU8Array'] = function(a) {
    //     ccall('print_u8_array', null, ['array', 'number'], [a, a.length]);
    // };
    // Module['returnString'] = function() {
    //     const ptr = ccall('return_string', 'number', []);
    //     const result = UTF8ToString(ptr);
    //     this.free(ptr);
    //     return result;
    // };
    // Module['reverse'] = function(a) {
    //     const ptr = this.malloc(a.length * 4);
    //     const base = ptr / 4;
    //     const aa = new Int32Array(a);
    //     for(let i = 0; i < a.length; i++) {
    //         HEAP32[base + i] = aa[i];
    //     }
    //     ccall('reverse', null, ['number', 'number'], [ptr, a.length]);
    //     var result = [];
    //     for(let i = 0; i < a.length; i++) {
    //         result.push(HEAP32[base + i]);
    //     }
    //     this.free(ptr);
    //     return result;
    // };
    // Module['sha256'] = function(s) {
    //     const utf8 = new TextEncoder().encode(s);
    //     const inputPtr = this.malloc(utf8.length);
    //     const i = new Uint8Array(HEAPU8.buffer, inputPtr, utf8.length);
    //     i.set(utf8);
    //     const outputPtr = this.malloc(32);
    //     ccall('sha256', null, ['number', 'number', 'number'], [inputPtr, utf8.length, outputPtr]);
    //     const o = new Uint8Array(new ArrayBuffer(32));
    //     o.set(new Uint8Array(HEAPU8.buffer, outputPtr, 32));
    //     this.free(inputPtr);
    //     this.free(outputPtr);
    //     return o;
    // };
    // Module['dataToHex'] = function(data) {
    //     const inputPtr = this.malloc(data.length);
    //     const i = new Uint8Array(HEAPU8.buffer, inputPtr, data.length);
    //     i.set(data);
    //     const outputPtr = ccall('data_to_hex', 'number', ['number', 'number'], [inputPtr, data.length]);
    //     const result = UTF8ToString(outputPtr);
    //     this.free(inputPtr);
    //     this.free(outputPtr);
    //     return result;
    // };
    // Module['hexToData'] = function(hex) {
    //     const utf8 = new TextEncoder().encode(hex);
    //     const inputPtr = this.malloc(utf8.length);
    //     const i = new Uint8Array(HEAPU8.buffer, inputPtr, utf8.length);
    //     i.set(utf8);

    //     const outputPtrPtr = this.malloc(4);
    //     const outputLenPtr = this.malloc(4);
    //     const outputPtr = 0;

    //     const success = ccall('hex_to_data', 'boolean', ['number', 'number', 'number', 'number'], [inputPtr, utf8.length, outputPtrPtr, outputLenPtr]);

    //     let result = null;
    //     if(success) {
    //         const outputLen = new Uint32Array(HEAPU8.buffer, outputLenPtr, 1)[0];
    //         const outputPtr = new Uint32Array(HEAPU8.buffer, outputPtrPtr, 1)[0];
    //         const output = new Uint8Array(HEAPU8.buffer, outputPtr, outputLen);
    //         result = new Uint8Array(new ArrayBuffer(outputLen));
    //         result.set(output);
    //     }

    //     this.free(inputPtr);
    //     this.free(outputLenPtr);
    //     this.free(outputPtr);
    //     this.free(outputPtrPtr);
    //     return result;
    // };
}
