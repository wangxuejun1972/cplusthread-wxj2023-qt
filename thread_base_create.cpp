//
// Created by Administrator on 2023/1/14.
//

#include "thread_base_create.h"

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
    void my_job00() {
        std::cout << "my thread Start! " << std::endl;
        std::cout << "thread do something! " << std::endl;
        std::cout << "my thread End!" << std::endl;
    }
    //阻塞方式启动子线程
    void test00() {
        //1. 使用函数--启动线程
        std::thread mythread(my_job00);   									// 被定义之后，线程立即开始执行
        if( mythread.joinable() )											// 判断是否可以加入
            mythread.join();   											// 线程对象进行join后，则需要等待该线程执行完毕后，方可继续执行主线程。
        std::cout << "main thread continue excute !" << std::endl;			// 其输出展示顺序应该在子线程函数中输出后面展示。
    }
    //启动线程后以分离方式运行
    void test01() {
        //1. 使用函数--启动线程
        thread mythread(my_job00);    									    // 被定义之后，线程立即开始执行
        mythread.detach();   					                            // 线程对象进行detach 分离后，主线程不需要等待该线程执行完毕，主线程可并行于子线程执行， 主线程可能先退出。
        cout << "main thread continue excute !" << endl;					// 其输出展示顺序可能在子线程输出内容之前或之间或之后，顺序不定，因为主线程子线程为并发方式运行。
    }
}

/*
    可运行对象类 -- 仿函数 --- 其他的线程启动方式
 */
namespace ThreadCreateBase_1 {
    using namespace std ;
    //2. 使用仿函数-可运行对象
    void test() {
        runtask _runtask(0);
        thread mythread(_runtask);  		//这里不能使用临时对象
        mythread.detach();
        //mythread2.join();
    }
}

/*
 * 使用lambda表达式--作为线程处理器函数
 */
namespace ThreadCreateBase_2{
    using namespace std ;
    void test () {
        //lamthread是通过lambda表达式自动推导出来的一个函数指针名称，该函数指针可作为线程的运行处理器
        auto lamthread = [] {
            cout << "my thread Start" << endl;
            cout << "my thread do something ..." << endl;
            cout << "my thread End" << endl;
        };
        thread mythread3(lamthread);
        mythread3.join();
        //mythread3.detach();
    }
}

