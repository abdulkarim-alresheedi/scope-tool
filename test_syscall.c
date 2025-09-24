#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>

int main() {
    int fd = open("demo.txt", O_CREAT | O_RDWR, 0644);
    if (fd >= 0) {
        write(fd, "Hello SCOPE!\n", 13);
        char buf[32] = {0};
        lseek(fd, 0, SEEK_SET);
        read(fd, buf, sizeof(buf)-1);
        close(fd);
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock >= 0) {
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(65000); 
        inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
        connect(sock, (struct sockaddr*)&addr, sizeof(addr));
        close(sock);
    }

    pid_t pid = fork();
    if (pid == 0) {
        execl("/bin/echo", "echo", "Running exec test", NULL);
        _exit(1);
    } else {
        waitpid(pid, NULL, 0);
    }

    void *mem = mmap(NULL, 4096, PROT_READ|PROT_WRITE,
                     MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    if (mem != MAP_FAILED) {
        strcpy((char*)mem, "Memory mapped region");
        munmap(mem, 4096);
    }

    return 0;
}
