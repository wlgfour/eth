mkdir tmpFiles

gcc -c ethlib.c -o tmpFiles/ethlib.o
gcc -c myRecv.c -o tmpFiles/myRecv.o
gcc -c mySend.c -o tmpFiles/mySend.o

gcc tmpFiles/myRecv.o tmpFiles/ethlib.o -o myRecv -lncurses -lpthread
gcc tmpFiles/mySend.o tmpFiles/ethlib.o -o mySend -lncurses -lpthread

rm -r tmpFiles
