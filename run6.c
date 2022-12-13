#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h> 
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>

double now() {
  struct timeval tv;
  gettimeofday(&tv, 0);
  return tv.tv_sec + tv.tv_usec / 1000000.0;
}

typedef struct {
	int fd;
	unsigned int xor;
	unsigned int block_size;
	unsigned int block_offset;
	unsigned int bytes_read;
	unsigned int* buf;
} thread;

unsigned int xorbuf(unsigned int *buffer, int size) {
    unsigned int result = 0;
    for (int i = 0; i < size; i++) {
        result ^= buffer[i];
    }
    return result;
}

void *child_thread (void *args) {
	thread *temp = (thread *)args;
	unsigned int bytes_read;
	unsigned int xor = 0;
	unsigned int byte_offset = temp->block_offset * temp->block_size;

	while ((bytes_read=pread(temp->fd, temp->buf, temp->block_size, byte_offset)) > 0) {
		xor ^= xorbuf(temp->buf, bytes_read / 4);
		byte_offset += bytes_read;
	}

	temp->xor = xor;
	temp->bytes_read = byte_offset;
    pthread_exit(NULL);
}

void read_file_threaded(unsigned int block_size, char *filename, unsigned int thread_count) {
	double start = now();
	unsigned int buf_size = block_size / 4;
	unsigned int buf[buf_size];
	unsigned int xor = 0;
	unsigned int block_offset = 0;
	int fd = open(filename, O_RDONLY);
	if(fd == -1) {
		printf("File error: %s -- could not be opened\n", filename);
		return;
	}
	int r;
	int count = 0;
	pthread_t child[thread_count];
	thread args[thread_count];
	for (int i = 0; i < thread_count; i++){
		args[i].block_offset = block_offset;
		args[i].block_size = block_size;
		args[i].fd = fd;
		args[i].buf = (unsigned int *) malloc(block_size);
		pthread_create(&child[i], NULL, child_thread, (void *)&args[i]);
	}
	unsigned int bytes_read = 0;
	for (unsigned int i=0; i < thread_count; i++) {
		pthread_join(child[i], NULL);
		xor ^= args[i].xor;
		bytes_read += args[i].bytes_read;
	}
	close(fd);
	double end = now();
	printf("%08x\n", xor);
	printf("Read speed: %f MiB/s\n", bytes_read / 1000000.0 / (end - start));
}

int main(int argc, char *argv[]) {
	if (argc != 4){
		printf("incorrect number of inputs please format in ./run6 <filename> block_size thread_count");
		return 0;
	}
    char* filename = argv[1];
    unsigned int block_size = atoi(argv[2]);
	unsigned int thread_count = atoi(argv[3]);

	read_file_threaded(block_size, filename, thread_count);
	
	return 0;
}