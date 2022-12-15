#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>

typedef struct {
	int fd;
	unsigned int xor;
	unsigned int block_size;
	unsigned int block_offset;
	unsigned int block_count;
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
	unsigned int blocks_read = 0;
	unsigned int xor = 0;
	unsigned int byte_offset = temp->block_offset * temp->block_size;

	while (blocks_read < temp->block_count && (bytes_read=pread(temp->fd, temp->buf, temp->block_size, byte_offset)) > 0) {
		xor ^= xorbuf(temp->buf, bytes_read / 4);
		byte_offset += bytes_read;
		blocks_read += 1;
	}

	temp->xor = xor;
    pthread_exit(NULL);
}

void read_file_threaded(unsigned int block_size, unsigned int blocks_per_thread, char *filename, unsigned int thread_count) {
	unsigned int buf_size = block_size / 4;
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
		args[i].block_count = blocks_per_thread;
		args[i].fd = fd;
		args[i].buf = (unsigned int *) malloc(block_size);
		pthread_create(&child[i], NULL, child_thread, (void *)&args[i]);
		block_offset += blocks_per_thread;
	}
	for (unsigned int i=0; i < thread_count; i++) {
		pthread_join(child[i], NULL);
		xor ^= args[i].xor;
	}
	close(fd);
	printf("%08x\n", xor);
}

int main(int argc, char *argv[]) {
	if (argc != 2){
		printf("incorrect number of inputs please format in ./fast <filename>");
		return 0;
	}
    char* filename = argv[1];
	struct stat st;
	stat(filename, &st);
    unsigned int block_size = 1256000;
	unsigned int thread_count = 6;
	unsigned int blocks_per_thread = (st.st_size / block_size + 1) / thread_count + 1;
	read_file_threaded(block_size, blocks_per_thread, filename, thread_count);
	
	return 0;
}