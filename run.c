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
unsigned int xorbuf(unsigned int *buffer, int size) {
    unsigned int result = 0;
    for (int i = 0; i < size; i++) {
        result ^= buffer[i];
    }
    return result;
}
void read_file(char *filename, int block_size, int block_count){
	double start = now();
	int buf_size = block_size / 4; // 4 because int
	unsigned int buf[buf_size];
	unsigned int xor;
	int fd  = open(filename,O_RDONLY);
	if (fd == -1) {
		printf("Cannot open %s\n", filename);
		return;
	}
	for (int i = 0; i < block_count; i++){
		int r = read(fd,buf,block_size);
		if (r < 0){
			break;
		}
		if (i == 0){
			xor = xorbuf(buf, buf_size);
		} else{
			xor ^= xorbuf(buf, buf_size);
		}
	}
	printf("%08x\n", xor);
	double end = now();
	printf("Read speed: %f MiB/s\n", block_size / 1000000.0 * block_count / (end - start));
	close(fd);
}

void write_file(char *filename, int block_size, int block_count){
	double start = now();
	char buf[block_size];
	int fd  = open(filename,O_WRONLY | O_CREAT, 0644);
	for (int i =0; i < block_count; i++){
		write(fd,buf,block_size);
	}
	close(fd);
	double end = now();
	printf("Write speed: %f MiB/s\n", block_size / 1000000.0 * block_count / (end - start));
}

int main (int argc,char *argv[]) {
   if (argc != 5){
   	printf("incorrect number of inputs please format in ./run <filename> [-r|-w] <block_size> <block_count>");
   	return 0;
   }
   char* filename = argv[1];
   char flag = argv[2][1];
   int block_size = atoi(argv[3]);
   int block_count = atoi(argv[4]);
   if (flag == 'r'){
   	read_file(filename,block_size,block_count);
   }
   else if (flag == 'w'){
   	write_file(filename,block_size,block_count);
   }
   else{
   	printf("Invalid flag; please use -w for writing, or -w for writing.");
   }
  
   return 0;
}
