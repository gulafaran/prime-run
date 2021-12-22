/*
 * --------------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <tomenglund26@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return. Tom Englund
 * --------------------------------------------------------------------------------
 */
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pci/pci.h>

#include "nvidia.h"
#include "utils.h"

static char *get_nvidia_id() {
    if(!file_exists(NV_PROC_DRIVER_PATH)) {
        fprintf(stderr, "%s doesnt exist, is the nvidia driver loaded?\n", NV_PROC_DRIVER_PATH);
        exit(EXIT_FAILURE);
    }

    char *nv_gpus = str_combine(NV_PROC_DRIVER_PATH, "gpus/");
    char *nv_id = get_first_dir_entry(nv_gpus);

    free(nv_gpus);

    return nv_id;
}

static char *get_nvidia_proc_path(char *id) {
    if(!file_exists(NV_PROC_DRIVER_PATH)) {
        fprintf(stderr, "%s doesnt exist, is the nvidia driver loaded?\n", NV_PROC_DRIVER_PATH);
        exit(EXIT_FAILURE);
    }

    char *nv_gpus = str_combine(NV_PROC_DRIVER_PATH, "gpus/");
    char *nv_path = str_combine(nv_gpus, id);
    char *ret = str_combine(nv_path, "/");

    free(nv_gpus);
    free(nv_path);

    return ret;
}

static char *get_nvidia_sys_path(char *id) {
    char *nv_sys = str_combine(SYS_DEVICE_PATH, id);

    if(!file_exists(nv_sys)) {
        fprintf(stderr, "%s doesnt exist, is the nvidia driver loaded?\n", nv_sys);
        free(nv_sys);
        exit(EXIT_FAILURE);
    }

    char *ret = str_combine(nv_sys, "/");

    free(nv_sys);

    return ret;
}

static char *get_nvidia_pm_control(char *sys_path, char *id) {
    char *control = str_combine(sys_path, "power/control");
    
    if(!file_exists(control)) {
        fprintf(stderr, "%s doesnt exist \n", control);
        free(control);
        exit(EXIT_FAILURE);
    }

    return control;
}

void set_nvidia_pm_control(char *pm_control, char *status) {
    if(!file_exists(pm_control)) {
        fprintf(stderr, "%s doesnt exist \n", pm_control);
        exit(EXIT_FAILURE);
    }

    write_file(pm_control, status);
}

static char *get_nvidia_status(char *proc_path) {
    char *status = str_combine(proc_path, "power");

    if(!file_exists(status)) {
        fprintf(stderr, "%s doesnt exist \n", status);
        free(status);
        exit(EXIT_FAILURE);
    }

    return status;
}

void print_nvidia_status(char *status) {
    if(!file_exists(status)) {
        fprintf(stderr, "%s doesnt exist \n", status);
        exit(EXIT_FAILURE);
    }

    print_file(status);
}

static char *get_nvidia_params() {
    char *params = str_combine(NV_PROC_DRIVER_PATH, "params");

    if(!file_exists(params)) {
        fprintf(stderr, "%s doesnt exist \n", params);
        exit(EXIT_FAILURE);
    }

    return params;
}

char *get_nvidia_param_value(char *params, char *param) {
    FILE *fp = fopen(params, "r");

    if(fp == NULL) {
        fprintf(stderr, "cant open file: %s \n", params);
        exit(EXIT_FAILURE);
    }

    char *line = NULL;
    size_t len = 0;
    char *value = NULL;
    char *str = NULL;
    char *return_str = NULL;

    while(getline(&line, &len, fp) != -1) {
        if(str = strstr(line, param)) {
            value = calloc(1, strlen(str) + 1);
            strcpy(value, str);
            char *token = strtok(value, ":");
            if(token != NULL && (token = strtok(NULL, ":")) != NULL) {
                return_str = malloc(strlen(token) + 1);
                strcpy(return_str, token);
                memmove(return_str, return_str+1, strlen(return_str));
            }
            else {
                fprintf(stderr, "cant split string: %s\n", line);
                exit(EXIT_FAILURE);
            }
            free(value);
            break;
        }
    }

    if(return_str != NULL) {
        return_str[strlen(return_str) - 1] = 0;
    }

    fclose(fp);
    free(line);

    return return_str;
}

char *get_nvidia_power_state(char *id) {
    char *nv_path = str_combine(SYS_DEVICE_PATH, id);
    char *nv_status = str_combine(nv_path, "/power/runtime_status");

    if(!file_exists(nv_status)) {
        fprintf(stderr, "%s doesnt exist \n", nv_status);
        free(nv_path);
        free(nv_status);
        exit(EXIT_FAILURE);
    }

    FILE *fp = fopen(nv_status, "r");

    if(fp == NULL) {
        fprintf(stderr, "cant open file: %s \n", nv_status);
        free(nv_path);
        free(nv_status);
        exit(EXIT_FAILURE);
    }

    char *line = NULL;
    size_t len = 0;
    char *return_str = NULL;

    if(getline(&line, &len, fp) != -1) {
        return_str = malloc(strlen(line) + 1);
        strcpy(return_str, line);
    }

    free(nv_path);
    free(nv_status);
    free(line);
    fclose(fp);

    return return_str;
}

void print_nvidia_pm(struct nv_struct *nv_st) {
    char *nv_state = get_nvidia_power_state(nv_st->id);

    printf("NVIDIA Power management is set to: ");
    print_file(nv_st->pm_control);
    printf("NVIDIA card is : %s", nv_state);
    
    free(nv_state);
}

bool print_nvidia_drm_modeset() {
    char *file = "/sys/module/nvidia_drm/parameters/modeset";

    if(!file_exists(file)) {
        return false;
    }

    uid_t euid = geteuid();

    if(euid != 0) {
        return false;
    }

    printf("nvidia_drm modeset = ");
    print_file(file);
    return true;
}

static char *get_nvidia_pci_id() {
    struct pci_access* pciaccess;
    struct pci_dev* dev;

    pciaccess = pci_alloc();
    pci_init(pciaccess);
    pci_scan_bus(pciaccess);

    char pci_id_buf[50];

    for(dev = pciaccess->devices; dev; dev = dev->next) {
        pci_fill_info(dev, PCI_FILL_IDENT | PCI_FILL_BASES | PCI_FILL_CLASS);
        if(dev->device_class == 768 && dev->vendor_id == 4318) {
            //found device class VGA and vendor id NVIDIA
            snprintf(pci_id_buf, sizeof(pci_id_buf), "PCI:%u:%u:%u", dev->bus, dev->dev, dev->func);
            break;
        }
    }

    pci_cleanup(pciaccess);

    char *ret = malloc(sizeof(pci_id_buf));
    strcpy(ret, pci_id_buf);

    return ret;
}

static char *get_internal_pci_id() {
    struct pci_access* pciaccess;
    struct pci_dev* dev;

    pciaccess = pci_alloc();
    pci_init(pciaccess);
    pci_scan_bus(pciaccess);

    char pci_id_buf[50];

    for(dev = pciaccess->devices; dev; dev = dev->next) {
        pci_fill_info(dev, PCI_FILL_IDENT | PCI_FILL_BASES | PCI_FILL_CLASS);
        if(dev->device_class == 768 && dev->vendor_id != 4318) {
            //found device class VGA and vendor id NVIDIA
            snprintf(pci_id_buf, sizeof(pci_id_buf), "PCI:%u:%u:%u", dev->bus, dev->dev, dev->func);
            break;
        }
    }

    pci_cleanup(pciaccess);

    char *ret = malloc(sizeof(pci_id_buf));
    strcpy(ret, pci_id_buf);

    return ret;
}

void print_nvidia_udev_template() {
    printf("# https://download.nvidia.com/XFree86/Linux-x86_64/470.63.01/README/dynamicpowermanagement.html\n\n");
    printf("# Remove NVIDIA USB xHCI Host Controller devices, if present\n");
    printf("ACTION==\"add\", SUBSYSTEM==\"pci\", ATTR{vendor}==\"0x10de\", ATTR{class}==\"0x0c0330\", ATTR{remove}=\"1\"\n");
    printf("# Remove NVIDIA USB Type-C UCSI devices, if present\n");
    printf("ACTION==\"add\", SUBSYSTEM==\"pci\", ATTR{vendor}==\"0x10de\", ATTR{class}==\"0x0c8000\", ATTR{remove}=\"1\"\n");
    printf("# Remove NVIDIA Audio devices, if present\n");
    printf("ACTION==\"add\", SUBSYSTEM==\"pci\", ATTR{vendor}==\"0x10de\", ATTR{class}==\"0x040300\", ATTR{remove}=\"1\"\n\n");
    
    printf("# Enable runtime PM for NVIDIA VGA/3D controller devices on driver bind\n");
    printf("ACTION==\"bind\", SUBSYSTEM==\"pci\", ATTR{vendor}==\"0x10de\", ATTR{class}==\"0x030000\", TEST==\"power/control\", ATTR{power/control}=\"auto\"\n");
    printf("ACTION==\"bind\", SUBSYSTEM==\"pci\", ATTR{vendor}==\"0x10de\", ATTR{class}==\"0x030200\", TEST==\"power/control\", ATTR{power/control}=\"auto\"\n\n");

    printf("# Disable runtime PM for NVIDIA VGA/3D controller devices on driver unbind\n");
    printf("ACTION==\"unbind\", SUBSYSTEM==\"pci\", ATTR{vendor}==\"0x10de\", ATTR{class}==\"0x030000\", TEST==\"power/control\", ATTR{power/control}=\"on\"\n");
    printf("ACTION==\"unbind\", SUBSYSTEM==\"pci\", ATTR{vendor}==\"0x10de\", ATTR{class}==\"0x030200\", TEST==\"power/control\", ATTR{power/control}=\"on\"\n");
}

void print_nvidia_xorg_template(struct nv_struct *nv_st) {
    printf("Section \"ServerFlags\"\n");
    printf("  Option \"AutoAddGPU\" \"false\"\n");
    printf("  Option \"AutoBindGPU\" \"false\"\n");
    printf("EndSection\n\n");

    printf("Section \"ServerLayout\"\n");
    printf("  Identifier \"default\"\n");
    printf("  Screen 0 \"internal\"\n");
    printf("  Inactive \"nvidia\"\n");
    printf("  Option \"AllowNVIDIAGPUScreens\"\n");
    printf("EndSection\n\n");

    printf("Section \"Device\"\n");
    printf("  Identifier \"internal\"\n");
    printf("  Driver \"modesetting\"\n");
    printf("  BusID \"%s\"\n", nv_st->internal_pci_id);
    printf("EndSection\n\n");

    printf("Section \"Device\"\n");
    printf("  Identifier \"nvidia\"\n");
    printf("  Driver \"nvidia\"\n");
    printf("  BusID \"%s\"\n", nv_st->pci_id); //detect?
    printf("  Option \"ProbeAllGpus\" \"false\"\n");
    printf("EndSection\n\n");

    printf("Section \"Screen\"\n");
    printf("  Identifier \"internal\"\n");
    printf("  Device     \"internal\"\n");
    printf("EndSection\n");

}

void set_nvidia_env_vars() {
    set_env_var("VK_ICD_FILENAMES", "/usr/share/vulkan/icd.d/nvidia_icd.json", 1);
    set_env_var("__VK_LAYER_NV_optimus", "NVIDIA_only", 1);
    set_env_var("__GLX_VENDOR_LIBRARY_NAME", "nvidia", 1);
    set_env_var("__NV_PRIME_RENDER_OFFLOAD", "1", 1);
    set_env_var("__NV_PRIME_RENDER_OFFLOAD_PROVIDER", "NVIDIA-G0", 1);
    set_env_var("__EGL_VENDOR_LIBRARY_FILENAMES", "/usr/share/glvnd/egl_vendor.d/10_nvidia.json", 1);
    set_env_var("DXVK_FILTER_DEVICE_NAME", "NVIDIA", 1);
    set_env_var("VKD3D_FILTER_DEVICE_NAME", "NVIDIA", 1);
}

struct nv_struct *init_nv_struct() {
    struct nv_struct *nv_st = malloc(sizeof(struct nv_struct));

    nv_st->id = get_nvidia_id();
    nv_st->proc_path = get_nvidia_proc_path(nv_st->id);
    nv_st->sys_path = get_nvidia_sys_path(nv_st->id);
    nv_st->pm_control = get_nvidia_pm_control(nv_st->sys_path, nv_st->id);
    nv_st->status = get_nvidia_status(nv_st->proc_path);
    nv_st->params = get_nvidia_params();
    nv_st->pci_id = get_nvidia_pci_id();
    nv_st->internal_pci_id = get_internal_pci_id();

    return nv_st;
}

void free_nv_struct(struct nv_struct *nv_st) {
    free(nv_st->id);
    free(nv_st->proc_path);
    free(nv_st->sys_path);
    free(nv_st->pm_control);
    free(nv_st->status);
    free(nv_st->params);
    free(nv_st->pci_id);
    free(nv_st->internal_pci_id);
    free(nv_st);
}