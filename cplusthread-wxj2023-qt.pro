QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
        thread_async_future_promise.cpp \
        thread_base_create.cpp \
        thread_condition_variable_test.cpp \
        thread_data_sharing.cpp \
        thread_future_other.cpp \
        thread_mutex_test.cpp \
        thread_sington_test.cpp \
        thread_transfer_parameters.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    thread_async_future_promise.h \
    thread_base_create.h \
    thread_condition_variable_test.h \
    thread_data_sharing.h \
    thread_future_other.h \
    thread_mutex_test.h \
    thread_sington_test.h \
    thread_transfer_parameters.h
