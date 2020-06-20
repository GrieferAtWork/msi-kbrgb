
# msi-kbrgb
A single-source-file C program to change set the colors of an MSI keyboard.

# Disclaimer
I did not reverse engineer the /magic/ used here myself. And looking around github in regards to the subject of home-made programs to set the colors of these keyboard, there's already more than one of them. However, none of the ones I managed to find were what this repo now holds: A simple, single-source-file, (probably portable), commandline C program with minimal dependencies

# The Magic
There really isn't too much to it, but you can just look between the `/* BEGIN: Magic */ ... /* END: Magic */` regions to take a look at the /magic/ constants and command sequences that must be sent to an HID device `vendor_id: 0x1770, product_id: 0xff00`

# Usage
The program is self-documenting, and you can run `kbrgb --help`, but here are some examples:
	- `kbrgb m:normal p:all:green:h` (Make the whole keyboard green)
	- `kbrgb m:normal p:l:red:h p:m:green:h p:r:blue:h` (Make the whole keyboard RGB (RedGreenBlue))
	- ...

# Dependencies
You will need to have installed:
	- A couple of standard-C headers (see top of `main.c`)
	- [`hidapi`](https://github.com/signal11/hidapi)
		- If you're using cygwin (like me), you don't even have to install this one from source.  
		  Instead, from the cygwin installer, install the following packages:
			- `libhidapi-devel`
			- `libhidapi0`
And that's it

# Building
Just run `make` from the cloned repo, or do your own `gcc -o kbrgb main.c -lhidapi` (after all: it's only a single source file)

# Tipps
Since (at least for my laptop), the default color of the keyboard is all-red after boot (and I have yet to find a way of changing this), I've had the quite neat idea of setting up scheduled tasks to run the following:
	- During logon: `kbrgb m:normal p:all:green:h`
	- During logoff: `kbrgb m:normal p:all:red:h`
Meaning that while I'm logged in, the keyboard is green, but while I'm logged off, it's red (and since red is the default color, it will also be red until I log on for the first time)
I think that's rather neat :)



