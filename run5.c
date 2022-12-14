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

void read_file(char *filename, unsigned int block_size, unsigned int block_count){
	double start = now();
	unsigned int buf_size = block_size / 4; // 4 because int
	unsigned int buf[buf_size];
	int fd  = open(filename,O_RDONLY);
	if (fd == -1) {
		printf("Cannot open %s\n", filename);
		return;
	}
	for (unsigned int i = 0; i < block_count; i++){
		unsigned int r = read(fd,buf,block_size);
		if (r < 0){
			break;
		}
	}
	double end = now();
	printf("Read speed: %f MiB/s\n", (float) (block_count / 1024.0 / 1024.0 / (end - start));
	printf("Time Taken: %f seconds\n", (float) ((end - start)));
	printf("Total Calls: %f \n", (float) (block_count));
	close(fd);
}

void lseek_file(char *filename, unsigned int block_size, unsigned int block_count){
	double start = now();
	unsigned int buf_size = block_size / 4; // 4 because int
	unsigned int buf[buf_size];
	int fd  = open(filename,O_RDONLY);
	if (fd == -1) {
		printf("Cannot open %s\n", filename);
		return;
	}
	for (unsigned int i = 0; i < block_count; i++){
		unsigned int r = lseek(fd,0,SEEK_SET);
		if (r < 0){
			break;
		}
	}
	close(fd);
	double end = now();
	printf("Read speed: %f MiB/s\n", (float) (block_count / 1024.0 / 1024.0 / (end - start)));
	printf("Time Taken: %f seconds\n", (float) ((end - start)));
	printf("Total Calls: %f\n", (float) (block_count));
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
   else if (flag == 'l'){
   	lseek_file(filename,block_size,block_count);
   }
   else{
   	printf("Invalid flag; please use -w for writing, or -w for writing.");
   }
  
   return 0;
}
