# f25-research
Results of fall 2025 research on the tiling of marked hexagons

Download cryptominisat and cairo to use. Edit Makefile in src

Usage for coronas:

```
./corona_checker < example.in > example.out
./viz < example.out
```

Creates out.pdf to display image. Look at c_example.txt for example of input

Usage for tt (translational tilings) Edit parallelograms.py. Change lim, upper, and lower (inclusive). Change line 3 of tt1 or tt2 to "anything_you_want.h". tt1 and tt2 are identical except tt1 also has a timeout limit (in seconds) that can be changed.

```
python3 parallelograms.py > anything_you_want.h
make tt1
./tt1 < example.in > example.out
./viz < example.out
```

Look at i_example.txt for example of input


## more_markings_src
In more_markings_src, there is support for multiple markings, 0 matches to 0, a matches to A, b matches to B, etc, as well as disabling reflections.

Building and running is same as in src, (mcc is the marked corona checker), only difference is that the file format for mcc looks like:
```
N
M Q shape1 shape2 ... shapeM C
...
```
Where N is the number of pages, followed by N lines of the format 

M Q shape1 shape2 ... shapeM

where M is the number of shapes, and Q is the maximum number of markings. C is the number of coronas to generate. For tt, file format is the same except there is no value C at the end of each line

The _norefs files don't allow reflections, and currently viz files in more_marking_src supports up to 4 different tiles and up to 3 different markings.
