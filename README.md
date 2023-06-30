This repository holds a minimal and basic example of: register holding name IDs, addresses, and types or variables that can be configured,
automatic registration of new objects of a defined class (PID in the example), sharing memory between two or more Linux cores, and finally
communicating commands of what address should be set to what value.

Known possible improvements:
1. The address array could be held in a singleton class rather than an global variable.
2. There could be two shared memory maps: one for the address array, the other for commands.
3. Enable setting of more than one command per iteration of the receiver main loop.
4. `sys/mmap.h` is available on Linux BSPs, so a different solution would be needed to map shared memory on a bare metal core's side.
5. There is no mechanism to ensure that component names are unique.

How to use this repository:
1. Clone it.
2. Build the two binaries, one for `receiver` (future: bare metal) and the other for `remote` (future: Linux) by executing the Makefile:
```
make all
```
3. In separate threads, e.g. separate terminal instances, run first the receiver then the remote.

To ensure the proper memory cleanup, the number of iterations of the receiver's loop is limited. Remote will not be able to use the shared
memory if the receiver binary is not currently being executed.
