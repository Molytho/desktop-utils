#include "../include/child_handler.h"
#include <signal.h>
#include <cassert>

class ChildHandler child_handler;

static void handle_sigchld(int signo, siginfo_t* info, void* context) {
    child_handler.on_terminated(info->si_pid);
}

void ChildHandler::init() {
    struct sigaction sigterm_action;
    sigterm_action.sa_flags = SA_NOCLDSTOP|SA_NOCLDWAIT|SA_SIGINFO;
    sigemptyset(&sigterm_action.sa_mask);
    sigterm_action.sa_sigaction = handle_sigchld;

    assert (!sigaction(SIGCHLD, &sigterm_action, nullptr));
}

void ChildHandler::register_child(int pid, Output &output) {
    children.emplace(pid, output);
}

void ChildHandler::shutdown_child(int pid) {
    shuting_down.insert(pid);
    kill(pid, SIGTERM);
    children.erase(pid);
}

void ChildHandler::on_terminated(int pid) {
    if (shuting_down.contains(pid)) {
        shuting_down.erase(pid);
    } else {
        Output& output = children.at(pid);
        output.on_swaybg_died(pid);
    }
}