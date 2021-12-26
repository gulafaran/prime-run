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
#include "pci.h"

//example /proc/driver/nvidia/gpus/0000:01:00.0/power
#define NV_PROC_DRIVER_PATH "/proc/driver/nvidia/"

//example /sys/bus/pci/devices/0000:01:00.0/power/control
#define SYS_DEVICE_PATH "/sys/bus/pci/devices/"

struct nv_struct {
    char *id;
    char *proc_path;
    char *sys_path;
    char *pm_control;
    char *status;
    char *params;

    struct pci_struct *pci_nv;
    struct pci_struct *pci_internal;
};

void set_nvidia_pm_control(char *pm_control, char *status);
void print_nvidia_status(char *status);
char *get_nvidia_param_value(char *params, char *param);
char *get_nvidia_power_state(char *id);
void print_nvidia_pm(struct nv_struct *nv_st);
bool print_nvidia_drm_modeset();
void print_nvidia_udev_template();
void print_nvidia_xorg_template(struct nv_struct *nv_st);
void set_nvidia_env_vars();
struct nv_struct *init_nv_struct();
void free_nv_struct(struct nv_struct *nv_st);