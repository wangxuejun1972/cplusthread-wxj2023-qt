#include <QCoreApplication>

/*
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    return a.exec();
}
*/

#include <iostream>

#include "thread_base_create.h"
#include "thread_transfer_parameters.h"
#include "thread_data_sharing.h"
#include "thread_mutex_test.h"
#include "thread_condition_variable_test.h"
#include "thread_async_future_promise.h"
#include "thread_future_other.h"


int main(int argc, char *argv[]) {

    QCoreApplication a(argc, argv);
    std::cout << "Main Thread start , main thread id: " << std::this_thread::get_id() << std::endl;

    //线程的基本方式创建与运行--开始**************************************************************************************************
/*
    ThreadCreateBase_0::test00();		//启动子线程后阻塞方式运行

    ThreadCreateBase_0::test01();		//启动线程后以分离方式运行

    ThreadCreateBase_1::test();			//使用仿函数-可运行对象-启动子线程后分离方式运行

    ThreadCreateBase_2::test();			//启动子线程后阻塞方式运行，使用lambda表达式--作为线程处理器函数
*/
    //线程的基本方式创建与运行--结束==================================================================================================


    //线程有参处理器，线程传参详解、detach注意事项，成员函数做线程函数处理器--开始***********************************************************
/*
    ThreadCreate_HasParams_0::test1();    //有参线程处理器的启动

    ThreadCreate_HasParams_0::test2();    //有参线程处理器的启动

    ThreadCreate_HasParams_1::test();     //使用类作为线程参数

    ThreadCreate_HasParams_2::test1();     ////直接引用mvar变量可能存在在分离线程时会存在 非法内存引用

    ThreadCreate_HasParams_2::test2();     ////使用临时类对象 --事实上这里会发生一次拷贝构造，临时类对象

    ThreadCreate_HasParams_2::test3();     ////使用std::ref可以将参数引用传递到线程地址空间，语法无法通过 ////使用类作为线程参数
*/
    //线程有参处理器，线程传参详解、detach注意事项，成员函数做线程函数处理器--结束===========================================================


    //线程有参处理器，使用智能指针--开始    ******************************************************************************************

    //ThreadCreate_HasParams_3::test() ;

    //线程有参处理器，使用智能指针--结束     ===========================================================++++++++++++++++++++++++++++++


    //使用类成员函数作为线程函数--开始    ******************************************************************************************

    //ThreadCreate_HasParams_4::test1() ;     ////该方式使用类成员函数作为线程处理器函数，同时传入该类对象(这里进行了拷贝构造)，在传入线程成员函数的参数，该种方式线程id等同测试函数调用处

    //ThreadCreate_HasParams_4::test2() ;     ////该函数中使用 std::ref直接引用对象地址，因此只能使用join,不能使用detach，因为涉及到内存异常引用（被引用的对象被释放了，因为直接引用为拷贝）

    //ThreadCreate_HasParams_4::test3() ;     //// 直接使用引用, (考虑join。不能使用detach,没有拷贝对象，不安全)， &方式等价std::ref

    //ThreadCreate_HasParams_4::test4() ;     //// 使用仿函数 ,传参时未引用，因此能使用detach

    //ThreadCreate_HasParams_4::test5() ;     //// 使用仿函数 ,传参时使用std::ref引用，因此不能使用detach（不安全）

    //使用类成员函数作为线程函数--结束     ===========================================================++++++++++++++++++++++++++++++


    //多线程数据共享--开始    ***************************************************************************************************

    //ThreadShared_0::test() ;                //// 创建和等待多个线程 ，开启10个线程

    //ThreadShared_1::test() ;                //// 仅仅存在读数据的情况,开启10个线程，读取公共资源队列，

    //ThreadShared_2::test() ;                  //// 共享资源队列在一个类中，开启一个写数据线程，一个读数据线程，但是在同一个类中未加互斥所保护

    //多线程数据共享--结束    ===========================================================++++++++++++++++++++++++++++++++++++++++

    //多线程数据共享互斥版本--开始    ***************************************************************************************************

    //ThreadSharedMutex_1::test() ;          //clion中加上互斥保护的线程运行似乎存在问题，经常性的连续执行某个线程，qt中没太大问题

    //ThreadSharedMutex_Die_2::test() ;          ////死锁的演示

    //ThreadSharedMutex_SLockOK::test() ;          ////单锁解决共享资源读写演示

    //ThreadSharedMutex_MLockOK::test() ;          ////多锁的解决共享资源读写演示

    //ThreadShared_unique_lock::test() ;          ////解决共享资源读写unique_lock的演示

    //多线程数据共享互斥版本--结束    ===========================================================++++++++++++++++++++++++++++++++++++++++

    //多线程数据共享互斥优化--开始    ***************************************************************************************************

    //ThreadShared_unique_lock_2c_optimize::test();                   //使用双重检查，休眠方式优化，不推荐

    //ThreadShared_unique_lock_condi_optimize::test();                //使用条件变量进行优化，推荐这种性能更优的方式 -- 通知使用notify_one()

    ThreadShared_unique_lock_condi_all_optimize::test();            //条件变量，通知使用notify_all()
    //多线程数据共享互斥优化--结束    ===========================================================++++++++++++++++++++++++++++++++++++++++


    //多线程数据共享互斥优化--开始    ***************************************************************************************************

    //Thread_sync_future_packaged_task_promise::test1();

    //Thread_sync_future_packaged_task_promise::test2();

    //Thread_sync_future_packaged_task_promise::test3();

    //Thread_sync_future_packaged_task_promise::test4();

    //Thread_sync_future_packaged_task_promise::test5();              //使用std::packaged 将线程函数包装一下，然后用这个包装对象启动线程

    //Thread_sync_future_packaged_task_promise::test6();               //

    //Thread_sync_future_packaged_task_promise::test7();               //

    //Thread_sync_future_packaged_task_promise::test8();               //容器packaged_task

    //Thread_sync_future_packaged_task_promise::test9();               //

    //多线程数据共享互斥优化--结束    ===========================================================++++++++++++++++++++++++++++++++++++++++

    //future others--开始    ***************************************************************************************************

    //thread_future_other::test1();

    //thread_future_other::test2();

    //thread_future_other::test3();


    //future others--结束    ===========================================================++++++++++++++++++++++++++++++++++++++++


    return a.exec();
    //std::cout << "Main Thread End!" << std::endl;

    return 0;
}
