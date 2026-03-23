# Home Assistant ePaper remote

e-Ink remote for Home Assistant built with [FastEPD](https://github.com/bitbank2/FastEPD).

![Preview](./preview.jpg)

It uses the REST API of Home Assistant, no plugin is required on the server.

## Hardware supported

- [Lilygo T5 E-Paper S3 Pro](https://lilygo.cc/products/t5-e-paper-s3-pro)
- [M5Stack M5Paper S3](https://docs.m5stack.com/en/core/PaperS3)

Note: this is really only tested and developed for the M5Paper S3, lilygo may work, but its diverged quite a bit from
when it was tested.

## Setup

You will need to install [PlatformIO](https://platformio.org/) to compile the project.

### 1. Generate icons

The project includes 38 device icons (Cupertino style) and 15 weather icons (Meteocons monochrome), organized by category:

- `icons-buttons/` — Device icons (on/off pairs, e.g. `lightbulb.png` / `lightbulb-off.png`)
- `icons-weather/` — Weather condition icons
- `icons-chrome/` — UI chrome (gear icon)
- `icons-statusbar/` — Status bar indicators (WiFi, HA, Alexa disconnected)
- `icons-ui/` — Full-screen UI icons (alerts, logos)

To regenerate `src/assets/icons.h` and the web UI icon manifest:

```
pip install Pillow cairosvg
python3 generate-icons.py
python3 scripts/generate_icon_manifest.py
```

To add custom icons, place 256x256 PNGs in the appropriate folder. Naming convention: `{name}.png` for on/active state, `{name}-off.png` for off/inactive state.

### 2. Get a Home Assistant token

In Home Assistant:

- Click on your username in the bottom left
- Go to "security"
- Click on "Create Token" in the "Long-lived access tokens" section
- Note the token generated

### 3. Update configuration

TODO - this is going away in favor of dynamic config.

Copy the example config and fill in your credentials:

```
cp src/config_remote.cpp.example src/config_remote.cpp
```

Edit `src/config_remote.cpp` with your WiFi SSID/password, Home Assistant URL, token, and entity configuration.

### 4. Build and upload

```
# For M5Paper S3
pio run -e m5-papers3
pio run -e m5-papers3 --target upload

# For Lilygo T5 E-Paper S3 Pro (untested)
pio run -e lilygo-t5-s3
pio run -e lilygo-t5-s3 --target upload
```

## Features

- On-screen battery percentage indicator (M5Paper S3)
- REST connection to Home Assistant for real-time entity state updates (Websocket updates were too slow)
- Slider widgets for dimmable lights and fan speed
- Toggle button widgets for switches and automations
- Partial display updates for responsive touch interaction
- Better power management for long battery life
- Captive portal for WIFI connection and Web driven configuration for device discovery and management
- Optional pin gating for configuration (default:1234)

### Updating the font

The font used is Montserrat Regular in size 26, it was converted
using [fontconvert from FastEPD](https://github.com/bitbank2/FastEPD/tree/main/fontconvert):

```
./fontconvert Montserrat-Regular.ttf `src/assets/Montserrat_Regular_26.h` 26 32 126
```

## License

[This project is released under Apache License 2.0.](./LICENSE)

See [NOTICE.md](./NOTICE.md) for full credits and third-party notices.

Note: This is vibe coded, dictated but not read.  Expect much AI Slop(tm).  Claude authored. YMMV.
