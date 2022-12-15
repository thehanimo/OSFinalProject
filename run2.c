#include <stdio.h>
#include <stdlib.h>
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

void read_file(char *filename, unsigned int block_size, unsigned int *out_xor, unsigned int *out_block_count, double *read_time){
	double start = now();
	unsigned int buf_size = block_size / 4; // 4 because int
	unsigned int buf[buf_size];
	unsigned int xor = 0;
	int fd  = open(filename,O_RDONLY);
	if (fd == -1) {
		printf("Cannot open %s\n", filename);
		return;
	}
	unsigned int block_count = 0;
	unsigned int bytes_read;
	while ((bytes_read=read(fd,buf,block_size)) > 0 && now() - start < 15){
		block_count += 1;
		xor ^= xorbuf(buf, bytes_read / 4);
	}
	*read_time = now() - start;
	*out_block_count = block_count;
	*out_xor = xor;
	close(fd);
}

void write_file(char *filename, unsigned int block_size, unsigned int block_count){
	char buf[block_size];
	int fd  = open(filename,O_WRONLY | O_CREAT, 0644);
	for (int i =0; i < block_count; i++){
		write(fd,buf,block_size);
	}
	close(fd);
}

int main (int argc,char *argv[]) {
   if (argc != 3){
   	printf("incorrect number of inputs please format in ./run2 <filename> <block_size>");
   	return 0;
   }
   char* filename = argv[1];
   unsigned int block_size = atoi(argv[2]);
   unsigned int out_xor;
   unsigned int out_block_count;
   unsigned int out_file_size;
   double read_time = 0;
   read_file(filename,block_size,&out_xor,&out_block_count,&read_time);
   while (read_time < 5){
	printf("Previous Read time: %f. Doubling file size and trying again\n", read_time);
	if (out_block_count == 0) out_block_count = 1;
	write_file(filename,block_size,out_block_count*2);
	read_file(filename,block_size,&out_xor,&out_block_count,&read_time);
   }
   printf("XOR: %08x Block Count: %u File size: %u Read speed: %f MiB/s\n", out_xor, out_block_count, out_block_count * block_size, out_block_count / 1024.0 / 1024.0 * block_size / read_time);
  
   return 0;
}
