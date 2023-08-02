#include "include/client.h"

int main(int argc, char const *argv[])
{
    client_init("192.168.250.100", 8888);
    while (true)
    {
        client_send();
        client_recv();
    }
    return 0;
}
