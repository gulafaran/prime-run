/*
 * --------------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <tomenglund26@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return. Tom Englund
 * --------------------------------------------------------------------------------
 */

#pragma once

#include <stdint.h>
#include <inttypes.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
#define PCI_U64_FMT_X PRIx64

//https://pci-ids.ucw.cz/ 
#define VGA_CLASS 768
#define VENDOR_NVIDIA 4318
#define VENDOR_INTEL 32902
#define VENDOR_AMD 4098

struct pci_struct {
    u16 vendor_id;
    u16 device_class;
    char *busid;
    char *pci_id;
};

struct pci_struct *init_pci_struct(u16 vendor_id);
void free_pci_struct(struct pci_struct *pci_st);