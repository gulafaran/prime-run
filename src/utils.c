/*
 * --------------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <tomenglund26@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return. Tom Englund
 * --------------------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <pci/pci.h>

#include "utils.h"

struct env_struct env_list[] = {
    {"VK_ICD_FILENAMES"},
    {"__VK_LAYER_NV_optimus"},
    {"__GLX_VENDOR_LIBRARY_NAME"},
    {"__NV_PRIME_RENDER_OFFLOAD"},
    {"__NV_PRIME_RENDER_OFFLOAD_PROVIDER"},
    {"__EGL_VENDOR_LIBRARY_FILENAMES"},
    {"DXVK_FILTER_DEVICE_NAME"},
    {"VKD3D_FILTER_DEVICE_NAME"},
    {NULL}
};

bool file_exists(char *path) {
    if(access(path, F_OK ) != -1) {
        return true;
    }

    return false;
}

char *str_combine(const char *str1, const char *str2) {
  char *str = malloc(strlen(str1) + strlen(str2) + 1);
  strcpy(str, str1);
  strcat(str, str2);

  return str;
}

char *get_first_dir_entry(char *path) {
    DIR *folder;
    struct dirent *entry;

    folder = opendir(path);

    if(folder == NULL) {
        fprintf(stderr, "Unable to read directory \n");
        exit(EXIT_FAILURE);
    }

    //dont return . or ..
    for(int i = 0; i < 3; i++) {
        entry=readdir(folder);
    }

    char *dir = malloc(strlen(entry->d_name) + 1);
    strcpy(dir, entry->d_name);

    closedir(folder);

    return dir;
}

void print_systemd_service_status(char *service) {
    FILE *fp;
    char path[500];
    char *cmd = str_combine("/usr/bin/systemctl is-active ", service);

    fp = popen(cmd, "r");
    if (fp == NULL) {
        printf("Failed to run command systemctl.\n");
        exit(EXIT_FAILURE);
    }
    
    fgets(path, sizeof(path), fp);

    printf("%s = %s", service, path);

    free(cmd);
    pclose(fp);
}

void print_env_var(char *env_var) {
    const char *env = getenv(env_var);

    if(env == NULL) {
        printf("%s is unset \n", env_var);
    }
    else {
        printf("%s = %s \n", env_var, env);
    }
}

void print_env_list() {
    for(int i = 0; env_list[i].env != 0; i++) {
        print_env_var(env_list[i].env);
    }
}

void set_env_var(char *env_var, char *value, int overwrite) {
    int ret;

    ret = setenv(env_var, value, overwrite);

    if(ret == -1) {
        fprintf(stderr, "Error while setting environment variable: %s\n", env_var);
        fprintf(stderr, "%s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
}

void print_file(char *file) {
    FILE *fp;
    fp = fopen(file, "r");

    if(!fp) {
        fprintf(stderr, "Error while opening: %s\n", file);
        fprintf(stderr, "%s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    char *line = NULL;
    size_t len = 0;

    while(getline(&line, &len, fp) != -1) {
        printf("%s", line);
    }

    free(line);
    fclose(fp);
}

void write_file(char *file, char *text) {
    FILE *fp;
    fp = fopen(file, "w");

    if(!fp) {
        fprintf(stderr, "Error while opening for writing: %s\n", file);
        fprintf(stderr, "%s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    fprintf(fp, "%s\n", text);
    fclose(fp);
}

void exec_file(const char *file, char *const argv[]) {
    int ret;

    ret = execvp(file, argv);

    if(ret == -1) {
        fprintf(stderr, "Error while trying to run: %s\n", file);
        fprintf(stderr, "%s\n", strerror(errno));
    }
}