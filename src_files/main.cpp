#include <iostream>
#include <pthread.h>
#include <stdlib.h>
#include <list>
#include <memory>

using namespace std;

typedef struct flags {
    int producers_threads;
    int consumers_threads;
    int items;
    int buffer_size;
}Flags;

// Static Memory
static int items_on_buffer = 0;

// Mutex
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Condition variables
pthread_cond_t c = PTHREAD_COND_INITIALIZER;
pthread_cond_t p = PTHREAD_COND_INITIALIZER;

// Heap variables
list<int> buffer;

// Functions
void printInformations(int thread,Flags *flags,int item, bool type){
    if (type) {
        cout << "Thread Produtora [" << thread << "]\t Item inserido = " << item;
    }else{
        cout << "Thread Consumidora [" << thread << "]\t Item removido = " << item;
    }
    cout << "\tItens no buffer = " << items_on_buffer << "\tBuffer = [";
    for (int item  : buffer) {
        cout << item << " ";
    }
    cout << "]" << endl;
}

void* Producer(void* flags){
    auto* flag = (Flags *) flags;

    for (int i = 0; i < flag->items; ++i) {
        pthread_mutex_lock(&mutex);
        while (items_on_buffer == flag->buffer_size) { // buffer is full
            pthread_cond_wait(&p, &mutex);
        }
        int item = rand() % 100;
        buffer.push_back(item); // insert
        ++items_on_buffer;
        printInformations(pthread_self(), flag, item ,true);
        if (items_on_buffer > 0)
            pthread_cond_signal(&c);
        pthread_mutex_unlock(&mutex);
    }
    return nullptr;
}

void* Consumer(void* flags) {
    auto* flag = (Flags *) flags;

    while (true) {
        pthread_mutex_lock(&mutex);
        while (items_on_buffer == 0) { // buffer is empty
            pthread_cond_wait(&c, &mutex);
        }
        int item = buffer.front();
        if (item == -1){
            pthread_mutex_unlock(&mutex);
            break;
        }
        buffer.pop_front();
        --items_on_buffer;
        printInformations(pthread_self(), flag, item, false);
        if(items_on_buffer < flag->buffer_size)
            pthread_cond_signal(&p);
        pthread_mutex_unlock(&mutex);
    }
    return nullptr;
}

int main(int argc, char** argv) {
    Flags flags;

    for (int it = 1; it < argc; ++it) {
        string flag = argv[it];

        if (flag == "-pt") { // number of producers threads
            flags.producers_threads = atoi(argv[++it]);
        } else if (flag == "-ct") { // number of consumers threads
            flags.consumers_threads = atoi(argv[++it]);
        } else if (flag == "-iq") { // number of items
            flags.items = atoi(argv[++it]);
        } else if (flag == "-bs") { // buffer size
            flags.buffer_size = atoi(argv[++it]);
        } else {
            cout << "Unused Option: " << argv[it];
            cout << "\t" << argv[++it] << endl;
        }
    }

    pthread_t producer[flags.producers_threads];
    pthread_t consumer[flags.consumers_threads];

    buffer.clear();

    // Producers create
    for (int i = 0; i < flags.producers_threads; ++i) {
        pthread_create(&(producer[i]), nullptr, Producer, &(flags));
    }

    // Consumers create
    for (int i = 0; i < flags.consumers_threads; ++i) {
        pthread_create(&(consumer[i]), nullptr, Consumer, &(flags));
    }

    // Producers join
    for (int i = 0; i < flags.producers_threads; ++i) {
        pthread_join(producer[i], nullptr);
    }

    // Insert -1 on buffer to end consumers
    for (int i = 0; i < flags.consumers_threads; ++i) {
        buffer.push_back(-1);
        ++items_on_buffer;
        pthread_cond_signal(&c);
    }

    // Consumers join
    for (int i = 0; i < flags.consumers_threads; ++i) {
        pthread_join(consumer[i], nullptr);
    }

    return 0;
}
