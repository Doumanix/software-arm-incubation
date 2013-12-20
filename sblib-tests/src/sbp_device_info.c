// automatically generated from arm-deviceinfo-20131220-2123.trx

#include "CUnit/Basic.h"

#include "sb_proto_test.h"
#include "sb_bus.h"
#include "sb_proto.h"
#include "sb_const.h"
#include "sb_eep_emu.h"

static void check_connect(void)
{
  CU_ASSERT(sbConnectedAddr == 0x0001);
}

static void check_disconnect(void)
{
  CU_ASSERT(sbConnectedAddr == 0x0000);
}

static void tc_setup(void)
{
  sbOwnPhysicalAddr = 0x117E; // set own address to 1.1.126
  eep [0x04] = 0x00;
  eep [0x05] = 0x00;
  eep [0x06] = 0x00;
  eep [0x07] = 0x00;
  eep [0x09] = 0x00;
  eep [0x0D] = 0x00;
}

Telegram device_info_t[] =
{ {TEL_RX,  7, 0, check_connect       , {0xB0, 0x00, 0x01, 0x11, 0x7E, 0x60, 0x80}} //   1
, {TEL_RX,  8, 2, NULL                , {0xB0, 0x00, 0x01, 0x11, 0x7E, 0x61, 0x43, 0x00}} //   2
, {TEL_TX,  7, 0, NULL                , {0xB0, 0x11, 0x7E, 0x00, 0x01, 0x60, 0xC2}} //   3
, {TEL_TX, 10, 0, NULL                , {0xB0, 0x11, 0x7E, 0x00, 0x01, 0x63, 0x43, 0x40, 0x00, 0x12}} //   4
, {TEL_RX,  7, 0, NULL                , {0xB0, 0x00, 0x01, 0x11, 0x7E, 0x60, 0xC2}} //   5
, {TEL_RX, 10, 2, NULL                , {0xB0, 0x00, 0x01, 0x11, 0x7E, 0x63, 0x46, 0x01, 0x01, 0x04}} //   6
, {TEL_TX,  7, 0, NULL                , {0xB0, 0x11, 0x7E, 0x00, 0x01, 0x60, 0xC6}} //   7
, {TEL_TX, 11, 0, NULL                , {0xB0, 0x11, 0x7E, 0x00, 0x01, 0x64, 0x46, 0x41, 0x01, 0x04, 0x00}} //   8
, {TEL_RX,  7, 0, NULL                , {0xB0, 0x00, 0x01, 0x11, 0x7E, 0x60, 0xC6}} //   9
, {TEL_RX,  9, 2, NULL                , {0xB0, 0x00, 0x01, 0x11, 0x7E, 0x62, 0x49, 0x81, 0x08}} //  10
, {TEL_TX,  7, 0, NULL                , {0xB0, 0x11, 0x7E, 0x00, 0x01, 0x60, 0xCA}} //  11
, {TEL_TX, 11, 0, NULL                , {0xB0, 0x11, 0x7E, 0x00, 0x01, 0x64, 0x49, 0xC1, 0x08, 0x05, 0xB0}} //  12
, {TEL_RX,  7, 0, NULL                , {0xB0, 0x00, 0x01, 0x11, 0x7E, 0x60, 0xCA}} //  13
, {TEL_RX, 10, 2, NULL                , {0xB0, 0x00, 0x01, 0x11, 0x7E, 0x63, 0x4E, 0x01, 0x00, 0x60}} //  14
, {TEL_TX,  7, 0, NULL                , {0xB0, 0x11, 0x7E, 0x00, 0x01, 0x60, 0xCE}} //  15
, {TEL_TX, 11, 0, NULL                , {0xB0, 0x11, 0x7E, 0x00, 0x01, 0x64, 0x4E, 0x41, 0x00, 0x60, 0x00}} //  16
, {TEL_RX,  7, 0, NULL                , {0xB0, 0x00, 0x01, 0x11, 0x7E, 0x60, 0xCE}} //  17
, {TEL_RX, 10, 2, NULL                , {0xB0, 0x00, 0x01, 0x11, 0x7E, 0x63, 0x52, 0x01, 0x01, 0x0D}} //  18
, {TEL_TX,  7, 0, NULL                , {0xB0, 0x11, 0x7E, 0x00, 0x01, 0x60, 0xD2}} //  19
, {TEL_TX, 11, 0, NULL                , {0xB0, 0x11, 0x7E, 0x00, 0x01, 0x64, 0x52, 0x41, 0x01, 0x0D, 0x00}} //  20
, {TEL_RX,  7, 0, NULL                , {0xB0, 0x00, 0x01, 0x11, 0x7E, 0x60, 0xD2}} //  21
, {TEL_RX,  9, 2, NULL                , {0xB0, 0x00, 0x01, 0x11, 0x7E, 0x62, 0x55, 0x84, 0x08}} //  22
, {TEL_TX,  7, 0, NULL                , {0xB0, 0x11, 0x7E, 0x00, 0x01, 0x60, 0xD6}} //  23
, {TEL_TX, 11, 0, NULL                , {0xB0, 0x11, 0x7E, 0x00, 0x01, 0x64, 0x55, 0xC4, 0x08, 0x05, 0xB0}} //  24
, {TEL_RX,  7, 0, NULL                , {0xB0, 0x00, 0x01, 0x11, 0x7E, 0x60, 0xD6}} //  25
, {TEL_RX, 10, 2, NULL                , {0xB0, 0x00, 0x01, 0x11, 0x7E, 0x63, 0x5A, 0x04, 0x01, 0x04}} //  26
, {TEL_TX,  7, 0, NULL                , {0xB0, 0x11, 0x7E, 0x00, 0x01, 0x60, 0xDA}} //  27
, {TEL_TX, 14, 0, NULL                , {0xB0, 0x11, 0x7E, 0x00, 0x01, 0x67, 0x5A, 0x44, 0x01, 0x04, 0x00, 0x00, 0x00, 0x00}} //  28
, {TEL_RX,  7, 0, NULL                , {0xB0, 0x00, 0x01, 0x11, 0x7E, 0x60, 0xDA}} //  29
, {TEL_RX, 10, 2, NULL                , {0xB0, 0x00, 0x01, 0x11, 0x7E, 0x63, 0x5E, 0x01, 0x00, 0x60}} //  30
, {TEL_TX,  7, 0, NULL                , {0xB0, 0x11, 0x7E, 0x00, 0x01, 0x60, 0xDE}} //  31
, {TEL_TX, 11, 0, NULL                , {0xB0, 0x11, 0x7E, 0x00, 0x01, 0x64, 0x5E, 0x41, 0x00, 0x60, 0x00}} //  32
, {TEL_RX,  7, 0, NULL                , {0xB0, 0x00, 0x01, 0x11, 0x7E, 0x60, 0xDE}} //  33
, {TEL_RX, 10, 2, NULL                , {0xB0, 0x00, 0x01, 0x11, 0x7E, 0x63, 0x62, 0x01, 0x01, 0x09}} //  34
, {TEL_TX,  7, 0, NULL                , {0xB0, 0x11, 0x7E, 0x00, 0x01, 0x60, 0xE2}} //  35
, {TEL_TX, 11, 0, NULL                , {0xB0, 0x11, 0x7E, 0x00, 0x01, 0x64, 0x62, 0x41, 0x01, 0x09, 0x00}} //  36
, {TEL_RX,  7, 0, NULL                , {0xB0, 0x00, 0x01, 0x11, 0x7E, 0x60, 0xE2}} //  37
, {TEL_RX,  7, 0, check_disconnect    , {0xB0, 0x00, 0x01, 0x11, 0x7E, 0x60, 0x81}} //  38
, {END}
};

Test_Case device_info =
{ "Device info"
, 0x00   // sbState
, tc_setup // init function
, device_info_t   // telegrams
};
