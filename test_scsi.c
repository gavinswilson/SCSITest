#include <stdio.h>
#include <sys/ioctl.h>
#include <string.h>
#include <scsi/sg.h> 


/* global struct to store return data from a scsi cmd*/

typedef struct SCSI_data {
    unsigned char    data[1024];

    unsigned char    raw_sens[252];
    unsigned char    sense_key;
    unsigned char    additional_sense_code;
    unsigned char    additional_sense_qualifier;
    unsigned char    additional_sense_length;

    unsigned char    sense_data_descriptors[10][244];

    int              result;
} SCSI_data;



/* global struct to store return data from a scsi cmd*/

typedef struct SCSI_cmd {
    int              sg_fd;
    unsigned char    cmdblk[32];
    int              cmdblklength;
    int              allocation_length;
    int              xfer;
    int              timeout;

} SCSI_cmd;


SCSI_data send_scsicmd(SCSI_cmd cmdobject) {
    int k;;

    unsigned char inqBuff[cmdobject.allocation_length];
    unsigned char sense_buffer[252];

    SCSI_data output_data;
    sg_io_hdr_t io_hdr;

    /* Prepare INQUIRY command */
    memset(&io_hdr, 0, sizeof(sg_io_hdr_t));
    io_hdr.interface_id = 'S';
    io_hdr.cmd_len = cmdobject.cmdblklength;
    io_hdr.mx_sb_len = sizeof(sense_buffer);
    io_hdr.dxfer_direction = cmdobject.xfer;
    io_hdr.dxfer_len = cmdobject.allocation_length;
    io_hdr.dxferp = inqBuff;
    io_hdr.cmdp = cmdobject.cmdblk;
    io_hdr.sbp = sense_buffer;
    io_hdr.timeout = cmdobject.timeout;

    if (ioctl(cmdobject.sg_fd, SG_IO, &io_hdr) < 0) {
        output_data.result=2;
        return output_data;
    }

    /* now for the error processing */
    if ((io_hdr.info & SG_INFO_OK_MASK) != SG_INFO_OK) {
        output_data.result=1;
        if (io_hdr.sb_len_wr > 0) {
            printf("INQUIRY sense data: ");
            for (k = 0; k < io_hdr.sb_len_wr; ++k) {
                if ((k > 0) && (0 == (k % 10)))
                    printf("\n  ");
                printf("0x%02x ", sense_buffer[k]);
            }
            printf("\n");
        }

        if (io_hdr.masked_status)
            printf("INQUIRY SCSI status=0x%x\n", io_hdr.status);

        if (io_hdr.host_status)
            printf("INQUIRY host_status=0x%x\n", io_hdr.host_status);

        if (io_hdr.driver_status)
            printf("INQUIRY driver_status=0x%x\n", io_hdr.driver_status);
    }

    else {  /* assume INQUIRY response is present */
        output_data.result=0;
        for (k=0;k<cmdobject.allocation_length;k++) {
            output_data.data[k]=inqBuff[k];
        }
    }
    return output_data;
}


int main(int argc, char * argv[]) {

    FILE *driveptr=fopen(argv[1], "r");
    printf("Investigating %s\n", argv[1]);
    int i;

    SCSI_data   scsi_data_read_capacity;

    SCSI_cmd    scsi_read_capacity;
    
    scsi_read_capacity.sg_fd=fileno(driveptr);
    scsi_read_capacity.cmdblk[0]=0x9e;
    scsi_read_capacity.cmdblk[1]=0x10;
    scsi_read_capacity.cmdblk[13]=32;
    scsi_read_capacity.cmdblklength=16;
    scsi_read_capacity.xfer=SG_DXFER_FROM_DEV;
    scsi_read_capacity.allocation_length=32;
    scsi_read_capacity.timeout=1000;
    
    scsi_data_read_capacity=send_scsicmd(scsi_read_capacity);
    printf("read_capacity_result = %i\n", scsi_data_read_capacity.result);
    if (scsi_data_read_capacity.result==0) {
        printf("    capacity in blocks: %02x%02x%02x%02x%02x%02x%02x%02x\n",
            scsi_data_read_capacity.data[0],
            scsi_data_read_capacity.data[1],
            scsi_data_read_capacity.data[2],
            scsi_data_read_capacity.data[3],
            scsi_data_read_capacity.data[4],
            scsi_data_read_capacity.data[5],
            scsi_data_read_capacity.data[6],
            scsi_data_read_capacity.data[7]);

        printf("             blocksize: %02x%02x%02x%02x\n",
            scsi_data_read_capacity.data[8],
            scsi_data_read_capacity.data[9],
            scsi_data_read_capacity.data[10],
            scsi_data_read_capacity.data[11]);

    }

    fclose(driveptr);

    return 0;
}