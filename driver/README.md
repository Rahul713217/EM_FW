# Beamformer Driver Assignment

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
| `bf_set_beam_angle(angle)` | Compute per-channel phases for desired beam angle |

## Beam-angle → Phase Logic
1. Compute phase shift:  
   `phase_shift = (2 * π * frequency * d / c) * sin(angle)`  
2. Apply relative phase per channel:  
   `channel_phase = index * phase_shift`  
3. Map to register I/Q values using `ADAR1000_PHASEMAP`.