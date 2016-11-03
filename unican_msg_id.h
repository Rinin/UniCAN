#ifndef UNICAN_MESSAGE_ID_H_
#define UNICAN_MESSAGE_ID_H_


#define UNICAN_RATESENSOR_TELEMETRY_X 0x0B10
#define UNICAN_RATESENSOR_TELEMETRY_Y 0x0B11
#define UNICAN_RATESENSOR_TELEMETRY_Z 0x0B12

#define UNICAN_MAGSENSOR_TELEMETRY_X 0x0B13
#define UNICAN_MAGSENSOR_TELEMETRY_Y 0x0B14
#define UNICAN_MAGSENSOR_TELEMETRY_Z 0x0B15

#define UNICAN_ADCU_MACHINE_SWITCH_TO_STATE_IDLE         0xF000
#define UNICAN_ADCU_MACHINE_SWITCH_TO_STATE_BDOT         0xF001
#define UNICAN_ADCU_MACHINE_SWITCH_TO_STATE_TRIAD        0xF002
#define UNICAN_ADCU_MACHINE_SWITCH_TO_STATE_OBSERVE      0xF003


#define UNICAN_ADCU_TRIAD_SET_ANGLES                     0xF010


#define UNICAN_ADCU_CONFIG_SET_RATES_OFFSET_X            0xF100
#define UNICAN_ADCU_CONFIG_SET_RATES_OFFSET_Y            0xF101
#define UNICAN_ADCU_CONFIG_SET_RATES_OFFSET_Z            0xF102

#define UNICAN_ADCU_CONFIG_SET_MAG_OFFSET_X              0xF110
#define UNICAN_ADCU_CONFIG_SET_MAG_OFFSET_Y              0xF111
#define UNICAN_ADCU_CONFIG_SET_MAG_OFFSET_Z              0xF112

#define UNICAN_ADCU_REGULAR_TELEMETRY                    0xF200
#define UNICAN_ADCU_SET_REGULAR_TELEMETRY_TIMEOUT        0xF210



//Управление скоростью (об/мин)
#define UNICAN_WHEEL_CMD_ANGULAR_RATE				0x0A01
//Управление моментом (Нм)
#define UNICAN_WHEEL_CMD_TORQUE						0x0A10
//Выключить выдачу регулярной телеметрии
#define UNICAN_WHEEL_DISABLE_REGULAR_TELEMETRY		0x0A22
//Запросить выдачу телеметрии Маховика
#define UNICAN_WHEEL_GET_REGULAR_TELEMETRY			0x0A31
//Запросить выдачу общей телеметрии
#define UNICAN_WHEEL_GET_GENERYC_TELEMETRY			0x0A41
//Выключить питание маховика (инвертора)
#define UNICAN_WHEEL_OUTPUT_DISABLE					0x0A51
//Включить питание маховика (инвертора)
#define UNICAN_WHEEL_OUTPUT_ENABLE					0x0A52
//Изменить DestinationID (изменяется до перезагрузки, если не передать команду "Установить по умолчанию\n") <- FLASH
#define UNICAN_WHEEL_SET_DEFAULT_DESTINATION_ID		0x0A61
//Изменить SourseID  (изменяется до перезагрузки, если не передать команду "Установить по умолчанию\n")		<- FLASH
#define UNICAN_WHEEL_SET_UNICAN_ID 					0x0A62
//Включить выдачу регулярной телеметрии
#define UNICAN_WHEEL_ENABLE_REGULAR_TELEMETRY		0x0A63
//Установить Пропорциональный коэффициент																	<- FLASH
#define UNICAN_WHEEL_SET_PID_PROP_KOEF				0x0A64
//Установить Интегральный коэффициент																		<- FLASH
#define UNICAN_WHEEL_SET_PID_INTEGR_KOEF			0x0A65
//Установить Дифференциальный коэффициент																	<- FLASH
#define UNICAN_WHEEL_SET_PID_DIFF_KOEFF				0x0A66
//Записать новые настройки во FLASH																			<- FLASH
#define UNICAN_WHEEL_PROGRAMM_SETTINGS				0x0A60

//Ответ телеметрии маховика со скоростью
#define UNICAN_WHEEL_SPEED          	            0x0AD1
//Ответ телеметрии маховика с моментом
#define UNICAN_WHEEL_TORQUE         	            0x0AD2

//Задать максимум катушки X
#define UNICAN_EMD_MAX_X                            0x0003
//Задать минимум катушки X
#define UNICAN_EMD_MIN_X                            0x0004
//Задать ноль катушки X
#define UNICAN_EMD_ZERO_X                           0x0005

//Задать максимум катушки Y
#define UNICAN_EMD_MAX_Y                            0x0006
//Задать минимум катушки Y
#define UNICAN_EMD_MIN_Y                            0x0007
//Задать ноль катушки Y
#define UNICAN_EMD_ZERO_Y                           0x0008

//Задать максимум катушки Z
#define UNICAN_EMD_MAX_Z                            0x0009
//Задать минимум катушки Z
#define UNICAN_EMD_MIN_Z                            0x000A
//Задать ноль катушки Z
#define UNICAN_EMD_ZERO_Z                           0x000B

//Задать ноль всвем катушкам
#define UNICAN_EMD_ZERO_ALL                         0x000C


#define UNICAN_START_LONG_MESSAGE 0xFFFE

#endif /* UNICAN_MESSAGE_ID_H_ */
