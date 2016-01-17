/* 
ImageProcessWithC 
Copyright (C) 2015 KosWu
KosWu's Blog: http://blog.koswu.com
Email :ws00298046@163.com
*/
/*HeadFile Declaration*/
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
/*struct align*/
#pragma pack(2)
/*Donstant define*/
#define STARBAR "*********************************************"
#define NAMEMAX 40
#define BI_RGB 0
#define BI_RLE8 1
#define BI_RLE4 2
#define BI_BITFIELDS 3
/*Typedef about variable*/
typedef unsigned int DWORD; 
typedef int LONG;
typedef unsigned short WORD;
typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned char BYTE;
 /* FILEHEADER */
 typedef struct tagBITMAPFILEHEADER
{
WORD bfType;  
DWORD bfSize; 
WORD bfReserved1;  
WORD bfReserved2; 
DWORD bfOffBits; 
} fileheader;
typedef struct{ /* BITMAPheader */ 
DWORD biSize;
LONG biWidth;
LONG biHeight;
WORD biPlanes; 
WORD biBitCount;
DWORD biCompression; 
DWORD biSizeImage; 
LONG biXPelsPerMeter; 
LONG biYPelsPerMeter; 
DWORD biClrUsed; 
DWORD biClrImportant;

} bitmapheader;
/*COLORTABLE*/
typedef struct {
        BYTE    rgbBlue;
        BYTE    rgbGreen;
        BYTE    rgbRed;
        BYTE    rgbReserved;
} RGBQUAD;
/*BITMAPDATA*/
typedef struct
{
	uchar * mapdata;
	uint mapheight;
	uint mapwidth;
	uint linebites;
	RGBQUAD * colortablefp;
	int mapimagebit;
	char * filename;
}bmpData;
/*IMAGE pixel with 24-bit image*/
typedef struct
{
	char R;
	char G;
	char B;
}colorimage;
/*function*/
bool readfilehead (FILE * file);
bool readbmp (bmpData * temp);
bool writebmp (bmpData map);
bool write4ascii (bmpData bmp,char filename[]);
bool bmpzoom (bmpData * temp,float scale);
char * chargets (char *buf,int bufsize,FILE * steam);
bool write4ascii (bmpData map,char filename[]);
bool bmpgra (bmpData * temp);
/*main*/
int main (void)
{
	/*Bariavle*/
	bmpData bmp;
	char filename[NAMEMAX];
	char ch;
	float scale;
	char asciiname[NAMEMAX];
	/*Copyright*/
	puts ("Welcome to use ImageProcessWithC");
	puts ("Copyright (C) 2015 KosWu");
	puts (STARBAR);
	/*Get file path*/
	GETFILE:fputs ("Please input bmp image path\n(Example: /sdcard/xx.bmp):",stdout); 
	chargets (filename,NAMEMAX-1,stdin);
	bmp.filename=filename;
	/*Read bmp*/
	while (!readbmp (&bmp))
	{
		fputs ("Read file false，Please try another file:",stdout);
		chargets (filename,NAMEMAX-1,stdin);
	bmp.filename=filename;
	}
	/*Print menu*/
	while (ch!='q')
	{
		puts (STARBAR);
		puts ("Function menu");
		puts ("a)Image resizing b)Image grayimn");
		puts ("c)Output file with ASCII PRINT d)Image save to file");
		puts ("r)read Another image  q)quite");
		puts (STARBAR);
		/*Read input*/
		fputs ("Please input your select: ",stdout);
		ch=getchar ();
		while (getchar()!='\n');//Absorb Enter
		switch (ch)
		{
			/*Zooming*/
			case 'a':
			fputs ("Please input zoom level :",stdout);
			scanf ("%f",&scale);
			while (getchar()!='\n');//Absorb Enter 
			if (!bmpzoom (&bmp,scale))
			{
				puts ("Error!");
			}
			else
			{
				puts ("Done!");
			}
			break;
			/*Graying*/
			case 'b':
			if (!bmpgra (&bmp))
			{
				puts ("Error!");
			}
			else
			{
				puts ("Done!");
			}
			break;
			/*ASCII Print*/
			case 'c':
			fputs ("Please  input save path(If file exists,Will be output to tail)\(Example: /sdcard/ascii.txt):",stdout);
			chargets (asciiname,NAMEMAX-1,stdin);
			if (!write4ascii (bmp,asciiname))
			{
				puts ("Output Error!");
			}
			else
			{
				puts ("Output Done!");
			}
			break;
			/*Save bmp*/
			case 'd' :
			fputs ("Please input The save file path(If file exists,Will cover files\n(Example: /sdcard/picture.bmp):",stdout);
	chargets (bmp.filename,NAMEMAX-1,stdin);
	if (!writebmp(bmp))
			{
				puts ("Save Error!");
			}
			else
			{
				puts ("Save Done!");
			}
			break;
			/*Read Again*/
			case 'r':
			/*Free Space*/
			free (bmp.mapdata);
			if (bmp.mapimagebit<16)
			{
				free (bmp.colortablefp);
			}
			goto GETFILE;//Return to get The file path
			break;
			case 'q':
			puts ("Program Finish,Good Bye!");
			return 0;
			default:
			puts ("Input Error,Please input Again!");
		}
	}
	return 0;
}
/*Filehead Read Function*/
bool readfilehead (FILE * file)
{
	/*Read filehead*/
	fileheader headtemp;
	fread (&headtemp,sizeof (fileheader),1,file);
	if (headtemp.bfType!=0x4D42)
	{
		puts ("The File isn't a bitmap file!");
		return false;
	}
	printf ("The file is bitmap file,The image size is%ubyte\n",headtemp.bfSize);
	return true;
}
/*Bitmap read Function*/
bool readbmp (bmpData * temp)
{
	/*Variable*/
	FILE * fp;
	bitmapheader map;
	/*Open image file*/
	fputs ("Opening The file……",stdout);
	if ((fp=fopen (temp->filename,"rb"))==NULL)
	{
		puts ("Error");
		return false;
	}
	puts ("Done");
	/*read and skip filehead*/
	if (!readfilehead (fp))
	{
		return false;
	}
	fseek (fp,sizeof (fileheader),SEEK_SET);
	/*read maphead*/
	bitmapheader maphead;
	fread (&maphead,sizeof(bitmapheader),1,fp);
	printf ("The Image Height is%dpixel，Width is%dpixel\nThe image%ubitmap\n",maphead.biHeight,maphead.biWidth,maphead.biBitCount);
	puts (STARBAR);
	if (maphead.biCompression!=BI_RGB)
	{
		puts ("The Image is compound image,The process can't support!");
		return false;
	}
	//Read bmpData
	temp->mapheight=maphead.biHeight;
	temp->mapwidth=maphead.biWidth;
	temp->mapimagebit=maphead.biBitCount;
	//Calculate image size
	temp->linebites=(maphead.biBitCount*maphead.biWidth/8+3)/4*4;
	//Read Color Table
	if (maphead.biBitCount<16)
	{
		/*Create Color table Space*/
		temp->colortablefp=(RGBQUAD *)malloc (sizeof (RGBQUAD)*pow(2,maphead.biBitCount));
		fread (temp->colortablefp,sizeof (RGBQUAD),pow (2,maphead.biBitCount),fp);
	}
	/*Create Data Space*/
	temp->mapdata=(uchar *) malloc (temp->linebites*temp->mapheight);
	/*Read Mapdata*/
	fread(temp->mapdata,1,(temp->linebites)*(temp->mapheight),fp); 
	/*Close File*/
	if (fclose (fp)==EOF)
	{
		puts ("Close Error!");
	}
	return true;
}
/*Output Image*/
bool writebmp (bmpData map)
{
	/*Variable*/
	fileheader filehead;
	bitmapheader maphead;
	FILE * output;
	int temp;
	/*To determine if there is a data structure*/
	if (map.mapimagebit<1||map.mapimagebit>36)
	{
		return false;
	}
	/*Open file*/
	if ((output=fopen (map.filename,"wb"))==NULL)
	{
		puts ("Save file Error!");
	}
	/*Calculat color table size*/
	if (map.mapimagebit<16)
	{
		temp=sizeof (RGBQUAD)*pow(2,map.mapimagebit);
	}
	else
	{
		temp=0;
	}
	/*Write filehead*/
	filehead.bfType=0x4D42;
	filehead.bfSize=temp+sizeof (fileheader)+sizeof (bitmapheader)+(map.linebites)*map.mapheight;
	filehead.bfReserved1=0;
	filehead.bfReserved2=0;
	filehead.bfOffBits=sizeof (fileheader)+sizeof (bitmapheader)+temp;
	/*Write MapData Head*/
	maphead.biSize=40;
	maphead.biWidth=map.mapwidth;
	maphead.biHeight=map.mapheight;
	maphead.biPlanes=1;
	maphead.biBitCount=map.mapimagebit;
	maphead.biCompression=BI_RGB;
	maphead.biXPelsPerMeter=0;
	maphead.biYPelsPerMeter=0;
	maphead.biClrUsed=temp/sizeof (RGBQUAD);
	maphead.biClrImportant=0;
	/*Write to file*/
	fwrite (&filehead,sizeof (fileheader),1,output);
	fwrite (&maphead,sizeof (bitmapheader),1,output);
	if (map.mapimagebit<16)
	{
		fwrite (map.colortablefp,temp,1,output);
	}
	fwrite (map.mapdata,map.linebites*map.mapheight,1,output);
	/*Close File*/
	if ((fclose (output))==EOF)
	{
		puts ("Close File Error!");
		return false;
	}
	return true;
}
/*Image zoom Function*/
bool bmpzoom (bmpData * temp,float scale)
{
	/*Variable*/
	bmpData newbmp;
	int linedataize;
	long i,j;
	/*Write maphead*/
	newbmp.mapheight=(long)(temp->mapheight*scale+0.5);
	newbmp.mapwidth=(long)(temp->mapwidth*scale+0.5);
newbmp.mapimagebit=temp->mapimagebit;
			newbmp.linebites=(newbmp.mapimagebit*newbmp.mapwidth/8+3)/4*4;
		newbmp.colortablefp=temp->colortablefp;
		newbmp.filename=temp->filename;
		/*Create mapdata Space*/
		newbmp.mapdata=(uchar *)malloc (newbmp.linebites*newbmp.mapheight);
		/*Write map data*/
		if (newbmp.mapimagebit==8)
		{
			for (i=0;i<newbmp.mapheight;i++)
			{
				for (j=0;j<newbmp.mapwidth;j++)
				{
					(*(newbmp.mapdata+(newbmp.linebites*i)+j))=*(temp->mapdata+((int)(temp->linebites*i*scale+0.5))+(int)(j*scale+0.5));
				}
			}
			/*Free Old Data Space*/
		free (temp->mapdata);
		/*Assignment Again*/
		*temp=newbmp;
			return true;
		}
		if (newbmp.mapimagebit==24)
		{
			colorimage *newfp=(colorimage *)newbmp.mapdata;
			colorimage *oldfp=(colorimage *)temp->mapdata;
			/*Write map data*/
			for (i=0;i<newbmp.mapheight;i++)
			{
				for (j=0;j<newbmp.mapwidth;j++)
				{
					*(newfp+((newbmp.mapwidth*i))+j)=*(oldfp+((temp->mapwidth)*((int)(i/scale+0.5))+((int)(j/scale+0.5))));
				}
			}
			/*Free old Data Space*/
		free (temp->mapdata);
		/*Assignment Again*/
		*temp=newbmp;
		return true;
		}
		puts ("Can't support The image bit.");
		return false;
}
/* Ascii Print Function*/
bool write4ascii (bmpData map,char filename[])
{
	if (map.mapimagebit!=24)
	{
		puts ("Sorry,Can't support The image bit.");
		return false;
	}
	/*Variable*/
	FILE * fp;
	uint grlevel;
	uint w,h;
	colorimage * rgb;
	/*Pointers*/
	rgb=(colorimage *)map.mapdata;
	/*Open file*/
	if ((fp=fopen (filename,"a"))==NULL)
	{
		puts ("Open failed.");
		return false;
	}
	/*Write file*/
	for (h=0;h<map.mapheight;h++)
	{
		for (w=0;w<map.mapwidth;w++)
		{
			/*Calculated pointer position*/
			rgb=((colorimage *)map.mapdata)+(map.mapheight-1-h)*map.mapwidth+w;
			/*Calculat gralevel*/
			grlevel=(rgb->R*30+rgb->G*59+rgb->B*11)/100;
			/*Output Char*/
			if (grlevel<32)
			{
				fputc ('#',fp);
			}
			else if (grlevel<64)
			{
				fputc ('&',fp);
			}
			else if (grlevel<96)
			{
				fputc ('$',fp);
			}
			else if (grlevel<128)
			{
				fputc ('*',fp);
			}
			else if (grlevel<159)
			{
				fputc ('+',fp);
			}
			else if (grlevel<191)
			{
				fputc (';',fp);
			}
			else if (grlevel<223)
			{
				fputc ('.',fp);
			}
			else 
			{
				fputc (' ',fp);
			}
		}
		/*Put \n*/
		fputc ('\n',fp);
	}
	fputc ('\n',fp);
	/*Close file*/
	fclose (fp);
	return true;
}
/*gray Function*/
bool bmpgra (bmpData * temp)
{
	if (temp->mapimagebit==24)
	{
		/*Variable*/
		int h,w;
		colorimage * rgb;
		int gralevel;
		/*Printer*/
		rgb=(colorimage *)(temp->mapdata);
		/*process image data*/
		for (h=0;h<temp->mapheight;h++)
		{
			for (w=0;w<temp->mapwidth;w++)
			{
				/*Pointer ++*/
				rgb++;
				/*calculat gralevel*/
				gralevel=(rgb->R*30+rgb->G*59+rgb->B*11)/100;
				/*Write*/
				rgb->R=rgb->G=rgb->B=gralevel;
			}
		}
		return true;
	}
	puts ("Error,Can't Support The image bit");
	return false;
}
/*Read Chars Function*/
char * chargets (char *buf,int bufsize,FILE * steam)
{
	short temp;
	char * ch;
	if ((ch=fgets (buf,bufsize,steam))!=buf)
	{
		return ch;
	}
	for (temp=0;temp<bufsize;temp++)
	{
		if (buf [temp]=='\n')
		{
			buf[temp]='\0';
			return ch;
		}
	}
	return ch;
}