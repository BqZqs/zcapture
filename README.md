# PixelShot

> A lightweight screenshot tool for Windows, inspired by Flameshot.

## Features

- Region / Fullscreen / Monitor screenshot
- Built-in image editor (rectangle, arrow, text, pencil, marker, pixelate)
- Undo / Redo support
- Save to file / clipboard
- Pin to desktop
- System tray daemon mode
- Global hotkey support
- Portable, no installation required

## Usage

```
pixelshot                    → Start as daemon (system tray)
pixelshot gui                → Interactive screenshot
pixelshot gui -c             → Interactive screenshot + copy to clipboard
pixelshot gui -p ./screens   → Interactive screenshot + save to path
pixelshot gui -d 3000        → Interactive screenshot with 3s delay
pixelshot full               → Fullscreen screenshot
pixelshot full -c            → Fullscreen screenshot + copy
pixelshot config             → Open configuration window
```

## Tech Stack

- C++20
- Qt6 (Widgets)
- Windows API
- QHotkey
- CMake

## License

GPLv3
