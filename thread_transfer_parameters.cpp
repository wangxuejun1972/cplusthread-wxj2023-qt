//
// Created by Administrator on 2023/1/14.
//

#include "thread_transfer_parameters.h"

//// 有参构造的线程的启动 -- 使用简单类型作为参数
namespace ThreadCreate_HasParams_0{
    using namespace std ;
    ////有参数线程函数
    ////在定义string类型的参数时需要添加 const ,否则会导致启动线程语法无法通过，此时传入的参数无法修改原处值，如int i 以传值方式进行传递
    void my_job1(int i, const string &tip) {
        std::cout << "my thread Start! " << std::endl;
        std::cout << "my param i=" << i << " tip=" << tip << std::endl;
        std::cout << "thread do something! " << std::endl;
        std::cout << "my thread End!" << std::endl;
    }

    ////并不是引用,指针(查看地址不同)   实际是值传递 实际得看具体thread构造函数的实现
    void my_job2(const int i, const string &pmybuf) {
        //std::ref
        cout <<  i << endl;
        //printf("i address is %p\n", &i);
        printf("pmybuf address is %p\n", &pmybuf);
        cout << pmybuf << endl;
        return ;
    }

    ////测试代码--测试函数
    void test1() {
        int i=100;
        string  testStr = "hello ,weicome to thread wrold !";
        thread mythread(my_job1, i, testStr);    							// 被定义之后，线程立即开始执行
        if( mythread.joinable() )											// 判断是否可以加入
            mythread.join();   											    // 线程对象进行join后，则需要等待该线程执行完毕后，方可继续执行主线程。
    }

    void test2() {
        int myvar = 1;
        int &myvary = myvar;
        char mybuf[] = "this is a test";

        printf("myvar address is %p\n", &myvar);
        printf("mybuf address is %p\n", &mybuf);

        thread mytobj(my_job2, myvar, mybuf);
        mytobj.join();
        //mytobj.detach();
    }

}

//// 有参构造的线程的启动 -- 使用类类型作为参数
namespace ThreadCreate_HasParams_1 {

    void my_job1(const int &i, const AClass &pmybuf) {  //这里面临的问题是对象实参如果发生构造函数是否会提前于main线程结束之前

        printf("address of buf is %p\n", &pmybuf);

    }
    void test () {
        int myvar = 1;
        int &myvary = myvar;
        char mybuf[] = "this is a test";

        // 2. 使用类
        int mysecond = 12;
        //// AClass a(mysecond);                                    ////这个对象会导致内存非法引用--在子线程为分离模式下，主线程未阻塞，因此局部变量mysecond可能会被释放掉，从而引起内存释放后被非法引用
        //// thread mythread(my_job1, myvar, mysecond);             ////这里不能保证隐式转换的执行点
        thread mythread(my_job1, myvar, AClass(mysecond));          ////使用临时对象
        mythread.join();
        //mythread.detach();
    }
}

namespace ThreadCreate_HasParams_2 {
    using namespace std ;
    //using namespace std::_GLIBCXX_VISIBILITY;

    //// 使用线程id证明临时对象的构造点
    void my_job (const A_advanced &mybuf) {
        cout << "son thread start at:" <<  &mybuf << "------son thread id: " << std::this_thread::get_id() << endl;
    }

    void test1() {
        cout << "Main thread is:"  << std::this_thread::get_id() << endl;
        int mvar = 1;
        thread mythread0(my_job, mvar);                                  //直接引用mvar变量可能存在在分离线程时会存在 非法内存引用
        mythread0.join();
    }
    void test2() {
        cout << "Main thread is:"  << std::this_thread::get_id() << endl;
        int mvar = 1;

        thread mythread1(my_job, A_advanced(mvar));                      //事实上这里会发生一次拷贝构造，临时类对象
        mythread1.join();
    }
    void test3() {
        cout << "Main thread is:"  << std::this_thread::get_id() << endl;
        int mvar = 1;

        //thread mythread2(my_job, std::ref(A_advanced(mvar)));           //使用std::ref可以将参数引用传递到线程地址空间，语法无法通过
        //mythread2.join();
    }

}

// 使用智能指针传递线程参数
namespace ThreadCreate_HasParams_3 {

    void my_job(unique_ptr<int> pzn) {
        cout << "son thread start at: " << &pzn << endl;
    }

    void test() {
        // 4. 智能指针
        unique_ptr<int>  upt(new int(100));
        thread myobj4(my_job, std::move(upt));
        myobj4.join();
    }
}

//// 使用成员函数作为线程函数
namespace ThreadCreate_HasParams_4 {
    void test1() {                                       ////该函数被调用时，线程ID等于调用处的线程id，这里是被主线程调用，因此ID等同于主线程
        //// 使用成员函数作为线程函数
        A_funP ap(10);

        //// 这种方式直接使用对象，进行了拷贝构造，因此使用detach也不会出现问题--但是执行结果不定，因为子线程与主线程并发执行，可能主线程先结束，然后直接kill了子线程（子线程无输出）
        //// 也可能是子线程启动了，然后主线程退出，子线程还在运行，此时会展示子线程输出,总之结果不定。
        thread mythread(&A_funP::thread_word, ap, 15);
        mythread.join();
        //mythread.detach();
    }

    void test2() {
        //// 使用成员函数作为线程函数. //// std::ref直接使用引用(考虑join。不能使用detach)
        A_funP ap(10);
        thread mythread(&A_funP::thread_word, std::ref(ap), 10);                //// std::ref直接使用引用(考虑join。不能使用detach)
        mythread.join();                      ////因为ap对象为局部变量，因此如果不阻塞在他的生命范围内就会被释放，从而引起内存异常引用，不安全
        //mythread.detach();              ////没有拷贝对象，不安全,结果不定，有可能存在子线程未执行就被kill了
    }

    void test3() {
        //// 使用成员函数作为线程函数 //直接使用引用
        A_funP ap(10);
        thread mythread(&A_funP::thread_word, &ap, 10);  //// 直接使用引用, (考虑join。不能使用detach,没有拷贝对象，不安全)， &方式等价std::ref
        mythread.join();
        //mythread.detach();              //// 没有拷贝对象，不安全,结果不定，有可能存在子线程未执行就被kill了
    }

    void test4() {
        //// 使用仿函数
        A_funP ap(10);
        thread mythread(ap, 10);     //// 使用仿函数,这里进行了2次拷贝构造函数，因此可以使用
        //mythread.join();
        mythread.detach();              ////这里进行了2次拷贝构造函数，因此可以使用
    }

    void test5() {
        //// 使用仿函数
        A_funP ap(10);
        thread mythread(std::ref(ap), 10);     //// 使用仿函数,这这里使用std::ref,因此没有执行拷贝构造，因此使用detach会不安全
        mythread.join();
        //mythread.detach();              //// 没有拷贝对象，不安全,结果不定，有可能存在子线程未执行就被kill了
    }
}
