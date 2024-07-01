#ifndef SWAYBG_RANDOM_BACKGROUND_MANAGER_H
#define SWAYBG_RANDOM_BACKGROUND_MANAGER_H

#include <vector>

#include "picture_manager.h"
#include "wayland/output.h"

class output {
    pid_t m_swaybg_pid {-1};
    const picture *m_picture {};
    uint32_t m_id;
    std::string m_name;

    void spawn_swaybg();

public:
    explicit output(uint32_t id, std::string name) : m_id{id}, m_name{std::move(name)} { }
    ~output();

    output(const output&) = delete;

    output(output&& other) noexcept : m_swaybg_pid{other.m_swaybg_pid}, m_picture{other.m_picture}, m_id{other.m_id}, m_name{other.m_name} {
        other.m_swaybg_pid = -1;
        other.m_picture = nullptr;
    }
    output& operator=(output&& other) noexcept {
        m_swaybg_pid = other.m_swaybg_pid;
        m_picture = other.m_picture;
        m_id = other.m_id;
        m_name = std::move(other.m_name);

        other.m_swaybg_pid = -1;
        other.m_picture = nullptr;
        other.m_id = 0;

        return *this;
    }

    void set_background(const picture& pic);
    void on_child_died(int32_t status);

    [[nodiscard]] constexpr const pid_t &child_pid() const noexcept {
        return m_swaybg_pid;
    }
    [[nodiscard]] constexpr const uint32_t &id() const noexcept {
        return m_id;
    }
    [[nodiscard]] constexpr const std::string &name() const noexcept {
        return m_name;
    }
};

class background_manager {
    std::vector<output> m_outputs {};
    std::vector<output>::iterator m_end_pos {m_outputs.end()};
    std::vector<output>::iterator m_current_pos {m_outputs.begin()};
    picture_manager m_picture_manager;

    const picture& get_next_picture();

public:
    using iterator = std::vector<output>::iterator;

    explicit background_manager(picture_manager pictures) : m_picture_manager{std::move(pictures)} {}

    void on_timer_expired();

    void add_output(output output);
    void remove_output(uint32_t id);

    iterator begin() {
        return m_outputs.begin();
    }
    iterator end() {
        return m_outputs.end();
    }
};

#endif //SWAYBG_RANDOM_BACKGROUND_MANAGER_H
