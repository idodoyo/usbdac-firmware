#include "usb_audio_class.h"

#include <stdint.h>
#include <usbd_core.h>

#define CONF_DESC_LENGTH 100

static const uint8_t configuration_descriptor[CONF_DESC_LENGTH] = {
  // Configuration descriptor
  0x09,                        // bLength
  USB_DESC_TYPE_CONFIGURATION, // bDescriptorType
  LOBYTE(CONF_DESC_LENGTH),    // wTotalLength
  HIBYTE(CONF_DESC_LENGTH),
  0x02,                        // bNumInterfaces
  0x01,                        // bConfigurationValue
  0x00,                        // iConfiguration
  0x80,                        // bmAttributes (Bus Powered)
  0xFA,                        // bMaxPower (500mA)

  // Standard AC Interface Descriptor
  0x09,                        // bLength
  USB_DESC_TYPE_INTERFACE,     // bDescriptorType
  0x00,                        // bInterfaceNumber
  0x00,                        // bAlternateSetting
  0x00,                        // bNumEndpoints
  0x01,                        // bInterfaceClass (Audio)
  0x01,                        // bInterfaceSubClass (Audio Control)
  0x00,                        // bInterfaceProtocol (Undefined)
  0x00,                        // iInterface

  // Class-specific Interface Descriptor
  0x09,                        // bLength
  0x24,                        // bDescriptorType (CS Interface)
  0x01,                        // bDescriptorSubtype (Header)
  0x00, 0x01,                  // bcdADC
  0x1E, 0x00,                  // wTotalLength
  0x01,                        // bInCollection
  0x01,                        // baInterfaceNr

  // Input Terminal Descriptor
  0x0C,                        // bLength
  0x24,                        // bDescriptorType (CS Interface)
  0x02,                        // bDescriptorSubtype (Input Terminal)
  0x01,                        // bTerminalID
  0x01, 0x01,                  // wTerminalType (USB Streaming)
  0x00,                        // bAssocTerminal
  0x02,                        // bNrChannels
  0x03,                        // wChannelConfig (Stereo)
  0x00,                        // iChannelNames
  0x00,                        // iTerminal

  // Output Terminal Descriptor
  0x09,                        // bLength
  0x24,                        // bDescriptorType (CS Interface)
  0x03,                        // bDescriptorSubtype (Output Terminal)
  0x02,                        // bTerminalID
  0x02, 0x06,                  // wTerminalType (Digital Audio Interface)
  0x00,                        // bAssocTerminal
  0x01,                        // bSourceID
  0x00,                        // iTerminal

  // Standard AS Interface Descriptor Alternative 0
  0x09,                        // bLength
  USB_DESC_TYPE_INTERFACE,     // bDescriptorType
  0x01,                        // bInterfaceNumber
  0x00,                        // bAlternateSetting
  0x00,                        // bNumEndpoints (Zero Bandwidth)
  0x01,                        // bInterfaceClass (Audio)
  0x01,                        // bInterfaceSubClass (Audio Streaming)
  0x00,                        // bInterfaceProtocol
  0x00,                        // iInterface

  // Standard AS Interface Descriptor Alternative 1
  0x09,                        // bLength
  USB_DESC_TYPE_INTERFACE,     // bDescriptorType
  0x01,                        // bInterfaceNumber
  0x01,                        // bAlternateSetting
  0x01,                        // bNumEndpoints
  0x01,                        // bInterfaceClass (Audio)
  0x01,                        // bInterfaceSubClass (Audio Streaming)
  0x00,                        // bInterfaceProtocol
  0x00,                        // iInterface

  // Class-Specific AS Interface Descriptor
  0x07,                        // bLength
  0x24,                        // bDescriptorType (CS Interface)
  0x01,                        // bDescriptorSubtype (Header)
  0x01,                        // bTerminalLink
  0x01,                        // bDelay
  0x01, 0x00,                  // wFormatTag (PCM)

  // Class-Specific AS Format Type Descriptor
  0x0B,                        // bLength
  0x24,                        // bDescriptorType (CS Interface)
  0x02,                        // bDescriptorSubtype (Format)
  0x01,                        // bFormatType
  0x02,                        // bNrChannels
  0x04,                        // bSubFrameSize
  0x18,                        // bBitResolution
  0x01,                        // bSamFreqType
  0x80, 0xBB, 0x00,            // 48000 Hz

  // Standard AS Isochronous Audio Data Endpoint Descriptor
  0x09,                        // bLength
  USB_DESC_TYPE_ENDPOINT,      // bDescriptorType
  USB_AS_OUT_EP,               // bEndpointAddress
  0x0B,                        // bmAttributes (Isochronous, Syncronous)
  LOBYTE(USB_AS_OUT_EP_SIZE),  // wMaxPacketSize
  HIBYTE(USB_AS_OUT_EP_SIZE),
  0x01,                        // bInterval (One packet per Frame)
  0x00,                        // bRefresh
  0x00,                        // bSynchAddress

  // Class-Specific AS Isochronous Audio Data Endpoint Descriptor
  0x07,                        // bLength
  0x25,                        // bDescriptorType (CS Endpoint)
  0x01,                        // bDescriptor (General)
  0x00,                        // bmAttributes
  0x00,                        // bLockDelayUnits
  0x00,                        // wLockDelay
};

static uint8_t buffer[USB_AS_OUT_EP_SIZE];

static uint8_t current_alternate_setting = 0;

static uint8_t init(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  USBD_LL_OpenEP(pdev, USB_AS_OUT_EP, USBD_EP_TYPE_ISOC, USB_AS_OUT_EP_SIZE);
  USBD_LL_PrepareReceive(pdev, USB_AS_OUT_EP, buffer, USB_AS_OUT_EP_SIZE);
  return USBD_OK;
}

static uint8_t deinit(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  USBD_LL_CloseEP(pdev, USB_AS_OUT_EP);
  return USBD_OK;
}

static uint8_t setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
  switch (req->bmRequest & USB_REQ_TYPE_MASK) {
    case USB_REQ_TYPE_STANDARD:
      switch (req->bRequest) {
        case USB_REQ_GET_INTERFACE:
          if (req->wIndex == 1) {
            USBD_CtlSendData(pdev, &current_alternate_setting, 1);
          } else {
            USBD_CtlError(pdev, req);
          }
          break;

        case USB_REQ_SET_INTERFACE:
          if (req->wIndex == 1 && (uint8_t)(req->wValue) < 2) {
            current_alternate_setting = (uint8_t)(req->wValue);
          } else {
            USBD_CtlError(pdev, req);
          }
          break;

        default:
          USBD_CtlError(pdev, req);
          break;
      }
      break;

    default:
      USBD_CtlError(pdev, req);
      break;
  }

  return USBD_OK;
}

static uint8_t ep0_tx_ready(USBD_HandleTypeDef *pdev) { return USBD_OK; }

static uint8_t ep0_rx_ready(USBD_HandleTypeDef *pdev) { return USBD_OK; }

static uint8_t data_in(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  return USBD_OK;
}

static uint8_t data_out(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  if (epnum == USB_AS_OUT_EP) {
    USBD_LL_PrepareReceive(pdev, USB_AS_OUT_EP, buffer, USB_AS_OUT_EP_SIZE);
  }

  return USBD_OK;
}

static uint8_t sof(USBD_HandleTypeDef *pdev) { return USBD_OK; }

static uint8_t iso_in_complete(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  return USBD_OK;
}

static uint8_t iso_out_complete(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  // TODO: Start DMA.
  return USBD_OK;
}

static uint8_t *get_configuration_descriptor(uint16_t *length)
{
  *length = sizeof(configuration_descriptor);
  return (uint8_t *)configuration_descriptor;
}

static uint8_t *error(uint16_t *length)
{
  // TODO: assert
  *length = 0;
  return NULL;
}

USBD_ClassTypeDef usb_audio_class_callbacks = {
  init,
  deinit,
  setup,
  ep0_tx_ready,
  ep0_rx_ready,
  data_in,
  data_out,
  sof,
  iso_in_complete,
  iso_out_complete,
  error,
  get_configuration_descriptor,
  error,
  error,
};
