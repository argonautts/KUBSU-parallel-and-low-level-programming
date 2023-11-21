#include <chrono>
#include <iostream>
#include <string>
using namespace std;

class LogDuration {
public:
    explicit LogDuration(const string& message = "")
        : messages(message + ": ")
        , start(chrono::steady_clock::now())
    {
    }

    ~LogDuration() {
        auto finish = chrono::steady_clock::now();
        auto duration = finish - start;
        cerr << messages
            << chrono::duration_cast<chrono::milliseconds>(duration).count()
            << " ms" << endl;
    }   
private:
    string messages;
    chrono::steady_clock::time_point start;
};

#define UNIQ_ID_IMPL(lineno) _a_local_var_##lineno
#define UNIQ_ID(lineno) UNIQ_ID_IMPL(lineno)

#define LOG_DURATION(messages) \
  LogDuration UNIQ_ID(__LINE__){messages}; 
