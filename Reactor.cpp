#include <iostream>
#include "pthread.h"

typedef void*(*f1)(void*);
typedef struct Reactor{
    pthread_t th;
    int fd;
    f1 f;

}react;

typedef struct request{
    react *_reactor;
    int id;
}req;

react *newReactor(){
    react *_reactor = (react*)malloc(sizeof(react));
    return _reactor;
}
void InstallHandler(react *_react , f1 f,int id){   
    _react->f = f;
    _react->fd = id;
    req *_req = (req*)malloc(sizeof(req));
    _req->_reactor = _react;
    _req->id = id;
    pthread_create(&_react->th , NULL , f , _req);
}
void RemoveHandler(react *_react , int t){
    pthread_join(_react->th , 0);
    _react->fd = -1;
    _react->f = NULL;
}