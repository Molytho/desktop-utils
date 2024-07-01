#include "background_manager.h"

#include <system_error>
#include <csignal>
#include <sys/eventfd.h>
#include <cstring>
#include <cassert>
#include <algorithm>

namespace {
    constexpr std::string swaybg_name = "swaybg";

    [[noreturn]] void spawn_child(owning_fd event_fd, int picture_fd, char *output_name) {
        if(dup2(picture_fd, 0) == -1) {
            perror("Error while dupping fd to stdin");
            exit(EXIT_FAILURE);
        }

        sigset_t sigset{};
        sigfillset(&sigset);
        sigprocmask(SIG_UNBLOCK, &sigset, nullptr);

        const std::array<char*, 6> argv = {
            strdup(swaybg_name.c_str()),
            strdup("-o"),
            output_name,
            strdup("-i"),
            strdup("/dev/stdin"),
            nullptr
        };

        eventfd_t buffer;
        eventfd_read(event_fd, &buffer);
        execvp(swaybg_name.c_str(), argv.data());
        perror("execvp failed:");
        exit(EXIT_FAILURE);
    }

    void kill_child(pid_t pid) {
        kill(pid, SIGTERM);
    }
}

output::~output() {
    if (m_swaybg_pid > 0) {
        //TODO: Maybe child_handler again?
        kill_child(m_swaybg_pid);
    }
}

void output::spawn_swaybg() {
    assert(m_picture);

    owning_fd event_fd {eventfd(0, EFD_CLOEXEC)};

    pid_t pid = fork();
    if (pid < 0) {
        throw std::system_error(errno, std::system_category());
    } else if (pid > 0) {
        m_swaybg_pid = pid;
        eventfd_write(event_fd, 1);
    } else {
        spawn_child(std::move(event_fd), *m_picture, strdup(m_name.c_str()));
    }
}

void output::set_background(const picture &pic) {
    m_picture = &pic;
    pid_t old_pid = m_swaybg_pid;
    spawn_swaybg();
    sleep(1);
    if (old_pid > 0) {
        kill_child(old_pid);
    }
}

void output::on_child_died(int32_t) {
    m_swaybg_pid = -1;
    spawn_swaybg();
}


const picture &background_manager::get_next_picture() {
    const picture &next_picture = m_picture_manager.get();
    m_picture_manager.next();
    return next_picture;
}

void background_manager::add_output(output output) {
    m_outputs.push_back(std::move(output));

    m_outputs.back().set_background(get_next_picture());
}

void background_manager::remove_output(uint32_t id) {
    auto pos = std::find_if(std::begin(m_outputs), std::end(m_outputs), [&id](const output& output) {
        return output.id() == id;
    });
    if (pos == std::end(m_outputs)) {
        return;
    }

    if (pos < m_current_pos) {
        --m_current_pos;
        --m_end_pos;
    }
    m_outputs.erase(pos);
}

void background_manager::on_timer_expired() {
    if (m_outputs.empty()) {
        return;
    }

    if (m_current_pos == m_end_pos) {
        m_current_pos = m_outputs.begin();
        m_end_pos = m_outputs.end();
        assert(m_current_pos != m_end_pos);
    }


    m_current_pos->set_background(get_next_picture());
    ++m_current_pos;
}