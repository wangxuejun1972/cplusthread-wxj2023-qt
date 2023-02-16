//
// Created by Administrator on 2023/1/15 0015.
//

#ifndef CPLUSTHREAD_WXJ2023_THREAD_SINGTON_TEST_H
#define CPLUSTHREAD_WXJ2023_THREAD_SINGTON_TEST_H
#include <iostream>
#include <algorithm>
#include <thread>
#include <vector>
#include <list>
#include <mutex>

/****
     *  单例设计模式 分析 对应代码空间 Sington_X
     *    1、设计模式大概谈：
     *          一些代码的写法（比较特殊，和常规写法不太一样）：程序灵活，维护起来可能方便，但是别人接管和阅读代码会很痛苦
     *          用设计模式理念写出来的代码很晦涩《head first》，为了应付特别大的项目，把项目的开发经验、模块划分经验，总结
     *          整理成 “ 设计模式 ”，设计模式到中国来，拿着程序往设计模式上套，一个小小的项目，没必要用设计模式，本末倒置
     *          （把简单的写成复杂的，这样不好）
     *    2、单例设计模式：整个项目中，由某个或者某些特殊的类，属于该类的对象，我只能创建一个，多了我创建不了
     *          使用频率高
     *    3、单例设计模式，多线程安全问题分析解决
     *          需要在我们自己创建的线程（而不是主线程）创建MyCAS这个单例类的对象，这种线程可能还不只一个
     *          GetInstance() 要互斥
     *    4、std::call_once():函数模板 C++11 引入的 第二个参数是一个函数名
     *          功能：保证函数只被调用一次 ：具备互斥量的能力，效率上比互斥量消耗的资源更少
     *          call_once() 需要和一个标记结合使用 std::once_flag,
     *          call_once() 通过标记决定对应的函数是否执行， 调用call_once成功够，就把std::once_flag 设置为一种已调用
     *          状态
     */

/****
 * 单例设计模式分析
 * 线程安全
 * call_once()
 */
namespace Sington_X{
    // 单线程情况下
    namespace edition1 {
        using namespace std;
        class MyCAS {
        private:
            MyCAS() {} //私有化构造函数
        private:
            static MyCAS *m_instance;               //类静态成员
        public:
            static MyCAS *GetInstance() {
                // 这里多线程下不安全
                if (m_instance == nullptr) {
                    // 两个线程可能同时执行到这里，这样不好
                    m_instance = new MyCAS();
                    // 创建一个静态类对象
                    static GarbageCollection c;
                }
                return m_instance;
            }

            void func() {
                std::cout << "test" << std::endl;
            }

            // 引入一个类，对new的对象进行回收
            class GarbageCollection {
            public:
                ~GarbageCollection() {
                    if (MyCAS::m_instance) {
                        // 连个线程可能同时
                        delete MyCAS::m_instance;
                        MyCAS::m_instance == nullptr;
                    }
                }
            };
        };

        void test();
    }
    // 加锁版本
    namespace edition2{
        using namespace std;
        class MyCAS{
        private:
            MyCAS(){} //私有化构造函数
        private:
            static MyCAS* m_instance;
            static std::mutex resource_mutex;
        public:
            static MyCAS* GetInstance(){
                // 双重锁定【双重检查】，提高效率
                if(m_instance == nullptr){
                    // 加锁
                    std::unique_lock<std::mutex> sbGuard(MyCAS::resource_mutex);
                    if(m_instance == nullptr){
                        m_instance = new MyCAS();
                        // 创建一个静态类对象
                        static GarbageCollection c;
                    }
                }
                return m_instance;
            }
            void func(){
                cout << "test" << endl;
            }
            // 引入一个类，对new的对象进行回收
            class GarbageCollection{
            public:
                ~GarbageCollection(){
                    if(MyCAS::m_instance){
                        // 连个线程可能同时
                        delete MyCAS::m_instance;
                        MyCAS::m_instance == nullptr;
                    }
                }
            };
        };

        void test();
    }

    namespace callOnce{
        using namespace std;
        class MyCAS{
        private:
            MyCAS(){} //私有化构造函数
        private:
            static MyCAS* m_instance;
            static std::mutex resource_mutex;
            static std::once_flag g_flag;
            static void CreateInstance(){
                cout << "createInstance()被执行了" << endl;
                m_instance = new MyCAS();
                static GarbageCollection c;
            }
        public:
            static MyCAS* GetInstance(){
                // 双重锁定【双重检查】，提高效率
                /*if(m_instance == nullptr){
                    // 加锁
                    std::unique_lock<std::mutex> sbGuard(MyCAS::resource_mutex);
                    if(m_instance == nullptr){
                        m_instance = new MyCAS();
                        // 创建一个静态类对象
                        static GarbageCollection c;
                    }
                }*/
                // 如果两个线程同时执行到这里，其中一个线程执行CreateInstance完毕后，才会执行另一个线程 ,*************感觉还是上面屏蔽代码性能更优*************
                std::call_once(g_flag,CreateInstance);
                return m_instance;
            }
            void func(){
                cout << "test" << endl;
            }
            // 引入一个类，对new的对象进行回收
            class GarbageCollection{
            public:
                ~GarbageCollection(){
                    if(MyCAS::m_instance){
                        // 连个线程可能同时
                        delete MyCAS::m_instance;
                        MyCAS::m_instance == nullptr;
                    }
                }
            };
        };

        void test();
    }

    // 线程入口函数
    void myThread();
    void myThread2();
}


/*
class thread_sington_test {

};
*/

#endif //CPLUSTHREAD_WXJ2023_THREAD_SINGTON_TEST_H
