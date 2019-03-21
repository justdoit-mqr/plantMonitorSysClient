/*
 *@file:   client.cpp
 *@author: 缪庆瑞
 *@date:   2016.9.14
 *@brief:  客户端程序
 */
#include "client.h"
#include <QGridLayout>
#include <QMessageBox>

Client::Client(QWidget *parent) :
    QWidget(parent)
{
    /*通过随机数产生模拟产生设备原始数据，随机数种子放在构造生成一次就行
    如果放在下面的sendMessage()方法里，同一时刻多个客户端发送的模拟数据可能一致
    因为如果电脑性能高的话，同一毫秒，多个客户端都执行sendMessage(),随机种子就会
    一致，随机数也就一致*/
    QTime t= QTime::currentTime();
    qsrand(t.msec()+t.second()*1000);//通过当前时间产生一个随机数种子

    this->resize(350,200);
    this->setFont(QFont("宋体",12));
    initUi();
    refreshSlot();
    nextBlockSize=0;//初始化从套接字中读取一个数据块的大小为0

    clientSocket = new QTcpSocket(this);
    //连接信号与槽
    connect(clientSocket,SIGNAL(connected()),this,SLOT(showConnected()));
    connect(clientSocket,SIGNAL(readyRead()),this,SLOT(receiveMessage()));
    connect(clientSocket,SIGNAL(disconnected()),this,SLOT(showDisconnected()));
    connect(clientSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(error()));
}

Client::~Client()
{
}
/*
 *@brief:   初始化界面
 *@author:  缪庆瑞
 *@date:    2017.3.25
 */
void Client::initUi()
{
    timeCorrectBtn = new QPushButton(this);
    timeCorrectBtn->setText("时间校准");
    timeCorrectBtn->setEnabled(false);
    refreshBtn = new QPushButton(this);
    refreshBtn->setText("刷新");
    connectBtn = new QPushButton(this);
    connectBtn->setText("连接");
    disconnectBtn = new QPushButton(this);
    disconnectBtn->setText("断开连接");
    QLabel *connectStatus = new QLabel("连接状态:",this);
    statusLabel = new QLabel();
    statusLabel->setWordWrap(true);//连接状态label设置自动换行
    connect(timeCorrectBtn,SIGNAL(clicked()),this,SLOT(sendCurrentTime()));
    connect(refreshBtn,SIGNAL(clicked()),this,SLOT(refreshSlot()));
    connect(connectBtn,SIGNAL(clicked()),this,SLOT(createConnectToServer()));
    connect(disconnectBtn,SIGNAL(clicked()),this,SLOT(disconnectToServer()));

    QLabel *clientAddr = new QLabel("设备地址:",this);
    QLabel *serverAddr = new QLabel("服务器地址:",this);
    ipAddressInputC = new QLineEdit(this);
    ipAddressInputS = new QLineEdit(this);
    QLabel *presetV = new QLabel("给定电压:",this);
    QLabel *actualV = new QLabel("实际电压:",this);
    presetVoltage = new QLabel(this);
    actualVoltage = new QLabel(this);
    voltageInputL = new QLineEdit(this);
    voltageInputL->setText("0");
    voltageInputH = new QLineEdit(this);
    voltageInputH->setText("400");
    QLabel *presetA = new QLabel("给定电流:",this);
    QLabel *actualA = new QLabel("实际电流:",this);
    presetCurrent = new QLabel(this);
    actualCurrent = new QLabel(this);
    currentInputL = new QLineEdit(this);
    currentInputL->setText("0");
    currentInputH = new QLineEdit(this);
    currentInputH->setText("400");

    QGridLayout *gridLayout = new QGridLayout(this);
    gridLayout->addWidget(timeCorrectBtn,0,0,1,1);
    gridLayout->addWidget(refreshBtn,0,2,1,2);

    gridLayout->addWidget(clientAddr,1,0,1,1);
    gridLayout->addWidget(ipAddressInputC,1,1,1,1);
    gridLayout->addWidget(connectBtn,1,2,1,2);

    gridLayout->addWidget(serverAddr,2,0,1,1);
    gridLayout->addWidget(ipAddressInputS,2,1,1,1);
    gridLayout->addWidget(disconnectBtn,2,2,1,2);

    gridLayout->addWidget(presetV,3,0,1,1);
    gridLayout->addWidget(presetVoltage,3,1,1,1);
    //gridLayout->addWidget(refreshBtn,2,2,1,2);

    gridLayout->addWidget(actualV,4,0,1,1);
    gridLayout->addWidget(actualVoltage,4,1,1,1);
    gridLayout->addWidget(voltageInputL,4,2,1,1);
    gridLayout->addWidget(voltageInputH,4,3,1,1);

    gridLayout->addWidget(presetA,5,0,1,1);
    gridLayout->addWidget(presetCurrent,5,1,1,1);

    gridLayout->addWidget(actualA,6,0,1,1);
    gridLayout->addWidget(actualCurrent,6,1,1,1);
    gridLayout->addWidget(currentInputL,6,2,1,1);
    gridLayout->addWidget(currentInputH,6,3,1,1);

    gridLayout->addWidget(connectStatus,7,0,1,1);
    gridLayout->addWidget(statusLabel,7,1,1,3);

    gridLayout->setColumnStretch(0,1);
    gridLayout->setColumnStretch(1,2);
    gridLayout->setColumnStretch(2,1);
    gridLayout->setColumnStretch(3,1);
}
/*
 *@brief:   客户端向服务器发起连接
 *@author:  缪庆瑞
 *@date:    2016.9.14
 */
void Client::createConnectToServer()
{
    connectBtn->setEnabled(false);
    disconnectBtn->setEnabled(true);
    statusLabel->setText("正在连接服务器......");
    clientSocket->connectToHost(QHostAddress(ipAddressInputS->text()),6666);
}
/*
 *@brief:   客户端向服务器取消连接
 *@author:  缪庆瑞
 *@date:    2016.9.19
 */
void Client::disconnectToServer()
{
    connectBtn->setEnabled(true);
    disconnectBtn->setEnabled(false);
    //clientSocket->abort();//取消连接，立即关闭socket，清空数据
    clientSocket->disconnectFromHost();//取消连接，有待传输数据等数据传输完成再关闭socket
}
/*
 *@brief:   刷新随机数范围
 *@author:  缪庆瑞
 *@date:    2017.3.31
 */
void Client::refreshSlot()
{
    //清空显示数据
    actualVoltage->clear();
    presetVoltage->clear();
    actualCurrent->clear();
    presetCurrent->clear();

    voltageL = voltageInputL->text().toInt();
    voltageH = voltageInputH->text().toInt();
    currentL = currentInputL->text().toInt();
    currentH = currentInputH->text().toInt();
    //刷新客户端ip
    if(ipAddressInputC->text().isEmpty())
    {
        for(int i=0;i<4;i++)
        {
            ipSegment[i] = 0;
        }
    }
    else
    {
        QStringList ipAddrSegmentList = ipAddressInputC->text().split(".");
        if(ipAddrSegmentList.size()!=4)//
        {
            QMessageBox::information(this,"infomation",tr("设备ip地址格式不符合规范"));
            return;
        }
        else
        {
            bool ok=true;
            for(int i=0;i<4;i++)
            {
                int ip = ipAddrSegmentList.at(i).toInt(&ok);
                if(!ok||ip<0||ip>255)
                {
                    QMessageBox::information(this,"infomation",tr("设备ip地址格式不符合规范"));
                    return;
                }
            }
            for(int j=0;j<4;j++)
            {
                int ip = ipAddrSegmentList.at(j).toInt();
                ipSegment[j]=(quint8)ip;
            }
        }
    }
}
/*
 *@brief:   显示连接成功状态
 *@author:  缪庆瑞
 *@date:    2016.9.14
 */
void Client::showConnected()
{
    statusLabel->setText("服务器连接成功......");
    //timeCorrectBtn->setEnabled(true);//暂时停用时间校准功能
}
/*
 *@brief:   显示断开连接状态
 *@author:  缪庆瑞
 *@date:    2016.9.22
 */
void Client::showDisconnected()
{
    statusLabel->setText("与服务器断开连接......");
}
/*
 *@brief:   客户端根据服务器的请求报文发送消息
 *@author:  缪庆瑞
 *@date:    2016.9.14
 */
void Client::sendMessage()
{
    QByteArray rawData;
    QByteArray enciphedData;

    uint weldVoltage;//电压
    quint8 weldVoltageH;
    quint8 weldVoltageL;
    uint weldCurrent;//电流
    quint8 weldCurrentH;
    quint8 weldCurrentL;
    uint year;//年 一个字节放不下
    quint8 yearH;
    quint8 yearL;
    quint8 month;//月
    quint8 day;//日
    quint8 hour;//时
    quint8 minute;//分
    quint8 second;//秒

    if(voltageH == voltageL)//避免除数为0异常
    {
        weldVoltage = voltageH;
    }
    else
    {
        weldVoltage =qrand()%(voltageH-voltageL)+voltageL;//焊接电压 小数点1位
    }
    actualVoltage->setText(QString::number(weldVoltage/10.0,'f',1)+"V");
    weldVoltageH=((weldVoltage>>8)&0xff);//焊接电压高字节
    weldVoltageL=weldVoltage&0xff;//焊接电压低字节

    if(currentH == currentL)//避免除数为0异常
    {
        weldCurrent = currentH;
    }
    else
    {
        weldCurrent =qrand()%(currentH-currentL)+currentL;//焊接电流
    }
    actualCurrent->setText(QString::number(weldCurrent)+"A");
    weldCurrentH=(weldCurrent>>8)&0xff;//焊接电流高字节
    weldCurrentL=weldCurrent&0xff;//焊接电流低字节

    QDate date = QDate::currentDate();//获取当前日期与时间
    QTime time = QTime::currentTime();
    year = date.year();
    yearH = ((year>>8)&0xff);//年高字节
    yearL = year&0xff;//年低字节
    month =date.month();
    day = date.day();
    hour = time.hour();
    minute = time.minute();
    second = time.second();
    //添加原始数据
    {
        rawData.append(0x7e);//包头
        rawData.append(0xf1);
        rawData.append(0x02);//控制字段
        rawData.append(ipSegment[0]);//ip 4段 表示客户端ip地址
        rawData.append(ipSegment[1]);
        rawData.append(ipSegment[2]);
        rawData.append(ipSegment[3]);
        rawData.append(weldVoltageH);//焊接电压H/L
        rawData.append(weldVoltageL);
        rawData.append(weldCurrentH);//焊接电流H/L
        rawData.append(weldCurrentL);

        rawData.append(yearH);//年H/L
        rawData.append(yearL);
        rawData.append(month);
        rawData.append(day);
        rawData.append(hour);
        rawData.append(minute);
        rawData.append(second);

        quint32 checkSum = 0xf1+0x02+ipSegment[0]+ipSegment[1]+ipSegment[2]+ipSegment[3]+
                weldVoltageH+weldVoltageL+weldCurrentH+weldCurrentL+yearH+yearL+month+day+
                hour+minute+second;
        rawData.append(checkSum&0xFF);//校验和取低字节
        rawData.append(0x7D);//包尾
    }
    enciphedData.append(0x7E);//添加HDLC开始标志
    for(int i=1; i<rawData.size()-1; i++)
    {
        //数据字节中出现标志字段则需要做处理，具体根据公司HDLC协议标准
        switch (rawData.at(i))
        {
        case 0x7E:
        case 0x00:
        case 0x7C:
        case 0x7D:
            enciphedData.append(0x7C);
            enciphedData.append(rawData.at(i)^0x20);
            break;
        default: {enciphedData.append(rawData.at(i)); break;}
        }
    }
    enciphedData.append(0x7D);//添加HDLC结束标志
    enciphedData.prepend(enciphedData.size());
    //qDebug()<<enciphedData.toHex();
    clientSocket->write(enciphedData);
    //qDebug()<<QDateTime::currentDateTime().toString("hh:mm:ss:zzz");
}
/*
 *@brief:   向服务器发送当前时间，用来同步客户端与服务器
 *@author:  缪庆瑞
 *@date:    2017.6.1
 */
void Client::sendCurrentTime()
{
    QByteArray rawData;
    QByteArray enciphedData;

    uint year;//年 一个字节放不下
    quint8 yearH;
    quint8 yearL;
    quint8 month;//月
    quint8 day;//日
    quint8 hour;//时
    quint8 minute;//分
    quint8 second;//秒
    QDate date = QDate::currentDate();//获取当前日期与时间
    QTime time = QTime::currentTime();
    year = date.year();
    yearH = ((year>>8)&0xff);//年高字节
    yearL = year&0xff;//年低字节
    month =date.month();
    day = date.day();
    hour = time.hour();
    minute = time.minute();
    second = time.second();
    //添加原始数据
    {
        rawData.append(0x7e);//包头
        rawData.append(0xf1);
        rawData.append(0x03);//控制字段 0x03表示发送的为校准时间数据
        rawData.append(yearH);//年H/L
        rawData.append(yearL);
        rawData.append(month);
        rawData.append(day);
        rawData.append(hour);
        rawData.append(minute);
        rawData.append(second);

        quint32 checkSum = 0xf1+0x03+yearH+yearL+month+day+
                hour+minute+second;
        rawData.append(checkSum&0xFF);//校验和取低字节
        rawData.append(0x7D);//包尾
    }
    enciphedData.append(0x7E);//添加HDLC开始标志
    for(int i=1; i<rawData.size()-1; i++)
    {
        //数据字节中出现标志字段则需要做处理，具体根据公司HDLC协议标准
        switch (rawData.at(i))
        {
        case 0x7E:
        case 0x00:
        case 0x7C:
        case 0x7D:
            enciphedData.append(0x7C);
            enciphedData.append(rawData.at(i)^0x20);
            break;
        default: {enciphedData.append(rawData.at(i)); break;}
        }
    }
    enciphedData.append(0x7D);//添加HDLC结束标志
    enciphedData.prepend(enciphedData.size());
    //qDebug()<<enciphedData.toHex();
    clientSocket->write(enciphedData);
    timeCorrectBtn->setEnabled(false);//只校准一次就行
}
/*
 *@brief:   接收并处理服务器的HDLC请求报文
 *@author:  缪庆瑞
 *@date:    2016.9.14
 */
void Client::receiveMessage()
{
//    qDebug()<<"available:"<<clientSocket->bytesAvailable();
    QByteArray enciphedData;//存放接收的HDLC处理后的加密数据
    quint8 temp;//从套接字中每次读一个字节临时存放
    QDataStream in(clientSocket);
    //in.setVersion(QDataStream::Qt_4_6);
    if(nextBlockSize == 0)
    {
        if(clientSocket->bytesAvailable()< 1)
        {
            return;
        }
        in >> nextBlockSize;
    }
    qDebug()<<"available:"<<clientSocket->bytesAvailable()<<"current:"<<nextBlockSize;
    if(clientSocket->bytesAvailable() < nextBlockSize)
    {
        return;
    }
    for(int i=0;i<nextBlockSize;i++)
    {
        in >> temp;
        enciphedData.append(temp);
    }
    qDebug()<<"接收到HDLC处理后的数据包："<<enciphedData.toHex();
    //判断HDLC包头包尾是否出错
    if(enciphedData[0]==(char)0x7E&&enciphedData[nextBlockSize-1]==(char)0x7D)
    {
        for(int i=1;i<enciphedData.size()-1;i++)
        {
            if(enciphedData[i]==(char)0x7C)
            {
                enciphedData.remove(i,1);//删除原数据中增加的数据
                enciphedData[i]=enciphedData[i]^0x20;
            }
        }
        qDebug()<<"HDLC处理前的原始数据包："<<enciphedData.toHex();
        int checkSum=0;//校验和
        int num=enciphedData.size();//处理后数据的长度
        /* 注意QByteArray的at()方法返回的是有符号char类型,最好在执行加法运算之前
         * 转换成无符号char。不然如果发送方采用无符号类型计算校验和，而接收方采用
         * 有符号类型计算校验和，两者最终求的checkSum的原码值(int 类型)会不一样。但
         * 由于计算机采用补码进行加减运算，两者求的checkSum的补码是相同的，而我们
         * 的校验和最终取得是checkSum(补码)的低8位自然也是相同的。
         * 所以这里不转成quint8也不会出错，只是转换成无符号计算更容易理解
         */
        for(int j=1;j<num-2;j++)
        {
            checkSum+=(quint8)enciphedData.at(j);//对接收的数据计算校验和
        }
        //如果校验和正确，控制命令为索要数据命令，则回复实时数据
        //因为字节数组中的元素默认为一字节的有符号char，最高位为1时则为负值，而int型的校验和为正值，故需要转换成unsigned char
        if((quint8)(checkSum&0xff)==(quint8)enciphedData.at(num-2)&&enciphedData.at(2)==0x01)
        {
            qDebug()<<"回复实时数据：";
            sendMessage();
        }
        //如果校验和正确，控制命令为回复数据命令，显示接收的数据,模拟服务器对客户端的控制
        else if((quint8)(checkSum&0xff)==(quint8)enciphedData.at(num-2)&&enciphedData.at(2)==0x02)
        {
            qDebug()<<"显示接收的数据：";
            quint8 showVoltageH = (quint8)enciphedData.at(3);
            quint8 showVoltageL = (quint8)enciphedData.at(4);
            presetVoltage->setText(QString::number(((showVoltageH<<8)+showVoltageL)/10.0,'f',1));
            quint8 showCurrentH = (quint8)enciphedData.at(5);
            quint8 showCurrentL = (quint8)enciphedData.at(6);
            presetCurrent->setText(QString::number((showCurrentH<<8)+showCurrentL));

        }
        else
        {
            qDebug()<<"校验和不正确";
        }
    }
    else
    {
        qDebug()<<"HDLC包头包尾数据错误";
    }
    nextBlockSize = 0;//接收到完整的块后，将下一个未知块大小设置为0
}

/*
 *@brief:   打印错误状态
 *@author:  缪庆瑞
 *@date:    2016.9.14
 */
void Client::error()
{
    statusLabel->setText(clientSocket->errorString());
    clientSocket->close();
}

