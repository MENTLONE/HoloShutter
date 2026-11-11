# Setup guide

## 1. Prepare the hardware

Wire the SG90 signal to **D9**, servo power to an **external regulated 5 V supply**, and connect the external supply ground to Nano **GND**. Follow the [wiring guide](../hardware/wiring.md).

## 2. Prepare Arduino IDE

1. Install the **Arduino Mbed OS Nano Boards** package.
2. Select **Arduino Nano RP2040 Connect** and the correct port.
3. Install **WiFiNINA** and **Servo** using Library Manager.
4. Upload the **verified working** HoloShutter sketch when it is available in `firmware/`.

The working sketch must be saved directly from the Arduino editor to avoid accidentally publishing a different, untested version.

## 3. Connect to the local controller

1. Open Serial Monitor at **115200 baud**.
2. Wait for the access point to initialize.
3. Join the Wi-Fi network **HoloShutter-RP2040** from your phone.
4. Enter the password configured in the sketch (prototype default: **HoloShutter530**).
5. Open **http://192.168.4.1** in Safari or another browser. If the printed IP differs, use the address shown in Serial Monitor.

Your phone may report that the network has **no internet**; this is expected. Stay connected to the access point.

## 4. Test without a laser

1. Press **OPEN SHUTTER** and confirm the blade clears the intended optical path.
2. Press **CLOSE SHUTTER** and confirm the blade fully blocks it.
3. Set **0 minutes, 10 seconds**.
4. Press **EXECUTE EXPOSURE**.
5. Confirm the shutter opens, settles, counts down, and closes automatically.
6. Start another exposure and test **CANCEL EXPOSURE**.

## Troubleshooting

| Symptom | Check |
| --- | --- |
| Phone says no internet | Normal for local AP; remain connected |
| Status stuck on CONNECTING | Confirm AP IP and `/api/status` response; reload the page |
| Corrupted text or random symbols | Ensure the full HTML response is transmitted with an accurate Content-Length and complete byte writes |
| Servo jitters or Nano resets | Check external regulated servo power, common ground and loose connections |
| Buttons work but countdown does not | Confirm browser JavaScript and firmware JSON API match |
| Servo moves the wrong way | Verify physical geometry and configured open/closed angles |

## Safety

A hobby servo does not provide fail-safe laser blocking. Never treat the web UI as a laser interlock. Use an independent beam stop and wavelength-appropriate laser safety equipment.
