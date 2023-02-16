//
// Created by Administrator on 2023/1/14 0014.
//

#ifndef CPLUSTHREAD_WXJ2023_THREAD_DATA_SHARING_H
#define CPLUSTHREAD_WXJ2023_THREAD_DATA_SHARING_H

#include <iostream>
#include <vector>
#include <thread>
#include <list>

/***
    chp3. 创建多个线程，数据共享问题分析

    1. 创建和等待多个线程
        多个线程执行顺序是乱的，跟操作系统内部对线程的运行调度机制有关
        将线程放进容器中进行管理
    2. 数据共享问题分析
        2.1  只读的数据
            不需要加锁，操作无异常
        2.2  有读有写
            处理: 读写不能同时进行
        2.3  其他
    3. 共享数据的保护
        简化网络游戏服务器。两个自己创建的线程:
                一个线程收集玩家命令(用数字代表)，并把命令数据写到一个队列中。
                一个线程从这个队列中取出玩家发出的命令,解析执行动作。

        解决办法: 互斥量
*/

namespace ThreadShared_0{
    using namespace std;
    //线程函数
    void my_job(int num);
    // 创建和等待多个线程
    void test();
}

////仅仅存在读数据的情况，无需考虑加锁问题
namespace ThreadShared_1 {
    void my_job(int num);
    //创建和等待多个线程，直接进行读数据的运行
    void test();
}

//处理: 读写不能同时进行，如果存在读写则需要对共享数据进行保护
namespace ThreadShared_2 {
    using namespace std;
    class AClass {
    public:
        //处理: 读写不能同时进行, 下面的测试函数实际是不稳定的，因为又读又写，但是没有进行互斥量保护，这里仅仅是演示问题。
        //将收到的消息(玩家命令)，收到一个队列中
        void inMsgRecQueue() {
            for (int j = 0; j < 10000; ++j) {
                cout << "inMsgRecQueue Start, insert Msg: " << j << endl;
                msgRecvQueue.push_back(j);
            }
        }
        //处理: 读写不能同时进行, 下面的测试函数实际是不稳定的，因为又读又写，但是没有进行互斥量保护，这里仅仅是演示问题。
        //将数据从消息队列中取出
        void outMsgRecQueue() {
            for (int i = 0; i < 10000; i++) {
                if (!msgRecvQueue.empty()) {
                    //消息不为空
                    int command = msgRecvQueue.front();
                    cout << "outMsgRecQueue Start, pop  Msg: " << command << endl;
                    msgRecvQueue.pop_front();
                } else {
                    //消息为空
                    cout << "Queue Empty Now!" << endl;
                }
            }
            cout << "OUTQueue End" << endl;
        }

    private:
        std::list<int> msgRecvQueue;   //命令队列
    };

    void test();
}








/*
class thread_data_sharing {
};*/

#endif //CPLUSTHREAD_WXJ2023_THREAD_DATA_SHARING_H
