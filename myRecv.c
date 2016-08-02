#include </home/gerecke/GitProjects/eth.git/ethlib.h>

//http://man7.org/linux/man-pages/man7/packet.7.html
//http://man7.org/linux/man-pages/man7/netdevice.7.html
//http://lxr.free-electrons.com/source/include/uapi/linux/if_ether.h#L31
//https://en.wikipedia.org/wiki/EtherType
//http://linux.die.net/man/3/htons
//https://www.kernel.org/doc/Documentation/input/event-codes.txt
//http://stackoverflow.com/questions/20943322/accessing-keys-from-linux-input-device
//http://yurovsky.blogspot.com/2010/02/heres-another-quick-linux-input-snippet.html

//tcpdump to observe packets(sudo tcpdump -i iface -XX)
//remember to turn promiscuous mode on!!!
//|||monitor mode works when not on WAP
//>>ifconfig wlan1 down
//>>iwconfig wlan1 mode monitor

int main(int argc, int* argv) {

	int i = 0, j = 0, curprint = 0, cbx = 0, cby = 0, paccounter, psiz, x, y;
	char refrsh;
	pthread_t readt;
	pthread_mutex_t pacount, data;
	pthread_mutex_init(&pacount, NULL);
	pthread_mutex_init(&data, NULL);
	targs args;
	memset(&args, 0, sizeof(args));
	const char *dev = "/dev/input/by-path/platform-i8042-serio-0-event-kbd";
	int fd = open(dev, O_RDONLY);//^and < declare keyboard device
	printf("initializing packet buffers...\n");
	args.datam = data;
	args.currt = -1;
	while(i < MAXPACBUF) {
		printf("%d.", i);
		while(j < ETH_FRAME_LEN) {
			args.llist[i].buf[j] = -1;
			args.llist[i].psiz = 32;
			j++;
		}
		i++;
	}
	printf("\n->done initialising\n");
	i = 0;
	j = 0;
	init0();
	getmaxyx(stdscr, y, x);
	WINDOW *winparse = newwin(6, x, 0, 0);
	WINDOW *winhex = newwin(50, (int)((x/3)*2), 7, 0);
	WINDOW *winalpha = newwin(50, (int)(x/3)-2, 7, (int)((x/3)*2)+2);
	pthread_create(&readt, NULL, &readpack, (void *)&args);
	while(1) {
		if(rdKey(fd, KEY_UP)) {//refresh pack
			refrsh = 'u';
		}else if(rdKey(fd, KEY_DOWN)) {//refresh pack
			refrsh = 'd';
			if(cby > 0)
				cby +=1 ;
		}else if(rdKey(fd, KEY_C)) {//if continue || jumo && not first packet
			refrsh = 'c';
			cby = 0;
		}else if(rdKey(fd, KEY_J)) {//jump to current read
			refrsh = 'j';
			cby = 0;
		}else if(rdKey(fd, KEY_LEFT)) {//move back one
			refrsh = 'l';
			curprint = mod(curprint-1, MAXPACBUF);
			cby = 0;
		}else if(rdKey(fd, KEY_RIGHT)) {//move forward one
			refrsh = 'r';
			curprint = mod(curprint+1, MAXPACBUF);
			cby = 0;
		}else if(rdKey(fd, KEY_Q)) {//quit
			end();
			return;
		}
		pthread_mutex_lock(&args.datam);
		if(((refrsh == 'u'|| refrsh == 'd' || refrsh == 'c' || refrsh == 'j') || (paccounter != curprint)) && args.currt != -1) {//needs tuning
			if(refrsh == 'u') {
				if(cby < args.llist[curprint].psiz)
					cby -= 1;
			}
			if((refrsh == 'c' || refrsh == 'j') && args.currt != -1) {
				curprint = args.currt;
			}
			paccounter = curprint;
			pbuf0(winhex, args.llist[curprint].buf, args.llist[curprint].psiz, 'x', cby);
			pbuf0(winalpha, args.llist[curprint].buf, args.llist[curprint].psiz, 'a', cby);
			pPackW(winparse, args.llist[curprint].psiz, args.llist[curprint].buf);
		}else if(args.currt < 0){
			mvprintw((int)y/2, (int)(x/2)-12, "Waiting for first packet");refresh();
		}
		mvprintw(y-1, 0, "current print:%d; current read:%d; auto-continut:%s; last key:%c; \tmax read before overwrite:%d;", \
			curprint, args.currt, refrsh=='j'?"on":"off", refrsh, MAXPACBUF-1); refresh();
		pthread_mutex_unlock(&args.datam);
		if(refrsh == 'c')
				refrsh = '0';
		usleep(80000);
	}
}
