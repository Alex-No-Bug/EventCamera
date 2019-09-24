/* 读取RAW图像*/
#include <pshpack2.h>
#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#define  samples  512
#define  lines    256
#define  bands    16

typedef  unsigned char  BYTE;

/*写入bmp图像*/
typedef  unsigned long DWORD;
typedef unsigned short WORD;
typedef long LONG;

typedef struct {
   	WORD    bfType;//文件类型，必须是0x424D,即字符“BM”
    DWORD   bfSize;//文件大小
    WORD    bfReserved1;//保留字
    WORD    bfReserved2;//保留字
    DWORD   bfOffBits;//从文件头到实际位图数据的偏移字节数
} BITMAPFILEHEADER;//位图文件头

typedef struct{
	  DWORD     	 biSize;//信息头大小
    LONG       biWidth;//图像宽度
    LONG       biHeight;//图像高度
    WORD       biPlanes;//位平面数，必须为1
    WORD       biBitCount;//每像素位数
    DWORD      biCompression;//压缩类型
    DWORD      biSizeImage;//压缩图像大小字节数
    LONG       biXPelsPerMeter;//水平分辨率
    LONG       biYPelsPerMeter;//垂直分辨率
    DWORD      biClrUsed;//位图实际用到的色彩数
    DWORD      biClrImportant;//本位图中重要的色彩数
}BITMAPINFOHEADER;//位图信息头

void saveBITmap(int simple,int line,unsigned char *pData,char* filename)
{
	const int height = line;
	const int width = simple;
	long int size = height * width*3;
	//double x,y;

	//生成文件头
	BITMAPFILEHEADER fileHeader;
	fileHeader.bfType = 0X4d42;
	fileHeader.bfReserved1 = 0;
	fileHeader.bfReserved2 = 0;
	fileHeader.bfSize = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+size;
	fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	//生成信息头
	BITMAPINFOHEADER bitmapHeader = { 0 };
	bitmapHeader.biSize = sizeof(BITMAPINFOHEADER);
  bitmapHeader.biHeight = -height;
  bitmapHeader.biWidth = width;
  bitmapHeader.biPlanes = 1;
  bitmapHeader.biBitCount = 24;
  bitmapHeader.biSizeImage = size;
  bitmapHeader.biCompression = 0; //BI_RGB

  //保存数据
  FILE *fp2 = fopen(filename,"wb");
  if(!fp2){
  	return;
  }
  fwrite(&fileHeader,1,sizeof(BITMAPFILEHEADER),fp2);
  fwrite(&bitmapHeader,1,sizeof(BITMAPINFOHEADER),fp2);
  fwrite(pData,1,size,fp2);
  fclose(fp2);

}

int main()
{
	FILE *fp  = NULL;
    static	BYTE raw_all[samples*lines*bands*2];
    static	BYTE raw_data[samples*lines*bands];//定义为全局变量，防止出现局部变量过大产生bug
	BYTE *ptr;

	if((fp = fopen("log.raw","rb"))==NULL)
	{
		printf("can not open the raw image");
		return 0;
	}
	else
	{
		printf("read OK\n");
	}

	printf("begin read\n");
	ptr = (BYTE*)malloc(samples*lines*bands*sizeof(BYTE)*2);
	for(int i=0;i<samples*lines*bands*2;i++)
	{
		fread(ptr,1,1,fp);
		raw_all[i] = *ptr;
	}

	for(int i=0,j=0;i<samples*lines*bands*2;i+=2)
	{
		raw_data[j] = raw_all[i];
		j=j+1;
	}
static struct{
	 	BYTE B;
	 	BYTE G;
	 	BYTE R;
	} pRGB[lines][samples];
	for(int i=0;i<lines;i++)
	{
		for(int j=0;j<samples;j++)
		{
			BYTE a=raw_data[i*bands*samples+j];
			pRGB[i][j].R=pRGB[i][j].G=pRGB[i][j].B=a;
		}
	}
	saveBITmap(samples,lines,(BYTE*)pRGB,"1.bmp");
	free( ptr);
	return 0;
}
