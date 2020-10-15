#include <Windows.h>
#include <conio.h>
#include <dirent.h>
#include <bits/stdc++.h>
#include "Library/bmpio.h"

using namespace std;

#define CIRCLE_COUNT 300
#define R_DEFAULT 255
#define G_DEFAULT 255
#define B_DEFAULT 255
#define X_OFFSET 500
#define Y_OFFSET 200
#define SWIDTH 512
#define SHEIGHT 512
#define BLINK_DELAY 800
#define LINE_ALPHA 0
#define MAX_ALPHA 1

struct Color
{
    unsigned char R, G, B;
};

struct Pos
{
    int X, Y;
};

struct Segment
{
    bool isVertical;
    int startParam, endParam;
    int constParam;
    bool isRD;
};

Color **data[256];
int Count[256] = {0};
int Rows;
vector<unsigned char> Letters;
int Width, Height;
unsigned char ImageRGB[2000][2000][3];
Color **ScaledImage;
Color **HalfScaled;
Color **OriginalRGB;
Color **CroppedImage;
Color **BigImage;
char **Table;
bool learned = false;
int lineTillNow = 8;
HDC MyDC;
unsigned char info[54];
vector<Segment> segments;
map<string, int> wordCounter;
vector<string> words;
Pos **positions;

void UpdateLearnProgress(double fraction);
int Menu();
void printEmptyLine();
void gotoxy(int x, int y);

void gotoxy(int x, int y)
{
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD cursorCoord;
    cursorCoord.X = y;
    cursorCoord.Y = x;
    SetConsoleCursorPosition(consoleHandle, cursorCoord);
}

void setTextColor(int textColor, int backColor = 0)
{
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    int colorAttribute = backColor << 4 | textColor;
    SetConsoleTextAttribute(consoleHandle, colorAttribute);
}

string alignText(string text, int space = 50)
{
    string ans = text;
    for (int i = 0; i < space - text.length() - 2; i++)
    {
        if (i % 2 == 0)
            ans = ans + " ";
        else
            ans = " " + ans;
    }
    return ans;
}

void printAlign(string text, int color, int back = 0, int space = 50)
{
    setTextColor(1);
    cout << "  " << (char)186;
    setTextColor(color, back);
    cout << alignText(text);
    setTextColor(1);
    cout << (char)186 << endl;
}

void printAlignByPercent(string text, int color, double frac, int pColor, int backN = 0, int space = 50)
{
    setTextColor(1);
    cout << "  " << (char)186;
    int fh = frac * (space - 2);
    setTextColor(color, pColor);
    string aligned = alignText(text);
    cout << aligned.substr(0, fh);
    setTextColor(color, backN);
    if (fh < aligned.length())
        cout << aligned.substr(fh);
    setTextColor(1);
    cout << (char)186 << endl;
}

void printHeader(int len = 50)
{
    setTextColor(1);
    cout << endl;
    cout << "  " << (char)201;
    for (int i = 0; i < len - 2; i++)
        cout << (char)205;
    cout << char(187) << endl;
}

void printFooter(int len = 50)
{
    setTextColor(1);
    cout << "  " << char(200);
    for (int i = 0; i < len - 2; i++)
        cout << (char)205;
    cout << char(188) << endl;
}

void print(Color **Image, int scale, int width, int height)
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            for (int k = 0; k < scale; k++)
            {
                for (int w = 0; w < scale; w++)
                {
                    SetPixel(MyDC, X_OFFSET + j * scale + w, Y_OFFSET + i * scale + k, RGB(Image[i][j].R, Image[i][j].G, Image[i][j].B));
                }
            }
        }
    }
}

Color lineColor;
Color lineColors[] = {{66, 165, 245}, {92, 107, 192}, {126, 87, 194}, {38, 198, 218}, {77, 182, 172}, {192, 202, 51}, {76, 175, 80}, {161, 136, 127}, {229, 57, 53}, {255, 160, 0}};
void drawLinePM1(int x1, int y1, int x2, int y2, bool dir)
{
    int m_new = 2 * (y2 - y1);
    int slope_error_new = m_new - (x2 - x1);
    for (int x = x1, y = y1; x <= x2; x++)
    {
        double alpha = LINE_ALPHA + (MAX_ALPHA - LINE_ALPHA) * ((double)(x - x1) / (x2 - x1));
        if (!dir)
            alpha = 1 - alpha;
        BigImage[y][x].R = BigImage[y][x].R * (1 - alpha) + lineColor.R * alpha;
        BigImage[y][x].G = BigImage[y][x].G * (1 - alpha) + lineColor.G * alpha;
        BigImage[y][x].B = BigImage[y][x].B * (1 - alpha) + lineColor.B * alpha;

        // Add slope to increment angle formed
        slope_error_new += m_new;

        // Slope error reached limit, time to
        // increment y and update slope error.
        if (slope_error_new >= 0)
        {
            y++;
            slope_error_new -= 2 * (x2 - x1);
        }
    }
}

void drawLinePM2(int x1, int y1, int x2, int y2, bool dir)
{
    int m_new = 2 * (x2 - x1);
    int slope_error_new = m_new - (y2 - y1);
    for (int y = y1, x = x1; y <= y2; y++)
    {
        double alpha = LINE_ALPHA + (MAX_ALPHA - LINE_ALPHA) * ((double)(y - y1) / (y2 - y1));
        if (!dir)
            alpha = 1 - alpha;
        BigImage[y][x].R = BigImage[y][x].R * (1 - alpha) + lineColor.R * alpha;
        BigImage[y][x].G = BigImage[y][x].G * (1 - alpha) + lineColor.G * alpha;
        BigImage[y][x].B = BigImage[y][x].B * (1 - alpha) + lineColor.B * alpha;

        // Add slope to increment angle formed
        slope_error_new += m_new;

        // Slope error reached limit, time to
        // increment y and update slope error.
        if (slope_error_new >= 0)
        {
            x++;
            slope_error_new -= 2 * (y2 - y1);
        }
    }
}

void drawLineNM1(int x1, int y1, int x2, int y2, bool dir)
{
    int m_new = 2 * (y1 - y2);
    int slope_error_new = m_new - (x2 - x1);
    for (int x = x1, y = y1; x <= x2; x++)
    {
        double alpha = LINE_ALPHA + (MAX_ALPHA - LINE_ALPHA) * ((double)(x - x1) / (x2 - x1));
        if (!dir)
            alpha = 1 - alpha;
        BigImage[y][x].R = BigImage[y][x].R * (1 - alpha) + lineColor.R * alpha;
        BigImage[y][x].G = BigImage[y][x].G * (1 - alpha) + lineColor.G * alpha;
        BigImage[y][x].B = BigImage[y][x].B * (1 - alpha) + lineColor.B * alpha;

        // Add slope to increment angle formed
        slope_error_new += m_new;

        // Slope error reached limit, time to
        // increment y and update slope error.
        if (slope_error_new >= 0)
        {
            y--;
            slope_error_new -= 2 * (x2 - x1);
        }
    }
}

void drawLineNM2(int x1, int y1, int x2, int y2, bool dir)
{
    int m_new = 2 * (x2 - x1);
    int slope_error_new = m_new - (y1 - y2);
    for (int y = y2, x = x2; y <= y1; y++)
    {
        double alpha = LINE_ALPHA + (MAX_ALPHA - LINE_ALPHA) * ((double)(y - y2) / (y1 - y2));
        if (!dir)
            alpha = 1 - alpha;
        BigImage[y][x].R = BigImage[y][x].R * (1 - alpha) + lineColor.R * alpha;
        BigImage[y][x].G = BigImage[y][x].G * (1 - alpha) + lineColor.G * alpha;
        BigImage[y][x].B = BigImage[y][x].B * (1 - alpha) + lineColor.B * alpha;

        // Add slope to increment angle formed
        slope_error_new += m_new;

        // Slope error reached limit, time to
        // increment y and update slope error.
        if (slope_error_new >= 0)
        {
            x--;
            slope_error_new -= 2 * (y1 - y2);
        }
    }
}

void prepareToDraw(int x1, int x2, int y1, int y2, bool rd)
{
    if ((x2 - x1) * (y2 - y1) > 0)
    {
        if (x2 > x1)
        {
            if ((y2 - y1) > (x2 - x1))
                drawLinePM2(x1, y1, x2, y2, rd);
            else
                drawLinePM1(x1, y1, x2, y2, rd);
        }
        else
        {
            if ((y2 - y1) > (x2 - x1))
                drawLinePM2(x2, y2, x1, y1, rd);
            else
                drawLinePM1(x2, y2, x1, y1, rd);
        }
    }
    else
    {
        if (x2 > x1)
        {
            if ((y2 - y1) > (x2 - x1))
                drawLineNM2(x1, y1, x2, y2, rd);
            else
                drawLineNM1(x1, y1, x2, y2, rd);
        }
        else
        {
            if ((y2 - y1) > (x2 - x1))
                drawLineNM2(x2, y2, x1, y1, rd);
            else
                drawLineNM1(x2, y2, x1, y1, rd);
        }
    }
}

void analyzeAndDraw()
{
    for (int i = 0; i < segments.size(); i++)
    {
        lineColor = lineColors[rand() % 10];
        if (segments[i].isVertical)
        {
            for (int j = segments[i].startParam; j < segments[i].endParam; j++)
            {

                int c = segments[i].constParam;
                int x1 = positions[c][j].X, x2 = positions[c][j + 1].X;
                int y1 = positions[c][j].Y, y2 = positions[c][j + 1].Y;
                for (int k = -1; k < 2; k++)
                    for (int w = -1; w < 2; w++)
                        prepareToDraw(x1 + k, x2 + k, y1 + w, y2 + w, segments[i].isRD);
            }
        }
        else
        {
            for (int j = segments[i].startParam; j < segments[i].endParam; j++)
            {
                int c = segments[i].constParam;
                int x1 = positions[j][c].X, x2 = positions[j + 1][c].X;
                int y1 = positions[j][c].Y, y2 = positions[j + 1][c].Y;
                for (int k = -1; k < 2; k++)
                    for (int w = -1; w < 2; w++)
                        prepareToDraw(x1 + k, x2 + k, y1 + w, y2 + w, segments[i].isRD);
            }
        }
    }
}

int xs, xf, ys, yf;
void cropAndScale()
{
    ScaledImage = (Color **)malloc(SHEIGHT * sizeof(Color *));
    for (int i = 0; i < SHEIGHT; i++)
    {
        ScaledImage[i] = (Color *)malloc(SWIDTH * sizeof(Color));
    }

    HalfScaled = (Color **)malloc(Height * sizeof(Color *));
    for (int i = 0; i < Height; i++)
    {
        HalfScaled[i] = (Color *)malloc(SWIDTH * sizeof(Color));
    }

    xs = 0, xf = Width - 1, ys = 0, yf = Height - 1;
    bool x = false, y = false;

    for (int i = 0; i < Height; i++)
    {
        bool s = false;
        for (int j = 0; j < Width; j++)
        {
            if (OriginalRGB[i][j].R != 255 || OriginalRGB[i][j].G != 255 || OriginalRGB[i][j].B != 255)
            {
                s = true;
                break;
            }
        }
        if (s)
        {
            ys = i;
            y = true;
            break;
        }
    }

    if (y)
        for (int i = ys + 1; i < Height; i++)
        {
            bool s = true;
            for (int j = 0; j < Width; j++)
            {
                if (OriginalRGB[i][j].R != 255 || OriginalRGB[i][j].G != 255 || OriginalRGB[i][j].B != 255)
                {
                    s = false;
                    break;
                }
            }
            if (s)
            {
                yf = i - 1;
                break;
            }
        }

    for (int i = 0; i < Width; i++)
    {
        bool s = false;
        for (int j = 0; j < Height; j++)
        {
            if (OriginalRGB[j][i].R != 255 || OriginalRGB[j][i].G != 255 || OriginalRGB[j][i].B != 255)
            {
                s = true;
                break;
            }
        }
        if (s)
        {
            xs = i;
            x = true;
            break;
        }
    }

    if (x)
        for (int i = xs + 1; i < Width; i++)
        {
            bool s = true;
            for (int j = 0; j < Height; j++)
            {
                if (OriginalRGB[j][i].R != 255 || OriginalRGB[j][i].G != 255 || OriginalRGB[j][i].B != 255)
                {
                    s = false;
                    break;
                }
            }
            if (s)
            {
                xf = min(i - 1, xf);
                break;
            }
        }

    Height = yf - ys + 1;
    Width = xf - xs + 1;
    CroppedImage = (Color **)malloc(Height * sizeof(Color *));
    for (int i = 0; i < Height; i++)
    {
        CroppedImage[i] = (Color *)malloc(Width * sizeof(Color));
    }

    for (int i = 0; i < Height; i++)
    {
        for (int j = 0; j < Width; j++)
        {
            CroppedImage[i][j] = OriginalRGB[i + ys][j + xs];
        }
    }

    double fraction = (double)SWIDTH / Width;
    for (int j = 0; j < Height; j++)
    {
        double partialFraction = fraction;
        int k = 0;
        for (int i = 0; i < SWIDTH; i++)
        {
            if (partialFraction >= 0.99999998)
            {
                double Rval = 0, Gval = 0, Bval = 0;
                Rval += CroppedImage[j][k].R;
                Gval += CroppedImage[j][k].G;
                Bval += CroppedImage[j][k].B;
                partialFraction--;
                HalfScaled[j][i] = Color{(unsigned char)Rval, (unsigned char)Gval, (unsigned char)Bval};
            }
            else
            {
                double Rval = 0, Gval = 0, Bval = 0;
                Rval += partialFraction * CroppedImage[j][k].R;
                Gval += partialFraction * CroppedImage[j][k].G;
                Bval += partialFraction * CroppedImage[j][k].B;
                k++;
                partialFraction = 1 - partialFraction;
                Rval += partialFraction * CroppedImage[j][k].R;
                Gval += partialFraction * CroppedImage[j][k].G;
                Bval += partialFraction * CroppedImage[j][k].B;
                partialFraction = fraction - partialFraction;
                HalfScaled[j][i] = Color{(unsigned char)Rval, (unsigned char)Gval, (unsigned char)Bval};
            }
        }
    }

    fraction = (double)SHEIGHT / Height;
    for (int j = 0; j < SWIDTH; j++)
    {
        double partialFraction = fraction;
        int k = 0;
        for (int i = 0; i < SHEIGHT; i++)
        {
            if (partialFraction >= 0.99999998)
            {
                double Rval = 0, Gval = 0, Bval = 0;
                Rval += HalfScaled[k][j].R;
                Gval += HalfScaled[k][j].G;
                Bval += HalfScaled[k][j].B;
                partialFraction--;
                ScaledImage[i][j] = Color{(unsigned char)Rval, (unsigned char)Gval, (unsigned char)Bval};
            }
            else
            {
                double Rval = 0, Gval = 0, Bval = 0;
                Rval += partialFraction * HalfScaled[k][j].R;
                Gval += partialFraction * HalfScaled[k][j].G;
                Bval += partialFraction * HalfScaled[k][j].B;
                k++;
                partialFraction = 1 - partialFraction;
                Rval += partialFraction * HalfScaled[k][j].R;
                Gval += partialFraction * HalfScaled[k][j].G;
                Bval += partialFraction * HalfScaled[k][j].B;
                partialFraction = fraction - partialFraction;
                ScaledImage[i][j] = Color{(unsigned char)Rval, (unsigned char)Gval, (unsigned char)Bval};
            }
        }
    }
}

void readData(string path)
{
    readBMP(path.c_str(), &Width, &Height, ImageRGB);

    OriginalRGB = (Color **)malloc(Height * sizeof(Color *));
    for (int i = 0; i < Height; i++)
    {
        OriginalRGB[i] = (Color *)malloc(Width * sizeof(Color));
    }

    for (int i = 0; i < Width; i++)
    {
        for (int j = 0; j < Height; j++)
        {
            OriginalRGB[j][i].R = ImageRGB[i][j][0];
            OriginalRGB[j][i].G = ImageRGB[i][j][1];
            OriginalRGB[j][i].B = ImageRGB[i][j][2];
        }
    }

    for (int i = 0; i < Height / 2; i++)
    {
        for (int j = 0; j < Width; j++)
        {
            Color temp = OriginalRGB[i][j];
            OriginalRGB[i][j] = OriginalRGB[Height - i - 1][j];
            OriginalRGB[Height - i - 1][j] = temp;
        }
    }

    cout << "Read Done" << endl;
}

long long fitness(unsigned char letter)
{
    long long score = 0;
    for (int i = 0; i < SHEIGHT; i++)
    {
        for (int j = 0; j < SWIDTH; j++)
        {
            score += abs((int)data[letter][i][j].R - (int)ScaledImage[i][j].R);
            score += abs((int)data[letter][i][j].G - (int)ScaledImage[i][j].G);
            score += abs((int)data[letter][i][j].B - (int)ScaledImage[i][j].B);
        }
    }

    return score;
}

char detect()
{
    pair<unsigned char, long long> minPenalty;
    minPenalty = {'?', LONG_MAX};
    for (int i = 0; i < Letters.size(); i++)
    {
        long long answer = fitness(Letters[i]);
        if (answer < minPenalty.second)
        {
            minPenalty.second = answer;
            minPenalty.first = Letters[i];
        }
    }

    return (char)minPenalty.first;
}

void tableImage(string path)
{
    readData(path);
    Rows = 0;
    bool s = true;
    for (int i = 0; i < Height; i++)
    {
        bool t = true;
        for (int j = 0; j < Width; j++)
        {
            if (s)
            {
                if (OriginalRGB[i][j].R != 255 || OriginalRGB[i][j].G != 255 || OriginalRGB[i][j].B != 255)
                {
                    s = false;
                    t = false;
                    Rows++;
                    break;
                }
            }
            else
            {
                if (OriginalRGB[i][j].R != 255 || OriginalRGB[i][j].G != 255 || OriginalRGB[i][j].B != 255)
                {
                    t = false;
                    break;
                }
            }
        }
        if (t)
        {
            s = true;
        }
    }

    BigImage = (Color **)malloc(Height * sizeof(Color *));
    for (int i = 0; i < Height; i++)
    {
        BigImage[i] = (Color *)malloc(Width * sizeof(Color));
    }

    for (int i = 0; i < Height; i++)
        for (int j = 0; j < Width; j++)
        {
            BigImage[i][j].R = OriginalRGB[i][j].R;
            BigImage[i][j].G = OriginalRGB[i][j].G;
            BigImage[i][j].B = OriginalRGB[i][j].B;
        }
    for (int i = 0; i < Height; i++)
    {
        Color *temp = OriginalRGB[i];
        free(temp);
    }

    Height = Height / Rows;
    Width = Width / Rows;
    int tHeight = Height, tWidth = Width;

    OriginalRGB = (Color **)malloc(Height * sizeof(Color *));
    for (int i = 0; i < Height; i++)
    {
        OriginalRGB[i] = (Color *)malloc(Width * sizeof(Color));
    }

    Table = (char **)malloc(Rows * sizeof(char *));
    for (int i = 0; i < Rows; i++)
    {
        Table[i] = (char *)malloc(Rows * sizeof(char));
    }

    positions = (Pos **)malloc(Rows * sizeof(Pos *));
    for (int i = 0; i < Rows; i++)
    {
        positions[i] = (Pos *)malloc(Rows * sizeof(Pos));
    }

    for (int i = 0; i < Rows; i++)
    {
        stringstream ss;
        for (int j = 0; j < Rows; j++)
        {
            for (int k = 0; k < Height; k++)
            {
                for (int w = 0; w < Width; w++)
                {
                    OriginalRGB[k][w].R = BigImage[i * Height + k][j * Width + w].R;
                    OriginalRGB[k][w].G = BigImage[i * Height + k][j * Width + w].G;
                    OriginalRGB[k][w].B = BigImage[i * Height + k][j * Width + w].B;
                }
            }
            cropAndScale();
            Height = tHeight, Width = tWidth;
            gotoxy(lineTillNow - (i == 0 ? 4 : 3), 0);
            Table[i][j] = detect();
            positions[i][j].Y = i * tHeight + ((ys + yf) / 2);
            positions[i][j].X = j * tWidth + ((xs + xf) / 2);
            ss << Table[i][j] << " ";
            printAlign(ss.str(), 9);
        }
        if (i != 0)
        {
            gotoxy(lineTillNow - 1, 0);
            printEmptyLine();
            printFooter();
            lineTillNow++;
        }
    }

    gotoxy(2, 0);
    printAlign("DONE", 11);
    gotoxy(lineTillNow - 2, 0);
    printAlign("Press Enter To Go Back To Menu", 11);
}

void freeArray()
{
    for (int i = 0; i < Height; i++)
    {
        Color *temp = OriginalRGB[i];
        free(temp);
    }

    for (int i = 0; i < Height; i++)
    {
        Color *temp = HalfScaled[i];
        free(temp);
    }

    for (int i = 0; i < SHEIGHT; i++)
    {
        Color *temp = ScaledImage[i];
        free(temp);
    }
}

void learn(char category)
{
    unsigned char nc = (unsigned char)category;
    for (int i = 0; i < SHEIGHT; i++)
    {
        for (int j = 0; j < SWIDTH; j++)
        {
            data[(unsigned char)nc][i][j].B = (unsigned char)((double)(Count[nc] * (int)data[nc][i][j].B + (int)ScaledImage[i][j].B) / (Count[nc] + 1));
            data[(unsigned char)nc][i][j].G = (unsigned char)((double)(Count[nc] * (int)data[nc][i][j].G + (int)ScaledImage[i][j].G) / (Count[nc] + 1));
            data[(unsigned char)nc][i][j].R = (unsigned char)((double)(Count[nc] * (int)data[nc][i][j].R + (int)ScaledImage[i][j].R) / (Count[nc] + 1));
        }
    }
    Count[nc]++;
    freeArray();
}

int maxPhoto = 0, current = 0;
void getDir(string address, string parent, bool counting = false)
{
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(address.c_str())) != NULL)
    {
        /* print all the files and directories within directory */
        while ((ent = readdir(dir)) != NULL)
        {
            string name = ent->d_name;
            if (name == "." || name == "..")
                continue;
            if (name.find(".bmp") == string::npos)
            {
                Letters.push_back(name[0]);
                getDir(address + name + "\\", name, counting);
            }
            else
            {
                if (!counting)
                {
                    current++;
                    readData(address + name);
                    cropAndScale();
                    learn(parent[0]);
                    UpdateLearnProgress((double)current / maxPhoto);
                }
                else
                {
                    maxPhoto++;
                }
            }
        }
        closedir(dir);
    }
}

void hideCursor()
{
    HANDLE myconsole = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_CURSOR_INFO CURSOR;
    CURSOR.dwSize = 1;
    CURSOR.bVisible = FALSE;
    SetConsoleCursorInfo(myconsole, &CURSOR);
}

void printEmptyLine()
{
    printAlign("", 1);
}

int selected = 0;
void printMenu()
{
    system("cls");
    setTextColor(1);
    printHeader();
    printAlign("Solve Letters Table Program", 11);
    printEmptyLine();
    printAlign(": Move With Arrows & Select By Enter :", 10);
    printEmptyLine();
    printAlign("Learning", 3, selected == 0 ? 8 : 0);
    printAlign("Detect", 3, selected == 1 ? 8 : 0);
    printAlign("Exit", 3, selected == 2 ? 8 : 0);
    printFooter();
}

void sleep(unsigned int mseconds)
{
    clock_t goal = mseconds + clock();
    while (goal > clock())
        ;
}

void LearnMenu()
{
    if (learned)
        for (int k = 0; k < 256; k++)
        {
            for (int i = 0; i < SHEIGHT; i++)
            {
                free(data[k][i]);
            }
        }
    learned = true;
    for (int k = 0; k < 256; k++)
    {
        data[k] = (Color **)malloc(SHEIGHT * sizeof(Color *));
        for (int i = 0; i < SHEIGHT; i++)
        {
            data[k][i] = (Color *)malloc(SWIDTH * sizeof(Color));
        }
    }
    maxPhoto = 0;
    current = 0;
    system("cls");
    printHeader();
    printEmptyLine();
    printEmptyLine();
    printEmptyLine();
    printEmptyLine();
    printEmptyLine();
    printFooter();
    gotoxy(4, 0);
    printAlign("Initialize Learning", 12);
    getDir(".\\DataSet\\", "DataSet", true);
    sleep(1500);
    gotoxy(4, 0);
    printAlign("Learn Progress : 0%", 12);
    getDir(".\\DataSet\\", "DataSet");
}

void BlinkAndExit()
{
    sleep(BLINK_DELAY);
    gotoxy(4, 0);
    printAlign("", 5);
    sleep(BLINK_DELAY);
    gotoxy(4, 0);
    printAlign("Learn Progress : 100.00%", 12, 8);
    sleep(BLINK_DELAY);
    gotoxy(4, 0);
    printAlign("", 5);
    sleep(BLINK_DELAY);
    gotoxy(4, 0);
    printAlign("Learn Progress : 100.00%", 12, 8);
    sleep(BLINK_DELAY);
    Menu();
}

void UpdateLearnProgress(double fraction)
{
    gotoxy(4, 0);
    stringstream ss;
    ss << "Learn Progress : " << fixed << setprecision(2) << fraction * 100 << "%";
    printAlignByPercent(ss.str(), 12, fraction, 8);
    if (current == maxPhoto)
        BlinkAndExit();
}

void checkEnv(string word, int x, int y)
{
    bool is = 1;
    int i;
    for (i = 0; (i < min((int)word.length(), Rows - x)) && is; i++)
    {
        if (Table[x + i][y] != word[i])
            is = 0;
    }
    if (is && i == (int)word.length())
    {
        wordCounter[word]++;
        segments.push_back({0, x, x + (int)word.length() - 1, y, true});
    }
    is = 1;
    for (i = 0; (i < min((int)word.length(), x + 1)) && is; i++)
    {
        if (Table[x - i][y] != word[i])
            is = 0;
    }
    if (is && i == (int)word.length())
    {
        wordCounter[word]++;
        segments.push_back({0, x - (int)word.length() + 1, x, y, false});
    }
    is = 1;
    for (i = 0; (i < min((int)word.length(), Rows - y)) && is; i++)
    {
        if (Table[x][y + i] != word[i])
            is = 0;
    }
    if (is && i == (int)word.length())
    {
        wordCounter[word]++;
        segments.push_back({1, y, y + (int)word.length() - 1, x, true});
    }
    is = 1;
    for (i = 0; (i < min((int)word.length(), y + 1)) && is; i++)
    {
        if (Table[x][y - i] != word[i])
            is = 0;
    }
    if (is && i == (int)word.length())
    {
        wordCounter[word]++;
        segments.push_back({1, y - (int)word.length() + 1, y, x, false});
    }
}

void solveTable()
{
    for (int i = 0; i < words.size(); i++)
    {
        for (int j = 0; j < Rows; j++)
        {
            for (int k = 0; k < Rows; k++)
            {
                checkEnv(words[i], k, j);
            }
        }
    }
}

void readDictionary()
{
    ifstream dic("Dictionary.txt");
    if (dic)
    {
        stringstream buffer;
        buffer << dic.rdbuf();
        dic.close();
        int n;
        buffer >> n;

        for (int i = 0; i < n; i++)
        {
            string t;
            buffer >> t;
            words.push_back(t);
        }
    }
}

int DetectMenu()
{
    lineTillNow = 8;
    if (!learned)
    {
        system("cls");
        printHeader();
        printEmptyLine();
        printAlign("System Can Not Detect", 11);
        printAlign("Learn First", 11);
        printEmptyLine();
        printFooter();
        sleep(2500);
        Menu();
        return 0;
    }
    system("cls");
    printHeader();
    printEmptyLine();
    printAlign("* Enter Address *", 11);
    printEmptyLine();
    printEmptyLine();
    printEmptyLine();
    printFooter();
    stringstream ss;
    while (true)
    {
        char last = getch();
        if (last == 13)
        {
            gotoxy(2, 0);
            printAlign(".. Proccessing ..", 11);
            printEmptyLine();
            printEmptyLine();
            printEmptyLine();
            tableImage(ss.str());
            segments.clear();
            words.clear();
            wordCounter.clear();
            readDictionary();
            solveTable();
            sleep(500);
            gotoxy(lineTillNow - 1, 0);
            printAlign("Press R To See Result", 11);
            printFooter();
            lineTillNow++;
            sleep(500);
            gotoxy(lineTillNow - 1, 0);
            printAlign("Press S To Save Result", 11);
            printFooter();
            lineTillNow++;
            while (true)
            {
                last = getch();
                if (last == 's' || last == 'S')
                {
                    analyzeAndDraw();
                    Width *= Rows;
                    Height *= Rows;
                    unsigned char ***TDa = new unsigned char[Height][Width][3];
                    saveWithLastHeader(BigImage, Width, Height);
                }
                else if (last == 'R' || last == 'r')
                {
                    system("cls");
                    printHeader();
                    printEmptyLine();
                    for (int l = 0; l < words.size(); l++)
                    {
                        stringstream sc;
                        sc << words[l] << "        " << wordCounter[words[l]];
                        printAlign(sc.str(), 9);
                    }
                    printEmptyLine();
                    printAlign("Press Enter To Go Back To Menu", 11);
                    printAlign("Press S To Save Result", 11);
                    printEmptyLine();
                    printFooter();
                    while (true)
                    {
                        last = getch();
                        if (last == 's' || last == 'S')
                        {
                            analyzeAndDraw();
                            Width *= Rows;
                            Height *= Rows;
                            saveWithLastHeader(BigImage, Width, Height);
                        }
                        else if ((int)last == 13)
                        {
                            break;
                        }
                    };
                    break;
                }
                else if ((int)last == 13)
                {
                    break;
                }
            };
            break;
        }
        else if (last == 8)
        {
            ss.str("");
            gotoxy(5, 0);
            printAlign(ss.str(), 12);
        }
        else
        {
            ss << last;
            gotoxy(5, 0);
            printAlign(ss.str(), 12);
        }
    }
    Menu();
}

int Menu()
{
    selected = 0;
    printMenu();
    char last;
    while (true)
    {
        last = getch();
        if ((int)last == -32)
        { // if the first value is esc
            switch ((int)getch())
            {
            case 80: //Down
                selected++;
                selected %= 3;
                gotoxy(6, 0);
                printAlign("Learning", 3, selected == 0 ? 8 : 0);
                printAlign("Detect", 3, selected == 1 ? 8 : 0);
                printAlign("Exit", 3, selected == 2 ? 8 : 0);
                break;
            case 72: //Up
                selected--;
                if (selected < 0)
                    selected += 3;
                gotoxy(6, 0);
                printAlign("Learning", 3, selected == 0 ? 8 : 0);
                printAlign("Detect", 3, selected == 1 ? 8 : 0);
                printAlign("Exit", 3, selected == 2 ? 8 : 0);
                break;
                // case 77: //Right
                // case 75: //Left
            }
        }
        else if ((int)last == 13)
        {
            if (selected == 0)
            {
                LearnMenu();
            }
            else if (selected == 1)
            {
                DetectMenu();
            }
            else
            {
                exit(0);
            }
        }
    }
}

void ChangeScreenSize(COORD NewSize)
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SMALL_RECT DisplayArea = {0, 0, 0, 0};
    CONSOLE_SCREEN_BUFFER_INFO SBInfo;

    GetConsoleScreenBufferInfo(hOut, &SBInfo);

    DisplayArea.Bottom = NewSize.Y;
    DisplayArea.Right = NewSize.X;

    SetConsoleWindowInfo(hOut, true, &DisplayArea);
}

int main()
{
    hideCursor();
    HWND MyConsole = GetConsoleWindow();
    MyDC = GetDC(MyConsole);
    GetStdHandle(STD_OUTPUT_HANDLE);
    COORD Temp;
    Temp.Y = 20;
    Temp.X = 54;
    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), Temp);
    ChangeScreenSize(Temp);
    Menu();
    return 0;
}
