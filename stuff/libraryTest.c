#include <stdio.h>
#include <bmpio.h>

unsigned char Pic[2000][2000][3];

int main()
{
    char p[100];
    gets(p);
    int width, height;
    readBMP(p, &width, &height, Pic);
    printf("%d %d\n", width, height);
	saveBMP(Pic, width / 2, height / 2, "123.bmp");
}
