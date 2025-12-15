
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "beamformer_driver.h"

#define ADAR1000_RX_GAIN(channel) (0x010 + (channel))
#define ADAR1000_TX_GAIN(channel) (0x01C + (channel))

#define ADAR1000_RX_PHASE_I(channel) (0x014 + 2 * (channel))
#define ADAR1000_RX_PHASE_Q(channel) (0x015 + 2 * (channel))

#define ADAR1000_TX_PHASE_I(channel) (0x020 + 2 * (channel))
#define ADAR1000_TX_PHASE_Q(channel) (0x021 + 2 * (channel))

#define DIVISION_FACTOR (2.8125f)
#define NUM_CHANNELS 4

#define SUCCESS 0
#define FAILURE -1

// Register mode using here
struct Phase_s
{
    uint16_t integerPart;
    uint16_t fractionalPart;
    uint8_t regI_t;
    uint8_t regQ_t;
};
// 360 / 2.8125 = 128
static const struct Phase_s g_PhaseMap[128] = ADAR1000_PHASEMAP;

// static const struct of_device_id adar1000_of_match[] = {
//     { .compatible = "adi,adar1000" },
//     { }
// };
// MODULE_DEVICE_TABLE(of, adar1000_of_match);

static int writeRegister(uint32_t addr, uint8_t data)
{
    // SPI write
    return SUCCESS;
} // need to implement

static int readRegister(uint32_t addr, uint8_t *data)
{
    // SPI read
    return SUCCESS;
} // need to implement

int bf_init(void)
{
    //as per datasheet page 50
    writeRegister(0x000, 0xBD); // Reset whole chip, enable SDO readback
    writeRegister(0x000, 0x18); // SDO Enable All chips

    writeRegister(0x401, 0x02); // Allow LDO adjustments from user settings
    writeRegister(0x400, 0x55); // Adjust LDO regulators

    /* Receive-mode PA bias settings */
    writeRegister(0x046, 0x85); // PA_BIAS1 ≈ -2.5V (RX)
    writeRegister(0x047, 0x85); // PA_BIAS2 ≈ -2.5V (RX)
    writeRegister(0x048, 0x85); // PA_BIAS3 ≈ -2.5V (RX)
    writeRegister(0x049, 0x85); // PA_BIAS4 ≈ -2.5V (RX)
    writeRegister(0x04A, 0x68); // LNA_BIAS ≈ -2V (TX)

    /* Transmit-mode PA bias settings */
    writeRegister(0x029, 0x39); // PA_BIAS1 ≈ -1.1V (TX)
    writeRegister(0x02A, 0x39); // PA_BIAS2 ≈ -1.1V (TX)
    writeRegister(0x02B, 0x39); // PA_BIAS3 ≈ -1.1V (TX)
    writeRegister(0x02C, 0x39); // PA_BIAS4 ≈ -1.1V (TX)
    writeRegister(0x02D, 0x00); // LNA_BIAS ≈ 0V (RX)

    /* Bias and control selection */
    writeRegister(0x030, 0x1F); // Enable LNA_BIAS, fixed output
    writeRegister(0x038, 0x60); // Use SPI (not internal RAM) for channel settings
    writeRegister(0x031, 0x1C); // TR input select, enable switch outputs

    /* Transmit path enable */
    writeRegister(0x02F, 0x7F); // Enable all TX channels, driver, VM, VGA
    writeRegister(0x036, 0x16); // TX VGA bias = 2, VM bias = 6
    writeRegister(0x037, 0x06); // TX driver bias = 6

    /* TX Channel 1 */
    writeRegister(0x01C, 0xFF); // Ch1 attenuator 0 dB, VGA max
    writeRegister(0x020, 0x36); // Ch1 I = +16
    writeRegister(0x021, 0x35); // Ch1 Q = +15 (45°)

    /* TX Channel 2 */
    writeRegister(0x01D, 0xFF); // Ch2 attenuator 0 dB, VGA max
    writeRegister(0x022, 0x36); // Ch2 I = +16
    writeRegister(0x023, 0x35); // Ch2 Q = +15 (45°)

    /* TX Channel 3 */
    writeRegister(0x01E, 0xFF); // Ch3 attenuator 0 dB, VGA max
    writeRegister(0x024, 0x36); // Ch3 I = +16
    writeRegister(0x025, 0x35); // Ch3 Q = +15 (45°)

    /* TX Channel 4 */
    writeRegister(0x01F, 0xFF); // Ch4 attenuator 0 dB, VGA max
    writeRegister(0x026, 0x36); // Ch4 I = +16
    writeRegister(0x027, 0x35); // Ch4 Q = +15 (45°)

    /* Receive path enable */
    writeRegister(0x02E, 0x7F); // Enable all RX channels, LNA, VM, VGA
    writeRegister(0x034, 0x08); // RX LNA bias = 8
    writeRegister(0x035, 0x16); // RX VGA bias = 2, VM bias = 6

    /* RX Channel 1 */
    writeRegister(0x010, 0xFF); // Ch1 attenuator 0 dB, VGA max
    writeRegister(0x014, 0x36); // Ch1 I = +16
    writeRegister(0x015, 0x35); // Ch1 Q = +15 (45°)

    /* RX Channel 2 */
    writeRegister(0x011, 0xFF); // Ch2 attenuator 0 dB, VGA max
    writeRegister(0x016, 0x36); // Ch2 I = +16
    writeRegister(0x017, 0x35); // Ch2 Q = +15 (45°)

    /* RX Channel 3 */
    writeRegister(0x012, 0xFF); // Ch3 attenuator 0 dB, VGA max
    writeRegister(0x018, 0x36); // Ch3 I = +16
    writeRegister(0x019, 0x35); // Ch3 Q = +15 (45°)

    /* RX Channel 4 */
    writeRegister(0x013, 0xFF); // Ch4 attenuator 0 dB, VGA max
    writeRegister(0x01A, 0x36); // Ch4 I = +16
    writeRegister(0x01B, 0x35); // Ch4 Q = +15 (45°)

    return 0;
}

int bf_set_phase(int channel, int degree)
{
    if (channel < 0 || channel >= NUM_CHANNELS)
        return FAILURE;

    if (degree < 0 || degree > 360)
        return FAILURE;

    uint32_t regIAddr = 0;
    uint32_t regQAddr = 0;
    // uint8_t phsIdx = 0;
    uint8_t regI = 0, regQ = 0;

    uint8_t l = 0, r = 127;
    while (l <= r)
    {
        uint8_t m = (l + r) >> 1;
        if (degree == g_PhaseMap[m].integerPart)
        {
            regI = g_PhaseMap[m].regI_t;
            regQ = g_PhaseMap[m].regQ_t;
        }
        if (degree < g_PhaseMap[m].integerPart)
        {
            r = m - 1;
        }
        else
        {
            l = m + 1;
        }
    }

    int prev_val = g_PhaseMap[r].integerPart;
    int next_val = g_PhaseMap[l].integerPart;

    printf("prev %d, curr %d, next %d\n", prev_val, degree, next_val);
    if (fabs(degree - prev_val) < fabs(degree - next_val))
    {
        regI = g_PhaseMap[r].regI_t, regQ = g_PhaseMap[r].regQ_t; // If not found, return the next higher phase I & Q
    }
    else
    {
        regI = g_PhaseMap[l].regI_t, regQ = g_PhaseMap[l].regQ_t; // If not found, return the previous lower phase I & Q
    }

    printf("regI - %02x, regQ - %02x\n", regI, regQ);

    // for both RX and TX directions
    regIAddr = ADAR1000_RX_PHASE_I(channel);
    regQAddr = ADAR1000_RX_PHASE_Q(channel);
    if (writeRegister(regIAddr, regI) != SUCCESS)
        return FAILURE;
    if (writeRegister(regQAddr, regQ) != SUCCESS)
        return FAILURE;

    regIAddr = ADAR1000_TX_PHASE_I(channel);
    regQAddr = ADAR1000_TX_PHASE_Q(channel);
    if (writeRegister(regIAddr, regI) != SUCCESS)
        return FAILURE;
    if (writeRegister(regQAddr, regQ) != SUCCESS)
        return FAILURE;

    return 0;
}

int bf_set_gain(int channel, int gain)
{
    if (gain < 0 || gain > 0xFF)
        return FAILURE;
    if (channel < 0 || channel >= NUM_CHANNELS)
    {
        return FAILURE;
    }
    uint32_t gainRegAddr = 0;

    printf("channel - %d, gain %02x\n", channel, gain);

    gainRegAddr = ADAR1000_RX_GAIN(channel);
    if (writeRegister(gainRegAddr,(uint8_t)gain) != SUCCESS)
        return FAILURE;
    gainRegAddr = ADAR1000_TX_GAIN(channel);
    if (writeRegister(gainRegAddr,(uint8_t)gain) != SUCCESS)
        return FAILURE;

    return 0;
}

int bf_set_beam_angle(int angle_degree)
{
    // float phsIdx_float = 0.0f;
    // int phsIdx = 0;
    int phase_deg = 0;
    float theta_rad = 0.0f, delta_phase_deg = 0.0f;

    printf("angle_degree - %d\n", angle_degree);

    if (angle_degree < -90 || angle_degree > 90)
        return -1;

    theta_rad = (float)angle_degree * (3.1415926536f / 180.0f);
    delta_phase_deg = 180.0f * sinf(theta_rad);

    for (int ch = 0; ch < NUM_CHANNELS; ch++)
    {
        phase_deg = (int)(ch * delta_phase_deg);

        if (phase_deg < 0)
            phase_deg += 360;
        if (phase_deg >= 360)
            phase_deg %= 360;
        printf("phase_deg - %d, channel_no - %d\n", phase_deg, ch);
        bf_set_phase(ch, phase_deg);
    }

    // phsIdx_float = angle_degree / DIVISION_FACTOR; // 2.8125; // convert to 0-127 range
    // uint32_t int_part = (uint32_t)phsIdx_float;
    // float frac_part = phsIdx_float - int_part;
    // phsIdx = (frac_part > 0.5) ? (int_part + 1) : int_part;
    return 0;
}

int bf_deinit(void)
{
    writeRegister(0x000, 0xBD); // Reset whole chip
    writeRegister(0x000, 0x00); //  Disable SDO pin on all chips   -> page 48 of datasheet

    return 0;
}
