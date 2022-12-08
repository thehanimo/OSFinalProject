#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

unsigned int xorbuf(unsigned int *buffer, int size) {
    unsigned int result = 0;
    for (int i = 0; i < size; i++) {
        result ^= buffer[i];
    }
    return result;
}
unsigned int read_file(char *filename, int block_size){
	int buf_size = block_size / 4; // 4 because int
	unsigned int buf[buf_size];
	unsigned int xor = 0;
	int fd  = open(filename,O_RDONLY);
	if (fd == -1) {
		printf("Cannot open %s\n", filename);
		return 0;
	}
	int block_count = 0;
	int r;
	while ((r=read(fd,buf,block_size)) > 0){
		block_count += 1;
		xor ^= xorbuf(buf, buf_size);
	}
	printf("%08x\n", xor);
	close(fd);
	return block_size * block_count;
}

void write_file(char *filename, int block_size, int block_count){
	char buf[block_size];
	int fd  = open(filename,O_WRONLY | O_CREAT);
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
   int block_size = atoi(argv[2]);
   unsigned int file_size = read_file(filename,block_size);
   printf("File size: %u\n", file_size);
  
   return 0;
}
