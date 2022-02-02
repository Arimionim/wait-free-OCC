#include <iostream>
#include "server.h"
#include "wf_scheduler.h"
#include "client.h"

int main() {
    server server;
    wf_scheduler scheduler(2, &server);
    client client(&scheduler);
    client.sendRandomTransactions(3000);
    return 0;
}