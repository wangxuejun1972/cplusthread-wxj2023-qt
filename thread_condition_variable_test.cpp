#include "thread_condition_variable_test.h"

/*
Thread_condition_variable_Test::Thread_condition_variable_Test()
{

}*/

//采用双重检查的方式进行细微优化，以及采用队列空方式休眠一段期间来优化
namespace ThreadShared_unique_lock_2c_optimize{
    void test(){
        ThreadShared_unique_lock_2c_optimize::AClass myObja;
        thread myOutMsgObj(&ThreadShared_unique_lock_2c_optimize::AClass::outMsgRecvQueue,&myObja); // 第二个参数是引用 才可以保证线程里调用的是一个对象
        thread myInMsgObj(&ThreadShared_unique_lock_2c_optimize::AClass::inMsgRecvQueue,&myObja);
        myOutMsgObj.join();
        myInMsgObj.join();
    }
}
//通过条件变量来进行通知--配合与互斥量来优化--notify_one()
namespace ThreadShared_unique_lock_condi_optimize{
    void test(){
        ThreadShared_unique_lock_2c_optimize::AClass myObja;
        thread myOutMsgObj(&ThreadShared_unique_lock_2c_optimize::AClass::outMsgRecvQueue,&myObja); // 第二个参数是引用 才可以保证线程里调用的是一个对象
        thread myInMsgObj(&ThreadShared_unique_lock_2c_optimize::AClass::inMsgRecvQueue,&myObja);
        myOutMsgObj.join();
        myInMsgObj.join();
    }
}
//通过条件变量来进行通知--配合与互斥量来优化--notify_all()
namespace ThreadShared_unique_lock_condi_all_optimize{
    void test(){
        ThreadShared_unique_lock_condi_all_optimize::AClass myObja;
        thread myOutMsgObj(&ThreadShared_unique_lock_condi_all_optimize::AClass::outMsgRecvQueue,&myObja); // 第二个参数是引用 才可以保证线程里调用的是一个对象
        thread myOutMsgObj1(&ThreadShared_unique_lock_condi_all_optimize::AClass::outMsgRecvQueue,&myObja); // 第二个参数是引用 才可以保证线程里调用的是一个对象
        thread myInMsgObj(&ThreadShared_unique_lock_condi_all_optimize::AClass::inMsgRecvQueue,&myObja);
        myOutMsgObj.join();
        myOutMsgObj1.join();
        myInMsgObj.join();
    }
}
