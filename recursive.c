#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>

void list_dir(const char *path) {
    DIR *dir;
    struct dirent *entry;

    if ((dir = opendir(path)) == NULL) {
        perror("opendir");
        return;
    }

    printf("\nDirectory: %s\n", path);

    while ((entry = readdir(dir)) != NULL) {
        // 현재 디렉토리와 부모 디렉토리는 생략
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        printf("%s\n", entry->d_name);

        // 디렉토리일 경우 재귀적으로 탐색
        if (entry->d_type == DT_DIR) {
            char subpath[1024];
            snprintf(subpath, sizeof(subpath), "%s/%s", path, entry->d_name);
            list_dir(subpath);
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <directory_path>\n", argv[0]);
        return 1;
    }

    list_dir(argv[1]);

    return 0;
}
