There are several modes to this tool 
in order to distinguish between them - use preprocessor.
SIMULATION - used to work with the attached bin file instead of a real eeprom flash - can be used for debugging (in lack of DB Hw)
CLI_COMMANDS - use the main c file - there is a CLI tool - usefull to establish a new DB.
when CLI_COMMANDS is not defined - you can use the api function get_db_eeprom_params - to access the DB eeprom 

This is the commands used in the CLI
EEPROM management.
Usage: eeprom [-A] | [-e[EEPROM version]]
                     [-p[Platform Type]]
                     [-c[Catalog number]]
                     [-s[Serial number]]
                     [-b[Board type]]
                     [-h[PCB Hw version]]
                     [-m[Assembly Hw Version]]
                     [-M[base MAC address]]
                     [-N[number of MAC addresses]]
                     [-t[MAC Type]]
Parameter without value returns EEPROM current value,
parameter with value sets EEPROM value.

    -B -- set bus id - daughter board num.
    -P -- set EEPROM DB pattern.
    -A -- get the entire EEPROM contents
    -e -- get/set EEPROM version. Possible values: 100
    -p -- get/set Platform Type. String up to 16 symbols.
    -c -- get/set catalog number. String up to 16 symbols.
    -s -- get/set serial number. String up to 16 symbols.
    -b -- get/set board type. String up to 16 symbols.
    -h -- get/set PCB Hw version. String up to 16 symbols.
    -m -- get/set Assembly Hw Version. integer up to 4 bytes.
    -M -- get/set base MAC address in format XX:XX:XX:XX:XX:XX
    -N -- get/set number of MAC addresses in format XX
    -t -- get/set MAC Type. Integer up to 4 bytes.
(*) (a) strings are truncated to the maximum allowed length
    (b) only ASCII printable characters (codes 32 till 126) allowed
    (c) leading and trailing white spaces are ignored

Examples:
    eeprom -A    -- show the entire EEPROM contents
    eeprom -s    -- show project serial number
    eeprom -e300  -M00:11:22:33:44:55 -N32 -dODM2 -n"The Company"
                 -- set EEPROM version, base MAC address,
                    number of MAC addresses, regulatory domain and company name




THE USE OF THE BUS ID IS NEEDED TO ACCESS THE RIGHT DB

./eeprom-tool -B4 -A  
means print all relevant data from DB 4
