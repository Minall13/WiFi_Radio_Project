# Touchscreen Hardware Notes

## Hardware

Touch Controller: XPT2046

Display Module: 4" SPI TFT Touch Display

Processor: ESP32-S3 N16R8

---

## Working Wiring

| Touch Pin | ESP32-S3 Pin  |
| --------- | ------------- |
| T_DO      | GPIO13        |
| T_DIN     | GPIO11        |
| T_CLK     | GPIO12        |
| T_CS      | GPIO14        |
| T_IRQ     | Not connected |

---

## Critical Discovery

The TFT and touch controller cannot both drive GPIO13.

The TFT SDO(MISO) connection must be disconnected.

```text
TFT SDO(MISO) = Not Connected
Touch T_DO    = GPIO13
```

If TFT SDO remains connected:

```text
Touched = YES | X = 0 | Y = 0 | Z = 4095
```

is reported continuously and the touch screen does not respond.

---

## Calibration Values

```cpp
#define TOUCH_MIN_X 300
#define TOUCH_MAX_X 3820
#define TOUCH_MIN_Y 420
#define TOUCH_MAX_Y 3770
```

---

## Coordinate Mapping

Landscape orientation:

```cpp
int screenX = map(p.x, TOUCH_MIN_X, TOUCH_MAX_X, 0, 480);
int screenY = map(p.y, TOUCH_MIN_Y, TOUCH_MAX_Y, 0, 320);
```
