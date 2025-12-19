CMS = /nix/store/1q6p1br0kmqnx0dcb1mpqiydjaz14zwl-cryptominisat
INCLUDES = -I$(CMS)/include/cryptominisat5 -I/usr/include/cairo -I/usr/include/libpng16 -I/usr/include/freetype2 -I/usr/include/pixman-1

CXX = g++
# OPT = -g
OPT = -O3  -DNDEBUG
CXXFLAGS = -std=c++17 -Wall -MMD $(INCLUDES) $(OPT)
LIBS = -lcairo

OBJECTS = tt.o tt_norefs.o viz.o viz_norefs.o mcc.o mcc_norefs.o
DEPENDS = ${OBJECTS:.o=.d}

all: tt tt_norefs viz viz_norefs mcc mcc_norefs

mcc: mcc.o 
	$(CXX) $(LIBS) -o mcc mcc.o -L$(CMS)/lib -lcryptominisat5
mcc_norefs: mcc_norefs.o 
	$(CXX) $(LIBS) -o mcc_norefs mcc_norefs.o -L$(CMS)/lib -lcryptominisat5


tt: tt.o 
	$(CXX) $(LIBS) -o tt tt.o -L$(CMS)/lib -lcryptominisat5

tt_norefs: tt_norefs.o 
	$(CXX) $(LIBS) -o tt_norefs tt_norefs.o -L$(CMS)/lib -lcryptominisat5

viz: viz.o
	$(CXX) $(LIBS) -L/opt/local/lib -o viz viz.o -lcairo
viz_norefs: viz_norefs.o
	$(CXX) $(LIBS) -L/opt/local/lib -o viz_norefs viz_norefs.o -lcairo


-include ${DEPENDS}

.PHONY: clean

clean:
	rm ${OBJECTS} ${DEPENDS}
