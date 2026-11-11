# Wiring guide

## Electrical connections

```text
Arduino Nano RP2040 Connect        SG90 servo
---------------------------        ----------
D9  -----------------------------> SIGNAL

GND -----------------+
                     |
External 5 V supply  |              SG90 servo
---------------------+              ----------
GND -----------------+------------> GND
+5 V ------------------------------> V+
```

The Nano and external servo supply **must share ground**. Power the Nano from its supported USB input; power the servo from a suitable regulated 5 V supply. Do not connect the servo supply's +5 V to a GPIO or 3.3 V pin.

Typical SG90 wire colors are **orange/yellow = signal**, **red = +5 V**, **brown/black = ground**. Verify the specific servo before wiring.

## Motion

- D9 controls the servo signal.
- Closed angle: **0 degrees**
- Open angle: **68 degrees**
- Opening settle delay: **700 milliseconds** before exposure timing starts.

The angles are specific to the prototype mechanism. Test with the laser off, ensure the blade clears the beam, and avoid driving the servo against hard stops.

## Notes

The shutter is a convenience mechanism for optical experiments, **not a safety-rated beam shutter**. Use an independent beam stop and appropriate laser safety practices. A servo position command does not prove the blade physically reached that position.
