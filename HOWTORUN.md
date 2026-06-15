## Cara connect program ke ESP32 (project PlatformIO)

Project ini pakai PlatformIO (terlihat dari platformio.ini), bukan Arduino IDE biasa. Ada 2 environment target:

- esp32dev (ESP32 biasa)
- esp32-s3-devkitc-1 (ESP32-S3 dengan PSRAM)

## Proses upload ke board:

1. Hubungkan ESP32 ke PC via USB (kabel data, bukan cuma charging).
2. Pilih environment yang sesuai dengan board fisik kamu (klik environment di status bar bawah VSCode, atau pakai PlatformIO icon di sidebar).
3. Klik Upload (ikon arrow →) di PlatformIO toolbar, atau jalankan:

```bash
pio run -e esp32dev -t upload
```

4. PlatformIO otomatis akan compile dulu, lalu upload hasil binary ke ESP32 via serial/USB.
5. Untuk lihat output serial (debug print, dll), pakai Serial Monitor (ikon plug) — baudrate sudah diset 115200 di config.
