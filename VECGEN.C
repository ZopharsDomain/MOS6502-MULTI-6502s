/***************************/
/* Make vectors for tables */
/***************************/

#include <stdio.h>
#include <ctype.h>

typedef struct _vectable
{
unsigned char bFound[256];
} VEC;

VEC vMain;

void ClearVec(VEC *v)
{
int i;
for (i = 0;i < 256;i++) v->bFound[i] = 0;
}

void OutVec(VEC *v,char *szLabel)
{
int i;
printf("\n%s:\n",szLabel);
for (i = 0;i < 256;i++)
	{
	if (i % 8 == 0) printf("    dw ");
	if (v->bFound[i])
		printf("__%02x",i);
	else
		printf("____");
	if (i % 8 == 7) printf("\n"); else printf(",");
	}
}

#define ISCODE(c) ((c) == '_')

void main(int argc,char *argv[])
{
int i,n;
char c;
FILE *f;
char szLine[256];

ClearVec(&vMain);
for (i = 1;i < argc;i++)
	{
	f = fopen(argv[i],"r");
	if (f == NULL) exit(fprintf(stderr,"Couldn't open %s.\n",argv[i]));
	while (fgets(szLine,255,f) != NULL)
		{
		if (szLine[0] == '_' &&
			ISCODE(szLine[1]) &&
			isxdigit(szLine[2]) &&
			isxdigit(szLine[3]) &&
			szLine[4] == ':')
				{
				c = tolower(szLine[1]);
				sscanf(szLine+2,"%x",&n);
				switch(c)
					{
					case	'_':    vMain.bFound[n] = 1;break;
					}
				}
		}
	fclose(f);
	}

OutVec(&vMain,"Vec6502");
}

