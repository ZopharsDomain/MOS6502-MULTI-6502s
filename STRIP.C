#include <stdio.h>
#include <string.h>

void main()
{
char szLine[256];
int i;
while (gets(szLine) != NULL)
	{
	while (strchr(szLine,';') != NULL) *(strchr(szLine,';')) = '\0';
	i = 0;
	while (szLine[i] != '\0')
		{
		if (szLine[i] == ' ' && szLine[i+1] == ' ')
								strcpy(szLine+i,szLine+i+1);
		else
			i++;
		}
	while (*szLine != '\0' && szLine[strlen(szLine)-1] <= ' ')
										szLine[strlen(szLine)-1] = '\0';
	while (*szLine != '\0' && *szLine <= ' ') strcpy(szLine,szLine+1);
	if (*szLine != '\0') printf("%s\n",szLine);
	}
}