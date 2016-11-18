#include "stdint.h"
#include "unican.h"
#include "unican_user.h"
#include "mtime.h"
#include "stdio.h"
#include "string.h"
#include "PMSM_settings.h"
#include "PMSM_state_machine.h"
#include "PWM_tools.h"
#include "IO_tools.h"
#include "IO_structs.h"
#include "HALL_sensor.h"
#include "CURRENT_sensor.h"
#include "PMSM_settings.h"
#include "FLASH_tools.h"


//TODO: remove this ASAP!
volatile uint16_t unican_send_time = 0;


/*!
 * @brief Массив настроек и телеметрии контроллера, обновляемый через CAN.
 * 	По умолчанию вся телеметрия кроме угловой скорости по Калману отключена.
 * */
volatile uint32_t UNICAN_CONTROL_ARRAY[UNICAN_NUM_ARRAY_CODES] = {
		[unican_MSG_ID_to_ARRAY_index(UNICAN_ENABLE_FIRST_ENABLE_CODE) ... unican_MSG_ID_to_ARRAY_index(UNICAN_ENABLE_Aposteriori_W - 1)] = 0,
		[unican_MSG_ID_to_ARRAY_index(UNICAN_ENABLE_Aposteriori_W)] = 1,
		[unican_MSG_ID_to_ARRAY_index(UNICAN_ENABLE_Aposteriori_W + 1) ... unican_MSG_ID_to_ARRAY_index(UNICAN_ENABLE_LAST_ENABLE_CODE)] = 0
};

/*!
 * @brief Массив отображения UNICAN_ID в offset настройки и её размер в памяти.
 * Чтоб узнать размер и смещение интересующей настройки в структуре настроек PMSM_settings,
 * выполняется следующая последовательность действий:
 * uint32_t offset = UNICAN_MSG_ID_SETTINGS_MAP[unican_MSG_ID_to_ARRAY_index(UNICAN_MSG_ID)].offset;
 * uint32_t size = UNICAN_MSG_ID_SETTINGS_MAP[unican_MSG_ID_to_ARRAY_index(UNICAN_MSG_ID)].size;
 * Для примера см. функции
 * void unican_read_settings(PMSM_SETTINGS *PMSM_configuration)
 * void unican_write_settings(PMSM_SETTINGS *PMSM_configuration)
 * */
const UNICAN_DATA_FIELD_DESCRIPTOR UNICAN_MSG_ID_SETTINGS_MAP[UNICAN_SET_NUM_SET_CODES] = {
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_L)] = {
				.address_offset = offsetof(PMSM_SETTINGS, wheel_model.L),
				.size = sizeof(((PMSM_SETTINGS*)0)->wheel_model.L)
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_R)] = {
				.address_offset = offsetof(PMSM_SETTINGS, wheel_model.R),
				.size = sizeof(((PMSM_SETTINGS*)0)->wheel_model.R)
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_J)] = {
				.address_offset = offsetof(PMSM_SETTINGS, wheel_model.J),
				.size = sizeof(((PMSM_SETTINGS*)0)->wheel_model.J)
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_Kt)] = {
				.address_offset = offsetof(PMSM_SETTINGS, wheel_model.Kt),
				.size = sizeof(((PMSM_SETTINGS*)0)->wheel_model.Kt)
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_Friction_static)] = {
				.address_offset = offsetof(PMSM_SETTINGS, wheel_model.Friction_static),
				.size = sizeof(((PMSM_SETTINGS*)0)->wheel_model.Friction_static)
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_Friction_dynamic)] = {
				.address_offset = offsetof(PMSM_SETTINGS, wheel_model.Friction_dynamic),
				.size = sizeof(((PMSM_SETTINGS*)0)->wheel_model.Friction_dynamic)
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_Imax)] = {
				.address_offset = offsetof(PMSM_SETTINGS, wheel_model.I_max),
				.size = sizeof(((PMSM_SETTINGS*)0)->wheel_model.I_max)
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_Wmax)] = {
				.address_offset = offsetof(PMSM_SETTINGS, wheel_model.w_max),
				.size = sizeof(((PMSM_SETTINGS*)0)->wheel_model.w_max)
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_Umax)] = {
				.address_offset = offsetof(PMSM_SETTINGS, wheel_model.U_amplitude),
				.size = sizeof(((PMSM_SETTINGS*)0)->wheel_model.U_amplitude)
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_Q_11)] = {
				.address_offset = offsetof(PMSM_SETTINGS, Q_matrix_init[0][0]),
				.size = sizeof(((PMSM_SETTINGS*)0)->Q_matrix_init[0][0])
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_Q_22)] = {
				.address_offset = offsetof(PMSM_SETTINGS, Q_matrix_init[1][1]),
				.size = sizeof(((PMSM_SETTINGS*)0)->Q_matrix_init[1][1])
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_Q_33)] = {
				.address_offset = offsetof(PMSM_SETTINGS, Q_matrix_init[2][2]),
				.size = sizeof(((PMSM_SETTINGS*)0)->Q_matrix_init[2][2])
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_Q_44)] = {
				.address_offset = offsetof(PMSM_SETTINGS, Q_matrix_init[3][3]),
				.size = sizeof(((PMSM_SETTINGS*)0)->Q_matrix_init[3][3])
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_Q_55)] = {
				.address_offset = offsetof(PMSM_SETTINGS, Q_matrix_init[4][4]),
				.size = sizeof(((PMSM_SETTINGS*)0)->Q_matrix_init[4][4])
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_Q_66)] = {
				.address_offset = offsetof(PMSM_SETTINGS, Q_matrix_init[5][5]),
				.size = sizeof(((PMSM_SETTINGS*)0)->Q_matrix_init[5][5])
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_Q_77)] = {
				.address_offset = offsetof(PMSM_SETTINGS, Q_matrix_init[6][6]),
				.size = sizeof(((PMSM_SETTINGS*)0)->Q_matrix_init[6][6])
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_P_11)] = {
				.address_offset = offsetof(PMSM_SETTINGS, P_matrix_init[0][0]),
				.size = sizeof(((PMSM_SETTINGS*)0)->P_matrix_init[0][0])
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_P_22)] = {
				.address_offset = offsetof(PMSM_SETTINGS, P_matrix_init[1][1]),
				.size = sizeof(((PMSM_SETTINGS*)0)->P_matrix_init[1][1])
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_P_33)] = {
				.address_offset = offsetof(PMSM_SETTINGS, P_matrix_init[2][2]),
				.size = sizeof(((PMSM_SETTINGS*)0)->P_matrix_init[2][2])
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_P_44)] = {
				.address_offset = offsetof(PMSM_SETTINGS, P_matrix_init[3][3]),
				.size = sizeof(((PMSM_SETTINGS*)0)->P_matrix_init[3][3])
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_P_55)] = {
				.address_offset = offsetof(PMSM_SETTINGS, P_matrix_init[4][4]),
				.size = sizeof(((PMSM_SETTINGS*)0)->P_matrix_init[4][4])
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_P_66)] = {
				.address_offset = offsetof(PMSM_SETTINGS, P_matrix_init[5][5]),
				.size = sizeof(((PMSM_SETTINGS*)0)->P_matrix_init[5][5])
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_P_77)] = {
				.address_offset = offsetof(PMSM_SETTINGS, P_matrix_init[6][6]),
				.size = sizeof(((PMSM_SETTINGS*)0)->P_matrix_init[6][6])
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_R_11)] = {
				.address_offset = offsetof(PMSM_SETTINGS, R_matrix_init[0][0]),
				.size = sizeof(((PMSM_SETTINGS*)0)->R_matrix_init[0][0])
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_R_22)] = {
				.address_offset = offsetof(PMSM_SETTINGS, R_matrix_init[1][1]),
				.size = sizeof(((PMSM_SETTINGS*)0)->R_matrix_init[1][1])
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_R_33)] = {
				.address_offset = offsetof(PMSM_SETTINGS, R_matrix_init[2][2]),
				.size = sizeof(((PMSM_SETTINGS*)0)->R_matrix_init[2][2])
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_W_CONTR_DEADZONE)] = {
				.address_offset = offsetof(PMSM_SETTINGS, Angular_rate_controller.Dead_zone),
				.size = sizeof(((PMSM_SETTINGS*)0)->Angular_rate_controller.Dead_zone)
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_W_CONTR_Kp)] = {
				.address_offset = offsetof(PMSM_SETTINGS, Angular_rate_controller.Kp),
				.size = sizeof(((PMSM_SETTINGS*)0)->Angular_rate_controller.Kp)
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_W_CONTR_Ki)] = {
				.address_offset = offsetof(PMSM_SETTINGS, Angular_rate_controller.Ki),
				.size = sizeof(((PMSM_SETTINGS*)0)->Angular_rate_controller.Ki)
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_W_CONTR_Kd)] = {
				.address_offset = offsetof(PMSM_SETTINGS, Angular_rate_controller.Kd),
				.size = sizeof(((PMSM_SETTINGS*)0)->Angular_rate_controller.Kd)
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_W_CONTR_SatP)] = {
				.address_offset = offsetof(PMSM_SETTINGS, Angular_rate_controller.Proportional_error_saturation),
				.size = sizeof(((PMSM_SETTINGS*)0)->Angular_rate_controller.Proportional_error_saturation)
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_W_CONTR_SatI)] = {
				.address_offset = offsetof(PMSM_SETTINGS, Angular_rate_controller.Integral_error_saturation),
				.size = sizeof(((PMSM_SETTINGS*)0)->Angular_rate_controller.Integral_error_saturation)
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_W_CONTR_SatD)] = {
				.address_offset = offsetof(PMSM_SETTINGS, Angular_rate_controller.Diff_error_saturation),
				.size = sizeof(((PMSM_SETTINGS*)0)->Angular_rate_controller.Diff_error_saturation)
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_W_CONTR_SatGen)] = {
				.address_offset = offsetof(PMSM_SETTINGS, Angular_rate_controller.Output_saturation),
				.size = sizeof(((PMSM_SETTINGS*)0)->Angular_rate_controller.Output_saturation)
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_Torque_CONTR_DEADZONE)] = {
				.address_offset = offsetof(PMSM_SETTINGS, Torque_controller.Dead_zone),
				.size = sizeof(((PMSM_SETTINGS*)0)->Torque_controller.Dead_zone)
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_Torque_CONTR_Kp)] = {
				.address_offset = offsetof(PMSM_SETTINGS, Torque_controller.Kp),
				.size = sizeof(((PMSM_SETTINGS*)0)->Torque_controller.Kp)
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_Torque_CONTR_Ki)] = {
				.address_offset = offsetof(PMSM_SETTINGS, Torque_controller.Ki),
				.size = sizeof(((PMSM_SETTINGS*)0)->Torque_controller.Ki)
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_Torque_CONTR_Kd)] = {
				.address_offset = offsetof(PMSM_SETTINGS, Torque_controller.Kd),
				.size = sizeof(((PMSM_SETTINGS*)0)->Torque_controller.Kd)
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_Torque_CONTR_SatP)] = {
				.address_offset = offsetof(PMSM_SETTINGS, Torque_controller.Proportional_error_saturation),
				.size = sizeof(((PMSM_SETTINGS*)0)->Torque_controller.Proportional_error_saturation)
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_Torque_CONTR_SatI)] = {
				.address_offset = offsetof(PMSM_SETTINGS, Torque_controller.Integral_error_saturation),
				.size = sizeof(((PMSM_SETTINGS*)0)->Torque_controller.Integral_error_saturation)
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_Torque_CONTR_SatD)] = {
				.address_offset = offsetof(PMSM_SETTINGS, Torque_controller.Diff_error_saturation),
				.size = sizeof(((PMSM_SETTINGS*)0)->Torque_controller.Diff_error_saturation)
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_Torque_CONTR_SatGen)] = {
				.address_offset = offsetof(PMSM_SETTINGS, Torque_controller.Output_saturation),
				.size = sizeof(((PMSM_SETTINGS*)0)->Torque_controller.Output_saturation)
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_Iq_CONTR_DEADZONE)] = {
				.address_offset = offsetof(PMSM_SETTINGS, Iq_controller.Dead_zone),
				.size = sizeof(((PMSM_SETTINGS*)0)->Iq_controller.Dead_zone)
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_Iq_CONTR_Kp)] = {
				.address_offset = offsetof(PMSM_SETTINGS, Iq_controller.Kp),
				.size = sizeof(((PMSM_SETTINGS*)0)->Iq_controller.Kp)
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_Iq_CONTR_Ki)] = {
				.address_offset = offsetof(PMSM_SETTINGS, Iq_controller.Ki),
				.size = sizeof(((PMSM_SETTINGS*)0)->Iq_controller.Ki)
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_Iq_CONTR_Kd)] = {
				.address_offset = offsetof(PMSM_SETTINGS, Iq_controller.Kd),
				.size = sizeof(((PMSM_SETTINGS*)0)->Iq_controller.Kd)
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_Iq_CONTR_SatP)] = {
				.address_offset = offsetof(PMSM_SETTINGS, Iq_controller.Proportional_error_saturation),
				.size = sizeof(((PMSM_SETTINGS*)0)->Iq_controller.Proportional_error_saturation)
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_Iq_CONTR_SatI)] = {
				.address_offset = offsetof(PMSM_SETTINGS, Iq_controller.Integral_error_saturation),
				.size = sizeof(((PMSM_SETTINGS*)0)->Iq_controller.Integral_error_saturation)
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_Iq_CONTR_SatD)] = {
				.address_offset = offsetof(PMSM_SETTINGS, Iq_controller.Diff_error_saturation),
				.size = sizeof(((PMSM_SETTINGS*)0)->Iq_controller.Diff_error_saturation)
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_Iq_CONTR_SatGen)] = {
				.address_offset = offsetof(PMSM_SETTINGS, Iq_controller.Output_saturation),
				.size = sizeof(((PMSM_SETTINGS*)0)->Iq_controller.Output_saturation)
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_Id_CONTR_DEADZONE)] = {
				.address_offset = offsetof(PMSM_SETTINGS, Id_controller.Dead_zone),
				.size = sizeof(((PMSM_SETTINGS*)0)->Id_controller.Dead_zone)
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_Id_CONTR_Kp)] = {
				.address_offset = offsetof(PMSM_SETTINGS, Id_controller.Kp),
				.size = sizeof(((PMSM_SETTINGS*)0)->Id_controller.Kp)
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_Id_CONTR_Ki)] = {
				.address_offset = offsetof(PMSM_SETTINGS, Id_controller.Ki),
				.size = sizeof(((PMSM_SETTINGS*)0)->Id_controller.Ki)
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_Id_CONTR_Kd)] = {
				.address_offset = offsetof(PMSM_SETTINGS, Id_controller.Kd),
				.size = sizeof(((PMSM_SETTINGS*)0)->Id_controller.Kd)
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_Id_CONTR_SatP)] = {
				.address_offset = offsetof(PMSM_SETTINGS, Id_controller.Proportional_error_saturation),
				.size = sizeof(((PMSM_SETTINGS*)0)->Id_controller.Proportional_error_saturation)
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_Id_CONTR_SatI)] = {
				.address_offset = offsetof(PMSM_SETTINGS, Id_controller.Integral_error_saturation),
				.size = sizeof(((PMSM_SETTINGS*)0)->Id_controller.Integral_error_saturation)
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_Id_CONTR_SatD)] = {
				.address_offset = offsetof(PMSM_SETTINGS, Id_controller.Diff_error_saturation),
				.size = sizeof(((PMSM_SETTINGS*)0)->Id_controller.Diff_error_saturation)
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_Id_CONTR_SatGen)] = {
				.address_offset = offsetof(PMSM_SETTINGS, Id_controller.Output_saturation),
				.size = sizeof(((PMSM_SETTINGS*)0)->Id_controller.Output_saturation)
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_PWM_mode)] = {
				.address_offset = offsetof(PMSM_SETTINGS, PWM_mode),
				.size = sizeof(((PMSM_SETTINGS*)0)->PWM_mode)
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_WINDING_TOPOLOGY)] = {
				.address_offset = offsetof(PMSM_SETTINGS, wheel_model.Winding_topology),
				.size = sizeof(((PMSM_SETTINGS*)0)->wheel_model.Winding_topology)
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_NUMBER_OF_POLUS)] = {
				.address_offset = offsetof(PMSM_SETTINGS, wheel_model.p),
				.size = sizeof(((PMSM_SETTINGS*)0)->wheel_model.p)
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_TelemetryPeriod)] = {
				.address_offset = offsetof(PMSM_SETTINGS, unican_configuration.Telemetry_period),
				.size = sizeof(((PMSM_SETTINGS*)0)->unican_configuration.Telemetry_period)
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_FrictionEnable)] = {
				.address_offset = offsetof(PMSM_SETTINGS, Friction_compensation),
				.size = sizeof(((PMSM_SETTINGS*)0)->Friction_compensation)
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_SRC_ID)] = {
				.address_offset = offsetof(PMSM_SETTINGS, unican_configuration.UNICAN_SELF_ID),
				.size = sizeof(((PMSM_SETTINGS*)0)->unican_configuration.UNICAN_SELF_ID)
		},
		[unican_MSG_ID_to_SETTINGS_index(UNICAN_SET_DEST_ID)] = {
				.address_offset = offsetof(PMSM_SETTINGS, unican_configuration.UNICAN_DESTINATION_ID),
				.size = sizeof(((PMSM_SETTINGS*)0)->unican_configuration.UNICAN_DESTINATION_ID)
		}
};

const UNICAN_DATA_FIELD_DESCRIPTOR UNICAN_MSG_ID_TELEMETRY_MAP[UNICAN_TELEMETRY_NUM_TELEMETRY_CODES] = {

		//Телеметрия по датчикам двигателя.
		[unican_MSG_ID_to_TELEMETRY_index(UNICAN_TELEMETRY_Id)] = {
				.address_offset = offsetof(PMSM_state_machine, Models[PMSM_MODEL_HARDWARE_MODEL].I.d),
				.size = sizeof(((PMSM_state_machine*)0)->Models[PMSM_MODEL_HARDWARE_MODEL].I.d)
		},
		[unican_MSG_ID_to_TELEMETRY_index(UNICAN_TELEMETRY_Iq)] = {
				.address_offset = offsetof(PMSM_state_machine, Models[PMSM_MODEL_HARDWARE_MODEL].I.q),
				.size = sizeof(((PMSM_state_machine*)0)->Models[PMSM_MODEL_HARDWARE_MODEL].I.q)
		},
		[unican_MSG_ID_to_TELEMETRY_index(UNICAN_TELEMETRY_angle)] = {
				.address_offset = offsetof(PMSM_state_machine, Models[PMSM_MODEL_HARDWARE_MODEL].angle),
				.size = sizeof(((PMSM_state_machine*)0)->Models[PMSM_MODEL_HARDWARE_MODEL].angle)
		},
		[unican_MSG_ID_to_TELEMETRY_index(UNICAN_TELEMETRY_HALL1)] = {
				.address_offset = offsetof(PMSM_state_machine, Models[PMSM_MODEL_HARDWARE_MODEL].HALL_raw.A),
				.size = sizeof(((PMSM_state_machine*)0)->Models[PMSM_MODEL_HARDWARE_MODEL].HALL_raw.A)
		},
		[unican_MSG_ID_to_TELEMETRY_index(UNICAN_TELEMETRY_HALL2)] = {
				.address_offset = offsetof(PMSM_state_machine, Models[PMSM_MODEL_HARDWARE_MODEL].HALL_raw.B),
				.size = sizeof(((PMSM_state_machine*)0)->Models[PMSM_MODEL_HARDWARE_MODEL].HALL_raw.B)
		},
		[unican_MSG_ID_to_TELEMETRY_index(UNICAN_TELEMETRY_HALL3)] = {
				.address_offset = offsetof(PMSM_state_machine, Models[PMSM_MODEL_HARDWARE_MODEL].HALL_raw.C),
				.size = sizeof(((PMSM_state_machine*)0)->Models[PMSM_MODEL_HARDWARE_MODEL].HALL_raw.C)
		},
		[unican_MSG_ID_to_TELEMETRY_index(UNICAN_TELEMETRY_CURR1)] = {
				.address_offset = offsetof(PMSM_state_machine, Models[PMSM_MODEL_HARDWARE_MODEL].CURRENT_raw.A),
				.size = sizeof(((PMSM_state_machine*)0)->Models[PMSM_MODEL_HARDWARE_MODEL].CURRENT_raw.A)
		},
		[unican_MSG_ID_to_TELEMETRY_index(UNICAN_TELEMETRY_CURR2)] = {
				.address_offset = offsetof(PMSM_state_machine, Models[PMSM_MODEL_HARDWARE_MODEL].CURRENT_raw.B),
				.size = sizeof(((PMSM_state_machine*)0)->Models[PMSM_MODEL_HARDWARE_MODEL].CURRENT_raw.B)
		},
		[unican_MSG_ID_to_TELEMETRY_index(UNICAN_TELEMETRY_CURR3)] = {
				.address_offset = offsetof(PMSM_state_machine, Models[PMSM_MODEL_HARDWARE_MODEL].CURRENT_raw.C),
				.size = sizeof(((PMSM_state_machine*)0)->Models[PMSM_MODEL_HARDWARE_MODEL].CURRENT_raw.C)
		},

		//Телеметрия контроллера угловой скорости.
		[unican_MSG_ID_to_TELEMETRY_index(UNICAN_TELEMETRY_W_CONTR_Perror)] = {
				.address_offset = offsetof(PMSM_state_machine, PID_controllers[PMSM_PID_ANGULAR_RATE].Prop_error),
				.size = sizeof(((PMSM_state_machine*)0)->PID_controllers[PMSM_PID_ANGULAR_RATE].Prop_error)
		},
		[unican_MSG_ID_to_TELEMETRY_index(UNICAN_TELEMETRY_W_CONTR_Ierror)] = {
				.address_offset = offsetof(PMSM_state_machine, PID_controllers[PMSM_PID_ANGULAR_RATE].Integr_error),
				.size = sizeof(((PMSM_state_machine*)0)->PID_controllers[PMSM_PID_ANGULAR_RATE].Integr_error)
		},
		[unican_MSG_ID_to_TELEMETRY_index(UNICAN_TELEMETRY_W_CONTR_DError)] = {
				.address_offset = offsetof(PMSM_state_machine, PID_controllers[PMSM_PID_ANGULAR_RATE].Diff_error),
				.size = sizeof(((PMSM_state_machine*)0)->PID_controllers[PMSM_PID_ANGULAR_RATE].Diff_error)
		},
		[unican_MSG_ID_to_TELEMETRY_index(UNICAN_TELEMETRY_W_CONTR_TotError)] = {
				.address_offset = offsetof(PMSM_state_machine, PID_controllers[PMSM_PID_ANGULAR_RATE].Output),
				.size = sizeof(((PMSM_state_machine*)0)->PID_controllers[PMSM_PID_ANGULAR_RATE].Output)
		},

		//Телеметрия контроллера момента.
		[unican_MSG_ID_to_TELEMETRY_index(UNICAN_TELEMETRY_Torque_CONTR_Perror)] = {
				.address_offset = offsetof(PMSM_state_machine, PID_controllers[PMSM_PID_TORQUE].Prop_error),
				.size = sizeof(((PMSM_state_machine*)0)->PID_controllers[PMSM_PID_TORQUE].Prop_error)
		},
		[unican_MSG_ID_to_TELEMETRY_index(UNICAN_TELEMETRY_Torque_CONTR_Ierror)] = {
				.address_offset = offsetof(PMSM_state_machine, PID_controllers[PMSM_PID_TORQUE].Integr_error),
				.size = sizeof(((PMSM_state_machine*)0)->PID_controllers[PMSM_PID_TORQUE].Integr_error)
		},
		[unican_MSG_ID_to_TELEMETRY_index(UNICAN_TELEMETRY_Torque_CONTR_DError)] = {
				.address_offset = offsetof(PMSM_state_machine, PID_controllers[PMSM_PID_TORQUE].Diff_error),
				.size = sizeof(((PMSM_state_machine*)0)->PID_controllers[PMSM_PID_TORQUE].Diff_error)
		},
		[unican_MSG_ID_to_TELEMETRY_index(UNICAN_TELEMETRY_Torque_CONTR_TotError)] = {
				.address_offset = offsetof(PMSM_state_machine, PID_controllers[PMSM_PID_TORQUE].Output),
				.size = sizeof(((PMSM_state_machine*)0)->PID_controllers[PMSM_PID_TORQUE].Output)
		},

		//Телеметрия контроллера Iq
		[unican_MSG_ID_to_TELEMETRY_index(UNICAN_TELEMETRY_Iq_CONTR_Perror)] = {
				.address_offset = offsetof(PMSM_state_machine, PID_controllers[PMSM_PID_IQ].Prop_error),
				.size = sizeof(((PMSM_state_machine*)0)->PID_controllers[PMSM_PID_IQ].Prop_error)
		},
		[unican_MSG_ID_to_TELEMETRY_index(UNICAN_TELEMETRY_Iq_CONTR_Ierror)] = {
				.address_offset = offsetof(PMSM_state_machine, PID_controllers[PMSM_PID_IQ].Integr_error),
				.size = sizeof(((PMSM_state_machine*)0)->PID_controllers[PMSM_PID_IQ].Integr_error)
		},
		[unican_MSG_ID_to_TELEMETRY_index(UNICAN_TELEMETRY_Iq_CONTR_DError)] = {
				.address_offset = offsetof(PMSM_state_machine, PID_controllers[PMSM_PID_IQ].Diff_error),
				.size = sizeof(((PMSM_state_machine*)0)->PID_controllers[PMSM_PID_IQ].Diff_error)
		},
		[unican_MSG_ID_to_TELEMETRY_index(UNICAN_TELEMETRY_Iq_CONTR_TotError)] = {
				.address_offset = offsetof(PMSM_state_machine, PID_controllers[PMSM_PID_IQ].Output),
				.size = sizeof(((PMSM_state_machine*)0)->PID_controllers[PMSM_PID_IQ].Output)
		},

		//Телеметрия контроллера Id
		[unican_MSG_ID_to_TELEMETRY_index(UNICAN_TELEMETRY_Id_CONTR_Perror)] = {
				.address_offset = offsetof(PMSM_state_machine, PID_controllers[PMSM_PID_ID].Prop_error),
				.size = sizeof(((PMSM_state_machine*)0)->PID_controllers[PMSM_PID_ID].Prop_error)
		},
		[unican_MSG_ID_to_TELEMETRY_index(UNICAN_TELEMETRY_Id_CONTR_Ierror)] = {
				.address_offset = offsetof(PMSM_state_machine, PID_controllers[PMSM_PID_ID].Integr_error),
				.size = sizeof(((PMSM_state_machine*)0)->PID_controllers[PMSM_PID_ID].Integr_error)
		},
		[unican_MSG_ID_to_TELEMETRY_index(UNICAN_TELEMETRY_Id_CONTR_DError)] = {
				.address_offset = offsetof(PMSM_state_machine, PID_controllers[PMSM_PID_ID].Diff_error),
				.size = sizeof(((PMSM_state_machine*)0)->PID_controllers[PMSM_PID_ID].Diff_error)
		},
		[unican_MSG_ID_to_TELEMETRY_index(UNICAN_TELEMETRY_Id_CONTR_TotError)] = {
				.address_offset = offsetof(PMSM_state_machine, PID_controllers[PMSM_PID_ID].Output),
				.size = sizeof(((PMSM_state_machine*)0)->PID_controllers[PMSM_PID_ID].Output)
		},

		//Вектор состояния согласно апостериори-оценке фильтра Калмана.
		[unican_MSG_ID_to_TELEMETRY_index(UNICAN_TELEMETRY_Aposteriori_Id)] = {
				.address_offset = offsetof(PMSM_state_machine, Models[PMSM_MODEL_MATH_MODEL].I.d),
				.size = sizeof(((PMSM_state_machine*)0)->Models[PMSM_MODEL_MATH_MODEL].I.d)
		},
		[unican_MSG_ID_to_TELEMETRY_index(UNICAN_TELEMETRY_Aposteriori_Iq)] = {
				.address_offset = offsetof(PMSM_state_machine, Models[PMSM_MODEL_MATH_MODEL].I.q),
				.size = sizeof(((PMSM_state_machine*)0)->Models[PMSM_MODEL_MATH_MODEL].I.q)
		},
		[unican_MSG_ID_to_TELEMETRY_index(UNICAN_TELEMETRY_Aposteriori_angle)] = {
				.address_offset = offsetof(PMSM_state_machine, Models[PMSM_MODEL_MATH_MODEL].angle),
				.size = sizeof(((PMSM_state_machine*)0)->Models[PMSM_MODEL_MATH_MODEL].angle)
		},
		[unican_MSG_ID_to_TELEMETRY_index(UNICAN_TELEMETRY_Aposteriori_W)] = {
				.address_offset = offsetof(PMSM_state_machine, Models[PMSM_MODEL_MATH_MODEL].w),
				.size = sizeof(((PMSM_state_machine*)0)->Models[PMSM_MODEL_MATH_MODEL].w)
		},
		[unican_MSG_ID_to_TELEMETRY_index(UNICAN_TELEMETRY_Aposteriori_R)] = {
				.address_offset = offsetof(PMSM_state_machine, Models[PMSM_MODEL_MATH_MODEL].R),
				.size = sizeof(((PMSM_state_machine*)0)->Models[PMSM_MODEL_MATH_MODEL].R)
		},
		[unican_MSG_ID_to_TELEMETRY_index(UNICAN_TELEMETRY_Aposteriori_Kf)] = {
				.address_offset = offsetof(PMSM_state_machine, Models[PMSM_MODEL_MATH_MODEL].Friction_dynamic),
				.size = sizeof(((PMSM_state_machine*)0)->Models[PMSM_MODEL_MATH_MODEL].Friction_dynamic)
		},
		[unican_MSG_ID_to_TELEMETRY_index(UNICAN_TELEMETRY_Aposteriori_Tl)] = {
				.address_offset = offsetof(PMSM_state_machine, Models[PMSM_MODEL_MATH_MODEL].Friction_static),
				.size = sizeof(((PMSM_state_machine*)0)->Models[PMSM_MODEL_MATH_MODEL].Friction_static)
		},
		[unican_MSG_ID_to_TELEMETRY_index(UNICAN_TELEMETRY_Aposteriori_Eps)] = {
				.address_offset = offsetof(PMSM_state_machine, Models[PMSM_MODEL_MATH_MODEL].e),
				.size = sizeof(((PMSM_state_machine*)0)->Models[PMSM_MODEL_MATH_MODEL].e)
		},
		[unican_MSG_ID_to_TELEMETRY_index(UNICAN_TELEMETRY_Aposteriori_Torque)] = {
				.address_offset = offsetof(PMSM_state_machine, Models[PMSM_MODEL_MATH_MODEL].Torque),
				.size = sizeof(((PMSM_state_machine*)0)->Models[PMSM_MODEL_MATH_MODEL].Torque)
		},
		[unican_MSG_ID_to_TELEMETRY_index(UNICAN_TELEMETRY_Ud)] = {
				.address_offset = offsetof(PMSM_state_machine, Models[PMSM_MODEL_MATH_MODEL].U.d),
				.size = sizeof(((PMSM_state_machine*)0)->Models[PMSM_MODEL_MATH_MODEL].U.d)
		},
		[unican_MSG_ID_to_TELEMETRY_index(UNICAN_TELEMETRY_Uq)] = {
				.address_offset = offsetof(PMSM_state_machine, Models[PMSM_MODEL_MATH_MODEL].U.q),
				.size = sizeof(((PMSM_state_machine*)0)->Models[PMSM_MODEL_MATH_MODEL].U.q)
		},
		[unican_MSG_ID_to_TELEMETRY_index(UNICAN_TELEMETRY_Voltage1_measured)] = {
				.address_offset = offsetof(PMSM_state_machine, Models[PMSM_MODEL_MATH_MODEL].U_power),
				.size = sizeof(((PMSM_state_machine*)0)->Models[PMSM_MODEL_MATH_MODEL].U_power)
		},
		[unican_MSG_ID_to_TELEMETRY_index(UNICAN_TELEMETRY_System_time)] = {
				.address_offset = offsetof(PMSM_state_machine, Models[PMSM_MODEL_MATH_MODEL].model_time),
				.size = sizeof(((PMSM_state_machine*)0)->Models[PMSM_MODEL_MATH_MODEL].model_time)
		}
};

//Вспомогательный массива с названиями переменных/настроек для отладки.
const char *UNICAN_SETTINGS_STRINGS[UNICAN_SET_NUM_SET_CODES] = {
		"UNICAN_SET_L",
		"UNICAN_SET_R",
		"UNICAN_SET_J",
		"UNICAN_SET_Kt",
		"UNICAN_SET_Friction_static",
		"UNICAN_SET_Friction_dynamic",
		"UNICAN_SET_Imax",
		"UNICAN_SET_Wmax",
		"UNICAN_SET_Umax",
		"UNICAN_SET_Q_11",
		"UNICAN_SET_Q_22",
		"UNICAN_SET_Q_33",
		"UNICAN_SET_Q_44",
		"UNICAN_SET_Q_55",
		"UNICAN_SET_Q_66",
		"UNICAN_SET_Q_77",
		"UNICAN_SET_P_11",
		"UNICAN_SET_P_22",
		"UNICAN_SET_P_33",
		"UNICAN_SET_P_44",
		"UNICAN_SET_P_55",
		"UNICAN_SET_P_66",
		"UNICAN_SET_P_77",
		"UNICAN_SET_R_11",
		"UNICAN_SET_R_22",
		"UNICAN_SET_R_33",
		"UNICAN_SET_W_CONTR_DEADZONE",
		"UNICAN_SET_W_CONTR_Kp",
		"UNICAN_SET_W_CONTR_Ki",
		"UNICAN_SET_W_CONTR_Kd",
		"UNICAN_SET_W_CONTR_SatP",
		"UNICAN_SET_W_CONTR_SatI",
		"UNICAN_SET_W_CONTR_SatD",
		"UNICAN_SET_W_CONTR_SatGen",
		"UNICAN_SET_Torque_CONTR_DEADZONE",
		"UNICAN_SET_Torque_CONTR_Kp",
		"UNICAN_SET_Torque_CONTR_Ki",
		"UNICAN_SET_Torque_CONTR_Kd",
		"UNICAN_SET_Torque_CONTR_SatP",
		"UNICAN_SET_Torque_CONTR_SatI",
		"UNICAN_SET_Torque_CONTR_SatD",
		"UNICAN_SET_Torque_CONTR_SatGen",
		"UNICAN_SET_Iq_CONTR_DEADZONE",
		"UNICAN_SET_Iq_CONTR_Kp",
		"UNICAN_SET_Iq_CONTR_Ki",
		"UNICAN_SET_Iq_CONTR_Kd",
		"UNICAN_SET_Iq_CONTR_SatP",
		"UNICAN_SET_Iq_CONTR_SatI",
		"UNICAN_SET_Iq_CONTR_SatD",
		"UNICAN_SET_Iq_CONTR_SatGen",
		"UNICAN_SET_Id_CONTR_DEADZONE",
		"UNICAN_SET_Id_CONTR_Kp",
		"UNICAN_SET_Id_CONTR_Ki",
		"UNICAN_SET_Id_CONTR_Kd",
		"UNICAN_SET_Id_CONTR_SatP",
		"UNICAN_SET_Id_CONTR_SatI",
		"UNICAN_SET_Id_CONTR_SatD",
		"UNICAN_SET_Id_CONTR_SatGen",
		"TELEMETRYUNICAN_SET_PWM_mode",
		"UNICAN_SET_WINDING_TOPOLOGY",
		"UNICAN_SET_NUMBER_OF_POLUS",
		"UNICAN_SET_TelemetryPeriod",
		"UNICAN_SET_FrictionEnable",
		"UNICAN_SET_SRC_ID",
		"UNICAN_SET_DEST_ID",
};

//Вспомогательный массива с названиями переменных/выключателей для отладки.
const char *UNICAN_ENABLE_STRINGS[UNICAN_ENABLE_NUM_ENABLE_CODES] = {
		"UNICAN_ENABLE_Id",
		"UNICAN_ENABLE_Iq",
		"UNICAN_ENABLE_angle",
		"UNICAN_ENABLE_HALL1",
		"UNICAN_ENABLE_HALL2",
		"UNICAN_ENABLE_HALL3",
		"UNICAN_ENABLE_CURR1",
		"UNICAN_ENABLE_CURR2",
		"UNICAN_ENABLE_CURR3",
		"UNICAN_ENABLE_W_CONTR_Perror",
		"UNICAN_ENABLE_W_CONTR_Ierror",
		"UNICAN_ENABLE_W_CONTR_DError",
		"UNICAN_ENABLE_W_CONTR_TotError",
		"UNICAN_ENABLE_Torque_CONTR_Perror",
		"UNICAN_ENABLE_Torque_CONTR_Ierror",
		"UNICAN_ENABLE_Torque_CONTR_DError",
		"UNICAN_ENABLE_Torque_CONTR_TotError",
		"UNICAN_ENABLE_Iq_CONTR_Perror",
		"UNICAN_ENABLE_Iq_CONTR_Ierror",
		"UNICAN_ENABLE_Iq_CONTR_DError",
		"UNICAN_ENABLE_Iq_CONTR_TotError",
		"UNICAN_ENABLE_Id_CONTR_Perror",
		"UNICAN_ENABLE_Id_CONTR_Ierror",
		"UNICAN_ENABLE_Id_CONTR_DError",
		"UNICAN_ENABLE_Id_CONTR_TotError",
		"UNICAN_ENABLE_Aposteriori_Id",
		"UNICAN_ENABLE_Aposteriori_Iq",
		"UNICAN_ENABLE_Aposteriori_angle",
		"UNICAN_ENABLE_Aposteriori_W",
		"UNICAN_ENABLE_Aposteriori_R",
		"UNICAN_ENABLE_Aposteriori_Kf",
		"UNICAN_ENABLE_Aposteriori_Tl",
		"UNICAN_ENABLE_Aposteriori_Eps",
		"UNICAN_ENABLE_Aposteriori_Torque",
		"UNICAN_ENABLE_Ud",
		"UNICAN_ENABLE_Uq",
		"UNICAN_ENABLE_Voltage1_measured",
		"UNICAN_ENABLE_System_time"
};


/*
This event occures when valid unican message received
it's calls from within can_receive_message () so if
can_receive_message () called in interrupt handler
you will not want to do a lot of work here
 */
void unican_RX_event (uint16_t msg_id, uint16_t length)
{
	/* User code */
	unican_take_message();
	/*end of user code*/
}

/*!
 * @brief Копирует настройки из структуры-параметра в массив.
 * Эту функцию нужно вызвать при считывании настроек двигателя из
 * энергонезависимой памяти в начале работы двигателя.
 * */
void unican_settings_array_write(PMSM_SETTINGS *PMSM_configuration){
	DEBUG_PRINT("%s\n", __FUNCTION__);
	int setting_index;
	for(setting_index = 0; setting_index < UNICAN_SET_NUM_SET_CODES; ++setting_index){
		DEBUG_PRINT("reading: %d\n", setting_index);
		unican_set_array_by_MSG_ID(unican_SETTINGS_index_to_MSG_ID(setting_index),
				(void *)PMSM_configuration+UNICAN_MSG_ID_SETTINGS_MAP[setting_index].address_offset,
				UNICAN_MSG_ID_SETTINGS_MAP[setting_index].size);

	}
}


/*!
 * @brief Копирует настройки из  массива в структуру-параметр.
 * Эту функцию нужно вызвать при считывании настроек двигателя из
 * энергонезависимой памяти в начале работы двигателя.
 * */
void unican_settings_array_read(PMSM_SETTINGS *PMSM_configuration){
	DEBUG_PRINT("%s\n", __FUNCTION__);
	int setting_index;
	for(setting_index = 0; setting_index < UNICAN_SET_NUM_SET_CODES; ++setting_index){
		unican_get_array_by_MSG_ID(unican_SETTINGS_index_to_MSG_ID(setting_index),
				(void *)PMSM_configuration+UNICAN_MSG_ID_SETTINGS_MAP[setting_index].address_offset,
				UNICAN_MSG_ID_SETTINGS_MAP[setting_index].size);

	}
}

void unican_dump_settings(){
	DEBUG_PRINT("%s\n", __FUNCTION__);

	DEBUG_SECTION_BEGIN

	PMSM_SETTINGS *motor_settings;
	PMSM_settings_get_pointer(&motor_settings);

	unican_settings_array_write(motor_settings);

	int setting_index;
	for(setting_index = 0; setting_index < UNICAN_SET_NUM_SET_CODES; ++setting_index){

		union{
			float float_value;
			uint32_t uint32_value;
		}convert_union;
		memset(&convert_union, 0, sizeof(convert_union));

		unican_get_array_by_MSG_ID(unican_SETTINGS_index_to_MSG_ID(setting_index),
				&convert_union,
				UNICAN_MSG_ID_SETTINGS_MAP[setting_index].size);

		DEBUG_PRINT("%s: -> float: %32.16f, uint32_t: %d,\n", UNICAN_SETTINGS_STRINGS[setting_index],
				convert_union.float_value,
				convert_union.uint32_value);

	}

	DEBUG_SECTION_END
}

void unican_send_single_telemetry(uint16_t telemetry_index){
	unican_settings *unican_settings_ptr;
	unican_get_current_settings_pointer(&unican_settings_ptr);

	//Получаем текущее состояние всей системы (регуляторы, модели и пр).
	PMSM_state_machine *machine_pointer;
	PMSM_machine_get_machine_pointer(&machine_pointer);

	//Формируем CAN-пакет телеметрии.
	unican_message msg;
	msg.unican_address_from = unican_settings_ptr->UNICAN_SELF_ID;
	msg.unican_address_to = unican_settings_ptr->UNICAN_DESTINATION_ID;
	//TODO: remove +2 !
	msg.unican_length = 6;//UNICAN_MSG_ID_TELEMETRY_MAP[telemetry_index].size + 2;
	msg.unican_msg_id = unican_TELEMETRY_index_to_MSG_ID(telemetry_index);

	/* TODO: remove all the data_buffer containing lines, uncomment commented line.
	 * remove the unican_send_time also.
	*/
	const uint8_t timestamp_offset = 4;
	uint8_t data_buffer[6];
	memcpy(data_buffer,
			(uint8_t *)machine_pointer + UNICAN_MSG_ID_TELEMETRY_MAP[telemetry_index].address_offset,
			UNICAN_MSG_ID_TELEMETRY_MAP[telemetry_index].size);
	memcpy(data_buffer + timestamp_offset, (uint16_t *)&unican_send_time, sizeof(uint16_t));
	msg.data = data_buffer;

	//msg.data = (uint8_t *)machine_pointer + UNICAN_MSG_ID_TELEMETRY_MAP[telemetry_index].address_offset;
	unican_send_message(&msg);

	//Небольшая отладочная секция для вывода телеметрии в UART
	DEBUG_SECTION_BEGIN
	float debug_value;
	memcpy(&debug_value,
			(uint8_t *)machine_pointer + UNICAN_MSG_ID_TELEMETRY_MAP[telemetry_index].address_offset,
			UNICAN_MSG_ID_TELEMETRY_MAP[telemetry_index].size);
	DEBUG_PRINT("single telemetry:%s %f\n", UNICAN_ENABLE_STRINGS[telemetry_index], debug_value);
	DEBUG_SECTION_END
}

void unican_send_regular_telemetry(){
	static system_timer_typedef prev_timer;
	static system_timer_typedef cur_timer;
	static uint8_t prev_timer_initialized = 0;
	if(!prev_timer_initialized){
		prev_timer_initialized = 1;
		system_timer_update(&prev_timer);
	}

	unican_settings *unican_settings_ptr;
	unican_get_current_settings_pointer(&unican_settings_ptr);

	float delta_time;
	system_timer_update(&cur_timer);
	system_timer_get_diff_float(&cur_timer, &prev_timer, &delta_time);
	if(delta_time > unican_settings_ptr->Telemetry_period){
		prev_timer = cur_timer;

		//TODO: remove this ASAP!
		unican_send_time =  system_timer_get_time(&cur_timer) * 10;

		uint16_t telemetry_index;
		for(telemetry_index = 0; telemetry_index < UNICAN_TELEMETRY_NUM_TELEMETRY_CODES; ++telemetry_index){
			//проверяем, нужно ли выдавать некоторое значение, и если да, то выдаём его.
			uint8_t cur_telemetry_enabled;
			unican_get_array_by_MSG_ID(unican_ENABLE_index_to_MSG_ID(telemetry_index), &cur_telemetry_enabled, sizeof(uint8_t));
			if(cur_telemetry_enabled){
				unican_send_single_telemetry(telemetry_index);
			}
		}
	}
}

void unican_output_enable_hanlder(unican_message *msg){
	DEBUG_PRINT("%s\n", __FUNCTION__);
	uint8_t value;
	memcpy(&value, msg->data, sizeof(uint8_t));
	if(value){
		PWM_enable_output();
	}
	else{
		PWM_disable_output();
	}

	unican_set_array_by_MSG_ID(UNICAN_COMMAND_OutputEnable, &value, sizeof(uint8_t));
};

void unican_programm_new_settings(){
	DEBUG_PRINT("%s\n", __FUNCTION__);
	PMSM_settings_update_from_CAN();
	PMSM_settings_save_to_FLASH();
}

void unican_enabler_handler(unican_message *msg){
	DEBUG_PRINT("%s\n", __FUNCTION__);
	unican_set_array_by_MSG_ID(msg->unican_msg_id, msg->data, sizeof(uint8_t));
}

void unican_setter_handler(unican_message *msg){
	DEBUG_PRINT("%s\n%s\n", __FUNCTION__, UNICAN_SETTINGS_STRINGS[unican_MSG_ID_to_SETTINGS_index(msg->unican_msg_id)]);
	unican_set_array_by_MSG_ID(msg->unican_msg_id,
			msg->data,
			UNICAN_MSG_ID_SETTINGS_MAP[unican_MSG_ID_to_SETTINGS_index(msg->unican_msg_id)].size);
}

void unican_set_command(unican_message *msg){
	DEBUG_PRINT("%s\n", __FUNCTION__);

	float cur_time = system_timer_get_system_time();

	struct command_format{
		float command_value;
		uint16_t action_time; //Время в команде момента передаётся в миллисекундах.
	} new_command;
	memcpy(&new_command, msg->data, sizeof(new_command));

	PMSM_COMMAND_VALUE pmsm_command;
	pmsm_command.command_value_vector[PMSM_COMMAND_VALUE_0] = new_command.command_value;
	pmsm_command.command_value_vector[PMSM_COMMAND_VALUE_1] = cur_time + new_command.action_time*1.e-3f; //преобразуем время в секунды.

	PMSM_COMMAND_DESCRIPTOR PMSM_command_code = unican_get_PMSM_COMMAND_by_MSG_ID(msg->unican_msg_id);
	if(PMSM_COMMAND_BAD_COMMAND != PMSM_command_code){
		PMSM_machine_set_command(PMSM_command_code, &pmsm_command);
	}
}

void unican_apply_new_settings(){
	PMSM_settings_update_from_CAN();
}

void unican_command_handler(unican_message *msg){
	DEBUG_PRINT("%s\n", __FUNCTION__);

	switch(msg->unican_msg_id & 0xFFFF){
	case UNICAN_COMMAND_W_SIMPLE ... UNICAN_COMMAND_CALIB_CURRENT:{
		unican_set_command(msg);
	}break;
	case UNICAN_COMMAND_APPLY_SETTINGS:{
		unican_apply_new_settings();
	}break;
	case UNICAN_COMMAND_FLASH_WRITE:{
		unican_programm_new_settings();
	}break;
	case UNICAN_COMMAND_OutputEnable:{
		unican_output_enable_hanlder(msg);
	}break;
	default:{
		/*Остальные коды команд являются служебными
			и обрабатываться никак не должны.*/
	}break;
	}
}

/*
This function should perform required software
reaction to unican message received from another
device.
 */
void unican_RX_message (unican_message *msg)
{
	DEBUG_PRINT("%s\n", __FUNCTION__);

	switch(msg->unican_msg_id & 0xFFFF){
	case UNICAN_SET_FIRST_SET_CODE ... UNICAN_SET_LAST_SET_CODE:{
		//Сообщение - сеттер.
		unican_setter_handler(msg);
	}break;
	case UNICAN_COMMAND_FIRST_COMMAND_CODE ... UNICAN_COMMAND_LAST_COMMAND_CODE:{
		//обработка команд.
		unican_command_handler(msg);
	}break;
	case UNICAN_ENABLE_FIRST_ENABLE_CODE ... UNICAN_ENABLE_LAST_ENABLE_CODE:{
		//Разрешение телеметрии.
		unican_enabler_handler(msg);
	}break;
	}
}

/*
This function should perform required software
reaction to errors in unican
 */
void unican_error (uint16_t errcode)
{
};
