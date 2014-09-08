/*
Save Game Tool - ppcasm (2014)

Arguments/supported games:
 -zelda:oot - Zelda: Ocarina of time
 -wr64 - Wave Race 64
 -yoshistory - Yoshi's Story
 -sf64 - StarFox 64
 -eb64 - ExciteBike 64
 -nbac2 - Kobe Bryant NBA Courtside 2

This tool replaces the "checksum" value in savegame files. The idea
behind the tool is that you can manually edit the save game file 
(such as in a hex editor) and then run this tool over it to generate 
the proper checksum value for the specified game so that it will load on N64. 

NOTE: For games that have backup saves embedded inside of them, this tool only
supports the first half of the save (second half is usually backup.) The reason for this
is so that in case something does go wrong, you won't lose your entire save. Games with a 
backup save file embedded in them will usually first check key structures and/or checksum value  
against the data, and if it doesn't match then it will default to the second half, which is
usually the backup. 

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#define SWAP4(x) ( ((x) << 24) | \
         (((x) << 8) & 0x00ff0000) | \
         (((x) >> 8) & 0x0000ff00) | \
         ((x) >> 24) )
         
#define SWAP2(x) ( (((x) & 0xff) << 8) | ((unsigned short)(x) >> 8) )

typedef unsigned short uint16_t;
typedef unsigned long uint32_t;
typedef unsigned char uint8_t;

uint8_t byte_chksum(uint8_t *buffer, uint32_t start, uint32_t end);
uint16_t halfword_chksum(uint8_t *buffer, uint32_t start, uint32_t end);
uint32_t word_chksum(uint8_t *buffer, uint32_t start, uint32_t end);
void write_word(FILE *fp, uint32_t offset, uint32_t value);
void write_half(FILE *fp, uint32_t offset, uint16_t value);
void write_byte(FILE *fp, uint32_t offset, uint8_t value);
void * ldfile(FILE *fp);

int main(int argc, char *argv[])
{
   if(argc!=3) {printf("Wrong Usage: %s <input filename> <game flag/-list>\n", argv[0]); return 1;}
   
   int argfl = 0;

   FILE *fp = fopen(argv[1], "rb+");
   if(!fp) {printf("File <%s> does not exist\n", argv[1]); return 1;}
    
   uint8_t *savefile = (uint8_t *)ldfile(fp);
   if (savefile == NULL)  exit(1);

   if(!strcmp(argv[2], "-zelda:oot"))
    {
		 argfl = 1;

         printf("Zelda: Ocarina of time\n");
         uint16_t chksum = halfword_chksum(savefile, 0x20, 0x9a9);
		 write_half(fp, 0x1372, chksum);
    }
    
   if(!strcmp(argv[2], "-wr64"))
    {
		 argfl = 1;

         printf("Wave Race 64\n");
        
         uint32_t *waverace = (uint32_t *)savefile;
      
         uint8_t t6 = 0;
         uint8_t t8 = 0;
         uint8_t t0 = 0;
         uint8_t t2 = 0;
         uint16_t t3 = 0;
         
         int t7 = 0x3fd;
         int t9 = 0;
         int t1 = 0;
         int v1 = 0x6fa;
         
         int i = 0;
         for(i=4;i<=0x1fc/4;i++)
         {
                 t6 = (waverace[i]&0x000000ff);
                 t8 = (waverace[i]&0x0000ff00)<<8>>16;
                 t0 = (waverace[i]&0x00ff0000)<<8>>24;
                 t2 = (waverace[i]&0xff000000)>>24; 
                 
                 t7 = t6+v1;
                 t9 = t7+t8;
                 t1 = t9+t0;
                 v1 = t1+t2;
                 t3 = v1&0xffff;
                 
         }
         write_half(fp, 2 ,t3);
    }
    
    if(!strcmp(argv[2], "-yoshistory"))
    {
		argfl = 1;

		printf("Yoshi's Story\n\n");
	
		unsigned int i = 0;

		uint32_t v1 = savefile[0];
		uint32_t t9 = savefile[1];
		uint32_t t6 = 0;
		uint32_t t7 = 0;
		uint32_t t8 = 0;
		uint32_t t3 = 0;
		uint32_t t0 = 0;
		uint32_t t4 = 0;
		uint32_t t5 = 0;
		uint32_t t1 = 0;
		uint32_t t2 = 0;

		v1 = v1<<1;
		t7 = v1>>0x10;
		t8 = t7&1;
		t6 = v1&0xfffe;
		v1 = t6|t8;
		v1 = v1^t9;
		v1 = v1<<1;
		t1 = v1>>0x10;
		t2 = t1&1;
		t0 = v1&0xfffe;
		v1 = t0|t2;
		
		for(i=2;i<0x3fa;i+=4)
		{
			t3 = savefile[i];
			t6 = savefile[i+1];
			t0 = savefile[i+2];
			
			v1 = v1^t3;
			v1 = v1<<1;
			t5 = v1>>0x10;
			t7 = t5&1;
			t4 = v1&0xfffe;
			v1 = t4|t7;
			v1 = v1^t6;
			v1 = v1<<1;
			t9 = v1>>0x10;
			t1 = t9&1;
			t8 = v1&0xfffe;
			v1 = t8|t1;
			v1 = v1^t0;
			v1 = v1<<1;
			t4 = savefile[i+3];
			t3 = v1>>0x10;
			t5 = t3&1;
			t2 = v1&0xfffe;
			v1 = t2|t5;		
			v1 = v1^t4;
			v1 = v1<<1;
			t6 = v1>>0x10;
			t9 = t6&1;
			t7 = v1&0xfffe;
			v1 = t7|t9;
		
		}

			write_half(fp, 0x3fa, v1); 		      
    }
    
    if(!strcmp(argv[2], "-sf64"))
    {
		argfl = 1;

    	printf("StarFox 64\n");
	
		uint32_t T0 = 0;
		uint32_t T1 = 0;
		uint32_t T2 = 0;
		uint32_t T3 = 0;
		uint32_t T4 = 0;
		uint32_t T5 = 0;
		uint32_t T6 = 0;
		uint32_t T7 = 0;
		uint32_t T8 = 0;
		uint32_t T9 = 0;
		uint32_t V0 = 0;
		uint32_t V1 = 0;
		
		int i = 0;

		T6 = savefile[i];
		T3 = savefile[i];
		
		T9 = T6<<1;
		T0 = T9>>8;
		T1 = T0&1;
		T2 = T9|T1;
		T4 = T2^T3;
		T6 = T4<<1;
		T8 = T6>>8;
		T0 = T8&1;
		T7 = T6&0xfe;
		V1 = T7|T0;	
		T9 = V1&0xffff;
		V1 = T9|0;
		
		for(i=2;i<0xfe;i+=4)
		{
	
			T1 = savefile[i];
			T0 = savefile[i+1];
			T2 = V1^T1;
			T5 = T2<<1;
			T6 = T5>>8;
			T8 = T6&1;
			T7 = T5|T8;
			T9 = T7^T0;
			T3 = T9<<1;
			T8 = savefile[i+2];
			T4 = T3>>8;
			T6 = T4&1;
			T5 = T3|T6;
			T7 = T5^T8;
			T1 = T7<<1;
			T6 = savefile[i+3];
			T2 = T1>>8;
			T4 = T2&1;
			T3 = T1|T4;
			T5 = T3^T6;
			T7 = T5<<1;
			T9 = T7>>8;
			T2 = T9&1;
			T0 = T7&0xfe;
			V1 = T0|T2;
			T1 = V1&0xffff;
			V1 = T1|0;
		}
			T4 = T1&0xff;
			T3 = T4|0x9500;
			V0 = T3&0xffff;
			
			write_half(fp, 0xFE, V0); 
	}

 	if(!strcmp(argv[2], "-eb64"))
    {
		argfl = 1;

		printf("ExciteBike 64\n");

		uint32_t *excitebike = (uint32_t *)savefile;

		uint32_t sumz = 0;
		unsigned int i = 0;
		
		excitebike[0]=0x5748FB3A;
	    sumz = excitebike[0];
		sumz += word_chksum(savefile, 0x2, 0x200-1);
		sumz = sumz^0xFCB1121D;
		write_word(fp, 4, sumz);
	
	}

	if(!strcmp(argv[2], "-nbac2"))
    {
		argfl = 1;

		printf("Kobe Bryant NBA Courtside 2\n");
	  
		uint32_t *nba2 = (uint32_t *)savefile;

		unsigned long V0 = 0;
		unsigned long V1 = 0xfffffbe8;
	
		unsigned int i = 0;
		for(i=1;i<=106;i++)
		{
			V0 = SWAP4(nba2[i]);
			V1 = V1+V0;

		}

		write_word(fp, 0, 0-V1);

	}

	if(!strcmp(argv[2], "-list"))
    {
		argfl = 1;

		printf("Usage: %s <filename_of_save> <game_arg>\n", argv[0]);
		printf("<game_arg>\n(supported games list):\n");
		printf(" -zelda:oot - Zelda: Ocarina of time\n");
		printf(" -wr64 - Wave Race 64\n");
 		printf(" -yoshistory - Yoshi's Story\n");
 		printf(" -sf64 - StarFox 64\n");
 		printf(" -eb64 - ExciteBike 64\n");
		printf(" -nbac2 - NBA Courtside 2\n");

	}

    if(!argfl) printf("No such option: %s : Try -list to see a list of supported games.\n", argv[2]);
	
    free(savefile);
    fclose(fp);
    return 0;
}

uint32_t word_chksum(uint8_t *buffer, uint32_t start, uint32_t end)
{
    uint32_t *data = (uint32_t *)buffer;
    uint32_t sum = 0;
    int i;
    
    for( i = start; i < end; i++ )
     {
        sum += SWAP4(data[i]);
     }
    
     return sum;
}
    
uint16_t halfword_chksum(uint8_t *buffer, uint32_t start, uint32_t end )
{
     uint16_t *data = (uint16_t *)buffer+(start/sizeof(uint16_t));
     uint16_t sum = 0;
     int i;
    
     for( i = 0; i < end; i++ )
     {
        sum += SWAP2(data[i]);
     }
    
     return sum;
} 

uint8_t byte_chksum(uint8_t *buffer, uint32_t start, uint32_t end)
{
     uint8_t *data = (uint8_t *)buffer;
     uint8_t sum = 0;
     int i;
    
     for( i = start; i < end; i++ )
     {
        sum += data[i];
     }
    
     return sum;
}

void * ldfile(FILE *fp)
{
     uint32_t filelen;
     fseek(fp, 0, SEEK_END);
     filelen=ftell(fp);
     fseek(fp, 0, SEEK_SET);
     uint8_t *savebuf = (uint8_t *)malloc(sizeof(uint8_t)*filelen+1);
     if(!savebuf)
     {
             printf("Could not allocate memory.\n");
             return;
     }
     
     fread(savebuf, sizeof(char), filelen, fp);
     return savebuf;
}

void write_half(FILE *fp, uint32_t offset, uint16_t value)
{
     fseek(fp, offset, SEEK_SET);
     uint16_t *valuez = (uint16_t *)&value;
     valuez[0] = SWAP2(valuez[0]);
     fwrite(valuez, sizeof(uint16_t), 1, fp);
     fseek(fp, 0, SEEK_SET);
}
     
void write_word(FILE *fp, uint32_t offset, uint32_t value)
{
     fseek(fp, offset, SEEK_SET);
     uint32_t *valuez = (uint32_t *)&value;
     valuez[0] = SWAP4(valuez[0]);
     fwrite(valuez, sizeof(uint32_t), 1, fp);
     fseek(fp, 0, SEEK_SET);
     
}

void write_byte(FILE *fp, uint32_t offset, uint8_t value)
{
     fseek(fp, offset, SEEK_SET);
     uint8_t *valuez = (uint8_t *)&value;
     fwrite(valuez, sizeof(uint8_t), 1, fp);
     fseek(fp, 0, SEEK_SET);
}
