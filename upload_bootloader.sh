#!/bin/bash
# Force upload in bootloader mode

echo "=== ESP32-S3 Force Upload Script ==="
echo ""
echo "INSTRUCTIONS:"
echo "1. Unplug USB from CrowPanel"
echo "2. Hold BOOT button"
echo "3. Plug in USB (while holding BOOT)"
echo "4. Release BOOT after 2 seconds"
echo "5. Press ENTER here immediately"
echo ""
read -p "Press ENTER when device is in bootloader mode..."

echo ""
echo "Attempting upload..."
pio run -t upload --upload-port /dev/cu.usbserial-130

echo ""
echo "Upload complete! The device should boot with the new firmware."
echo "Run: pio device monitor"
