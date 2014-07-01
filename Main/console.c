// --------------------------------------------------------------------------------------
// Module     : CMD
// Description: Command processing module
// Author     : James
// --------------------------------------------------------------------------------------
// DragonBall.com
// --------------------------------------------------------------------------------------




#include <includes.h>
#include <string.h>
#include "console.h"




static char        cmd_str[CONSOLE_CMD_MAX+1];    // add 1 to account for NULL terminator
static uint8_t     cmd_str_index;
static uint8_t     cmd_hist_str_index;
static char        cmd_hist_str[CONSOLE_CMD_HIST_MAX][CONSOLE_CMD_MAX+1];
static uint16_t    cmd_comment = 0;

#define CMD_HELP_HELP           "help: lists available commands, for detailed help type: command -h."
#define CMD_HELP_TIME           "time: shows time in seconds since last reset or power on."
#define CMD_HELP_RESET          "reset: resets system."
#define CMD_HELP_INFO           "info: shows firmware information."
#define CMD_HELP_THREADS        "threads: shows thread information."
#define CMD_HELP_MEM            "mem: shows memory usage."
#define CMD_HELP_STATS          "stats: shows runtime statistics."
#define CMD_HELP_HIST           "hist: shows command history."
#define CMD_HELP_TRF7970_REG    "reg: trf7970 register read/write."
#define CMD_HELP_TRF7970_CMD    "cmd: trf7970 command write."
#define CMD_HELP_RAND           "rand: generates random number."
#define CMD_HELP_ANTENNA        "ant: antenna select."
#define CMD_HELP_TRF            "trf: en [on/off] rf[on/off] ant[bike/card] find [on/off] reg [<val1, val2, ...>] cmd [<val1, val2, ...>]."
#define CMD_HELP_ID             "id: shows or sets system id [<val1, val2, ..., val6>]."
#define CMD_HELP_MSN            "msn: shows or sets the MSN (ASCII format MSB..LSB) [<val1val2...val6>]."
#define CMD_HELP_M              "m: validates the entered MSN against the one in EEPROM (ASCII format MSB..LSB) [<val1val2...val6>]."
#define CMD_HELP_BEEP           "beep: turns buzzer on for 1 second then off."
#define CMD_HELP_MOTOR          "motor [<on|off> <l|u>][num]: motor on or off lock/unlock, if num speified repeat lock/unlock num times, if no argument returns state."
#define CMD_HELP_LED            "led [<g|y|r> <on|off>]: turns leds (green, yellow, red) on or off, if no argument returns state."
#define CMD_HELP_JTAG           "jtag [<on|off>]: turns JTAG pins on or off so they can be used for other functions, if no argument returns state."
#define CMD_HELP_BOOT           "boot [<on|off|crc>]: set bootload mode and reboot (crc=corrupt the CRC to see if it catches it)"
#define CMD_HELP_TEST_SERIAL    "testserial: test the RS232 serial port (requires external loopback)."
#define CMD_HELP_DIAGS          "t [l] [num]: run tests, all tests will run if no options, l lists all the tests, num will select test."


// User commands
int32_t cmd_help(char **arg);


cmd_t cmds[] =
{
    // User Commands
    {"help",        cmd_help,           CONSOLE_CMD_ARG_TYPE_STR,   CMD_HELP_HELP           , 0},
    {"h",           cmd_help,           CONSOLE_CMD_ARG_TYPE_STR,   CMD_HELP_HELP           , 0},
};

#define CONSOLE_CMDS_MAX    (sizeof(cmds)/sizeof(cmd_t))

/* Create task for console */
#define OS_CONSOLE_TASK_STACK_SIZE 64
#define OS_TASK_CONSOLE_PRIO           12
CPU_STK  ConsoleTaskStk[OS_CONSOLE_TASK_STACK_SIZE];

OS_TCB ConsoleTaskStkTCB;


static void console_thread(void *pdata);
OS_SEM ConsoleSem;

void console_init(void) 
{

    OS_ERR      err;
	
		int i;
    cmd_str[0]      = '\0';
    cmd_str_index   = 0;

    cmd_hist_str_index = 0;
    for(i=0; i<CONSOLE_CMD_HIST_MAX; i++)
    {
        cmd_hist_str[i][0] = '\0';
    }
		
		serial_puts("console Init\r\n");
		
		/* Create Task */
		 OSSemCreate(&ConsoleSem,
									"ConsoleSem",
									(OS_SEM_CTR)0,
									&err);
		
		
		  OSTaskCreate((OS_TCB *)&ConsoleTaskStkTCB, 
                (CPU_CHAR *)"console task", 
                (OS_TASK_PTR)console_thread, 
                (void * )0, 
                (OS_PRIO)OS_TASK_CONSOLE_PRIO, 
                (CPU_STK *)&ConsoleTaskStk[0], 
                (CPU_STK_SIZE)OS_CONSOLE_TASK_STACK_SIZE/10, 
                (CPU_STK_SIZE)OS_CONSOLE_TASK_STACK_SIZE, 
                (OS_MSG_QTY) 0, 
                (OS_TICK) 0, 
                (void *)0,
                (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                (OS_ERR*)&err);
	
}



int32_t console_cmd_execute(uint8_t cmd_index, char **arg, uint8_t arg_max)
{
    int32_t rc = 0;

    arg_max = arg_max;
#if 0
    // check argument type
    for(i=0; i<arg_max; i++)
    {
        if(cmds[cmd_index].arg_type == CMD_ARG_TYPE_INT)
        {
            isdigit();
        }
    }
#endif

    // do not execute cmd while record on
    rc = cmds[cmd_index].cmd_fctn(arg);

    return(rc);
}



void cmd_backspace(uint8_t len)
{
    uint8_t i;

    for(i=0; i<len; i++)
    {
        serial_putc(CONSOLE_CMD_BSP);
    }
}

extern ring_buffer Uart_Rxbuffer;



int32_t console_cmd_get(void)
{
    int32_t rc = 0;
    uint8_t  i, len;
	  int8_t c;
		OS_ERR err;
		CPU_TS ts;

//    rc = uart_getc(&c);
//    if(rc != ERR_PASS)
//    {
 //       return(ERR_FAIL);
//    }
		c = RB_pop(&Uart_Rxbuffer);
		if (c < 0) {
				OSSemPend(&ConsoleSem,
									10,	
									OS_OPT_PEND_BLOCKING,
									&ts,
									&err);
				return -1;
				
		}
		
		
		
    switch(c)
    {
        case '\n':
        case '\r':
            if(cmd_str_index == 0)
            {   // erase last command if newline is pressed
                cmd_str[cmd_str_index] = '\0';
            }
            else
            {
                cmd_str_index = 0;
            }
            serial_putc('\n');
            cmd_comment = 0;
            rc = ERR_READY;
            break;

        case CONSOLE_CMD_CTRL_C:
            serial_puts("Ctrl-C pressed\n");
            break;

        case CONSOLE_CMD_DEL:
        case CONSOLE_CMD_BSP:
            if(cmd_str_index > 0)
            {
                len = strlen(&cmd_str[cmd_str_index]) + 1;
                memmove(&cmd_str[cmd_str_index-1], &cmd_str[cmd_str_index], len);
                serial_putc(CONSOLE_CMD_BSP);
                serial_puts(&cmd_str[cmd_str_index]);
                serial_putc(' ');
                cmd_backspace(len);
                cmd_str_index--;
                //cmd_str[cmd_str_index] = '\0';
            }
            break;

        case CONSOLE_CMD_ESC:
          
							if(cmd_str_index > 0)
							{
									for(i=0; i<cmd_str_index; i++)
									{
											serial_putc(CONSOLE_CMD_BSP);
											serial_putc(' ');
											serial_putc(CONSOLE_CMD_BSP);
									}
									cmd_str_index = 0;
									cmd_str[cmd_str_index] = '\0';
							}
            
            break;

        case CONSOLE_CMD_HASH:
            cmd_comment = 1; // comment, ignore following characters except newline
            serial_putc(c);
            break;

        default:
            if(cmd_comment)
            {
                serial_putc(c);
                break;
            }
            if(c<32 || c>126)
            {   // non printable characters
                break;
            }
            cmd_str[cmd_str_index] = c;
            cmd_str_index++;
            if(cmd_str_index >= CONSOLE_CMD_MAX)
            {
                cmd_str_index = 0;
                serial_puts("\nERR: line too long\n");
                cmd_str[cmd_str_index] = '\0';
                rc = ERR_TOO_LONG;
            }
            else
            {
                serial_putc(c);
            }
            cmd_str[cmd_str_index] = '\0';
            break;
    }

    return(rc);
}


void console_update(void)
{
    int32_t rc = 0;
    char *token[CONSOLE_CMD_TOKEN_MAX];
    uint8_t token_cnt = 0;
    uint8_t cmd_index;
    char *p, *tokp;

	  int cmd_found = 0;

    // get command
	  // get command
    rc = console_cmd_get();
    if(rc != ERR_READY)  //1 Ready 
    {
        return;
    }
		
		
	  strcpy(cmd_hist_str[cmd_hist_str_index], cmd_str);
    cmd_hist_str_index++;
    if(cmd_hist_str_index >= CONSOLE_CMD_HIST_MAX)
    {
        cmd_hist_str_index = 0;
    }

    // parse command
    p = strtok_r(cmd_str, CONSOLE_CMD_SEP, &tokp); // parse "cmd arg1, arg2"
    if(p == NULL)
    {	
				serial_puts("\r");
        serial_puts(CONSOLE_CMD_PROMPT);
        return;
    }
		
		 while(p != NULL)
    {
        if(token_cnt >= CONSOLE_CMD_TOKEN_MAX)
        {
//            xlogerr("too many paramaters");
            break;
        }
        token[token_cnt++] = p;
        //xlogmsg("token: %s\n", p);
        p = strtok_r(NULL, CONSOLE_CMD_SEP, &tokp);
        token[token_cnt] = 0;
    }
		

    // lookup command
    for(cmd_index=0; cmd_index<CONSOLE_CMDS_MAX; cmd_index++)
    {
        if(strcmp(token[0], cmds[cmd_index].cmd_name) == 0)
        {
            cmd_found = 1;
            // check for cmd switch
            if(strcmp(token[1], "-h") == 0)
            {
                serial_puts(cmds[cmd_index].cmd_help);
                serial_puts("\n");
            }
            else
            {   // no switch, execute command
                rc = console_cmd_execute(cmd_index, &token[0], token_cnt);
            }
            break;
        }
    }

    if(cmd_found == 0)
    {
        serial_puts("command not found\n");
    }
		serial_puts("\r");
		serial_puts(CONSOLE_CMD_PROMPT);
		
		
		
		
}

static void console_thread(void *pdata) 
{
//		OS_ERR err;
	//  CPU_TS  ts=0;
    pdata = pdata;
		
		while ( 1) {
				console_update();
		}
}


// ------------------------------------------------------------------

int32_t cmd_help(char **arg)
{
    int32_t rc = 0;
    uint8_t i;
    uint8_t show_hidden = 0;

    if(arg[1] != 0)
    {
        if(strcmp(arg[1], "all") == 0)
        {
            show_hidden = 1;
        }
    }

    serial_puts("\nCommands\n");

    for(i=0; i<CONSOLE_CMDS_MAX; i++)
    {
        // show all commands when all option used, otherwise show only not hidden
        if(show_hidden == 1 || (cmds[i].hidden == 0))
        {
//            xlogmsg("  %s\n", cmds[i].cmd_name);
        }
    }

    serial_puts("\nFor detailed help type: <command> -h\n");

    return(rc);
}

