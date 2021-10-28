/// <reference types="emscripten" />

export interface LifeHashModule extends EmscriptenModule {
    // intSqrt(x: number): number;
    // add(a: number, b: number): number;
    // printU8(n: number): void;
    // incrementU8(n: number): void;
    // printU16(n: number): void;
    // printU8Array(a: number[]): void;
    // returnString(): string;
    // reverse(a: number[]): number[];
    // sha256(s: string): Uint8Array;
    // dataToHex(data: Uint8Array): string;
    // hexToData(hex: string): Uint8Array|null;
}

export default function instantiate_lifehash(mod?: any): Promise<LifeHashModule>;
