# Hardware Setup & Calibration

A start-to-finish bench guide: from an unflashed ESP32 to `GET /moisture` returning
live JSON. Do the parts in order — each one isolates a different class of problem
(toolchain → wiring → firmware → calibration), so if something breaks you know where.

**Your setup:** a standalone ESP32 dev board + an Elegoo kit (breadboard, jumper
wires) + the capacitive soil moisture sensor.

---

## What you need

- [ ] ESP32 dev board
- [ ] USB cable (must be a **data** cable, not charge-only — a common early gotcha)
- [ ] Capacitive Soil Moisture Sensor v1.2 (3-pin: VCC, GND, AOUT)
- [ ] Breadboard + 3 male-to-male jumper wires (Elegoo kit)
- [ ] Arduino IDE installed on your computer
- [ ] A cup of water for calibration

---

## Part A — Prove the toolchain (no sensor yet)

The goal here is only to confirm your computer can flash the ESP32. Don't wire
anything yet.

1. **Add ESP32 board support.** Arduino IDE → Tools → Board → Boards Manager →
   search **esp32** (by Espressif Systems) → Install.
2. **Plug the ESP32 into USB.**
3. **Select the board.** Tools →
   - Board: **ESP32 Dev Module**
   - Port: the new port that appeared when you plugged in
   - Upload Speed: **115200**
4. **If no port appears:** your board almost certainly uses the **CH340** USB chip
   (common on ESP32 dev boards). Install the CH340 driver, replug, and the port
   shows up. This is expected, not a broken board.
5. **Flash Blink.** File → Examples → 01.Basics → **Blink** → click Upload (→).
   - Some ESP32 boards need you to **hold the BOOT button** while "Connecting…"
     appears, then release. If upload stalls at `Connecting.....`, try that.
6. **Success =** the onboard LED blinks. Your toolchain works.

> Don't move on until Blink works. Every later step assumes you can flash reliably.

---

## Part B — Wire the sensor

Unplug the USB first (wire with the board powered off).

| Sensor pin | Wire to ESP32 | Notes |
|---|---|---|
| VCC | **3.3V** | **Not 5V** — see warning below |
| GND | **GND** | any GND pin |
| AOUT (signal) | **GPIO32** | must be an ADC1 pin (GPIO32–39) |

### The three connections

```
   CAPACITIVE SOIL MOISTURE SENSOR v1.2                    ESP32 DevKit
   ┌────────────────────────────────────┐
   │ ░░░░░░░░  exposed probe  ░░░░░░░░  │
   │ ░░░░░  (this goes in soil)  ░░░░░  │
   │                                    │
   │  ──── max waterline ────           │
   │                                    │
   │                      ┌────┬────┬────┐
   └──────────────────────┤AOUT│ GND│ VCC│
                          └──┬─┴──┬─┴──┬─┘
                             │    │    │
                             │    │    └── red ─────────►  3V3
                             │    │                        (NOT VIN / 5V)
                             │    └─────── black ────────►  GND
                             │                              (any GND pin)
                             └──────────── yellow ───────►  GPIO32
                                                            (silkscreen may
                                                             say "D32")
```

> **Read the silkscreen, don't trust the order above.** Pin order on these generic
> boards varies between production runs. Each of the three pins is labelled on the
> sensor PCB right by the header — match by *label*, not by position. Getting VCC
> and GND swapped is the one mistake that can damage the sensor.

### On the breadboard

Straddle the ESP32 across the center gap so each pin lands in its own 5-hole row.
If you push it to one side instead, the pins short together down a single row.

```
        ╔═══════════════════════════════════════════════╗
        ║  ○ ○ ○ ○ ○   ○ ○ ○ ○ ○   ○ ○ ○ ○ ○   ○ ○ ○ ○  ║
        ║  ○ ○ ○ ○ ○   ○ ○ ○ ○ ○   ○ ○ ○ ○ ○   ○ ○ ○ ○  ║
   3V3 ─╫──● ○ ○ ○ ○ ┌───────────────────────┐ ○ ○ ○ ○  ║
   GND ─╫──● ○ ○ ○ ○ │                       │ ○ ○ ○ ○  ║
        ║  ○ ○ ○ ○ ○ │        ESP32          │ ○ ○ ○ ○  ║
        ╠════════════│   (straddles gap)     │══════════╣  ← center gap
        ║  ○ ○ ○ ○ ○ │                       │ ○ ○ ○ ○  ║
 GPIO32─╫──● ○ ○ ○ ○ └───────────────────────┘ ○ ○ ○ ○  ║
        ║  ○ ○ ○ ○ ○   ○ ○ ○ ○ ○   ○ ○ ○ ○ ○   ○ ○ ○ ○  ║
        ╚═══════════════════════════════════════════════╝

        ● = jumper from the sensor lands in the same row as the ESP32 pin
```

Each ESP32 pin is electrically joined to the 4 free holes in its row — that's what
lets you reach it with a jumper. The exact rows depend on where you seat the board;
what matters is that the sensor wire goes into *the same row* as its target pin.

**Before you plug USB back in, check:** the red wire goes to a pin labelled 3V3
(not VIN, not 5V), and red and black are not swapped at the sensor end.

> ⚠️ **Power the sensor at 3.3V, never 5V.** The sensor's output voltage scales with
> its supply. At 5V, AOUT can exceed 3.3V and overrun the ESP32's ADC input range —
> clipped readings and stress on the pin.

> ⚠️ **AOUT must go to an ADC1 pin (GPIO32–39).** ADC2 pins (GPIO0, 2, 4, 12–15,
> 25–27) stop working the moment Wi-Fi turns on. GPIO32 is our chosen pin and matches
> `SENSOR_PIN` in the firmware.

Double-check the three wires before repowering.

---

## Part C — Flash the firmware

1. Open `firmware/moisture/moisture.ino` in the Arduino IDE.
2. **Set your Wi-Fi credentials** near the top of the sketch:
   ```cpp
   const char* WIFI_SSID     = "your-network-name";
   const char* WIFI_PASSWORD = "your-password";
   ```
   Use a **2.4 GHz** network — the ESP32 does not join 5 GHz networks.

   > ⚠️ **This repo is public.** Real credentials in this file must not be committed.
   > Confirm the guard is installed (it is not automatic on a fresh clone):
   > ```
   > git config core.hooksPath .githooks
   > ```
   > `.githooks/pre-commit` then blocks any commit whose staged `moisture.ino` is
   > missing the placeholders, and `.githooks/pre-push` re-checks every outgoing
   > commit. To commit other work meanwhile:
   > ```
   > git stash push firmware/moisture/moisture.ino
   > git commit ...
   > git stash pop
   > ```

   > ⚠️ **Careful pasting serial output.** The firmware prints
   > `Connecting to <your-SSID>` on boot. Don't paste that first line into docs,
   > issues, or a chat — the heartbeat lines below it are the only part needed for
   > calibration.
3. Leave the calibration constants (`RAW_DRY`, `RAW_WET`) at their defaults for now;
   you'll correct them in Part D.
4. Upload (same board/port/speed as Part A).
5. Open **Tools → Serial Monitor**, set baud to **115200**. You should see:
   ```
   Connecting to <your-network>....
   Connected. IP: 192.168.x.x
   mDNS started: http://esp32.local
   HTTP server started on port 80
   ```
   Note the IP address — it's your fallback if `esp32.local` doesn't resolve.

   Below that, a reading prints once per second — this is the calibration heartbeat
   you'll use in Part D:
   ```
   raw ADC (avg of 10): 2887  ->  1%
   ```

If Wi-Fi fails, the firmware waits 15 seconds, says so, and **carries on anyway**:

```
Wi-Fi failed (check SSID/password, and that it's 2.4 GHz).
Continuing without networking — sensor readings below still valid.
raw ADC (avg of 10): 2887  ->  1%
```

That's deliberate — you can confirm your wiring is right (Part D) without having
solved Wi-Fi first. Only Part E needs the network.

---

## Part D — Calibrate

The sensor's raw ADC value must be mapped to 0–100%, and the endpoints depend on your
specific unit. The firmware prints a reading to Serial Monitor once per second, so
calibration is just "hold the sensor still, watch the numbers settle, write one down."

**Sanity check first:** with the sensor in open air, the raw value should sit somewhere
in the 2500–3200 range and stay steady. If it reads 0 or 4095, or jumps around wildly,
stop and recheck the wiring from Part B before calibrating.

1. **Dry reading (`RAW_DRY`):** hold the sensor in **open air**. Take ~10 readings
   from the serial log, average them. Expected ballpark ~2900.
2. **Wet reading (`RAW_WET`):** put the sensor tip in water **up to but not past the
   line** on the board. Take ~10 readings, average them. Expected ballpark ~1200.
3. **Update the constants** in `firmware/moisture/moisture.ino` (lines 21–22):
   ```cpp
   const int RAW_DRY = 2900;  // ← your measured dry average
   const int RAW_WET = 1200;  // ← your measured wet average
   ```
4. **Reflash.** Now dry air should read near **0%** and the tip in water near **100%**.
5. **Log your values** in `docs/sprints/mvp-sprint.md` (the Calibration Log table).

Key facts:
- Raw values **decrease** as moisture increases (wetter = lower ADC).
- `constrain()` clamps output to 0–100, so bone-dry or fully-submerged never returns
  an out-of-range number.
- Recalibrate if you ever swap to a different sensor unit.

---

## Part E — Verify (the MVP done-condition)

From a computer on the same Wi-Fi network:

```
curl http://esp32.local/moisture
→ {"moisture_percent": 42}
```

Or open `http://esp32.local/moisture` in a browser.

- **Fallback if `esp32.local` doesn't resolve** (common on some Android/Windows
  setups): use the IP from Part C, e.g. `curl http://192.168.x.x/moisture`.
- A **sensor fault** (disconnected wire, or ADC railed) returns:
  ```json
  {"moisture_percent": 0, "error": "sensor fault: raw value out of range (4095)"}
  ```
  If you see this, recheck the AOUT wire to GPIO32.

**MVP is done when** that curl returns valid JSON with a sensible value, stable over
~5 minutes. That unblocks V1 (the Expo app).

---

## Troubleshooting

| Symptom | Likely cause | Fix |
|---|---|---|
| No serial port in Arduino IDE | Missing USB driver / charge-only cable | Install CH340 driver; use a data cable |
| Upload stalls at `Connecting.....` | Board not in flash mode | Hold BOOT button during connect, then release |
| Stuck at `Connecting to <network>...` | Wrong creds or 5 GHz network | Fix SSID/password; use a 2.4 GHz network |
| `esp32.local` won't resolve | mDNS unsupported on client | Use the device IP directly |
| Reading pinned at 0% or 100% | Calibration constants wrong/swapped | Re-measure `RAW_DRY`/`RAW_WET`, reflash |
| `sensor fault` error in JSON | AOUT wire loose or on wrong pin | Reseat AOUT → GPIO32 |
| Readings jump around wildly | Loose breadboard connection / noise | Reseat wires; sampling already averages 10 reads |
