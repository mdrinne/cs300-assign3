all: p536 pp3

p536: p536.c
	gcc p536.c -o p536 -lpthread

pp3: pp3.c
	gcc pp3.c -o pp3 -lpthread
