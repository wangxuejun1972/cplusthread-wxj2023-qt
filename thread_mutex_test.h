//
// Created by Administrator on 2023/1/15 0015.
//

#ifndef CPLUSTHREAD_WXJ2023_THREAD_MUTEX_TEST_H
#define CPLUSTHREAD_WXJ2023_THREAD_MUTEX_TEST_H

#include <iostream>
#include <algorithm>
#include <thread>
#include <vector>
#include <list>
#include <mutex>


using namespace std;

/**
    chp4. 互斥量概念，死锁
    1. 互斥量mutex的基本概念
        一个类对象，一把锁; 多个线程尝试用lock()成员函数来加锁
        只有一个线程可以锁定成功，成功的标志是lock()返回,如果没有成功会阻塞
    2. 互斥量的用法
        2.1 lock() unlock()
            lock,操作临界区，unlock
        2.2  std::lock_guard()类模板
            只能匹配unlock, 不可以搭配unlock使用
            std::lock_guard<std::mutext> m(my_mutex);
            原理: 在其构造函数中执行lock(), 在其析构函数中执行unlock();
            不足: 析构函数的作用域外才可以解锁，所以不是很难灵活.可以用大括号{}来灵活控制
            作用域生命周期
    3. 死锁
        3.1  死锁演示
                必要条件是至少两把锁: Lock_a, Lock_b
                两个线程： thread_a, thread_b
            线程thread_a执行的时候，这个线程先锁Lock_a成功，然后去锁Lock_b
            出现上下文切换，thread_b开始执行，先锁Lock_b成功，然后去锁Lock_a
            死锁发生。
        3.2  死锁的解决办法
            保证两个互斥量上锁的顺序一样，便不会死锁
        3.3  std::lock()函数模板
            一次至少锁住两个或者两个以上的互斥量
            不存在因为在多个线程中因为加锁顺序问题而导致的死锁问题。
            ***最后需要手动使用unlock解锁
        3.4  std::lock_guard的std::adopt_lock参数
        std::adopt_lock_t 一个结构体对象
            解决了std::lock()需要手动解锁的问题
            std::lock_guard(my_mutex_a, std::adopt_lock_t);
            std::lock_guard(my_mutex_a, std::adopt_lock_t);

*/

//处理: 读写不能同时进行，如果存在读写则需要对共享数据进行保护
namespace ThreadSharedMutex_1 {
    using namespace std;
    class AClass {
    public:
        //处理: 读写不能同时进行, 下面的测试函数实际是不稳定的，因为又读又写，但是没有进行互斥量保护，这里仅仅是演示问题。
        //将收到的消息(玩家命令)，收到一个队列中
        void inMsgRecQueue() {
            for (int j = 0; j < 10000; ++j) {
                cout << " >>>>>>>>> inMsgRecQueue Start, insert Msg: " <<  "  ------ j = " << j << endl;
                my_mutex.lock();
                msgRecvQueue.push_back(j);
                my_mutex.unlock();
            }
        }
        bool outMsgLULPProc(int &command) {
            my_mutex.lock();
            if (!msgRecvQueue.empty()) {
                command = msgRecvQueue.front();         //// 返回队列中第一个元素，但不检查元素是否存在
                msgRecvQueue.pop_front();                   //// 移除第一个元素，但是不返回
                my_mutex.unlock();
                return true;
            }
            my_mutex.unlock();
            return false;
        }
        //处理: 读写不能同时进行, 因此需要加锁解锁
        //将数据从消息队列中取出
        void outMsgRecQueue() {
            int command = 0;
            for (int i = 0; i < 10000; i++) {
                if (outMsgLULPProc(command)) {
                    cout << " ******** outMsgRecQueue Start, pop  Msg: " << command <<  "------ i = " << i << endl ;
                }else {
                    //消息为空
                    cout << " ******** Queue Empty Now! ------ i = " << i <<  endl;
                }
            }
            cout << "OUTQueue End" << endl;
        }

    private:
        std::list<int> msgRecvQueue;   //命令队列
        std::mutex     my_mutex;
    };

    void test();
}

////死锁的演示 -- 至少两把锁: Lock_a, Lock_b 两个线程： thread_a, thread_b
namespace ThreadSharedMutex_Die_2 {
    using namespace std;
    class Game{
    public:
        void inMsgRecQueue() {
            for(int i = 0; i < 10000; i ++) {
                //std::lock_guard<std::mutex> myguard(my_mutex);
                //std::lock(my_mutex_a, my_mutex_b);
                my_mutex_a.lock();   //加锁
                //....
                my_mutex_b.lock();
                cout << "inMsgRecQueue Start-----Insert Msg:" << i << endl;
                MsgRecQueue.push_back(i);
                my_mutex_b.unlock();
                my_mutex_a.unlock();
            }
            return;
        }

        bool outMsgLULProc() {

            //std::lock_guard<std::mutex>  myguard(my_mutex);
            //std::lock(my_mutex_a, my_mutex_b);
            my_mutex_b.lock();
            my_mutex_a.lock();
            if(!MsgRecQueue.empty()) {

                cout << "OutMsgRecQueue Start-----Pop Msg:"  << MsgRecQueue.front() << endl;
                MsgRecQueue.pop_front();
                //数据处理
                my_mutex_a.unlock();  //一定要注意是在返回值之前解锁
                my_mutex_b.unlock();
                return true;
            }else{
                my_mutex_a.unlock();
                my_mutex_b.unlock();
                return false;
            }
        }

        void outMsgRecQueue() {
            for(int i = 0; i < 10000; i ++) {
                bool result = outMsgLULProc();
                if(!result){
                    cout << "Queue Empty Now!" << endl;
                }
            }
            return;
        }
    private:
        list<int> MsgRecQueue;   //共享数据
        std::mutex my_mutex_a;   //互斥量
        std::mutex my_mutex_b;
    };

    void test();
}

//单锁解决共享读写 --单锁方式演示
namespace ThreadSharedMutex_SLockOK{
    class AClass{
    public:
        // 把收到的消息(玩家命令)放入到一个队列
        void inMsgRecvQueue(){
            for(int i = 0; i< 2000; ++i){
                cout << "inMsgRecvQueue() excute insert elem " << i << "  cur i = " << i  << endl;
                /**
                 * 加锁解锁要保护的数据
                 */
                //my_mutex.lock();
                {
                    // scope
                    lock_guard<std::mutex> sbGuard(my_mutex);
                    msgRecvQueue.push_back(i);
                }
                //my_mutex.unlock();

            }
        }

        bool outMsgLULProc(int& command){
            //my_mutex.lock();
            std::lock_guard<std::mutex> sbGuard(my_mutex); //用了lock_guard 就不能用unlock和lock了
            if(!msgRecvQueue.empty()){
                // 消息队列不为空
                command = msgRecvQueue.front();// 返回第一个元素，不检查元素是否存在
                msgRecvQueue.pop_front();
                //my_mutex.unlock();  // 这个分支也要有unlock(),否则有可能会一直锁住
                return true;
            }
            //my_mutex.unlock();
            return false;
        }
        // 把数据从消息队列中取出的线程
        void outMsgRecvQueue(){
            int command = 0;
            for(int i = 0; i< 2000; ++i){
                bool result = outMsgLULProc(command);
                if(result){
                    cout << "outMsgRecvQueue() excute get one elem " << command << "  cur i = " << i << endl;
                }
                else cout << "outMsgRecvQueue() excute ,but get msg is empty , cur i = " << i << endl;
            }
            cout << "end " << endl;
        }
    private:
        std::list<int> msgRecvQueue;
        std::mutex my_mutex;
        std::mutex my_mutex1;
        std::mutex my_mutex2;
    };

    void test() ;
}

// 解决死锁问题---多锁方式演示
namespace ThreadSharedMutex_MLockOK{
    class AClass{
    public:
        // 把收到的消息(玩家命令)放入到一个队列
        void inMsgRecvQueue(){
            for(int i = 0; i< 2000; ++i){
                cout << "inMsgRecvQueue() excute insert elem " << i << "  cur i = " << i  << endl;
                /**
                 * 加锁解锁要保护的数据
                 */
                std::lock(my_mutex1,my_mutex2); // 解决死锁问题
                std::lock_guard<std::mutex> sbGuard1(my_mutex1,std::adopt_lock);
                std::lock_guard<std::mutex> sbGuard2(my_mutex2,std::adopt_lock);
                //my_mutex1.lock();
                //my_mutex2.lock();

                msgRecvQueue.push_back(i);
                //my_mutex2.unlock();
                //my_mutex1.unlock();
            }
        }

        bool outMsgLULProc(int& command){
            //my_mutex2.lock();
            //my_mutex1.lock();
            std::lock(my_mutex1,my_mutex2); // 解决死锁问题
            // 参数std::adopt_lock 表示不用再lock了
            std::lock_guard<std::mutex> sbGuard1(my_mutex1,std::adopt_lock);
            std::lock_guard<std::mutex> sbGuard2(my_mutex2,std::adopt_lock);

            if(!msgRecvQueue.empty()){
                // 消息队列不为空
                command = msgRecvQueue.front();// 返回第一个元素，不检查元素是否存在
                msgRecvQueue.pop_front();
                //my_mutex1.unlock();
                //my_mutex2.unlock();
                return true;
            }
            //my_mutex1.unlock();
            //my_mutex2.unlock();
            return false;

        }
        // 把数据从消息队列中取出的线程
        void outMsgRecvQueue(){
            int command = 0;
            for(int i = 0; i< 2000; ++i){
                bool result = outMsgLULProc(command);
                if(result){
                    cout << "outMsgRecvQueue() excute get one elem " << command << "  cur i = " << i << endl;
                }
                else {
                    cout << "outMsgRecvQueue() excute ,but get msg is empty , cur i = " << i << endl;
                }
            }
            cout << "end " << endl;
        }
    private:
        std::list<int> msgRecvQueue;
        std::mutex my_mutex;
        std::mutex my_mutex1;
        std::mutex my_mutex2;
    };

    void test();
}

/****
 *  unique_lock 详解
     * 对应代码空间 ThreadShared_unique_lock
     * 1、 unique_lock 取代lock_guard
     *    unique_lock 是一个类模板 工作中一般lock_guard(推荐使用)
     *    unique_lock 更灵活：效率上差一点、内存占用多一点，问题不是很大
     *
     *
     *
     * 2、unique_lock 第二参数
     *     unique_lock 还支持一些lock_guard 不支持的标志
     *     1、std::adopt_lock : 表示这个互斥量已经被lock了（你必须提前lock，否则报异常），通知lock
     *     2、std::try_to_lock 一个线程卡住20s 另一个线程也会卡住，不灵活：unique_lock 第二个参数 try_to_lock 解决这个问题
     *          我们会尝试用mutex的lock(),去锁住这个mutex 但如果没有锁定成功，我也会立即返回、并不会阻塞在那里
     *          用这个try_to_lcok  之前不能用lock 否则会锁住两次
                unique_lock<std::mutex> sbGuard1(my_mutex,std::try_to_lock); //使用这个第二参数 不能再之前lock 否则会卡住
                 // 如果拿到锁的话
                if(sbGuard1.owns_lock()){
                    cout << "inMsgRecvQueue 执行，拿到锁头..." << endl;
                    msgRecvQueue.push_back(i);
                }
                else{
                    cout << "本线程没有拿到锁头..." << endl;
                }
     *
     *     3、std::defer_lock 第二参数 ，随时加锁，随时解锁
     *          前提：不能先lock(),否则会异常,defer_lock 表示没有给mutex加锁、初始化了一个没有加锁的 mutex，这样就可以调用
     *          unique_lock 的重要成员函数
                unique_lock<std::mutex> sbGuard1(my_mutex,std::defer_lock); //这里实际没有加锁
                sbGuard1.lock();    //真实加锁，但是后面就不用自己写解锁了
                保护段数据代码
                sbGuard1.unlock();
                //处理一些非保护段代码
                sbGuard1.lock();        //重新加锁
                msgRecvQueue.push_back(i);      //被保护代码
     * 3、成员函数
     *      1、lock()
     *      2、unlock()  有一些非共享代码要处理 可以先unlock() 然后再lock()
                unique_lock<std::mutex> sbGuard1(my_mutex,std::defer_lock); //这里实际没有加锁
                sbGuard1.lock();    //真实加锁，但是后面就不用自己写解锁了
                保护段数据代码
                sbGuard1.unlock();  //解锁
                //处理一些非保护段代码
                sbGuard1.lock();        //重新加锁
                msgRecvQueue.push_back(i);      //被保护代码
     *      3、try_lock() 如果拿不到锁 返回false 拿到了 返回true  和defer_lock 搭配使用
                unique_lock<std::mutex> sbGuard1(my_mutex,std::defer_lock); //这里实际没有加锁
                if (sbGuard1.try_lock()==true) {
                    //拿到锁
                    msgRecvQueue.push_back(i);      //被保护代码
                }else {
                    //没有拿到锁
                }

     *      4、realease() 返回它所管理的mutex指针 并释放所有权 这个unique_lock 和mutex 不再有关系
     *          这个和unlock() 不一样，放弃unique_lock 和 mutex的绑定关系，之后就要自己管理锁
     *          如果之前是加锁 你需要手动解锁
     *          返回值 是原本的互斥量的指针
                unique_lock<std::mutex> sbGuard1(my_mutex,std::defer_lock); //这里实际没有加锁，但是建立了sbGuard1与my_mutex的绑定
                std::mutex *ptx = sbGuard1.release();
                msgRecvQueue.push_back(i);
                // 这里必须手动解除
                ptx->unlock();
     * 4、unique_lock 所有权的传递
     *      1、一个mutex 应该只和一个unique_lock 绑定在一起，这个时候unique_lock 就拥有 一个mutex的所有权
     *          可以转移给其他的unique_lock 对象
     *          unique_lock对象这个mutex的所有权不能复制 mutex也没有复制构造函数
     *         移动语义 这个时候就直接把所有权转移到了sbGuard2了
     *        unique_lock<std::mutex> sbGuard1(my_mutex);
              unique_lock<std::mutex> sbGuard2(std::move(sbGuard1));
           2、也可以作为返回值 ：
                unique_lock<std::mutex> sbGuard2 = rtn_unique_lock();
     *
     */
namespace ThreadShared_unique_lock{
    class AClass{
    public:
        std::unique_lock<std::mutex> rtn_unique_lock(){
            std::unique_lock<std::mutex> tmpGuard(my_mutex);
            return tmpGuard; // 返回临时对象，调用unique_lock 移动构造函数
        }
        // 把收到的消息(玩家命令)放入到一个队列
        void inMsgRecvQueue(){
            for(int i = 0; i< 2000; ++i){
                cout << "inMsgRecvQueue() excute insert elem " << i << "  cur i = " << i  << endl;
                /**
                 * 加锁解锁要保护的数据
                 */
                //lock_guard<std::mutex> sbGuard(my_mutex);
                // unique_lock 可以完全取代 lock_guard
                //my_mutex.lock();
                //unique_lock<std::mutex> sbGuard1(my_mutex);
                // 移动语义 这个时候就直接把所有权转移到了sbGuard2了
                //unique_lock<std::mutex> sbGuard2(std::move(sbGuard1));
                // 也可以通过返回
                unique_lock<std::mutex> sbGuard2 = rtn_unique_lock();           //所有权的传递
                // 解除关系
                //std::mutex *ptx = sbGuard1.release();
                std::mutex *ptx = sbGuard2.release();
                msgRecvQueue.push_back(i);
                // 这里必须手动解除
                ptx->unlock();
                // 自己有责任解锁
//                unique_lock<std::mutex> sbGuard1(my_mutex,std::adopt_lock);
//                unique_lock<std::mutex> sbGuard1(my_mutex,std::try_to_lock); //使用这个第二参数 不能再之前lock 否则会卡住
                // 如果拿到锁的话
//                if(sbGuard1.owns_lock()){
//                    cout << "inMsgRecvQueue 执行，拿到锁头..." << endl;
//                    msgRecvQueue.push_back(i);
//                }
//                else{
//                    cout << "inMsgRecvQueue 执行，但是没有拿到锁头..." << endl;
//                }
//                unique_lock<std::mutex> sbGuard1(my_mutex,std::defer_lock); // 初始化一个额没有加锁的mymutex
//                sbGuard1.lock(); // 自己加锁
//                // 有一些非共享代码要处理 可以先unlock()
//                sbGuard1.unlock();
//                sbGuard1.lock(); // 自己加锁
//                if(sbGuard1.try_lock()){
//                    cout << "inMsgRecvQueue 执行，拿到锁头..." << endl;
//                    msgRecvQueue.push_back(i);
//                }
//                else{
//                    cout << "inMsgRecvQueue 执行，但是没有拿到锁头..." << endl;
//                }
//                msgRecvQueue.push_back(i);
//                sbGuard1.unlock();
            }
        }

        bool outMsgLULProc(int& command){
            //std::lock_guard<std::mutex> sbGuard(my_mutex); //用了lock_guard 就不能用unlock和lock了
            std::unique_lock<std::mutex> sbGuard1(my_mutex, std::try_to_lock);     //unique_lock 就不能用unlock和lock了

            //std::chrono::milliseconds dura(20000);        //单位ms 20s
            //std::this_thread::sleep_for(dura);            // 线程休息20s 这回导致另外一个线程也会等待

            //my_mutex.lock();
            if (sbGuard1.owns_lock()) {
                if(!msgRecvQueue.empty()){
                    // 消息队列不为空
                    command = msgRecvQueue.front();// 返回第一个元素，不检查元素是否存在
                    msgRecvQueue.pop_front();
                    //my_mutex.unlock() ;
                    return true;
                }else {
                    //my_mutex.unlock();
                    //队列空
                    //不能在这种里面进行休眠，会导致插入线程的持续执行
                    //std::chrono::milliseconds dura(2);        //单位ms 20ms
                   // std::this_thread::sleep_for(dura);         // 线程休息20s 这回导致另外一个线程也会等待
                }
            }else {
                //没有拿到锁
                std::cout << "no get lock so i only do other things" << endl;
            }
            return false;
        }

        // 把数据从消息队列中取出的线程
        void outMsgRecvQueue(){
            int command = 0;
            for(int i = 0; i< 2000; ++i){
                bool result = outMsgLULProc(command);
                if(result){
                    cout << "outMsgRecvQueue() excute get one elem " << command << "  cur i = " << i << endl;
                }
                else {
                    cout << "outMsgRecvQueue() excute ,but get msg is empty , cur i = " << i << endl;
                }
            }
            cout << "end " << endl;
        }
    private:
        std::list<int> msgRecvQueue;
        std::mutex my_mutex;

    };

    void test();
}


/*
class thread_mutex_test {
};
*/

#endif //CPLUSTHREAD_WXJ2023_THREAD_MUTEX_TEST_H
