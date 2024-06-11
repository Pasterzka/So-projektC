#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>

void print_file_info(const char* path, const char* filename, struct stat* info) {
    char permissions[11];
    struct passwd* pw;
    struct group* gr;
    char timebuf[80];
    struct tm* timeinfo;

    // typ 
    permissions[0] = (S_ISDIR(info->st_mode)) ? 'd' : '-';
    // dostêpnoœæ user
    permissions[1] = (info->st_mode & S_IRUSR) ? 'r' : '-';
    permissions[2] = (info->st_mode & S_IWUSR) ? 'w' : '-';
    permissions[3] = (info->st_mode & S_IXUSR) ? 'x' : '-';
    // dostêpnoœæ grupa
    permissions[4] = (info->st_mode & S_IRGRP) ? 'r' : '-';
    permissions[5] = (info->st_mode & S_IWGRP) ? 'w' : '-';
    permissions[6] = (info->st_mode & S_IXGRP) ? 'x' : '-';
    // dostêpnoœæ other
    permissions[7] = (info->st_mode & S_IROTH) ? 'r' : '-';
    permissions[8] = (info->st_mode & S_IWOTH) ? 'w' : '-';
    permissions[9] = (info->st_mode & S_IXOTH) ? 'x' : '-';
    permissions[10] = '\0';

    // User i grupa
    pw = getpwuid(info->st_uid);
    gr = getgrgid(info->st_gid);

    // Czas ostatniej modyfikacji
    timeinfo = localtime(&info->st_mtime);
    strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", timeinfo);

    // Wypisanie informacji 
    printf("%s %2ld %s %s %5ld %s %s/%s\n",
        // Dostêp
        permissions,
        // Dowi¹zania twarde
        (long)info->st_nlink,
        // W³aœciciel pliku
        pw->pw_name,
        // Grupa w³aœciciela
        gr->gr_name,
        // Rozmiar pliku
        (long)info->st_size,
        // Data i czas ostatniej modyfikacji
        timebuf,
        // Œcierzka do pliku
        path,
        // Nazwa pliku
        filename);
}

void list_directory(const char* path) {
    struct dirent** namelist;
    int n;

    n = scandir(path, &namelist, NULL, alphasort);
    if (n < 0) {
        perror("scandir");
        return;
    }

    
    // Wyœwietl aktualn¹ œcie¿kê folderu
    printf("\n%s:\n", path);

    // Wyœwietl wszystkie czêœci folderu
    for (int i = 0; i < n; i++) {
        struct stat file_info;
        char fullpath[1024];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, namelist[i]->d_name);
        if (stat(fullpath, &file_info) == 0) {
            print_file_info(path, namelist[i]->d_name, &file_info);
        }
        else {
            perror("stat");
        }
    }

    // Rekurencyjne wyœwietlanie katalogów
    for (int i = 0; i < n; i++) {
        struct stat file_info;
        char fullpath[1024];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, namelist[i]->d_name);
        if (stat(fullpath, &file_info) == 0 && S_ISDIR(file_info.st_mode)) {
            if (strcmp(namelist[i]->d_name, ".") != 0 && strcmp(namelist[i]->d_name, "..") != 0) {
                list_directory(fullpath);
            }
        }
        free(namelist[i]);
    }
    free(namelist);
}

int main() {
    list_directory(".");
    return EXIT_SUCCESS;
}
