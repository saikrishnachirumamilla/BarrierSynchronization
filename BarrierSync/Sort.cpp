#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <string>
#include <fstream>
#include <math.h>
#include <pthread.h>
#include <vector>
#include <semaphore.h>
#include <algorithm>

using namespace std;

vector<int> numbersList;

struct sortingArgs{
	int numbersSize;
	int threadIndex;
	int noOfThreads;
	char mode[128];
};

typedef struct sortingArgs sortingStruct;


void *sortingAlgorithm(void *sortingArgs);

sem_t counterMutex;
sem_t upperBarrier;
sem_t lowerBarrier;
int tally = 0;




int main(int argc, char **argv) {

	string fileName = argv[1];
	string mode = argv[2];
	int number = 0;
	int lines = 0;
	int firstLine;
	int semaphoreValue = 0;

	fstream file(fileName+".txt");
	file >> firstLine;

	while(firstLine!=0){

		for (int k = 0; k < firstLine; ++k) {
			file >> number;
			numbersList.push_back(number);
		}

		fstream semaphoreFile("sema.init.txt");
		semaphoreFile >> semaphoreValue;

		semaphoreFile >> semaphoreValue;
		sem_init(&counterMutex, 0, semaphoreValue);
		semaphoreFile >> semaphoreValue;
		sem_init(&upperBarrier, 0, semaphoreValue);
		semaphoreFile >> semaphoreValue;
		sem_init(&lowerBarrier, 0, semaphoreValue);

		if(numbersList.size()>8){
			for (int i = 0;   i < 8; ++  i) {
				printf("%d ",numbersList[i]);
			}
			printf("\n");
			for( int i = 8; i < numbersList.size(); i++ ){
				printf("%d ",numbersList[i]);
			}
			printf("\n");

		}else{
			for( int i = 0; i < numbersList.size(); i++ ){
				printf("%d ",numbersList[i]);
			}
			printf("\n");
		}



		int threadCount = firstLine/2;
		pthread_t sortingThread[threadCount];

		for (int i = 0; i < threadCount; i++) {
			sortingStruct *sortingArgs;
			sortingArgs = (sortingStruct *)malloc(sizeof(sortingStruct));
			sortingArgs->numbersSize = numbersList.size();
			sortingArgs->threadIndex = i;
			sortingArgs->noOfThreads = threadCount;
			strcpy(sortingArgs->mode, mode.c_str());
			pthread_create(&sortingThread[i], NULL, sortingAlgorithm, (void *)sortingArgs);
		}

		for (int i = 0; i < threadCount; i++) {
			pthread_join(sortingThread[i],NULL);
		}

		if(numbersList.size()>8){
			for (int i = 0;   i < 8; ++  i) {
				printf("%d ",numbersList[i]);
			}
			printf("\n");
			for( int i = 8; i < numbersList.size(); i++ ){
				printf("%d ",numbersList[i]);
			}
			printf("\n");

		}else{
			for( int i = 0; i < numbersList.size(); i++ ){
				printf("%d ",numbersList[i]);
			}
			printf("\n");
		}

		numbersList.clear();
		cout<<"------------------------------"<<endl;
		file >> firstLine;
	}

	return 0;
}

void *sortingAlgorithm(void *sortingArgs){
	sortingStruct *args;
	args = (sortingStruct *)sortingArgs;
	int s = 1;
	while(s<=log2(args->numbersSize)){
		for (int p= 1; p <= log2(args->numbersSize); p++){
			sem_wait(&counterMutex);
			tally = tally+1;
			if(tally == args->noOfThreads){
			sem_wait(&lowerBarrier);
			sem_post(&upperBarrier);
			}
			sem_post(&counterMutex);
			sem_wait(&upperBarrier);
			sem_post(&upperBarrier);

			int numGroups = pow(2,(p-1));
			int groupSize = args->numbersSize/numGroups;
			int gindex = args->threadIndex/(groupSize/2);
			int mindex = args->threadIndex%(groupSize/2);
			int groupStart = gindex * groupSize;
			int groupEnd = (gindex+1)*groupSize-1;
			int data1 = groupStart+mindex;
			int data2 = groupEnd-mindex;
			if(numbersList[data1] > numbersList[data2]){
			int temp = numbersList[data1];
			numbersList[data1] = numbersList[data2];
			numbersList[data2] = temp;
			}
            for(int i=0;i<data1+data2;i++){
            }

			sem_wait(&counterMutex);
			if(strcmp(args->mode, "-o")==0){
				printf ("Thread %d  finished stage %d phase %d\n", args->threadIndex+1,s,p);
				if(tally == 1){
					if(numbersList.size()>8){
						for (int i = 0;   i < 8; ++  i) {
							printf("%d ",numbersList[i]);
						}
						printf("\n");
						for( int i = 8; i < numbersList.size(); i++ ){
							printf("%d ",numbersList[i]);
						}
						printf("\n");

					}else{
						for( int i = 0; i < numbersList.size(); i++ ){
							printf("%d ",numbersList[i]);
						}
						printf("\n");
					}
				}
			}
			tally = tally-1;
			if(tally == 0){
			sem_wait(&upperBarrier);
			sem_post(&lowerBarrier);
			}
			sem_post(&counterMutex);
			sem_wait(&lowerBarrier);
			sem_post(&lowerBarrier);
		}
		if (is_sorted(numbersList.begin(), numbersList.end())) {
		    break;
		}
	s++;
	}
	return NULL;
}
