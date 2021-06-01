all:		image
		display raytrace.pam

image:		test
		./test | convert pam:- raytrace.png

.PHONY: clean
clean:
		rm -f *.o *~ test raytrace.pbm raytrace.pam

test:		test.o triangle.o toolbox.o shade.o load.o
		cc -o test test.o triangle.o toolbox.o shade.o load.o -lm

test.o:		test.c types.h toolbox.h
		cc -c test.c

triangle.o:	triangle.c types.h toolbox.h
		cc -c triangle.c

toolbox.o:	toolbox.c types.h toolbox.h
		cc -c toolbox.c

shade.o:	shade.c types.h toolbox.h
		cc -c shade.c

load.o:		load.c types.h toolbox.h
		cc -c load.c
