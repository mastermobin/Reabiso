extern void readBMP(const char *path, int *Width, int *Height, unsigned char OriginalRGB[2000][2000][3]);
extern void saveBMP(unsigned char Image[2000][2000][3], int width, int height, char *address);