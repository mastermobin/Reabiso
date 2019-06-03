#include <bmpio.h>
#include <stdio.h>
unsigned char pic[2000][2000][3], ans[2000][2000][3], height[2000][2000][3], width[2000][2000][3] = {0};
int main()
{

    int b = 0;
    char p[100];
    gets(p);
    //	int color1 = 255;
    //	int color2 = 255;
    //	int color3 = 255;
    int i, j;
    int cni = 0, cnj = 0, ci = 0, cj = 0;
    int tp = 0;
    int w, h;
    int up = 0, left = 0, right = 0, down = 0;
    readBMP(p, &w, &h, pic);
    printf("%d %d \n ", w, h);
    double tw, th;
    scanf("%lf%lf", &tw, &th);

    double dw = tw / w;
    double sahm = dw;
    double dh = th / h;

    double r = 0, r2 = 0;
    for (i = 0; i < h; i++)
    {
    	cj = 0;
        sahm = dw;
        for (j = 0; j < tw; j++)
        {
            r = 1;
            while (r > 0)
            {
                if (sahm > r)
                {
                    //	printf( "." );
                    sahm -= r;
                    width[i][j][0] += (unsigned char)(r * pic[i][cj][0]);
                    width[i][j][1] += (unsigned char)(r * pic[i][cj][1]);
                    width[i][j][2] += (unsigned char)(r * pic[i][cj][2]);
                    break;
                }
                else
                {
                    //	printf ("/\n");
                    r -= sahm;
                    width[i][j][0] += (unsigned char)(sahm * pic[i][cj][0]);
                    width[i][j][1] += (unsigned char)(sahm * pic[i][cj][1]);
                    width[i][j][2] += (unsigned char)(sahm * pic[i][cj][2]);
                    sahm = dw;
                    cj++;
                }
            }
        }
    }
    sahm = dh;
    ci = 0;
    cj = 0;
    /*	
	for ( int i = 0; i < tw; i++ ) {
		for ( int j = 0; j < th; j++ ) { 
			r = 1;
			while ( r > 0 ) { 
				if ( sahm > r ) { 
					sahm -= r;
					width [ i ] [ j ] [ 0 ] += r * pic [ ci ] [ cj ] [ 0 ]; 
					width [ i ] [ j ] [ 1 ] += r * pic [ ci ] [ cj ] [ 1 ]; 
					width [ i ] [ j ] [ 2 ] += r * pic [ ci ] [ cj ] [ 2 ]; 
					r = 0;
					continue; 
				}
				else {
					r -= sahm;	 
					width [ i ] [ j ] [ 0 ] += sahm * pic [ ci ] [ cj ] [ 0 ]; 
					width [ i ] [ j ] [ 1 ] += sahm * pic [ ci ] [ cj ] [ 1 ]; 
					width [ i ] [ j ] [ 2 ] += sahm * pic [ ci ] [ cj ] [ 2 ]; 
					sahm = dh;
					cj++; 
				}
			}
			
		}
		ci++; 
	}
*/
    /*	
	for ( int i = 0; i < 500; i ++ ) { 
		for ( int j = 0; j < 500; j++ ) { 
			ans [ i ] [ j ] [ 0 ] = 0.5 * width [ i ] [ j ] [ 0 ] + 0.5*height [ i ] [ j ] [ 0 ];
			ans [ i ] [ j ] [ 1 ] = 0.5 * width [ i ] [ j ] [ 1 ] + 0.5*height [ i ] [ j ] [ 1 ];
			ans [ i ] [ j ] [ 2 ] = 0.5 * width [ i ] [ j ] [ 2 ] + 0.5*height [ i ] [ j ] [ 2 ];
			
		}
	}
*/
    /*	for ( int i = 0; i < h; i++ ) { 
		for ( int j = 0; j < w; j++ ) { 
			if ( pic [ i ] [ j ] [ 0 ] != 255 || pic [ i ] [ j ] [ 1 ] != 255 || pic [ i ] [ j ] [ 2 ] != 255 && b == 0 ) { 
				up = i; 
				tp = j; 
				b = 1;
				break;
			}
		}
		if ( b == 1 )
			break; 
	}
	b = 0;
	
	for ( int i = up; i < h; i++ ) {
		b = 0; 
		for ( int j = 0; j < w; j++ ) { 
			if ( pic [ i ] [ j ] [ 0 ] != 255 ||pic [ i ] [ j ] [ 1 ] != 255 || pic [ i ] [ j ] [ 2 ] != 255 ) { 
				b = 1;
			}
		}
		if ( b == 0 ) { 
			down = i - 1; 
			break; 
		}
	}
	b = 0;
	for ( int i = tp; i < w; i++ ) { 
		b = 0;
		for ( int j = up; j < h; j++ ) { 
			if ( pic [ j ] [ i ] [ 0 ] != 255 || pic [ j ] [ i ] [ 1 ] != 255 || pic [ j ] [ i ] [ 2 ] != 255 ) { 
				b = 1; 	
			}
		}
		if ( b == 0 ) { 
			right = i - 1; 
			break; 
		}
	}
	
	for ( int i = tp; i > 0; i-- ) { 
		b = 0;
		for ( int j = up; j < h; j++ ) { 
			if ( pic [ j ] [ i ] [ 0 ] != 255 || pic [ j ] [ i ] [ 1 ] != 255 || pic [ j ] [ i ] [ 2 ] != 255 ) { 
				b = 1; 	
			}
		}
		if ( b == 0 ) { 
			left = i + 1; 
			break; 
		}
	}
	printf ( "%d %d\n%d %d", up, down, left, right );	
	for ( int i = up; i <= down; i++ ) { 
		for ( int j = left; j <= right; j++ ) { 
			ans [ i-up ] [ j-left ] [ 0 ] = pic [ i ] [ j ] [ 0 ];
			ans [ i-up ] [ j-left ] [ 1 ] = pic [ i ] [ j ] [ 1 ];
			ans [ i-up ] [ j-left ] [ 2 ] = pic [ i ] [ j ] [ 2 ];
			
		}
	}
	printf ( "%d %d\n%d %d", up, down, left, right );
	saveBMP ( ans, right- left , down-up, "Image.bmp " ); 
*/
    saveBMP(width, tw, h, "Image.bmp");
}
