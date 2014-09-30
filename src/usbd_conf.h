#ifndef USBD_CONF_H
#define USBD_CONF_H

#include <stm32f3xx.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define USBD_MAX_NUM_INTERFACES    1
#define USBD_MAX_NUM_CONFIGURATION 1
#define USBD_SUPPORT_USER_STRING   0
#define USBD_SELF_POWERED          1
#define USBD_DEBUG_LEVEL           2

static inline void *USBD_malloc(size_t size) { return malloc(size); }

static inline void USBD_free(void *ptr) { free(ptr); }

static inline void *USBD_memset(void *ptr, int value, size_t num)
{
  return memset(ptr, value, num);
}

static inline void *USBD_memcpy(void *destination, const void *source,
                                size_t num)
{
  return memcpy(destination, source, num);
}

#if (USBD_DEBUG_LEVEL > 0)
#define  USBD_UsrLog(...) printf(__VA_ARGS__);\
                          printf("\n");
#else
#define USBD_UsrLog(...)
#endif


#if (USBD_DEBUG_LEVEL > 1)

#define  USBD_ErrLog(...) printf("ERROR: ") ;\
                          printf(__VA_ARGS__);\
                          printf("\n");
#else
#define USBD_ErrLog(...)
#endif


#if (USBD_DEBUG_LEVEL > 2)
#define  USBD_DbgLog(...) printf("DEBUG : ") ;\
                          printf(__VA_ARGS__);\
                          printf("\n");
#else
#define USBD_DbgLog(...)
#endif

#endif  // USBD_CONF_H
