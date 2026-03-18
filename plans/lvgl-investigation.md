# LVGL Investigation

**Status**: TODO — needs proof-of-concept test
**Created**: 2026-03-17

---

## Question

Can LVGL coexist with FastEPD on the same display? Specifically: use FastEPD for the existing widget screens, and LVGL for new screens (idle screen with clock/weather, settings, etc.) — switching between them as needed.

## What needs testing

Build a minimal test app on the M5Paper S3 that:
1. Draws something with FastEPD (e.g., a rectangle + text)
2. Switches to LVGL, renders a simple screen (label + button)
3. Switches back to FastEPD and draws again

If both can take turns driving the display without corruption or conflicts, they can coexist.

## Key concerns
- LVGL wants to own the framebuffer and flush cycle
- FastEPD manages its own framebuffer and partial update tracking
- Can they hand off cleanly, or do they fight over display state?
- RAM: LVGL needs ~64KB+ for a 540x960 1-bit framebuffer (PSRAM available)

## LVGL e-ink resources
- [lv_port_esp32-epaper](https://github.com/martinberlin/lv_port_esp32-epaper) — LVGL on ESP32 with parallel e-paper via EPDiy
- [LVGL e-paper forum thread](https://forum.lvgl.io/t/lvgl-port-to-be-used-with-epaper-displays/5630)
- PlatformIO lib: `lvgl/lvgl`
