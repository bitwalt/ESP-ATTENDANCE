#include "systhread.h"

sysThread::sysThread() {
     stopped = false;
}

void sysThread::stop() {
       stopped = true;
}
