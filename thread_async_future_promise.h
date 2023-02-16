#ifndef THREAD_ASYNC_FUTURE_PROMISE_H
#define THREAD_ASYNC_FUTURE_PROMISE_H

/**
 * async、future、packaged_task、promise
 */

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <thread>
#include <future>

/****
     * 一、 async、future、packaged_task、promise
     * 1、希望线程返回一个结果：std::async,std::future
     *      std::async 是一个函数模板，启动一个异步任务 返回一个std::future（类模板） 对象
     *      std::async 就是自动创建一个线程，并开始执行相应的线程入口函数，
     *      返回值std::future对象里面就有线程入口函数所返回的结果（就是线程返回的结果）
     *      可以通过future对象的成员函数get(), 获取结果：std::future提供了一种访问异步操作结果的机制
     *      （这个结果你可能不能马上拿到，但是在不久的将来，线程函数执行完毕的时候，future对象会保存一个值）
     *      get()函数只能调用一次，不能连续调用多次，否则会有异常，而且应该使用get() 或者wait() 等待子线程结束
     *      如果没有get() 程序会等待子线程执行完 再退出，主线程会走到最后等待，不退出
     *      通过额外向std::async() 传递一个参数 参数类型 为std::launch类型（枚举类型）
     *      a) std::launch::deferred 表示线程入口函数调用被延迟到std::future的wait()或者get()函数调用才执行
     *          如果wait或者get没有被调用 线程会执行吗，答：不仅不会执行，线程实际没有创建
     *      b) std::launch::async 表示调用async函数的时候 就开始创建线程
     * 2、package_task 打包任务（类模板）
     *      模板参数 是可调用对象，通过package_task可以把各种可调用对象包装起来，方便将来作为线程入口函数
     *      package_task包装起来的可调用对象还可以直接调用，也就是说它本身也是可以调用
     *
     * 3、std::promise 类模板
     *      我们能够在某个线程中给它赋值，在其他线程中 把这个值取出来用,实现线程之间的通信
     */


using namespace std;

namespace Thread_sync_future_packaged_task_promise{
    class AClass{
    public:
        int myThread(int myVar){
            cout << myVar << endl;
            cout << "myThread start " << "thread id: " << std::this_thread::get_id() << endl;
            std::chrono::milliseconds dura(5000); // 休息 5 s
            std::this_thread::sleep_for(dura);
            cout << "myThread end " << "thread id: " << std::this_thread::get_id() << endl;
            return 4;
        }
    };

    int myThread1();

    int myThread2(int myVar);

    void myThread3(std::promise<int>& temp,int calc);

    void myThread4(std::future<int>& tmpf);

    //=========packaged_task 包装可调用对象=========
    //使用未来对象获取线程的返回值--使用std::async，std::future
    void test1();

    //使用类成员函数作为线程处理器
    void test2();

    // 表示延迟调用线程函数(使用std::launch::deferred) 直到get或者wait调用 后才真正执行线程
    void test3();

    void test4();

    //将线程函数 myThread2做一个外壳包装--然后启动线程
    void test5();

    //将lambda 做一个外壳包装线程函数 --然后启动线程, packaged_task 包装lambda 表达式
    void test6();

    //直接调用，函数调用
    void test7();

    //容器packaged_task
    void test8();

    //std::promise用法
    void test9();
}



/*
class thread_async_future_promise
{
public:
    thread_async_future_promise();
}; */

#endif // THREAD_ASYNC_FUTURE_PROMISE_H
