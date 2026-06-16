# ZCapture

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
zcapture                    → Start as daemon (system tray)
zcapture gui                → Interactive screenshot
zcapture gui -c             → Interactive screenshot + copy to clipboard
zcapture gui -p ./screens   → Interactive screenshot + save to path
zcapture gui -d 3000        → Interactive screenshot with 3s delay
zcapture full               → Fullscreen screenshot
zcapture full -c            → Fullscreen screenshot + copy
zcapture config             → Open configuration window
```

## Tech Stack

- C++20
- Qt6 (Widgets)
- Windows API
- QHotkey
- CMake

## License

GPLv3
