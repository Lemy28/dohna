#include <core/SignalManager.h> 

Signal::Signal(int signo, const char* signame, std::function<void(int, siginfo_t*, void*)> handler)
    : signo_(signo), signame_(signame), handler_(handler)
{
    struct sigaction sa;
    sa.sa_sigaction = [](int signo, siginfo_t* siginfo, void* ucontext) {
        Signal* signal = reinterpret_cast<Signal*>(siginfo->si_value.sival_ptr);
        signal->HandleSignal(signo, siginfo, ucontext);
    };
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO;
    sigaction(signo, &sa, nullptr);
}

int Signal::getSigno() const {
    return signo_;
}

const char* Signal::getSigname() const {
    return signame_;
}

void Signal::HandleSignal(int signo, siginfo_t* siginfo, void* ucontext) {
    handler_(signo, siginfo, ucontext);
}

SignalManager::SignalManager() {}

void SignalManager::RegisterSignal(const Signal& signal) {
    signals_[signal.getSigno()] = signal;
}

void SignalManager::RegisterSignal(int signo, const char* signame, std::function<void(int, siginfo_t*, void*)> handler) {
    Signal signal(signo, signame, handler);
    RegisterSignal(signal);
}

void SignalManager::HandleSignals() {
    sigset_t set;
    sigemptyset(&set);
    for (const auto& pair : signals_) {
        sigaddset(&set, pair.first);
    }
    while (true) {
        int signo;
        siginfo_t siginfo;
        sigwaitinfo(&set, &siginfo);
        signo = siginfo.si_signo;
        auto it = signals_.find(signo);
        if (it != signals_.end()) {
            Signal& signal = it->second;
            signal.HandleSignal(signo, &siginfo, nullptr); //没有传递额外参数
            }
    }
}
