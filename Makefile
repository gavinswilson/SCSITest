all: scsi_main.c
	gcc -o SCSI scsi_main.c scsi_utils.c