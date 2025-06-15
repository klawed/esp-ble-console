# esp-ble-console

## DI-Why?

Because crawling under your desk to plug in a USB-to-serial adapter every time you need to check your Raspberry Pi's console output is getting old. This ESP32 turns into a wireless Bluetooth bridge so you can debug from your phone while staying comfortably on the couch.

Also works for any device with a UART console - Arduino projects, other SBCs, embedded systems, etc.

## What It Does

Creates a transparent Bluetooth UART bridge between your device's serial console and your phone/tablet. The ESP32 sits between your target device and your mobile device, forwarding data bidirectionally over Bluetooth.

```
[Raspberry Pi] <--UART--> [ESP32] <--Bluetooth--> [iPhone/iPad]
```

## Hardware Requirements

- ESP32 or ESP32-S3 development board
- 3 jumper wires (or 4 if you need power)
- Target device with UART console (like a Raspberry Pi)

## Wiring

### For Raspberry Pi Console Access
```
ESP32 GPIO16 (RX) ──> Pi GPIO14 (TXD, Pin 8)
ESP32 GPIO17 (TX) ──> Pi GPIO15 (RXD, Pin 10)  
ESP32 GND        ──> Pi GND (Pin 6)
```

**Pi setup**: Make sure you have console enabled on the Pi's UART pins. Add `enable_uart=1` to `/boot/config.txt` and reboot.

### For Other 3.3V Devices
```
ESP32 GPIO16 (RX) ──> Device TX
ESP32 GPIO17 (TX) ──> Device RX
ESP32 GND        ──> Device GND
```

**Note**: Both ESP32 and Pi run at 3.3V, so direct connection is fine. For 5V devices, you'd need level shifters.

## Installation

### Option 1: Web Installation (Recommended)
Visit the project's installation page: **https://klawed.github.io/esp-ble-console/**

1. Connect your ESP32 via USB
2. Click "Install ESP32 Console Bridge"
3. Select your device and follow the prompts
4. Done! No software installation needed.

### Option 2: PlatformIO
```bash
git clone https://github.com/klawed/esp-ble-console.git
cd esp-ble-console
pio run -e esp32dev -t upload
```

### Option 3: Arduino IDE
1. Copy `src/main.cpp` content to a new Arduino sketch
2. Install "ESP32 BluetoothSerial" library if not already included
3. Select your ESP32 board and upload

## Usage

1. **Power up**: ESP32 starts advertising as "ESP32-Console"
2. **Connect**: Pair with your phone/tablet via Bluetooth
3. **Terminal app**: Use any Bluetooth terminal app to connect
4. **Debug away**: You now have wireless access to the console

### iOS/iPadOS Apps That Work
- **Serial Bluetooth Terminal** - Simple and reliable
- **BlueTerm** - Good alternative option

### Status LED (GPIO2)
- **Slow blink**: Waiting for Bluetooth connection
- **Solid on**: Connected and ready

### Debug Commands (via USB Serial)
- Send `i` - Show connection info and status
- Send `r` - Restart the ESP32
- Other characters get forwarded to the target device

## Range and Performance

- **Range**: ~10-30 feet depending on obstacles
- **Baud rate**: 115200 (standard Pi console speed)
- **Latency**: Very low, good for interactive use
- **Throughput**: Handles typical console output fine

## Troubleshooting

**Can't connect via Bluetooth**: Make sure your device supports Bluetooth Classic (not just BLE). Most phones do, but some tablets are BLE-only.

**No data flowing**: Check your wiring and make sure the target device's UART is actually enabled and outputting data.

**Connection drops**: Power issues are common. Make sure the ESP32 has stable power - USB power from a Pi can sometimes be flaky under load.

**iOS won't find device**: Some iOS versions are picky. Try forgetting and re-pairing, or restarting Bluetooth on your device.

## Security Note

Bluetooth Classic isn't encrypted by default in this simple implementation. Don't use this for sensitive systems accessible by others. It's fine for local debugging but consider it like having an open serial port.

## License

MIT License - hack away.
