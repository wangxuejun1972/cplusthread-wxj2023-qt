#include "thread_future_other.h"

/*
Thread_future_other::Thread_future_other()
{

}
*/
namespace thread_future_other{
    using namespace std;

    int g_mycout = 0;
    std::mutex my_mutex;
    std::atomic<int> g_atomic(0);  // 封装了一个类型为int的原子对象
    std::atomic<bool> g_ifend = { false}; // 线程退出标记

    void myThreadAtomicBool(){
        std::chrono::seconds dura(1);
        while(!g_ifend){
            // 线程没退出
            std::cout << "thread bool id "  << std::this_thread::get_id() << "start"<< std::endl;
            std::this_thread::sleep_for(dura);
        }
        std::cout << "thread bool id "  << std::this_thread::get_id() << "end" << std::endl;
    }

    void myThreadAtomic(){
        for(int i= 0;i < 1000000; ++i){
//            my_mutex.lock();
            g_mycout++;         // 普通变量
            g_atomic++;         // 原子变量、效率比加锁减锁效率高
//            my_mutex.unlock();
        }
        return;
    }
    int myThread(){
        cout << "myThread start " << "thread id: " << std::this_thread::get_id() << endl;
        std::chrono::milliseconds dura(5000); // 休息 5 s
        std::this_thread::sleep_for(dura);
        cout << "myThread end " << "thread id: " << std::this_thread::get_id() << endl;
        return 0;
    }
    int myThread2(int temp){
        cout << temp;
        cout << "myThread2 start " << "thread id: " << std::this_thread::get_id() << endl;
        std::chrono::milliseconds dura(5000); // 休息 5 s
        std::this_thread::sleep_for(dura);
        cout << "myThread2 end " << "thread id: " << std::this_thread::get_id() << endl;
        return 0;
    }
    void myThread3(std::future<int>& tmpf){
        auto result = tmpf.get(); //第二次会异常，因为get() 是一个移动语义
        cout << "mythread3 result " << result << endl;
    }
    void myThread4(std::shared_future<int>& tmpf){
        auto result = tmpf.get(); //第二次会异常，因为get() 是一个移动语义
        cout << "mythread3 result " << result << endl;
    }

    //std::future_status 状态测试
    void test1() {
        /**
         * 1、std::future_status status wait_for()
         */
        cout << "hello world" << "main thread id: " << std::this_thread::get_id() <<endl;
        // 自动创建一个线程并执行，result 是一个将来值
        std::future<int> result = std::async(std::launch::async,thread_future_other::myThread);
        //std::future<int> result = std::async(std::launch::deferred,thread_future_other::myThread);        //推迟方式执行线程，在get或者wait执行时才启动
        cout << "continue.......!" << endl;
        std::future_status status = result.wait_for(std::chrono::seconds(6)); // 等待一秒 ，等待一秒获得线程状态
        if(status == std::future_status::timeout){  //超时：我想等待1s，希望你返回，但是你没有返回，所以超时
            //表示线程还没执行完，
            cout << "超时，线程还没执行完" << endl;
        }
        else if(status == std::future_status::ready){
            cout << " 线程成功返回 " << endl;
        }
        else if(status == std::future_status::deferred){
            cout << "线程被延迟执行" << endl;
            // 这个线程函数是在主线程执行的，相当于没有创建子线程
            cout << result.get() << endl;
        }
    }

    //share_future，（类模板）解决多个线程都像得到结果，他的get() 是复制可以get多次
    void test2() {
        std::packaged_task<int(int)> mypt(thread_future_other::myThread2);
        std::thread t1(std::ref(mypt),1);
        t1.join();
        std::future<int> result = mypt.get_future();
        std::shared_future<int> result_s(std::move(result));
        bool ifcanget = result.valid();
        //std::shared_future<int> result_s(result.share());
        //std::shared_future<int> result_s(mypt.get_future()); //通过get_future 构造一个shared_future 对象
        auto mythreadRes = result_s.get();
        mythreadRes = result_s.get();
        ifcanget = result.valid();

        std::thread t2(thread_future_other::myThread4,std::ref(result_s));
        t2.join();
        cout << result_s.get() << endl;
        cout << result_s.get() << endl;
        cout << result_s.get() << endl;
    }

    void test3() {
        thread myObj1(thread_future_other::myThreadAtomicBool);
        thread myObj2(thread_future_other::myThreadAtomicBool);
        std::chrono::seconds dura(5);
        std::this_thread::sleep_for(dura);
        thread_future_other::g_ifend = true;
        myObj2.join();
        myObj1.join();

        cout << "last result: " << thread_future_other::g_mycout << endl;
        cout << "last result: " << thread_future_other::g_atomic << endl;
    }
}

