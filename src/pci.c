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

#include "utils.h"
#include "pci.h"

struct pci_struct *init_pci_struct(u16 vendor_id) {
    struct pci_struct *pci_st = NULL;

    struct pci_access *pciaccess;
    struct pci_dev *dev;

    pciaccess = pci_alloc();
    pci_init(pciaccess);
    pci_scan_bus(pciaccess);

    char pci_id_buf[50];

    for(dev = pciaccess->devices; dev; dev = dev->next) {
        pci_fill_info(dev, PCI_FILL_IDENT | PCI_FILL_BASES | PCI_FILL_CLASS);
        if(dev->device_class == VGA_CLASS && dev->vendor_id == vendor_id) {
            //found device class VGA and vendor id
            pci_st = malloc(sizeof(struct pci_struct));
            snprintf(pci_id_buf, sizeof(pci_id_buf), "PCI:%u:%u:%u", dev->bus, dev->dev, dev->func);
            pci_st->vendor_id = dev->vendor_id;
            pci_st->device_class = dev->device_class;
            pci_st->busid = malloc(sizeof(pci_id_buf));
            strcpy(pci_st->busid, pci_id_buf);
            pci_st->pci_id = "#TODO"; // #TODO hardcode for now.
            break;
        }
    }

    pci_cleanup(pciaccess);

    return pci_st;
}

void free_pci_struct(struct pci_struct *pci_st) {
    free(pci_st->busid);
    //free(pci_st->pci_id);
    free(pci_st);
}