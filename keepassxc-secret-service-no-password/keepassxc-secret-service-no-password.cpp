#define _POSIX_C_SOURCE 200809L
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/capability.h>

#include <iostream>

constexpr char CRED_PATH[] = "/home/robin/.config/.dbus_keepassxc";
constexpr char DB_PATH[]   = "/home/robin/Dokumente/DBusSecretService.kdbx";
constexpr char EXE_PATH[]  = "/usr/bin/keepassxc";
constexpr char INI_PATH[]  = "/home/robin/.config/keepassxc/keepassxc_dbus.ini";

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
    execl(EXE_PATH, EXE_PATH, "--pw-stdin", "--config", INI_PATH, DB_PATH, NULL);
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
