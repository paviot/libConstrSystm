LIB = libConstrSystm.a

#SRCS = Variables.cpp LinearExpr.cpp LinearCstr.cpp LinearSyst.cpp Utilities.cpp lps.cpp Flots.cpp
SRCS = Variables.cpp LinearExpr.cpp LinearCstr.cpp LinearSyst.cpp Utilities.cpp lps.cpp
INCD = /opt/local/include ../lp_solve_5.5.0.13 ../libCamiProdRead

OBJS = $(SRCS:.cpp=.o)
DEPS = $(SRCS:.cpp=.d)
INCDIR = $(addprefix -I, $(INCD))

CXX = g++
CXXFLAGS= -O3  -Wall $(INCDIR) #-pg
#CXXFLAGS= -g3 -O0  -Wall $(INCDIR) -D_GLIBCXX_DEBUG
#CXXFLAGS= -g3 -O0  -Wall $(INCDIR)

all: $(LIB)

$(LIB): $(OBJS)
	ar ru $(LIB) $(OBJS)
	ranlib $(LIB)
	doxygen Doxyfile

Variables.o: Variables.cpp Variables.h
LinearExpr.o: LinearExpr.cpp Variables.h LinearExpr.h LinearCstr.h
LinearCstr.o: LinearCstr.cpp Variables.h LinearExpr.h LinearCstr.h
LinearSyst.o: LinearSyst.cpp Variables.h LinearExpr.h LinearCstr.h LinearSyst.h Utilities.h
Utilities.o: Utilities.cpp Utilities.h
lps.o: LinearSyst.cpp Variables.h LinearExpr.h LinearCstr.h LinearSyst.h Utilities.h lps.h
Flots.o: Flots.cpp

clean:
	rm -f $(LIB) *.o *.d *.a *~ 

# implicit rules

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $<

%.tab.cpp: %.y
#  -p cpr_
	bison -d -p cpr_ $<

	mv $*.tab.c $*.tab.cpp

%.lex.cpp: %.l 
	flex  -o$*.lex.cpp -Pcpr_ $<

%.d: %.cpp
	@$(SHELL) -ec '$(CXX) -M $(CXXFLAGS) $(INCLUDES) $< \
                | sed '\''s/\($*\)\.o[ :]*/\1.o $@ : /g'\'' > $@; \
                [ -s $@ ] || rm -f $@'

-include $(DEPS)        












