#include "unican.h"
#include "CAN.h"


void can_HW_init (void)
{

  /* HW depended user code */


  /*end of user code*/

}

void can_HW_close(void)
{

  /* HW depended user code */

  /*end of user code*/
  
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
  /* HW depended user code */

  /*end of user code*/
}

/*
This function could be implemented in any way, but as
result user program should send provided can_message
to network.
Function name and parameters are mandatory
*/
void can_HW_send_message (can_message* msg)
{

	CAN_send(msg->can_identifier, msg->can_extbit,  msg->data, msg->can_dlc);

}



















// -------------------------------------------------------------------------
// can1.c --
//
/*/

#define CAN_FROM_RADIO1_BUS_ID	 		(ADDRESS_RADIO1<<5 | ADDRESS_OBC)
#define CAN_FROM_RADIO2_BUS_ID	 		(ADDRESS_RADIO2<<5 | ADDRESS_OBC)
#define CAN_FROM_OBC_BUS_ID				(ADDRESS_BKU<<5 | ADDRESS_OBC) //0x34
#define CAN_FROM_WSS_X_BUS_ID			(ADDRESS_WHEELS_X<<5 | ADDRESS_OBC)
#define CAN_FROM_WSS_Y_BUS_ID			(ADDRESS_WHEELS_Y<<5 | ADDRESS_OBC)
#define CAN_FROM_WSS_Z_BUS_ID			(ADDRESS_WHEELS_Z<<5 | ADDRESS_OBC)
#define CAN_FROM_WSS_R_BUS_ID			(ADDRESS_WHEELS_R<<5 | ADDRESS_OBC)
#define CAN_FROM_GYRO_1_BUS_ID			(ADDRESS_GYRO_1<<5 | ADDRESS_OBC)
#define CAN_FROM_GYRO_2_BUS_ID			(ADDRESS_GYRO_2<<5 | ADDRESS_OBC)
#define CAN_FROM_GYRO_3_BUS_ID			(ADDRESS_GYRO_3<<5 | ADDRESS_OBC)
#define CAN_FROM_GYRO_4_BUS_ID			(ADDRESS_GYRO_4<<5 | ADDRESS_OBC)
#define CAN_FROM_MTCU_BUS_ID			(ADDRESS_MTCU<<5 | ADDRESS_OBC)
#define CAN_FROM_VMS_BUS_ID				(ADDRESS_VMS<<5 | ADDRESS_OBC)
#define CAN_FROM_STAR_BUS_ID			(ADDRESS_STAR<<5 | ADDRESS_OBC)
#define CAN_FROM_PDCU_BUS_ID			(ADDRESS_PDCU<<5 | ADDRESS_OBC)

extern lp_aos_get_mode* 				aos_get_mode;
extern lp_stb_get_mode* 				stb_get_mode;

bool cdh_get_queue_stat( cdh_queue_stat* lp_queue, uchar uDataType ) CYGBLD_ATTRIB_WEAK;
bool cdh_getparameter( uchar uDataType, void* lpDataPtr, uchar tmp  ) CYGBLD_ATTRIB_WEAK;


//#define ADDRESS_DOKA					0x01 //DOKA  00010
//#define ADDRESS_OBC					0x14 //OBC 10100
static cyg_count32 recv_count = 0;
static cyg_count32 send_count = 0;
//static char  nCanErrCode = NOERR;
static uchar fCanInitialized = false;

static cyg_can_message tx_msg;
static cyg_can_event rx_event;
static cyg_io_handle_t can_handle;
// -------------------------------------------------------------------------
// can_init --
//



bool static add_can_filter (cyg_io_handle_t handle, unsigned short int filter)
{
	bool ok = true;
    cyg_can_filter rx_filter;
    cyg_uint32      len;
    ok = check_can_status();
    rx_filter.cfg_id  = CYGNUM_CAN_MSGBUF_RX_FILTER_ADD;
    rx_filter.msg.id  = filter & 0x7FF;
    rx_filter.msg.ext = CYGNUM_CAN_ID_STD;
    len = sizeof(rx_filter);
    ok = (ENOERR == cyg_io_set_config(handle, CYG_IO_SET_CONFIG_CAN_MSGBUF ,&rx_filter, &len));
    if (CYGNUM_CAN_MSGBUF_NA == rx_filter.handle)
        ok = false;
    return ok;
}

static bool can_do_reinit ( void )
{
	stb_config_info stb_info;
	aos_config_info aos_info;
	stb_get_mode( &stb_info );
	aos_get_mode( &aos_info );
	volatile bool ok_adding = true;
	bool ok = true;
	cyg_can_msgbuf_cfg     msgbox_cfg;
    cyg_uint32      len;


	msgbox_cfg.cfg_id = CYGNUM_CAN_MSGBUF_RESET_ALL;
	len = sizeof(msgbox_cfg);
	ok_adding = (ENOERR == cyg_io_set_config(can_handle, CYG_IO_SET_CONFIG_CAN_MSGBUF ,&msgbox_cfg, &len));
	if (!ok_adding)
		drv_add_err_log( MGR_DRV_CAN_ERRORS, DVR_COM_REINIT_ERROR, EXT_DRV_CAN, 0  );
	ok &= ok_adding;

	ok_adding = add_can_filter(can_handle,CAN_FROM_RADIO1_BUS_ID);
	if (!ok_adding)
		drv_add_err_log( MGR_DRV_CAN_ERRORS, DVR_COM_REINIT_ERROR, EXT_DRV_CAN, 1  );
	ok &= ok_adding;
	ok_adding = add_can_filter(can_handle,CAN_FROM_RADIO1_BUS_ID | 0x0400);//DATA_BIT
	if (!ok_adding)
		drv_add_err_log( MGR_DRV_CAN_ERRORS, DVR_COM_REINIT_ERROR, EXT_DRV_CAN, 2  );
	ok &= ok_adding;

	return ok_adding;
}


short int can_download( usint bid, uchar* bdata, uchar blen )
{
	char nCanErrCode = NOERR;
	if(!fCanInitialized)
	   	return ERR_COM_CAN_INIT;

	int i = 0;
    CYG_CAN_MSG_SET_PARAM(tx_msg, bid, CYGNUM_CAN_ID_STD, blen, CYGNUM_CAN_FRAME_DATA);
    for (i = 0; i < CYG_NELEM(tx_msg.data.bytes); i++)
         tx_msg.data.bytes[i] = bdata[i];//Надо подумать о замене на memcpy

    tx_msg.id = bid;
    cyg_uint32 len = sizeof(tx_msg);
    nCanErrCode = cyg_io_write(can_handle, &tx_msg, &len);
    if(nCanErrCode==ENOERR)
    {
    	send_count++;
    }
    return nCanErrCode;

}

short int can_upload(  usint* bid, uchar* bdata, uchar* blen )
{

	cyg_uint32 len = sizeof(rx_event);

	char nCanErrCode = cyg_io_read(can_handle, &rx_event, &len);
    if(nCanErrCode == ENOERR)
    {
        if (rx_event.flags & CYGNUM_CAN_EVENT_RX)
        {
        	//{//TODO Исправить логику работы CAN
            	*bid = rx_event.msg.id;
            	*blen = rx_event.msg.dlc;
            	memset (bdata, 0x00, CAN_DEF_MSG_SIZE);
				for (i = 0; i < CYG_NELEM(rx_event.msg.data.bytes); i++)
					bdata[i]=rx_event.msg.data.bytes[i];//Надо подумать о memcpy
				recv_count++;
        }
        else
        	nCanErrCode = -EAGAIN;
    }
    return nCanErrCode;
}


void can_flush_output(void)
{
	cyg_io_set_config(can_handle, CYG_IO_SET_CONFIG_CAN_OUTPUT_FLUSH ,NULL, NULL);
}

void can_flush_input(void)
{
	cyg_io_set_config(can_handle, CYG_IO_SET_CONFIG_CAN_INPUT_FLUSH ,NULL, NULL);
}
*/
