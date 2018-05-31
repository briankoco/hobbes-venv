# Hobbes-venv

Scripts and infrastructure for generating simple busybox Linux environments
compatible with the Hobbes node virtualization layer (NVL)

### Prerequisites

* If you just want to play around with the infrastructure, a pre-built isoimage
containing busybox and a 3.8.1 Linux kernel is shipped with this repository
(`images/linux3-8-1-with-nvl.iso`). This image can boot on raw x86_64 hardware,
but it is easier to play around with via QEMU. Prerequisites for this image
are:

    * QEMU 2.7<sup>1</sup>

* On the other hand, if you want to be able to build images with full NVL support
and with your own custom kernel images, you need the following:

    * QEMU 2.7<sup>1</sup>
    * Autoconf
    * Autoheader
    * gcc
    * git
    * Compiled version of the Linux kernel<sup>2</sup>

* If you plan to leverage QEMU/KVM to boot into the NVL, and you want Palacios VMM
support, you need to enable nested virtualization in KVM:

    * For AMD processors: `echo 1 > /sys/module/kvm_amd/parameters/nested`
    * For Intel processors: `echo 1 > /sys/module/kvm_intel/parameters/nested`

<p>
<sup>1</sup>
<sub>If you want **Palacios virtualization** support in the image, then you
need to have at least QEMU version 2.7 installed. This is because older
versions of QEMU do not properly expose MSRs needed to support nested
virtualization. The rest of the NVL infrastructure (Leviathan, XEMEM, Pisces,
Petos) will function with older versions of QEMU. It is not difficult to install
QEMU via source, should you desire Palacios functionality
</sub>
<br>
<sup>2</sup>
<sub>A compiled kernel is required (i.e., a <code>bzImage</code>), not just
kernel headers. Headers are sufficient for building several of the Hobbes NVL
components, but an image is needed for booting into the environment.
</sub>
</p>

## Quick Start

Invoke: 

    ./run-in-qemu.sh images/lnx3-8-1-with-nvl.iso

to boot a pre-built version of the Hobbes NVL with version 3.8.1 of the Linux
kernel via QEMU/KVM.

### Login information

Use these credentials to login to the initramfs. These are the same credentials
to use for the guest initramfs as wel;.

* **username:** root
* **password:** hobbes


## Customizing your Builds

If you plan to make modifications to the Hobbes NVL components, or you want to
add different utilities/applications/etc. to the initramfs, then you will need
to setup a build environment. 

1. The first step to do so is to run the setup script:

    ```
    ./setup.sh
    ```

    This will clone any required external repositories, and then configure and
    complile them.

2. Open the `config.sh` file with a text editor of choice. In it, you will see
   several environment variables required by the build process. Most have default
   values that you probably don't need to modify at first. *The one exception* is
   the `KERNEL_SOURCE` variable, which you must set to the location of the Linux
   source code that you want to run your NVL environment in:
   ```
   KERNEL_SOURCE=${KERNEL_SOURCE:-""}
   ```

   For each variable in `config.sh`, you can modify the value in 1 of 2 ways:
    * Modify the value to the right of the `:-` operator to change the *default*
    * Set the value in your environment

   Once you have determined your desired settings, invoke the build script as follows:

   ```
   key1=val1 key2=val2 ... ./build.sh
    ```

   where key/val pairs are your desired environment values. You can also export them via
   `export` in most shells, rather than prepending them to the `./build.sh` command.
   
3. Assuming you left the `WANT_ISOIMAGE` option on, if the build script
   succeeds you will have an isoimage located in `images/image.iso`. This image
   can now be booted on hardware via, e.g., USB, or loaded via QEMU/KVM using
   the `./run-in-qemu.sh` command. If you chose to disable `WANT_ISOIMAGE`, two
   files will be created: `images/initrd.img` and `images/bzImage`, which are the
   initrd and kernel images respectively, which can also be loaded via QEMU, or booted 
   on hardware via, e.g, PXE.

4. At any point in time you can add additional programs and libraries to the
   initramfs. The safest way to do so is to compile everything statically in your
   host build environment and then copy the binaries into the initramfs (e.g.,
   under the opt/ directory). However, dynamic binaries are supported as well,
   provided all of their shared libraries are present. You can use `ldd` on the
   binary to determine its dependencies, and if they are not present under
   `initramfs/lib` , you will need to manually copy them in.


### `config.sh` configuration options

This section enumerates the `config.sh` configuration options:

* `KERNEL_SOURCE`: top level directory of a **pre-compiled** Linux kernel
* `WANT_LEVIATHAN`: (0/1) whether or not to install the
  [Leviathan](http://www.prognosticlab.org/leviathan/) framework in your initramfs
    * **Note:**Currently, Leviathan utilities must be pre-compiled using the
      setup scripts in the [Leviathan repository](https://gitlab.prognosticlab.org/prognosticlab/leviathan)
    * See the section Building Leviathan below

* `LEVIATHAN_SOURCE`: top level directory of a **pre-compiled** Leviathan source tree.
  Only used if `WANT_LEVIATHAN` is set to 1

* `WANT_MODULES`: (0/1) whether or not to install kernel modules from `KERNEL_SOURCE` into
  the `/lib/modules/` directory of the initramfs

* `WANT_ISOIMAGE`: (0/1) whether or not to build an isoimage containing the initramfs and 
  the kernel bzImage

* `WANT_GUEST_ISOIMAGE`: (0/1) whether or not to build an isoimage containing
  the `guest-initramfs`, and to store this isoimage within the initramfs itself. This is
  useful if you want to run the Palacios VMM in the resulting NVL image

* `GUEST_KERNEL_SOURCE`: top level directory of a **pre-compiled** Linux kernel, but to be
  used as the guest OS kernel. Defaults to `KERNEL_SOURCE`. Only used if `WANT_GUEST_ISOIMAGE`
  is set to 1



### Building Leviathan

If you want to install the full Hobbes NVL infrastructure, you need to download and compile
the Leviathan framework. 

1. First run:
    ```
    git clone https://gitlab.prognosticlab.org/prognosticlab/leviathan.git
    ```

2. You need to ensure that the Leviathan components are built against the same
   version of the Linux kernel that you are specified in `config.sh`. The easiest
   way to do this is to export the following variable in your current shell:

    ```
    export LINUX_KERN=*path to Linux kernel*
    ```

   Then, from the top level of the source code, run:
    ```
    ./setup.sh
    ```

3. If you plan to use Palacios in your image, you need to modify the Palacios
   configuration to enable serial and virtio consoles. To do so, set the
   following two options:

   * cd <leviathan_dir>/palacios
   * `make menuconfig`
       1. Target Configuration --> Host Interfaces --> [x] Stream Support
       2. Virtual Devices --> [x] Stream based character frontend

   You will need to make these changes in both `<leviathan_dir>/palacios` and
   `<leviathan_dir>/palacios-lwk`

   
4. Finally, invoke:

    ```
    make
    ```

   from the top level of the Leviathan tree to build each of the NVL components against
   this kernel version.

If these steps succeed, you can then set the `WANT_LEVIATHAN` to 1 and
`LEVIATHAN_SOURCE` to the top of the Leviathan source tree in your `config.sh`
file. This will allow the build script to install Leviathan utilities in the
initramfs under the `initramfs/opt/` directory.


## Linux Kernel Configuration Settings

The NVL infrastrucutre relies on a set of configuration options in the host Linux
kernel. We are aware of at least the following requirements:

* CONFIG_MEMORY_HOTPLUG=y
* CONFIG_MEMORY_HOTREMOVE=y
* CONFIG_MMU_NOTIFIER=y
* CONFIG_HOTPLUG_CPU=y
* \# CONFIG_DEBUG_PAGEALLOC is **not set** (this **must** be disabled)
* CONFIG_UEVENT_HELPER="/sbin/hotplug" (required for compatibility with the busybox `mdev` utility)

Furthermore, to enable serial console access via QEMU, the kernel needs (at
least) the following options:

* CONFIG_SERIAL_8250=y
* CONFIG_SERIAL_8250_CONSOLE=y

### Nested **Guest** Kernel Configuration Settings

Generally, you can use the same kernel configuration for nested Palacios VMs. However, it helps
greatly if you also have the Linux virtio console driver compiled in the guest:

* CONFIG_VIRTIO=y
* CONFIG_VIRTIO_PCI=y
* CONFIG_VIRTIO_PCI_LEGACY=y
* CONFIG_VIRTIO_MMIO=y
* CONFIG_HVC_DRIVER=y
* CONFIG_VIRTIO_CONSOLE=y


## Known Issues

1. **Guest kernel panics during boot process**

    When running images via QEMU, the `run-in-qemu.sh` can be configured to use
    the `-cpu host` model to emulate the exact CPU model of the host machine,
    which is the most reliable way to ensure that the emulated CPU has
    virtualization extensions required for Palacios. However, `-cpu host`
    frequently causes problems in QEMU/KVM, including kernel panics during the
    boot process, usually as a result of `rdmsr/wrmsr` instructions.

    There are two general workarounds to this problem:

    1. If the panics are MSR related, use: 

        ```echo 1 > /sys/module/kvm/parameters/ignore_msrs``` 
        
       on the host kernel to tell KVM not to inject a GPF into the guest when
       it accesses non-virtualized MSRs.

    2. Remove the `-cpu host` flag from the QEMU command line. This will likely
       prevent Palacios from working in the VM, but the remaining NVL framework
       should still function.

2. **Failed to execute /init during boot process**

    This is likely also an issue due to `-cpu host`; if you compiled the isoimage
    on a different system/architecture than you're running on, this error will
    likely result
 
## TODO

1. Automate kernel build process
    * And double check config against the required options
2. Automate Leviathan build process
3. Support mounting of a persistent root filesystem

## Authors

* **Brian Kocoloski**
