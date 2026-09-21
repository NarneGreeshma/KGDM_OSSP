#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
    char msg[100], reply[100];

    printf("Enter message: ");
    fgets(msg, sizeof(msg), stdin);

    int out = open("reqfifo", O_WRONLY);
    write(out, msg, sizeof(msg));
    close(out);

    int in = open("respfifo", O_RDONLY);
    read(in, reply, sizeof(reply));
    close(in);

    printf("%s\n", reply);

    return 0;
}
