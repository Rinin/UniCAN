#include "usbtinreader.h"

#include <QCoreApplication>

QT_USE_NAMESPACE

USBtinReader::USBtinReader(const QString &serialPortName, QObject *parent)
    : QObject(parent)
    , m_standardOutput(stdout)
{
    m_serialPort = new QSerialPort;

    QObject::connect(m_serialPort, SIGNAL(readyRead()), this, SLOT(handleReadyRead()));
    QObject::connect(m_serialPort, SIGNAL(error(QSerialPort::SerialPortError serialPortError)),
                     this, SLOT(handleError(QSerialPort::SerialPortError serialPortError)));
    QObject::connect(&m_timer, SIGNAL(timeout()), this, SLOT(handleTimeout()));

    m_serialPortName.append(serialPortName);
    m_serialPortBaudRate = CAN_COM_BAUDRATE;
    m_rx_state = USBtinReader::IDLE;

    m_printCanMessage = false;
    m_printInSerialStream = false;
}

USBtinReader::USBtinReader(        QString *serialPortName,
                                   quint32 serialPortBaudRate,
                                   QObject *parent)
    : QObject(parent)
    , m_standardOutput(stdout)
{
    m_serialPort = new QSerialPort;

    QObject::connect(m_serialPort, SIGNAL(readyRead()), this, SLOT(handleReadyRead()));
    QObject::connect(m_serialPort, SIGNAL(error()), this, SLOT(handleError()));
    QObject::connect(&m_timer, SIGNAL(timeout()), this, SLOT(handleTimeout()));

    m_serialPortName.append(*serialPortName);
    m_serialPortBaudRate = serialPortBaudRate;
    m_rx_state = USBtinReader::IDLE;

    m_printCanMessage = false;
    m_printInSerialStream = false;
}

USBtinReader::~USBtinReader()
{
    can_HW_close();
}

void USBtinReader::handleReadyRead()
{
    QByteArray  var;
    qint32     i = 0;

    m_readData.append(m_serialPort->readAll());

    if (m_printInSerialStream)
        m_standardOutput << m_readData << endl;

    if (m_rx_state == USBtinReader::IDLE)
    {
        if (m_readData.count())
        {
            while (m_readData.count())
            {
                if (*(m_readData.mid(0, 1).data()) == CAN_MSG_STD_TAG )
                {
                    m_rx_state = USBtinReader::RXSTD;
                    break;  // of loop
                }
                else if (*(m_readData.mid(0, 1).data()) == CAN_MSG_EXT_TAG )
                {
                    m_rx_state = USBtinReader::RXEXT;
                    break;  // of loop
                }
                else if (*(m_readData.mid(0, 1).data()) == CAN_RTR_STD_TAG )
                {
                    m_rx_state = USBtinReader::RTRSTD;
                    break;  // of loop
                }
                else if (*(m_readData.mid(0, 1).data()) == CAN_RTR_EXT_TAG )
                {
                    m_rx_state = USBtinReader::RTREXT;
                    break;  // of loop
                }
                m_readData = m_readData.right( m_readData.count() - 1 );
            }
        }
    }

    // no 'else if' here, we process packet at the same function entry
    switch (m_rx_state)
    {
    case USBtinReader::RXSTD :
        if (m_readData.count() >= CAN_MSG_HEADER_SZ)
        {
            // clear can msg struct
            memset((char*)&message, 0x00, sizeof(can_message));

            var = m_readData.mid(CAN_MSG_ID_POS, 3);
            message.can_identifier = var.toInt(0,16);

            var = m_readData.mid(CAN_MSG_DLEN_POS, 1);
            message.can_dlc = var.toInt(0,16);
            message.can_dlc = (message.can_dlc > CAN_MSG_DLEN_MAX) ? CAN_MSG_DLEN_MAX : message.can_dlc;

            if (m_readData.count() >= (CAN_MSG_HEADER_SZ + message.can_dlc) )
            {
                for(i = 0; i < message.can_dlc; i++)
                {
                    var = m_readData.mid(CAN_MSG_HEADER_SZ + i*CAN_MSG_CHAR_SZ, 2);
                    message.data[i] = var.toInt(0,16);
                }
                // TODO: here we call for can_receive_message (&tmp_msg);
                //
                can_receive_message(message);

                emit can_message_ready(message);
                //
                // clear processed data out from rx buffer
                // TODO: !!ask smdy about leaks!!! ->
                if ((CAN_MSG_HEADER_SZ + message.can_dlc * CAN_MSG_CHAR_SZ + 1) >= m_readData.count())
                    m_readData.clear();
                else
                    m_readData = m_readData.right( m_readData.count() - (CAN_MSG_HEADER_SZ + message.can_dlc * CAN_MSG_CHAR_SZ) );
                m_rx_state = USBtinReader::IDLE;
            }
        }
        break;
    case USBtinReader::RXEXT :
        if (m_readData.count() >= CAN_EXT_HEADER_SZ)
        {
            // clear can msg struct
            memset((char*)&message, 0x00, sizeof(can_message));
            message.can_extbit = 1;

            var = m_readData.mid(CAN_EXT_ID_POS, 8);
            message.can_identifier = var.toInt(0,16);

            var = m_readData.mid(CAN_EXT_DLEN_POS, 1);
            message.can_dlc = var.toInt(0,16);
            message.can_dlc = (message.can_dlc > CAN_MSG_DLEN_MAX) ? CAN_MSG_DLEN_MAX : message.can_dlc;

            if (m_readData.count() >= (CAN_EXT_HEADER_SZ + message.can_dlc) )
            {
                for(i = 0; i < message.can_dlc; i++)
                {
                    var = m_readData.mid(CAN_EXT_HEADER_SZ + i*CAN_MSG_CHAR_SZ, 2);
                    message.data[i] = var.toInt(0,16);
                }
                // TODO: here we call for can_receive_message (&tmp_msg);
                //
                can_receive_message(message);

                emit can_message_ready(message);
                //
                // clear processed data out from rx buffer
                if ((CAN_EXT_HEADER_SZ + message.can_dlc * CAN_MSG_CHAR_SZ + 1) >= m_readData.count())
                    m_readData.clear();
                else
                    m_readData = m_readData.right( m_readData.count() - (CAN_EXT_HEADER_SZ + message.can_dlc * CAN_MSG_CHAR_SZ) );
                m_rx_state = USBtinReader::IDLE;
            }
        }
        break;
    case USBtinReader::RTRSTD :
        if (m_readData.count() >= CAN_MSG_HEADER_SZ)
        {
            // clear can msg struct
            memset((char*)&message, 0x00, sizeof(can_message));
            message.can_rtr = 1;

            var = m_readData.mid(CAN_MSG_ID_POS, 3);
            message.can_identifier = var.toInt(0,16);

            var = m_readData.mid(CAN_MSG_DLEN_POS, 1);
            message.can_dlc = var.toInt(0,16);
            message.can_dlc = (message.can_dlc > CAN_MSG_DLEN_MAX) ? CAN_MSG_DLEN_MAX : message.can_dlc;

            // TODO: here we call for can_receive_message (&tmp_msg);
            //
            can_receive_message(message);

            emit can_message_ready(message);
            //
            // clear processed data out from rx buffer
            if ( CAN_MSG_HEADER_SZ >= m_readData.count() )
                m_readData.clear();
            else
                m_readData = m_readData.right( m_readData.count() - CAN_MSG_HEADER_SZ );
            m_rx_state = USBtinReader::IDLE;
        }
        break;
    case USBtinReader::RTREXT :
        if (m_readData.count() >= CAN_EXT_HEADER_SZ)
        {
            // clear can msg struct
            memset((char*)&message, 0x00, sizeof(can_message));
            message.can_rtr     = 1;
            message.can_extbit  = 1;

            var = m_readData.mid(CAN_EXT_ID_POS, 8);
            message.can_identifier = var.toInt(0,16);

            var = m_readData.mid(CAN_EXT_DLEN_POS, 1);
            message.can_dlc = var.toInt(0,16);
            message.can_dlc = (message.can_dlc > CAN_MSG_DLEN_MAX) ? CAN_MSG_DLEN_MAX : message.can_dlc;

            // TODO: here we call for can_receive_message (&tmp_msg);
            //
            can_receive_message(message);

            emit can_message_ready(message);
            //
            // clear processed data out from rx buffer
            if ( CAN_EXT_HEADER_SZ >= m_readData.count() )
                m_readData.clear();
            else
                m_readData = m_readData.right( m_readData.count() - CAN_EXT_HEADER_SZ );
            m_rx_state = USBtinReader::IDLE;
        }
        break;
    default :
        m_rx_state = USBtinReader::IDLE;
        break;
    }
}

void USBtinReader::handleTimeout()
{
    if (m_readData.isEmpty())
    {
        m_standardOutput << QObject::tr("No data was currently available for reading from port %1")
                            .arg(m_serialPort->portName()) << endl;
    }
    else
    {
        m_standardOutput << QObject::tr("Data successfully received from port %1")
                            .arg(m_serialPort->portName()) << endl;
        m_standardOutput << m_readData << endl;
    }

    QCoreApplication::quit();
}

void USBtinReader::handleError(QSerialPort::SerialPortError serialPortError)
{
    if (serialPortError == QSerialPort::ReadError) {
        m_standardOutput << QObject::tr("An I/O error occurred while reading the data from port %1, error: %2")
                            .arg(m_serialPort->portName())
                            .arg(m_serialPort->errorString()) << endl;
        QCoreApplication::exit(1);
    }
}

void USBtinReader::can_receive_message(can_message &msg)
{
    if (!m_printCanMessage)
        return;

    if ( (!msg.can_rtr) && (!msg.can_extbit) )
        m_standardOutput << QObject::tr("INCOMING: ");
    else if ( (!msg.can_rtr) && (msg.can_extbit) )
        m_standardOutput << QObject::tr("INCOMING EXTENDED: ");
    else if ( (msg.can_rtr) && (!msg.can_extbit) )
        m_standardOutput << QObject::tr("INCOMING RTR: ");
    else if ( (msg.can_rtr) && (msg.can_extbit) )
        m_standardOutput << QObject::tr("INCOMING EXTENDED RTR: ");

    m_standardOutput << QObject::tr("ID: %1, ").arg(msg.can_identifier);
    m_standardOutput << QObject::tr("DLEN: %1").arg(msg.can_dlc) << endl;
    if ( !msg.can_rtr)
    {
        m_standardOutput << QObject::tr("DATA: %1 %2 %3 %4 %5 %6 %7 %8")
                            .arg(msg.data[0])
                            .arg(msg.data[1])
                            .arg(msg.data[2])
                            .arg(msg.data[3])
                            .arg(msg.data[4])
                            .arg(msg.data[5])
                            .arg(msg.data[6])
                            .arg(msg.data[7]);
        m_standardOutput << QObject::tr("") << endl;
    }
}

qint32 USBtinReader::can_write_data(const char *data, quint32 len)
{
    qint64 bytesWritten = m_serialPort->write(data, len);

    if (bytesWritten == -1) {
        m_standardOutput << QObject::tr("Failed to write the data to port %1, error: %2")
                            .arg(m_serialPortName)
                            .arg(m_serialPort->errorString()) << endl;
        return 1;
    } else if (bytesWritten != len) {
        m_standardOutput << QObject::tr("Failed to write all the data to port %1, error: %2")
                            .arg(m_serialPortName)
                            .arg(m_serialPort->errorString()) << endl;
        return 1;
    } else if (!m_serialPort->waitForBytesWritten(5000)) {
        m_standardOutput << QObject::tr("Operation timed out or an error occurred for port %1, error: %2")
                            .arg(m_serialPortName)
                            .arg(m_serialPort->errorString()) << endl;
        return 1;
    }

    return 0;
}

qint32 USBtinReader::can_write_setup(const char *cmd, const char *resp)
{
    QString CmdStr;
    QString RespStr;
    QString RxStr;
    ;

    CmdStr.fromLocal8Bit(cmd);
    RespStr.fromLocal8Bit(resp);

    can_write_data(cmd, CmdStr.length());

    //
    if (RxStr.compare(RespStr) == 0)
        return 0;
    return ERROR_CMD_WRONG_RESP;
}

qint32 USBtinReader::can_read_data()
{
    QByteArray readData;

    readData = m_serialPort->readAll();
    while (m_serialPort->waitForReadyRead(20000))
        readData.append(m_serialPort->readAll());

    if (m_serialPort->error() == QSerialPort::ReadError)
    {
        m_standardOutput << QObject::tr("Failed to read from port %1, error: %2")
                            .arg(m_serialPortName)
                            .arg(m_serialPort->errorString()) << endl;
        return 1;
    } else if (m_serialPort->error() == QSerialPort::TimeoutError && readData.isEmpty())
    {
        m_standardOutput << QObject::tr("No data was currently available for reading from port %1")
                            .arg(m_serialPortName) << endl;
        return 0;
    }

    m_standardOutput << QObject::tr("Data successfully received from port %1")
                        .arg(m_serialPortName) << endl;
    m_standardOutput << readData << endl;

    return 0;
}

void USBtinReader::can_HW_send_message(can_message * msg)
{
    char    var[32];
    quint32 offset = 0;
    quint32 i = 0;
    ;

    msg->can_dlc = (msg->can_dlc > CAN_MSG_DLEN_MAX) ? CAN_MSG_DLEN_MAX : msg->can_dlc;

    if (msg->can_rtr)
    {
        if (msg->can_extbit)
        {
            sprintf(var, "R%08X%01d\r", (msg->can_identifier & 0x1FFFFFFF), msg->can_dlc );
            can_write_data(var, CAN_EXT_HEADER_SZ + 1);
        }
        else
        {
            sprintf(var, "r%03X%01d\r", (msg->can_identifier & 0x7FF), msg->can_dlc );
            can_write_data(var, CAN_MSG_HEADER_SZ + 1);
        }
    }
    else
    {
        if (msg->can_extbit)
        {
            sprintf(var, "T%08X%01d", (msg->can_identifier & 0x1FFFFFFF), msg->can_dlc );
            offset = CAN_EXT_HEADER_SZ;
            for (i = 0; i < msg->can_dlc; i++)\
            {
                sprintf( (char*)((int)var + offset), "%02X", msg->data[i] );
                offset += CAN_MSG_CHAR_SZ;
            }
            *(char*)((int)var + offset) = 0x0D;
            offset++;
            can_write_data(var, offset);
        }
        else
        {
            sprintf(var, "t%03X%01d", (msg->can_identifier & 0x7FF), msg->can_dlc );
            offset = CAN_MSG_HEADER_SZ;
            for (i = 0; i < msg->can_dlc; i++)\
            {
                sprintf( (char*)((int)var + offset), "%02X", msg->data[i] );
                offset += CAN_MSG_CHAR_SZ;
            }
            *(char*)((int)var + offset) = 0x0D;
            offset++;
            can_write_data(var, offset);
        }
    }
}

void USBtinReader::can_HW_init()
{
    m_serialPort->setPortName(m_serialPortName);
    m_serialPort->setBaudRate(m_serialPortBaudRate);
    if (!m_serialPort->open(QIODevice::ReadWrite))
    {
        m_standardOutput << QObject::tr("Failed to open port %1, error: %2")
                            .arg(m_serialPortName)
                            .arg(m_serialPort->errorString()) << endl;
    }
    else
    {
        can_write_data(CAN_CMD_SWVERSION, CAN_CMD_SWVERSION_LEN);
        can_write_data(CAN_CMD_HWVERSION, CAN_CMD_HWVERSION_LEN);
        can_write_data(CAN_CMD_BAUDRATE, CAN_CMD_BAUDRATE_LEN);
        can_write_data(CAN_CMD_OPENLINE, CAN_CMD_OPENLINE_LEN);
    }

    return;
}

void USBtinReader::can_HW_close()
{
    if (m_serialPort->isOpen())
    {
        can_write_data(CAN_CMD_CLOSELINE, CAN_CMD_CLOSELINE_LEN);
    }
    m_serialPort->close();

    return;
}

void USBtinReader::can_HW_receive_message()
{

}
