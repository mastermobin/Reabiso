#include<stdio.h>
#include<unistd.h>
#include<pthread.h>
#include<stdlb.h>

int arrey[150] ,sum;
int startNum=0, sumThread[10];

void *myThread(void *arg){
	sum=0
	for(int i=0; i<15; i++){
		sum+=arrey[i + *(int *)arg];
	}
}
void *myThread2(void *arg){
	sum=0
	for(int i=0; i<10; i++){
		sum+=sumThread[i];
	}
}

int main(){
	for(int i=0; i<150; i++){
		arrey[i]=(i%5)+1;
	}
	ptread_t tid[10];
	pthread_create(&tid[0], NULL, myThread, (void *)startNum);
	startNum += 15;
	sumThread[0]=sum;
	pthread_create(&tid[1], NULL, myThread, (void *)startNum);
	startNum += 15;
	sumThread[1]=sum;
	pthread_create(&tid[2], NULL, myThread, (void *)startNum);
	startNum += 15;
	sumThread[2]=sum;
	pthread_create(&tid[3], NULL, myThread, (void *)startNum);
	startNum += 15;
	sumThread[3]=sum;
	pthread_create(&tid[4], NULL, myThread, (void *)startNum);
	startNum += 15;
	sumThread[4]=sum;
	pthread_create(&tid[5], NULL, myThread, (void *)startNum);
	startNum += 15;
	sumThread[5]=sum;
	pthread_create(&tid[6], NULL, myThread, (void *)startNum);
	startNum += 15;
	sumThread[6]=sum;
	pthread_create(&tid[7], NULL, myThread, (void *)startNum);
	startNum += 15;
	sumThread[7]=sum;
	pthread_create(&tid[8], NULL, myThread, (void *)startNum);
	startNum += 15;
	sumThread[8]=sum;
	pthread_create(&tid[9], NULL, myThread, (void *)startNum);
	sumThread[9]=sum;
	
	ptread_t tid1;
	pthread_create(&tid1, NULL, myThread2, NULL);
	exit (0);
}
