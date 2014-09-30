#include "usb_descriptors.h"

#include <stdint.h>
#include <usbd_def.h>

static const uint8_t device_descriptor[18] = {
  0x12,                             // bLength
  USB_DESC_TYPE_DEVICE,             // bDescriptorType
  0x00, 0x02,                       // bcdUSB
  0x00,                             // bDeviceClass
  0x00,                             // bDeviceSubClass
  0x00,                             // bDeviceProtocol
  USB_MAX_EP0_SIZE,                 // bMaxPacketSize
  LOBYTE(USB_VID), HIBYTE(USB_VID), // idVendor
  LOBYTE(USB_PID), HIBYTE(USB_PID), // idProduct
  0x00, 0x00,                       // bcdDevice
  USBD_IDX_MFC_STR,                 // iManufacturer
  USBD_IDX_PRODUCT_STR,             // iProduct
  USBD_IDX_SERIAL_STR,              // iSerialNumber
  1                                 // bNumConfigurations
};

typedef struct string_descriptor {
  uint8_t size;
  uint8_t type;
  wchar_t unicode_string[];
} StringDescriptor;

static const StringDescriptor language_string = {
  2, USB_DESC_TYPE_STRING, { 0x0409 }
};

static const StringDescriptor manufacturer_string = {
  8, USB_DESC_TYPE_STRING,
  L"nano"
};

static const StringDescriptor product_string = {
  12, USB_DESC_TYPE_STRING,
  L"usbdac"
};

uint8_t *device_descriptor_callback(USBD_SpeedTypeDef speed, uint16_t *length)
{
  *length = sizeof(device_descriptor);
  return (uint8_t *)device_descriptor;
}

uint8_t *language_string_callback(USBD_SpeedTypeDef speed, uint16_t *length)
{
  *length = sizeof(language_string);
  return (uint8_t *)&language_string;
}

uint8_t *manufacturer_string_callback(USBD_SpeedTypeDef speed, uint16_t *length)
{
  *length = sizeof(manufacturer_string);
  return (uint8_t *)&manufacturer_string;
}

uint8_t *product_string_callback(USBD_SpeedTypeDef speed, uint16_t *length)
{
  *length = sizeof(product_string);
  return (uint8_t *)&product_string;
}

USBD_DescriptorsTypeDef usb_descriptor_callbacks = {
  &device_descriptor_callback,
  &language_string_callback,
  &manufacturer_string_callback,
  &product_string_callback,
};
