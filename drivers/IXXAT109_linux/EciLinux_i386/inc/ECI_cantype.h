///////////////////////////////////////////////////////////////////////////////
// (C) 2008-2011 IXXAT Automation GmbH, all rights reserved
///////////////////////////////////////////////////////////////////////////////
/**
  Definition of types, structs and unions for CAN initialization and 
  communication.

  @file ECI_cantype.h
*/


#ifndef __ECI_CANTYPE_H__
#define __ECI_CANTYPE_H__


//////////////////////////////////////////////////////////////////////////
// include files
#include "OsEci.h"

#include "ECI_pshpack1.h"

//////////////////////////////////////////////////////////////////////////
// constants and macros

/*****************************************************************************
* CAN baud rates (for CAN controller with 16Mhz)
****************************************************************************/
#define ECI_CAN_BT0_10KB           0x31   ///<  BT0   10 KB @ingroup CanTypes
#define ECI_CAN_BT1_10KB           0x1C   ///<  BT1   10 KB @ingroup CanTypes
#define ECI_CAN_BT0_20KB           0x18   ///<  BT0   20 KB @ingroup CanTypes
#define ECI_CAN_BT1_20KB           0x1C   ///<  BT1   20 KB @ingroup CanTypes
#define ECI_CAN_BT0_50KB           0x09   ///<  BT0   50 KB @ingroup CanTypes
#define ECI_CAN_BT1_50KB           0x1C   ///<  BT1   50 KB @ingroup CanTypes
#define ECI_CAN_BT0_100KB          0x04   ///<  BT0  100 KB @ingroup CanTypes
#define ECI_CAN_BT1_100KB          0x1C   ///<  BT1  100 KB @ingroup CanTypes
#define ECI_CAN_BT0_125KB          0x03   ///<  BT0  125 KB @ingroup CanTypes
#define ECI_CAN_BT1_125KB          0x1C   ///<  BT1  125 KB @ingroup CanTypes
#define ECI_CAN_BT0_250KB          0x01   ///<  BT0  250 KB @ingroup CanTypes
#define ECI_CAN_BT1_250KB          0x1C   ///<  BT1  250 KB @ingroup CanTypes
#define ECI_CAN_BT0_500KB          0x00   ///<  BT0  500 KB @ingroup CanTypes
#define ECI_CAN_BT1_500KB          0x1C   ///<  BT1  500 KB @ingroup CanTypes
#define ECI_CAN_BT0_800KB          0x00   ///<  BT0  800 KB @ingroup CanTypes
#define ECI_CAN_BT1_800KB          0x16   ///<  BT1  800 KB @ingroup CanTypes
#define ECI_CAN_BT0_1000KB         0x00   ///<  BT0 1000 KB @ingroup CanTypes
#define ECI_CAN_BT1_1000KB         0x14   ///<  BT1 1000 KB @ingroup CanTypes

#define ECI_CAN_BT01_10KB          0x31,0x1C   ///<   10 KB @ingroup CanTypes
#define ECI_CAN_BT01_20KB          0x18,0x1C   ///<   20 KB @ingroup CanTypes
#define ECI_CAN_BT01_50KB          0x09,0x1C   ///<   50 KB @ingroup CanTypes
#define ECI_CAN_BT01_100KB         0x04,0x1C   ///<  100 KB @ingroup CanTypes
#define ECI_CAN_BT01_125KB         0x03,0x1C   ///<  125 KB @ingroup CanTypes
#define ECI_CAN_BT01_250KB         0x01,0x1C   ///<  250 KB @ingroup CanTypes
#define ECI_CAN_BT01_500KB         0x00,0x1C   ///<  500 KB @ingroup CanTypes
#define ECI_CAN_BT01_800KB         0x00,0x16   ///<  800 KB @ingroup CanTypes
#define ECI_CAN_BT01_1000KB        0x00,0x14   ///< 1000 KB @ingroup CanTypes


/** Maximum possible 11bit CAN ID @ingroup CanTypes */
#define ECI_CAN_MAX_11BIT_ID 0x7FF

/** Maximum possible 29bit CAN ID @ingroup CanTypes */
#define ECI_CAN_MAX_29BIT_ID 0x1FFFFFFF

/**
  CAN controller types

  @ingroup CanTypes
*/
typedef enum
{
  ECI_CAN_CTRL_UNKNOWN = 0x00,         ///< unknown
  ECI_CAN_CTRL_82527   = 0x01,         ///< Intel 82527
  ECI_CAN_CTRL_82C200  = 0x02,         ///< Intel 82C200
  ECI_CAN_CTRL_81C90   = 0x03,         ///< Intel 81C90
  ECI_CAN_CTRL_81C92   = 0x04,         ///< Intel 81C92
  ECI_CAN_CTRL_SJA1000 = 0x05,         ///< Philips SJA 1000
  ECI_CAN_CTRL_82C900  = 0x06,         ///< Infineon 82C900 (TwinCAN)
  ECI_CAN_CTRL_TOUCAN  = 0x07,         ///< Motorola TOUCAN
  ECI_CAN_CTRL_MSCAN   = 0x08,         ///< Freescale Star12 MSCAN
  ECI_CAN_CTRL_FLEXCAN = 0x09,         ///< Freescale Coldfire FLEXCAN
  ECI_CAN_CTRL_IFI     = 0x0A,         ///< IFI CAN ( ALTERA FPGA CAN )
  ECI_CAN_CTRL_CCAN    = 0x0B,         ///< CCAN (Bosch C_CAN)
  ECI_CAN_CTRL_MAXVAL  = 0xFF          ///< Maximum value for controller type
} e_CANCTRLCLASS;


/**
  CAN controller supported features.

  @ingroup CanTypes
*/
typedef enum
{
  ECI_CAN_FEATURE_UNDEFINED      = 0x0000,  ///< undefined
  ECI_CAN_FEATURE_STDOREXT       = 0x0001,  ///< 11 OR 29 bit (exclusive)
  ECI_CAN_FEATURE_STDANDEXT      = 0x0002,  ///< 11 AND 29 bit (simultaneous)
  ECI_CAN_FEATURE_RMTFRAME       = 0x0004,  ///< Reception of remote frames
  ECI_CAN_FEATURE_ERRFRAME       = 0x0008,  ///< Reception of error frames
  ECI_CAN_FEATURE_BUSLOAD        = 0x0010,  ///< Bus load measurement
  ECI_CAN_FEATURE_IDFILTER       = 0x0020,  ///< Exact message filter
  ECI_CAN_FEATURE_LISTONLY       = 0x0040,  ///< Listen only mode
  ECI_CAN_FEATURE_SCHEDULER      = 0x0080,  ///< Cyclic message scheduler
  ECI_CAN_FEATURE_GENERRFRM      = 0x0100,  ///< Error frame generation
  ECI_CAN_FEATURE_DELAYEDTX      = 0x0200,  ///< Delayed message transmission
  ECI_CAN_FEATURE_SSM            = 0x0400,  ///< Single shot mode
  ECI_CAN_FEATURE_HI_PRIO        = 0x0800,   ///< High priority message
  ECI_CAN_FEATURE_STT            = 0x2000    ///< single transmission try messages with acknowledge error
} e_CANCTRLFEATURE;


/**
  CAN controller bus coupling types.

  @ingroup CanTypes
*/
typedef enum
{
  ECI_CAN_BUSC_UNDEFINED = 0x0000,     ///< undefined
  ECI_CAN_BUSC_LOWSPEED  = 0x0001,     ///< Low speed coupling
  ECI_CAN_BUSC_HIGHSPEED = 0x0002      ///< High speed coupling
} e_CANBUSC ;


/**
  CAN controller operating modes.

  @ingroup CanTypes
*/
typedef enum
{
  ECI_CAN_OPMODE_UNDEFINED     = 0x00,     ///< undefined
  ECI_CAN_OPMODE_STANDARD      = 0x01,     ///< Reception of 11-bit id messages
  ECI_CAN_OPMODE_EXTENDED      = 0x02,     ///< Reception of 29-bit id messages
  ECI_CAN_OPMODE_ERRFRAME      = 0x04,     ///< Enable reception of error frames
  ECI_CAN_OPMODE_LISTONLY      = 0x08,     ///< Listen only mode (TX passive)
  ECI_CAN_OPMODE_LOWSPEED      = 0x10,     ///< Use low speed bus interface
  ECI_CAN_OPMODE_STT           = 0x20,     ///< @deprecated enables single transmission try messages with acknowledge error
} e_CANOPMODE;


/**
  CAN message types (used by <CANMSGINFO.Bytes.bType>)

  @ingroup CanTypes
*/
typedef enum
{
  ECI_CAN_MSGTYPE_DATA    = 0,         ///< Data frame
  ECI_CAN_MSGTYPE_INFO    = 1,         ///< Info frame
  ECI_CAN_MSGTYPE_ERROR   = 2,         ///< Error frame
  ECI_CAN_MSGTYPE_STATUS  = 3,         ///< Status frame
  ECI_CAN_MSGTYPE_WAKEUP  = 4,         ///< Wakeup frame
  ECI_CAN_MSGTYPE_TIMEOVR = 5,         ///< Timer overrun
  ECI_CAN_MSGTYPE_TIMERST = 6          ///< Timer reset
} e_CANMSGTYPE;


/**
  CAN message information flags (used by <CANMSGINFO.Bytes.bFlags>).

  @ingroup CanTypes
*/
typedef enum
{
  ECI_CAN_MSGFLAGS_DLC = 0x0F,         ///< Data length code
  ECI_CAN_MSGFLAGS_OVR = 0x10,         ///< Data overrun flag
  ECI_CAN_MSGFLAGS_SRR = 0x20,         ///< Self reception request
  ECI_CAN_MSGFLAGS_RTR = 0x40,         ///< Remote transmission request
  ECI_CAN_MSGFLAGS_EXT = 0x80          ///< Frame format (0=11-bit, 1=29-bit)
} e_CANMSGFLAGS;


/**
  CAN message information flags2 (used by <CANMSGINFO.Bytes.bFlags2>).

  @ingroup CanTypes
*/
typedef enum
{
  ECI_CAN_MSGFLAGS_SSM = 0x01,         ///< Single shot message. @copydetails ECI_CANMSGINFO::ssm
  ECI_CAN_MSGFLAGS_HPM = 0x06,         ///< High priority message channel 0-3. @copydetails ECI_CANMSGINFO::hpm
} e_CANMSGFLAGS2;


/**
  Information supplied in the abData[0] field of info frames
  (CANMSGINFO.Bytes.bType = CAN_MSGTYPE_INFO).

  @ingroup CanTypes
*/
typedef enum
{
  ECI_CAN_INFO_START = 1,              ///< Start of CAN controller
  ECI_CAN_INFO_STOP  = 2,              ///< Stop of CAN controller
  ECI_CAN_INFO_RESET = 3               ///< Reset of CAN controller
} e_CANINFO;


/**
  Error information supplied in the abData[0] field of error frames
  (CANMSGINFO.Bytes.bType = CAN_MSGTYPE_ERROR).

  @ingroup CanTypes
*/
typedef enum
{
  ECI_CAN_ERROR_UNDEFINED = 0,             ///< Unknown or no error
  ECI_CAN_ERROR_STUFF     = 1,             ///< Stuff error
  ECI_CAN_ERROR_FORM      = 2,             ///< Form error
  ECI_CAN_ERROR_ACK       = 3,             ///< Acknowledgment error
  ECI_CAN_ERROR_BIT       = 4,             ///< Bit error
  ECI_CAN_ERROR_CRC       = 6,             ///< CRC error
  ECI_CAN_ERROR_OTHER     = 7              ///< Other (unspecified) error
} e_CANERROR;


/**
  Status information supplied in the abData[0] field of status frames
  (CANMSGINFO.Bytes.bType = ECI_CAN_MSGTYPE_STATUS) and in
  ECI_CANSTATUS::u::V0::dwStatus.

  @ingroup CanTypes
*/
typedef enum
{
  ECI_CAN_STATUS_TXPEND       = 0x01,        ///< Transmission pending
  ECI_CAN_STATUS_OVRRUN       = 0x02,        ///< Data overrun occurred
  ECI_CAN_STATUS_ERRLIM       = 0x04,        ///< Error warning limit exceeded
  ECI_CAN_STATUS_BUSOFF       = 0x08,        ///< Bus off status
  ECI_CAN_STATUS_ININIT       = 0x10,        ///< Init mode active
  ECI_CAN_STATUS_BUSCERR      = 0x20,        ///< Bus coupling error
  ECI_CAN_STATUS_ACKERR       = 0x40         ///< acknowledge error
} e_CANSTATUS;


/**
  Information on which tx fifo should be cleared

  @ingroup CanClearTxFifo
*/
typedef enum
{
  ECI_CAN_CLRDEV_REMPEND_BIT   = 0x01, ///< Remove the actual pending message
  ECI_CAN_CLRDEV_CTRL_BIT      = 0x02, ///< Clear the controller fifo
  ECI_CAN_CLRDEV_FIFO_BIT      = 0x04, ///< Clear the hardware fifo
  ECI_CAN_CLRDEV_COMM_BIT      = 0x08, ///< Clear the communication buffers
  ECI_CAN_CLRDRV_FIFO_BIT      = 0x10, ///< Clear the software fifo
  ECI_CAN_CLRSRV_FIFO_BIT      = 0x20, ///< Clear the software fifo
  ECI_CAN_CLRALL               = 0xFF  ///< Clear all
} e_CANCLEARTX;

/**
  filter selection HW filter sets acceptance filter
  and SW filter uses filter ids

  @ingroup CanTypes
*/
typedef enum
{
  ECI_CAN_HW_FILTER_STD = 0x00,         ///< select standard filter (11-bit)
  ECI_CAN_HW_FILTER_EXT = 0x80,         ///< select extended filter (29-bit)
  ECI_CAN_SW_FILTER_STD = 0x02,         ///< select standard filter (11-bit)
  ECI_CAN_SW_FILTER_EXT = 0x82          ///< select extended filter (29-bit)
} e_CANFILTER;

#define CAN_FILTER_ACC_CODE_ALL  0x00000000     ///< filter id to receive all messages
#define CAN_FILTER_ACC_MASK_ALL  0x00000000     ///< filter mask to receive all messages

#define CAN_FILTER_ACC_CODE_NONE 0xFFFFFFFF     ///< filter id to filter all messages
#define CAN_FILTER_ACC_MASK_NONE 0x80000000     ///< filter mask to filter all messages

#define ECI_CAN_MAX_11BIT_FILTERID ((ECI_CAN_MAX_11BIT_ID << 1) | 0x1)
#define ECI_CAN_MAX_29BIT_FILTERID ((ECI_CAN_MAX_29BIT_ID << 1) | 0x1)


//////////////////////////////////////////////////////////////////////////
// data types


/**
  CAN controller configuration.

  @ingroup CanTypes
*/
typedef struct
{
  DWORD dwVer;          ///< Version of valid union struct

  union
  {
    struct
    {
      BYTE  bOpMode;    ///< CAN operating mode @see e_CANOPMODE
      BYTE  bReserved;  ///< Reserved set to 0
      BYTE  bBtReg0;    ///< Bus timing register 0 according to SJA1000/16MHz
      BYTE  bBtReg1;    ///< Bus timing register 1 according to SJA1000/16MHz
    } __PACKED__  V0;               ///< Version 0
  } __PACKED__  u;                  ///< Version controlled structs container
} __PACKED__  ECI_CANINITLINE;


/**
  CAN controller capabilities.
  
  @ingroup CanTypes
*/
typedef struct
{
  DWORD dwVer;                ///< Version of valid union struct

  union
  {
    struct
    {
      WORD  wCanType;         ///< Type of CAN controller @see e_CANCTRLCLASS
      WORD  wBusCoupling;     ///< Type of Bus coupling @see e_CANBUSC
      DWORD dwFeatures;       ///< Supported features @see e_CANCTRLFEATURE
      DWORD dwClockFreq;      ///< Clock frequency of the primary counter in Hz
      DWORD dwTscDivisor;     ///< Divisor for the message time stamp counter
      DWORD dwDtxDivisor;     ///< Divisor for the delayed message transmitter
      DWORD dwDtxMaxTicks;    ///< Maximum tick count value of the delayed message transmitter
      DWORD dwNoOfPrioQueues; ///< Number of priority TX queues
    } __PACKED__  V0;                     ///< Version 0
  } __PACKED__  u;                        ///< Version controlled structs container
} __PACKED__  ECI_CANCAPABILITIES;


/**
  CAN controller status.
  
  @ingroup CanTypes
*/
typedef struct
{
  DWORD dwVer;                ///< Version of valid union struct

  union
  {
    struct
    {
      BYTE  bOpMode;          ///< Current CAN operating mode @see e_CANOPMODE
      BYTE  bBusLoad;         ///< Average bus load in percent (0..100)
      BYTE  bBtReg0;          ///< Current bus timing register 0 value according to SJA1000/16MHz
      BYTE  bBtReg1;          ///< Current bus timing register 1 value according to SJA1000/16MHz
      DWORD dwStatus;         ///< Status of the CAN controller @see e_CANSTATUS
    } __PACKED__  V0;                     ///< Version 0
  } __PACKED__  u;                        ///< Version controlled structs container
} __PACKED__  ECI_CANSTATUS;


/**
  CAN message information.
  
  @ingroup CanTypes
*/
typedef union
{
  struct
  {
    BYTE  bType;          ///< Message type @see e_CANMSGTYPE
    BYTE  bFlags2;        ///< Flags @see e_CANMSGFLAGS2
    BYTE  bFlags;         ///< Flags @see e_CANMSGFLAGS
    BYTE  bAccept;        ///< Acceptance filter code
  } __PACKED__  Bytes;                ///< CAN Message information in byte format

  struct
  {
    DWORD type: 8;        ///< Message type @see e_CANMSGTYPE
    DWORD ssm : 1;        ///< Single shot message (TX direction only).
                          ///< If set is tried to sent the message once only. If
                          ///< transmission or bus arbitration fails the message is discarded.
    DWORD hpm : 2;        ///< High priority message channel 0-3 (TX direction only).
                          ///< Selects the priority of the message where 0 is the lowest
                          ///< and "normal" message channel and 3 is the highest priority 
                          ///< message channel. The number of supported priority queues 
                          ///< is available in @ref ECI_CANCAPABILITIES::dwNoOfPrioQueues.
    DWORD res : 5;        ///< Reserved for future use
    DWORD dlc : 4;        ///< Data length code
    DWORD ovr : 1;        ///< Possible data overrun
    DWORD srr : 1;        ///< Self reception request
    DWORD rtr : 1;        ///< Remote transmission request
    DWORD ext : 1;        ///< Extended frame format (0=standard, 1=extended)
    DWORD afc : 8;        ///< Acceptance filter code
  } __PACKED__  Bits;                 ///< CAN Message information in bit format
} __PACKED__  ECI_CANMSGINFO;


/**
  CAN message structure.
  
  @ingroup CanTypes
*/
typedef struct
{
  DWORD dwVer;                    ///< Version of valid union struct

  union
  {
    struct
    {
      DWORD           dwTime;     ///< Time stamp for receive message in [us]
      DWORD           dwMsgId;    ///< CAN message identifier (INTEL format)
      ECI_CANMSGINFO  uMsgInfo;   ///< CAN message information (bit field)
      BYTE            abData[8];  ///< Message data
    } __PACKED__  V0;                         ///< Version 0
  } __PACKED__  u;                            ///< Version controlled structs container
} __PACKED__  ECI_CANMESSAGE;


/**
  CAN command structure.
  
  @ingroup CanTypes
*/
typedef struct
{
  WORD  wCode;                    ///< Command request code
} __PACKED__  ECI_CAN_CMD_REQ_HD;


/**
  CAN Command structure.
  
  @ingroup CanTypes
*/
typedef struct
{
  WORD  wResult;                  ///< Command result code
} __PACKED__  ECI_CAN_CMD_RES_HD;


/**
  CAN command structure.
  
  @ingroup CanTypes
*/
typedef struct
{
  DWORD                   dwVer;      ///< Version of valid union struct

  union
  {
    struct
    {
      ECI_CAN_CMD_REQ_HD  sCmdHeader; ///< CAN command header @see ECI_CAN_CMD_REQ_HD
      DWORD               dwReserved; ///< reserved for future use
    } __PACKED__  V0;                             ///< Version 0
  } __PACKED__  u;                                ///< Version controlled structs container
} __PACKED__  ECI_CANCMDREQUEST;


/**
  CAN command structure.
  
  @ingroup CanTypes
*/
typedef struct
{
  DWORD                   dwVer;      ///< Version of valid union struct

  union
  {
    struct
    {
      ECI_CAN_CMD_RES_HD  sCmdHeader; ///< CAN command header @see ECI_CAN_CMD_RES_HD
      DWORD               dwReserved; ///< reserved for future use
    } __PACKED__  V0;                 ///< Version 0
  } __PACKED__  u;                    ///< Version controlled structs container
} __PACKED__  ECI_CANCMDRESPONSE;


/**
  CAN filter structure.
        
  @ingroup CanTypes
*/
typedef struct
{
  DWORD dwVer;                    ///< Version of valid union struct

  union
  {
    struct
    {
      DWORD dwIsExtended;         ///< specifies whether to choose extended or standard id
      DWORD dwCode;               ///< id code for filter settings
      DWORD dwMask;               ///< mask code for filter settings
    } __PACKED__  V0;             ///< Version 0
  } __PACKED__  u;                ///< Version controlled structs container
} __PACKED__  ECI_CANFILTER ;

#include "ECI_poppack.h"

#endif
