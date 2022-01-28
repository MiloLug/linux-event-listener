#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <linux/input.h>


int main(int argc, char *argv[])
{
    if(argc < 6) {
        printf(
            "\n"
            "Usage: event-listener <event-source> <type> <code> <value> <command> [skip-count]\n"
            "Example: event-listener /dev/input/event0 4 4 85 \"shutdown -P now\"\n\n"
            "<type> - int; '*' = any\n"
            "<code> - int; '*' = any\n"
            "<value> - int; '*' = any\n"
            "<command> - string; will be executed on match\n"
            "[skip-count] - int; 0 by default; how many times to skip the event\n"
            "    example: when it occurs two times and you want it to happen only once, just set skip-count to 1\n"
            "\n"
        );
        return 0;
    }

    const char *dev = argv[1];
    const char *command = argv[5];
    u_int16_t target_type = 0, target_code = 0;
    int32_t target_value = 0;
    u_int8_t ignore_mask = 0;
    u_int32_t skip_count = 0;
    FILE *dev_fd;
    struct input_event e;
    
    if (argv[2][0] != '*') sscanf(argv[2], "%hd", &target_type);
    else ignore_mask |= 1;
    
    if (argv[3][0] != '*') sscanf(argv[3], "%hd", &target_code);
    else ignore_mask |= 1 << 1;
    
    if (argv[4][0] != '*') sscanf(argv[4], "%i", &target_value);
    else ignore_mask |= 1 << 2;

    if (argc > 6) sscanf(argv[6], "%i", &skip_count);


    if ((dev_fd = fopen(dev, "rb")) == NULL) {
        perror("opening device");
		exit(EXIT_FAILURE);
    }

    u_int32_t dup_count = 0;
    while (fread(&e, sizeof(struct input_event), 1, dev_fd)) {
        if ((
            (e.type == target_type)
            | (e.code == target_code)<<1
            | (e.value == target_value)<<2
            | ignore_mask
        ) == 0b111) {
            if (dup_count == skip_count) {
                system(command);
                dup_count = 0;
            }
            else dup_count++;
        }
    }

    fclose(dev_fd);

    return 0;
}
