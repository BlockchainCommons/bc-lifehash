# LifeHash Test Vectors Format

## Overview

The file `test-vectors.json` contains an array of test vectors for validating LifeHash implementations. Each vector specifies an input and its parameters, along with the expected output image dimensions and pixel data.

## JSON Structure

The file is a JSON array of objects. Each object has the following fields:

| Field | Type | Description |
|-------|------|-------------|
| `input` | string | The input value. Interpretation depends on `input_type`. |
| `input_type` | string | Either `"utf8"` or `"hex"`. See Input Types below. |
| `version` | string | The LifeHash version used. One of: `"version1"`, `"version2"`, `"detailed"`, `"fiducial"`, `"grayscale_fiducial"`. |
| `module_size` | integer | The module (pixel) size. `1` produces the base resolution; `2` doubles width and height. |
| `has_alpha` | boolean | Whether an alpha channel is included in the output. |
| `width` | integer | The width of the output image in pixels. |
| `height` | integer | The height of the output image in pixels. |
| `colors` | array of integers | Flattened pixel data as unsigned bytes (0-255). |

## Input Types

| Type | Description |
|------|-------------|
| `utf8` | The `input` field is a UTF-8 string, passed to `make_from_utf8`. |
| `hex` | The `input` field is a hex-encoded byte sequence (e.g. `"00ff80"`), decoded to raw bytes and passed to `make_from_data`. |

## Pixel Data Layout

The `colors` array contains pixel data in row-major order (left to right, top to bottom).

- When `has_alpha` is `false`: each pixel is 3 bytes (R, G, B). The array length is `width * height * 3`.
- When `has_alpha` is `true`: each pixel is 4 bytes (R, G, B, A). The array length is `width * height * 4`.

## Versions

| Version | Description |
|---------|-------------|
| `version1` | Deprecated. Uses HSB color gamut. |
| `version2` | Recommended default. Uses a CMYK-friendly color gamut. |
| `detailed` | Double resolution. CMYK-friendly gamut. |
| `fiducial` | Optimized for machine-vision fiducials. High contrast, CMYK-friendly. |
| `grayscale_fiducial` | Grayscale variant of fiducial. High contrast. |

## Generating

To regenerate the test vectors:

```sh
cd test
make generate-test-vectors-json
```

Or equivalently:

```sh
cd test
make generate-test-vectors
./generate-test-vectors > test-vectors.json
```

The output is deterministic. Running the generator twice produces identical output.
