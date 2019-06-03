#include <stdio.h>
#include <stdlib.h>

unsigned char infom[54];
unsigned char ImageRGB[2000][2000][3];

void readBMP(const char *path, int *Width, int *Height, unsigned char OriginalRGB[2000][2000][3])
{
    unsigned char *Data;
    int i, Padding = 0;
    FILE *MyFile = fopen(path, "rb");
    if (MyFile != NULL)
    {
        fread(infom, sizeof(unsigned char), 54, MyFile);

        int H, W;

        W = *(int *)&infom[18];
        H = *(int *)&infom[22];

        Padding = 4 - ((W * 3) % 4); //Calculate Gap At Every Row
        if (Padding == 4)
        {
            Padding = 0;
        }

        int mySize = 3 * (W + Padding) * H;
        unsigned char *Data = (unsigned char *)malloc(mySize * sizeof(unsigned char));
        fread(Data, sizeof(unsigned char), mySize, MyFile);
        fclose(MyFile);

        for (int j = 0; j < H; j++)
        {
            for (int i = 0; i < W; i++)
            {
                for (int k = 0; k < 3; k++)
                {
                    ImageRGB[i][j][k] = Data[(((j * W) + i) * 3) + (Padding * j) + k]; //Convert BGR To RGB
                }
            }
        }

        *Height = H;
        *Width = W;

        for (int i = 0; i < W; i++)
        {
            for (int j = 0; j < H; j++)
            {
                OriginalRGB[j][i][0] = ImageRGB[i][j][2];
                OriginalRGB[j][i][1] = ImageRGB[i][j][1];
                OriginalRGB[j][i][2] = ImageRGB[i][j][0];
            }
        }

        for (int i = 0; i < H / 2; i++)
        {
            for (int j = 0; j < W; j++)
            {
                for (int k = 0; k < 3; k++)
                {
                    unsigned char temp = OriginalRGB[i][j][k];
                    OriginalRGB[i][j][k] = OriginalRGB[H - i - 1][j][k];
                    OriginalRGB[H - i - 1][j][k] = temp;
                }
            }
        }
    }
}

void saveBMP(unsigned char Image[2000][2000][3], int width, int height, char *address)
{
    unsigned char ***wData;
    wData = (unsigned char ***)malloc(width * sizeof(unsigned char **));
    for (int k = 0; k < width; k++)
    {
        wData[k] = (unsigned char **)malloc(height * sizeof(unsigned char *));
        for (int i = 0; i < height; i++)
        {
            wData[k][i] = (unsigned char *)malloc(3 * sizeof(unsigned char));
        }
    }
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            wData[j][height - 1 - i][0] = Image[i][j][2];
            wData[j][height - 1 - i][1] = Image[i][j][1];
            wData[j][height - 1 - i][2] = Image[i][j][0];
        }
    }

    int Padding = 4 - ((width * 3) % 4); //Calculate Gap At Every Row
    if (Padding == 4)
    {
        Padding = 0;
    }

    int mySize = 3 * (width + Padding) * height;
    unsigned char *raw = (unsigned char *)malloc(mySize * sizeof(unsigned char));
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            raw[(((j * width) + i) * 3) + (Padding * j)] = wData[i][j][0];
            raw[(((j * width) + i) * 3) + (Padding * j) + 1] = wData[i][j][1];
            raw[(((j * width) + i) * 3) + (Padding * j) + 2] = wData[i][j][2];
        }
    }
    int *a = (int *)&infom[18];
    int *b = (int *)&infom[22];
    *a = width;
    *b = height;
    FILE *Out = fopen(address, "wb");
    fwrite(infom, sizeof(unsigned char), 54, Out);
    fwrite(raw, sizeof(unsigned char), mySize, Out);
    fclose(Out);
}
