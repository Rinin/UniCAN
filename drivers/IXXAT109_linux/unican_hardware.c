#include "../../unican.h"

static  ECI_CTRL_HDL handle;
static int EciHwUsePollingMode = 0;

void can_HW_init (void)
{
  ECI_RESULT hResult;
  ECI_HW_PARA stcHwPara = {0};
  stcHwPara.wHardwareClass = ECI_HW_USB;
  stcHwPara.dwFlags = ECI_SETTINGS_FLAG_NONE;

  if( EciHwUsePollingMode )
    stcHwPara.dwFlags = ECI_SETTINGS_FLAG_POLLING_MODE;

  hResult = ECIDRV_Initialize(1, &stcHwPara);

  if( hResult==ECI_OK )
  {
    uint8 bt0, bt1;
    DWORD dwHwIndex = 0;
    DWORD dwCtrlIndex = 0;
    handle = ECI_INVALID_HANDLE;

    bt0 = ECI_CAN_BT0_125KB;
    bt1 = ECI_CAN_BT1_125KB;

      ECI_CTRL_CONFIG stcCtrlConfig;

    int opmode = ECI_CAN_OPMODE_STANDARD | ECI_CAN_OPMODE_EXTENDED ;//|  ECI_CAN_OPMODE_ERRFRAME;

    memset(&stcCtrlConfig, 0, sizeof(stcCtrlConfig));

    stcCtrlConfig.wCtrlClass = ECI_CTRL_CAN;
    stcCtrlConfig.u.sCanConfig.dwVer = ECI_STRUCT_VERSION_V0;
    stcCtrlConfig.u.sCanConfig.u.V0.bBtReg0 = bt0;
    stcCtrlConfig.u.sCanConfig.u.V0.bBtReg1 = bt1;
    stcCtrlConfig.u.sCanConfig.u.V0.bOpMode = opmode;
    hResult = ECIDRV_CtrlOpen(&handle, dwHwIndex, dwCtrlIndex, &stcCtrlConfig);
    if (hResult == ECI_OK)
    {
      hResult = ECIDRV_CtrlStart (handle);
    }
    else
    {
      unican_error(UNICAN_HW_ERROR);
    }
  }
  else
  {
    unican_error(UNICAN_HW_ERROR);
  }
}

void can_HW_close(void)
{

	ECI_RESULT  hResult     = ECI_OK;

	//*** Stop Controller
	hResult = ECIDRV_CtrlStop(handle, ECI_STOP_FLAG_NONE);

	//*** Close ECI Controller
	hResult = ECIDRV_CtrlClose(handle);
	handle = ECI_INVALID_HANDLE;

	//*** Clean up ECI driver
	ECIDRV_Release();
}

/*
This function could be implemented in any way, but as
result user programm should call receive_can_message()
with valid can_message structures provided.
Function name and parameters are not mandatory, but
shold be changed in unican_user.h accordingly
*/
void can_HW_receive_message (void)
{
  can_message tmp_msg;
  /* HW depended user code */

  int i;
  DWORD count = 1;
  ECI_RESULT hResult;
  ECI_CTRL_MESSAGE stcCtrlMsg;
  memset(&stcCtrlMsg, 0, sizeof(stcCtrlMsg));

  if( EciHwUsePollingMode )
  {
    do
    {
      hResult = ECIDRV_CtrlReceive(handle, &count, &stcCtrlMsg, 0);
    }
    while( ECI_OK!=hResult );
  }
  else
  {
    hResult = ECIDRV_CtrlReceive(handle, &count, &stcCtrlMsg, 0);
  }
  if( hResult!=ECI_OK )
    {
      //unican_error(UNICAN_HW_ERROR);
    }
  else
    {
      tmp_msg.can_dlc = stcCtrlMsg.u.sCanMessage.u.V0.uMsgInfo.Bits.dlc;
      tmp_msg.can_extbit = stcCtrlMsg.u.sCanMessage.u.V0.uMsgInfo.Bits.ext;
      tmp_msg.can_identifier = stcCtrlMsg.u.sCanMessage.u.V0.dwMsgId;
      tmp_msg.can_rtr = stcCtrlMsg.u.sCanMessage.u.V0.uMsgInfo.Bits.rtr;
      for (i = 0; i<stcCtrlMsg.u.sCanMessage.u.V0.uMsgInfo.Bits.dlc; i++)
        tmp_msg.data[i]=stcCtrlMsg.u.sCanMessage.u.V0.abData[i];
      can_receive_message (&tmp_msg);
    }
  /*end of user code*/
}

/*
This function could be implemented in any way, but as
result user programm should send provided can_message
to network.
Function name and parameters are mandatory
*/
void can_HW_send_message (can_message* msg)
{
  /* HW depended user code */
 // bool can_send_packet ( ECI_CTRL_HDL* handle, tag_can_packet* packet )

    int i;
    ECI_RESULT hResult;
    int timeout = 500;
    ECI_CTRL_MESSAGE stcCtrlMsg;


    memset(&stcCtrlMsg, 0, sizeof(stcCtrlMsg));

    stcCtrlMsg.wCtrlClass = ECI_CTRL_CAN;
    stcCtrlMsg.u.sCanMessage.dwVer = ECI_STRUCT_VERSION_V0;
    stcCtrlMsg.u.sCanMessage.u.V0.dwMsgId = msg->can_identifier;
    stcCtrlMsg.u.sCanMessage.u.V0.uMsgInfo.Bits.dlc = msg->can_dlc;

    for(i=0; i<msg->can_dlc; i++){
      stcCtrlMsg.u.sCanMessage.u.V0.abData[i] = msg->data[i];//TODO: MemCpy?
    }

    if( EciHwUsePollingMode )
    {
      //DWORD dwStartTime = OS_GetTimeInMs();
      do
      {
        hResult = ECIDRV_CtrlSend(handle, &stcCtrlMsg, 0);
        //if( ECI_OK!=hResult )
        //  OS_Sleep(1);
      } while( ECI_OK!=hResult );
    }
    else
    {
      hResult = ECIDRV_CtrlSend(handle, &stcCtrlMsg, timeout);
    }

    if( hResult!=ECI_OK )
      {
        unican_error(UNICAN_HW_ERROR);
      }
  /*end of user code*/
}
