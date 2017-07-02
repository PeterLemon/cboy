target = cboy
csources = $(wildcard *.c)
objects = $(csources:.c=.o)
CFLAGS= -std=c99 -O3 -ggdb -Wall -Werror `sdl2-config --cflags`
CXXFLAGS= -O3 -ggdb -Wall `sdl2-config --cflags`

.PHONY: all
all: $(target)

.INTERMEDIATE: bootrom.c

ifneq ($(MAKECMDGOALS),clean)
-include $(sources:%.c=deps/%.d)
endif

$(target): Makefile $(objects)
	$(CXX) $(CFLAGS) $(objects) `sdl2-config --libs` -o $(target)

.PHONY: clean
clean:
	rm -Rf $(target) *.o deps
	make -C tools clean

deps/%.d: %.c
	@mkdir -p deps
	@set -e; rm -f $@; \
	$(CC) -MM $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

tools/bin2c: Makefile
	make -C tools

bootrom.c: ../bootrom/bootrom.bin tools/bin2c
	tools/bin2c bootrom_bin < $< > $@
