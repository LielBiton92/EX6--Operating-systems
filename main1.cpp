
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

int th;

pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
#define PORT "3552" // the port users will be connecting to
#define BACKLOG 10	// how many pending connections queue will hold

void *caesar(void *p)
{
	std::string *data = (std::string *)p;
	for (int i = 0; i < data->size(); i++)
	{
		if(data->at(i)=='z'){
			data->at(i)= 'a';
		}
		else if(data->at(i)=='Z'){
			data->at(i)= 'A';
		}
		else
		data->at(i) = (data->at(i)) + 1;
	}
	return (void *)data;
}
void *A1F1(void *t)
{
	// std::string *data = (std::string*)t;
    // enQ(q2 , data );

    return t;
}

void *lower_or_upper(void *t){
    std::string *data = (std::string*)t;
    for(int i = 0 ; i < data->size();i++){
        char c = data->at(i);
        if(isupper(c)){
            data->at(i) = tolower(c);
        }
        else{
            data->at(i)=toupper(c);
        }
        }
    
    return (void*)data;
}

void *A2F1(void *t){

    return t;

}



void *A3F1(void *t){
	return t;
}

// void *A3F2(void *t){

// }

typedef struct QueueNode
{
	void *data;
	struct QueueNode *next;
} QueueNode;

typedef struct Queue
{
	int Capacity;
	struct QueueNode *front, *back;
} Queue;

struct QueueNode *newNode(void *k)
{
	struct QueueNode *element = (struct QueueNode *)malloc(sizeof(struct QueueNode));
	element->data = k;
	element->next = NULL;
	return element;
}

struct Queue *createQ()
{
	struct Queue *q = (struct Queue *)malloc(sizeof(struct Queue));
	q->front = q->back = NULL;
	q->Capacity = 0;
	return q;
}

void enQ(struct Queue *q, void *k)
{
	pthread_mutex_lock(&lock);
	struct QueueNode *N = newNode(k);
	if (q->back == NULL)
	{
		q->front = q->back = N;
		q->Capacity++;
		pthread_cond_signal(&cond1);
		pthread_mutex_unlock(&lock);

		return;
	}
	else
	{
		q->back->next = N;
		q->back = N;
		q->Capacity++;
		pthread_mutex_unlock(&lock);
	}
}

void *deQ(struct Queue *q)
{
	pthread_mutex_lock(&lock);
	struct QueueNode *temp = q->front;
	if (q->front == NULL)
	{
		pthread_cond_wait(&cond1, &lock);
		pthread_mutex_unlock(&lock);
	}

	if (q->front)
	{
		q->front = q->front->next;
		q->Capacity--;
		pthread_mutex_unlock(&lock);
		return (void *)temp->data;
	}

	else
	{
		q->back = NULL;
		pthread_mutex_unlock(&lock);
		return NULL;
	}
}

void deleteQ(struct Queue *q)
{
	while (q->front != NULL)
	{
		deQ(q);
	}
	free(q);
}

Queue *q1;
Queue *q2;
Queue *q3;

typedef struct Active_Object
{

	void *(*function1)(void *);
	void *(*function2)(void *);
	struct Queue *Q;
	pthread_t t;

} Active_o;

typedef struct pipline
{
	Active_o *ActiveOne;
	Active_o *ActiveTwo;
	Active_o *ActiveThree;
    

} Pipl;

void * send(void *p) {
    QueueNode * Data = (QueueNode *) p;
    // printf("nptr->data: %s\n", nptr->getData());
    // char * toSend = stdStringToCharPointer(*(std::string *)nptr->getData());
    char * toSend = (char *)Data->data;
    printf("sending: %s\n", toSend);
    send(th, toSend, strlen(toSend), 0);
    return Data;
}

void *runner(void *ActiveO)
{
	Active_o *Active = (Active_o *)ActiveO;
	while (Active->Q->Capacity > 0)
	{
		std::cout << "data in runner : " << *(std::string *)(Active->Q->front->data) << std::endl;
		void *func1 = Active->function1(deQ(Active->Q));
		void *funct2 = Active->function2(func1);
	}

	return (void *)Active;
}

Active_o *newAO(struct Queue *q, void *function1(void *), void *function2(void *))
{
	Active_o *Active_obj = (Active_o *)malloc(sizeof(Active_o));
	Active_obj->Q = q;
	// printf("in const: %s\n", (char *)(Active_obj->Q->front->data));
	std::cout << "in const : " << *(std::string *)Active_obj->Q->front->data << std::endl;
	std::cout << "address: " << (std::string *)(Active_obj->Q->front->data) << std::endl;
	Active_obj->function1 = function1;
	Active_obj->function2 = function2;
	std::cout << "before thread" << std::endl;
	pthread_create(&Active_obj->t, NULL, runner, ((void *)Active_obj));
	std::cout << "after thread" << std::endl;

	return Active_obj;
}

void destroyAO(Active_o *AO)
{
	deleteQ(AO->Q);
	pthread_cancel(AO->t);
	free(AO);
}

void sigchld_handler(int s)
{
	// waitpid() might overwrite errno, so we save and restore it:
	int saved_errno = errno;

	while (waitpid(-1, NULL, WNOHANG) > 0)
		;

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

std::string char_arr_to_string(char *arr)
{
	std::string ans;
	for (int i = 0; i < strlen(arr); i++)
	{
		ans += arr[i];
	}
	return ans;
}

char *string_to_char_arr(std::string str)
{
	char *ans = (char *)malloc(sizeof(char) * str.size());
	for (int i = 0; i < str.size(); i++)
	{
		ans[i] = str[i];
	}
	return ans;
}

void initializePip(Pipl *p, char *input , int sock){
			th = sock;
			std::string str_input = char_arr_to_string(input);
			std::cout << "str_input: " << str_input << std::endl;
            Queue *q3 = createQ();
            Queue *q2 = createQ();
			Queue *q1 = createQ();
			enQ(q1, &str_input);
            enQ(q2, &str_input);
			enQ(q3, &str_input);
			std::cout << "Data1 address: " << (q1->front->data) << std::endl;
			std::cout << "Data1: " << *(std::string *)(q1->front->data) << std::endl;
			p->ActiveOne = newAO(q1, caesar, A1F1);
			pthread_join(p->ActiveOne->t, NULL);
			std::cout << "input_ans_after_ceaser: " << str_input << std::endl;
            p->ActiveTwo= newAO(q2 , lower_or_upper , A2F1);
            pthread_join(p->ActiveTwo->t , NULL);
            p->ActiveThree = newAO(q3 , send , A3F1 );
			pthread_join(p->ActiveThree->t , NULL);
            char *ans = string_to_char_arr(str_input);
			destroyAO(p->ActiveOne);
            destroyAO(p->ActiveTwo);
			destroyAO(p->ActiveThree);
			free(p);

}

void *T_FUNCTION(void *new_fdcl)
{

	

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
			input[strlen(input) - 1] = '\0';
            Pipl *p = (Pipl*)malloc(sizeof(Pipl));
			initializePip(p ,input , th_cl);


		}
	}
	close(th_cl);
	printf("finish task %d\n", th_cl);
	return NULL;
}

int main(void)
{
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