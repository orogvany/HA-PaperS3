# Home Assistant ePaper Remote

## Project Overview
ESP32-S3 based Home Assistant remote control using e-ink displays with capacitive touch. Targets **M5Paper S3** and **LilyGo T5 E-Paper S3 Pro** boards (both 540x960 resolution).

## Build & Deploy
- **Build system**: PlatformIO with Arduino framework on ESP-IDF
- **Build**: `pio run -e m5-papers3` or `pio run -e lilygo-t5-s3`
- **Upload**: `pio run -e <target> --target upload`
- **Icon generation**: `python3 generate-icons.py` (converts PNGs in `icons-buttons/` and `icons-ui/` to C header arrays in `src/assets/icons.h`)

## Configuration
1. Copy `src/config_remote.cpp.example` → `src/config_remote.cpp`
2. Set WiFi SSID/password, HA WebSocket URL, and long-lived access token in `src/config.h`
3. Declare entities and widgets in `configure_remote()`

## Architecture

### FreeRTOS Tasks
| Task | Stack | Role |
|------|-------|------|
| UI | 2048 | Display rendering (4BPP full / 1BPP partial) |
| WiFi | system | Event-driven WiFi state tracking |
| Home Assistant | 8192 | WebSocket client for HA communication |
| Touch | 4096 | GT911 capacitive touch polling |

### Key Source Layout
- `src/main.cpp` — Task creation and initialization
- `src/store.h/cpp` — Thread-safe central data store (mutexes, event groups, task notifications)
- `src/ui_state.h/cpp` — Shared UI state with version counter for change detection
- `src/managers/` — WiFi, Home Assistant (WebSocket), UI rendering, touch input
- `src/widgets/` — Widget base class, OnOffButton, Slider
- `src/config.h` — Credentials and SSL certs (gitignored)
- `src/boards.h` — Board-specific pin definitions (conditional compilation)
- `src/constants.h` — All magic numbers (timing, sizes, limits)

### Display Rendering
- Full 4BPP refresh on mode changes (CLEAR_SLOW for quality)
- 1BPP partial updates during interaction (damage rectangle accumulation)
- Full refresh forced every 5s to prevent ghosting (`DISPLAY_FULL_REDRAW_TIMEOUT_MS`)

### Widget System
- Max 8 widgets per screen (`MAX_WIDGETS_PER_SCREEN`)
- `OnOffButton`: Circular toggle with on/off icon states (100px + 15px touch margin)
- `Slider`: Horizontal 0-100% with left icon offset, continuous touch tracking

### Home Assistant Integration
- WebSocket protocol: connect → auth → subscribe to entity state changes
- Command rate limiting: 500ms between commands (`HASS_TASK_SEND_DELAY_MS`) to prevent Zigbee flooding
- Update debouncing: 1s ignore window after local command to prevent jitter
- Reconnection backoff: 10s on failure

## Key Constants (src/constants.h)
- `MAX_ENTITIES` / `MAX_WIDGETS_PER_SCREEN`: 8
- `HASS_MAX_JSON_BUFFER`: 20KB
- `TOUCH_RELEASE_TIMEOUT_MS`: 50ms
- Touch polling: 25ms active, 200ms idle

## Dependencies
- **FastEPD** — E-ink display abstraction
- **bb_captouch** (v1.3.2) — GT911 capacitive touch driver
- **cJSON** — JSON parsing for HA WebSocket messages
- **Arduino/ESP-IDF** — Core framework with FreeRTOS

## Current Power Profile (Known Weakness)
- WiFi is always-on (WebSocket stays connected)
- Display power permanently enabled (`einkPower(true)` — disabling breaks GT911 touch)
- No deep sleep, light sleep, or wake-on-touch implemented
- README states "only lasts a few hours on battery"

## Development Goals
- **Battery optimization**: Investigate ESP32-S3 best practices for:
  - WiFi controller sleep/wake patterns (poll instead of persistent connection)
  - CPU light sleep / deep sleep with wake-on-touch via GT911 interrupt (pin 48 on M5Paper S3)
  - Disabling unused peripherals and sensors
  - Display power management that doesn't break touch
- **UI/UX improvements**: Feature additions and interaction refinements
- **Feature additions**: New widget types, multi-screen support, etc.