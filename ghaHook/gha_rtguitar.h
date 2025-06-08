#ifndef GHA_RTGUITAR_H
#define GHA_RTGUITAR_H

typedef struct _rtGuitar
{
    void *usbDevice;
    int location;
    int deviceNum;
    char interfaceNumber;
    char configurationValue;
    char endpointAddress;
    int productId;
    int vendorId;
    int unk_0x18;
} rtGuitar;

#endif // GHA_RTGUITAR_H
