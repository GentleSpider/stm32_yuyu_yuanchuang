#ifndef ESP8266_WIFI_H_INCLUDE
#define ESP8266_WIFI_H_INLCUDE



#define   AT          "AT\r\n"	
#define   RST         "AT+RST\r\n"	
#define 	CWMODE2			"AT+CWMODE=2\r\n"
#define		CWSAP			 	"AT+CWSAP=\"esp-test\",\"llllllll\",1,3\r\n"
#define		CIPMUX			"AT+CIPMUX=1\r\n"
#define		CIPSERVER		"AT+CIPSERVER=1,8080\r\n"
#define		CIPSTO			"AT+CIPSTO=2880\r\n"

#define	ESP8266_rcv_buf usart2_rcv_buf
#define ESP8266_rcv_len usart2_rcv_len

#define ESP8266_cmd_buf usart2_cmd_buf



extern void ESP8266_Rst(void);
extern void ESP8266_AP_Init(void);
extern void SendCmd(char* cmd, char* result, int timeOut);
extern void TCP_SendData_test(void);
extern int ESP8266_printf(char* cmd);
extern int ESP8266_sprintf(char *fmt, ...);
extern int ESP8266_SendLenStr(u8 *cmd,u16 len);


#endif
