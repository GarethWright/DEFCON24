/*
# Copyright 2016 Aaron Luo
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#--------------------------------------
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <ctype.h>
#include <unistd.h> 
#define MAX_PATH 256
char mypath[MAX_PATH];
void ReadData(void);
int fd;
int longitude,latitude;
char *state = (char*)"no-fix";
char *fake_state = (char*)"Normal....";
void hexdump(void *ptr, int buflen) {
  unsigned char *buf = (unsigned char*)ptr;
  int i, j;
  for (i=0; i<buflen; i+=16) {
    printf("%06x: ", i);
    for (j=0; j<16; j++) 
      if (i+j < buflen)
        printf("%02x ", buf[i+j]);
      else
        printf("   ");
    printf(" ");
    for (j=0; j<16; j++) 
      if (i+j < buflen)
        printf("%c", isprint(buf[i+j]) ? buf[i+j] : '.');
    printf("\n");
  }
}
void alert(void)
{
        #define maxstrlen 256
        char commandline[maxstrlen];
        snprintf(commandline,maxstrlen,"python %salert.py",mypath);
        system(commandline);
}
void lcd_output(char *line1,char* line2)
{
	#define maxstrlen 256
	char commandline[maxstrlen];
	snprintf(commandline,maxstrlen,"python %slcd_i2c.py %s %s",mypath,line1,line2);
	system(commandline);
}
int main(int argc,char **argv)
{
	strcpy(mypath,argv[0]);
	int plen = strlen(mypath);
	for (int i=plen-1;i>=0;i--)
	{
		if (mypath[i] == '/')
		{
			mypath[i+1] = '\x0';
			break;
		}
	}
	if ((fd = serialOpen ("/dev/ttyUSB0", 9600)) < 0)
	{
		fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno)) ;
		return 1;
	}	
	/*Reset*/
	write(fd,"\xB5\x62\x06\x09\x0D\x00\xFF\xFB\x00\x00\x00\x00\x00\x00\xFF\xFF\x00\x00\x17\x2B\x7E",21);
	write(fd,"\x24\x47\x4E\x54\x58\x54\x2C\x30\x31\x2C\x30\x31\x2C\x30\x32\x2C\x52\x65\x73\x65\x74\x74\x69\x6E\x67\x20\x47\x4E\x53\x53\x2A\x33\x42\x0D\x0A",35);
	write(fd,"\xB5\x62\x05\x01\x02\x00\x06\x09\x17\x40",10);
	/*Disable NMEA*/
	write(fd,"\xB5\x62\x06\x01\x03\x00\xF0\x0A\x00\x04\x23",11); 
	write(fd,"\xB5\x62\x06\x01\x03\x00\xF0\x09\x00\x03\x21",11); 
	write(fd,"\xB5\x62\x06\x01\x03\x00\xF0\x00\x00\xFA\x0F",11); 
	write(fd,"\xB5\x62\x06\x01\x03\x00\xF0\x01\x00\xFB\x11",11); 
	write(fd,"\xB5\x62\x06\x01\x03\x00\xF0\x0D\x00\x07\x29",11); 
	write(fd,"\xB5\x62\x06\x01\x03\x00\xF0\x06\x00\x00\x1B",11); 
	write(fd,"\xB5\x62\x06\x01\x03\x00\xF0\x02\x00\xFC\x13",11); 
	write(fd,"\xB5\x62\x06\x01\x03\x00\xF0\x07\x00\x01\x1D",11); 
	write(fd,"\xB5\x62\x06\x01\x03\x00\xF0\x03\x00\xFD\x15",11); 
	write(fd,"\xB5\x62\x06\x01\x03\x00\xF0\x04\x00\xFE\x17",11); 
	write(fd,"\xB5\x62\x06\x01\x03\x00\xF0\x0F\x00\x09\x2D",11); 
	write(fd,"\xB5\x62\x06\x01\x03\x00\xF0\x05\x00\xFF\x19",11); 
	write(fd,"\xB5\x62\x06\x01\x03\x00\xF0\x08\x00\x02\x1F",11); 
	write(fd,"\xB5\x62\x06\x01\x03\x00\xF1\x00\x00\xFB\x12",11); 
	write(fd,"\xB5\x62\x06\x01\x03\x00\xF1\x01\x00\xFC\x14",11); 
	write(fd,"\xB5\x62\x06\x01\x03\x00\xF1\x03\x00\xFE\x18",11); 
	write(fd,"\xB5\x62\x06\x01\x03\x00\xF1\x04\x00\xFF\x1A",11); 
	write(fd,"\xB5\x62\x06\x01\x03\x00\xF1\x05\x00\x00\x1C",11); 
	write(fd,"\xB5\x62\x06\x01\x03\x00\xF1\x06\x00\x01\x1E",11); 
	write(fd,"\xB5\x62\x06\x01\x03\x00\xF0\x00\x00\xFA\x0F",11); 
	write(fd,"\xB5\x62\x06\x01\x03\x00\xF0\x01\x00\xFB\x11",11);
	/*Enable UBX-RXM-SFRBX*/
	write(fd,"\xB5\x62\x06\x01\x03\x00\x02\x13\x01\x20\x6C",11);
	/*Enable UBX-NAV-POSLLH*/
	write(fd,"\xB5\x62\x06\x01\x03\x00\x01\x02\x01\x0E\x47",11);
	/*Enable UBX-NAV-STATUS*/
	write(fd,"\xB5\x62\x06\x01\x03\x00\x01\x03\x01\x0F\x49",11);
	while(true)
	{
		ReadData();
	}
	close(fd);
	return 0;
}
#define MAX_BUFFERSIZE 0x1000
char g_buffer [MAX_BUFFERSIZE];
unsigned int g_currentptr;
enum {status_none,status_getheaderok,status_getclassidok,status_getlengthok,status_getdataok,status_getchecksumok};
void updateinfo (void)
{
	char line1[20];
	char line2[20];
	sprintf(line1,"%s%s",fake_state,state);
	sprintf(line2,"%.4f,%.4f",longitude/10000000.0,latitude/10000000.0);
	lcd_output((char*)line1,(char*)line2);
}
void parseubx(unsigned char majorid,unsigned char minorid,unsigned char *data,unsigned int datalen)
{
	if (majorid == 0x02 && minorid == 0x13) //UBX-RXM-SFRBX
	{
		if (data[0] == 0x0) //only process the GPS
		{
			if (datalen >= 4*10+8)
			{
				unsigned int sbf[10];
				memcpy(&sbf[0],data+8,4*10);
				printf("%08x %08x %08x %08x %08x %08x %08x %08x %08x %08x\n",sbf[0],sbf[1],sbf[2],sbf[3],sbf[4],sbf[5],sbf[6],sbf[7],sbf[8],sbf[9]);
				int subframeid = 0x7&sbf[1]>>8;
				printf("svid:%d subframe:%d\n",data[1],subframeid);
				int iweek,itow;
				itow = sbf[1]>>13; 
				printf("itow:%d\n",itow); //toe toc
				if (subframeid == 1)
				{
				   iweek = int(sbf[2] >> 20);
				   itow = sbf[1]>>13; 
				   printf("week: %d\n",iweek);
				}
				else if (subframeid == 2)
				{
				}
				else if (subframeid == 3)
				{
				}
				else if (subframeid == 4)
				{
					if (sbf[3] == 0)
					{
						fake_state = (char*)"Fake-GPS..";
						printf("Fake GPS detected! Satellite ID: %d\n",data[1]);
						updateinfo();
						alert();
					}
					else
					{
						fake_state = (char*)"Normal....";
						updateinfo();
					}
				}
				else if (subframeid == 5)
				{
					if (sbf[3] == 0)
					{
						fake_state = (char*)"Fake-GPS..";
						printf("Fake GPS detected! Satellite ID: %d\n",data[1]);
						updateinfo();
						alert();
					}
					else
					{
						fake_state = (char*)"Normal....";
						updateinfo();
					}
				}
			}
		}
	}
	else
	if (majorid == 0x01 && minorid == 0x02) //UBX-NAV-POSLLH
	{
		int itow = *(int*)data;
		longitude = ((int*)data)[1];
		latitude = ((int*)data)[2];
		itow /= 1000;
		int day = itow/60/60/24;
		int hour = itow/60/60-24*day;
		int minute = itow/60-day*60*24-hour*60;
		int second = itow-day*60*60*24-hour*60*60-minute*60;
		printf("time: %02d:%02d:%02d:%02d %f %f\n",day,hour,minute,second,longitude/10000000.0,latitude/10000000.0);

	}
	if (majorid == 0x01 && minorid == 0x03) //UBX-NAV-STATUS
	{
		char gpsfix = data[4];
		switch(gpsfix)
		{
			 case 0x00:
				 printf("no-fix\n");
				 state = (char*)"no-fix";
				 updateinfo();
				 break;

			 case 0x01:
				 printf("dead reckoning only\n");
				 break;

			 case 0x02:
				 printf("2D-fix\n");
				 state = (char*)"2D-fix";
				 updateinfo();
				 break;

			 case 0x03:
				 printf("3D-fix\n");
				 state = (char*)"3D-fix";
				 updateinfo();
				 break;

			 case 0x04:
				 printf("GPS + dead reckoning combined\n");
				 break;

			 case 0x05:
				 printf("Time only fix\n");
				 break;

			 default:
				 printf("Reserved value. Current state unknown\n");
				 break;

		}
	}
}
void ReadData(void)
{
	#define tmpbuffersize 0x100
	unsigned char tmpbuffer[tmpbuffersize];
	g_currentptr = 0;
	unsigned int nBytesRead;
	int stage = status_none;
	while (serialDataAvail(fd)!=-1)
	{
		int nBytesRead = read(fd,&tmpbuffer,tmpbuffersize);
		if (nBytesRead)
		{
			if ((g_currentptr+nBytesRead)<MAX_BUFFERSIZE)
			{
				memcpy(&g_buffer[g_currentptr],&tmpbuffer,nBytesRead);
				g_currentptr+=nBytesRead;
			}
			else
			{
				g_buffer[0] = g_buffer[g_currentptr-1];//if last byte = 0xB5...
				memcpy(&g_buffer[1],tmpbuffer,nBytesRead);
				g_currentptr = 1+nBytesRead;
			}
			if (stage == status_none)
			{
				bool found = false;
				int header_found = 0;
				for (unsigned int i=0;i<g_currentptr-1;i++)
				{
					if (g_buffer[i]=='\xB5' && g_buffer[i+1]=='\x62')
					{
						found = true;
						header_found = i;	
						break;
					}
				}
				if (found)
				{
					memcpy(&g_buffer[0],&g_buffer[header_found],g_currentptr-header_found);
					g_currentptr = g_currentptr-header_found;
					stage = status_getheaderok;	
				}
			}
			if (stage == status_getheaderok)
			{
				if (g_currentptr>=6)
				stage = status_getlengthok;
			}
			if (stage == status_getlengthok)
			{
				unsigned int datalen = *(unsigned short*)((unsigned char*)&g_buffer[4]);
				if (g_currentptr>=6+datalen+2)
				{
					unsigned char checksumA = 0;
					unsigned char checksumB = 0;
					for (unsigned int j=2;j<6+datalen;j++)
					{
						checksumA = checksumA + g_buffer[j];
						checksumB = checksumB + checksumA;
					}
					if ((checksumA == (unsigned char)g_buffer[6+datalen]) && (checksumB == (unsigned char)g_buffer[6+datalen+1]))
					{
						parseubx(g_buffer[2],g_buffer[3],(unsigned char*)&g_buffer[6],datalen);
					}
					memcpy(&g_buffer[0],&g_buffer[6+datalen+2],g_currentptr-(6+datalen+2));
					g_currentptr = g_currentptr-(6+datalen+2);
					stage = status_none;
				}
			}
		}
	}
}
