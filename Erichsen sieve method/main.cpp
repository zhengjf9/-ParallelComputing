#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <omp.h>
#include <string.h>

#define I 2
void originalMethod(int n);
void parallelMethod(int n);
void setFalse(bool *array, int loc, int sizeOfThread, int n);//多线程设置合数为false
void count(bool * array, int n);//计数

int main()
{
	int n = pow(2, I) * 1000;
	//普通方法
	float startTime1 = omp_get_wtime();
	originalMethod(n);
	float endTime1 = omp_get_wtime();
	printf("Time of originalMethod is %f seconds\n", endTime1 - startTime1);

    //并行化方法
	float startTime2 = omp_get_wtime();
	parallelMethod(n);
    float endTime2 = omp_get_wtime();
	printf("Time of parallelMethod is %f secondes\n", endTime2 - startTime2);
}

void originalMethod(int n)
{   
    bool* is_prime = new bool[n + 1]; 
    int p=0;  
    for(int i=0;i<=n;i++)  
        is_prime[i] = true;  
    is_prime[0] = is_prime[1] = false;  
    for(int i=2;i<=n;i++){  
        if(is_prime[i]){  
            is_prime[p++] = i;  
            for(int j=2*i;j<=n;j+=i)  is_prime[j] = false;  
        }  
    }  
	printf("Original Method:\n"); 
	printf("below %d there is %d primes\n",n,p);
	delete[] is_prime;
}

void parallelMethod(int n)
{
	bool *array = new bool[n + 1];
	omp_set_num_threads(4);	//处理器最多只支持4线程，所以设置成4
#pragma omp parallel for//初始化
	for (int i = 0; i <= n; i++)
		array[i] = true;
	int sizeOfThread = n/4;
	int loc = 1;
	int last = (int)(floor(sqrt((double)n)));
	while (loc <= last)
	{
		loc++;
	    while (!array[loc] && loc <= last)
		        loc++;			
		setFalse(array, loc, sizeOfThread, n);
	}
    count(array, n);
    delete[] array;
}

void setFalse(bool *array, int loc, int sizeOfThread, int n)
{
	int beginLocOfthread;
	int beginLocOfMul;
#pragma omp parallel private(beginLocOfthread) private(beginLocOfMul)
	{
		beginLocOfthread = omp_get_thread_num() * sizeOfThread + 1;
		if(beginLocOfthread % loc == 0)
		    beginLocOfMul = beginLocOfthread;
	    else
		    beginLocOfMul = beginLocOfthread + loc - beginLocOfthread % loc;
#pragma omp master
		beginLocOfMul += loc;
		for (int i = beginLocOfMul; i < beginLocOfthread + sizeOfThread && i <= n; i += loc)
			array[i] = false;
	}
}

void count(bool * array, int n)
{
    int num_of_prime = 0;
#pragma omp parallel for reduction(+:num_of_prime)//计数，采用归约方法，防止同时修改
	for (int i = 2; i <= n; i++)
		if (array[i])
			num_of_prime++;
	printf("Parallel Method:\n");
	printf("below %d there is %d primes\n",n, num_of_prime);
}
