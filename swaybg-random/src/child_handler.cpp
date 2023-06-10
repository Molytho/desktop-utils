#include "../include/child_handler.h"
#include <wait.h>
#include <signal.h>
#include <cassert>

class child_handler child_handler;

static void handle_sigchld(int signo, siginfo_t* info, void* context) {
    child_handler.terminated(info->si_pid);
}

void child_handler::init() {
    struct sigaction sigterm_action;
    sigterm_action.sa_flags = SA_NOCLDSTOP|SA_NOCLDWAIT|SA_SIGINFO;
    sigemptyset(&sigterm_action.sa_mask);
    sigterm_action.sa_sigaction = handle_sigchld;

    assert (!sigaction(SIGCHLD, &sigterm_action, nullptr));
}

void child_handler::register_child(int pid, Output &output) {
    children.insert({pid, output});
}

void child_handler::shutdown_child(int pid) {
    shuting_down.insert(pid);
    kill(pid, SIGTERM);
}

void child_handler::terminated(int pid) {
    if (shuting_down.contains(pid)) {
        shuting_down.erase(pid);
    } else {
        Output& output = children.at(pid);
        output.on_swaybg_died(pid);
    }
}