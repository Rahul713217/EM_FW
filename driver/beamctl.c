
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "beamformer_driver.h"  // driver APIs

static void print_usage(const char *prog)
{
    printf("Usage:\n");
    printf("  %s --phase <channel> <degree>\n", prog);
    printf("  %s --gain  <channel> <gain_db>\n", prog);
    printf("  %s --angle <angle_degree>\n", prog);
}

int main(int argc, char *argv[])
{
    int ret;

    if (argc < 2) {
        print_usage(argv[0]);
        return -1;
    }

    ret = bf_init();
    if (ret) {
        fprintf(stderr, "bf_init failed: %d\n", ret);
        return ret;
    }

    /* --phase <channel> <degree> */
    if (strcmp(argv[1], "--phase") == 0) {
        if (argc != 4) {
            print_usage(argv[0]);
            return -1;
        }
        int channel = atoi(argv[2]);
        int degree  = atoi(argv[3]);
        ret = bf_set_phase(channel, degree);
        if (ret)
            fprintf(stderr, "bf_set_phase failed\n");
    }
    /* --gain <channel> <gain_db> */
    else if (strcmp(argv[1], "--gain") == 0) {
        if (argc != 4) {
            print_usage(argv[0]);
            return -1;
        }
        int channel = atoi(argv[2]);
        int gain    = atoi(argv[3]);
        ret = bf_set_gain(channel, gain);
        if (ret)
            fprintf(stderr, "bf_set_gain failed\n");
    }
    /* --angle <angle_degree> */
    else if (strcmp(argv[1], "--angle") == 0) {
        if (argc != 3) {
            print_usage(argv[0]);
            return -1;
        }
        int angle = atoi(argv[2]);
        // float angle = atoi(argv[2]);
        ret = bf_set_beam_angle(angle);
        if (ret)
            fprintf(stderr, "bf_set_beam_angle failed\n");
    }
    else {
        print_usage(argv[0]);
        return -1;
    }

    bf_deinit();

    return ret;
}
