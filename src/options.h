/*
 *  options.h: Command line processing routines for nwipe.
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


#ifndef OPTIONS_H_
#define OPTIONS_H_

/* Program knobs. */
#define NWIPE_KNOB_ENTROPY                "/dev/urandom"
#define NWIPE_KNOB_IDENTITY_SIZE          512
#define NWIPE_KNOB_LABEL_SIZE             128
#define NWIPE_KNOB_LOADAVG                "/proc/loadavg"
#define NWIPE_KNOB_LOG_BUFFERSIZE         1024                /* Maximum length of a log event. */
#define NWIPE_KNOB_PARTITIONS             "/proc/partitions"
#define NWIPE_KNOB_PARTITIONS_PREFIX      "/dev/"
#define NWIPE_KNOB_PRNG_STATE_LENGTH      512                 /* 128 words */
#define NWIPE_KNOB_SCSI                   "/proc/scsi/scsi"
#define NWIPE_KNOB_SLEEP                  1
#define NWIPE_KNOB_STAT                   "/proc/stat"

#define MAX_NUMBER_EXCLUDED_DRIVES        5

/* Function prototypes for loading options from the environment and command line. */
int nwipe_options_parse( int argc, char** argv );
void nwipe_options_log( void );

/* Function to display help text */
void display_help();

typedef struct /* nwipe_options_t */
{
	int            autonuke;  /* Do not prompt the user for confirmation when set.          */
	int            noblank;   /* Do not perform a final blanking pass.                      */
    int            nowait;    /* Do not wait for a final key before exiting.                */
    int            nosignals; /* Do not allow signals to interrupt a wipe.                  */
    int            nogui ;    /* Do not show the GUI.                                       */
	char*          banner;    /* The product banner shown on the top line of the screen.    */
//	nwipe_method_t method;    /* A function pointer to the wipe method that will be used.   */
	void*          method;    /* A function pointer to the wipe method that will be used.   */
	char           logfile[FILENAME_MAX];   /* The filename to log the output to            */
	char*          exclude_drives[MAX_NUMBER_EXCLUDED_DRIVES]; /* Drives excluded from the search */
	nwipe_prng_t*  prng;      /* The pseudo random number generator implementation.         */
	int            rounds;    /* The number of times that the wipe method should be called. */
	int            sync;      /* A flag to indicate whether writes should be sync'd.        */
	nwipe_verify_t verify;    /* A flag to indicate whether writes should be verified.      */
} nwipe_options_t;

extern nwipe_options_t nwipe_options;

#endif /* OPTIONS_H_ */

/* eof */
