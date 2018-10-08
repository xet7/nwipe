/*
 *  context.h: The internal state representation of nwipe.
 *
 *  Copyright Darik Horn <dajhorn-dban@vanadac.com>.
 *  
 *  Modifications to original dwipe Copyright Andy Beverley <andy@andybev.com>
 *
 *  This program is free software; you can redistribute it and/or modify it under
 *  the terms of the GNU General Public License as published by the Free Software
 *  Foundation, version 2.
 *
 *  This program is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 *  details.
 *
 *  You should have received a copy of the GNU General Public License along with
 *  this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA. 
 *
 */

#ifndef CONTEXT_H_
#define CONTEXT_H_

#include "prng.h"

typedef enum nwipe_device_t_
{
	NWIPE_DEVICE_UNKNOWN = 0,   /* Unknown device. */
	NWIPE_DEVICE_IDE,
	NWIPE_DEVICE_SCSI,
	NWIPE_DEVICE_COMPAQ,        /* Unimplemented.  */
	NWIPE_DEVICE_USB,           /* Unimplemented.  */
	NWIPE_DEVICE_IEEE1394       /* Unimplemented.  */
} nwipe_device_t;

typedef enum nwipe_pass_t_
{
	NWIPE_PASS_NONE = 0,     /* Not running.                     */
	NWIPE_PASS_WRITE,        /* Writing patterns to the device.  */
	NWIPE_PASS_VERIFY,       /* Verifying a pass.                */
	NWIPE_PASS_FINAL_BLANK,  /* Filling the device with zeros.   */
	NWIPE_PASS_FINAL_OPS2    /* Special case for nwipe_ops2.     */
} nwipe_pass_t;

typedef enum nwipe_select_t_
{
	NWIPE_SELECT_NONE = 0,     /* Unused.                                                                  */
	NWIPE_SELECT_TRUE,         /* Wipe this device.                                                        */
	NWIPE_SELECT_TRUE_PARENT,  /* A parent of this device has been selected, so the wipe is implied.       */
	NWIPE_SELECT_FALSE,        /* Do not wipe this device.                                                 */
	NWIPE_SELECT_FALSE_CHILD,  /* A child of this device has been selected, so we can't wipe this device.  */
	NWIPE_SELECT_DISABLED      /* Do not wipe this device and do not allow it to be selected.              */
} nwipe_select_t;


#define NWIPE_KNOB_SPEEDRING_SIZE         30
#define NWIPE_KNOB_SPEEDRING_GRANULARITY  10

typedef struct nwipe_speedring_t_
{
	u64    bytes[NWIPE_KNOB_SPEEDRING_SIZE];
	u64    bytestotal;
	u64    byteslast;
	time_t times[NWIPE_KNOB_SPEEDRING_SIZE];
	time_t timestotal;
	time_t timeslast;
	u32    position;
} nwipe_speedring_t;


typedef struct nwipe_context_t_
{
	int               block_size;       /* The soft block size reported the device.                       */
	int               device_bus;       /* The device bus number.                                         */
	int               device_fd;        /* The file descriptor of the device file being wiped.            */
	int               device_host;      /* The host number.                                               */
	struct hd_driveid device_id;        /* The WIN_IDENTIFY data for IDE drives.                          */
	int               device_lun;       /* The device logical unit number.                                */
	int               device_major;     /* The major device number.                                       */
	int               device_minor;     /* The minor device number.                                       */
	int               device_part;      /* The device partition or slice number.                          */
	char*             device_name;      /* The device file name.                                          */
	long long         device_size;      /* The device size in bytes.                                      */
	char*             device_size_text; /* The device size in a more (human)readable format.              */
	struct stat       device_stat;      /* The device file state from fstat().                            */
	nwipe_device_t    device_type;      /* Indicates an IDE, SCSI, or Compaq SMART device.                */
	int               device_target;    /* The device target.                                             */
	u64               eta;              /* The estimated number of seconds until method completion.       */
	int               entropy_fd;       /* The entropy source. Usually /dev/urandom.                      */
	char*             label;            /* The string that we will show the user.                         */
	int               pass_count;       /* The number of passes performed by the working wipe method.     */
	u64               pass_done;        /* The number of bytes that have already been i/o'd in this pass. */
	u64               pass_errors;      /* The number of errors across all passes.                        */
	u64               pass_size;        /* The total number of i/o bytes across all passes.               */
	nwipe_pass_t      pass_type;        /* The type of the current working pass.                          */
	int               pass_working;     /* The current working pass.                                      */
	nwipe_prng_t*     prng;             /* The PRNG implementation.                                       */
	nwipe_entropy_t   prng_seed;        /* The random data that is used to seed the PRNG.                 */
	void*             prng_state;       /* The private internal state of the PRNG.                        */
	int               result;           /* The process return value.                                      */
	int               round_count;      /* The number of rounds performed by the working wipe method.     */
	u64               round_done;       /* The number of bytes that have already been i/o'd.              */
	u64               round_errors;     /* The number of errors across all rounds.                        */
	u64               round_size;       /* The total number of i/o bytes across all rounds.               */
	double            round_percent;    /* The percentage complete across all rounds.                     */
	int               round_working;    /* The current working round.                                     */
	int               sector_size;      /* The hard sector size reported by the device.                   */
	nwipe_select_t    select;           /* Indicates whether this device should be wiped.                 */
	int               signal;           /* Set when the child is killed by a signal.                      */
	nwipe_speedring_t speedring;        /* Ring buffer for computing the rolling throughput average.      */
	short             sync_status;      /* A flag to indicate when the method is syncing.                 */
	pthread_t         thread;           /* The ID of the thread.                                          */
	u64               throughput;       /* Average throughput in bytes per second.                        */
	u64               verify_errors;    /* The number of verification errors across all passes.           */
	char              serial_no[21];    /* Serial number(processed), 20 characters, plus null termination */
	struct hd_driveid  identity;	      /* Identity contains the raw serial number of the drive           */
                                       /* (where applicable), however, for use within nwipe use the      */
                                       /* processed serial_no[21] string above. To access serial no. use */
                                       /* c[i]->serial_no) and not c[i]->identity.serial_no);                                              */
} nwipe_context_t;


/* We use 2 data structs to pass data between threads. */

/* The first contains any required values:                                      */
/* Values cannot form part of the second array below, hence the need for this.  */
typedef struct
{
        int       nwipe_enumerated;  /* The number of devices available.                */
        int       nwipe_selected;    /* The number of devices being wiped.              */
        time_t    maxeta;            /* The estimated runtime of the slowest device.    */
        u64       throughput;        /* Total throughput                                */
        u64       errors;            /* The combined number of errors of all processes. */
        pthread_t *gui_thread;       /* The ID of GUI thread.                           */
} nwipe_misc_thread_data_t;

/* The second points to the first structure, as well as the structure of all the devices   */
typedef struct 
{
        nwipe_context_t **c;                                 /* Pointer to the nwipe context structure.           */
        nwipe_misc_thread_data_t *nwipe_misc_thread_data;    /* Pointer to the misc structure above.              */
} nwipe_thread_data_ptr_t;


#endif /* CONTEXT_H_ */

/* eof */
