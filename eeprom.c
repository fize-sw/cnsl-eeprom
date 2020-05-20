/************************************************************************
| Copyright (c) OSR Enterprises AG, 2018.     All rights reserved.
|
| This software is the exclusive and confidential property of
| OSR Enterprises AG and may not be disclosed, reproduced or modified
| without permission from OSR Enterprises AG.
|
| Description: read/write EEPROMs
|	       There is a two ways to use this code:
| 	       Using it by library function get_db_eeprom_params()
| 	       or either cli commands
|
************************************************************************/

#include <arpa/inet.h>
#include <ctype.h>
#include <eeplib.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <syslog.h>
#include <unistd.h>

#include "eeprom.h"

static char *progname;

static int wr_mac_str(const char *const str, char *mac)
{
	unsigned int m[MAC_SIZE];
	int rc;

	memset((void *)mac, 0, 8);
	rc = sscanf(str, "%x:%x:%x:%x:%x:%x", &m[0], &m[1], &m[2], &m[3], &m[4], &m[5]);
	if (rc == MAC_SIZE) {
		int i = MAC_SIZE;

		while (--i >= 0) {
			mac[i] = m[i];
		}
	} else {
		ERR("error parsing string <%s>", str);
		rc = -1;
	}
	return rc;
}

void usage()
{
	fprintf(stderr,
		"NOTE: -B parameter is MANDATORY and MUST BE FIRST.\n"
		"Usage: %s -B<board number> [OPTIONS]\n"
		"Parameter without value returns EEPROM current value,\n"
		"parameter with value sets EEPROM value.\n\n"
		"    -B -- set bus id (Mandatory) - board number (14 - MB, 15 - TOP, 1-6 - "
		"DB) .\n"
		"    -A -- get the entire EEPROM contents\n"
		"    -e -- get/set EEPROM version. Possible values: 1000(MB), 2000(DB), 3000(TOP)\n"
		"    -p -- get/set Platform Type. String up to 16 symbols.\n"
		"    -c -- get/set catalog name. String up to 32 symbols.\n"
		"    -s -- get/set serial number. String up to 16 symbols.\n"
		"    -b -- get/set Assembly Hw Version. String up to 16 symbols.\n"
		"    -M -- get/set base MAC address in format XX:XX:XX:XX:XX:XX\n"
		"    -N -- get/set number of MAC addresses in format XX\n"
		"    -U -- get CRC\n"
		"(*) (a) strings are truncated to the maximum allowed length\n"
		"    (b) only ASCII printable characters (codes 32 till 126) allowed\n"
		"    (c) leading and trailing white spaces are ignored\n",
		progname);

	fprintf(stderr, "\nExamples:\n"
			"    %s -B14 -A        -- show the entire EEPROM contents of Main board\n"
			"    %s -B1 -s         -- show DB1 serial number\n"
			"    %s -B4 -e1000  -- set eeprom version to 1000 for DB4\n"
			"    %s -B15 -e3000 -M00:11:22:33:44:55\n"
			"                      -- set EEPROM version, base MAC address.\n",
		progname, progname, progname, progname);

	return;
}

int check_params(int fd, int ac, char **av, eb_t *eb)
{
	int rc = 0;
	int er = 0;
	int c;
	Eeprom_BrdNum board = EEP_BRD_NUM_UNKNOWN;
	uint32_t int_input_param = 0;

	if (ac == 1) {
		usage();
	}

	// if (pattern(eb) != EEPROM_PATTERN){  /*check pattern*/
	// PR("check pattern failed");
	// rc = -1;
	// }
	//	printf("check params\n");

	while ((c = getopt(ac, av, "hS::B:Ae::p::c::s::b::M::N::U")) != -1) {
#ifdef WRITE_PROTECT
		if (optarg) {
			PR("Writing is forbidden in production mode");
			exit(0);
		}
#endif
		switch (c) {
		case 'A': /* print all*/
			if (board) {
				PR_all;
			}
			break;

		case 'B':
			board = (Eeprom_BrdNum)atoi(optarg);

			eb->location.__params = (eeprom_param_t *)eb->data;
			rc = get_eeprom_all(board, params(eb));
			if (rc == EEP_ERR_READ) {
				PR("board not exist!!");
			}
			if (rc != EEP_OK) {
				if (rc == EEP_ERR_INV_PARAM) {
					usage();
				}
				return rc;
			}
			if (htonl(crc_32(eb)) != crc_eeprom_calc(eb)) {
				ERR("CRC32: found:0x%x calc:0x%x => Data is corrupted!!!",
				    htonl(crc_32(eb)), crc_eeprom_calc(eb));
				rc = EEP_ERR_FAILED;
			}

			break;

		case 'e': /* EEPROM version */
			if (board) {
				if (optarg) {
					int_input_param = atoi(optarg);
					if (version(eb) != int_input_param) {
						if ((rc = set_eep_version(
							 board, &int_input_param)) != EEP_OK) {
							ERR("eeprom version write failed!");
							return rc;
						} else {
							params(eb)->eeprom_ver_num =
							    htonl(int_input_param);
						}
					}
				}
				PR_version;
			}
			break;

		case 'p': /* platform type */
			if (board) {
				if (optarg) {
					if (strncmp(platform(eb), optarg, STR_SIZE) != 0) {
						if ((rc = set_eep_platform(board, optarg)) !=
						    EEP_OK) {
							ERR("eeprom platform write failed!");
							return rc;
						} else {
							strncpy(platform(eb), optarg, STR_SIZE);
						}
					} else
						syslog(LOG_INFO,
						       "platform current value is equal, write "
						       "was not performed");
				}
				PR_platform;
			}
			break;

		case 'c': /* catalog  name */
			if (board) {
				if (optarg) {
					if (strncmp(catalog(eb), optarg, STR_DSIZE) != 0) {
						if ((rc = set_eep_catalog(board, optarg)) !=
						    EEP_OK) {
							ERR("eeprom catalog write failed!");
							return rc;
						} else {
							strncpy(catalog(eb), optarg, STR_DSIZE);
						}
					} else
						syslog(LOG_INFO,
						       "catalog current value is equal, write "
						       "was not performed");
				}
				PR_catalog;
			}
			break;

		case 's': /* serial */
			if (board) {
				if (optarg) {
					if (strncmp(serial(eb), optarg, STR_SIZE) != 0) {
						if ((rc = set_eep_serial(board, optarg)) !=
						    EEP_OK) {
							ERR("eeprom serial write failed!");
							return rc;
						} else
							strncpy(serial(eb), optarg, STR_SIZE);
					} else
						syslog(LOG_INFO,
						       "serial current value is equal, write was "
						       "not performed");
				}
				PR_serial;
			}
			break;

		case 'b': /* assy hw version */
			if (board) {
				if (optarg) {
					if (strncmp(assy(eb), optarg, STR_SIZE) != 0) {
						if ((rc = set_eep_assy(board, optarg)) != EEP_OK) {
							ERR("eeprom assy HW version write failed!");
							return rc;
						} else
							strncpy(assy(eb), optarg, STR_SIZE);
					} else
						syslog(LOG_INFO, "assy current value is equal, "
								 "write was not performed");
				}
				PR_assy;
			}
			break;

		case 'M': /* mac address */
			if (board) {
				if (optarg) {
					if (strncmp(mac(eb), optarg, STR_SIZE)) {
						if ((rc = set_eep_mac(board, optarg)) != EEP_OK) {
							ERR("eeprom MAC adress write failed!");
							return rc;
						} else {
							if (wr_mac_str(optarg, mac(eb)) == -1);
						}
					}
				}
				PR_mac;
			}
			break;

		case 'N': /*  # of mac addresses */
			if (board) {
				if (optarg) {
					int_input_param = atoi(optarg);
					if (mac_n(eb) != int_input_param) {
						if ((rc = set_eep_num_macs(
							 board, &int_input_param)) != EEP_OK) {
							ERR("eeprom num of macs write failed!");
							return rc;
						} else {
							params(eb)->num_macs =
							    htonl(int_input_param);
						}
					}
				}
				PR_macnum;
			}
			break;

		case 'S': /* eeprom size */
			if (board) {
				PR("eeprom size: %d", sizeof(eeprom_param_t));
			}
			break;

		case 'U': // get CRC
			if (board) {
				if ((eb->device != EEP_BRD_NUM_MB) &&
				    (eb->device != EEP_BRD_NUM_TOP)) {
					rc = get_eeprom_all(board, params(eb));
					if (rc != EEP_OK) {
						if (rc == EEP_ERR_INV_PARAM) {
							usage();
							return rc;
						}
						return rc;
					}
				}
				break;
			}

		case 'h': /* help */
		case '?':
		default:
			usage();
			return 0;
			break;
		}

		if (board == EEP_BRD_NUM_UNKNOWN) {
			ERR("-B argument is mandatory!\n");
			usage();
			return EEP_ERR_INV_PARAM;
		}
	}
	return rc;
}

/*!
 * Start program
 * @param[in] argc  Number of arguments
 * @param[in] argv  Pointer to array of arguments
 * @return 0 for succsess and other for failure
 */
int main(int argc, char **argv)
{
	int fd = 0;
	eb_t eb_s;
	eb_t *eb = &eb_s;

	progname = argv[0];

	if (check_params(fd, argc, argv, eb)) {
		//		usage();
		return -1;
	}
	return 0;
}
