/***************************************************************************

					6502 Opcode emulation generator

 ***************************************************************************/

#include <stdio.h>
#include <string.h>

static int nCount = 0;

static int nGenerated[256] = { 0 };

#define A_INDX		(1)
#define A_ZERO      (5)
#define	A_IMM		(9)
#define A_ABS		(0xD)
#define A_INDY		(0x11)
#define A_ZEROX		(0x15)
#define A_ABSY		(0x19)
#define A_ABSX		(0x1D)

/**********************************/
/* return address format for mode */
/**********************************/

char *AFormat(int nMode)
{
char *p = "???";
switch(nMode)
	{
	case	A_INDX:	p = "indx";break;
	case	A_ZERO:	p = "zero";break;
	case	A_IMM:	p = "imm";break;
	case	A_ABS:	p = "abs";break;
	case	A_INDY:	p = "indy";break;
	case	A_ZEROX:p = "zerox";break;
	case	A_ABSY:	p = "absy";break;
	case	A_ABSX:	p = "absx";break;
	}
return(p);
}

/*************************/
/* Output new definition */
/*************************/

void Define(int nOpcode,char *szDesc)
{
printf("\n");
printf("__%02x: ; Opcode %02x (%s)\n",
						nOpcode,nOpcode,szDesc);
nCount++;
nGenerated[nOpcode] = 1;
}

/******************/
/* Section Header */
/******************/

void Section(char *szSection)
{
printf("; ************************************************************************\n");
printf(";\n;%*s%s\n;\n",37-strlen(szSection)/2-1,"",szSection);
printf("; ************************************************************************\n");
}

/******************************************/
/* Handle lda,sta,ora,and,eor,adc,cmp,sbc */
/******************************************/

static char *szIn1[] = { "ora","and","eor","adc","sta","lda","cmp","sbc" };

static int nMode1[] = { A_INDX,A_ZERO,A_IMM,A_ABS,A_INDY,A_ZEROX,A_ABSY,A_ABSX };

void Generate1()
{
int n,nOpcode,nMode;
char szDesc[32],*szZero;
Section("Main memory reference (ora,and,eor,adc,sta,lda,cmp,sbc)");
for (n = 0;n < 8;n++)
	{
	nOpcode = n * 0x20;
	for (nMode = 0;nMode < 8;nMode++)
		{
		szZero = (nMode == 1 || nMode == 5) ? "z" : "";
		sprintf(szDesc,"%s %s",szIn1[n],AFormat(nMode1[nMode]));
		if (nMode != 2 || n != 4)
				Define(nOpcode + nMode1[nMode],szDesc);

		switch(n)
			{
			case 4:	if (nMode != 2)		/* can't store immediate */
						{
						printf("    eac_%s\n",AFormat(nMode1[nMode]));
						printf("    write%s cl\n",szZero);
						printf("    ret\n");
						}
					break;
			case 5:	if (nMode == 2)		/* load */
						printf("    fetch cl\n");
					else
						{
						printf("    eac_%s\n",AFormat(nMode1[nMode]));
						printf("    read%s cl\n",szZero);
						}
					printf("    setzn cl\n");
					printf("    ret\n");
					break;
			default:if (nMode == 2)		/* other operations */
						printf("    fetch al\n");
					else
						{
						printf("    eac_%s\n",AFormat(nMode1[nMode]));
						printf("    read%s al\n",szZero);
						}
					printf("    alu_%s\n",szIn1[n]);
					if (n != 6) printf("    setzn cl\n");
					printf("    ret\n");
					break;
			}
		}
	}
}

/**************************************************/
/* Handle asl,rol,lsr,ror,inc,dec,ldx,ldy,stx,sty */
/**************************************************/

static int nMode2[] = { A_ZERO,A_ABS,A_ZEROX,A_ABSX };

static char *szOp2[] = { "asl","rol","lsr","ror","stx","ldx","dec","inc","sty","ldy", NULL };

static int  nBase2[] = { 0x06 ,0x26 ,0x46 ,0x66, 0x86 ,0xA6, 0xC6, 0xE6, 0x84, 0xA4, NULL};

void Generate2()
{
int n = 0;
int nMode;
int bVeto;
char szTemp[32],cIx,*szZero;
Section("Rotates,Shifts,Increments,Decrements,load/save index registers");
while (szOp2[n] != NULL)
	{
	for (nMode = 0;nMode < 4;nMode++)
		{
		szZero = (nMode == 0 || nMode == 2) ? "z" : "";
		/* no sty abs,x stx abs,y ldy abs,x */
		bVeto = ((n == 4 || n == 8) && nMode == 3);
		sprintf(szTemp,"%s %s",szOp2[n],AFormat(nMode2[nMode]));
		if (!bVeto) Define(nBase2[n] - A_ZERO + nMode2[nMode],szTemp);
		switch(n)
			{
			case 0:       				/* Rotates and Shifts */
			case 1:
			case 2:
			case 3:
			case 6:						/* and the increments */
			case 7:
					printf("    eac_%s\n",AFormat(nMode2[nMode]));
					printf("    push bx\n");
					printf("    read%s al\n",szZero);
					printf("    alu_%s\n",szOp2[n]);
					printf("    pop bx\n");
					printf("    write%s al\n",szZero);
					printf("    setzn al\n");
					printf("    ret\n");
					break;
			case 4:						/* stx and sty,ldx and ldy */
			case 5:
			case 8:
			case 9: if (bVeto) break;
					sprintf(szTemp,"eac_%s",AFormat(nMode2[nMode]));
					if (nMode == 2 || nMode == 3)
						{
						if (n == 4 || n == 5)
							szTemp[strlen(szTemp)-1] = 'y';
						}
					printf("    %s\n",szTemp);
					cIx = (n == 4 || n == 5) ? 'x':'y';
					if (n == 5 || n == 9)
						{
						printf("    read%s al\n",szZero);
						printf("    mov [_%c],al\n",cIx);
						printf("    setzn al\n");
						}
					else
						{
						printf("    mov al,[_%c]\n",cIx);
						printf("    write%s al\n",szZero);
						}
					printf("    ret\n");
					break;
			}
		}
	n++;
	}
for (n = 0;n < 4;n++)
	{
	sprintf(szTemp,"%s a",szOp2[n]);
	Define(nBase2[n]+4,szTemp);
	printf("    mov al,cl\n");
	printf("    alu_%s\n",szOp2[n]);
	printf("    mov cl,al\n");
	printf("    setzn al\n");
	printf("    ret\n");
	}
for (n = 0;n < 2;n++)
	{
	Define(0xA0 + n * 2,n ? "ldx imm" : "ldy imm");
	printf("    fetch al\n");
	printf("    mov [_%c],al\n",n ? 'x' : 'y');
	printf("    setzn al\n");
	printf("    ret\n");
	}
}

/***********************/
/* Handle the branches */
/***********************/

static char *szOp4[] = { "bpl","bmi","bvc","bvs","bcc","bcs","bne","beq" };

void Generate4()
{
int nOp,nTest;
for (nOp = 0;nOp < 8;nOp++)
	{
	Define(0x10+nOp*0x20,szOp4[nOp]);
	printf("    eac_relative\n");
	switch(nOp / 2)
		{
		case 0:	printf("    test dl,128\n");break;
		case 1: printf("    test ch,VFlag\n");break;
		case 2: printf("    test ch,CFlag\n");break;
		case 3: printf("    cmp dh,0\n");break;
		}
	nTest = nOp & 1;
	if (nOp >= 6) nTest = 1-nTest;
	if (nTest == 0)
		printf("    jnz ret\n");
	else
		printf("    jz  ret\n");
	printf("    mov bp,bx\n");
	printf("    ret\n");
	}
}

/*****************************************/
/* Handle set & clear flag register bits */
/*****************************************/

char *szOp5[] = { "clc","sec","cli","sei",NULL,"clv","cld","sed" };

char *szFlag5[] = { "CFlag","IFlag","VFlag","DFlag" };

void Generate5()
{
int n,n1;
Section("Flag set & clear");
for (n = 0;n < 8;n++)
	{
	if (szOp5[n] != NULL)
		{
		Define(0x18 + n * 0x20,szOp5[n]);
		n1 = n & 1;
		if (n == 5) n1 = 1-n1;
		printf("    %s %s\n",n1 ? "setf" : "clearf",szFlag5[n / 2]);
		if (n == 2 || n == 3) printf("    intmode\n");
		if (n >= 6) printf("    mathmode\n");
		printf("    ret\n");
		}
	}
}

/*****************/
/* Compare x & y */
/*****************/

int nOp6[] = { 0,4,0x0C };

char *szOp6[] = { "imm","zero","abs" };

void Generate6()
{
int nOp;
int nX;
char szTemp[32];
Section("CPX and CPY");
for (nOp = 0;nOp < 3;nOp++)
	{
	for (nX = 0;nX < 2;nX++)
		{
		sprintf(szTemp,"cp%c %s",nX ? 'x' : 'y',szOp6[nOp]);
		Define(0xC0 + nX*32 + nOp6[nOp],szTemp);
		printf("    mov ax,cx\n");
		printf("    push ax\n");
		printf("    mov cl,[_%c]\n",nX ? 'x' : 'y');
		if (nOp == 0)
			printf("    fetch al\n");
		else
			{
			printf("    eac_%s\n",szOp6[nOp]);
			printf("    read%s al\n",(nOp == 1 ? "z" : ""));
			}
		printf("    alu_cmp\n");
		printf("    pop ax\n");
		printf("    mov cl,al\n");
		printf("    ret\n");
		}
	}
}

void Bad(int n,int s)
{
Define(n,"Invalid opcode");
if (s > 1) printf(" add bp,%d\n",s-1);
printf(" ret\n");
}

void BadOp()
{
int i;
Bad(0x80,1);
for (i = 0;i < 256;i = i + 32)
	{
	Bad(i+7,2);
	}
}

void main()
{
int i,j,n;
printf("; ************************************************************************\n");
Section("This code is automatically generated. Do NOT modify it");
printf("; ************************************************************************\n");
printf("\n\n");
Generate1();
Generate2();
Generate4();
Generate5();
Generate6();
BadOp();
printf("\n; Generated %d of 256.\n\n",nCount);
for (i = 0;i < 31;i++)
	{
	n = 0;
	for (j = 0;j < 8;j++)
		if (nGenerated[i+j*32] == 0) n++;
	if (n > 0)
		{
		printf(";   +%02x: ",i);
		for (j = 0;j < 8;j++)
			if (nGenerated[i+j*32] == 0)
				{
				printf("%02x ",i+j*32);
				}
		printf("\n");
		}
	}
}