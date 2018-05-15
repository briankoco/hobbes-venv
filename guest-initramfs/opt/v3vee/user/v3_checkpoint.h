/* 
 * V3 checkpoint interface
 * (c) 2015, Jack Lange <jacklange@cs.pitt.edu>
 */


#ifndef __V3_CHECKPOINT__
#define __V3_CHECKPOINT__

#ifdef __cplusplus
extern "C" {
#endif

#define CHKPT_KEY_LEN   32

struct block_header {
    char     name[CHKPT_KEY_LEN];
    uint32_t offset;
    uint32_t size;
} __attribute__((packed));

struct chkpt_header {
    uint8_t  magic[4];
    uint32_t version;

    uint8_t  arch[4];

    uint32_t num_blks;
    uint32_t size;

    struct block_header blocks[0];
} __attribute__((packed));

/* 
 * Returns a file descriptor for accessing the checkpoint data 
 */
int v3_create_checkpoint(int vm_id);


ssize_t v3_chkpt_get_size(int chckpt_fd);

struct chkpt_header * v3_chkpt_get(int chkpt_fd);

int v3_chkpt_save(int chkpt_fd);
int v3_chkpt_load(int chkpt_fd);



/* TODO: 
 *    Include block structures from each chkpt source
 */

#ifdef __cplusplus
}
#endif


#endif
