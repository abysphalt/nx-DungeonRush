// NxLink stuff.
#include <switch.h>
#include <stdio.h>
#include "debug.h"
int s_nxlinkSock = -1;

void initNxLink()
{
    if (R_FAILED(socketInitializeDefault()))
        return;

    s_nxlinkSock = nxlinkStdio();
    if (s_nxlinkSock >= 0)
        TRACE("printf output now goes to nxlink server");
    else
        socketExit();
}

void deinitNxLink()
{

    if (s_nxlinkSock >= 0)
    {
        close(s_nxlinkSock);
        socketExit();
        s_nxlinkSock = -1;
    }
}

#ifdef __cplusplus
extern "C" {
#endif

void userAppInit()
{
    initNxLink();
}

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif

void userAppExit()
{
    deinitNxLink();
}

#ifdef __cplusplus
}
#endif