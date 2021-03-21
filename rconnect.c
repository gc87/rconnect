#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/resource.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include "rconnect.h"

#define LOCKMODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

int lock_file(int fd)
{
    struct flock fl;

    fl.l_type = F_WRLCK;
    fl.l_start = 0;
    fl.l_whence = SEEK_SET;
    fl.l_len = 0;
    return (fcntl(fd, F_SETLK, &fl));
}

int already_running(char *path)
{
    int fd;
    char buf[16] = {'\0'};

    fd = open(path, O_RDWR | O_CREAT, LOCKMODE);
    if (0 > fd) {
        exit(1);
    }

    int ret = lock_file(fd);

    if (0 > ret) {
        if (EACCES == errno || EAGAIN == errno) {
            close(fd);
            return (1);
        }
        exit(1);
    }

    ret = ftruncate(fd, 0);
    sprintf(buf, "%ld", (long)getpid());
    ret = write(fd, buf, strlen(buf) + 1);
    return (0);
}

void daemonize(const char *cmd)
{
    int i, fd0, fd1, fd2;
    pid_t pid;
    struct rlimit rl;
    struct sigaction sa;

    umask(0);

    if (0 > getrlimit(RLIMIT_NOFILE, &rl)) {
    }

    if (0 > (pid = fork())) {
    } else if (0 != pid) {
        exit(0);
    }

    setsid();

    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (0 > sigaction(SIGHUP, &sa, NULL)) {
    }
    if (0 > (pid = fork())) {
    } else if (0 != pid) {
        exit(0);
    }

    if (0 > chdir("/")) {
    }

    if (RLIM_INFINITY == rl.rlim_max) {
        rl.rlim_max = 1024;
    }

    for (i = 0; i < rl.rlim_max; i++) {
        close(i);
    }

    fd0 = open("/dev/null", O_RDWR);
    fd1 = dup(0);
    fd2 = dup(0);

    if (0 != fd0 || 1 != fd1 || 2 != fd2) {
        exit(1);
    }
}

int main(int argc, char **argv)
{
    // work dir
    char workdir[1024 + 1] = {'\0'};
    char *home = getenv("HOME");
    snprintf(workdir, 1024 + 1, "%s/%s", home, WORK_DIR);

    struct option long_options[] = {
        {"version", no_argument, NULL, 'v'},
        {"workdir", required_argument, NULL, 'w'},
        {"daemon", no_argument, NULL, 'd'},
        {"port", no_argument, NULL, 'p'},
        {0, 0, 0, 0},
    };

    int is_daemon = 0;
    int port = 8097;
    int option_index = 0;
    int c = 0;
    while ((c = getopt_long(argc, argv, "vw:dp:", long_options, &option_index)) != -1) {
        switch (c) {
            case 'v':
                printf("version: %d.%d.%d\n", VERSION_MAJOR, VERSION_MINOR, VERSION_PATH);
                exit(0);
                break;
            case 'w':
                memset(workdir, 0x00, sizeof(workdir));
                strncpy(workdir, optarg, sizeof(workdir) - 1);
                break;
            case 'd':
                is_daemon = 1;
                break;
            case 'p':
                port = atoi(optarg);
                break;

            default:
                break;
        }
    }

    mkdir(workdir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    int ret = chdir(workdir);
    char *cwd = getcwd(workdir, sizeof(workdir));

    // create node dir and data dir
    mkdir(NODE_DIR, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    mkdir(DATA_DIR, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    mkdir(ALL_LOG_DIR, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    // log init
    char log_dir[PATH_MAX + 1] = {'\0'};
    snprintf(log_dir, PATH_MAX + 1, "%s/%s/%s", workdir, ALL_LOG_DIR, LOG_FILE);
    FILE *log_fd = fopen(log_dir, "a");

    printf("[main]main node:%d, pipe buf:%d, work dir:%s\n", getpid(), PIPE_BUF, workdir);

    // daemon
    if (1 == is_daemon) {
        daemonize("server");
    }

    // single process
    char lock_path[PATH_MAX + 1] = {'\0'};
    snprintf(lock_path, PATH_MAX, "%s/%s", workdir, LOCK_FILE);
    if (0 != already_running(lock_path)) {
        exit(1);
    }

    signal(SIGPIPE, SIG_IGN); /*Do not handle SIGPIPE*/

    exit(0);
}
