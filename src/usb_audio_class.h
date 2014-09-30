#ifndef USB_AUDIO_H
#define USB_AUDIO_H

#include <usbd_def.h>

#define USB_AS_OUT_EP 1
#define USB_AS_OUT_EP_SIZE 384

extern USBD_ClassTypeDef usb_audio_class_callbacks;

#endif  // USB_AUDIO_B
