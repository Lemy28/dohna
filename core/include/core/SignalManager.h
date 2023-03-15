#pragma once

#include <csignal>
#include <map>
#include <functional>

class Signal {
public:
    Signal(int signo = 0, const char* signame = nullptr, std::function<void(int, siginfo_t*, void*)> handler = nullptr);
    int getSigno() const;
    const char* getSigname() const;
    void HandleSignal(int signo, siginfo_t* siginfo, void* ucontext);
private:
    int signo_;
    const char* signame_;
    std::function<void(int, siginfo_t*, void*)> handler_;
};

class SignalManager {
public:
    SignalManager();
    void RegisterSignal(const Signal& signal);
    void RegisterSignal(int signo, const char* signame, std::function<void(int, siginfo_t*, void*)> handler);
    void HandleSignals();
private:
    std::map<int, Signal> signals_;
};
