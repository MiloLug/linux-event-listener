#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include <linux/input.h>


int main(int argc, char *argv[])
{
    if(argc < 6) {
        printf(
            "\n"
            "Usage: event-listener <event-source> <type> <code> <value> <command> [skip-count]\n"
            "Example: event-listener /dev/input/event0 4 4 85 \"shutdown -P now\"\n\n"
            "Parameters:\n"
            "    <event-source> - string; event-source path\n"
            "    <type>         - int; '*' = any\n"
            "    <code>         - int; '*' = any\n"
            "    <value>        - int; '*' = any\n"
            "    <command>      - string; will be executed on match\n"
            "    [skip-count]   - int; 0 by default; how many times to skip the event\n"
            "        example: when it occurs two times and you want it to happen only once, just set skip-count to 1\n\n"
            "The command's env variables:\n"
            "    E_TIME_SEC     - int; timestamp's seconds\n"
            "    E_TIME_USEC    - int; timestamp's remaining microseconds\n"
            "    E_TYPE         - int\n"
            "    E_CODE         - int\n"
            "    E_VALUE        - int\n\n"
        );
        return 0;
    }

    const char *dev = argv[1];
    const char *command = argv[5];
    uint16_t target_type = 0, target_code = 0;
    int32_t target_value = 0;
    uint8_t ignore_mask = 0;
    uint32_t skip_count = 0, dup_count = 0;
    FILE *dev_fd;
    struct input_event e;
    
    char *env_command = malloc(strlen(command) + 136);
    // + strlen("export E_TIME_SEC=4294967295 ; export E_TIME_USEC=4294967295 ; export E_TYPE=65536 ; export E_CODE=65535 ; export E_VALUE=4294967295 ; ")

    if (argv[2][0] != '*') sscanf(argv[2], "%hi", &target_type);
    else ignore_mask |= 1;
    
    if (argv[3][0] != '*') sscanf(argv[3], "%hi", &target_code);
    else ignore_mask |= 1 << 1;
    
    if (argv[4][0] != '*') sscanf(argv[4], "%i", &target_value);
    else ignore_mask |= 1 << 2;

    if (argc > 6) sscanf(argv[6], "%i", &skip_count);


    if ((dev_fd = fopen(dev, "rb")) == NULL) {
        perror("opening device");
		exit(EXIT_FAILURE);
    }

    while (fread(&e, sizeof(struct input_event), 1, dev_fd)) {
        if ((
            (e.type == target_type)
            | (e.code == target_code)<<1
            | (e.value == target_value)<<2
            | ignore_mask
        ) == 0b111) {
            if (dup_count == skip_count) {
                #if uint64_t == __kernel_size_t
                    sprintf(
                        env_command, "export E_TIME_SEC=%li ; export E_TIME_USEC=%li ; export E_TYPE=%hi ; export E_CODE=%hi ; export E_VALUE=%i ; %s",
                        e.time.tv_sec, e.time.tv_usec, e.type, e.code, e.value, command
                    );
                #else
                    sprintf(
                        env_command, "export E_TIME_SEC=%li ; export E_TIME_USEC=%i ; export E_TYPE=%hi ; export E_CODE=%hi ; export E_VALUE=%i ; %s",
                        e.time.tv_sec, e.time.tv_usec, e.type, e.code, e.value, command
                    );
                #endif
                system(env_command);
                dup_count = 0;
            }
            else dup_count++;
        }
    }

    fclose(dev_fd);
    free(env_command);

    return 0;
}
