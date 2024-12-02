#include <sys/stat.h>
#include <unistd.h>

int main() {
    mkdir("new_dir", 0755);
    return 0;
}
