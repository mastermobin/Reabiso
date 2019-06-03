#include <stdio.h>
#include <bmpio.h>
//#include <Windows.h>

#define X_OFFSET 500
#define Y_OFFSET 200

unsigned char Pic[2000][2000][3];
unsigned char Cropped[2000][2000][3];

int main()
{
	int i, j;
    char p[100] = "OFCP.bmp";
    gets(p);
    int width, height;
    readBMP(p, &width, &height, Pic);
    int is = 0, ie = height - 1, js = 0, je = width - 1;
    printf("%d %d\n", width, height);
    
    int temp = -1;
    for(i = 0; i < height; i++){
    	int flag = 0;
    	for(j = 0; j < width; j++){
    		if(Pic[i][j][0] != 255 || Pic[i][j][1] != 255 || Pic[i][j][2] != 255){
    			flag = 1;
    			temp = j;
    			break;
			}
		}
		if(flag){
			is = i;
			break;
		}
	}
	
	printf("XS: %d\n", is);
	
	if(temp == -1){
		printf("Nothing :(");
		return;
	}
	
	for(j = temp; j < width; j++){
		int flag = 1;
		for(i = 0; i < height; i++){
			if(Pic[i][j][0] != 255 || Pic[i][j][1] != 255 || Pic[i][j][2] != 255){
    			flag = 0;
    			break;
			}
		}
		if(flag){
			je = j - 1;
			break;
		}
	}
	
	printf("YE: %d\n", je);
		
	for(j = temp; j >= 0; j--){
		int flag = 1;
		for(i = 0; i < height; i++){
			if(Pic[i][j][0] != 255 || Pic[i][j][1] != 255 || Pic[i][j][2] != 255){
    			flag = 0;
    			break;
			}
		}
		if(flag){
			js = j + 1;
			break;
		}
	}
	
	printf("YS: %d\n", js);
	
	for(i = is; i < height; i++){
    	int flag = 1;
    	for(j = js; j <= je; j++){
    		if(Pic[i][j][0] != 255 || Pic[i][j][1] != 255 || Pic[i][j][2] != 255){
    			flag = 0;
    			break;
			}
		}
		if(flag){
			ie = i - 1;
			break;
		}
	}
	
	printf("XE: %d\n", ie);
	
	for(i = is; i <= ie; i++){
		for(j = js; j <= je; j++){
			Cropped[i - is][j - js][0] = Pic[i][j][0];
			Cropped[i - is][j - js][1] = Pic[i][j][1];
			Cropped[i - is][j - js][2] = Pic[i][j][2];
		}
	}
	
	saveBMP(Cropped, je - js + 1, ie - is + 1, "Crop.bmp");
}
