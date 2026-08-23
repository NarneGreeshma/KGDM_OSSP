#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main()
{
    int source_fd, destination_fd;
    ssize_t bytes_read, bytes_written;
    char buffer[BUFFER_SIZE];

    source_fd = open("source.txt", O_RDONLY);

    if (source_fd == -1)
    {
        perror("Error opening source file");
        return 1;
    }

    destination_fd = open("destination.txt",
                          O_WRONLY | O_CREAT | O_TRUNC,
                          0644);

    if (destination_fd == -1)
    {
        perror("Error opening destination file");
        close(source_fd);
        return 1;
    }

    while ((bytes_read = read(source_fd, buffer, BUFFER_SIZE)) > 0)
    {
        bytes_written = write(destination_fd, buffer, bytes_read);

        if (bytes_written != bytes_read)
        {
            perror("Error writing to destination file");
            close(source_fd);
            close(destination_fd);
            return 1;
        }
    }

    if (bytes_read == -1)
    {
        perror("Error reading source file");
    }

    close(source_fd);
    close(destination_fd);

    printf("File copied successfully using system calls.\n");

    return 0;
}
