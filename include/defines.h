#pragma once

#define SCREEN2WEB_NM screen2web

#define _START_SCREEN2WEB_NM_ \
    namespace screen2web      \
    {

#define _END_SCREEN2WEB_NM_ \
    }

#define SCREEN2WEB_SINGLETON(classname)    \
public:                                    \
    static classname &GetInstance()        \
    {                                      \
        static classname instance;         \
        return instance;                   \
    }                                      \
                                           \
private:                                   \
    classname() = default;                 \
    ~classname() = default;                \
    classname(const classname &) = delete; \
    classname &operator=(const classname &) = delete;
