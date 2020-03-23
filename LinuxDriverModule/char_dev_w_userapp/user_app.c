#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#define DEVICE "/dev/nkuzmic_device"

int main() {
	int i,fd;
	char ch,write_buf[100],read_buf[100];

	fd = open(DEVICE, O_RDWR); // open for reading and writing

	if(fd == -1){
		printf("File %s either does not exist or has been locked by another process!\n",DEVICE);
		exit(-1);
	}

	printf("r = read from device\nw = write to device\nenter command: ");
	scanf("%c",&ch);

	switch(ch){
		case 'w':
			printf("Enter data: ");
			scanf(" %[^\n]", write_buf);
			write(fd,write_buf,sizeof(write_buf));
			break;
		case 'r':
			read(fd,read_buf,sizeof(read_buf));
			printf("Device: %s\n",read_buf);
			break;
		default:
			printf("Command not recognized.\n");
			break;
	}

	close(fd);

	return 0;
}