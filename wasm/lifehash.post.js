/// <reference types="emscripten" />

Module['onRuntimeInitialized'] = function () {
    Module['allocString'] = function(s) {
        const utf8 = new TextEncoder().encode(s);
        const sPtr = this.malloc(utf8.length + 1);
        const i = new Uint8Array(HEAPU8.buffer, sPtr, utf8.length);
        i.set(utf8);
        HEAPU8[sPtr + utf8.length] = 0;
        return sPtr;
    };

    Module['produceImage'] = function(imagePtr, version) {
        const imageStruct = new Uint32Array(HEAPU8.buffer, imagePtr, 3);
        const width = imageStruct[0];
        const height = imageStruct[1];
        const inColorsPtr = imageStruct[2];
        const colorsLength = width * height * 4;
        const inColors = new Uint8Array(HEAPU8.buffer, inColorsPtr, colorsLength);
        const outColors = new Uint8ClampedArray(new ArrayBuffer(colorsLength));
        outColors.set(inColors);
        const canvas = document.createElement('canvas');
        const context = canvas.getContext('2d');
        canvas.width = width;
        canvas.height = height;
        const imageData = context.createImageData(width, height);
        imageData.data.set(outColors);
        context.putImageData(imageData, 0, 0);
        const dataURI = canvas.toDataURL();
        const image = new Image();
        const scale = version === 2 ? 0.5 : 1.0;
        image.width = width * scale;
        image.height = height * scale;
        image.src = dataURI;
        return image;
    }

    Module['makeFromUTF8'] = function(s, version, moduleSize) {
        const inputPtr = this.allocString(s);
        const imagePtr = ccall('lifehash_make_from_utf8', 'number', ['number', 'number', 'number', 'boolean'], [inputPtr, version, moduleSize, true]);
        const image = this.produceImage(imagePtr, version);
        this.free(inputPtr);
        this.imageFree(imagePtr);
        return image;
    }

    Module['makeFromData'] = function(data, version, moduleSize) {

    }
    Module['makeFromDigest'] = function(digest, version, moduleSize) {

    }
    Module['free'] = cwrap('free', null, ['number']);
    Module['malloc'] = cwrap('malloc', 'number', ['number']);
    Module['imageFree'] = cwrap('lifehash_image_free', null, ['number']);
    Module['sha256'] = function(s) {
        const utf8 = new TextEncoder().encode(s);
        const inputPtr = this.malloc(utf8.length);
        const i = new Uint8Array(HEAPU8.buffer, inputPtr, utf8.length);
        i.set(utf8);
        const outputPtr = this.malloc(32);
        ccall('lifehash_sha256', null, ['number', 'number', 'number'], [inputPtr, utf8.length, outputPtr]);
        const o = new Uint8Array(new ArrayBuffer(32));
        o.set(new Uint8Array(HEAPU8.buffer, outputPtr, 32));
        this.free(inputPtr);
        this.free(outputPtr);
        return o;
    };
    Module['dataToHex'] = function(data) {
        const inputPtr = this.malloc(data.length);
        const i = new Uint8Array(HEAPU8.buffer, inputPtr, data.length);
        i.set(data);
        const outputPtr = ccall('lifehash_data_to_hex', 'number', ['number', 'number'], [inputPtr, data.length]);
        const result = UTF8ToString(outputPtr);
        this.free(inputPtr);
        this.free(outputPtr);
        return result;
    };
    Module['hexToData'] = function(hex) {
        const utf8 = new TextEncoder().encode(hex);
        const inputPtr = this.malloc(utf8.length);
        const i = new Uint8Array(HEAPU8.buffer, inputPtr, utf8.length);
        i.set(utf8);

        const outputPtrPtr = this.malloc(4);
        const outputLenPtr = this.malloc(4);
        const outputPtr = 0;

        const success = ccall('lifehash_hex_to_data', 'boolean', ['number', 'number', 'number', 'number'], [inputPtr, utf8.length, outputPtrPtr, outputLenPtr]);

        let result = null;
        if(success) {
            const outputLen = new Uint32Array(HEAPU8.buffer, outputLenPtr, 1)[0];
            const outputPtr = new Uint32Array(HEAPU8.buffer, outputPtrPtr, 1)[0];
            const output = new Uint8Array(HEAPU8.buffer, outputPtr, outputLen);
            result = new Uint8Array(new ArrayBuffer(outputLen));
            result.set(output);
        }

        this.free(inputPtr);
        this.free(outputLenPtr);
        this.free(outputPtr);
        this.free(outputPtrPtr);
        return result;
    };
}
