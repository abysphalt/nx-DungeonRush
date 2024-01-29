#ifndef NXLINK_H
#define NXLINK_H

#include <switch.h>
#include <stdio.h>

// nxlink
#ifndef DEBUG
#define TRACE(fmt,...) ((void)0)
#else
#include <unistd.h>
#define TRACE(fmt,...) printf("%s: " fmt "\n", __PRETTY_FUNCTION__, ## __VA_ARGS__)

void initNxLink();
void deinitNxLink();

#ifdef __cplusplus
extern "C" {
#endif

void userAppInit();
void userAppExit();

#ifdef __cplusplus
}
#endif


#endif // ENABLE_NXLINK

#endif // NXLINK_H
