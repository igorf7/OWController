#ifndef USERTYPES_H
#define USERTYPES_H

#include "qglobal.h"

/* Report ID */
typedef enum
{
    eRepId_0 = (quint8)0,
    eRepId_1,
    eRepId_2,
    eRepId_3,
    eRepId_4
} TRepId;

/* Opcodes */
typedef enum
{
    eOwSearch = (quint8)1,
    eOwEnumerate,
    eOwEnumerateDone,
    eOwReadData,
    eOwWriteData,
    eGetRtcData,
    eSyncRtc
} TOpcode;

/* Application Layer Packet Structure */
typedef struct
{
    quint8 rep_id;
    quint8 opcode;
    quint8 size;
    quint8 data[60];
} TAppLayerPacket;

typedef QMap<QString, quint8> TDeviceMap;

#endif // USERTYPES_H
