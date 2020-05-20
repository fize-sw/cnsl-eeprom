/************************************************************************
| Copyright (c) OSR Enterprises AG, 2018.     All rights reserved.
|
| This software is the exclusive and confidential property of
| OSR Enterprises AG and may not be disclosed, reproduced or modified
| without permission from OSR Enterprises AG.
|
| Description:
|
************************************************************************/
//#include <eeplib.h>
#ifndef EEPROM_H_
#define EEPROM_H_

#ifdef DEBUG
#define PR(format, ...)                                                                            \
	do {                                                                                       \
		fprintf(stdout, "%s:%d %s - " format " \n", __FILE__, __LINE__, __func__,          \
			##__VA_ARGS__);                                                            \
	} while (0)
#else
#define PR(format, ...)                                                                            \
	do {											   \
		fprintf(stdout, format "\n", ##__VA_ARGS__);                                       \
	} while (0)
#endif

#define ERR(format, ...)                                                                           \
	do {                                                                                       \
		fprintf(stderr, "%s:%d %s - " format "\n", __FILE__, __LINE__, __func__,           \
			##__VA_ARGS__);                                                            \
	} while (0)

#define FATAL ERR("(%d) [%s]\n", errno, strerror(errno))

#define PR_pattern PR("pattern         : %x", pattern(eb))
#define PR_version PR("eeprom ver num  : %d", version(eb))
#define PR_platform PR("platform type   : %.*s", STR_SIZE, platform(eb))
#define PR_catalog PR("catalog name    : %.*s", STR_DSIZE, catalog(eb))
#define PR_serial PR("serial number   : %.*s", STR_SIZE, serial(eb))
#define PR_assy PR("assy hw ver     : %.*s", STR_SIZE, assy(eb))

#define PR_mac                                                                                     \
	PR("mac address     : %02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx", (uint8_t)mac(eb)[0],     \
	   (uint8_t)mac(eb)[1], (uint8_t)mac(eb)[2], (uint8_t)mac(eb)[3], (uint8_t)mac(eb)[4],     \
	   (uint8_t)mac(eb)[5])
#define PR_macnum PR("# of macs       : %d", mac_n(eb))
#define PR_crc32 PR("crc32           : 0x%x", htonl(crc_32(eb)))

#define PR_all                                                                                     \
	PR_pattern;                                                                                \
	PR_version;                                                                                \
	PR_platform;                                                                               \
	PR_catalog;                                                                                \
	PR_serial;                                                                                 \
	PR_assy;                                                                                   \
	PR_mac;                                                                                    \
	PR_macnum;


/*!
 * @brief Load eeprom content to structure
 * @param[in] eb eeprom buffer
 * @param[in] fd file discriptor (?)
 * @return 0 for succsess
 */
static int wr_mac_str(const char *const str, char *mac);
void usage(void);
int check_params(int, int, char **, eb_t *);

#endif /* EEPROM_H_ */
