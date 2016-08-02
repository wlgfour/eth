#ifndef ETHLIB_H_INCLUDED
#define ETHLIB_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <ncurses.h>//needs -lncurses
#include <pthread.h>//needs -lpthread
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/input.h>
#include <linux/if_packet.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <net/ethernet.h>
#include <net/if.h>

#define RECV "wlan1"
#define TRANS "wlan0"
#define PACLEN 65
#define LINLEN 23
#define PROTO 0X0000
#define MAXPACBUF 1000

typedef union packet{
	struct ethhdr macs;
	unsigned char buf[PACLEN];
} Packet;
typedef struct ifreq iface;
typedef struct pPackWt_s{
	WINDOW *win;
	int psiz;
	unsigned char buf[ETH_FRAME_LEN];
} pPackW_t;
typedef struct pbuf_s{
	WINDOW *win;
	char txt[ETH_FRAME_LEN];
	char mode;
	int s1;
} pbuf_t;
typedef struct bufnode_t {
	int psiz;
	unsigned char buf[ETH_FRAME_LEN];
} bufnode;
typedef struct targs_t {
	bufnode llist[MAXPACBUF+1];
	int currt;//curent read/write
	pthread_mutex_t datam;//data
} targs;

int getMac(char *ifacef, unsigned char *mac);
//gets mac addres in hex from file descriptor
int socDes(struct sockaddr_ll pack, char *interface, int mysoc);
//fill out sockaddr_ll socket descriptor
int packDes(Packet packSnd, char *trans, char *recv, unsigned short proto, unsigned char buf[PACLEN-ETH_HLEN], int size);
//fill out Packet union (struct ethhdr & unsigned char buf[])
int pPack(unsigned char *buf, int size);
//prints contents of packet given char bufand sizeof(buf)
int mod(int a, int b);
//modulo that works with less than 0
///////////////////////////////////////////////////////////////////////////////////////////////
int init0();
//initiate ncurses
int end();
//uninitiate ncurses
int winit0(int *coords[4], int number, WINDOW **windows);
//initiate array of windows
int menueDraw0(WINDOW *win, char *txt[], int s1, int s2, int highlight, int bx, int by);
//draw a txt buffer to window
int menue0(WINDOW *win, char *txt[], int s1, int s2);
//uses above function and adds selection options
int ppad0(WINDOW *win, char *txt, int s1, int bx, int by, int xrng, int yrng, char mode);
//writes text to buffer
int pbuf0(WINDOW *win, char *txt, int s1, char mode, int cby);
//draws txt on buffer with arrow key scrolling
int pPackW(WINDOW *win, int psiz, unsigned char *buf);
//parses ether header and prints to *WINDOW
int rdKey(int fd, int key);
//taked file descriptor and key code(number) returns 1 if pressed 0 else
void *readpack(void *argp);
//reads incomming packets

#endif
