#ifndef SERIALPORTREADER_H
#define SERIALPORTREADER_H

#include <QtSerialPort/QSerialPort>

#include <QTextStream>
#include <QTimer>
#include <QByteArray>
#include <QObject>
#include <stdint.h>

#include "unican.h"

#define CAN_COM_NAME            QString("COM21")
#define CAN_COM_BAUDRATE        115200

#define CAN_CMD_BAUDRATE        "S8\r"
#define CAN_CMD_BAUDRATE_LEN    3
#define CAN_CMD_OPENLINE        "O\r"
#define CAN_CMD_OPENLINE_LEN    2
#define CAN_CMD_CLOSELINE       "C\r"
#define CAN_CMD_CLOSELINE_LEN   2
#define CAN_CMD_SWVERSION       "V\r"
#define CAN_CMD_SWVERSION_LEN   2
#define CAN_CMD_HWVERSION       "v\r"
#define CAN_CMD_HWVERSION_LEN   2

#define ERROR_CMD_WRONG_RESP    0x80000010

#define CAN_MSG_HEADER_SZ   5
#define CAN_MSG_CHAR_SZ     2
#define CAN_MSG_ID_POS      1
#define CAN_MSG_DLEN_POS    4
#define CAN_MSG_DLEN_MAX    8

#define CAN_EXT_HEADER_SZ   10
#define CAN_EXT_ID_POS      1
#define CAN_EXT_DLEN_POS    9

#define CAN_MSG_STD_TAG     't'
#define CAN_MSG_EXT_TAG     'T'
#define CAN_RTR_STD_TAG     'r'
#define CAN_RTR_EXT_TAG     'R'

QT_USE_NAMESPACE


// USBtinReader is HAL class for USB-Can converter known as USBtin project
// (http://www.fischl.de/usbtin/).
// Programmer: Roman Zharkih (Sputnix).
// Tested with Sputnix CANVerter V2.0 hardware. (Serial: 115200 8-N-1)
// CAN speed is strictly set to 1Mbit/s.
// Supports input/output of CAN data: standard and extended CAN messages,
// standard and extended RTR requests.
//
// Provides initialization and message exchange with hardware device via
// serial port. All the UART protocol is incapsulated, all the external
// communications use can_HW* functions:
//      can_HW_init     - initialization of serial port and USBtin setup
//      can_HW_close    - cut off USBtin from line.
//      can_HW_send_message - transmit CAN message (or RTR).
//      can_message_ready   - signal to pass received CAN message to user
//                              handler.
//
// Input parameters for new instance:
//      QString    * serialPortName - serial port system name where USBtin's attached.
//      quint32    serialPortBaudRate - serial port exhange speed (typically 115200).
//
// Additional parameters:
//      bool    m_printCanMessage - print incoming CAN packets to stdout (false by default).
//      bool    m_printInSerialStream - print all the incoming serial port
//                                      data stream to stdout (false by default).
//
// Typical usage:
//      1. Prepare parameters (Serial port name QString, port baudrate)
//          and create new instance of USBtinReader.
//      2. Call USBtinReader.can_HW_init() before do any exchanges.
//      3. Connect signal 'can_message_ready' to your handler
//      4. When you need to send something, create an instance of can_message
//          struct, fill it and call USBtinReader.can_HW_send_message(message).
//      5. Before program quit call USBtinReader.can_HW_close().

class USBtinReader : public QObject
{
    Q_OBJECT
    Q_ENUMS(can_rx_state)

public:
    enum can_rx_state
        {
            IDLE,
            RXSTD,
            RXEXT,
            RTRSTD,
            RTREXT
        };

    USBtinReader(const QString    &serialPortName,
                     QObject    * parent = 0);

    USBtinReader(    QString    * serialPortName,
                     quint32    serialPortBaudRate,
                     QObject    * parent = 0);

    ~USBtinReader();


private slots:
    void handleReadyRead();
    void handleTimeout();
    void handleError(QSerialPort::SerialPortError error);

private:
    QSerialPort     * m_serialPort;
    QByteArray      m_readData;
    QTextStream     m_standardOutput;
    QTimer          m_timer;
    can_rx_state    m_rx_state;
    QString         m_serialPortName;
    quint32         m_serialPortBaudRate;

    qint32 can_write_data(const char *data, quint32 len);
    qint32 can_write_setup(const char *cmd, const char *resp);
    qint32 can_read_data();

signals:
    void can_message_ready(can_message msg);

public:
    void can_receive_message(can_message &msg);
    void can_HW_send_message(can_message * msg);
    void can_HW_init();
    void can_HW_close();
    void can_HW_receive_message();

    bool    m_printCanMessage;
    bool    m_printInSerialStream;
    can_message message;
};

#endif
