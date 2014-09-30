#include <stm32f3xx_hal.h>
#include <usbd_core.h>

#include "usb_audio_class.h"
#include "usb_descriptors.h"

void setup()
{
  static USBD_HandleTypeDef usb_handle;
  USBD_Init(&usb_handle, &usb_descriptor_callbacks, 0);
  USBD_RegisterClass(&usb_handle, &usb_audio_class_callbacks);
  USBD_Start(&usb_handle);
}

void loop()
{

}
