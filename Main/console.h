
// --------------------------------------------------------------------------------------
// Module     : CMD
// Description: Command processing module
// Author     : James
// --------------------------------------------------------------------------------------
// DragonBall.com
// --------------------------------------------------------------------------------------

#ifndef CONSOLE_H
#define CONSOLE_H

#define CONSOLE_CMD_MAX         80
#define CONSOLE_CMD_HIST_MAX    10

#define CONSOLE_CMD_SEP         " ,"
#define CONSOLE_CMD_TOKEN_MAX   32   // FORMAT: cmd [arg1] [arg2] [arg3]

#define CONSOLE_CMD_PROMPT      "#>"



typedef enum
{
    ERR_PASS = 0,
    ERR_FAIL,
    ERR_INV_PARAM,
    ERR_INV_MSG_ID,
    ERR_INV_ADDR,
    ERR_BUSY,
    ERR_READY,
    ERR_TIMEOUT,
    ERR_OVERRUN,
    ERR_UNDERRUN,
    ERR_TOO_LONG,
    ERR_TOO_SHORT,
    ERR_MSG_TOO_SHORT,
    ERR_IN_PROGRESS,
    ERR_NO_HANDLER,
    ERR_FILE_DESC,
    ERR_SOCK_DISC,
    ERR_SOCK_FAIL,
    ERR_CRC_FAIL,
    ERR_LEN_FAIL,
    ERR_LEN_TOO_LONG,
    ERR_FIRMWARE_FAILED_ERASE,
    ERR_FIRMWARE_FAILED_PROGRAM,
    ERR_FIRMWARE_FAILED_VERIFY,
    ERR_FIRMWARE_ADDRESS_OUT_OF_RANGE,
    ERR_UNEXP,
    ERR_NOT_IMPLEMENTED,
    ERR_RX_ACTIVITY,
    ERR_DONE,
    ERR_UNKNOWN,
    ERR_COMMS_TIMEOUT,
    ERR_CTRL_C_PRESSED,

} err_t;

typedef enum
{
    CONSOLE_CMD_CTRL_C  = 3,
    CONSOLE_CMD_ESC     = 27,
    CONSOLE_CMD_BSP     = 8,
    CONSOLE_CMD_DEL     = 127,
    CONSOLE_CMD_HASH    = 35
} console_cmd_key_t;

typedef int32_t (*console_cmd_fctn_ptr_t)(char **arg);

typedef enum
{
    CONSOLE_CMD_ARG_TYPE_NONE,
    CONSOLE_CMD_ARG_TYPE_INT,
    CONSOLE_CMD_ARG_TYPE_STR
} console_cmd_arg_type_t;

typedef struct
{
    char                    *cmd_name;
    console_cmd_fctn_ptr_t  cmd_fctn;
    console_cmd_arg_type_t  arg_type;
    char                    *cmd_help;
    uint8_t                 hidden; // hidden command
} cmd_t;

void  console_init(void);
void  console_update(void);
int32_t console_cmd_get(void);
int32_t console_cmd_execute(uint8_t cmd_index, char **arg, uint8_t arg_max);

#endif // CONSOLE_H
