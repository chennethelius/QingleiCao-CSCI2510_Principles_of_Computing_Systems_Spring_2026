//This program brute-forces a given password hash by trying all possible
//passwords of a given length.
//
//Usage:
//crack <threads> <keysize> <target>
//
//Where <threads> is the number of threads to use, <keysize> is the maximum
//password length to search, and <target> is the target password hash.
//
//For example:
//
//./crack 1 5 na3C5487Wz4zw
//
//Should return the password 'apple'

#define _XOPEN_SOURCE
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <crypt.h>
#include <pthread.h>

typedef struct {
	long start;
	long end;
	int length;
	char salt[3];
	const char* target;
} thread_args_t;

static void index_to_password( long idx, int length, char* buf ){
	for( int i = length - 1; i >= 0; i-- ){
		buf[i] = 'a' + (idx % 26);
		idx /= 26;
	}
	buf[length] = '\0';
}

static void* worker( void* arg ){
	thread_args_t* args = (thread_args_t*)arg;
	struct crypt_data data;
	data.initialized = 0;
	char password[9];

	for( long i = args->start; i < args->end; i++ ){
		index_to_password(i, args->length, password);
		char* result = crypt_r(password, args->salt, &data);
		if( result != NULL && strcmp(result, args->target) == 0 ){
			printf("%s\n", password);
			fflush(stdout);
			exit(0);
		}
	}
	return NULL;
}

int main( int argc, char* argv[] ){

	if( argc != 4 ){
		fprintf(stderr, "Usage: %s <threads> <keysize> <target>\n", argv[0]);
		return 1;
	}

	int num_threads = atoi(argv[1]);
	int keysize = atoi(argv[2]);
	const char* target = argv[3];

	if( num_threads < 1 || keysize < 1 || keysize > 8 || strlen(target) < 2 ){
		fprintf(stderr, "Invalid arguments\n");
		return 1;
	}

	char salt[3];
	salt[0] = target[0];
	salt[1] = target[1];
	salt[2] = '\0';

	for( int length = 1; length <= keysize; length++ ){
		long total = 1;
		for( int i = 0; i < length; i++ ) total *= 26;

		pthread_t* threads = malloc(num_threads * sizeof(pthread_t));
		thread_args_t* args = malloc(num_threads * sizeof(thread_args_t));

		long chunk = total / num_threads;
		long rem = total % num_threads;
		long current = 0;

		for( int t = 0; t < num_threads; t++ ){
			args[t].start = current;
			args[t].end = current + chunk + (t < rem ? 1 : 0);
			current = args[t].end;
			args[t].length = length;
			strcpy(args[t].salt, salt);
			args[t].target = target;
			pthread_create(&threads[t], NULL, worker, &args[t]);
		}

		for( int t = 0; t < num_threads; t++ ){
			pthread_join(threads[t], NULL);
		}

		free(threads);
		free(args);
	}

	return 0;
}
