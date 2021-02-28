#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "usage: %s hexstring file [file2 ...]\n", argv[0]);
        exit(1);
    }
    if (strlen(argv[1]) % 2) {
        fprintf(stderr, "invalid hex string (odd length)\n");
        exit(1);
    }
    size_t blen = strlen(argv[1]) / 2;
    uint8_t *bytes = malloc(blen);
    if (!bytes) {
        perror("malloc");
        fprintf(stderr, "Here's a nickel kid, go buy yourself enough RAM to hold %zu bytes\n", blen);
        exit(1);
    }
    char *pos = argv[1];
    for (size_t count = 0; count < blen; count++) {
        if (1 != sscanf(pos, "%2hhx", &bytes[count])) {
            fprintf(stderr, "invalid hex string: %s\n", argv[1]);
            exit(1);
        }
        pos += 2;
    }

    for (int i = 2; i < argc; i++) {
        struct stat st;
        int fd = open(argv[i], O_RDONLY);
        if (-1 == fd) {
            perror("open");
            continue;
        }
        if (-1 == fstat(fd, &st)) {
            perror("fstat");
            goto end2;
        }
        void *haystack = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE|MAP_POPULATE, fd, 0);
        if (!haystack) {
            perror("mmap");
            goto end1;
        }
        void *hayp = haystack;
        void *found;
        while (found = memmem(hayp, haystack+st.st_size-hayp, bytes, blen)) {
            printf("%s: %#tx\n", argv[i], found - haystack);
            hayp = found+1;
        }
end1:
        munmap(haystack, st.st_size);
end2:
        close(fd);
    }
    return 0;
}
