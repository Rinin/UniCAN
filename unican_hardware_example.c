#include "unican.h"
#include "unican_hardware_example.h"

Typdef_CAN_HW_TX_buf CAN1_HW_TX_buf={
		.NowCanBufPoint=0,
		.LastCanBufPoint=0,
};

void CAN1_TX_IRQHandler(void)
{
	if(CAN_GetFlagStatus(CAN1, CAN_FLAG_RQCP0))
	{
		CAN_ClearFlag(CAN1, CAN_FLAG_RQCP0);

		CAN1_HW_TX_buf.NowCanBufPoint += 1;
		if (CAN1_HW_TX_buf.NowCanBufPoint==CAN1_HW_TX_buf.LastCanBufPoint) {
			NVIC_DisableIRQ(CAN1_TX_IRQn);
		}
		else{
			CAN_Transmit(CAN1, &CAN1_HW_TX_buf.TxMsg[CAN1_HW_TX_buf.NowCanBufPoint]);
		}
	}
}

void CAN1_RX0_IRQHandler(void)
{
	can_HW_receive_message();
}

void can_filter_init(uint16_t CAN_ID, uint8_t CAN_FilterScale)
{
	CAN_FilterInitTypeDef   CAN_FilterInitStructure;
	CAN_FilterInitStructure.CAN_FilterNumber = 0;

	CAN_FilterInitStructure.CAN_FilterMode       = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale      = CAN_FilterScale;
	CAN_FilterInitStructure.CAN_FilterIdHigh     = CAN_ID<<5;
	CAN_FilterInitStructure.CAN_FilterIdLow      = CAN_ID<<5;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x001F<<5;

	switch(CAN_FilterScale){
	case CAN_FilterScale_16bit:{
		CAN_FilterInitStructure.CAN_FilterMaskIdLow    = 0x001F<<5;
		break;}
	case CAN_FilterScale_32bit:{
		CAN_FilterInitStructure.CAN_FilterMaskIdLow    = 0x001F<<5;
		break;}
	default:break;
	}

	CAN_FilterInitStructure.CAN_FilterFIFOAssignment  = 0;
	CAN_FilterInitStructure.CAN_FilterActivation    = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);
}

void can_HW_init (void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

	GPIO_InitTypeDef  GPIO_InitStr;
	GPIO_StructInit(&GPIO_InitStr);

	GPIO_InitStr.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_12;
	GPIO_InitStr.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStr.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStr.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStr.GPIO_PuPd = GPIO_PuPd_NOPULL;

	GPIO_Init(GPIOA, &GPIO_InitStr);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_CAN1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_CAN1);

	CAN_DeInit(CAN1);
	CAN_InitTypeDef CAN_InitStructure;
	CAN_StructInit(&CAN_InitStructure);

	CAN_InitStructure.CAN_TTCM = DISABLE;
	CAN_InitStructure.CAN_ABOM = DISABLE;
	CAN_InitStructure.CAN_AWUM = DISABLE;
	CAN_InitStructure.CAN_NART = DISABLE;
	CAN_InitStructure.CAN_RFLM = DISABLE;
	CAN_InitStructure.CAN_TXFP = DISABLE;
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
	CAN_InitStructure.CAN_SJW  = CAN_SJW_4tq;

	CAN_InitStructure.CAN_BS1 = CAN_BS1_2tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;
	CAN_InitStructure.CAN_Prescaler = 5;
	CAN_Init(CAN1, &CAN_InitStructure);

	can_filter_init(UNICAN_ID, CAN_FilterScale_16bit);
	CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);
	CAN_ITConfig(CAN1, CAN_IT_TME, ENABLE);
	NVIC_EnableIRQ(CAN1_RX0_IRQn);
	/*end of user code*/
}

void can_HW_close(void)
{
	/* HW depended user code */
	CAN_DeInit(CAN1);
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
	can_message tmp_msg;

	/* HW depended user code */
	CanRxMsg RxMessage;
	uint8_t i;
	CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);

	tmp_msg.can_dlc=RxMessage.DLC;
	tmp_msg.can_extbit=RxMessage.IDE;
	switch(tmp_msg.can_extbit){
	case CAN_STANDART_HEADER:{
		tmp_msg.can_identifier=RxMessage.StdId ;
		break;}
	case CAN_EXTENDED_HEADER:{
		tmp_msg.can_identifier=RxMessage.ExtId;
		break;}
	default:break;
	}
	tmp_msg.can_rtr=RxMessage.RTR;

	for (i = 0; i < tmp_msg.can_dlc; ++i) {
		tmp_msg.data[i]=RxMessage.Data[i];
	}

	/*end of user code*/
	can_receive_message (&tmp_msg);
}

void can_HW_send_message (can_message* msg)
{
	/* HW depended user code */
	uint16_t i;

	switch(msg->can_extbit){
	case CAN_STANDART_HEADER:{
		CAN1_HW_TX_buf.TxMsg[CAN1_HW_TX_buf.LastCanBufPoint].StdId = msg->can_identifier;
		break;}
	case CAN_EXTENDED_HEADER:{
		CAN1_HW_TX_buf.TxMsg[CAN1_HW_TX_buf.LastCanBufPoint].ExtId = msg->can_identifier;
		break;}
	default:break;
	}

	CAN1_HW_TX_buf.TxMsg[CAN1_HW_TX_buf.LastCanBufPoint].RTR = msg->can_rtr;
	CAN1_HW_TX_buf.TxMsg[CAN1_HW_TX_buf.LastCanBufPoint].IDE = msg->can_extbit;
	CAN1_HW_TX_buf.TxMsg[CAN1_HW_TX_buf.LastCanBufPoint].DLC = msg->can_dlc;
	for(i=0;i<CAN1_HW_TX_buf.TxMsg[CAN1_HW_TX_buf.LastCanBufPoint].DLC;i++)
	{
		CAN1_HW_TX_buf.TxMsg[CAN1_HW_TX_buf.LastCanBufPoint].Data[i] =msg->data[i];
	}

	if (CAN1_HW_TX_buf.NowCanBufPoint==CAN1_HW_TX_buf.LastCanBufPoint) {
		CAN_Transmit(CAN1, &CAN1_HW_TX_buf.TxMsg[CAN1_HW_TX_buf.LastCanBufPoint]);
		NVIC_EnableIRQ(CAN1_TX_IRQn);
	}

	CAN1_HW_TX_buf.LastCanBufPoint += 1;
}



