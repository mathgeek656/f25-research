CMS = /nix/store/1q6p1br0kmqnx0dcb1mpqiydjaz14zwl-cryptominisat
INCLUDES = -I$(CMS)/include/cryptominisat5 -I/usr/include/cairo -I/usr/include/libpng16 -I/usr/include/freetype2 -I/usr/include/pixman-1

CXX = g++
# OPT = -g
OPT = -O3  -DNDEBUG
CXXFLAGS = -std=c++17 -Wall -MMD $(INCLUDES) $(OPT)
LIBS = -lcairo

OBJECTS = corona_checker.o tt1.o tt2.o viz.o
DEPENDS = ${OBJECTS:.o=.d}

all: corona_checker tt1 tt2 viz

corona_checker: corona_checker.o 
	$(CXX) $(LIBS) -o corona_checker corona_checker.o -L$(CMS)/lib -lcryptominisat5

tt1: tt1.o 
	$(CXX) $(LIBS) -o tt1 tt1.o -L$(CMS)/lib -lcryptominisat5

tt2: tt2.o 
	$(CXX) $(LIBS) -o tt2 tt2.o -L$(CMS)/lib -lcryptominisat5

viz: viz.o
	$(CXX) $(LIBS) -L/opt/local/lib -o viz viz.o -lcairo

-include ${DEPENDS}

.PHONY: clean

clean:
	rm ${OBJECTS} ${DEPENDS}
