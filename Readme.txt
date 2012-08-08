Linux Kernel for VyBrid VF600 Release1
Release Note

2012/7/13 Lineo Solutions, Inc.

-------------------------------------------------------------------------------

1. Release Contents
   1.1 Linux kernel source code
       Kernel source code is not included in this release. It is managed by github and obtainable from the URL below.
         https://github.com/tsfs/Vybrid-Linux
       All the changes to Release1 are registered to 3.0-vybrid-lineo branch of the repository above.

   1.2 Document
       Design Documents and Test Plans are stored in the docs directory.

   1.3 Test Report
       Test reports are stored in the test_report directory.

   1.4 Test Program
       Programs used for the test are stored in the test_program directory.


2. Testing Environment
   2.1 HW
       HW used for testing is the following.
       Use either TWR-SER or TWR-SER2 at once. (No simultaneous usage.)

       - TWR-VF600

       - TWR-ELEV
         Connect A41 to A43, and A42 to A44 on the primary (white connectors) elevator.

       - TWR-SER
         J12: RMII mode (short 9 and 10)
         J2: 50MHz (short 3 and 4)
         J3: 50MHz (short 2 and 3)
         Please see 2012-06-15 18.52.51.jpg.

       - TWR-SER2
         10/100 Dual RMII (open J8 and J9, SW1 11000000, SW2 10100000)
         Serial-to-USB (short 1 and 2 of J7, short 3 and 4 of J7)
         RS232 DB9 (short 2 and 3 od J1, short 2 and 3 of J2, short 1 and 2 of J13)
         Please see 2012-07-03 19.46.01.jpg.

   2.2 SW
       - Linux Host
         Fedora12 / Ubuntu 10.10

       - Toolchain
         gcc-linaro-arm-linux-gnueabihf-2012.05-20120523_linux.tar.bz2
         http://www.linaro.org/downloads/1205


3. Kernel Supported Features
   - MSL
     ARM CortexA5
     Interrupt
     Timer (GPT): System timer tick support
     IOMUX
     GPIO
     eDMA
     Timer (FMT, PIT, LPTMR)

   - Character Device Drivers
     UART: Support as serial console only

   - Input Device Drivers
     Keypad

   - Networking Drivers
     Ethernet (Single)

   - General drivers
     SRTC

   - Filesystem
     initramfs and NFS are tested


4. Note
   - Ethernet MAC address
     MAC address of Ethernet is hard-coded on arch/mach-mvf/mvf_fec.c as ae:c6:09:97:21:00.

   - Ethernet clock
     Ethernet did not work under u-boot clock configuration as-is, thus changed setting to use the 'ENET External clk' in the kernel.

   - L2 Switch
     Current status of L2 Switch
     * It is confirmed that Phy up/down and Phy Speed of FEC0/FEC1 are detecting correct status corresponding to the connected HUB.
     * It is also confirmed that it sends out bootp request from kernel, as well as receives bootpreply though such operations are not quite stable.
     * However, transmit from Switch IP stops in mid-flow and NFS mount is not carried out, thus Switch operation isnÅft succeeded.

   - Conditions to enable L2 cache
     L2 cache is enabled when two conditions below are cleared.
     * The value of 'Processor x Configuration 1 Register' is NOT zero.
     * The address (0x3f480000) is NOT writable.

   - arch/arm/kernel/hw_breakpoint.c
     Although it is supposed to be supported as DBGVCR (according to the "9.4 Debug register summary" in CA5 Reference Manual), undefined
     instruction exception occurs in the following coprocessor instruction: mcr 14, 0, r3, cr0, cr7, {0}.
     Avoiding hw_breakpoint.c to be compiled, temporarily make CONFIG_HAVE_HW_BREAKPOINT in kernel configuration OFF .


5. Known Issues/Limitations
   - SRTC
     Time delay occurs at a rate of one second in 10 seconds or so.

   - Keypad
     Key event occurs twice at a time of keypad push/release.

   - L2Switch Driver
     * Interrupt error occurs as starting to handle interrupt.
	------------[ cut here ]------------
	WARNING: at kernel/irq/handle.c:130 handle_irq_event_percpu+0x180/0x19c()
	irq 114 handler switch_enet_interrupt+0x0/0x3d8 enabled interrupts
	Modules linked in:
	[<8002deb4>] (unwind_backtrace+0x0/0xf8) from [<8003f5c8>] (warn_slowpath_common+0x4c/0x64)
	[<8003f5c8>] (warn_slowpath_common+0x4c/0x64) from [<8003f674>] (warn_slowpath_fmt+0x30/0x40)
	[<8003f674>] (warn_slowpath_fmt+0x30/0x40) from [<800716ec>] (handle_irq_event_percpu+0x180/0x19c)
	[<800716ec>] (handle_irq_event_percpu+0x180/0x19c) from [<8007175c>] (handle_irq_event+0x54/0x84)
	[<8007175c>] (handle_irq_event+0x54/0x84) from [<8007374c>] (handle_fasteoi_irq+0x84/0x13c)
	[<8007374c>] (handle_fasteoi_irq+0x84/0x13c) from [<80070f9c>] (generic_handle_irq+0x2c/0x40)
	[<80070f9c>] (generic_handle_irq+0x2c/0x40) from [<80024030>] (asm_do_IRQ+0x30/0x84)
	[<80024030>] (asm_do_IRQ+0x30/0x84) from [<8002978c>] (__irq_svc+0x4c/0xc8)
	Exception stack(0x8782fdc8 to 0x8782fe10)
	fdc0:                   8782c000 8782c5c0 8782fdb0 00000000 8782e000 8782c000
	fde0: 803cf2b0 8782c5c0 8782c168 8782e000 803cb60c 8782fe94 803cf2b0 8782fe10
	fe00: 802dd288 802dd28c 60000113 ffffffff
	[<8002978c>] (__irq_svc+0x4c/0xc8) from [<802dd28c>] (__schedule+0x13c/0x43c)
	[<802dd28c>] (__schedule+0x13c/0x43c) from [<802dde9c>] (schedule_timeout+0x148/0x1e4)
	[<802dde9c>] (schedule_timeout+0x148/0x1e4) from [<8001d280>] (ip_auto_config+0x89c/0xeb8)
	[<8001d280>] (ip_auto_config+0x89c/0xeb8) from [<800243f0>] (do_one_initcall+0x34/0x174)
	[<800243f0>] (do_one_initcall+0x34/0x174) from [<800088fc>] (kernel_init+0x74/0x110)
	[<800088fc>] (kernel_init+0x74/0x110) from [<8002a4ec>] (kernel_thread_exit+0x0/0x8)
	---[ end trace 29f4b0ef99030a27 ]---

     * Transmit stops in mid-flow. (ESW IP seems to be frozen.)
