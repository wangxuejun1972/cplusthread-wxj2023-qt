#include "thread_async_future_promise.h"

/**
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

namespace Thread_sync_future_packaged_task_promise{

    int myThread1(){
        cout << "myThread start " << "thread id: " << std::this_thread::get_id() << endl;
        std::chrono::milliseconds dura(5000); // 休息 5 s
        std::this_thread::sleep_for(dura);
        cout << "myThread end " << "thread id: " << std::this_thread::get_id() << endl;
        cout << "mythread1 return value " << endl;
        return 5;
    }

    int myThread2(int myVar){
        cout << myVar << endl;
        cout << "myThread start " << "thread id: " << std::this_thread::get_id() << endl;
        std::chrono::milliseconds dura(5000); // 休息 5 s
        std::this_thread::sleep_for(dura);
        cout << "myThread end " << "thread id: " << std::this_thread::get_id() << endl;
        return 3;
    }

    void myThread3(std::promise<int>& temp,int calc){
        //做一系列复杂的参数
        calc++;
        calc*=10;
        std::chrono::milliseconds dura(5000); // 休息 5 s
        std::this_thread::sleep_for(dura);

        //计算出结果
        int result = calc;
        // 保存结果
        temp.set_value(result); // 结果保存在temp中
        return;
    }

    void myThread4(std::future<int>& tmpf){
        auto result = tmpf.get();
        cout << "mythread4 result " << result << endl;
    }

    //直接使用线程函数处理器启动线程
    void test1(){
        Thread_sync_future_packaged_task_promise::AClass a;
        int tmp = 12;
        cout << "hello world" << "main thread id: " << std::this_thread::get_id() <<endl;
        //自动创建一个线程并执行，result 是一个将来值
        std::future<int> result = std::async(Thread_sync_future_packaged_task_promise::myThread1);

        cout << "continue.......!" << endl;
        int def = 0;
        //result.get() ;//这里会卡住 等待线程函数执行完毕 拿到结果
        //result.wait(); // 等待线程返回，但是不返回结果 类似于join()
        cout << "mythread4 result =  " << result.get() << endl;     //result.get()会卡在这里
        //    cout << result.get() << endl;  // 不可以连续两次get()
         cout << "mythread1 finish " << endl;
    }

    //使用类成员函数作为线程处理器
    void test2(){
        Thread_sync_future_packaged_task_promise::AClass a;
        int tmp = 12;
        cout << "hello world" << "main thread id: " << std::this_thread::get_id() <<endl;
        //以成员函数作为线程入口，第二个参数用的引用 才保住线程里是一个对象 这里好像没有创建线程，因为线程id是一样的
        std::future<int> result = std::async(&Thread_sync_future_packaged_task_promise::AClass::myThread, &a, tmp);
        cout << "continue.......!" << endl;
        int def = 0;
        result.get() ;//这里会卡住 等待线程函数执行完毕 拿到结果
        //result.wait(); // 等待线程返回，但是不返回结果 类似于join()
        cout << result.get() << endl;
        //    cout << result.get() << endl;  // 不可以连续两次get()
    }

    // 表示延迟调用线程函数(使用std::launch::deferred) 直到get或者wait调用 后才真正执行线程
    void test3(){
        Thread_sync_future_packaged_task_promise::AClass a;
        int tmp = 12;
        cout << "hello world" << "main thread id: " << std::this_thread::get_id() <<endl;

        // 表示延迟调用线程函数(使用std::launch::deferred) 直到get或者wait调用 后才真正执行线程
        std::future<int> result = std::async(std::launch::deferred,&Thread_sync_future_packaged_task_promise::AClass::myThread, &a, tmp);

        cout << "continue.......!" << endl;
        int def = 0;
        def = result.get() ;//这里会卡住 等待线程函数执行完毕 拿到结果
        //result.wait(); // 等待线程返回，但是不返回结果 类似于join()
        cout << def << endl;
        //    cout << result.get() << endl;  // 不可以连续两次get()
    }

    void test4(){
        Thread_sync_future_packaged_task_promise::AClass a;
        int tmp = 12;
        //表示调用async函数的时候 就开始创建线程,立即执行线程函数
        std::future<int> result = std::async(std::launch::async,&Thread_sync_future_packaged_task_promise::AClass::myThread, &a, tmp);
        cout << "continue.......!" << endl;
        int def = 0;
        result.get() ;      //  这里会卡住 等待线程函数执行完毕 拿到结果
        //result.wait();    //  等待线程返回，但是不返回结果 类似于join()
        cout << result.get() << endl;
        //    cout << result.get() << endl;  // 不可以连续两次get()
    }

    //将线程函数 myThread2做一个外壳包装--然后启动线程
    void test5()
    {
        cout <<  "main thread id: " << std::this_thread::get_id() <<endl;

        //将线程函数 myThread2做一个外壳包装
        std::packaged_task <int(int)>  mypt(myThread2);             //可以理解为定义一个函数指针变量
        //使用这个外壳包装对象启动线程
        std::thread t1(std::ref(mypt), 1) ;

        t1.join() ; //等待结束

        std::future<int> result = mypt.get_future() ;

        cout << "result.get()  = " << result.get() << endl;             //这里已经不会阻塞了
    }

    //将lambda 做一个外壳包装线程函数 --然后启动线程
    void test6()
    {
        cout <<  "main thread id: " << std::this_thread::get_id() <<endl;

        /*
        //将 lambda做一个外壳包装为线程函数  -- packaged_task 包装lambda 表达式
        std::packaged_task <int(int)>  mypt([](int mywar){
            cout << mywar << endl;
            cout << "myThread start " << "thread id: " << std::this_thread::get_id() << endl;
            std::chrono::milliseconds dura(5000); // 休息 5 s
            std::this_thread::sleep_for(dura);
            cout << "myThread end " << "thread id: " << std::this_thread::get_id() << endl;
            return 3;
        });             //可以理解为定义一个函数指针变量
        */
        function<int(int)> ll;              //函数指针
        ll = [](int x) -> int {
            cout << x << endl;
            cout << "myThread start " << "thread id: " << std::this_thread::get_id() << endl;
            std::chrono::milliseconds dura(5000); // 休息 5 s
            std::this_thread::sleep_for(dura);
            cout << "myThread end " << "thread id: " << std::this_thread::get_id() << endl;
            return 2;
        };
        std::packaged_task<int(int)> mypt(std::ref(ll));


        //使用这个外壳包装对象启动线程
        std::thread t1(std::ref(mypt), 1) ;

        t1.join() ; //等待结束

        std::future<int> result = mypt.get_future() ;

        cout << "result.get()  = " << result.get() << endl;             //这里已经不会阻塞了
    }

    //直接调用，函数调用
    void test7() {
        function<int(int)> ll;
        ll = [](int x) -> int {
            cout << x << endl;
            cout << "myThread start " << "thread id: " << std::this_thread::get_id() << endl;
            std::chrono::milliseconds dura(5000); // 休息 5 s
            std::this_thread::sleep_for(dura);
            cout << "myThread end " << "thread id: " << std::this_thread::get_id() << endl;
            return 2;
        };
        std::packaged_task<int(int)> mypt2(std::ref(ll));

        mypt2(20);  // 直接调用，相当于函数调用
        std::future<int> result3 = mypt2.get_future();
        cout << result3.get() << endl;
    }

    //容器packaged_task
    void test8() {

        //将 lambda做一个外壳包装为线程函数  -- packaged_task 包装lambda 表达式
        std::packaged_task <int(int)>  mypt2([](int mywar){
            cout << mywar << endl;
            cout << "myThread start " << "thread id: " << std::this_thread::get_id() << endl;
            std::chrono::milliseconds dura(5000); // 休息 5 s
            std::this_thread::sleep_for(dura);
            cout << "myThread end " << "thread id: " << std::this_thread::get_id() << endl;
            return 3;
        });             //可以理解为定义一个函数指针变量

        vector<packaged_task<int(int)>> myTasks;
        // 把对象 移到容器里
        myTasks.push_back(std::move(mypt2));                //mypt2是函数指针，通过移动将mypt2移动到容器，此句执行后mypt2为空
        std::packaged_task<int(int)> mypt3;                 //函数指针
        auto iter = myTasks.begin();                        //从容器中获取元素
        mypt3 = std::move(*iter);                           //把容器中的元素的指向函数指针已经为空了，被移动给了mypt3
        myTasks.erase(iter);// 迭代器失效,移除容器里那一项
        mypt3(10);
        std::future<int> result = mypt3.get_future();
        cout << result.get() << endl;
    }
    //std::promise用法
    void test9() {
        std::promise<int> myProm;  // 声明一个对象，保存一个int
        std::thread t1(Thread_sync_future_packaged_task_promise::myThread3, std::ref(myProm), 180);
        t1.join(); // 如果你不等 会报异常

        //获取结果值
        std::future<int> result = myProm.get_future();
    //    cout  << result.get() << endl;
        std::thread t2(Thread_sync_future_packaged_task_promise::myThread4,std::ref(result));
        t2.join();
    }
}


























/*
thread_async_future_promise::thread_async_future_promise()
{

}
*/
