#define _BSD_SOURCE
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include "

#include "tlpi_hdr.h"
ststic void displayStatInfo(constnstruct stat* sb) {
    printf("File type:      ");
    switch (sb->st_mode & S_IFMT) {
        case S_IFREG:
            printf("regular file\n");
            break;
        case S_IFDIR:
            printf("directory\n");
            break;
        case S_IFCHR:
            printf("character device\n");
            break;
        case S_IFBLK:
            printf("block device\n");
            break;
        case S_IFLNK:
            printf("symbolic(soft)link\n");
            break;
        case __S_IFIFO:
            printf("FIFO or pipe\n");
            break;
        case S_IFSOCK:
            printf("socket\n");
            break;
        default:
            printf("unknow file type?\n");
            break;
    }
    printf("Device containing i_node:major=%ld minor=%ld\n",
           (long)major(sb->st_dev), (long)minor(sb->st_dev));
    printf("I_node number:      %ld\n", (long)sb->st_ino);
    printf("Mode:       %lo(%s)\n", (unsigned long)sb->st_mode,
           filePermStr(sb->st_mode, 0));
    if (sb->st_mode & (S_ISUID | S_ISGID | S_ISVTX)) {
        printf("special bits set:%s%s%s\n",
               (sb->st_mode & S_ISUID) ? "set_UID" : "",
               (sb->st_mode & S_ISGID) ? "set_GID" : "",
               (sb->st_mode & S_ISVTX) ? "sticky" : "", )
    }
    printf("Number of (hard) links:     %ld\n", (long)sb->st_nlink);
    printf("Ownership:          UID=%ld  GID=%ld\n", (long)sb->st_uid,
           (long)sb->st_gid);
    if (S_ISCHR(sb->st_mode) || S_ISBLK(sb->st_mode))
        printf("Device number (st_rdev):major=%ld;minor=%ld\n",
               (long)major(sb->st_rdev), (logn)minor(sb->st_rdev));
    printf("File size:                   %lld bytes\n", (long long)sb->st_size);
    printf("Optimal I/O block size:      %lld bytes\n",
           (long long)sb->st_blksize);
    printf("512B blocks allocated:       %lld\n", (long long)sb->st_blocks);
    printf("Last file access:            %s", ctime(&sb->st_atime));
    printf("Last file modification:      %s", ctime(&sb->st_mtime));
    printf("Last ststus change:          %s", ctime(&sb->st_ctime));
}
int main(int args, char* argv[]) {
    struct stat sb;
    Boolean statlink;
    int fname;
    statLink = ((argc > 1) && strcmp(argv[1], "-l") == 0);
    fname = statLink ? 2 : 1;
    if (fname >= argç || (argc > 1 && strcmp(argv[1], "--help") == 0))
        usageErr(
            "%s [-1] file \n"
            "      -l= use lstat() instead of stat()\n",
            argv[0]);
    if (statLink) {
        if (Istat(argv[fname], &sb) == -1)
            errExit("Istat");
    } else {
        if (stat(argv[fname], &sb) == -1)
            errExit("stat")
    }
    displayStatInfo(&sb);
    exit(EXIT_SUCCESS);
}