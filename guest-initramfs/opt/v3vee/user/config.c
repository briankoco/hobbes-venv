/*
 * Copyright (c) 2016, Jack Lange <jacklange@cs.pitt.edu>
 * All rights reserved.
 *
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "V3VEE_LICENSE".
 */

#include <string.h>

#include <pet_xml.h>
#include <ezxml.h>  /* TODO: Remove EZXML dependencies and go through pet_xml API */

#include "config.h"

#define SCHEDULE_HZ "100"


pet_xml_t 
v3_create_base_config(int  mem_size,
		      int  num_cpus)
{
    pet_xml_t    root         = NULL;
    pet_xml_t    pic          = NULL;
    pet_xml_t    apic         = NULL;
    pet_xml_t    keyboard     = NULL;
    pet_xml_t    pit          = NULL;
    pet_xml_t    ioapic       = NULL;
    pet_xml_t    pci          = NULL;
    pet_xml_t    northbridge  = NULL;
    pet_xml_t    southbridge  = NULL; 
    pet_xml_t    nvram        = NULL;
    pet_xml_t    bochs_dbg    = NULL;
    pet_xml_t    os_dbg       = NULL;
    
    root = pet_xml_new_tree("vm");
    pet_xml_add_val(root, "class", "PC");
    
    
    v3_set_mem_size(root,  mem_size);
    
    
    /* sets the paging to the defaults */
    {
	pet_xml_t page = NULL;
	
	page = pet_xml_new_tree("paging");
	
	pet_xml_add_val(page, "mode", "nested");
	pet_xml_add_val(page, "large_pages", "true");
	
	ezxml_insert(page, root, 0); 
    }
    
    
    pet_xml_add_val(root, "schedule_hz", SCHEDULE_HZ);
    
    
    v3_set_num_cpus(root,  num_cpus  );
    
    
    pet_xml_add_subtree_tail(root, "devices");
    
    
    
    pic         = v3_make_device("PIC"         , "8259A"       );
    apic        = v3_make_device("apic"        , "LAPIC"       );
    keyboard    = v3_make_device("keyboard"    , "KEYBOARD"    );
    pit         = v3_make_device("PIT"         , "8254_PIT"    );
    ioapic      = v3_make_device("ioapic"      , "IOAPIC"      );
    pci         = v3_make_device("pci0"        , "PCI"         );
    northbridge = v3_make_device("northbridge" , "i440FX"      );
    southbridge = v3_make_device("southbridge" , "PIIX4"       );
    nvram       = v3_make_device("nvram"       , "NVRAM"       );
    bochs_dbg   = v3_make_device("bochs-dbg"   , "BOCHS_DEBUG" );
    os_dbg      = v3_make_device("os_debug"    , "OS_DEBUG"    );
    
    
    pet_xml_add_val(ioapic      , "apic"    , "apic");
    pet_xml_add_val(northbridge , "bus"     , "pci0");
    pet_xml_add_val(southbridge , "bus"     , "pci0");
    pet_xml_add_val(nvram       , "storage" , "ide" );
    
    

    v3_add_device( root , os_dbg      );
    v3_add_device( root , bochs_dbg   );
    v3_add_device( root , pic         );
    v3_add_device( root , keyboard    );
    v3_add_device( root , pit         );    
    v3_add_device( root , apic        );
    v3_add_device( root , ioapic      );
    v3_add_device( root , pci         );
    v3_add_device( root , northbridge );
    v3_add_device( root , southbridge );
    
    /* Add IDE device with special tags */
    {
	pet_xml_t ide = NULL;
	
	ide = v3_make_device("ide", "IDE");
	
	pet_xml_add_val(ide , "bus"        , "pci0"       );
	pet_xml_add_val(ide , "controller" , "southbridge");
	
	v3_add_device(root, ide);
	
    }
    
    v3_add_device( root , nvram);
    
    return root;
}


pet_xml_t
v3_add_device(pet_xml_t root,
	      pet_xml_t device)
{
    pet_xml_t devices = NULL;

    devices = pet_xml_get_subtree(root, "devices");
    ezxml_insert(device, devices, 0);
  
  
    return root;
}


static pet_xml_t
add_ide(pet_xml_t   root,
	char      * file_path,
	char      * id,
	uint8_t     writable,
	uint8_t     bus_num,
	uint8_t     drive_num,
	uint8_t     is_cdrom)
{
    pet_xml_t dev      = NULL;
    pet_xml_t frontend = NULL;

    char * bus_str = NULL;
    char * drv_str = NULL;

    dev = v3_make_device( id, "FILEDISK");

    frontend = pet_xml_add_subtree_tail(dev, "frontend");
  
    pet_xml_add_val(dev,      "writable", ((writable == 1) ? "1" : "0")  );
    pet_xml_add_val(dev,      "path"    , file_path   );
    pet_xml_add_val(frontend, "tag"     , "ide"     );

    if ( is_cdrom ) {
	pet_xml_add_val(frontend, "model" , "V3Vee CDROM");
	pet_xml_add_val(frontend, "type"  , "CDROM"      );
    } else {
	pet_xml_add_val(frontend, "model" , "V3Vee HDD");
	pet_xml_add_val(frontend, "type"  , "HD" );    
    }

    asprintf(&bus_str, "%u", bus_num);
    asprintf(&drv_str, "%u", drive_num);
  
    pet_xml_add_val(frontend, "bus_num"  , bus_str );
    pet_xml_add_val(frontend, "drive_num", drv_str );
  
    free(bus_str);
    free(drv_str);
    
    v3_add_device(root, dev);

    return dev;
}


pet_xml_t
v3_add_hda(pet_xml_t   root,
	   char      * hd_path)
{
    return add_ide(root, hd_path, "hda", 1, 0, 0, 0);
}


pet_xml_t
v3_add_hdb(pet_xml_t  root,
	   char      * hd_path)
{
    return add_ide(root, hd_path, "hdb", 1, 0, 1, 0);
}


/*
  pet_xml_t
  v3_add_hdc(pet_xml_t root, char* hd_path)
  {
  return add_ide(root, hd_path, "hdb", 1, 1, 0, 0);
  }
*/

pet_xml_t
v3_add_cd(pet_xml_t   root,
	  char      * cd_path)
{
    return add_ide(root, cd_path, "cdrom", 0, 1, 0, 1);
}



static pet_xml_t
add_vd(pet_xml_t   root,
       char      * id,
       uint8_t     writable,
       char      * path)
{
    pet_xml_t file_dev  = NULL;
    pet_xml_t frontend  = NULL;
    pet_xml_t vd_dev    = NULL;

    char    * vd_dev_id = NULL;

    asprintf(&vd_dev_id, "%s-virtio", id);

    vd_dev   = v3_make_device(vd_dev_id, "LNX_VIRTIO_BLK");
    file_dev = v3_make_device(id,         "FILEDISK");

    frontend = pet_xml_add_subtree_tail(file_dev, "frontend");

    pet_xml_add_val(vd_dev   , "bus"      , "pci0"                        );
    pet_xml_add_val(file_dev , "writable" , ((writable == 1) ? "1" : "0") );
    pet_xml_add_val(file_dev , "path"     , path                          );
    pet_xml_add_val(frontend , "tag"      , vd_dev_id                    );

    free(vd_dev_id);
    
    v3_add_device(root, vd_dev);
    v3_add_device(root, file_dev);

    return file_dev;
}

pet_xml_t
v3_add_vda(pet_xml_t   root,
	   char      * file_path)
{
    return add_vd(root, "vda", 1, file_path);
}

pet_xml_t
v3_add_vdb(pet_xml_t   root,
	   char      * file_path)
{
    return add_vd(root, "vdb", 1, file_path);
}


pet_xml_t
v3_make_device(char * id,
	       char * dev_class)
{
  
    pet_xml_t device = NULL;
  
    device = pet_xml_new_tree("device");
  
    pet_xml_add_val(device, "id"   , id   );
    pet_xml_add_val(device, "class", dev_class);
  
    return device;
}


pet_xml_t
v3_add_net_bridge(pet_xml_t   root,
		  char      * bridge_if_name,
		  char      * mac_addr_str)
{
    pet_xml_t bridge       = NULL;
    pet_xml_t bridge_front = NULL;
    pet_xml_t virtio_nic   = NULL;

    virtio_nic = v3_make_device( "virtio-nic", "LNX_VIRTIO_NIC");
    bridge     = v3_make_device( "nic-bridge", "NIC_BRIDGE"    );

    bridge_front = pet_xml_add_subtree_tail(bridge, "frontend");
    
    pet_xml_add_val(bridge_front, "tag",  "virtio-nic");
    pet_xml_add_val(bridge,       "name", bridge_if_name);
    
    pet_xml_add_val(virtio_nic , "bus" , "pci0");

    if (mac_addr_str != NULL) {
	pet_xml_add_val(bridge,   "mac",  mac_addr_str);
    }
    
    v3_add_device(root, virtio_nic);
    v3_add_device(root, bridge);

    return bridge;    
}


pet_xml_t
v3_add_curses(pet_xml_t root)
{

    pet_xml_t    curses       = NULL;
    pet_xml_t    curses_front = NULL;
    pet_xml_t    cga          = NULL;

    curses   = v3_make_device( "curses", "CURSES_CONSOLE" );
    cga      = v3_make_device( "cga"   , "CGA_VIDEO"      );

    curses_front = pet_xml_add_subtree_tail(curses, "frontend");

    pet_xml_add_val(curses_front , "tag"        , "cga"    );
    pet_xml_add_val(cga          , "passthrough", "disable");

    v3_add_device( root , cga   );
    v3_add_device( root , curses);
  

    return curses;
}


int
v3_set_mem_size(pet_xml_t root,
		uint64_t  mem_size)
{
    pet_xml_t mem_tree = NULL;
    char*     mem_str  = NULL;

    asprintf(&mem_str, "%lu", mem_size);

    mem_tree = pet_xml_add_subtree(root, "memory");
    pet_xml_add_val(mem_tree, "size", mem_str );

    free(mem_str);
  
    return 0;
}


int
v3_set_num_cpus(pet_xml_t root,
		uint32_t  num_cpus)
{
    pet_xml_t core_tree = NULL;
    char*     cpu_str   = NULL;
    int       i         = 0;

    asprintf(&cpu_str, "%u", num_cpus);

    core_tree = pet_xml_add_subtree_tail(root, "cores");
    pet_xml_add_val(core_tree, "count", cpu_str);

    for(i = 0; i < num_cpus; i++){
	pet_xml_add_subtree_tail(core_tree, "core");
    }

    free(cpu_str);
  
    return 0;
}


int
v3_disable_large_pages(pet_xml_t root)
{
    pet_xml_t iter = NULL;
  
    iter = pet_xml_get_subtree(root, "paging");
    iter = ezxml_child(iter, "large_pages");
    if(iter != NULL){
	ezxml_set_txt(iter, "false");
    }

    return 0;
}


int
v3_disable_nested_pages(pet_xml_t root)
{
    pet_xml_t pages = NULL;
  
    pages = pet_xml_get_subtree(root, "paging");
  
    pet_xml_add_val(pages, "mode", "shadow" );

    return 0;
}
