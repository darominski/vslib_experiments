This repository contains the code necessary to test on the FGC4 hardware the prototype for both component and parameter interface, and more importantly, the remote parameter value setting.

I. Parameter setting mechanism

The parameter setting takes place over shared memory between two cores and domains of the FGC4: the bare-metal domain reports the structure of settable parameters and their owning components as a JSON object. This object is serialized to characters and placed in the shared memory structure at a fixed address. The Linux domain application can read this manifest, parse it, and prepare appropriate commands that fit the extant parameters. To test failure cases, incorrect commands are also created. Then, these commands are communicated over the same shared memory structure to the bare-metal application. The bare-metal application validates the correctness of the command: whether it fits the schema, whether the requested parameter name is correct, and finally if the value provided can be set. Finally, after all commands in a round are executed and no new commands came in the last cycle, the background and real-time buffers are swapped, and then synchronized.

II. How to use this repository

To manually test the contents of this repository, it is necessary to separately compile three applications: `bmboot`, `vloop`, and `remote`. The bmboot contains the monitor and handles execution of the payload, which is `vloop`, while the `remote` is a Linux-domain application that can be run directly by the user.

1. Clone the repository:

```
git clone https://gitlab.cern.ch/doaromin/fgc4_parameter_setting.git
```

2. Get the dependencies:

Download an archive with the required compiler (GCC 12.3 for AArch64 hosted on x86_64)
```
wget https://cernbox.cern.ch/s/sLCfVKpyy5p2ZVt/download -O dependencies.zip
```
unpack it in your /opt directory:

```
sudo unzip dependencies.zip /opt
```

Add the compilers to your PATH:

```
export PATH=/opt/bin/arm-gnu-toolchain-12.3.rel1-x86_64-aarch64-none-elf:/opt/bin/arm-gnu-toolchain-12.3.rel1-x86_64-aarch64-none-linux-gnu:$PATH
```

2. Build Bmboot

a. Navigate to bmboot directory:

```
cd source/bmboot
```

b. Configure the build:

```
cmake -B build-bmboot -DCMAKE_C_COMPILER=aarch64-none-linux-gnu-gcc -DCMAKE_CXX_COMPILER=aarch64-none-linux-gnu-g++ -DCMAKE_C_COMPILER_AARCH64_NONE_ELF=aarch64-none-elf-gcc -DCMAKE_CXX_COMPILER_AARCH64_NONE_ELF=aarch64-none-elf-g++ -DCMAKE_OBJCOPY_AARCH64_NONE_ELF=/opt/bin/arm-gnu-toolchain-12.3.rel1-x86_64-aarch64-none-elf/bin/aarch64-none-elf-objcopy -DCMAKE_OBJDUMP_AARCH64_NONE_ELF=/opt/bin/arm-gnu-toolchain-12.3.rel1-x86_64-aarch64-none-elf/bin/aarch64-none-elf-objdump -DCMAKE_SIZE_AARCH64_NONE_ELF=aarch64-none-elf-size -DCMAKE_BUILD_TYPE=Release
```

c. Compile and link:

```
cmake --build build-bmboot
```

d. Upload to the device

```
scp -r build-bmboot root@fgc4-1:~/mylib
```

3. Build Vloop

a. Navigate to vloop directory:

```
cd ../vloop
```

b. Configure the build:

```
cmake -B build-vloop -DCMAKE_C_COMPILER=aarch64-none-elf-gcc -DCMAKE_CXX_COMPILER=aarch64-none-elf-g++ -DCMAKE_SIZE=aarch64-none-elf-size -DCMAKE_BUILD_TYPE=Release
```

c. Compile and link:

```
cmake --build build-vloop
```

d. Upload to the device

```
scp -r build-vloop root@fgc4-1:~/mylib
```


4. Build the Linux-domain application

a. Navigate to remote application directory:

```
cd ../linux_parameter_setter
```

b. Configure the build:

```
cmake -B build-remote -DCMAKE_CXX_COMPILER=aarch64-none-linux-gnu-g++
```

c. Compile and link:

```
cmake --build build-remote
```

d. Upload to the device

```
scp -r build-remote root@fgc4-1:~/mylib
```

III Testing on the hardware

1. Connect to the board

To test the parameter setting on the hardware, one needs to have two terminals opened and connected to the fgc4 board:

```
ssh root@fgc4-1
```

2. Navigate to the main source file

```
cd mylib
```

3. Check status of the bare-metal domain

```
./build-bmboot/bmctl status cpu1
```

The expected response should be `in_reset` if the board has been recently restarted.

4. Startup the bare-metal domain

If the `cpu1` is in `in_reset` state or any other state than `monitor_ready`, reset the `cpu1`:

```
./build-bmboot/bmctl reset cpu1
```

and check its status again:

```
./build-bmboot/bmctl status cpu1
```

5. Start the vloop payload

Now, we are ready to start the vloop payload:

```
./build-bmboot/bmctl exec cpu1 ./build-vloop/vloop.bin
```

6. Check that vloop is running:

```
./build-bmboot/console cpu1
```

The console should output text with the manifest containing two PIDs and then periodically (once a second)
output the current value of PIDs parameters.

7. Remotely set the parameters

Now, without exiting from the console, which will be useful for monitoring, and in a separate ssh connection, start
the `remote` application:

```
./build-remote/remote
```

It should print to the terminal the manifest it read from the shared memory, with two PIDs, and then periodically
output its thread counter.

You can switch back to the bare-metal console and see how the parameter changing commands are received and eventually
taken into account by the bare metal side.
