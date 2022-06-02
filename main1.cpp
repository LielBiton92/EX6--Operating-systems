#include <stdlib.h>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <string>
#include <errno.h>
#include <string.h>
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
#define PORT "3541" // the port users will be connecting to
#define BACKLOG 10 // how many pending connections queue will hold



void* caesar_shift_by_1(void *p) {
    std::string *data = (std::string*)p;
    for (int i = 0 ; i < data->size();i++) {
        data->at(i) = (data->at(i))+1;
    }
    return (void*)data;
}

void *A1F1(void *t){

    return t;

}

// void *A1F2(void *t){


// }

// void *A2F1(void *t){


// }

// void *A2F2(void *t){


// }

// void *A3F1(void *t){


// }

// void *A3F2(void *t){


// }


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

Queue *q1;
Queue *q2;
Queue *q3;




	


typedef struct Active_Object {

    void* (*function1)(void*);
    void* (*function2)(void*);
    struct Queue* Q;
    pthread_t t;

}Active_o;

typedef struct pipline {
	Active_o *ActiveOne;
	Active_o *ActiveTwo;
	Active_o *ActiveThree; 

}Pipl;

void* runner(void *ActiveO){
	Active_o *Active = (Active_o*)ActiveO;
	while(Active->Q->Capacity > 0){
        // printf("data in runner : %s\n",(char*)(Active->Q->front->data));
        std::cout << "data in runner : "<< *(std::string*)(Active->Q->front->data);
		void* func1 = Active->function1(deQ(Active->Q));
		void * funct2 = Active->function2(func1);
	}

	return (void*)Active;
}

Active_o* newAO(struct Queue *q,void* function1 (void*),void* function2 (void*))
{   
    
    Active_o *Active_obj;
	Active_obj->Q = q;
    printf("in const %s\n", (char*)(Active_obj->Q->front->data));
	// std::cout << "in const : "<< *(std::string*)Active_obj->Q->front->data<<std::endl;
	    std::cout << "address : "<< (std::string*)(Active_obj->Q->front->data)<<std::endl;
    Active_obj->function1 = function1;
    Active_obj->function2 = function2;
    std::cout << "before thread "<<std::endl;
    pthread_create(&Active_obj->t,NULL, runner, ((void*)Active_obj));
    std::cout << "after thread;"<<std::endl;

    return Active_obj;
}

void destroyAO(Active_o *AO){
	pthread_cancel(AO->t);
	free(AO);
}

void sigchld_handler(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while (waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}


void *T_FUNCTION(void *new_fdcl)
{

       Queue *q1 = createQ();
	Queue *q2 = createQ();
	Queue *q3 = createQ();


    int th_cl = *(int *)new_fdcl;
    printf("new client connect to server %d\n", th_cl);
    if (send(th_cl, "Hello client!", 13, 0) == -1)
    {
        perror("send");
    }
    char *input;
    size_t buf_size = 1024;
    while (true)
    {
        input = (char *)calloc(buf_size, sizeof(char));
        if (recv(th_cl, input, buf_size, 0) != 0)
        {
            input[strlen(input)-1]='\0';
			
			enQ(q1 ,(void*)input);
			char *data = (char*)(q1->front->data);
			printf("DATA : %s\n" , data);
            Active_o *A1 = newAO(q1 , caesar_shift_by_1 , A1F1);
            printf("DATA2 : %s\n" , data);
            pthread_join(A1->t , NULL);
            std::cout<< "size :" << A1->Q->Capacity <<std::endl;
            // printf("DATA3 : %s" , data);

            // printf("input : %s ", input);
            // destroyAO(A1);
        }
    

            
    }
    close(th_cl);
    printf("finish task %d\n", th_cl);
    return NULL;
}

// void *run_server(){


// }





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

// int main()
// {   
	// std::string st = "ABC";
	// Queue *q = createQ();
	// enQ(q, &st);

	// Active_o *test = newAO(q , func1 ,func2);




    // pthread_join(test->t , NULL);
    // std::cout <<"ans"<<std::endl;
	// std::cout << "address : "<< &st<<std::endl;

    // std::cout << st;

  

		

	
// }


int main(void)
{
 

	// Active_o *A2 = (Active_o*)malloc(sizeof(Active_o));
	// pthread_t t2;
	// A2->Q = q2;
	// A2->function1 = A2F1;
	// A2->function2 = A2F2;
	// A2->t = t2;

	// Active_o *A3 = (Active_o*)malloc(sizeof(Active_o));
	// pthread_t t3;
	// A3->Q = q3;
	// A3->function1 = A3F1;
	// A3->function2 = A3F2;
	// A3->t = t3;
    int sockfd, new_fd; // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes = 1;
    char s[INET6_ADDRSTRLEN];
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1)
        {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                       sizeof(int)) == -1)
        {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL)
    {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1)
    {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(1);
    }

    printf("server: waiting for connections...\n");

    while (1)
    { // main accept() loop
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1)
        {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family,
                  get_in_addr((struct sockaddr *)&their_addr),
                  s, sizeof s);
        printf("server: got connection from %s\n", s);
        pthread_t myth;
        pthread_create(&myth, NULL, T_FUNCTION, &new_fd);


    }
    return 0;
}











