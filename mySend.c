#include </home/gerecke/Desktop/cFiles/eth.f/ethlib.f/ethlib.h>

//http://man7.org/linux/man-pages/man7/packet.7.html
//http://man7.org/linux/man-pages/man7/netdevice.7.html
//http://lxr.free-electrons.com/source/include/uapi/linux/if_ether.h#L31
//https://en.wikipedia.org/wiki/EtherType
//tcpdump to observe packets(sudo tcpdump -i iface -XX)

int main(int argc, int* argv) {

	int mysoc = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));	
	Packet packSnd;//declare packet

	int i = 0;
	unsigned char buf[PACLEN-ETH_HLEN];//data

	while(1) {

		i = 0;
		while(i<(LINLEN*3)+8) {
			printf("-");
			i++;
		}
		printf("\n");

		i = 0;
		memset(&buf, 0, sizeof(buf)); 
		while(i++<sizeof(buf)) buf[i] = i % 20;

		packDes(packSnd, TRANS, RECV, htons(PROTO), buf, PACLEN);	

		if( write(mysoc, packSnd.buf, PACLEN) ) {//send packet
			printf("sent\n");
		}else {
			printf("failed to send\n");
		}


		printf("'q' to quit: ");

		if (getchar() == 'q') {
			return 0;
		}else {
			continue;
		}
	}
}
