# XBase Sample Project Template

This directory contains a sample Makefile and minimal C program. The Makefile will pull in sources recursively from `src`, and will copy binary data from `res` into the output directory.

## Environment

It is assumed that `human68k-gcc` and co. are already present in your PATH.

[TODO: Why not tell these fine readers how to get human68k-gcc?]

### Getting Started

Copy the contents of this directory to a new repository, and then run the following:

```
	$ git submodule add git@github.com:mikejmoffitt/xbase
	$ git submodule update
```

Doing this adds XBase as a submodule and pulls in the requisite files.

### Structure

The directory structure of the project is as follows:

	Makefile
	xbase/          XBase git submodule.
	res/            Binary resources packaged with application.
	src/            Source files (.c and .a68)

The following files are build artifacts.

	obj/            Object files from compilation.
	out/            The compiled application alongside data from res/.
	(APPNAME).map   Map file generated from compilation.

## Building

If a source file that defines the symbol `main` is present in `src/`, and `human68k-gcc` is in your path, all you have to do is invoke make.

```
	$ make
```

If all is well, a .X file will reside in `out/`. You may delete `out/` and other build artifacts by running the clean target.

```
	$ make clean
```

### C Compilation

C sources have a number of compiler flags set that may be checked in the Makefile, but a few are being called out here for clarity of purpose:

`-fcall-used-d2 -fcall-used-a2`
These flags are set so as to conform with the ABI used with older X68000 C compilers. In particular, these flags were chosen for compatibility with XSP2LIB linkage.

### Assembly

Assembler sources use the filename extension `.a68` and are assembled with `assembler-with-cpp`. This means that the C preprocessor #define and friends may be used, allowing for a limited amount of "code" reuse between assembly and C.

## Testing

### On Hardware: SCSI2SD V6
My preferred way of testing on hardware is to use a SCSI2SD V6 SCSI disk emulator. Aside from being able to use an SD card as a SCSI hard disk, it also supports a multitude of other SCSI devices.
It is possible to emulate a Magneto-optical on a second device ID, in conjunction with any other devices. A SCSI2SD V6-unique feature is the ability to read and write from these SCSI devices over USB, even while mounted and in use in the target machine.
We are fortunate that Human68k is able to mount and read a DOS Filesystem from MO media (as long as all filenames conform to an uppercase 8.3 naming scheme). As a result of this, a test process can be developed as follows:

#### First Setup
* Create an MO device on the SCSI2SD V6 (with a unique vendor name `x68k` and device name `DEVDISK`)
* Format the MO device with Human68k (the first time)
* Mount the MO drive using `SUSIE.X <Drive>: -ID<SCSI ID>`

#### During Testing
* After compiling, mount the MO device on host PC (taking advantage of the device name declared for ease)
* After compiling the program, copy contents of `out/` to MO mount point
* On Human68k, go to the drive and run `dir` to clear any filesystem cache
* Run the program normally from Human68k

All of the above is a little janky, but to date it is the most efficient way of iterating on hardware that I have found.

I have made a make target that encompasses this process called `upload`:

```
	$ make upload
```

Those who have worked in the embedded industry for a long time may gripe at the use of this word and prefer had I written `download`, but I digress.

### On Harware: Floppy

The contents of `out/` may simply be copied to an appropriately formatted floppy disk. 


### On Emulator: XM6

I like to use XM6 for debugging on occasion, as it has the ability to mount a directory as a floppy drive. Thus, simply mounting `out/` as a floppy is adequate. However, under the hood, I am fairly certain this is simply creating a temporary floppy image each time; if the contents have been changed, you must remember to go to the menu and do it again.
