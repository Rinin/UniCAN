#include "unican.h"
#include "unican_hardware_example.h"
#include "usbtinreader.h"
#include "helperthread.h"
#include "QString"

USBtinReader *can_port = NULL;

void USBTin_unican_init(const QString &port_name){
    //Уничтожаем уже существующие объекты, если они есть.
    unican_close();
    //Создаём экземпляр с нужным именем порта.
    can_port = new USBtinReader(port_name);
    //Инициализируем unican вместе с хардверной частью.
    unican_init();
}

void USBTin_unican_close(){
    unican_close();
}

//Получить указатель на экземпляр для соединения
//с его сигналами и слотами.
USBtinReader *USBTin_get_can_port(){
    return can_port;
}

void can_HW_init(void){
    can_port->can_HW_init();
}

void can_HW_close(void)
{
    if(can_port){
        can_port->can_HW_close();
        delete can_port;
        can_port = NULL;
    }
}

/*
Эту функцию необходимо вызвать в обработчике сигнала пришедших данных,
соединённых с сигналом объекта, полученного при помощи
 */
void can_HW_receive_message (void)
{
    if(!can_port){
        return;
    }

    can_receive_message(&can_port->message);
}

void can_HW_send_message (can_message* msg)
{
    if(!can_port){
        return;
    }
    can_port->can_HW_send_message(msg);
}



