DEPENDENCIES and INSTALLATION:

This program requires a graphics card that supports modern opengl, and can run on Windows and Linux. It has been tested on
Windows 7 and Red Hat Enterprise Linux. On windows, it should be compiled using the MSVC 2013 compiler. On linux, it should
compile using g++ or clang. On either platform, c++11 compliance is required.

This program requires Jon Macey's NGL library, which can be found at:

https://github.com/NCCA/NGL

The program was developed using the Qt Creator IDE. It comes with a qt project, which should hold all the necessary compiler
options. It also comes with a makefile, which should allow the project to be built by using the 'make' command in the terminal 
on linux.

USAGE:

Once the program is built, we need to give it music to work with. This program currently works only with stereo, 16-bit WAV files. 
A 441000 Hz sampling rate is recommended.

config.txt
Contains a set of parameters that the user can set before the program is run. None of these should be deleted. Comments begin 
with a # character.

resources/sources.txt
Contains a list of filenames that will be loaded on program startup which are relative to the resources/songs/ folder. Only
one filename should be in this file at one time.

resources/songs/
Place your sounds in this folder.

At runtime, the user can input commands into the terminal to manipulate the program.Type 'help' into the terminal to
see a list of the commands, and what they do:

order : Prints the order of the markov model.
order <arg> : Sets the order of the markov model to <arg>.
visualise : Visualise the model. This is the most important command.
add <arg> : Add a source named <arg> from within the resources/songs/ folder.
clear : Clears the markov model.
quit : Quits the program.

