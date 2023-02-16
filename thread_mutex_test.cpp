//
// Created by Administrator on 2023/1/15 0015.
//

#include "thread_mutex_test.h"

//处理: 读写不能同时进行，如果存在读写则需要对共享数据进行保护
namespace ThreadSharedMutex_1 {
    using namespace std;

    void test() {
        AClass myobj;
        std::thread myOutMsgObj(&AClass::outMsgRecQueue, &myobj);           ////启动一个线程, 以myobj对象，AClass成员函数outMsgRecQueue作为线程处理器
        std::thread myInMsgObj(&AClass::inMsgRecQueue, &myobj);             ////启动一个线程, 以myobj对象，AClass成员函数inMsgRecQueue作为线程处理器

        myOutMsgObj.join();
        myInMsgObj.join();
    }
}

//死锁的演示
namespace ThreadSharedMutex_Die_2 {
    void test() {
        Game mygame;
        thread in(&Game::inMsgRecQueue, &mygame);
        thread out(&Game::outMsgRecQueue, &mygame);
        in.join();
        out.join();
    }
}

//单锁解决死锁 --单锁方式
namespace ThreadSharedMutex_SLockOK{
    void test() {
        ThreadSharedMutex_SLockOK::AClass myObja;
        thread myOutMsgObj(&ThreadSharedMutex_SLockOK::AClass::outMsgRecvQueue,&myObja); // 第二个参数是引用 才可以保证线程里调用的是一个对象
        thread myInMsgObj(&ThreadSharedMutex_SLockOK::AClass::inMsgRecvQueue,&myObja);
        myOutMsgObj.join();
        myInMsgObj.join();
    }
}

// 解决死锁问题 -- 多锁方式
namespace ThreadSharedMutex_MLockOK{

    void test() {
        ThreadSharedMutex_MLockOK::AClass myObja;
        thread myOutMsgObj(&ThreadSharedMutex_MLockOK::AClass::outMsgRecvQueue,&myObja); // 第二个参数是引用 才可以保证线程里调用的是一个对象
        thread myInMsgObj(&ThreadSharedMutex_MLockOK::AClass::inMsgRecvQueue,&myObja);
        myOutMsgObj.join();
        myInMsgObj.join();
    }
}

namespace ThreadShared_unique_lock{

    void test() {
        ThreadShared_unique_lock::AClass myObja;
        thread myOutMsgObj(&ThreadShared_unique_lock::AClass::outMsgRecvQueue,&myObja); // 第二个参数是引用 才可以保证线程里调用的是一个对象
        thread myInMsgObj(&ThreadShared_unique_lock::AClass::inMsgRecvQueue,&myObja);
        myOutMsgObj.join();
        myInMsgObj.join();
    }
}

