#ifndef THREAD_FUTURE_OTHER_H
#define THREAD_FUTURE_OTHER_H

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <thread>
#include <future>


/***
     * 一、 future其他成员函数、shared_future、atomic
     * 1、 future其他成员函数：std::future_status status wait_for()
     * 2、 share_future （类模板）解决多个线程都像得到结果，他的get() 是复制
     * 3、atomic 原子操作：
     *      互斥量：多线程编程中，保护共享数据：先锁，操作共享数据，开锁
     *      有两个线程对一个变量进行操作atomVal共享变量，一个线程读、一个线程写，即使这样也会出问题
     *      原子操作不需要哦用到互斥量加锁（无锁）技术的多线程并发编程方式
     *      在多线程中，不会被打断的程序执行片段，效率比互斥量高，原子操作是不可分割的状态
     *      互斥量是针对一个代码段，往往是一个代码段，而原子操作一般对某个变量操作
     *      std::atomic 类模板，为了封装某个类型的值
     *      // 读线程
     *      int temVal = atomVal;
     *      //写线程
     *      atomVal = 6;
     *
     */

/**
 * future其他成员函数、shared_future、atomic
 */

namespace thread_future_other{
    using namespace std;

    void myThreadAtomicBool();

    void myThreadAtomic();
    int myThread();
    int myThread2(int temp);
    void myThread3(std::future<int>& tmpf);
    void myThread4(std::shared_future<int>& tmpf);

    //std::future_status 状态测试
    void test1() ;

    //share_future，（类模板）解决多个线程都像得到结果，他的get() 是复制可以get多次
    void test2() ;

    //原子操作：atomic
    void test3() ;
}




















/*
class Thread_future_other
{
public:
    Thread_future_other();
};
*/

#endif // THREAD_FUTURE_OTHER_H
