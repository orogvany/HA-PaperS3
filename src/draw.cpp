#include "draw.h"
#include "assets/Montserrat_Regular_26.h"
#include "boards.h"
#include <FastEPD.h>
#include <cstddef>
#include <cstdint>
#include <cstdio>

void drawCenteredIconWithText(FASTEPD* epaper, const uint8_t* icon, const char* const* lines, uint8_t line_spacing,
                              uint8_t icon_spacing) {
    BB_RECT rect;
    epaper->setFont(Montserrat_Regular_26);
    epaper->setTextColor(BBEP_BLACK);

    // Figure out the height of the text
    uint16_t text_height = 0;
    for (size_t i = 0; lines[i] != nullptr; ++i) {
        epaper->getStringBox(lines[i], &rect);
        text_height += rect.h;
        if (i > 0) {
            text_height += line_spacing;
        }
    }

    // Draw the icon
    const int icon_x = DISPLAY_WIDTH / 2 - 256 / 2;
    uint16_t cursor_y = DISPLAY_HEIGHT / 2 - (256 + icon_spacing + text_height) / 2;
    epaper->loadBMP(icon, icon_x, cursor_y, 0xf, BBEP_BLACK);

    // Draw each line
    cursor_y += icon_spacing + 256;
    for (size_t i = 0; lines[i] != nullptr; ++i) {
        epaper->getStringBox(lines[i], &rect);
        const int text_x = DISPLAY_WIDTH / 2 - rect.w / 2;

        epaper->setCursor(text_x, cursor_y);
        epaper->write(lines[i]);

        cursor_y += rect.h + line_spacing;
    }
}

constexpr uint16_t BATT_ICON_W = 28;
constexpr uint16_t BATT_ICON_H = 14;
constexpr uint16_t BATT_TIP_W = 3;
constexpr uint16_t BATT_BORDER = 2;
constexpr uint16_t BATT_MARGIN = 10;

void drawBatteryIndicator(FASTEPD* epaper, uint8_t percentage, bool charging) {
    char label[8];
    if (charging) {
        snprintf(label, sizeof(label), "%d%%+", percentage);
    } else {
        snprintf(label, sizeof(label), "%d%%", percentage);
    }

    BB_RECT text_rect;
    epaper->setFont(Montserrat_Regular_26);
    epaper->setTextColor(BBEP_BLACK);
    epaper->getStringBox(label, &text_rect);

    uint16_t total_w = BATT_ICON_W + BATT_TIP_W + 6 + text_rect.w;
    uint16_t x = DISPLAY_HEIGHT - BATT_MARGIN - total_w;
    uint16_t y = BATT_MARGIN;

    // Clear the area
    epaper->fillRect(x - 4, y - 2, total_w + 8, std::max((int)BATT_ICON_H, text_rect.h) + 4, 0xf);

    // Battery outline
    epaper->drawRect(x, y, BATT_ICON_W, BATT_ICON_H, BBEP_BLACK);
    epaper->fillRect(x + BATT_ICON_W, y + 3, BATT_TIP_W, BATT_ICON_H - 6, BBEP_BLACK);

    // Fill level
    uint16_t fill_w = (BATT_ICON_W - 2 * BATT_BORDER) * percentage / 100;
    if (fill_w > 0) {
        epaper->fillRect(x + BATT_BORDER, y + BATT_BORDER, fill_w, BATT_ICON_H - 2 * BATT_BORDER, BBEP_BLACK);
    }

    // Percentage text
    uint16_t text_x = x + BATT_ICON_W + BATT_TIP_W + 6;
    epaper->setCursor(text_x, y + (BATT_ICON_H + text_rect.h) / 2 - 2);
    epaper->write(label);
}

void drawIdleScreen(FASTEPD* epaper, uint16_t offset_x, uint16_t offset_y) {
    epaper->setMode(BB_MODE_4BPP);
    epaper->fillScreen(0xf);

    BB_RECT rect;
    epaper->setFont(Montserrat_Regular_26);
    epaper->setTextColor(BBEP_BLACK);

    const char* line = "Press button to wake";
    epaper->getStringBox(line, &rect);

    uint16_t x = (DISPLAY_HEIGHT - rect.w) / 2 + offset_x;
    uint16_t y = (DISPLAY_WIDTH - rect.h) / 2 + offset_y;

    epaper->setCursor(x, y);
    epaper->write(line);

    epaper->fullUpdate(CLEAR_SLOW, false);
}
