# PMS150G Power Controller & BM8563 RTC Research

**Status**: Research complete — implementation pending (Phase 4)
**Created**: 2026-03-18

---

## Hardware Wiring (from M5PaperS3 official pin map)

| Component | SDA (G41) | SCL (G42) | G48 | PMS150G PA6/CIN- | G3 | G21 |
|---|---|---|---|---|---|---|
| GT911 (touch) | SDA | SCL | INT | | | |
| BM8563 (RTC) | SDA | SCL | | **INT** | | |
| Battery Detect | | | | | ADC_VBAT | |
| Buzzer | | | | | | BUZ_PWM |

**Critical finding**: The BM8563 RTC INT pin is wired directly to the PMS150G's PA6/CIN- input. This confirms the RTC alarm CAN trigger the PMS150G to power the device back on.

### Other key pins
- **GPIO 44**: PWROFF_Pulse — sends pulse to PMS150G to power off
- **GPIO 45**: EPD PWR — e-ink display power rail
- **GPIO 5**: USB_DET — USB power supply detection (>0.2V = USB connected)
- **BMI270 INT1**: Connected to PMS150G via E_TRIG (not to any ESP32 GPIO) — IMU motion can also trigger power-on

---

## PMS150G Behavior

- **OTP (One-Time Programmable)** — firmware cannot be changed
- Uses ~few µA when running, never turns off itself
- **Power-off**: Requires pulse on GPIO 44 (PWROFF_Pulse). M5Unified toggles it high/low with 50ms delays, 5 times. Simply holding it low does a reset, the pulse pattern does a power-off.
- **Power-on triggers**:
  1. Physical button press (side button)
  2. BM8563 RTC alarm (via INT → PA6/CIN-)
  3. BMI270 IMU motion (via INT1 → E_TRIG)
- **Power consumption when off**: ~9.28µA (only PMS150G + RTC + IMU standby drawing power)

Source: [M5Unified Power_Class.cpp](https://github.com/m5stack/M5Unified/blob/master/src/utility/Power_Class.cpp), [M5Stack community thread](https://community.m5stack.com/topic/7108/paper-s3-questions/14)

---

## BM8563 RTC

- **I2C address**: 0x51
- **Shares I2C bus** with GT911 and BMI270 (SDA=G41, SCL=G42)
- **Timer**: 8-bit counter, max 255 intervals. At 1/60 Hz frequency = 255 minutes (~4.25 hours) maximum
- **Alarm**: Supports minute/hour/day/weekday alarm via `setAlarmIRQ()`
- **INT output**: Active low, wired to PMS150G PA6/CIN-

### Timer vs Alarm
- `setAlarmIRQ(int seconds)` — uses the timer function, works reliably, max ~4.25 hours
- `setAlarmIRQ(date, time)` — uses the alarm function, reportedly unreliable in M5Unified (community reports it doesn't work)

Source: [M5Stack community](https://community.m5stack.com/topic/7176/has-anyone-gotten-m5-rtc-setalarmirq-to-work-on-esp32s3-for)

---

## M5Unified Library Status

### What works
- `M5.Power.timerSleep(int seconds)` — sets RTC timer, then enters ESP32 deep sleep. Device wakes on RTC alarm. BUT stays in deep sleep (5.1mA) rather than true power-off (9.28µA).
- `M5.Power.powerOff()` — sends pulse on GPIO 44 to trigger PMS150G power-off. Gets to ~9.6µA. BUT doesn't set up RTC alarm first, so no timed wake.

### What doesn't work (as of Jan 2025 community reports)
- `timerSleep()` calling `_powerOff(true)` — the `_powerOff` path that should trigger PMS150G was reportedly incomplete, falling back to ESP32 deep sleep
- `setAlarmIRQ()` in date/time form — unreliable

### What should work (untested approach)
Since we now know the RTC INT is wired to PMS150G PA6/CIN-, the correct sequence should be:
1. Configure BM8563 timer alarm via I2C (address 0x51)
2. Send power-off pulse on GPIO 44
3. PMS150G cuts power → 9.28µA
4. After N hours, RTC timer fires → INT goes low → PMS150G PA6/CIN- triggers → power on
5. ESP32 boots fresh

This can be done **without M5Unified** — just direct I2C writes to BM8563 and GPIO toggle on pin 44. No library dependency needed.

### PR #141
felmue created [PR #141](https://github.com/m5stack/M5Unified/pull/141) to address the power-off functionality gap. Status of merge unknown.

Source: [M5Stack community thread](https://community.m5stack.com/topic/7108/paper-s3-questions/14), [Slow battery thread](https://community.m5stack.com/topic/8087/slow-battery-performance-with-m5papers3)

---

## Power Consumption Summary

| State | Current | Battery Life (1800mAh) |
|---|---|---|
| Operating (WiFi + CPU active) | ~154 mA | ~12 hours |
| Our Phase 2 (light sleep + modem sleep) | ~8-10 mA | ~7-9 days |
| Our Phase 3 (WiFi off, light sleep) | ~6 mA | ~12 days |
| ESP32 deep sleep (main power on) | ~5.1 mA | ~15 days |
| **PMS150G power-off** | **~9.28 µA** | **~22,000 hours (~2.5 years)** |

---

## Implementation Plan for Phase 4

### Direct approach (no M5Unified dependency)

```
// 1. Set RTC timer (e.g., 4 hours = 240 minutes)
Wire.beginTransmission(0x51);
Wire.write(0x01);  // Control_Status_2 — clear alarm flag
Wire.write(0x00);
Wire.endTransmission();

Wire.beginTransmission(0x51);
Wire.write(0x0E);  // Timer_Control register
Wire.write(0x83);  // Enable timer, 1/60 Hz (1 tick = 1 minute)
Wire.endTransmission();

Wire.beginTransmission(0x51);
Wire.write(0x0F);  // Timer register
Wire.write(240);   // 240 minutes = 4 hours
Wire.endTransmission();

// 2. Enable timer interrupt
Wire.beginTransmission(0x51);
Wire.write(0x01);  // Control_Status_2
Wire.write(0x01);  // TIE = 1 (timer interrupt enable)
Wire.endTransmission();

// 3. Display "Press button to wake" on e-ink

// 4. Power off via PMS150G
for (int i = 0; i < 5; i++) {
    digitalWrite(44, LOW);
    delay(50);
    digitalWrite(44, HIGH);
    delay(50);
}
```

### Feature Flags (constants.h)
- `PMS150G_AUTO_SHUTDOWN_ENABLED` — master toggle (default: true). When false, device stays in Phase 3 light-sleep forever.
- `PMS150G_SHUTDOWN_IDLE_MS` — idle time before auto-shutdown (default: 6 hours)
- `PMS150G_RTC_WAKE_INTERVAL_MIN` — RTC timer wake interval for idle screen refresh (default: 240 minutes / 4 hours)
- All documented in README with explanation of behavior when enabled/disabled.

### Idle Screen Integration
Before power-off, display the idle screen (clock, weather, or custom content — designed separately, see `plans/power-management-overhaul.md` "Future: Idle Screen"). On each RTC wake cycle (every ~4 hours), refresh the idle screen with updated content (time from RTC, potentially cached weather), then power off again. The e-ink retains the image between power-off cycles.

### Gotchas
- After power-off, the RTC keeps running on battery (draws negligible current)
- On power-on, `setup()` runs from scratch — all RAM state is lost
- WiFi reconnect takes ~1-3 seconds
- RTC timer max is 255 minutes (~4.25 hours) — for longer sleep, chain multiple sleep cycles (wake, reset timer, sleep again)
- If USB is connected (GPIO 5 > 0.2V), skip power-off to prevent confusion during development

---

## Working Reference Implementation

wsanders published a working badge sketch using power-off + RTC wake:
[github.com/wsanders/M5PaperS3-Stuff/papers3badge.ino](https://github.com/wsanders/M5PaperS3-Stuff/blob/main/papers3badge.ino)

---

## Sources

- [M5PaperS3 Official Datasheet (updated 2026-03-04)](https://m5stack.oss-cn-shenzhen.aliyuncs.com/resource/docs/static/pdf/static/en/core/PaperS3.pdf) — pin map confirms RTC INT → PMS150G wiring
- [M5Unified Power_Class.cpp](https://github.com/m5stack/M5Unified/blob/master/src/utility/Power_Class.cpp) — GPIO 44 pulse pattern, power-off implementation
- [M5Stack Community: Paper S3 questions](https://community.m5stack.com/topic/7108/paper-s3-questions/14) — power consumption measurements, timerSleep issues
- [M5Stack Community: Slow battery performance](https://community.m5stack.com/topic/8087/slow-battery-performance-with-m5papers3) — GPIO 44 PWROFF_Pulse confirmation
- [M5Stack Community: Wake on IMU](https://community.m5stack.com/topic/7748/paper-s3-wake-on-imu) — E_TRIG / PMS150G wake mechanism
- [PMS150G Datasheet](https://www.padauk.com.tw/upload/doc/PMS15B,PMS150G%20datasheet_EN_V008_20230216.pdf)
- [wsanders M5PaperS3 badge sketch](https://github.com/wsanders/M5PaperS3-Stuff/blob/main/papers3badge.ino) — working power-off + RTC wake example
