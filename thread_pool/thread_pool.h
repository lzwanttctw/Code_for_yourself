
#include<deque>
#include<pthread.h>

enum{
    closed,
    running
};

class Task{
public:
    Task(){};
    virtual void* work() = 0;
private:
    void *_arg;
};


class thread_pool
{
private:
    pthread_t* threads;
    pthread_cond_t cond;
    pthread_mutex_t mutex;
    std::deque<Task*> tasks;
    
    int thread_count;
    int status;

    Task* take_task();
    static void* run(void*);
    int wait_all_done();
    
public:
    int init(int);
    int commit_task(Task*);
    int destroy();


    thread_pool(){};
    ~thread_pool(){};
};
