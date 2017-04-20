mdriver:	mdriver.c memory.h libmm.a
	gcc -g -o mdriver mdriver.c -L. -lmm


libmm.a:	mm_free.o mm_malloc.o mm_realloc.o	
	ar rs libmm.a 	mm_free.o mm_malloc.o mm_realloc.o 

mm_free.o:	mm_free.c memory.h
	gcc -g -c mm_free.c

mm_malloc.o:	mm_malloc.c memory.h
	gcc -g -c mm_malloc.c

mm_realloc.o:	mm_realloc.c memory.h
	gcc -g -c mm_realloc.c

mdriver.o:	mdriver.c memory.h
	gcc  -g -c  mdriver.c

clean:	
	rm -f *.o libmm.a mdriver

cleantraces:	
	rm -f trace*.out*


