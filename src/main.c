/*
 * --------------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <tomenglund26@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return. Tom Englund
 * --------------------------------------------------------------------------------
 */

#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "utils.h"
#include "nvidia.h"
#include "opengl.h"

void print_help(bool quit) {
    printf("To run an application on the NVIDIA gpu.\n");
    printf("Use: prime-run <application>\n\n");
    printf("To run various options prime-run handles.\n");
    printf("Use: prime-run [options]\n\n");
    printf(" Options:\n");
    printf("  -g , --opengl                 run a opengl test to see if NVIDIA gets used prints card to stdout.\n");
    printf("  -x , --xorg                   print an template xorg.conf.d conf to stdout\n");
    printf("  -u , --udev                   print an template udev rule to disable NVIDIA PCI devices to stdout\n");
    printf("  -c , --check                  print various nvidia powermanagement/prime related services and module options.\n");
    printf("  -e , --envvars                print PRIME associated environment vars.\n");
    printf("  -p , --powermanagement [arg]  print or set (ON/OFF/AUTO) nvidia powermanagement status.\n");
    printf("  -s , --status                 print nvidia status file.\n");
    printf("  -h , --help                   print this help and exit.\n");

    if(quit) {
        exit(EXIT_SUCCESS);
    }
}

int main(int argc, char **argv) {
    struct option long_opt[] = {
        {"opengl", no_argument, 0, 'g'},
        {"xorg", no_argument, 0, 'x'},
        {"udev", no_argument, 0, 'u'},
        {"check", no_argument, 0, 'c'},
        {"envvars", no_argument, 0, 'e'},
        {"powermanagement", optional_argument, 0, 'p'},
        {"status", no_argument, 0, 's'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    int option_index = 0;
    int opt;

    if(argc == 1) {
        print_help(true);
    }

    //set nvidia environment variables
    set_nvidia_env_vars();

    //assume we want to run application instead of parsing argv so we can send used args to application.
    if(argc > 2) {
        if(strstr(argv[1], "-") == NULL && strstr(argv[1], "--") == NULL) {
            opt = -1;
            goto run_application;
        }
    }

    struct nv_struct *nv_st = init_nv_struct();

    //because getopt_long modifies argv..
    char** new_argv = malloc((argc + 1) * sizeof *new_argv);

    for(int i = 0; i < argc; ++i) {
        size_t length = strlen(argv[i]) + 1;
        new_argv[i] = malloc(length);
        memcpy(new_argv[i], argv[i], length);
    }
    new_argv[argc] = NULL;

    while((opt = getopt_long(argc, new_argv, "gxucepsh::", long_opt, &option_index)) != -1) {
        if(opt == 'g') {
            gl_main(argc, argv);
            break;
        }
        else if(opt == 'x') {
            print_nvidia_xorg_template(nv_st);
            break;
        }
        else if(opt == 'u') {
            print_nvidia_udev_template();
            break;
        }
        else if(opt == 'c') {
            nvidia_check_options(nv_st);
            break;
        }
        else if(opt == 'e') {
            print_env_list();
            break;
        }
        else if(opt == 'p') {
            if(optarg == NULL && optind < argc) {
                optarg = argv[optind++];
            }
            if(optarg == NULL) {
                print_nvidia_pm(nv_st);
            }
            else {
                set_nvidia_pm_control(nv_st->pm_control, optarg);
            }
            break;
        }
        else if(opt == 's') {
            print_nvidia_status(nv_st->status);
            break;
        }
        else if(opt == 'h') {
            print_help(false);
            break;
        }
    }

    free_nv_struct(nv_st);
    for(int i = 0; i < argc; i++) {
        free(new_argv[i]);
    }
    free(new_argv);

run_application:
    if(argc >= 2 && opt == -1) {
        //assume arg 2 is application and rest is arguments to said application
        char **app_argv = argv + 1;

        exec_file(app_argv[0], app_argv);
    }

    return EXIT_SUCCESS;
}