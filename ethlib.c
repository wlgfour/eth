#include </home/gerecke/GitProjects/eth.git/ethlib.h>

//http://www.humbug.in/2014/find-network-interface-mac-address-programatically-linux/
//ex: wlan0, MAC;
int getMac(char *ifacef, unsigned char *mac) {

	char *p1 = "/sys/class/net/";
	char *p2 = "/address";
	char *filename, addr[13];
	filename = malloc(sizeof(addr)+sizeof(p1)+sizeof(p2)+sizeof(char));
	memset(filename, 0, sizeof(filename));
	strcat(filename, p1);
	strcat(filename, ifacef);
	strcat(filename, p2);
	FILE *fp = fopen(filename, "r");
	if (fp != NULL) {
		char c;
		int i = 0;
		while(i<12) {
			c = fgetc(fp);
			if( c != ':' ) addr[i++] = c;
		}
		addr[12] = '\0';

		printf("%s: %s\n", ifacef, addr);

		char hol[2];
		int k = 0; i = 0;
		while(i<6) {
			hol[0] = addr[k];
			hol[1] = addr[k+1];
			mac[i] = (int)strtol(hol, NULL, 16);
			k+=2; i++;
		}
	}else printf("error opening %s", filename);

	fclose(fp);
	return 0;
}

int socDes(struct sockaddr_ll sock, char *interface, int mysoc) {
	iface idx;

	memset(&idx, 0, sizeof(iface));
	memset(&sock, 0, sizeof(struct sockaddr_ll));

	strncpy(idx.ifr_name, interface, IFNAMSIZ-1);
	ioctl(mysoc, SIOCGIFINDEX, &idx);

	sock.sll_family = AF_PACKET; printf("family: int-%d hex-%X\n", AF_PACKET, AF_PACKET);
	sock.sll_ifindex = idx.ifr_ifindex; printf("ifndex: int-%d hex-%X\n", idx.ifr_ifindex, idx.ifr_ifindex);
	sock.sll_protocol = htons(ETH_P_ALL); printf("protocol: int-%d hex-%X\n", htons(ETH_P_ALL), htons(ETH_P_ALL));//defaults to socket protocol
	sock.sll_halen = ETH_ALEN; printf("halen: int-%d hex-%X\n", sock.sll_halen, sock.sll_halen);
	getMac(interface, sock.sll_addr);//sll_addr and sl_halen

	return 0;
}

int packEthDes(PacketEth packSnd, char *trans, char *recv, unsigned short proto, unsigned char buf[ETH_FRAME_LEN-ETH_HLEN], int size) {
	printf("pack size: %d\n\theader: %d\n\tbody: %d\n", size, ETH_HLEN, size-ETH_HLEN);
	
	memset(&packSnd, 0, ETH_FRAME_LEN);//zero out union

	int i = ETH_HLEN;
	while(i<size) {//fill out payload (buf)
		packSnd.buf[i] = buf[i-ETH_HLEN];
		i++;
	}

	printf("trans: "); getMac(trans, packSnd.macs.h_source);
	printf("recv: "); getMac(recv, packSnd.macs.h_dest);//fill mac field
	packSnd.macs.h_proto = proto;
	printf("proto: %04x\n", htons(packSnd.macs.h_proto));
	printf("packet:\n\t");
	i = 0;
	int k = 0;//print buffer
	while(i<size) {
		printf("%02X ", packSnd.buf[i]);
		if(i == ETH_HLEN-1 || k == LINLEN) {
			printf("\n\t");
			k = 0;//break after header
		}
		i++;
		k++;
	}printf("\n");
	
	return 0;
}

int pPack(unsigned char *buf, int size) {
	int i, j;
	
	struct ethhdr hdr;
	memcpy(&hdr, buf, ETH_HLEN);
	
	printf("pack size: %d\n\theader: %d\n\tbody: %d\n", size, ETH_HLEN, size-ETH_HLEN);
	
	printf("trans: ");
	i = 0;
	while(i<ETH_ALEN) {//print mac
		printf("%02X", hdr.h_source[i]);
		if(i!=ETH_ALEN-1) printf(":");
		i++;
	}
	printf("\nrecv : ");
	i = 0;
	while(i<ETH_ALEN) {//print mac
		printf("%02X", hdr.h_dest[i]);
		if(i!=ETH_ALEN-1) printf(":");
		i++;
	}
	printf("\nproto: %04x\n", ntohs(hdr.h_proto));
	printf("packet:\n\t");
	
	j = 0;
	i = 0;//print buffer
	while(i%LINLEN != 0 || i == 0) {//header hex
		if(i < ETH_HLEN) {
			printf("%02X ", buf[i]);
		}else {
			printf("   ");
		}
		i++;
	}
	while(j<=i) {//header ascii
		if(buf[j] >= 32 && buf[j] <= 175) {
			printf("%c", buf[j]);
		}else {
			printf(".");
		}
		j++;
	}
	printf("\n\t");
	while(i < size) {//body
		printf("%02X ", buf[i]);
		i++;
		while(i%LINLEN != 0) {//hex
			printf("%02X ", buf[i]);
			i++;
		}
		while(j<=i) {//ascii
			if(buf[j] >= 32 && buf[j] <= 176) printf("%c", buf[j]);
			else printf(".");
			j++;
		}
		printf("\n\t");
	}
	printf("\n");
	
	return 0;
}
////////////////////////////////////////////////////////////////////////////////
int mod(int a, int b) {
    int r = a % b;
    return r < 0 ? r + b : r;
}

int init0() {
	initscr();
	clear();
	noecho();
	cbreak();
	curs_set(0);
	scrollok(stdscr, TRUE);
	keypad(stdscr, TRUE);
	return 0;
}
int end() {
	endwin();
	scrollok(stdscr, FALSE);
}
int winit0(int *coords[4], int number, WINDOW **windows) {
	number-=1;
	while(number >= 0) {
		windows[number] = newwin(coords[number][0], coords[number][1], coords[number][2], coords[number][3]);
		number--;
	}
	return 0;
}
int menueDraw0(WINDOW *win, char *txt[], int s1, int s2, int highlight, int bx, int by) {
	werase(win);
	int x, y, i, range = 0, high = 0, c = 0, opt = 0, lin = 0;
	char men[s1][s2];
	i = 0;
	while(i < s1) {
		strcpy(men[i], txt[i]);
		i++;
	}
	getmaxyx(win, y, x);
	lin = 1;
	wmove(win, lin, 1);
	opt = by;
	while(opt < s1 && lin < y) {//row
		if(opt == highlight) {//attribute on(highlight line)
		  	wattron(win, A_REVERSE);
			high = 1;
		}
		c = bx;
		while(c < s2) {//col
			if(c == '\n') {//new line
				lin++;
				wmove(win, lin, 1);
				
			}else {
				if( isalpha(men[opt][c]) ) wprintw(win, "%c", men[opt][c]);//print char to screen if not new line
				
			}
			if(c+1 == x) {//out of range of win line
				break;
			}else {//incriment char
				c++;
			}
			
		}
		opt++;
		lin++;
		wmove(win, lin, 1);
		if(high == 1) {//highlight off if on when option done only
			wattroff(win, A_REVERSE);
			high = 0;
		}
	}
	box(win, 0, 0);
	wrefresh(win);
	return 0;
}
int menue0(WINDOW *win, char *txt[], int s1, int s2) {//must include '\n' and '\0' in option strings
	int highlight = 0, bx = 0;
	char cha = 0;
	menueDraw0(win, txt, s1, s2, highlight, bx, highlight);
	while((cha = getch()) != 'c') {//untill selection, then return
		switch(cha) {
			case (char)KEY_UP:
				if(highlight != 0) {
					highlight -= 1;
				}
				break;
			case (char)KEY_DOWN:
				if(highlight+1 != s1) {
					highlight += 1;
				}
				break;
			case (char)KEY_LEFT:
				if(bx != 0) {
					bx -= 1;
				}
				break;
			case (char)KEY_RIGHT:
				if(bx+1 != s2) {
					bx += 1;
				}
				break;
		}
		menueDraw0(win, txt, s1, s2, highlight, bx, highlight);
	}
	return highlight;
}
int ppad0(WINDOW *win, char *txt, int s1, int bx, int by, int xrng, int yrng, char mode) {
	int i = bx, lin = by;
	unsigned char txt0[s1];
	memcpy(txt0, txt, s1);
	werase(win);
	while(i < s1 && lin < yrng) {
		if(lin == xrng) {
			wmove(win, lin, 0);
			lin++;
		}
		if(mode == 'x') {
			wprintw(win, "%02X", txt0[i]);
			if(i+1 != s1)
				wprintw(win, ":");
		}else if(mode == 'a') {
			if(txt0[i] >= 32 && txt0[i] <= 176) wprintw(win, "%c", txt0[i]);
			else wprintw(win, ".");
		}else {
			wprintw(win, "-");
		}
		i++;
	}
	wrefresh(win);
	return 0;
}
int pbuf0(WINDOW *win, char *txt, int s1, char mode, int cby) {
	int by = 0, bx = 0, x, y;
	unsigned char txt0[s1];
	memcpy(txt0, txt, s1);
	getmaxyx(win, y, x);
	ppad0(win, txt0, s1, bx, cby, x, y, mode);
	return 0;
}
int pPackW(WINDOW *win, int psiz, unsigned char *buf) {
	werase(win);
	struct ethhdr hdr;
	memcpy(&hdr, buf, ETH_HLEN);
	wattron(win, A_REVERSE);
	mvwprintw(win, 0, 0, "Ethernet:\n");
	wattroff(win, A_REVERSE);
	wprintw(win, "pack size:%d-->head:%d\n\t    -->body:%d\n", psiz, ETH_HLEN, psiz-ETH_HLEN);
	wprintw(win, "trans: ");
	int i = 0;
	while(i<ETH_ALEN) {//print mac
		wprintw(win, "%02X", hdr.h_source[i]);
		if(i!=ETH_ALEN-1) wprintw(win, ":");
		i++;
	}
	wprintw(win, "\nrecv : ");
	i = 0;
	while(i<ETH_ALEN) {//print mac
		wprintw(win, "%02X", hdr.h_dest[i]);
		if(i!=ETH_ALEN-1) wprintw(win, ":");
		i++;
	}
	wprintw(win, "\nproto: %04x", ntohs(hdr.h_proto));
	if(ntohs(hdr.h_proto) == 0x0800) {
		pIpv4W(win, psiz-sizeof(struct ethhdr), buf+sizeof(struct ethhdr));
	}else if(ntohs(hdr.h_proto) == 0x86DD) {
		pIpv6W(win, psiz-sizeof(struct ethhdr), buf+sizeof(struct ethhdr));
	}
	wrefresh(win);
}
int pIpv4W(WINDOW *win, int psiz, unsigned char *buf) {
	int lin = 0;
	union {
		struct iphdr ip;
		unsigned char buf[psiz];
	} ip;
	memset(&ip, 0, psiz);
	memcpy(&ip, buf, psiz);
	wattron(win, A_REVERSE);
	mvwprintw(win, lin, 28, "ipv%d", ip.ip.version); lin++;
	wattroff(win, A_REVERSE);
	mvwprintw(win, lin, 28, "ihl:%u", ip.ip.ihl); lin++;
	mvwprintw(win, lin, 28, "tos:%u", ip.ip.tos); lin++;
	mvwprintw(win, lin, 28, "totLen:%u", ntohs(ip.ip.tot_len)); lin++;
	mvwprintw(win, lin, 28, "id:%u", ntohs(ip.ip.id)); lin++;
	mvwprintw(win, lin, 28, "fragOff:%x", ntohs(ip.ip.frag_off)); lin++;
	lin = 1;
	mvwprintw(win, lin, 42, "ttl:%u", ip.ip.ttl); lin++;
	mvwprintw(win, lin, 42, "protocol:%u", ip.ip.protocol); lin++;
	mvwprintw(win, lin, 42, "check:%x", ntohs(ip.ip.check)); lin++;
	mvwprintw(win, lin, 42, "send:%d.%d.%d.%d", ip.ip.saddr & 0xFF, (ip.ip.saddr >> 8) & 0xFF, (ip.ip.saddr >> 16) & 0xFF, (ip.ip.saddr >> 24) & 0xFF); lin++;
	mvwprintw(win, lin, 42, "recv:%d.%d.%d.%d", ip.ip.daddr & 0xFF, (ip.ip.daddr >> 8) & 0xFF, (ip.ip.daddr >> 16) & 0xFF, (ip.ip.daddr >> 24) & 0xFF); lin++;
	wrefresh(win);
	if(ip.ip.protocol == 17) {
		pUdpW(win, psiz-(ip.ip.ihl*4), buf+(ip.ip.ihl*4), 64);
	}else if(ip.ip.protocol == 6) {
		pTcpW(win, psiz-(ip.ip.ihl*4), buf+(ip.ip.ihl*4), 64);
	}
}
int pIpv6W(WINDOW *win, int psiz, unsigned char *buf) {
	int lin = 0;
	int i = 0;
	union {
		struct ipv6hdr ip;
		unsigned char buf[psiz];
	} ip;
	memset(&ip, 0, psiz);
	memcpy(&ip, buf, psiz);
	wattron(win, A_REVERSE);
	mvwprintw(win, lin, 28, "ipv%d", ip.ip.version); lin++;
	wattroff(win, A_REVERSE);
	mvwprintw(win, lin, 28, "ihl:%u", ip.ip.priority); lin++;
	mvwprintw(win, lin, 28, "flow_lbl:%u-%u-%u", ip.ip.flow_lbl[0], ip.ip.flow_lbl[1], ip.ip.flow_lbl[2]); lin++;
	mvwprintw(win, lin, 28, "payldLen:%u", ntohs(ip.ip.payload_len)); lin++;
	mvwprintw(win, lin, 28, "nextHdr:%u", ip.ip.nexthdr); lin++;
	mvwprintw(win, lin, 28, "fragOff:%u", ntohs(ip.ip.hop_limit)); lin++;
	lin = 1;
	mvwprintw(win, lin, 46, "source--"); 
	while(i++ <4)
		wprintw(win, "%x%c", ip.ip.saddr.in6_u.u6_addr32[i-1], i==4?' ':':');
	i = 0;
	lin++;
	mvwprintw(win, lin, 46, "dest----");
	while(i++ <4)
		wprintw(win, "%x%c", ip.ip.daddr.in6_u.u6_addr32[i-1], i==4?' ':':');
	wrefresh(win);
	if(ip.ip.nexthdr == 17) {
		pUdpW(win, psiz-sizeof(struct ipv6hdr), buf+sizeof(struct ipv6hdr), 96);
	}else if(ip.ip.nexthdr == 6) {
		pTcpW(win, psiz-sizeof(struct ipv6hdr), buf+sizeof(struct ipv6hdr), 96);
	}
}
int pUdpW(WINDOW *win, int psiz, unsigned char *buf, int x) {
	int lin = 0;
	union {
		struct udphdr udp;
		unsigned char buf[psiz];
	} udp;
	memset(&udp, 0, psiz);
	memcpy(&udp, buf, psiz);
	wattron(win, A_REVERSE);
	mvwprintw(win, lin, x, "UDP"); lin++;
	wattroff(win, A_REVERSE);
	mvwprintw(win, lin, x, "source:%u", ntohs(udp.udp.source)); lin++;
	mvwprintw(win, lin, x, "dest:%u", ntohs(udp.udp.dest)); lin++;
	mvwprintw(win, lin, x, "len:%u", ntohs(udp.udp.len)); lin++;
	mvwprintw(win, lin, x, "check:%x", ntohs(udp.udp.check)); lin++;
}
int pTcpW(WINDOW *win, int psiz, unsigned char *buf, int x) {
	int lin = 0;
	union {
		struct tcphdr tcp;
		unsigned char buf[psiz];
	} tcp;
	memset(&tcp, 0, psiz);
	memcpy(&tcp, buf, psiz);
	wattron(win, A_REVERSE);
	mvwprintw(win, lin, x, "TCP"); lin++;
	wattroff(win, A_REVERSE);
	mvwprintw(win, lin, x, "source:%u", ntohs(tcp.tcp.source)); lin++;
	mvwprintw(win, lin, x, "dest:%u", ntohs(tcp.tcp.dest)); lin++;
	mvwprintw(win, lin, x, "seq:%x", ntohl(tcp.tcp.seq)); lin++;
	mvwprintw(win, lin, x, "ackSeq:%x", ntohl(tcp.tcp.ack_seq)); lin++;
	lin = 1;	
	mvwprintw(win, lin, x+14, "window:%u", ntohs(tcp.tcp.window)); lin++;
	mvwprintw(win, lin, x+14, "check:%x", ntohs(tcp.tcp.check)); lin++;
	mvwprintw(win, lin, x+14, "urg_ptr:%u", ntohs(tcp.tcp.urg_ptr)); lin++;
}
int rdKey(int fd, int key) {//faster not to open file every time
//	const char *dev = "/dev/input/by-path/platform-i8042-serio-0-event-kbd";
//	int keyboard = open(dev, O_RDONLY);
	char arr[(KEY_MAX+7)/8];
	memset(arr, 0, sizeof(arr));
	ioctl(fd, EVIOCGKEY(sizeof(arr)), arr);
	return !!(arr[key/8] & (1<<(key % 8)));
}
void *readpack(void *argp) {
	targs *args = argp;
	int mysoc = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL)), next, psiz;
	unsigned char buf[ETH_FRAME_LEN];
	while(1) {
		memset(&buf, 0, sizeof(buf));
		psiz = read(mysoc, buf, ETH_FRAME_LEN);
		pthread_mutex_lock(&args->datam);
		next = (args->currt + 1) % MAXPACBUF;
		memcpy(&args->llist[next].buf, buf, sizeof(args->llist[next].buf));
		args->llist[next].psiz = psiz;
		args->currt = next;
		pthread_mutex_unlock(&args->datam);
	}
}













