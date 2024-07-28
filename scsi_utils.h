#ifndef SCSI_UTILS_H
#define SCSI_UTILS_H

#define _16_REPLY_LEN 512
#define _16_CMD_LEN 16

#define EBUFF_SZ 256

int read_scsi(char* file_name, int* lba);
int write_scsi(char* file_name, unsigned char *character, int* lba);

#endif
