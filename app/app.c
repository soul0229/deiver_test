#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "T1.h"

struct st7789v_frame {
    unsigned short xsta;
    unsigned short ysta;
    unsigned short xend;
    unsigned short yend;
    unsigned short color[240*240];
};
static struct st7789v_frame s_frame;

int main(int argc,char *argv[])
{
	int fd;
    int i,j;
    s_frame.xsta = 35;
    s_frame.xend = 171;
    s_frame.ysta = 0;
    s_frame.yend = 240;

    memcpy(s_frame.color, gImage_T1, sizeof(gImage_T1));
	fd = open("/dev/st7789v",O_RDWR);
	if(fd<0)
	{
		perror("open error \n");
		return fd;
	}

	write(fd,&s_frame,sizeof(s_frame));

	close(fd);
	return 0;
}
