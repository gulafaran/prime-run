/*
 * --------------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <tomenglund26@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return. Tom Englund
 * --------------------------------------------------------------------------------
 */

#pragma once

#include <stdbool.h>

struct env_struct {
    char *env;
};

bool file_exists(char *path);
char *str_combine(const char *str1, const char *str2);
char *get_first_dir_entry(char *path);
void print_systemd_service_status(char *service);
void print_env_var(char *env_var);
void print_env_list();
void set_env_var(char *env_var, char *value, int overwrite);
void print_file(char *file);
void write_file(char *file, char *text);
void exec_file(const char *file, char *const argv[]);