/*
Save Game Tool by ppcasm 
Arguments/supported games:
 -zeldaoot - Zelda: Ocarina of time
 -mk64 - Mario Kart 64
 -drmario -Dr. Mario
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
void write_word(FILE *fp, uint32_t offset, uint32_t *value);
void write_half(FILE *fp, uint32_t offset, uint16_t *value);
void write_byte(FILE *fp, uint32_t offset, uint8_t *value);
void * ldfile(FILE *fp);

int main(int argc, char *argv[])
{
    if(argc!=3) {printf("Wrong Usage: %s <input filename> <game flag>\n", argv[0]); return 1;}
   
    FILE *fp = fopen(argv[1], "rb+");
    if(!fp) {printf("File <%s> does not exist\n", argv[1]); return 1;}
    
    uint8_t *savefile = (uint8_t *)ldfile(fp);
   
    if(!strcmp(argv[2], "-zelda:oot"))
    {
         printf("Zelda: Ocarina of time\n");
         uint16_t chksum = halfword_chksum(savefile, 0x20, 0x9a9);
         write_half(fp, 0x1372, chksum);
    }
    
    if(!strcmp(argv[2], "-wr64"))
    {
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
    
    if(!strcmp(argv[2], "-drmario"))
    {
          printf("Dr. Mario\n");
          //0x800E51b0
    }
    
    free(savefile);
    fclose(fp);
    return 0;
}

uint32_t word_chksum(uint8_t *buffer, uint32_t start, uint32_t end)
{
    uint32_t *data = (uint32_t *)buffer+(start/sizeof(uint32_t));
    uint32_t sum = 0;
    int i;
    
    for( i = 0; i < end; i++ )
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
     uint8_t *data = (uint8_t *)buffer+(start/sizeof(uint8_t));
     uint8_t sum = 0;
     int i;
    
     for( i = 0; i < end; i++ )
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

void write_half(FILE *fp, uint32_t offset, uint16_t *value)
{
     fseek(fp, offset, SEEK_SET);
     uint16_t *valuez = (uint16_t *)&value;
     valuez[0] = SWAP2(valuez[0]);
     fwrite(valuez, sizeof(uint16_t), 1, fp);
     fseek(fp, 0, SEEK_SET);
}
     
void write_word(FILE *fp, uint32_t offset, uint32_t *value)
{
     fseek(fp, offset, SEEK_SET);
     uint32_t *valuez = (uint32_t *)&value;
     valuez[0] = SWAP4(valuez[0]);
     fwrite(valuez, sizeof(uint32_t), 1, fp);
     fseek(fp, 0, SEEK_SET);
     
}

void write_byte(FILE *fp, uint32_t offset, uint8_t *value)
{
     fseek(fp, offset, SEEK_SET);
     uint8_t *valuez = (uint8_t *)&value;
     fwrite(valuez, sizeof(uint8_t), 1, fp);
     fseek(fp, 0, SEEK_SET);
}
