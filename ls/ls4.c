#include <dirent.h>  //包含struct dirent结构体类型和alphasort函数
#include <errno.h>
#include <grp.h>  //包含struct group,getgrgid
#include <pwd.h>  //包含struct paswd,getpwgid
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>   //struct stat
#include <sys/types.h>  //struct stat,getpwgid
#include <time.h>  //包含ctime函数->它将一个 time_t 值（通常表示自1970年1月1日以来的秒数）转换为一个人类可读的日期和时间字符串
#include <unistd.h>  //包含getopt函数,struct stat
#define STR_SIZE sizeof("rwxrwxrwx")
#define PATH_MAX 4096
#define COLOR_RESET "\033[0m"
#define COLOR_RED "\033[31m"
#define COLOR_GREEN "\033[32m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_BLUE "\033[34m"
#define COLOR_CYAN "\033[36m"
#define COLOR_WHITE "\033[37m"
// struct parametric{
//     int flag_a = 0;//列出所有文件
//     int flag_l = 0;//列出详细信息
//     int flag_R = 0;//列出目录中子目录下的所有文件
//     int flag_t = 0;//按修改时间由近到远排序
//     int flag_r = 0;//以首字母ASCLL码从大到小排序
//     int flag_i = 0;//输出i节点的索引信息
//     int flag_s = 0;//在文件名前面输出文件大小
// };
// struct stat {
//     dev_t st_dev;          // 文件所在设备的ID
//     ino_t st_ino;          // 文件的inode号
//     mode_t st_mode;        // 文件的类型和权限
//     nlink_t st_nlink;      // 文件的硬链接数
//     uid_t st_uid;          // 文件所有者的用户ID
//     gid_t st_gid;          // 文件所有者的组ID
//     dev_t st_rdev;         // 如果是设备文件，表示设备ID
//     off_t st_size;         // 文件的大小（字节）
//     blksize_t st_blksize;  // 文件系统I/O的最小块大小
//     blkcnt_t st_blocks;    // 文件占用的块数
//     time_t st_atime;       // 文件最后访问时间
//     time_t st_mtime;       // 文件最后修改时间
//     time_t st_ctime;       // 文件的元数据最后修改时间
// };
void list_directory(char* path);
void list_name(char* path);
void list_l(char* path);
void list_R(char* path);
int list_r(const struct dirent** a, const struct dirent** b);
int list_t(const struct dirent** a, const struct dirent** b);
void list_i(char* path);
void list_s(char* path);
int filter(const struct dirent* entry);
char* str(mode_t perm);
char* get_uid(struct stat* sb);
char* get_gid(struct stat* sb);
char type(struct stat* sb);
int nature_sort(const struct dirent** a, const struct dirent** b);
const char* get_cwd(const struct dirent** a);
char* get_color(mode_t mode);
int flag_a = 0, flag_l = 0, flag_R = 0, flag_t = 0, flag_r = 0, flag_i = 0,
    flag_s = 0;
int main(int argc, char* argv[]) {
    int opt;
    while ((opt = getopt(argc, argv, "alRtris")) != -1) {
        switch (opt) {
            case 'a':
                flag_a = 1;
                break;
            case 'l':
                flag_l = 1;
                break;
            case 'R':
                flag_R = 1;
                break;
            case 't':
                flag_t = 1;
                break;
            case 'r':
                flag_r = 1;
                break;
            case 'i':
                flag_i = 1;
                break;
            case 's':
                flag_s = 1;
                break;
            default:
                printf("请输入正确参数\n");
                break;
        }
    }
    char** path = (char**)malloc(10 * sizeof(char*));
    int j = 0;
    if (argv[argc - 1][0] == '-' || argc == 1) {
        path[j++] = ".";
    } else {
        for (int i = 1; i < argc; i++) {
            if (argv[i][0] == '-')
                continue;
            path[j++] = argv[i];
        }
    }
    for (int i = 0; i < j; i++) {
        if(flag_R){
        list_R(path[i]);
    }else{ 
        if (j>1){
            printf("%s:\n", path[i]);
        }
        list_directory(path[i]);
    }
    }
    free(path);
}
void list_name(char* path) {
    int n = 0;
    int m;
    struct dirent** namelist;
    char pathname[PATH_MAX];
    struct stat* sb = (struct stat*)malloc(sizeof(struct stat));
    if (flag_t) {
        if ((n = scandir(path, &namelist, filter, list_t)) == -1) {
            printf("扫描出错！\n");
            return;
        }
    }else if(flag_r){
        if ((n =scandir(path, &namelist, filter, list_r)) == -1) {
            printf("扫描出错！\n");
            return;
        }
    } else {
        if ((n = scandir(path, &namelist, filter, nature_sort)) == -1) {
            printf("扫描出错！\n");
            return;
        }
    }
    for (int i = 0; i < n; i++) {
        snprintf(pathname, PATH_MAX, "%s/%s", path, namelist[i]->d_name);
        if ((m = stat(pathname, sb)) == -1) {
            perror("获取信息失败！\n");
            continue;
        }
        char* color = get_color(sb->st_mode);
        printf("%s%s\033[0m\n", color, namelist[i]->d_name);
        free(namelist[i]);
    }
    free(namelist);
    free(sb);
}
void list_directory(char* path) {
    if (flag_l) {
        list_l(path);
    } else if (flag_i && flag_s) {
        list_i(path);
    } else if (flag_s) {
        list_s(path);
    } else if (flag_i) {
        list_i(path);
    }
    if (!flag_i && !flag_l && !flag_s) {
        list_name(path);
    }
}
int filter(const struct dirent* entry) {
    if (flag_a == 0 &&
        (entry->d_name[0] == '.' || strcmp(entry->d_name, "..") == 0)) {
        return 0;
    }
    return 1;
}
void list_l(char* path) {
    long count = 0;
    struct stat* sb = (struct stat*)malloc(sizeof(struct stat));
    int m;
    int n = 0;
    struct dirent** namelist;
    char pathname[PATH_MAX];
    if (flag_t) {
        if ((m = scandir(path, &namelist, filter, list_t)) == -1) {
            printf("扫描出错！\n");
            return;
        }
    } else if (flag_r) {
        if ((m = scandir(path, &namelist, filter, list_r)) == -1) {
            printf("扫描出错！\n");
            return;
        }
    } else {
        if ((m = scandir(path, &namelist, filter, nature_sort)) == -1) {
            printf("扫描出错！\n");
            return;
        }
    }
    for (int i = 0; i < m; i++) {
        snprintf(pathname, PATH_MAX, "%s/%s", path, namelist[i]->d_name);
        if ((n = stat(pathname, sb)) == -1) {
            perror("获取信息失败！\n");
            continue;
        }
    }
    for (int i = 0; i < m; i++) {
        snprintf(pathname, PATH_MAX, "%s/%s", path, namelist[i]->d_name);
        if ((n = lstat(pathname, sb)) == -1) {
            perror("获取信息失败！\n");
            continue;
        }
        char* strb = str(sb->st_mode);
        char* time_str = ctime(&sb->st_mtime);
        time_str[strlen(time_str) - 1] = '\0';
        char c = type(sb);
        if (flag_i && flag_s) {
            printf("%-8ld %2ld %c%s %2ld %s %s%6ld %s ", namelist[i]->d_ino,
                   (long)sb->st_blocks, c, strb, (long)sb->st_nlink,
                   get_uid(sb), get_gid(sb), (long)sb->st_size, time_str);
            char* color = get_color(sb->st_mode);
            printf("%s%s\033[0m\n", color, namelist[i]->d_name);
        } else if (flag_i && !flag_s) {
            printf("%-8ld%c%s %2ld %s %s%6ld %s ", namelist[i]->d_ino, c,
                   strb, (long)sb->st_nlink, get_uid(sb), get_gid(sb),
                   (long)sb->st_size, time_str);
            char* color = get_color(sb->st_mode);
            printf("%s%s\033[0m\n", color, namelist[i]->d_name);
        } else if(flag_s&&!flag_i){
            printf("192");
            printf("%-2ld %c%s %2ld %s %s%6ld %s ", (long)sb->st_blocks, c,
                   strb, (long)sb->st_nlink, get_uid(sb), get_gid(sb),
                   (long)sb->st_size, time_str);
            char* color = get_color(sb->st_mode);
            printf("%s%s\033[0m\n", color, namelist[i]->d_name);
        }else {
            printf("%c%s %2ld %s %s%6ld %s ", c, strb, (long)sb->st_nlink,
                   get_uid(sb), get_gid(sb), (long)sb->st_size, time_str
                );
            char* color = get_color(sb->st_mode);
            printf("%s%s\033[0m\n", color, namelist[i]->d_name);
        }
        free(namelist[i]);
    }
    free(namelist);
    free(sb);
}
char* str(mode_t perm) {
    static char str[STR_SIZE];
    snprintf(str, STR_SIZE, "%c%c%c%c%c%c%c%c%c", (perm & S_IRUSR) ? 'r' : '-',
             (perm & S_IWUSR) ? 'w' : '-', (perm & S_IXUSR) ? 'x' : '-',
             (perm & S_IRGRP) ? 'r' : '-', (perm & S_IWGRP) ? 'w' : '-',
             (perm & S_IXGRP) ? 'x' : '-', (perm & S_IROTH) ? 'r' : '-',
             (perm & S_IWOTH) ? 'w' : '-', (perm & S_IXOTH) ? 'x' : '-');
    return str;
}
char* get_uid(struct stat* sb) {
    struct passwd* pwd = getpwuid(sb->st_uid);
    return pwd->pw_name;
}
char* get_gid(struct stat* sb) {
    struct group* pwd = getgrgid(sb->st_gid);
    return pwd->gr_name;
}

void list_i(char* path) {
    long count = 0;
    struct stat* sb = (struct stat*)malloc(sizeof(struct stat));
    int m;
    int n = 0;
    struct dirent** namelist;
    char pathname[PATH_MAX];
    if (flag_t) {
        if ((m = scandir(path, &namelist, filter, list_t)) == -1) {
            printf("扫描出错！\n");
            return;
        }
    } else if (flag_r) {
        if ((m = scandir(path, &namelist, filter, list_r)) == -1) {
            printf("扫描出错！\n");
            return;
        }
    } else {
        if ((m = scandir(path, &namelist, filter, nature_sort)) == -1) {
            printf("扫描出错！\n");
            return;
        }
    }
    // if (flag_s) {
    //     for (int i = 0; i < m; i++) {
    //         snprintf(pathname, PATH_MAX, "%s/%s", path, namelist[i]->d_name);
    //         if ((n = stat(pathname, sb)) == -1) {
    //             perror("获取信息失败！\n");
    //             continue;
    //         }
    //     }
    // }
    for (int i = 0; i < m; i++) {
        //for (int i = 0; i < m; i++) {
            snprintf(pathname, PATH_MAX, "%s/%s", path, namelist[i]->d_name);
            if ((n = stat(pathname, sb)) == -1) {
                perror("获取信息失败！\n");
                continue;
            }
       // }
        if (flag_s) {
            printf("%-8ld %2ld ", (long)namelist[i]->d_ino,
                   (long)sb->st_blocks);
            char* color = get_color(sb->st_mode);
            printf("%s%s\033[0m\n", color, namelist[i]->d_name);
        } else {
            printf("%-8ld ", (long)namelist[i]->d_ino);
            char* color = get_color(sb->st_mode);
            //printf("color=%s\n", color);
            printf("%s%s\033[0m\n", color, namelist[i]->d_name);
        }

        free(namelist[i]);
    }
    free(namelist);
    free(sb);
}
char type(struct stat* sb) {
    char c;
    if (S_ISREG(sb->st_mode)) {
        c = '-';
    } else if (S_ISDIR(sb->st_mode)) {
        c = 'd';
    } else if (S_ISLNK(sb->st_mode)) {
        c = 'l';
    } else if (S_ISCHR(sb->st_mode)) {
        c = 'c';
    } else if (S_ISBLK(sb->st_mode)) {
        c = 'b';
    } else if (S_ISFIFO(sb->st_mode)) {
        c = 'p';
    } else if (S_ISSOCK(sb->st_mode)) {
        c = 's';
    } else {
        printf("文件类型未知\n");
        return 0;
    }
    return c;
}
void list_s(char* path) {
    struct stat* sb = (struct stat*)malloc(sizeof(struct stat));
    int m;
    int n = 0;
    struct dirent** namelist;
    char pathname[PATH_MAX];
    if (flag_t) {
        if ((m = scandir(path, &namelist, filter, list_t)) == -1) {
            printf("扫描出错！\n");
            return;
        }
    } else if (flag_r) {
        if ((m = scandir(path, &namelist, filter, list_r)) == -1) {
            printf("扫描出错！\n");
            return;
        }
    } else {
        if ((m = scandir(path, &namelist, filter, nature_sort)) == -1) {
            printf("扫描出错！\n");
            return;
        }
    }
    // for (int i = 0; i < m; i++) {
    //     snprintf(pathname, PATH_MAX, "%s/%s", path, namelist[i]->d_name);
    //     if ((n = stat(pathname, sb)) == -1) {
    //         perror("获取信息失败！\n");
    //         continue;
    //     }
    // }
    for (int i = 0; i < m; i++) {
        snprintf(pathname, PATH_MAX, "%s/%s", path, namelist[i]->d_name);
        if ((n = stat(pathname, sb)) == -1) {
            perror("获取信息失败！\n");
            continue;
        }
        printf("%-2ld ", (long)sb->st_blocks);
        char* color = get_color(sb->st_mode);
        printf("%s%s\033[0m\n", color, namelist[i]->d_name);
        free(namelist[i]);
    }
        free(namelist);
        free(sb);
}
int list_r(const struct dirent** a, const struct dirent** b) {
    return strcmp((*b)->d_name, (*a)->d_name);
}
int nature_sort(const struct dirent** a, const struct dirent** b) {
    return -(strcmp((*b)->d_name, (*a)->d_name));
}
int list_t(const struct dirent** a, const struct dirent** b) {
    char path_a[PATH_MAX], path_b[PATH_MAX];
    if (getcwd(path_a, sizeof(path_a)) == NULL) {
        perror("getcwd failed");
        return 0;
    }
    strcat(path_a, "/");
    strcat(path_a, (*a)->d_name);
    if (getcwd(path_b, sizeof(path_b)) == NULL) {
        perror("getcwd failed");
        return 0;
    }
    strcat(path_b, "/");
    strcat(path_b, (*b)->d_name);
    int n_a = 0, n_b = 0;
    struct stat* sb_a = (struct stat*)malloc(sizeof(struct stat));
    struct stat* sb_b = (struct stat*)malloc(sizeof(struct stat));
    int result = 0;
    if ((n_a = stat(path_a, sb_a)) == -1) {
        perror("获取信息失败！\n");
    }
    if ((n_b = stat(path_b, sb_b)) == -1) {
        perror("获取信息失败！\n");
    }
    if (sb_a->st_mtime > sb_b->st_mtime) {
        result=-1;
    } else if (sb_a->st_mtime < sb_b->st_mtime) {
        result=1;
    }
    if (flag_r) {
        result=-result;
    }
    free(sb_a);
    free(sb_b);
    return result;
}
char* get_color(mode_t mode) {
   // printf("------");
    char* color = COLOR_WHITE;
    if (S_ISDIR(mode)) {
        color=COLOR_BLUE;
       // printf("==================");
    } else if (S_ISLNK(mode)) {
        color = COLOR_CYAN;
    } else if (S_ISFIFO(mode)) {
        color=COLOR_YELLOW;
    } else if (mode & S_IXUSR) {
        color=COLOR_GREEN;
    }
    return color;
}
void list_R(char* path) {
    list_directory(path);
    printf("\n");
    DIR* dirp = opendir(path);
    char pathname[PATH_MAX];
    int n;
    if (!dirp) {
        perror("opendir失败");
        return;
    }
    struct dirent* entry;
    while ((entry = readdir(dirp)) != NULL) {
    struct stat* sb = (struct stat*)malloc(sizeof(struct stat));
    snprintf(pathname, PATH_MAX, "%s/%s", path, entry->d_name);
    
    if ((n = lstat(pathname, sb)) == -1) {
       // printf("433\n");
        perror("获取信息失败！\n");
    }
    char c = type(sb);
   // printf("%c\n", c);

    if (c == 'd') {
        //printf("%s:\n", pathname);
        // if (flag_a == 0 &&
        //     (entry->d_name[0] == '.' || strcmp(entry->d_name, "..") == 0)) {
        //     break;
        // }
        if ((strcmp(entry->d_name, ".") == 0 ||
             strcmp(entry->d_name, "..") == 0)) {
            continue;
        }
        if(!flag_a&&entry->d_name[0]=='.'){
            continue;
        }
        if (entry->d_name != "." && entry->d_name != "..") {
            printf("%s:\n", pathname);
            list_R(pathname);
        }
        }
        //printf("%s\t", entry->d_name);
        //snprintf(pathname, PATH_MAX, "%s/%s", path, namelist->d_name);
        free(sb);
    }
    if (entry == NULL) {
        if (entry != 0) {
            perror("readdir错误");
        }
    }
    closedir(dirp);
    
}