/* 
 * V3 Control header file 
 * (c) Jack lange, 2010
 */

#ifndef __V3_IOCTL_H__
#define __V3_IOCTL_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Global Control IOCTLs */
#define V3_ADD_CPU                 100   /* Add a physical CPU for use by Palacios                     */
#define V3_ADD_MEM                 101   /* Add a physical memory region to Palacios memory manager    */
#define V3_ADD_PCI                 102   /* Add a passthrough PCI device for VM assignment             */
				   
#define V3_REMOVE_CPU              105   /* Remove a physical CPU for use by Palacios                  */
#define V3_REMOVE_MEM              106   /* Remove a physical memory region to Palacios memory manager */
#define V3_REMOVE_PCI              107   /* Remove passthrough PCI device for VM assignment            */
				   
#define V3_CREATE_GUEST            112   /* Create a VM from a configuration image                     */
#define V3_FREE_GUEST              113   /* Free a VM and all of its associated state                  */
				   
#define V3_SHUTDOWN                900
				   
				   
/* VM Specific IOCTLs */	   
				   
#define V3_VM_PAUSE                123   /* Pause a running VMs execution                              */
#define V3_VM_CONTINUE             124   /* Continue a running VMs execution                           */
				   
#define V3_VM_LAUNCH               125   /* Launch a previously created VM                             */
#define V3_VM_STOP                 126   /* Stop a running VM                                          */
#define V3_VM_LOAD                 127   /* Load a VM's execution state from a checkpoint              */
#define V3_VM_SAVE                 128   /* Save a VM's execution state to a checkpoint                */
#define V3_VM_SIMULATE             129   /* Cause a VM to enter simulation mode                        */
				   
#define V3_VM_INSPECT              130   /* Request inspection of a VM's state (OBSOLETE)              */
#define V3_VM_DEBUG                131   /* Send a Debug command to a VM                               */
				   
#define V3_VM_MOVE_CORE            133   /* Migrate a VM's VCPU to another physical CPU                */
				   
#define V3_VM_SEND                 134   /* Migration command                                          */
#define V3_VM_RECEIVE              135   /* Migration command                                          */
				   
#define V3_VM_SHUTDOWN             136   /* Shutdown a VM                                              */

#define V3_VM_CONSOLE_CONNECT      140   /* Connect to a VM's text mode console                        */
#define V3_VM_CONSOLE_DISCONNECT   141   /* Disconnect from a VM's text mode console                   */
#define V3_VM_KEYBOARD_EVENT       142   /* Send a scan scode to the VM's virtual keyboard             */
#define V3_VM_STREAM_CONNECT       145   /* Connect to a VM's named data stream                        */
#define V3_VM_CREATE_CHECKPOINT    146   /* Create a checkpoint context                                */

#define PISCES_VM_CONS_CONNECT     200   /* Pisces version of VM Console connect                       */
#define PISCES_VM_CONS_DISCONNECT  201   /* Pisces version of VM Console disconnect                    */


/* Checkpoint IOCTLs */
#define V3_CHKPT_SIZE              5000  /* Query the size of the checkpoint data                      */
#define V3_CHKPT_SAVE              5001  /* Save VM state to a checkpoint                              */
#define V3_CHKPT_LOAD              5002  /* Load VM state from a checkpoint                            */
#define V3_CHKPT_WRITE_OUT         5003  /* Write a checkpoint out to a specified store location       */
#define V3_CHKPT_READ_IN           5004  /* Read a checkpoint in from a specified store location       */
			          
#define V3_VM_XPMEM_CONNECT        12000

#include <stdio.h>

#include "v3_types.h"


#define V3_DEV_FILENAME "/dev/v3vee"
#define V3_VM_FILENAME  "/dev/v3-vm"



static inline char * 
get_vm_dev_path(int vm_id) 
{
    char * dev_path = NULL;
    
    asprintf(&dev_path, "/dev/v3-vm%d", vm_id);

    return dev_path;
}

static inline int
get_vm_id_from_path(char * dev_path)
{
    int vm_id = -1;

    if (sscanf(dev_path, "/dev/v3-vm%d", &vm_id) != 1) {
        return -1;
    }

    return vm_id;
}

struct v3_guest_img {
    u64       size;
    uintptr_t guest_data;
    char      name[128];
} __attribute__((packed));


struct v3_mem_region {
    u64 base_addr;
    u64 num_pages;
} __attribute__((packed));


struct v3_core_move_cmd{
    u16 vcore_id;
    u16 pcore_id;
} __attribute__((packed));


struct v3_debug_cmd {
    u32 core; 
    u32 cmd;
} __attribute__((packed));


#define MAX_CHKPT_STORE_LEN 128
#define MAX_CHKPT_URL_LEN   256

struct v3_chkpt_info {
    char store[MAX_CHKPT_STORE_LEN];
    char url[MAX_CHKPT_URL_LEN];     /* This might need to be bigger... */
} __attribute__((packed));



struct v3_hw_pci_dev {
    char url[128];
    u32  bus;
    u32  dev;
    u32  func;
} __attribute__((packed));


struct v3_hobbes_console_info {
    uintptr_t ring_buf_pa;
    u32       num_pages;
    u32       cons_fd;
} __attribute__((packed));


#ifdef __cplusplus
}
#endif

#endif
