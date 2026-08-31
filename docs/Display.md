# Display Hardware Notes

## Hardware

Display: 4" SPI TFT Touch Display

Touch Controller: XPT2046

Controller advertised by seller: ILI9488

Board used: ESP32-S3 N16R8

---

## TFT_eSPI Configuration

Working TFT_eSPI setup:

```cpp
#define USE_HSPI_PORT

#define ILI9481_DRIVER

#define TFT_MISO 13
#define TFT_MOSI 11
#define TFT_SCLK 12
#define TFT_CS   10
#define TFT_DC    9
#define TFT_RST   8
```

---

## Important Discovery

Although the display was advertised and silk-screened as an ILI9488 display, it would not operate correctly using:

```cpp
#define ILI9488_DRIVER
```

The display only operated correctly when configured as:

```cpp
#define ILI9481_DRIVER
```

This should be considered the known-good configuration for this display module.

---

## Touchscreen Discovery

For the XPT2046 touchscreen to function correctly:

```text
TFT SDO (MISO) must be disconnected
```

Working touch wiring:

```text
T_DO  -> GPIO13
T_DIN -> GPIO11
T_CLK -> GPIO12
T_CS  -> GPIO14
T_IRQ -> Not connected
```

If TFT SDO remains connected to GPIO13, the touchscreen reports:

```text
Touched = YES | X = 0 | Y = 0 | Z = 4095
```

continuously and does not respond to touch input.
