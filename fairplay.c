#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include "crypto/crypto.h"
#include "fairplay.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <CoreFoundation/CoreFoundation.h>

static void print_buf(unsigned char *data, int len)
{
	int i;
	for (i=0; i<len; i++)
		fprintf(stderr, "%02x ",data[i]);
	fprintf(stderr, "\n");
}

#define SERVER_PORT 19999

static int fairplay_sock_fd = 0;
static int get_fairplay_socket(const char *ip)
{

	struct sockaddr_in ser_addr;

	if (fairplay_sock_fd > 0) return fairplay_sock_fd;

	memset(&ser_addr, 0, sizeof(ser_addr));
	ser_addr.sin_family = AF_INET;
    
//    inet_aton(ip, (struct in_addr *)&ser_addr.sin_addr);
//	inet_aton("106.186.117.173", (struct in_addr *)&ser_addr.sin_addr);
	//inet_aton("127.0.0.1", (struct in_addr *)&ser_addr.sin_addr);
	ser_addr.sin_port = htons(SERVER_PORT);
    ser_addr.sin_addr.s_addr = inet_addr(ip);
    
    CFSocketRef socket_ref = CFSocketCreate(NULL, PF_INET, 0, 0, kCFSocketReadCallBack, NULL, NULL);
    CFDataRef address_data = CFDataCreate(NULL, (UInt8 *)&ser_addr, sizeof(struct sockaddr_in));
    CFSocketError error = CFSocketConnectToAddress(socket_ref, address_data, 2000);
    
	fairplay_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(fairplay_sock_fd <= 0)
	{
		fprintf(stderr, "%s:%d, create socket failed", __FILE__, __LINE__);
		return 0;
	}

    int connection_status = connect(fairplay_sock_fd, (struct sockaddr *)&ser_addr, sizeof(ser_addr));
	if(connection_status < 0)
	{
		fprintf(stderr, "%s:%d, create socket failed", __FILE__, __LINE__);
		fairplay_sock_fd = 0;
		return 0;
	}

	return fairplay_sock_fd;
}

static void close_fairplay_socket()
{
	if (fairplay_sock_fd > 0) close(fairplay_sock_fd);
	fairplay_sock_fd = 0;
}

unsigned char * fairplay_query(int cmd, const unsigned char *data, int len, int *size_p, const char *ip)
{
	int sock_fd;
	unsigned char recvbuf[1024] = {0};
	unsigned char sendbuf[1024] = {0};
	int sendlen = 0;
	int retlen;
	unsigned char *buf;

	if (cmd < 1 || cmd > 3) return NULL;

	sock_fd = get_fairplay_socket(ip);
    
//	sendlen = len + 2;
//	sendbuf[0] = cmd & 0xff;
//	sendbuf[1] = sendlen & 0xff;
//	memcpy(sendbuf+2, data, len);
//    sendbuf[8] = 0x02;
    
    sendlen = len;
//    sendbuf[0] = cmd & 0xff;
//    sendbuf[1] = sendlen & 0xff;
    memcpy(sendbuf, data, len);
//    sendbuf[6] = 0x02;

	retlen = send(sock_fd, sendbuf, sendlen, 0) ;
	if (retlen < 0) {
		close_fairplay_socket();
		return NULL;
	}

	retlen = recv(sock_fd, recvbuf, 1024, 0) ;

	if (retlen <= 0) {
		close_fairplay_socket();
		return NULL;
	}

	*size_p = retlen;
	buf = (unsigned char*)malloc(retlen); 
	memcpy(buf, recvbuf, retlen);

	if (cmd == 3) 
		close_fairplay_socket();

	return buf;
}

//int airplay_decrypt(AES_KEY *ctx, unsigned char *in, unsigned int len, unsigned char *out)
//{
//	unsigned char *pin,*pout;
//	unsigned int n;
//	unsigned char k;
//	int i,remain = 0;
//	int l = len, len1 = 0;
//
//	if (l == 0) return 0;
//
//	pin = in; pout = out;
//
//	fprintf(stderr, "remain=%d\n", ctx->remain_bytes);
//
//	if (ctx->remain_bytes) {
//		n = ctx->remain_bytes;
//		do {
//			*pout = *pin ^ ctx->out[n];
//			n = (n + 1) & 0xf;
//			ctx->remain_bytes = n;
//			l--;
//			pout++;
//			pin++;
//			if (l == 0) return 0;
//		} while (n != 0);
//	}
//
//	if (l <= 15) {
//		remain = l;
//		AES_ecb_encrypt(&ctx->in, &ctx->out, ctx, AES_ENCRYPT);
//	} else {
//		len1 = l;
//		do {
//			AES_ecb_encrypt(&ctx->in, &ctx->out, ctx, AES_ENCRYPT);
//			i = 15;
//			do {
//				k = ctx->in[i] + 1;
//				ctx->in[i] = k;
//				if (k) break;
//				-- i;
//			} while (i != -1);
//			for (i=0; i<16; i++)
//			{
//				pout[i] = pin[i] ^ ctx->out[i];
//			}
//			pout += 16;
//			pin += 16;
//			l -= 16;
//		} while (l > 15);
//		if (l == 0) return 0;
//
///*
//		i = (len1 - 16) & 0xfffffff0 + 16;
//
//		pin = in + i;
//		pout = out + i;
//*/
//		AES_ecb_encrypt(&ctx->in, &ctx->out, ctx, 1);
//		remain = l;
//	}
//
//	i = 15;
//	do {
//		k = ctx->in[i] + 1;
//		ctx->in[i] = k;
//		if (k) break;
//		-- i;
//	} while (i != -1);
//
//	for (i=0; i<remain; i++)
//	{
//		pout[i] = pin[i] ^ ctx->out[i];
//	}
//	if (ctx->remain_flags == 0) 
//		ctx->remain_bytes += remain;
//
//	return 0;
//}

