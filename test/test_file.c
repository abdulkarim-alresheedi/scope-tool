#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
    char buffer[64];
    int fd = open("flag.txt", O_RDONLY);   
    if (fd == -1) {
        perror("open failed");
        return 1;
    }

    int bytes = read(fd, buffer, sizeof(buffer)-1);  
    if (bytes == -1) {
        perror("read failed");
        return 1;
    }
    buffer[bytes] = '\0'; 

    write(1, buffer, bytes);   

    close(fd); 
    return 0;
}
