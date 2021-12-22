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

//https://download.nvidia.com/XFree86/Linux-x86_64/470.63.01/README/dynamicpowermanagement.html
//https://download.nvidia.com/XFree86/Linux-x86_64/470.63.01/README/powermanagement.html
//https://download.nvidia.com/XFree86/Linux-x86_64/470.63.01/README/nvidia-persistenced.html
//https://download.nvidia.com/XFree86/Linux-x86_64/470.63.01/README/randr14.html
//udev rules
/*
    Function 0: VGA controller / 3D controller
    Function 1: Audio device
    Function 2: USB xHCI Host controller
    Function 3: USB Type-C UCSI controller
    https://download.nvidia.com/XFree86/Linux-x86_64/435.17/README/dynamicpowermanagement.html
    
    /sys/bus/pci/devices/0000:01:00.0/
    /sys/bus/pci/devices/0000:01:00.1/
    /sys/bus/pci/devices/0000:01:00.2/
    /sys/bus/pci/devices/0000:01:00.3/

    Out of the four PCI functions, the NVIDIA driver directly manages the VGA controller / 3D Controller PCI function.
    Other PCI functions are managed by the device drivers provided with the Linux kernel.
    And may interfere with power management.
*/

void check_options(struct nv_struct *nv_st) {
    print_systemd_service_status("nvidia-persistenced.service");

    if(!print_nvidia_drm_modeset()) {
        printf("nvidia-drm modeset=1 is required to get PRIME synchronisation working.\n");
        printf("    sudo cat /sys/module/nvidia_drm/parameters/modeset either returns Y or N.\n");
        printf("    or run this again as root.\n\n");
    }

    char *nv_dyn_pm = get_nvidia_param_value(nv_st->params, "DynamicPowerManagement");
    char *nv_pm_tresh = get_nvidia_param_value(nv_st->params, "DynamicPowerManagementVideoMemoryThreshold");
    char *nv_s0ix = get_nvidia_param_value(nv_st->params, "EnableS0ixPowerManagement");
    char *nv_s0ix_tresh = get_nvidia_param_value(nv_st->params, "S0ixPowerManagementVideoMemoryThreshold");
    char *nv_pre_mem = get_nvidia_param_value(nv_st->params, "PreserveVideoMemoryAllocations");
    
    printf("NVreg_DynamicPowerManagement = %s \n", nv_dyn_pm);
    printf("NVreg_DynamicPowerManagementVideoMemoryThreshold = %s \n", nv_pm_tresh);
    printf("NVreg_EnableS0ixPowerManagement = %s \n", nv_s0ix);
    printf("NVreg_S0ixPowerManagementVideoMemoryThreshold = %s \n", nv_s0ix_tresh);
    printf("NVreg_PreserveVideoMemoryAllocations = %s \n\n", nv_pre_mem);

    free(nv_dyn_pm);
    free(nv_pm_tresh);
    free(nv_s0ix);
    free(nv_s0ix_tresh);
    free(nv_pre_mem);

    printf("PCI functions that may interfere with powermanagement if they exist.\n");

    char *nv_path = str_combine(SYS_DEVICE_PATH, nv_st->id);

    nv_path[strlen(nv_path) - 1] = '1';
    printf("1: Audio device\n");
    if(file_exists(nv_path)) {
        printf("%s , exists.\n\n", nv_path);
    }
    else {
        printf("%s , doesnt exist.\n\n", nv_path);
    }

    nv_path[strlen(nv_path) - 1] = '2';
    printf("2: USB xHCI Host controller\n");
    if(file_exists(nv_path)) {
        printf("%s , exists.\n\n", nv_path);
    }
    else {
        printf("%s , doesnt exist.\n\n", nv_path);
    }

    nv_path[strlen(nv_path) - 1] = '3';
    printf("3: USB Type-C UCSI controller\n");
    if(file_exists(nv_path)) {
        printf("%s , exists.\n", nv_path);
    }
    else {
        printf("%s , doesnt exist.\n", nv_path);
    }

    free(nv_path);
}

void print_help() {
    printf("To run an application on the NVIDIA gpu.\n");
    printf("Use: prime-run <application>\n\n");
    printf("To run various options prime-run handles.\n");
    printf("Use: prime-run [options]\n\n");
    printf(" Options:\n");
    printf("  -g , --opengl              run a opengl test to see if NVIDIA gets used prints card to stdout.\n");
    printf("  -x , --xorg                print an template xorg.conf.d conf to stdout\n");
    printf("  -u , --udev                print an template udev rule to disable NVIDIA PCI devices to stdout\n");
    printf("  -c , --check               print various nvidia powermanagement/prime related services and module options.\n");
    printf("  -e , --envvars             print PRIME associated environment vars.\n");
    printf("  -p , --powermanagement     print nvidia powermanagement status.\n");
    printf("  -s , --status              print nvidia status file.\n");
    printf("  -h , --help                print this help and exit.\n");
}

int main(int argc, char **argv) {
    struct option long_opt[] = {
        {"opengl", no_argument, 0, 'g'},
        {"xorg", no_argument, 0, 'x'},
        {"udev", no_argument, 0, 'u'},
        {"check", no_argument, 0, 'c'},
        {"envvars", no_argument, 0, 'e'},
        {"powermanagement", no_argument, 0, 'p'},
        {"status", no_argument, 0, 's'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    int option_index = 0;
    int opt;

    if(argc == 1) {
        print_help();
        exit(EXIT_SUCCESS);
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

    while((opt = getopt_long(argc, new_argv, ":gxucepsh", long_opt, &option_index)) != -1) {
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
            check_options(nv_st);
            break;
        }
        else if(opt == 'e') {
            print_env_list();
            break;
        }
        else if(opt == 'p') {
            print_nvidia_pm(nv_st);
            break;
        }
        else if(opt == 's') {
            print_nvidia_status(nv_st->status);
            break;
        }
        else if(opt == 'h') {
            print_help();
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