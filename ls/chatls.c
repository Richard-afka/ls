#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define PATH_MAX 4096
#define STR_SIZE 11  // 权限字符串的长度：rwxr-xr-x

// 获取文件所有者的名称
char* get_uid(struct stat* sb) {
    struct passwd* pwd = getpwuid(sb->st_uid);
    return (pwd != NULL) ? pwd->pw_name : "Unknown";
}

// 获取文件所属组的名称
char* get_gid(struct stat* sb) {
    struct group* grp = getgrgid(sb->st_gid);
    return (grp != NULL) ? grp->gr_name : "Unknown";
}

// 格式化文件权限字符串
char* str(mode_t perm) {
    static char str[STR_SIZE];
    snprintf(str, STR_SIZE, "%c%c%c%c%c%c%c%c%c", (perm & S_IRUSR) ? 'r' : '-',
             (perm & S_IWUSR) ? 'w' : '-', (perm & S_IXUSR) ? 'x' : '-',
             (perm & S_IRGRP) ? 'r' : '-', (perm & S_IWGRP) ? 'w' : '-',
             (perm & S_IXGRP) ? 'x' : '-', (perm & S_IROTH) ? 'r' : '-',
             (perm & S_IWOTH) ? 'w' : '-', (perm & S_IXOTH) ? 'x' : '-');
    return str;
}

// 过滤函数，去除隐藏文件
int filter(const struct dirent* entry) {
    return (entry->d_name[0] != '.');  // 过滤掉以 '.' 开头的文件
}

void list_l(char* path) {
    struct stat* sb = (struct stat*)malloc(sizeof(struct stat));
    int m;
    int n = 0;
    struct dirent** namelist;
    char pathname[PATH_MAX];
    if ((m = scandir(path, &namelist, filter, NULL)) == -1) {
        printf("扫描出错！\n");
        return;
    }
    for (int i = 0; i < m; i++) {
        printf("进for\n");
        snprintf(pathname, PATH_MAX, "%s/%s", path, namelist[i]->d_name);
        if ((n = lstat(pathname, sb)) == -1) {
            perror("获取信息失败！\n");
            continue;
        }
        char* strb = str(sb->st_mode);
        char* time_str = ctime(&sb->st_mtime);
        time_str[strlen(time_str) - 1] = '\0';
        printf("%s %ld %s %s %s %s\n", strb, (long)sb->st_nlink, get_uid(sb),
               get_gid(sb), time_str, namelist[i]->d_name);
        free(namelist[i]);
    }
    printf("\n");
    free(namelist);
    free(sb);
}
int main() {
    list_l(".");  // 假设列出当前目录
    return 0;
}
