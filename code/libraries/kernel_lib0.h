#include <stdint.h>
#define KERNEL_PRODUCTION_YEAR 2023 //important to update minimum once at 100 years

#define VIDEO_ADDR 0xB8000
#define VIDEO_X 80
#define VIDEO_Y 25

#define CMOS_IN 0x70
#define CMOS_OUT 0x71

#define INT8 int8_t
#define INT16 int16_t
#define INT32 int32_t
#define UINT8 uint8_t
#define UINT16 uint16_t
#define UINT32 uint32_t
#define VOID void
//#define NULL 0

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
VOID initialization();

VOID initialization()
{
	//VOID delay_cpu_tick(UINT32)
	//
		OUT8(0x40,(65535)&0xFF);
		OUT8(0x40,(65535>>8));
		OUT8(0x43,0b0000000);
	//
	char message0[] = {'s','u','c','c','e','s','s','f','u','l','l','y',' ','l','o','a','d','e','d',' ','k','e','r','n','e','l','\0'};
	print_string(message0,1,2,0x0F);
	char message1[] = {'s','u','c','c','e','s','s','f','u','l','l','y',' ','i','n','i','t','i','a','l','i','z','e','d',' ',
			'3','2','-','b','i','t',' ','P','r','o','t','e','c','t','e','d',' ','M','o','d','e','\0'};
	print_string(message1,1,3,0x0F);
	return;
	
}

VOID print_string(const INT8* message,const INT32 x,const INT32 y,const INT8 attribute)
{
	INT8* operation_video_addr = (INT8*)((VIDEO_ADDR+(VIDEO_X*y)*2)+(x*2));
	INT32 i = 0;
	while(1)
	{
		
		*(operation_video_addr+i) = *message; 
		*(operation_video_addr+i+1) = attribute;
		i+=2;
		message+=1;
		if(*message=='\0') break;
	}
	return;
}

VOID print_char(const INT8 char0,const INT32 x,const INT32 y,const INT8 attribute)
{
	INT8* operation_video_addr = (INT8*)((VIDEO_ADDR+(VIDEO_X*y)*2)+(x*2));
	*(operation_video_addr) = char0; 
	*(operation_video_addr+1) = attribute;
	return;
}

VOID delay_cpu_tick(UINT32 time)
{
	INT32 count0 = 0;
	UINT32 time_variable = 0;
	for(;;)
	{
		count0 += (IN8(0x40)<<8)|(IN8(0x40));
		if(count0>131069)
		{
			count0-=131070;
			time_variable++;
		}
		if(time_variable==time) return;
	} 
}

VOID print_RTC(struct RTC_struct* rtc_in,const INT32 x,const INT32 y,const INT8 attribute)
{
	INT8 data[20];
	uint32_to_string(rtc_in->year,data);
	data[4] = ':';
	if(rtc_in->month<10)
	{
		data[5] = '0';
		data[6] = rtc_in->month+48; 
	}
	else uint32_to_string(rtc_in->month,&data[5]);
	data[7] = ':';
	if(rtc_in->day<10)
	{
		data[8] = '0';
		data[9] = rtc_in->day+48; 
	}
	else uint32_to_string(rtc_in->day,&data[8]);
	data[10] = ':';
	if(rtc_in->hour<10)
	{
		data[11] = '0';
		data[12] = rtc_in->hour+48; 
	}
	else uint32_to_string(rtc_in->hour,&data[11]);
	data[13] = ':';
	if(rtc_in->minute<10)
	{
		data[14] = '0';
		data[15] = rtc_in->minute+48; 
	}
	else uint32_to_string(rtc_in->minute,&data[14]);
	data[16] = ':';
	if(rtc_in->second<10)
	{
		data[17] = '0';
		data[18] = rtc_in->second+48;
		data[19] = '\0';
	}
	else uint32_to_string(rtc_in->second,&data[17]);
	print_string(data,x,y,attribute);
	return;
}

VOID read_RTC(struct RTC_struct* rtc_out)
{
	UINT8 last_second;
	UINT8 last_minute;
	UINT8 last_hour;
	UINT8 last_day;
	UINT8 last_month;
	UINT8 last_year;
	UINT8 registerB;
 
 
	while (get_RTC_update_flag());  //Make sure an update isn't in progress
	rtc_out->second = get_RTC_register(0x00);
	rtc_out->minute = get_RTC_register(0x02);
	rtc_out->hour = get_RTC_register(0x04);
	rtc_out->day = get_RTC_register(0x07);
	rtc_out->month = get_RTC_register(0x08);
	rtc_out->year = get_RTC_register(0x09);
 
	do {
		last_second = rtc_out->second;
		last_minute = rtc_out->minute;
		last_hour = rtc_out->hour;
		last_day = rtc_out->day;
		last_month = rtc_out->month;
		last_year = rtc_out->year;
 
		while (get_RTC_update_flag());  //Make sure an update isn't in progress
		rtc_out->second = get_RTC_register(0x00);
		rtc_out->minute = get_RTC_register(0x02);
		rtc_out->hour = get_RTC_register(0x04);
		rtc_out->day = get_RTC_register(0x07);
		rtc_out->month = get_RTC_register(0x08);
		rtc_out->year = get_RTC_register(0x09);

	}while((last_second != rtc_out->second) || (last_minute != rtc_out->minute) || (last_hour != rtc_out->hour) ||
	(last_day != rtc_out->day) || (last_month != rtc_out->month) || (last_year != rtc_out->year));
 
	registerB = get_RTC_register(0x0B);
 
	//Convert BCD to binary values if necessary (from e.g 0x20 hexa to 20 decimal)
	if (!(registerB & 0x04))
	{
		rtc_out->second = (rtc_out->second & 0x0F) + ((rtc_out->second / 16) * 10);
		rtc_out->minute = (rtc_out->minute & 0x0F) + ((rtc_out->minute / 16) * 10);
		rtc_out->hour = ( (rtc_out->hour & 0x0F) + (((rtc_out->hour & 0x70) / 16) * 10) )|(rtc_out->hour&0x80);
		rtc_out->day = (rtc_out->day & 0x0F) + ((rtc_out->day / 16) * 10);
		rtc_out->month = (rtc_out->month & 0x0F) + ((rtc_out->month / 16) * 10);
		rtc_out->year = (rtc_out->year & 0x0F) + ((rtc_out->year / 16) * 10);
	}
 
	//Convert 12 hour clock to 24 hour clock if necessary
	if(!(registerB & 0x02) && (rtc_out->hour & 0x80)) rtc_out->hour = ((rtc_out->hour & 0x0F)+12)%24;
	
	//Calculate the full (4-digit) year
	rtc_out->year+=(KERNEL_PRODUCTION_YEAR/100)*100;
	if(rtc_out->year<KERNEL_PRODUCTION_YEAR) rtc_out->year += 100;

	return;
}

VOID disable_cursor()
{
	OUT8(0x3D4, 0x0A);
	OUT8(0x3D5, 0x20);
	return;
}


VOID stop()
{
	__asm__ volatile
	(
		"hlt"
	);
	return;
}

VOID uint32_to_string(UINT32 data_in,INT8* const data_out)
{
	INT8 data_rev[10];
	for(INT32 i = 0;; i++)
	{
		if(data_in < 10)
		{
			data_rev[i] = data_in+48;
			data_out[i+1] = '\0';
			for(INT32 j = 0; j!=i+1; j++) data_out[j] = data_rev[i-j];
			return;
		}
		data_rev[i] = (data_in%10)+48;
		data_in = (data_in-(data_in%10))/10;
	}
}

INT32 get_RTC_update_flag()
{
      OUT8(CMOS_IN, 0x0A);
      return (IN8(CMOS_OUT)&0x80);
}
 
UINT8 get_RTC_register(INT32 reg)
{
      OUT8(CMOS_IN, reg);
      return IN8(CMOS_OUT);
}

UINT8 IN8(UINT16 port)
{
	UINT8 result;
	__asm__ volatile
	(
		"in %%dx,%%al" : "=a" (result) : "d" (port)
	);
	return result;
}

UINT16 IN16(UINT16 port)
{
	UINT16 result;
	__asm__ volatile
	(
		"in %%dx,%%ax" : "=a" (result) : "d" (port)
	);
	return result;
}

VOID OUT8(UINT16 port, UINT8 data)
{
	__asm__ volatile
	(
		"out %%al,%%dx" : : "a" (data),"d" (port)
	);
	return;
}

VOID OUT16(UINT16 port, UINT16 data)
{
	__asm__ 
	(
		"out %%ax,%%dx" : : "a" (data),"d" (port)
	);
	return;
}