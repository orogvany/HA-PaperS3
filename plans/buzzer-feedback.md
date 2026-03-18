# Touch Audio Feedback (Buzzer)

**Status**: TODO — quick feature, ~5 lines
**Created**: 2026-03-18

---

## Feature

Short audible click via the onboard passive buzzer when a widget is touched. Provides instant confirmation that a tap registered, before the e-ink display visually updates.

## Implementation

- Single `tone(BUZZER_PIN, frequency, duration)` call in touch handler on widget activation
- Feature flag: `BUZZER_FEEDBACK_ENABLED` in constants.h (default: true, easily toggled)
- Need to identify the buzzer GPIO pin on M5Paper S3
- Duration: ~10-20ms click, frequency: ~2000-4000Hz (test for least annoying sound)

## Context

Device is wall-mounted near front door as a home control center. Audio feedback is appropriate for this use case. Feature-flagged so it can be disabled if annoying after testing.
