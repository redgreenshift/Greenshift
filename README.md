# Greenshift

A high-performance visualization engine with 4D graphing capabilities.

Greenshift was designed to achieve the speed of Geiss without resorting to assembly, and attempted to outdo G-Force with smooth transitions between effects. It is a C++ project that provides a powerful engine for generating dynamic particle and wave visualizations via user-defined mathematical expressions.

Originally conceived as a tool for 4D function graphing (X, Y, Z, T), it was adapted into its current form: an efficient Winamp visualization plugin.

## Key Features

* **4D Function Graphing:** Supports $y(x, z, t)$ where the third parameter is time, allowing for dynamic spatial-temporal visualizations.
* **Mathematically Rich Syntax:** Supports complex mathematical expressions, including hyperbolic trigonometric functions.
* **Dynamic Expression Engine:** Allows for an infinite range of behaviors via user-defined formulas in external text files, evaluated in real-time without needing to recompile.
* **Color Space Support:** Built-in support for RGB, HSV, HLS, and CMY palettes.
* **Efficient Rendering:** Designed to maintain high framerates when switching between pre-calculated delta fields.

## Controls (Hotkeys)

| Key | Action |
| :--- | :--- |
| `D` | Toggle display text |
| `F` | Toggle framerate display |
| `Esc` | Close Greenshift |
| `Alt + Enter` / Double Click | Toggle fullscreen |

## Known Information

### The Name "Greenshift"
The name was inspired by a technical quirk discovered during development. When working in 16-bit color (5 bits Red, 6 bits Green, 5 bits Blue), repeatedly averaging the color channels of adjacent pixels leads to precision loss. Because green has one more bit of precision, it stays brighter slightly longer—causing a shift toward the green "end" of the spectrum. I dubbed this phenomenon "greenshift," and the name has stuck ever since.

## Getting Started

* **Platform:** Windows.
* **Usage:** Primarily used as a Winamp visualization engine.

## About this project's development

Contributions from AI agents are welcome so long as they're reviewed by humans before committing — all changes MUST be approved by a real person, not merely accepted by an automated process or another agent.

## License

Greenshift is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; version 2 only.

Greenshift is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program; see LICENSE.

Greenshift uses the MersenneTwister library (Richard J. Wagner), which is licensed separately under the GNU Lesser General Public License, version 2.1 (or later). See `MersenneTwister.h` and `licenses/LGPL-2.1-or-later.txt` for details.

---

## Contact & Links

* [GitHub Repository](https://github.com/redgreenshift/greenshift)
* [Website](http://greenshift.net)
* **Author:** [Jared Ivey](mailto:jared.ivey+greenshift@outlook.com)