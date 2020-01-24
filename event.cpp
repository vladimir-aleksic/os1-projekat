#include "event.h"
#include "const.h"
#include "kernelev.h"
#include "thread.h"

Event::Event(IVTNo ivtNo) {
	__lock__
	myImpl = new KernelEv(ivtNo);
	__unlock__
}

Event::~Event () {
	__lock__
	delete myImpl;
	__unlock__
}

void Event::wait () {
	myImpl->wait();
}

void Event::signal() {
	myImpl->signal();
}
