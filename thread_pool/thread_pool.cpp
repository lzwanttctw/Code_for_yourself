#include<iostream>
#include<string.h>
#include"thread_pool.h"
#include<unistd.h>

Task* thread_pool::take_task(){
    if(status==closed||tasks.empty())return NULL;
    Task* mission=tasks.front();
    tasks.pop_front();
    return mission;
}

int thread_pool::commit_task(Task* task){
    pthread_mutex_lock(&this->mutex);
    if(task==NULL||status==closed)return -1;
    tasks.push_back(task);
    pthread_mutex_unlock(&this->mutex);
    return 0;
}

void *thread_pool::run(void *arg){
    std::cout<<pthread_self()<<std::endl;
    thread_pool *pool = static_cast<thread_pool*>(arg);   
    while(true){ 
        pthread_mutex_lock(&pool->mutex);
        while(pool->tasks.empty()&&pool->status==running){
            pthread_cond_wait(&pool->cond, &pool->mutex);    
        }
        if(pool->status==closed)break;
        Task* task=pool->take_task();
        pthread_mutex_unlock(&pool->mutex);
        task->work();
    }
    pthread_mutex_unlock(&pool->mutex);
    pthread_exit(NULL);
    return NULL;
}

int thread_pool::init(int thread_count) {
    
    if(pthread_mutex_init(&this->mutex,NULL)!=0){
        std::cout<<"pool init failed"<<std::endl;
        return -1;
    }

    if(pthread_cond_init(&this->cond,NULL)!=0){
        std::cout<<"pool init failed"<<std::endl;
        return -2;
    }

    this->threads=(pthread_t*)malloc(sizeof(pthread_t)*thread_count);
    if(this->threads==NULL){
        std::cout<<"pool init failed"<<std::endl;
        return -3;
    }
    status = running;
    for (int i = 0; i < thread_count; i++)
    {
        if(pthread_create(&this->threads[i],NULL,run,this)!=0){
            continue;
        }
    }
    return thread_count;
}

int thread_pool::wait_all_done(){
    for(int i = 0; i < thread_count; i++){
        pthread_join(threads[i],NULL);  
    }
    return 0;
}

int thread_pool::destroy(){
    if (this==NULL)return -1;
    this->status=closed;
    pthread_cond_broadcast(&this->cond);
    wait_all_done();
    pthread_mutex_destroy(&this->mutex);
    pthread_cond_destroy(&this->cond);
    free(this->threads);
    return 0;
}