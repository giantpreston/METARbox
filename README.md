# ✈️ METARbox

**METARbox** is an Arduino-based weather display that shows real-time **METAR** data from any airport on a 16x2 LCD screen using an **ESP8266-01s WiFi module**.

Designed to be simple, educational, and reliable — just upload and it works.

---

### Creator’s Note 🛠️

> **This project has been a labor of love and frustration.** After spending over 72 hours troubleshooting, debugging, and tweaking every wire and line of code, I've learned more than I ever expected from a simple LCD + ESP8266 + Arduino project.
>
> If you're looking at this project and wondering why so many wires, or why certain parts don’t seem to work at first, trust me, I've been there. The path to making this thing work was filled with unexpected challenges, and sometimes it felt like the project was more "complex mess" than "working prototype." But through the chaos came the satisfaction of getting everything to work!
>
> So, if you're tackling this project, **prepare for a lot of trial and error**, and don't be discouraged when things don't click instantly. It's all part of the process. The end result is totally worth it, just take it one step at a time and keep at it! <3

---

## 📦 What It Does

* Connects to your WiFi using an ESP-01s module (via AT commands)
* Fetches live METAR weather from a public API
* Displays the report on a 16x2 LCD screen
* Smoothly scrolls long output
* Updates automatically every 30 minutes

---

## 🧰 What You Need

### Hardware

* Arduino Uno, Nano, or compatible board
* ESP8266-01s module
* LCD 16x2 display (HD44780-compatible)
* 10k potentiometer (for LCD contrast)
* Jumper wires and breadboard
* Voltage divider (for ESP-01 RX)
* Optional: 3.3V LDO regulator (if not using an adapter)

### ESP-01 Voltage Warning ⚠️

The ESP-01 **requires 3.3V power** — not 5V.
Do **not** power it from the Arduino 3.3V pin directly — it cannot supply enough current.

Use:

* A proper ESP-01 adapter **or**
* A 3.3V LDO (e.g. AMS1117) **or**
* A separate 3.3V supply that can deliver at least 200–250mA

---

## 🔌 Wiring

### LCD (16x2) Wiring

Some LCD modules have extra pins like `RW`, `LED+`, and `LED-`.

Here's the most common wiring:

| LCD Pin  | Purpose          | Connect to                   |
| -------- | ---------------- | ---------------------------- |
| VSS      | Ground           | GND                          |
| VDD      | Power            | 5V                           |
| VO       | Contrast control | Center of 10k pot            |
| RS       | Register Select  | Arduino D12                  |
| RW       | Read/Write       | GND (always write)           |
| E        | Enable           | Arduino D11                  |
| D4       | Data bit 4       | Arduino D5                   |
| D5       | Data bit 5       | Arduino D4                   |
| D6       | Data bit 6       | Arduino D3                   |
| D7       | Data bit 7       | Arduino D2                   |
| A (LED+) | Backlight +      | 5V (with resistor if needed) |
| K (LED-) | Backlight -      | GND                          |

### ESP8266-01s Wiring

| Arduino Pin | ESP-01 Pin   | Notes                                         |
| ----------- | ------------ | --------------------------------------------- |
| D8 (RX)     | ESP TX       | Connect directly                              |
| D9 (TX)     | ESP RX       | Use a voltage divider                         |
| GND         | GND          | Shared ground                                 |
| 3.3V        | VCC & CH\_PD | Only if external 3.3V supply is strong enough |

Don't forget to pull `CH_PD` high to enable the chip.

---

## ⚙️ Setup

1. Open `METARbox.ino` in the Arduino IDE

2. Set your board to **Arduino Uno** (or your board)

3. Enter your WiFi credentials:

   ```cpp
   const char* SSID = "YourNetworkName";
   const char* PASS = "YourPassword";
   ```

4. Click **Upload**

5. Open the **Serial Monitor at 9600 baud** to view connection and data logs

---

## 🌐 API Usage

This project uses a **free METAR API server** hosted by the project author:

```
http://152.70.212.248:3000/metar/KATL
```

You can change the airport code (`KATL`) in the code:

```cpp
const String stationCode = "KATL";
```

If you want to use your own server:

* Replace the host and port in the sketch:

  ```cpp
  const char* proxyHost = "152.70.212.248";
  const int   proxyPort = 3000;
  ```
#### (Replace this with yours!)

* You'll need to serve plain text METAR strings at the `/metar/<station>` endpoint.

---

## 🔁 Refresh Rate

METARbox fetches a new report every 30 minutes. You can change this by editing:

```cpp
const unsigned long fetchInterval = 30UL * 60UL * 1000UL;  // 30 minutes
```

---

## 🧾 License

This project is open source under the **MIT License**.
You're free to use, modify, and share — attribution appreciated.


Built with care by giantpreston — for weather, flying, and fun.
