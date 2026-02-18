#define _POSIX_C_SOURCE 200809L
#include <fcntl.h>
#include <stdlib.h>
#include <sys/capability.h>
#include <unistd.h>

#include <iostream>

#define KEY_FILE      ".dbus_keepassxc"
#define DATABASE_FILE "DBusSecretService.kdbx"
#define INI_FILE      "keepassxc_dbus.ini"
#define BASE_PATH     "/home/robin/"

constexpr char EXE_PATH[]   = "/usr/bin/keepassxc";
constexpr char CRED_PATH[]  = BASE_PATH ".config/" KEY_FILE;
constexpr char INI_PATH[]   = BASE_PATH ".config/keepassxc/" INI_FILE;
constexpr char CACHE_PATH[] = BASE_PATH ".cache/keepassxc/" INI_FILE;
constexpr char DB_PATH[]    = BASE_PATH "Dokumente/" DATABASE_FILE;

void open_cred_file() {
    int cred_fd = open(CRED_PATH, O_RDONLY | O_CLOEXEC | O_NOFOLLOW);
    if (cred_fd < 0) {
        perror("open_cred_file open");
        exit(1);
    }

    int ret = dup2(cred_fd, 0);
    if (ret < 0) {
        perror("open_cred_file dup2");
        exit(2);
    }
}

[[noreturn]] void spawn_keepass() {
    execl(EXE_PATH, EXE_PATH, "--pw-stdin", "--config", INI_PATH, "--localconfig", CACHE_PATH, DB_PATH, NULL);
    perror("spawn_keepass");
    exit(3);
}

int main(int argc, char **argv) {
    if (argc != 1) {
        std::cout << argv[0] << " takes no arguments.\n";
        return 1;
    }

    open_cred_file();
    spawn_keepass();

    return 255;
}
