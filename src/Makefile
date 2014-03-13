# variables
OBJECTS = $(foreach object, $(basename $(wildcard *.cc)), $(OBJDIR)/$(object).o)

# compiling vars
INCLUDE = -I../$(INCDIR)
SDL_FLAGS = `sdl-config --cflags`
LIB = `sdl-config --libs`
#LD_FLAGS = -lm -lSDL_image -lSDL_mixer -pg
LD_FLAGS = -lm -lSDL_image -lSDL_mixer


# targets
all: $(PROGNAME)

$(PROGNAME): $(OBJDIR) $(OBJECTS)
	$(CXX) -o $@ $(OBJECTS) $(LIB) $(LD_FLAGS)

$(OBJDIR):
	mkdir $@

clean:
	-rm -rf $(OBJDIR) *.o $(PROGNAME) core a.out *.log tags

$(OBJDIR)/%.o: %.cc ../include/%.h
	$(CXX) -o $@ $(INCLUDE) $(CFLAGS) $(SDL_FLAGS) -c $<
