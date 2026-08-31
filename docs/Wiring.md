# Hardware Wiring

======================================

## TFT Pin Connections

======================================

| TFT Signal | TFT Pin | ESP32-S3 Pin  | Notes                    |
| ---------- | ------- | ------------- | ------------------------ |
| VCC        | 14      | 3V3           | 3.3V supply              |
| GND        | 13      | GND           | Ground                   |
| CS         | 12      | GPIO10        | TFT Chip Select          |
| RESET      | 11      | GPIO8         | Hardware Reset           |
| DC / RS    | 10      | GPIO9         | Data / Command           |
| MOSI (SDI) | 9       | GPIO11        | SPI Data Out             |
| SCK        | 8       | GPIO12        | SPI Clock                |
| LED        | 7       | 3V3           | Backlight permanently on |
| MISO (SDO) | 6       | Not Connected | Display not read         |
| T_CLK      | 5       | GPIO12        | Shared SPI Clock         |
| T_CS       | 4       | GPIO14        | Touch CS                 |
| T_DIN      | 3       | GPIO11        | Shared MOSI              |
| T_DO       | 2       | GPIO13        | Touch MISO               |
| T_IRQ      | 1       | Not Connected | Interrupt unused         |

======================================

## MAX98357A Pin Connections

======================================

| MAX98357A | ESP32-S3 |
|-----------|----------|
| DIN       | GPIO5    |
| BCLK      | GPIO6    |
| LRC        | GPIO7    |
| VIN       | 5V        |
| GND       | GND       |
| SD        | Floating    |
| GAIN      | 3.3V   |

> The installed MAX98357A V914 breakout modules require `GAIN` connected
> to 3.3 V. Leaving `GAIN` floating produced no output, and mixed gain
> configurations caused severe distortion. Both amplifiers therefore use
> the same 3.3 V gain connection.
