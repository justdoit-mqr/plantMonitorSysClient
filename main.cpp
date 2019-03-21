/*
 *@file:   main.cpp
 *@author: 缪庆瑞
 *@date:   2016.9.14
 *@brief:  主程序入口
 */
#include "client.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Client w;
    w.show();

    return a.exec();
}
