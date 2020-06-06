all:
	gcc -o lab5 lab5.c -lm;
	./lab5 W
#Here, just change the W to any file/directory name of your choice
clean:
	rm lab5
