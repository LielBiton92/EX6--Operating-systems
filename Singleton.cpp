#include <iostream>
#include <mutex>
#include <stdlib.h>
#include <string>
using namespace std;

template <typename T>
class Singleton
{
public:
    static Singleton *instance(T data)
    {
        if (sig_instance == nullptr)
        {
            std::lock_guard<mutex> guard(mutex_);
            if (sig_instance == nullptr)
            {
                sig_instance = new Singleton();
                sig_instance->data = data;
            }
        }
        return sig_instance;
    }

    void set_data(T a)
    {
        this->data = a;
    }
    T getd_()
    {
        return (this->data);
    }

        void Destroy()
    {

        delete sig_instance;
        sig_instance = nullptr;
        
    }


private:

    T data;

    Singleton() {}
    Singleton(const Singleton &) {}
    Singleton &operator=(const Singleton &) {}
    static Singleton *sig_instance;
    static mutex mutex_;
};
template <typename T>
Singleton<T> *Singleton<T>::sig_instance = 0;
template <typename T>
mutex Singleton<T>::mutex_;

int main(int argc, char const *argv[])
{
    FILE *PTR;
    Singleton<FILE *> *check = Singleton<FILE *>::instance(PTR);
    Singleton<FILE *> *check2 = Singleton<FILE *>::instance(PTR);

    cout << check << endl;
    cout << check2 <<endl;

    cout << check->getd_()<<endl;
    cout << check2->getd_() <<endl;


    

    


    


}
