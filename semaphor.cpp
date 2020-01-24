#include "const.h"
#include "kersem.h"
#include "semaphor.h"

Semaphore::Semaphore(int init) {
    __lock__
    myImpl = new KernelSem(init);
    __unlock__
}

Semaphore::~Semaphore() {
	__lock__
    delete myImpl;
    __unlock__
}

int Semaphore::wait(Time maxTimeToWait) {
    return myImpl->wait(maxTimeToWait);
}

int Semaphore::signal(int n) {
    return myImpl->signal(n);
}

int Semaphore::val() const {
    return myImpl->value();
}

