all:		image
		display raytrace.png

image:		test
		./test 2> debug.log | convert pam:- raytrace.png

.PHONY: clean
clean:
		rm -f *.o *~ test raytrace.pbm raytrace.pam

test:		test.o triangle.o toolbox.o shade.o load.o
		cc -o test test.o triangle.o toolbox.o shade.o load.o -lm

test.o:		test.c types.h toolbox.h
		cc -Wall -c test.c

triangle.o:	triangle.c types.h toolbox.h
		cc -Wall -c triangle.c

toolbox.o:	toolbox.c types.h toolbox.h
		cc -Wall -c toolbox.c

shade.o:	shade.c types.h toolbox.h
		cc -Wall -c shade.c

load.o:		load.c types.h toolbox.h
		cc -Wall -c load.c
