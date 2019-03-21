/*
 *@file:   client.h
 *@author: 缪庆瑞
 *@date:   2016.9.14
 *@brief:  Client的头文件
 */
#ifndef CLIENT_H
#define CLIENT_H

#include <QWidget>
#include <QtNetwork>
#include <QDataStream>
#include <QTime>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>

class QTcpSocket;

class Client : public QWidget
{
    Q_OBJECT

public:
    explicit Client(QWidget *parent = 0);
    ~Client();
    void initUi();

public slots:
    void createConnectToServer();//连接到服务器
    void disconnectToServer();//取消连接到服务器
    void refreshSlot();//刷新随机数据范围
    void showConnected();
    void showDisconnected();
    void sendMessage();
    void sendCurrentTime();
    void receiveMessage();
    void error();

private:
    QTcpSocket *clientSocket;
    quint8 nextBlockSize;//即将接收的数据块大小

    QPushButton *timeCorrectBtn;//校准时间
    QPushButton *refreshBtn;//刷新按钮
    QPushButton *connectBtn;//连接按钮
    QPushButton *disconnectBtn;//取消链接
    QLabel *statusLabel;//显示连接状态
    QLineEdit *ipAddressInputC;//输入连接的设备地址（模拟客户端地址）
    QLineEdit *ipAddressInputS;//输入连接的服务器地址

    QLabel *actualVoltage;//电压
    QLabel *presetVoltage;
    QLineEdit *voltageInputL;//模拟产生的最小电压
    QLineEdit *voltageInputH;//模拟产生的最大电压
    QLabel *actualCurrent;//电流
    QLabel *presetCurrent;
    QLineEdit *currentInputL;//模拟产生的最小电流
    QLineEdit *currentInputH;//模拟产生的最大电流

    uint voltageL;
    uint voltageH;
    uint currentL;
    uint currentH;

    quint8 ipSegment[4];//存储设备(客户端)ip的4个段
};

#endif // CLIENT_H
