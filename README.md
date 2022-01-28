# linux-event-listener
## Compilation Requirements
```
cmake make gcc linux-headers
```

## Compilation
```sh
# run:
./build.sh
# resulting binary:
./build/event-listener/event-listener
```

## Help
```sh
Usage: event-listener <event-source> <type> <code> <value> <command> [skip-count]
Example: event-listener /dev/input/event0 4 4 85 "shutdown -P now"

<type> - int; '*' = any
<code> - int; '*' = any
<value> - int; '*' = any
<command> - string; will be executed on match
[skip-count] - int; 0 by default; how many times to skip the event
    example: when it occurs two times and you want it to happen only once, just set skip-count to 1
```
