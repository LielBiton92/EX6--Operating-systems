#include <stdlib.h>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <string>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdio.h>


pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

typedef struct QueueNode {
	void* data;
	struct QueueNode* next;
}QueueNode;

typedef struct Queue {
	int Capacity;
	struct QueueNode *front, *back;
}Queue;

struct QueueNode* newNode(void* k){
	struct QueueNode* element = (struct QueueNode*)malloc(sizeof(struct QueueNode));
	element->data = k;
	element->next = NULL;
	return element;
}



struct Queue* createQ(){
	struct Queue* q = (struct Queue*)malloc(sizeof(struct Queue));
	q->front=q->back=NULL;
	q->Capacity = 0 ; 
	return q;
}


void enQ(struct Queue* q, void* k){
	pthread_mutex_lock(&lock);
	struct QueueNode *N = newNode(k);
	if(q->back==NULL){
		q->front = q->back = N;
		q->Capacity++;
		pthread_cond_signal(&cond1);
		pthread_mutex_unlock(&lock);

		return;
	}
	else{
		q->back->next = N;
		q->back = N;
		q->Capacity++;
		pthread_mutex_unlock(&lock);
	}
}

void* deQ(struct Queue* q){
    pthread_mutex_lock(&lock);
	struct QueueNode *temp = q->front;
    if (q->front == NULL) {
        pthread_cond_wait(&cond1, &lock);
		pthread_mutex_unlock(&lock);
    }
    
    if (q->front) {
        q->front = q->front->next;
		q->Capacity--;
		pthread_mutex_unlock(&lock);
		return (void*)temp->data;
    }

    else {
        q->back = NULL;
    pthread_mutex_unlock(&lock);
	return NULL;
}

}


void deleteQ(struct Queue *q){
	    while(q->front!=NULL)
    {
        deQ(q);
    }
	free(q);
}



typedef struct Active_Object {

    void* (*function1)(void*);
    void* (*function2)(void*);
    struct Queue* Q;
    pthread_t t;

}Active_o;

void* runner(void *ActiveO){
	Active_o *Active = (Active_o*)ActiveO;
	int i = 0;
	while(Active->Q->Capacity > 0){
		void* func1 = Active->function1(deQ(Active->Q));
		void * funct2 = Active->function2(func1);
	}

	return (void*)Active;
}

Active_o* newAO(struct Queue *q,void* function1 (void*),void* function2 (void*))
{   
    
    Active_o *Active_obj = (Active_o*)malloc(sizeof(Active_o));
	Active_obj->Q = q;
	std::cout << "in const : "<< *(std::string*)Active_obj->Q->front->data<<std::endl;
	    std::cout << "address : "<< (std::string*)(Active_obj->Q->front->data)<<std::endl;
    Active_obj->function1 = function1;
    Active_obj->function2 = function2;
    pthread_create(&Active_obj->t,NULL, runner, ((void*)Active_obj));

    return Active_obj;
}

void destroyAO(Active_o *AO){
	pthread_cancel(AO->t);
	free(AO);
}

// void *func1(void *pt){
//     std::cout << "in func1 : "<<*(std::string*)(pt) <<std::endl;
//     std::cout << "address : "<< (std::string*)(pt)<<std::endl;
//      std::string *st = (std::string*)pt;

//     for(int i = 0 ; i<st->size();i++){
        
//         st->at(i)=(st->at(i))+1;
//     }
//     std::cout <<"ans in func1 "<<*st<<std::endl;
        

// 	return (void*)st;
// }

// void *func2(void *pt2)
// {
// 	return pt2;
// }

int main()
{   
	// std::string st = "ABC";
	// Queue *q = createQ();
	// enQ(q, &st);

	// Active_o *test = newAO(q , func1 ,func2);




    // pthread_join(test->t , NULL);
    // std::cout <<"ans"<<std::endl;
	// std::cout << "address : "<< &st<<std::endl;

    // std::cout << st;

  

		

	
}











