# ![Greenshift](assets/project-greenshift-monotype-corsiva.svg)

A free, open-source Winamp visualization plugin designed to graph four-dimensional mathematical functions at high speed while offering greater flexibility than comparable visualizers.

[![Winamp](https://img.shields.io/badge/Winamp-F93821.svg?logo=winamp&logoColor=white)](https://www.winamp.com)
[![Windows](https://img.shields.io/badge/Windows-0078D4.svg?logo=data:image/svg%2bxml;base64,PD94bWwgdmVyc2lvbj0iMS4wIiBlbmNvZGluZz0idXRmLTgiPz48IS0tIE9yaWdpbmFsIGZyb206IFNWRyBSZXBvLCB3d3cuc3ZncmVwby5jb20sIEdlbmVyYXRvcjogU1ZHIFJlcG8gTWl4ZXIgVG9vbHM7IGhhbmQgbW9kaWZpZWQgdG8gd2hpdGUgbW9ub2Nocm9tZSAtLT4KPHN2ZyBmaWxsPSIjRkZGRkZGIiB3aWR0aD0iODAwcHgiIGhlaWdodD0iODAwcHgiIHZpZXdCb3g9IjAgMCA1MTIgNTEyIiBpZD0iaWNvbnMiIHhtbG5zPSJodHRwOi8vd3d3LnczLm9yZy8yMDAwL3N2ZyI+PHBhdGggZD0iTTMxLjg3LDMwLjU4SDI0NC43VjI0My4zOUgzMS44N1oiLz48cGF0aCBkPSJNMjY2Ljg5LDMwLjU4SDQ3OS43VjI0My4zOUgyNjYuODlaIi8+PHBhdGggZD0iTTMxLjg3LDI2NS42MUgyNDQuN3YyMTIuOEgzMS44N1oiLz48cGF0aCBkPSJNMjY2Ljg5LDI2NS42MUg0NzkuN3YyMTIuOEgyNjYuODlaIi8+PC9zdmc+)](https://www.microsoft.com/windows)
[![C++20](https://img.shields.io/badge/C%2B%2B-20-00599C.svg)](https://en.cppreference.com/w/cpp/20)
[![MMX supported](https://img.shields.io/badge/MMX-supported-0071C5.svg?logo=intel&logoColor=white)](https://en.wikipedia.org/wiki/MMX_(instruction_set))
[![DirectDraw 7](https://img.shields.io/badge/DirectDraw-7-107C10.svg)](https://en.wikipedia.org/wiki/DirectDraw)
[![License: GPL-2.0-only](https://img.shields.io/badge/License-GPL--2.0--only-F58220.svg)](LICENSE)


Greenshift was designed to achieve the performance of Geiss without resorting to assembly, and aims to surpass G-Force with smooth transitions between effects,
more palettes, and greater extensibility. It is a C++ project that provides a powerful engine for generating dynamic particle and wave visualizations
extended via user-defined mathematical expressions stored in external config files.

Originally conceived as a tool for 4D function graphing (X, Y, Z, T), it was adapted into its current form: an efficient, extensible Winamp visualization plugin.

## Key Features

* **4D Function Graphing:** Supports $y(x, z, t)$ where the third parameter is time, allowing for dynamic spatial-temporal visualizations.
* **Mathematically Rich Syntax:** Supports complex mathematical expressions, including hyperbolic trigonometric functions.
* **Dynamic Expression Engine:** Allows for an infinite range of behaviors extended via user-defined formulas in external text files, evaluated in real-time without recompilation.
* **Color Space Support:** Built-in support for RGB, HSV, HLS, CMY, CMYK, and OKLab palettes.
* **Efficient Rendering:** Designed to maintain high framerates while smoothly transitioning between precalculated delta fields — the code that moves every pixel on the screen.

## Controls (Hotkeys)

| Key | Action |
| :--- | :--- |
| `D` | Toggle display text |
| `F` | Toggle framerate display |
| `Esc` | Close Greenshift |
| `Alt + Enter` / Double Click | Toggle fullscreen |

## Getting Started

* **Platform:** Windows.
* **Usage:** Primarily used as a Winamp visualization engine.

## About this project's development

### The Name "Greenshift"
The name was inspired by a technical quirk discovered during development. When working in 16-bit color (5 bits Red, 6 bits Green, 5 bits Blue), repeatedly averaging the color channels of adjacent pixels leads to precision loss. Because green has one more bit of precision, it stays brighter slightly longer—causing a shift toward the green "end" of the spectrum. I dubbed this phenomenon "greenshift," and the name has stuck ever since.

### Design Philosophy: Preserving the "Feel"

The design philosophy behind Greenshift centers on an attempt to preserve the visual and behavioral "feel" of its configs, regardless of the hardware or system settings. This approach focuses on decoupling the artistic output from the technical implementation so that system upgrades don't fundamentally alter the character of the art. This includes using normalized ranges to maintain composition across various resolutions, narrowing precision during the RNG seeding process to keep random behaviors consistent when moving between 32-bit and 64-bit math, and normalizing line widths so that drawing scales naturally with screen resolution. The intent is that the character of each visualization remains stable and recognizable, even as system parameters are changed or improved.

## AI Policy

Contributions from AI agents are welcome, provided they are reviewed by a
human before being committed. Every change MUST be approved by a real person;
approval by an automated process or another AI agent alone is insufficient.

AI tools may be used to suggest code ideas or help draft comments, but all
code is reviewed by the project author before committing. Code that the
author does not fully understand is not committed.

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