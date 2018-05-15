/*
 * Copyright (c) 2016, Jack Lange <jacklange@cs.pitt.edu>
 * All rights reserved.
 *
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "V3VEE_LICENSE".
 */

#include <getopt.h>
#include <stdint.h>

#include "v3vee.h"
#include "util.h"
#include "config.h"

#include "pet_xml.h"

#define DEFAULT_NAME           "v3_VM"
#define DEFAULT_MEM_SIZE       512
#define DEFAULT_CPU_CNT        1
#define DEFAULT_HDA_PATH       NULL
#define DEFAULT_HDB_PATH       NULL
#define DEFAULT_CDROM_PATH     NULL
#define DEFAULT_VDA_PATH       NULL
#define DEFAULT_VDB_PATH       NULL
#define DEFAULT_LARGE_PAGES    1
#define DEFAULT_NESTED_PAGING  1


static char     * name               = DEFAULT_NAME;
static uint64_t   mem_size           = DEFAULT_MEM_SIZE;
static uint32_t   cpu_cnt            = DEFAULT_CPU_CNT;
static char     * hda_path           = DEFAULT_HDA_PATH;
static char     * hdb_path           = DEFAULT_HDA_PATH;
static char     * vda_path           = DEFAULT_HDA_PATH;
static char     * vdb_path           = DEFAULT_HDA_PATH;
static char     * cdrom_path         = DEFAULT_HDA_PATH;
static uint8_t    use_large_pages    = DEFAULT_LARGE_PAGES;
static uint8_t    use_nested_paging  = DEFAULT_NESTED_PAGING;


static int cmd_line_mem         = 0;
static int cmd_line_smp         = 0;
static int cmd_line_hda         = 0;
static int cmd_line_hdb         = 0;
static int cmd_line_cdrom       = 0;
static int cmd_line_vda         = 0;
static int cmd_line_vdb         = 0;
static int cmd_line_large_pgs   = 0;
static int cmd_line_nested_pgts = 0;

void
usage(char * exe_name)
{
    printf("Usage: %s [cmd options] [vm options] \n"						 \
	   "cmd options: \n"									 \
	   "\t[--help | -h]\n"									 \
	   "\t[--save=<file> | -s <file>]                     save configuration to a file\n"	 \
	   "vm options:\n"									 \
	   "\t[-name <name>]             (default: v3_VM[x])  name of the VM\n"			 \
	   "\t[-smp <cpus>]              (default: 1)         number of CPUs\n"			 \
	   "\t[-m <size in MB>]          (default: 512MB)     amount of memory in MB\n"		 \
	   "\t[-hda <file>]                                   path to IDE disk 1\n"		 \
	   "\t[-hdb <file>]                                   path to IDE disk 2\n"		 \
	   "\t[-vda <file>]                                   path to virtio disk 1\n"		 \
	   "\t[-cdrom <file>]                                 path to cdrom image\n"		 \
	   "\t[--disable_large_pages]    (default: enabled)   disable large pages\n"		 \
	   "\t[--disable_nested_paging]  (default: enabled)   disable nested paging\n",
	   exe_name);

    exit(-1);
}


int
main(int argc, char ** argv)
{

    pet_xml_t   vm_xml_cfg      = PET_INVALID_XML;
    char      * save_file_name  = NULL;
    
    /* Parse Options */
    {
	int  opt_index   = 0;
	char c           = 0;
	
	
	static struct option long_options[] = {
	    { "name"                  , required_argument , 0                       , 'n' }, /* 0 */
	    { "smp"                   , required_argument , &cmd_line_smp           ,  1  }, /* 1 */
	    { "mem"                   , required_argument , &cmd_line_mem           ,  1  }, /* 2 */
	    { "hda"                   , required_argument , &cmd_line_hda           ,  1  }, /* 3 */
	    { "hdb"                   , required_argument , &cmd_line_hdb           ,  1  }, /* 4 */
	    { "vda"                   , required_argument , &cmd_line_vda           ,  1  }, /* 5 */
	    { "vdb"                   , required_argument , &cmd_line_vdb           ,  1  }, /* 6 */
	    { "cdrom"                 , required_argument , &cmd_line_cdrom         ,  1  }, /* 7 */
	    { "disable_large_pages"   , required_argument , &cmd_line_large_pgs     ,  1  }, /* 8 */
	    { "disable_nested_paging" , required_argument , &cmd_line_nested_pgts   ,  1  }, /* 9 */
	    { "help"                  , no_argument       , 0                       , 'h' },
	    { "save"                  , required_argument , 0                       , 's' },
	    {0,0,0,0}
	};
	
	
	//parse inputs
	while ((c = getopt_long_only(argc, argv, "hs:n:", long_options, &opt_index)) != -1) {
	    	    
	    switch(c) {
		case 'h':
		    usage(argv[0]);
		    return -1;
		case 's':
		    save_file_name = optarg;
		    break;
		case 'n':       /* name */
		    name = optarg;
		    break;
		case 0:
		    switch (opt_index) {
			case 1:       /* smp */
			    if (!isdigit(*optarg)) {
				usage(argv[0]);
			    }

			    cpu_cnt = v3_atou32(0xffffffff, optarg);

			    if (cpu_cnt == 0xffffffff) {
				printf("Invalid SMP argument\n");
				usage(argv[0]);
			    }
			    
			    break;
			case 2:
			    if (!isdigit(*optarg)) {
				usage(argv[0]);
			    }
			    
			    mem_size = v3_atou32(0xffffffff, optarg);

			    if (mem_size == 0xffffffff) {
				printf("Invalid MEM argument\n");
				usage(argv[0]);
			    }
			    
			    break;			    
			case 3:
			    hda_path = optarg;
			    break;
			case 4:
			    hdb_path = optarg;
			    break;
			case 5:
			    vda_path = optarg;
			    break;
			case 6:
			    vdb_path = optarg;
			    break;
			case 7:
			    cdrom_path = optarg;
			    break;
			case 8:
			    use_large_pages = 0;
			    break;
			case 9:
			    use_nested_paging = 0;
			    break;
			default:
			    break;
		    }
		    
		    break;
		case '?':
		default:
		    usage(argv[0]);
		    break;
	    }
	}
    }

    printf("Creating VM [%d cpus, %luMB RAM]\n", cpu_cnt, mem_size);
    
    vm_xml_cfg = v3_create_base_config(mem_size, cpu_cnt);
    
    v3_add_curses(vm_xml_cfg);
    printf("...with curses console\n");
    
    if (hda_path) {
	v3_add_hda(vm_xml_cfg, hda_path);
	printf("...with hda: %s\n", hda_path);
    }
    
    if (hdb_path) {
	v3_add_hdb(vm_xml_cfg, hdb_path);
	printf("...with hdb: %s\n", hdb_path);
    }
    
    if (cdrom_path) {
	v3_add_cd(vm_xml_cfg, cdrom_path);
	printf("...with cdrom:  %s\n", cdrom_path);
    }
    
    if (vda_path) {
	v3_add_vda(vm_xml_cfg, vda_path);
	printf("...with vda: %s\n", vda_path);
    }

    if (vdb_path) {
	v3_add_vdb(vm_xml_cfg, vdb_path);
	printf("...with vdb: %s\n", vdb_path);
    }

    if (use_large_pages == 0) {
	v3_disable_large_pages(vm_xml_cfg);
	printf("...large pages disabled\n");
    }
    
    if (use_nested_paging == 0) {
	v3_disable_nested_pages(vm_xml_cfg);
	printf("...nested_paging disabled\n");
    }
    
    
    /* save the xml for debuging purposes */
    if (save_file_name) {
	FILE * save_file = NULL;
	char * xml_str   = pet_xml_get_str(vm_xml_cfg);

  	save_file = fopen(save_file_name, "w");

	if (save_file == NULL) {
	    printf("Error: Could not open configuration save file [%s]\n", save_file_name);
	    return -1;
	}

	fprintf(save_file, "%s", xml_str);

	fclose(save_file);	
	free(xml_str); 
    }
    
    
    /*create the VM */
    {
	uint8_t * img_data = NULL;
	uint32_t  img_size = 0;
	int       vm_id    = 0;


	img_data = v3_build_vm_image(vm_xml_cfg, &img_size);

	if (img_data == NULL) {
	    printf("Error: Could not build VM image\n");
	    return -1;
	}
	
	vm_id = v3_create_vm(name, img_data, img_size);

	if (vm_id == -1) {
	    printf("Error: Could not create VM\n");
	    return -1;
	}
	
	if (v3_launch_vm(vm_id) == -1) {
	    printf("Error: Could not launch VM\n");
	    return -1;
	}

	printf("VM Launched: ID = %d\n", vm_id);
    }
    

  return 0;
}
