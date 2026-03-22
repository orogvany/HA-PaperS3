import glob
from pathlib import Path
from PIL import Image
from io import BytesIO, TextIOWrapper

WIDGET_ICON_SIZE = (64, 64)
UI_ICON_SIZE = (256, 256)
CHROME_ICON_SIZE = (64, 64)
STATUSBAR_ICON_SIZE = (24, 24)
WEATHER_ICON_SIZE = (128, 128)
HEADER = """// AUTO-GENERATED FILE — DO NOT EDIT
#pragma once
#include <pgmspace.h>
#include <cstdint>
#include <cstddef>

"""


def bmp_to_c_array(name: str, bmp_bytes: bytes) -> str:
    hex_bytes = ", ".join(f"0x{b:02X}" for b in bmp_bytes)
    return (
        f"const uint8_t {name}[] PROGMEM = {{ {hex_bytes} }};\n"
        f"const size_t {name}_len = {len(bmp_bytes)};\n"
    )


def process_image(path: Path, size: tuple[int, int]) -> bytes:
    im = Image.open(path).convert("RGBA")

    # Resize
    white_bg = Image.new("RGBA", im.size, (255, 255, 255, 255))
    im = Image.alpha_composite(white_bg, im)
    im = im.resize(size, Image.Resampling.LANCZOS)

    # Convert to 1-bit monochrome
    bw = im.convert("L").point(lambda x: 0 if x < 128 else 255, "1")

    # Save to BMP in memory
    buf = BytesIO()
    bw.save(buf, format="BMP")
    return buf.getvalue()


def handle_file(file_path: str, out: TextIOWrapper, size: tuple[int, int], prefix: str = "") -> None:
    path = Path(file_path)
    name = prefix + path.stem.replace("-", "_").replace(" ", "_")
    bmp_data = process_image(path, size)

    print(f"Processing {path} → {name}")

    out.write(bmp_to_c_array(name, bmp_data))
    out.write("\n")


def main() -> None:
    with open("src/assets/icons.h", "w") as out:
        out.write(HEADER)
        for file_path in sorted(glob.glob("icons-buttons/*.png")):
            handle_file(file_path, out, WIDGET_ICON_SIZE, "btn_")
        for file_path in sorted(glob.glob("icons-ui/*.png")):
            handle_file(file_path, out, UI_ICON_SIZE, "ui_")
        for file_path in sorted(glob.glob("icons-chrome/*.png")):
            handle_file(file_path, out, CHROME_ICON_SIZE, "chrome_")
        for file_path in sorted(glob.glob("icons-statusbar/*.png")):
            handle_file(file_path, out, STATUSBAR_ICON_SIZE, "status_")
        for file_path in sorted(glob.glob("icons-weather/*.png")):
            handle_file(file_path, out, WEATHER_ICON_SIZE, "weather_")

    print("Done")


if __name__ == "__main__":
    main()
