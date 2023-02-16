//
// Created by Administrator on 2023/1/14 0014.
//

#include "thread_data_sharing.h"

namespace ThreadShared_0 {
    void my_job(int num) {
        cout << "Thread Start, No." << num << " -----------  thread id:" << std::this_thread::get_id() << endl;
        std::cout << " -----------  thread id:" << std::this_thread::get_id() << " thread do something! " << std::endl;
        cout << "Thread End, No." << num << " -----------  thread id:" << std::this_thread::get_id() << endl;
    }
    //创建和等待多个线程
    void test() {
        vector<std::thread> threads;
        //创建10个线程，线程入口函数统一使用myprint
        for(int i = 0; i < 10; i ++) {
            threads.push_back(thread(my_job, i));
        }
        for(auto iter = threads.begin(); iter != threads.end(); iter++) {
            iter->join();
        }
    }
}

////仅仅存在读数据的情况
namespace ThreadShared_1 {
    std::vector <int>  gVecOnlyRead = {1,2,3,4,5};   //共享数据，只读
    void my_job(int num) {
        std::cout << "Thread Start, No." << num << " -----------  thread id:" << std::this_thread::get_id() << std::endl;
        std::cout << " thread id:" << std::this_thread::get_id() << " thread reading! " << gVecOnlyRead[0] << gVecOnlyRead[1]
                << gVecOnlyRead[2] << gVecOnlyRead[3] << gVecOnlyRead[4] << std::endl;
        std::cout << "Thread End, No." << num << " -----------  thread id:" << std::this_thread::get_id() << std::endl;
    }
    //创建和等待多个线程
    void test() {
        std::vector<std::thread> threads;
        //创建10个线程，线程入口函数统一使用myprint
        for(int i = 0; i < 10; i ++) {
            threads.push_back(std::thread(my_job, i));
        }
        for(auto iter = threads.begin(); iter != threads.end(); iter++) {
            iter->join();
        }
    }
}

namespace ThreadShared_2 {
    //处理: 读写不能同时进行, 下面的测试函数实际是不稳定的，因为又读又写，但是没有进行互斥量保护，这里仅仅是演示问题。
    void test(){
        AClass myobj;
        std::thread myOutMsgObj(&AClass::outMsgRecQueue, &myobj);           ////启动一个线程, 以myobj对象，AClass成员函数outMsgRecQueue作为线程处理器
        std::thread myInMsgObj(&AClass::inMsgRecQueue, &myobj);             ////启动一个线程, 以myobj对象，AClass成员函数inMsgRecQueue作为线程处理器

        myOutMsgObj.join();
        myInMsgObj.join();

    }
}