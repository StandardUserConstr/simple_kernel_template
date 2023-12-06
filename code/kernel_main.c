#include "libraries/kernel_lib0.h"

/*
#define VIDEO_X 80
#define VIDEO_Y 25

struct RTC_struct
{
	UINT8 second;
	UINT8 minute;
	UINT8 hour;
	UINT8 day;
	UINT8 month;
	UINT16 year;
}RTC;

UINT8 IN8(UINT16 port);
UINT16 IN16(UINT16 port);
VOID OUT8(UINT16 port, UINT8 data);
VOID OUT16(UINT16 port, UINT16 data);
INT32 get_RTC_update_flag();
UINT8 get_RTC_register(INT32 reg);

VOID uint32_to_string(UINT32 data_in,INT8* const data_out);

VOID disable_cursor();
VOID stop();
VOID read_RTC(struct RTC_struct* rtc_out);
VOID print_string(const INT8* message,const INT32 x,const INT32 y,const INT8 attribute);
VOID print_RTC(struct RTC_struct* rtc_in,const INT32 x,const INT32 y,const INT8 attribute);
VOID print_char(const INT8 char0,const INT32 x,const INT32 y,const INT8 attribute);
VOID delay_cpu_tick(UINT32 time);
VOID start_information();
*/



VOID __main()
{
	disable_cursor();
	initialization();

 	read_RTC(&RTC);
	print_RTC(&RTC,VIDEO_X-20,0,0x0F);	
	INT32 seconds = RTC.second;
	INT8 message[] = {'y','o','u',' ','h','a','v','e',' ','b','e','e','n',' ','l','a','n','d','e','d',' ','i','n','t','o',' ','O','S','\0'};
	for(int i = 26,j = 0;;)
	{
		delay_cpu_tick(700);
		if(*(message+j)!='\0')
		{
			print_char(*(message+j),i,6,0x0F);
			j++;
			i++;
		}
	
		read_RTC(&RTC);
		if(RTC.second!=seconds)
		{
			seconds = RTC.second;
			print_RTC(&RTC,VIDEO_X-20,0,0x0F);
		}	
	}

	stop();
	return;
}




