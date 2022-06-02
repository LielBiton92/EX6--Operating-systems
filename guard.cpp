#include <pthread.h>
#include <iostream>
pthread_mutex_t guardlock = PTHREAD_MUTEX_INITIALIZER;

class guard
{
private:
public:
    guard()
    {
        pthread_mutex_lock(&guardlock);
        std::cout << " lock" << std::endl;
    };
    ~guard()
    {
        pthread_mutex_unlock(&guardlock);
        std::cout << " unlock" << std::endl;
    };
};
int runner = 0;
int runner2 = 0;
void *tf(void *arg)
{
    guard gg{};
    for (int i = 0; i < 100; i++)
    {

        std::cout << *((std::string *)arg) << " runner :  " << runner++ << std::endl;
    }
    return NULL;
}

void *tf2(void *arg)
{
    guard gg{};

    for (size_t i = 0; i < 10; i++)
    {
        runner2 = runner2 + *((int *)arg);
        std::cout << "thread id = " << *((int *)arg) << " runner2 : " << runner2 << std::endl;
    }
    return NULL;
}

int main(int argc, char const *argv[])
{
    pthread_t th1, th2;
    std::string threadNumber1 = "Thread number 1";
    std::string threadNumber2 = "Thread number 2";
    if (pthread_create(&th1, NULL, &tf, &threadNumber1) != 0)
    {
        return 1;
    }
    if (pthread_create(&th2, NULL, &tf, &threadNumber2) != 0)
    {
        return 1;
    }
    pthread_join(th1, NULL);
    pthread_join(th2, NULL);

    pthread_t th3, th4;
    int id3 = 3;
    int id4 = 4;
    if (pthread_create(&th3, NULL, &tf2, &id3) != 0)
    {
        return 1;
    }
    if (pthread_create(&th4, NULL, &tf2, &id4) != 0)
    {
        return 1;
    }
    pthread_join(th3, NULL);
    pthread_join(th4, NULL);
}
