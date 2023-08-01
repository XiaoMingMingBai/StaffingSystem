#include "include/server.h"


int main(int argc, char const *argv[])
{
    server_init(8888);
    server_run();
    return 0;
}
