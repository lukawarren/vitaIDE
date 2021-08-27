#include <psp2/kernel/processmgr.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#include "debugScreen.h"
#pragma GCC diagnostic pop

#define printf psvDebugScreenPrintf

int main()
{
    psvDebugScreenInit();

    printf("Hello world!\n");

    sceKernelDelayThread(3*1000000);
    sceKernelExitProcess(0);
    return 0;
}