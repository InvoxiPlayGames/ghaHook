#ifndef GHA_JAMMA_H
#define GHA_JAMMA_H

typedef struct _jammamem_struct
{
    // TODO
} jammamem_struct;


typedef enum _jammaOp
{
    kOpReadDipSwitches = 0x1,
    kOpReadData = 0x2,
    kOpGetBoardRevString = 0x9,
    kOpInitialize = 0x1c,
    kOpGetBoardRevInt1 = 0x27,
    kOpGetBoardRevInt2 = 0x28
} jammaOp;

/*
    The structure returned by JammaOp(2, &str).
    Contains which buttons were pressed on the keypad.
*/
typedef struct _jammaop2_struct
{
    char unk[0x20];
    int startP1;
    int startP2;
    int coin1;
    int coin2;
    int coin3;
    int coin4;
    int test;
    int volume1;
    int volume2;
} jammaop2_struct;

#endif // GHA_JAMMA_H
