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


int main(int argc, char * argv[]) 
    {
    int k;
    char * file_name = 0;
    int *lba = 2;
    for (k = 1; k < argc; ++k) {
        if (*argv[k] == '-') {
            printf("Unrecognized switch: %s\n", argv[k]);
            file_name = 0;
            break;
        }
        else if (0 == file_name)
            file_name = argv[k];
        else {
            printf("too many arguments\n");
            file_name = 0;
            break;
        }
    }
    if (0 == file_name) {
        printf("Usage: 'sg_simple16 <sg_device>'\n");
        return 1;
    }
    unsigned char *character = "C";
    read_scsi(file_name, lba);
    write_scsi(file_name, (unsigned char*)character, lba);
    read_scsi(file_name, lba);
    
    return 0;

    }



