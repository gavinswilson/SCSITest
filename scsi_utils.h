#ifndef SCSI_UTILS_H
#define SCSI_UTILS_H

#define _16_REPLY_LEN 512
#define _16_CMD_LEN 16

#define EBUFF_SZ 256

void read_scsi(char* file_name);
void write_scsi(char* file_name, char* character)
#endif
