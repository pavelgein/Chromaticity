#include "singlethread_exectuer.h"

void TSingleThreadExecuter::Add(std::unique_ptr<ITask>&& task) {
    task->Do();
}

void TSingleThreadExecuter::Stop() {

}

size_t TSingleThreadExecuter::Size() {
    return 0;
}

