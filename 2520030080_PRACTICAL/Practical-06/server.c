#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

int main()
{
    char msg[100], reply[120];
    mkfifo("reqfifo", 0666);
    mkfifo("respfifo", 0666);

    printf("Server waiting...\n");

    while (1)
    {
        int in = open("reqfifo", O_RDONLY);
        read(in, msg, sizeof(msg));
        close(in);

        printf("Received: %s", msg);

        sprintf(reply, "Processed: %s", msg);

        int out = open("respfifo", O_WRONLY);
        write(out, reply, strlen(reply) + 1);
        close(out);
    }

    return 0;
}
