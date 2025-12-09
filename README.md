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
