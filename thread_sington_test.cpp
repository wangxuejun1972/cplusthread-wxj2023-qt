//
// Created by Administrator on 2023/1/15 0015.
//

#include "thread_sington_test.h"



namespace Sington_X{
    using namespace std;
    // 单线程情况下
    namespace edition1 {
        MyCAS* MyCAS::m_instance = nullptr;
        void test() {
            Sington_X::edition1::MyCAS* a = Sington_X::edition1::MyCAS::GetInstance();
            Sington_X::edition1::MyCAS* b = Sington_X::edition1::MyCAS::GetInstance();
            a->func();
            b->func();
        }
    }
    // 加锁版本
    namespace edition2{
        MyCAS* MyCAS::m_instance = nullptr;
        std::mutex MyCAS::resource_mutex;
        void test() {
            std::thread myObj1(Sington_X::myThread);
            std::thread myObj2(Sington_X::myThread);
            myObj1.join();
            myObj2.join();
        }
    }

    namespace callOnce{
        MyCAS* MyCAS::m_instance = nullptr;
        std::once_flag MyCAS::g_flag; // 系统定义的标记

        void test() {
            // call_once
            std::thread myObj1(Sington_X::myThread2);
            std::thread myObj2(Sington_X::myThread2);
            myObj1.join();
            myObj2.join();
        }
    }

    // 线程入口函数
    void myThread(){
        std::cout << "thread id: " << std::this_thread::get_id() << " ------ my thread start excuting" << std::endl;
        edition2::MyCAS::GetInstance(); // 这里可能有问题
        std::cout << "thread id: " << std::this_thread::get_id() << " ------ my thread excute finish" << std::endl;
        return;
    }
    void myThread2(){
        std::cout << "thread id: " << std::this_thread::get_id() << " ------ my thread start excuting" << std::endl;
        callOnce::MyCAS::GetInstance(); // 这里可能有问题
        std::cout << "thread id: " << std::this_thread::get_id() << " ------ my thread excute finish" << std::endl;
        return;
    }

}
