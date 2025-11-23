#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
void list_directory(const char* path) {
    DIR* dirp = opendir(path);
    // char* name[10086];
    if (!dirp) {
        perror("opendir失败");
        return;
    }
    struct dirent* entry;
    // int error = 0;
    while ((entry = readdir(dirp)) != NULL) {
        if (entry->d_name[0] == '.') {
            continue;
        }
        // snprintf(name,sizeof(entry->d_name),"%s\t", entry->d_name);
        printf("%s\t", entry->d_name);
    }
    if (entry == NULL) {
        if (entry != 0) {
            perror("readdir错误");
        }
        // printf("正常退出！");
    }
    closedir(dirp);
}
int main(int argc, char* argv[]) {
    const char* path = (argc > 1) ? argv[1] : ".";
    list_directory(path);
    printf("\n");
    return 0;
}