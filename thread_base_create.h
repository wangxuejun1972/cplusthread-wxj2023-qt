//
// Created by Administrator on 2023/1/14.
//

#ifndef CPLUSTHREAD_WXJ2023_THREAD_BASE_CREATE_H
#define CPLUSTHREAD_WXJ2023_THREAD_BASE_CREATE_H

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <thread>

/*
   线程启动，结束，创建线程多种方法
   1. 演示线程运行的开始和结束
      创建线程对象(子线程开始执行):  thread  my(_addr);  //其中_addr表示可调用对象(函数，仿函数)
      线程运行(阻塞主线程，等待子线程执行完毕):  my.join();  //main进程(主线程--->my线程---->主线程)
      线程运行(主线程和子线程各自运行,子线程相等于被运行时库(CRT)接管,守护进程): my.detach();
      判断是否可以成功使用join()或者detach(): joinable();
*/
namespace ThreadCreateBase_0{
    using namespace std ;
    //无参线程函数
    void my_job00() ;
    //阻塞方式启动子线程
    void test00() ;
    //启动线程后以分离方式运行
    void test01() ;
}

/*
    可运行对象类 -- 仿函数 --- 其他的线程启动方式
 */
namespace ThreadCreateBase_1 {
    using namespace std ;
    //可运行对象类 -- 仿函数
    class runtask{
    public:
        runtask() {}													//默认构造函数
        runtask(int _mem) : mem(_mem) {
            cout << this->mem << "ctor start" << endl;
        }

        runtask(const runtask& _th) : mem(_th.mem + 1) {			    //拷贝构造函数
            cout << this->mem << "copy ctor start" << endl;
        }

        ~runtask() {													//析构函数
            cout << this->mem << "dtor start" << endl ;
        }

        //添加了该运行接口，则为可运行对象，此对象就可以作为参数传递到c++线程（std::thread）构造中启动线程。，该接口相当于线程处理器。
        void operator()() {
            cout << "my thread Start!" << endl;
            cout << "my thread do something ..." << endl;
            cout << "my thread End!" << endl;
        }
    private:
        int mem = 0;
    };

    //2. 使用仿函数-可运行对象
    void test();
}

/*
 * 使用lambda表达式--作为线程处理器函数
 */
namespace ThreadCreateBase_2{
    using namespace std ;
    void test () ;
}



/*
class thread_base_create {

};
 */
#endif //CPLUSTHREAD_WXJ2023_THREAD_BASE_CREATE_H
