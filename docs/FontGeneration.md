# Font Generation

## Purpose

This project uses TFT_eSPI Smooth Fonts (`.vlw`) to provide a clean, professional typography system for the Internet Radio user interface.

The generated font files are stored in:

```text
/data/fonts/
```

---

## Font Family

Google Roboto SemiCondensed

---

## Font Generator

The `.vlw` fonts were generated using:

- Processing IDE
- TFT_eSPI `Create_font.pde`

The TFT_eSPI font-generation utility can be found within the installed TFT_eSPI library under:

```text
Tools/Create_Smooth_Font/Create_font/
```

---

## Project Font Set

The Smooth Font files currently stored with the project are:

| File | Size |
|------|-----:|
| Roboto24.vlw | 24 pt |
| Roboto28.vlw | 28 pt |
| Roboto32.vlw | 32 pt |
| Roboto36.vlw | 36 pt |
| Roboto38.vlw | 38 pt |
| Roboto40.vlw | 40 pt |

The active typography mappings are defined in `10_UI_Typography.ino`.
`Roboto38.vlw` is retained in the font set but is not currently assigned to a UI typography role.

The font files are stored in:

```text
/data/fonts/
```

---

## Processing Settings

For each font generated:

* `fontType = ""`
* `fontSize = required size`
* `displayFontSize = same as fontSize`
* Unicode Block:

  * Basic Latin (`0x0021`–`0x007E`)
* Anti-aliased:

  * Enabled

The generated `.vlw` file is copied into:

```text
/data/fonts/
```

and renamed using the project's standard naming convention.

---

## Design Principle

Application code should never reference a specific font family.

All text rendering is performed through the Typography module (`10_UI_Typography.ino`) using semantic roles such as:

* Header
* Section
* Body
* Button
* Caption
* Status
* Display

This allows the font family, weight and point size to be changed in one location without modifying the application UI.
