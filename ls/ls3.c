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
// struct parametric{
//     int flag_a = 0;//列出所有文件
//     int flag_l = 0;//列出详细信息
//     int flag_R = 0;//列出目录中子目录下的所有文件
//     int flag_t = 0;//按修改时间由近到远排序
//     int flag_r = 0;//以首字母ASCLL码从大到小排序
//     int flag_i = 0;//输出i节点的索引信息
//     int flag_s = 0;//在文件名前面输出文件大小
// };
// struct dirent{
//     ino_t d_ino;目录项的i节点
//     char* d_name；//目录项的名称
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
void list_t(char* path);
void list_r(char* path);
void list_i(char* path);
void list_s(char* path);
int filter(const struct dirent* entry);
char* str(mode_t perm);
char* get_uid(struct stat* sb);
char* get_gid(struct stat* sb);
char type(struct stat* sb);
// int optind;
// char* optarg;
// struct file{
//     pathname;
//     name;
//     time;
// };
int flag_a = 0, flag_l = 0, flag_R = 0, flag_t = 0, flag_r = 0, flag_i = 0,
    flag_s = 0;
int main(int argc, char* argv[]) {
    // int flag_a = 0,flag_l = 0,flag_R = 0,flag_t = 0,flag_r = 0,flag_i
    // =0,flag_s = 0;
    int opt;
    while ((opt = getopt(argc, argv, "alRtris")) != -1) {
        // printf("11\n");
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
    // printf("%d\n", flag_l);
    char** path = (char**)malloc(10 * sizeof(char*));
    int j = 0;
    if (argv[argc - 1][0] == '-' || argc == 1) {
        path[j++] = ".";
    } else {
        for (int i = 1; i < argc; i++) {
            // int j = 0;
            if (argv[i][0] == '-')
                continue;
            path[j++] = argv[i];
        }
    }
    for (int i = 0; i < j; i++) {
        printf("%s:\n", path[i]);
        list_directory(path[i]);
    }
    free(path);
    // printf("\n");
    //  void list_directory(path);
}
void list_name(char* path) {
    int n = 0;
    struct dirent** namelist;
    // if (flag_a == 0 && path[0] == '.') {
    //     return;
    // }
    // (struct dirent**)malloc(len * sizeof(struct dirent*));
    if ((n = scandir(path, &namelist, filter, NULL)) == -1) {
        printf("扫描出错！\n");
        return;
    }
    for (int i = 0; i < n; i++) {
        // printf("1\n");
        printf("%s  ", namelist[i]->d_name);
        free(namelist[i]);
    }
    printf("\n");
    free(namelist);
}
void list_directory(char* path) {
    while (*path != '\0') {
        // if(flag_a==0&&*path=='.'){
        // continue;
        // path++;
        // }
        if (flag_l && flag_i&&flag_s) {
            list_l(path);
        } else if (flag_i&&flag_s) {
            list_i(path);
        } 
        else if (flag_s) {
            list_s(path);
        }else if (flag_l) {
            //printf("140\n");
            list_l(path);
        }
        if (flag_R) {
            list_R(path);
        }
        if (!flag_i && !flag_l && !flag_R && !flag_s) {
            list_name(path);
        }
        if (flag_t && flag_r) {
            list_t(path);
            list_t(path);
        } else if (flag_t) {
            list_t(path);
        } else if (flag_r) {
            list_r(path);
        }
        path++;
    }
}
int filter(const struct dirent* entry) {
    if (flag_a == 0 && entry->d_name[0] == '.') {
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
    if ((m = scandir(path, &namelist, filter, NULL)) == -1) {
        printf("扫描出错！\n");
        return;
    }
    for (int i = 0; i < m;i++){
        snprintf(pathname, PATH_MAX, "%s/%s", path, namelist[i]->d_name);
        if ((n = stat(pathname, sb)) == -1) {
            perror("获取信息失败！\n");
            continue;
        }
        count +=(long) (sb->st_blocks);
       // free(namelist[i]);
    }
    long cout = count / 1024;
    printf("%ld %ld\n", count, cout);
    printf("总计 %ld\n", count);
    for (int i = 0; i < m; i++) {
        // printf("进for\n");
        snprintf(pathname, PATH_MAX, "%s/%s", path, namelist[i]->d_name);
        if ((n = lstat(pathname, sb)) == -1) {
            perror("获取信息失败！\n");
            continue;
        }
        char* strb = str(sb->st_mode);
        char* time_str = ctime(&sb->st_mtime);
        time_str[strlen(time_str) - 1] = '\0';
        char c = type(sb);
        if (flag_i&&flag_s) {
            // printf("192");
            printf("%2ld %-8ld%c%s %2ld %s %s%6ld %s %s\n", (long)sb->st_blocks,
                   namelist[i]->d_ino, c, strb, (long)sb->st_nlink, get_uid(sb),
                   get_gid(sb), (long)sb->st_size, time_str,
                   namelist[i]->d_name);
        }
        else if(flag_i && !flag_s) {
            // printf("192");
            printf("%-8ld%c%s %2ld %s %s%6ld %s %s\n", namelist[i]->d_ino, c,
                   strb, (long)sb->st_nlink, get_uid(sb), get_gid(sb),
                   (long)sb->st_size, time_str, namelist[i]->d_name);
        }
        else {
            printf("%c%s %2ld %s %s %s %s\n", c,strb, (long)sb->st_nlink,
                   get_uid(sb), get_gid(sb), time_str, namelist[i]->d_name);
        }
        free(namelist[i]);
    }
    //printf("\n");
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
    if ((m = scandir(path, &namelist, filter, NULL)) == -1) {
        printf("扫描出错！\n");
        return;
    }
    if(flag_s){
            for (int i = 0; i < m; i++) {
                snprintf(pathname, PATH_MAX, "%s/%s", path,
                         namelist[i]->d_name);
                if ((n = stat(pathname, sb)) == -1) {
                    perror("获取信息失败！\n");
                    continue;
                }
                count += (long)(sb->st_blocks);
                // free(namelist[i]);
            }
            long cout = count / 1024;
            printf("%ld %ld\n", count, cout);
            printf("总计 %ld\n", count);
        }
    for (int i = 0; i < m; i++) {
        if(flag_s){
            printf("%2ld%-8ld%-7s  ",(long)sb->st_blocks, (long)namelist[i]->d_ino,
                   namelist[i]->d_name);
        }
        else{
            printf("%-8ld%-7s  ",(long)namelist[i]-> d_ino,namelist[i]->d_name);
        }
    
        free(namelist[i]);
        if(i%5==4){
            printf("\n");
        }
    }
   // printf("\n");
    free(namelist);
}
char type(struct stat* sb) {
    char c;
    if (S_ISREG(sb->st_mode)) {
        c='-';
    } else if (S_ISDIR(sb->st_mode)) {
        c='d';
    } else if (S_ISLNK(sb->st_mode)) {
       c='l';
    } else if (S_ISCHR(sb->st_mode)) {
       c='c';
    } else if (S_ISBLK(sb->st_mode)) {
       c='b';
    } else if (S_ISFIFO(sb->st_mode)) {
       c='p';
    } else if (S_ISSOCK(sb->st_mode)) {
       c='s';
    } else {
        printf("文件类型未知\n");
        return 0;
    }
    return c;
}
void list_s(char* path) {
    long count = 0;
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
        snprintf(pathname, PATH_MAX, "%s/%s", path, namelist[i]->d_name);
        if ((n = stat(pathname, sb)) == -1) {
            perror("获取信息失败！\n");
            continue;
        }
        count += (long)(sb->st_blocks);
        // free(namelist[i]);
    }
    long cout = count / 1024;
    printf("%ld %ld\n", count, cout);
    printf("总计 %ld\n", count);
    for (int i = 0; i < m; i++) {
        // printf("1\n");
        // if(flag_l){
        //     printf("%-8ld", (long)namelist[i]->d_ino);
        // }
        printf("%-2ld %-7s", (long)sb->st_blocks, namelist[i]->d_name);
        free(namelist[i]);
        if (i % 5 == 4) {
            printf("\n");
        }
    }
   // printf("\n");
    free(namelist);
}
void list_t(char* path) {
    // struct file** b = directory(path);
    // struct file** p=b;
    // int i = 0;
    // while (*p != '\0') {
    //     p++;
    //     i++;
    // }
    //     for (int j = 0; j < i - 1; j++) {
    //         for (int k = j; k < i - j - 1; k++) {
    //             if (b[k]->time < b[k + 1]->time) {
    //                 struct file* temp = (struct file*)malloc(sizeof(struct file));
    //                 temp->time = b[k]->time;
    //                 b[k]->time = b[k + 1]->time;
    //                 b[k]->time = temp->time;
    //                 free(temp);
    //             }
    //         }
    //     }

}
struct file** directory(char* path){
    // int m;
    // int n = 0;
    // struct dirent** namelist;
    // //struct file** b;
    // char pathname[PATH_MAX];
    // int i = 0;
    // struct stat* sb = (struct stat*)malloc(sizeof(struct stat));
    // if ((m = scandir(path, &namelist, filter, NULL)) == -1) {
    //     printf("扫描出错！\n");
    //     return;
    // }
    // struct file** b = (struct file*)malloc(m * sizeof(struct file));
    // for (i = 0; i < m; i++) {
    //     snprintf(pathname, PATH_MAX, "%s/%s", path, namelist[i]->d_name);
    //     if ((n = stat(pathname, sb)) == -1) {
    //         perror("获取信息失败！\n");
    //         continue;
    //     }
    //     b[i]->pathname= pathname;
    //     b[i]->time = sb->st_mtime;
    //     b[i]->name = namelist[i]->d_name;
    // }
    // return b;
}
void list_r(char* path) {}
void list_R(char* path) {}