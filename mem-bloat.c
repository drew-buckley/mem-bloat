#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

long long parse_mem_suffix(char* mem_suffix) {
    long long byte_value = 1;
    switch(mem_suffix[0]) {
        case 'G':
            byte_value *= 1024;
        case 'M':
            byte_value *= 1024;
        case 'K':
            byte_value *= 1024;
        case 'B':
            break;
        default:
            if (!isdigit((unsigned char)mem_suffix[0])) {
                return -1;
            }
    }

    return byte_value;
}

void adjust_suffixed_string(char** suffixed_string) {
    char* suffixed_str = *suffixed_string;
    size_t len = strlen(suffixed_str);
    for (size_t i = len - 1; i >= 0; i--) {
        if (!isdigit((unsigned char)suffixed_str[i])) {
            suffixed_str[i] = '\0';
        }
        else {
            break;
        }
    }
}

void mem_bloat(long long mem_limit, float growth_step_ratio, float growth_secs) {
    long long growth_step_val = (long long)((float)mem_limit * growth_step_ratio);
    useconds_t growth_usecs = (useconds_t)(growth_secs * 1e6);
    long long current_mem;
    size_t i = 0;
    static void* pointers[1024 * 1024] = {0};
    for (current_mem = 0; current_mem < mem_limit; current_mem += growth_step_val) {
        printf("Allocating %lld bytes\n", growth_step_val);
        if(!(pointers[i] = malloc((size_t)growth_step_val))) {
            fprintf(stderr, "failed to allocate more memory\n");
            exit(-1);
        }

        for (int n = 0; n < growth_step_val; n++) {
            ((char*)pointers[i])[n] = (char)(n % 256);
        }

        usleep(growth_usecs);
        i++;
    }
}

int main(int argc, char *argv[]) {
    long long mem_limit = -1;
    float growth_step = -1.0;
    float growth_secs = -1.0;
    char *e;
    int opt;
    while ((opt = getopt(argc, argv, "m:s:t:")) != -1) {
		switch (opt) {
            case 'm':
                if (strlen(optarg) >= 2) {
                    if ((mem_limit = parse_mem_suffix(optarg + strlen(optarg) - 2)) == -1) {
                        fprintf(stderr, "-m opt parsing failed\n");
                        exit(-1);
                    }
                    adjust_suffixed_string(&optarg);
                }
                else {
                    mem_limit = 1;
                }
                mem_limit *= strtoll(optarg, &e, 0);
                if (errno != 0 || *e != 0) {
                    fprintf(stderr, "-m opt parsing failed\n");
                    exit(-1);
                }
                fprintf(stderr, "mem_limit =  %lld\n", mem_limit);
                break;
            case 's':
                growth_step = strtof(optarg, &e);
                if (errno != 0 || *e != 0) {
                    fprintf(stderr, "-s opt parsing failed\n");
                    exit(-1);
                }
                if (growth_step > 1.0) {
                    fprintf(stderr, "-s must be <= 1.0\n");
                    exit(-1);
                }
                if (growth_step <= 0.0) {
                    fprintf(stderr, "-s must be > 0.0\n");
                    exit(-1);
                }
                break;
            case 't':
                growth_secs = strtof(optarg, &e);
                if (errno != 0 || *e != 0) {
                    fprintf(stderr, "-t opt parsing failed\n");
                    exit(-1);
                }
                if (growth_secs < 0.0) {
                    fprintf(stderr, "-t must be >= 0.0\n");
                    exit(-1);
                }
                break;
        }
    }

    if (mem_limit < 0) {
        fprintf(stderr, "must provide -m argument");
        exit(-1);
    }

    if (growth_step < 0.0) {
        fprintf(stderr, "must provide -s argument");
        exit(-1);
    }

    if (growth_secs < 0.0) {
        fprintf(stderr, "must provide -t argument");
        exit(-1);
    }

    mem_bloat(mem_limit, growth_step, growth_secs);
    pause();
    exit(0);
}
