#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>

double now() {
  struct timeval tv;
  gettimeofday(&tv, 0);
  return tv.tv_sec + tv.tv_usec / 1000000.0;
}

unsigned int xorbuf(unsigned int *buffer, unsigned int size) {
    unsigned int result = 0;
    for (unsigned int i = 0; i < size; i++) {
        result ^= buffer[i];
    }
    return result;
}

void read_file(char *filename, unsigned int block_size, unsigned int block_count){
	double start = now();
	unsigned int buf_size = block_size / 4; // 4 because int
	unsigned int buf[buf_size];
	unsigned int xor = 0;
	int fd  = open(filename,O_RDONLY);
	if (fd == -1) {
		printf("Cannot open %s\n", filename);
		return;
	}
	unsigned int blocks_read = 0;
	unsigned int bytes_read;
	while ((bytes_read=read(fd,buf,block_size)) > 0 && blocks_read < block_count){
		xor ^= xorbuf(buf, bytes_read / 4);
		blocks_read += 1;
	}
	close(fd);
	double end = now();
	printf("%08x\n", xor);
	printf("Read speed: %f MiB/s\n", block_size / 1024.0 / 1024.0 * blocks_read / (end - start));
}

void write_file(char *filename, int block_size, int block_count){
	double start = now();
	char buf[block_size];
	int fd  = open(filename,O_WRONLY | O_CREAT, 0644);
	for (unsigned int i =0; i < block_count; i++){
		write(fd,buf,block_size);
	}
	close(fd);
	double end = now();
	printf("Write speed: %f MiB/s\n", block_size / 1024.0 / 1024.0 * block_count / (end - start));
}

int main (int argc,char *argv[]) {
   if (argc != 5){
   	printf("incorrect number of inputs please format in ./run <filename> [-r|-w] <block_size> <block_count>");
   	return 0;
   }
   char* filename = argv[1];
   char flag = argv[2][1];
   unsigned int block_size = atoi(argv[3]);
   unsigned int block_count = atoi(argv[4]);
   if (flag == 'r'){
   	read_file(filename,block_size,block_count);
   }
   else if (flag == 'w'){
   	write_file(filename,block_size,block_count);
   }
   else{
   	printf("Invalid flag; please use -w for writing, or -r for reading.");
   }
  
   return 0;
}
