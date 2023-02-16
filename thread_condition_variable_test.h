#ifndef THREAD_CONDITION_VARIABLE_TEST_H
#define THREAD_CONDITION_VARIABLE_TEST_H

#include <iostream>
#include <algorithm>
#include <thread>
#include <vector>
#include <list>
#include <mutex>
#include <condition_variable>
using namespace std;

/***
     * 1、条件变量：condition_variable、wait()、notify_one()、notify_all
     * 2、   线程 A：等待一个条件满足
     *       线程 B：专门往消息队列中扔消息
     * 3、   condition_variable 实际上是一个类，是一个和条件相关的类，说白了就是等待一个条件达成
     *       这个类需要和互斥量配合工作
     * 4、 上述代码深入思考
     * 5、 notify_all()
     */

//采用双重检查的方式进行细微优化，以及采用队列空方式休眠一段期间来优化
namespace ThreadShared_unique_lock_2c_optimize{
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
                // 也可以通过返回
                unique_lock<std::mutex> sbGuard2 = rtn_unique_lock();           //所有权的传递
                // 解除关系
                //std::mutex *ptx = sbGuard1.release();
                std::mutex *ptx = sbGuard2.release();
                msgRecvQueue.push_back(i);
                // 这里必须手动解除
                ptx->unlock();
            }
        }

        bool outMsgLULProc(int& command){
            //采用双重锁定方式（双重检查） 可以轻微的提升性能
            if (!msgRecvQueue.empty()) {
                std::unique_lock<std::mutex> sbGuard1(my_mutex);     //unique_lock 就不能用unlock和lock了
                if(!msgRecvQueue.empty()){
                    // 消息队列不为空
                    command = msgRecvQueue.front();// 返回第一个元素，不检查元素是否存在
                    msgRecvQueue.pop_front();
                    //my_mutex.unlock() ;
                    return true;
                }
            }else {
                //std::chrono::milliseconds dura(5);         //单位ms 20ms
                //std::this_thread::sleep_for(dura);         // 线程休息5ms 这回导致另外一个线程也会等待
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
                    std::chrono::milliseconds dura(5);         //单位ms 20ms
                    std::this_thread::sleep_for(dura);         // 线程休息5ms,这里无锁因此不会导致另外的线程等待
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

//通过条件变量来进行通知--配合与互斥量来优化 --notify_one()
namespace ThreadShared_unique_lock_condi_optimize{
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
                // 也可以通过返回
                unique_lock<std::mutex> sbGuard2 = rtn_unique_lock();           //所有权的传递
                msgRecvQueue.push_back(i);  //假设这个数字就是为收到的命令，我直接弄到消息队列中
                my_cond.notify_one();//通过该句我们尝试把wait()的线程唤醒，执行完这样代码，那么outMsgRecvQueue()内的wait就会被唤醒，因为刚插入一个数据，队列已经不空，因此lambda返回true,继续执行lambad后的代码
                                     //这里就相当于把sbGuard2锁给解锁了
                //其他处理
            }
        }

        // 把数据从消息队列中取出的线程
        void outMsgRecvQueue(){
            int command = 0;
            while(true) {
                std::unique_lock<std::mutex> sbGuard1(my_mutex);     //unique_lock 就不能用unlock和lock了
                // wait 等一个东西，
                // 如果第二个参数lambda的返回值为false，那么wait将解锁互斥量，并堵塞到本行（就一直等待在该句），堵塞到某个线程调用notify_one()成员函数为止
                // 如果第二个参数返回值为true，wait()直接返回。直接执行lambda下面的代码。
                // 如果没有第二个参数：那就和第二参数为false类似堵塞到某个线程调用notify_one()成员函数为止
                // 当其他线程用notify_one()将本句wait（）唤醒后，wait就恢复干活
                    //a、wait 不断尝试获取锁，如果获取不到，那么wait久在这里继续反复获取锁，获取到了锁那么就执行 b
                    //b、重新上锁 （实际上获取到锁就锁定了）
                        //b.1、如果wait存在第二表达式，则执行lambda,如果lambda返回false,那么wait又休眠，在这里继续等待其他线程唤醒（my_cond.notify_one()）。
                        //b.2、如果lambda返回true，则wait返回，执行wait后续代码（此时锁还被锁住）。
                        //b.3、如果wait不存在第二个参数，则无条件的执行后续代码
                my_cond.wait(sbGuard1, [this]{
                    if(!msgRecvQueue.empty()){
                        return true;
                    }else return false;
                });

                //处理事务

                //流程能走到这里，互斥锁一定是锁住的，队列中至少存在一条数据，这里不能只处理一条
                command = msgRecvQueue.front();// 返回第一个元素，不检查元素是否存在
                msgRecvQueue.pop_front();
                sbGuard1.unlock();      //因为std::unique_lock的灵活性，所以我们可以随时解锁，以免锁住太长时间
                cout << "outMsgRecvQueue() excute get one elem " << command  << endl;

                //处理事务时，而没有卡在 wait处 ，my_cond.notify_one();就无效

            }
        }
    private:
        std::list<int>              msgRecvQueue;
        std::mutex                  my_mutex;
        std::condition_variable     my_cond;                //条件变量对象
    };

    void test();
}


//通过条件变量来进行通知--配合与互斥量来优化 --notify_all()
namespace ThreadShared_unique_lock_condi_all_optimize{
    class AClass{
    public:
        std::unique_lock<std::mutex> rtn_unique_lock(){
            std::unique_lock<std::mutex> tmpGuard(my_mutex);
            return tmpGuard; // 返回临时对象，调用unique_lock 移动构造函数
        }
        // 把收到的消息(玩家命令)放入到一个队列
        void inMsgRecvQueue(){
            for(int i = 0; i< 2000; ++i){
                /**
                 * 加锁解锁要保护的数据
                 */
                // 也可以通过返回
                unique_lock<std::mutex> sbGuard2 = rtn_unique_lock();           //所有权的传递
                cout << "inMsgRecvQueue() excute insert elem " << i << "  cur i = " << i  << endl;
                msgRecvQueue.push_back(i);  //假设这个数字就是为收到的命令，我直接弄到消息队列中
                my_cond.notify_all();//通过该句我们尝试把wait()的线程唤醒，执行完这样代码，那么outMsgRecvQueue()内的wait就会被唤醒，因为刚插入一个数据，队列已经不空，因此lambda返回true,继续执行lambad后的代码
                                     //这里就相当于把sbGuard2锁给解锁了
                //其他处理
            }
        }

        // 把数据从消息队列中取出的线程
        void outMsgRecvQueue(){
            int command = 0;
            while(true) {
                std::unique_lock<std::mutex> sbGuard1(my_mutex);     //unique_lock 就不能用unlock和lock了
                // wait 等一个东西，
                // 如果第二个参数lambda的返回值为false，那么wait将解锁互斥量，并堵塞到本行（就一直等待在该句），堵塞到某个线程调用notify_one()成员函数为止
                // 如果第二个参数返回值为true，wait()直接返回。直接执行lambda下面的代码。
                // 如果没有第二个参数：那就和第二参数为false类似堵塞到某个线程调用notify_one()成员函数为止
                // 当其他线程用notify_one()将本句wait（）唤醒后，wait就恢复干活
                    //a、wait 不断尝试获取锁，如果获取不到，那么wait久在这里继续反复获取锁，获取到了锁那么就执行 b
                    //b、重新上锁 （实际上获取到锁就锁定了）
                        //b.1、如果wait存在第二表达式，则执行lambda,如果lambda返回false,那么wait又休眠，在这里继续等待其他线程唤醒（my_cond.notify_one()）。
                        //b.2、如果lambda返回true，则wait返回，执行wait后续代码（此时锁还被锁住）。
                        //b.3、如果wait不存在第二个参数，则无条件的执行后续代码
                my_cond.wait(sbGuard1, [this]{
                    if(!msgRecvQueue.empty()){
                        return true;
                    }else return false;
                });

                //处理事务

                //流程能走到这里，互斥锁一定是锁住的，队列中至少存在一条数据，这里不能只处理一条
                command = msgRecvQueue.front();// 返回第一个元素，不检查元素是否存在
                msgRecvQueue.pop_front();

                cout << "thread id = " << std::this_thread::get_id() << "----outMsgRecvQueue() excute get one elem " << command  << endl;
                sbGuard1.unlock();      //因为std::unique_lock的灵活性，所以我们可以随时解锁，以免锁住太长时间


                //处理事务时，而没有卡在 wait处 ，my_cond.notify_one();就无效

            }
        }
    private:
        std::list<int>              msgRecvQueue;
        std::mutex                  my_mutex;
        std::condition_variable     my_cond;                //条件变量对象
    };

    void test();
}








/*
class Thread_condition_variable_Test
{
public:
    Thread_condition_variable_Test();
};
*/

#endif // THREAD_CONDITION_VARIABLE_TEST_H
