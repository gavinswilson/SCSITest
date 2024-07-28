#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <scsi/sg.h>
#include "scsi_utils.h"

#define _16_REPLY_LEN 512
#define _16_CMD_LEN 16

#define EBUFF_SZ 256

int read_scsi(char* file_name)
{
    int sg_fd, ok;
    uint8_t r16_cdb [_16_CMD_LEN] =
                {0x88, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0};
    sg_io_hdr_t io_hdr;
    //char * file_name = 0;
    char ebuff[EBUFF_SZ];
    unsigned char inBuff[_16_REPLY_LEN];
    uint8_t sense_buffer[32];

    if ((sg_fd = open(file_name, O_RDWR)) < 0) 
    {
        snprintf(ebuff, EBUFF_SZ,
                 "sg_simple16: error opening file: %s", file_name);
        perror(ebuff);
        return 1;
    }
    
    // if ((ioctl(sg_fd, SG_GET_VERSION_NUM, &k) < 0) || (k < 30000)) 
    // {
    //     printf("sg_simple16: %s doesn't seem to be an new sg device\n",
    //            file_name);
    //     close(sg_fd);
    //     return 1;
    // }

    memset(&io_hdr, 0, sizeof(sg_io_hdr_t));
    io_hdr.interface_id = 'S';
    io_hdr.cmd_len = sizeof(r16_cdb);
    io_hdr.mx_sb_len = sizeof(sense_buffer);
    io_hdr.dxfer_direction = SG_DXFER_FROM_DEV;
    io_hdr.dxfer_len = _16_REPLY_LEN;
    io_hdr.dxferp = inBuff;
    io_hdr.cmdp = r16_cdb;
    io_hdr.sbp = sense_buffer;
    io_hdr.timeout = 20000;     /* 20000 millisecs == 20 seconds */
    

    if (ioctl(sg_fd, SG_IO, &io_hdr) < 0) {
        perror("sg_simple16: Inquiry SG_IO ioctl error");
        close(sg_fd);
        return 1;
    }

    ok = 1;
    
    
    if (ok) { /* output result if it is available */
        printf("READ_16 duration=%u millisecs, resid=%d, msg_status=%d\n",
               io_hdr.duration, io_hdr.resid, (int)io_hdr.msg_status);
        // char* p = (char *)io_hdr.dxferp;
        // printf("p: %.8s \n", p);
        for (int i = 0; i < _16_REPLY_LEN; i++)
        {
            printf("%hx ", inBuff[i]);
        }
    }

    close(sg_fd);
}

int write_scsi(char* file_name, unsigned char *character)
{
    int sg_fd, k, ok;
    uint8_t w16_cdb [_16_CMD_LEN] =
                {0x8A, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    sg_io_hdr_t io_hdr;
    //char * file_name = 0;
    char ebuff[EBUFF_SZ];
    unsigned char outBuff[_16_REPLY_LEN];
    uint8_t sense_buffer[32];

    for (int i = 0; i < _16_REPLY_LEN; i++)
        {
            outBuff[i] = 'B';
        }

    if ((sg_fd = open(file_name, O_RDWR)) < 0) {
        snprintf(ebuff, EBUFF_SZ,
                 "sg_simple16: error opening file: %s", file_name);
        perror(ebuff);
        return 1;
    }
    /* Just to be safe, check we have a new sg device by trying an ioctl */
    if ((ioctl(sg_fd, SG_GET_VERSION_NUM, &k) < 0) || (k < 30000)) {
        printf("sg_simple16: %s doesn't seem to be an new sg device\n",
               file_name);
        close(sg_fd);
        return 1;
    }

    memset(&io_hdr, 0, sizeof(sg_io_hdr_t));
    io_hdr.interface_id = 'S';
    io_hdr.cmd_len = sizeof(w16_cdb);
    /* io_hdr.iovec_count = 0; */  /* memset takes care of this */
    io_hdr.mx_sb_len = sizeof(sense_buffer);
    io_hdr.dxfer_direction = SG_DXFER_TO_DEV;
    io_hdr.dxfer_len = _16_REPLY_LEN;
    io_hdr.dxferp = outBuff;
    io_hdr.cmdp = w16_cdb;
    io_hdr.sbp = sense_buffer;
    io_hdr.timeout = 20000;     

    if (ioctl(sg_fd, SG_IO, &io_hdr) < 0) {
        perror("sg_simple16: Inquiry SG_IO ioctl error");
        close(sg_fd);
        return 1;
    }

    ok = 1;
    
    close(sg_fd);
    return 0;
}