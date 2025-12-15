# Beamformer Driver

## IC Selected
**ADAR1000** – 4-channel RF beamformer IC with SPI control.

## Driver Overview
User-space C driver providing APIs to:
- Initialize/deinitialize the IC
- Set channel phase & gain
- Set beam angle (convert angle -> per-channel phase and updates all channel phases)

## Functions are Structured:
- `beamformer_driver.c` / `.h` – driver
- `beamctl.c` – simple CLI tester

## Functions
| Function | Purpose |
|----------|---------|
| `bf_init()` | Initialize IC / SPI Programming - As per page 50 of datasheet and enable SDO |
| `bf_deinit()` | Reset IC and disabled SDO |
| `bf_set_phase(channel, degree)` | Set channel phase |
| `bf_set_gain(channel, gain)` | Set channel gain (dB) |
| `bf_set_beam_angle(angle)` | Convert angle into per-channel phases |

## angle → Phase Logic
1. Compute phase:  
   `θ(rad) = angle_degree × π / 180`
   `Δφ = 180 × sin(θ)`

2. Apply relative phase per channel:  
   `phase(ch) = ch × Δφ`  
3. Map to register I/Q values using `ADAR1000_PHASEMAP`.

## Execute the command
 `gcc -o beamctl beamctl.c beamformer_driver.c`