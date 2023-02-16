//
// Created by Administrator on 2023/1/14.
//

#ifndef CPLUSTHREAD_WXJ2023_THREAD_TRANSFER_PARAMETERS_H
#define CPLUSTHREAD_WXJ2023_THREAD_TRANSFER_PARAMETERS_H

#include <functional>
#include <iostream>
#include <algorithm>
#include <thread>

#include <memory>                           //unique_ptr
using namespace std;

//#include <big/refwrap.h>
/*
       chp2. 线程传参详解，detach注意事项，成员函数做线程函数
       1. 传递临时对象作为线程参数
           个人觉得这个问题出现的必要是因为使用引用传参可以提高效率(减少copy ctor的调用次数)，而应用本身和线程detach的内存模型又是矛盾的，所以才会产生这么多的解决办法
            my_job1: 函数参数传递引用和指针的不确定性，所以将引用改为传值，将char指针改为const string &
            my_job2: 函数参数使用类对象的话，使用引用可以减少copy ctor的调用次数，实参的选择很关键：
            a. 不能直接在main中构造一个对象，因为会被释放掉
            b. 所以只能采取临时对象，但是需要禁止隐式构造，因为不能确保其执行点在main结束之前~
            c. 综上所述，只有使用显式临时对象

       2. 临时对象
           获取线程id : std::this_thread::get_id()
           通过myprint3可以发现
                a. 使用隐式转换时，构造发生在子线程
                b. 使用显式转换时，构造发生在主线程
                                addr                thread-id
                    ctor/dtor   0x7ffee2446688      0x11565d5c0
                    copy/dtor   0x7ffee24465c8      0x11565d5c0
                    copy        0x7f99a3402670      0x11565d5c0(拷贝构造发生在主线程)
                    dtor        0x7f99a3402670      0x700001ef8000(析构发生在子线程)
                c. 如果函数参数使用传值而不是引用，会多一次copy ctor和dctr
        3. 使用智能指针作为线程参数
            使用std::move()传递参数
        4. 使用成员函数指针做线程函数
            需要传递成员函数地址作为参数
*/

//// 有参构造的线程的启动--简单类型做参数
namespace ThreadCreate_HasParams_0{
    using namespace std ;
    //有参数线程函数
    void my_job1(int i, const string &tip) ;            //// 在定义string类型的参数时需要添加 const ,否则会导致启动线程语法无法通过，此时传入的参数无法修改原处值，如int i 以传值方式进行传递
    void my_job2(const int i, const string &pmybuf) ;   //// 并不是引用,指针(查看地址不同)   实际是值传递 实际得看具体thread构造函数的实现

    void test1();                                       //// 使用简单类型
    void test2();                                       //// 使用简单类型
}

//// 有参构造的线程的启动 -- 使用类类型作为参数
namespace ThreadCreate_HasParams_1 {
    using namespace std ;
    class AClass
    {
    public:
        AClass (int a) : m_i(a) {
            cout << "ctor start" << this << endl;
        }

        AClass(const AClass& a) : m_i(a.m_i) {
            cout << "copy ctor start" << this << endl;
        }

        ~AClass() {
            cout << "dtor start" << this << endl;
        }

    private:
        int m_i;

    };

    void my_job1(const int &i, const AClass &pmybuf);     //// 这里面临的问题是对象实参如果发生构造函数是否会提前于main线程结束之前

    //// 使用类
    void test ();
}

//// 使用线程id证明临时对象的构造点
namespace ThreadCreate_HasParams_2 {

    using namespace std ;

    class A_advanced
    {
    public:
        A_advanced (int a) : m_i(a) {
            cout << "ctor start at: " << this <<
                 "------thread id: " << std::this_thread::get_id() << endl;
        }

        A_advanced(const A_advanced& a) : m_i(a.m_i) {
            cout << "copy ctor start at: " << this <<
                 "------thread id: " << std::this_thread::get_id() << endl;
        }

        ~A_advanced() {
            cout << "dtor start at: " << this <<
                 "------thread id: " << std::this_thread::get_id() << endl;
        }

    private:
        int m_i;

    };

    //// 使用线程id证明临时对象的构造点
    void my_job (const A_advanced &mybuf) ;

    void test1();       ////直接引用mvar变量可能存在在分离线程时会存在 非法内存引用
    void test2();       ////使用临时类对象 --事实上这里会发生一次拷贝构造，临时类对象
    void test3();       ////使用std::ref可以将参数引用传递到线程地址空间，语法无法通过 ////使用类作为线程参数

}

//// 使用智能指针传递线程参数
namespace ThreadCreate_HasParams_3 {

    void my_job(unique_ptr<int> pzn);

    void test();
}

//// 使用成员函数作为线程函数
namespace ThreadCreate_HasParams_4 {
    class A_funP
    {
    public:

        A_funP(int a) : _num(a) {
            cout << "constructor start at: " << this <<
                 "------thread id: " << std::this_thread::get_id() << endl;
        }

        A_funP(const A_funP& a) : _num(a._num) {
            cout << "copy constructor start at: " << this <<
                 "------thread id: " << std::this_thread::get_id() << endl;
        }

        ~A_funP() {
            cout << "destructor start at: " << this <<
                 "------thread id: " << std::this_thread::get_id() << endl;
        }
        //明确的线程处理器
        void thread_word(int num) {
            cout << "thread start at:" <<  this <<
                 "------son thread id: " << std::this_thread::get_id() << endl;
            _num = num;
        }

        //仿函数--作为可执行对象的线程处理器
        void operator()(int num) {

            cout << "thread() processor run start at:" <<  this <<
                 "------son thread id: " << std::this_thread::get_id() << endl;
            _num = num;

        }
    private:
        int _num;
    };

    void test1();

    void test2();

    void test3();

    void test4();

    void test5();
}

/*
class thread_transfer_parameters {

};
 */


#endif //CPLUSTHREAD_WXJ2023_THREAD_TRANSFER_PARAMETERS_H
