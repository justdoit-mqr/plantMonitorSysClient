# plantMonitorSysClient
这是个人本科毕业设计作品的客户端程序,模拟一台车间设备,与plantMonitorSys服务器程序进行通信。
## 功能概述:
该客户端程序采用Tcp socket与Tcp server进行通信，因为当时毕设演示条件的局限性，无法在多台不同ip地址的设备上运行程序来模拟多台设备。所以设备的ip地址全由该程序的通信数据模拟，而不是使用真实的客户机ip，这样便可以在同一台机器上运行多个客户端程序，模拟多个ip地址的设备，但实际上这些设备都共用一个真实的客户机ip。  
另外客户端与服务器之间数据交互由服务器做控制，客户端连接服务器后并不会主动发送数据，而是服务器主动的发送一个请求数据的命令包，客户端接收到请求包解析无误后向服务器回复实时(模拟)数据。  
## 运行截图:
![client1.png](./screenshots/client1.png "client1.png")  
![client2.png](./screenshots/client2.png "client2.png")  
![client3.png](./screenshots/client3.png "client3.png")  
## 作者联系方式
**邮箱:justdoit_mqr@163.com**  
**新浪微博:@为-何-而来**  
