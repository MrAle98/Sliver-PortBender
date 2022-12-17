#include "pch.h"
#include "PortBenderWrapper.h"


PortBenderWrapper::PortBenderWrapper(PortBender _portbender) : portbender(_portbender) {

}

PortBenderWrapper::~PortBenderWrapper() {
}

PortBenderWrapper::PortBenderWrapper(const PortBenderWrapper& source) : portbender(source.portbender) {

}

void PortBenderWrapper::start() {
    this->thread_ptr = std::make_unique<std::thread>(&PortBender::Start, &this->portbender);
}

void PortBenderWrapper::stop() {
    this->portbender.Stop();
    this->thread_ptr->join();
    this->thread_ptr.reset();
}

std::tuple<UINT16, UINT16, OperatingMode, std::string> PortBenderWrapper::getData() {
    return this->portbender.getData();
}
