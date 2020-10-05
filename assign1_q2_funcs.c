#include "assign1_q2_funcs.h"

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <limits.h>


#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/types.h>

// Merge sort solution of Tutorial 1
void merge_4_way(int* array, int low, int mid1, int mid2, int mid3, int high) {
    int n1 = mid1 - low;
    int n2 = mid2 - mid1;
    int n3 = mid3 - mid2;
    int n4 = high - mid3;


    int arr1[n1], arr2[n2], arr3[n3], arr4[n4];
    for (int i = 0; i < n1; ++i) {
        arr1[i] = array[low+i];
    }

    for (int i = 0; i < n2; ++i) {
        arr2[i] = array[mid1+i];
    }

    for (int i = 0; i < n3; ++i) {
        arr3[i] = array[mid2+i];
    }

    for (int i = 0; i < n4; ++i) {
        arr4[i] = array[mid3+i];
    }


    int i = 0, j = 0, k = 0, p = 0, l = low;  

    // choose smaller of the smallest in the three ranges  
    while (l < high) {
        int min_value = INT_MAX;
        int min_index = -1;
        
        if(i < n1 && min_value >= arr1[i]) {
            min_value = arr1[i];
            min_index = 0;
        }

        if(j < n2 && min_value >= arr2[j]) {
            min_value = arr2[j];
            min_index = 1;
        }

        if(k < n3 && min_value >= arr3[k]) {
            min_value = arr3[k];
            min_index = 2;
        }

        if(p < n4 && min_value >= arr4[p]) {
            min_value = arr4[p];
            min_index = 3;
        }


        if(min_index == 0) {
            array[l++] = arr1[i++]; 
        } else if (min_index == 1) {
            array[l++] = arr2[j++]; 
        } else if(min_index == 2) {
            array[l++] = arr3[k++]; 
        } else if(min_index == 3) {
            array[l++] = arr4[p++]; 
        } else {
            printf("error occurs! \n");
            return;
        }
    }  
}  

void swap(int *a, int *b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
} 
  
/* Performing the merge sort algorithm on the  
given array of values in the rangeof indices  
[low, high). low is minimum index, high is  
maximum index (exclusive) */
void mergesort_4_way_rec(int* array, int low, int high) {
    // If array size is 1 then do nothing  
    if (high - low < 2)  
        return;

    if (high - low == 2) {
        int x = array[low], y = array[low+1];
        if (x > y) swap(&x, &y);
        array[low] = x;
        array[low+1] = y;
        return;
    }  

     if (high - low == 3) {
        int x = array[low], y = array[low+1], z = array[low+2];
        if (x > y) swap(&x, &y);
        if (y > z) swap(&y, &z);
        if (x > y) swap(&x, &y);
        array[low] = x;
        array[low+1] = y;
        array[low+2] = z;
        return;
     }  
  

    // Splitting array into 3 parts  
	int length = high - low;
    int quarter = length / 4;
    int remainder = length % 4;
    int mid1 = low + quarter;
    int mid2 = mid1 + quarter;
    int mid3 = mid2 + quarter;
    if (remainder == 2)
        mid3++;
    if (remainder == 3){
        mid2++;
		mid3 +=2;
    }

    // Sorting 3 arrays recursively  
    mergesort_4_way_rec(array, low, mid1);  
    mergesort_4_way_rec(array, mid1, mid2);  
    mergesort_4_way_rec(array, mid2, mid3);  
    mergesort_4_way_rec(array, mid3, high);  
  
    // Merging the sorted arrays  
    merge_4_way(array, low, mid1, mid2, mid3, high);  
} 


/* from K&R
 - produces a random number between 0 and 32767.*/
int rand_seed=10;
int rand()
{
    rand_seed = rand_seed * 1103515245 +12345;
    return (unsigned int)(rand_seed / 65536) % 32768;
}

int* generateIntArray(int size)
{
	int * array = (int*) malloc(size*sizeof(int));
	for (int i=0; i < size; i++){
        array[i]=rand();
    }
	
	return array;
}

void printArray(int* array, int low, int high)
{
	for(int i=0;i < high - low; i++){
		printf("%d ", array[low + i]);
	}
	printf("\n");
}



void bubble_sort(int *array, int size)
{
	int x,y,t;
	for (x=0; x < size-1; x++)
        for (y=0; y < size-x-1; y++)
            if (array[y] > array[y+1])
            {
                t=array[y];
                array[y]=array[y+1];
                array[y+1]=t;
            }
}

bool verifySortResults(int* array_bubble, int* array_mergesort, int size)
{
	int num_unequal = 0;
	for(int i = 0; i< size; i++){
		if(array_bubble[i] != 	array_mergesort[i])
			num_unequal++;
	}
	
	if(num_unequal!=0){
		printf("The sort result by merge sort is not correct. The number of unequal values: %d.\n", num_unequal);
		return false;
	}
	else{
		printf("The sort result by merge sort is corrent, verified by bubble sort.\n");
		return true;
	}
}



void mergesort4Way4Processes(int* array, int low, int high)
{
	// Q2.1: Write your solution
  pid_t child2, child3, child4, wpid;
  int status = 0;
  int min2 = (low + high)/2; 
  int min1 = (low + min2)/2;
  int min3 = (min2 + high)/2;
  int size_data = high * sizeof(int);
  int shmid = shmget(IPC_PRIVATE, size_data, 0666|IPC_CREAT);
  int *shmc = shmat(shmid, NULL, 0);
  for(int i = 0; i < high; i++){
    shmc[i] = array[i]; 
  }
  shmdt(shmc);
  // shmc = YOUR_ARRAY;
  // shmdt(shmc);
	// for(int i = 0; i < high; i++){
  //   array[i] = YOUR_ARRAY[i]; 
  // }
  // while ((wpid = wait(&status)) > 0);
  child4 = fork();
  if(child4 == 0){
    int* shmg = shmat(shmid, NULL, 0);
    printf("Process ID: %d; Sorted %d integers: ", getpid(), min1 - low);
    mergesort_4_way_rec(shmg, low, min1);
    for(int i = low; i < min1; i++){
      printf("%d ", shmg[i]);
    }
    printf("\n");
    shmdt(shmg);
    exit(0);
  }else if(child4 > 0){
    child3 = fork();
    if(child3 == 0){
      int* shmg = shmat(shmid, NULL, 0);
      printf("Process ID: %d; Sorted %d integers: ", getpid(), min2 - min1);
      mergesort_4_way_rec(shmg, min1, min2);
      for(int i = min1; i < min2; i++){
        printf("%d ", shmg[i]);
      }
      printf("\n");
      shmdt(shmg);
      exit(0);
    }else if(child3 > 0){
      child2 = fork();
      if(child2 == 0){
        int* shmg = shmat(shmid, NULL, 0);
        printf("Process ID: %d; Sorted %d integers: ", getpid(), min3 - min2);
        mergesort_4_way_rec(shmg, min2, min3);
        for(int i = min2; i < min3; i++){
          printf("%d ", shmg[i]);
        }
        printf("\n");
        shmdt(shmg);
        exit(0);
      }else if(child2 > 0){
        int* shmg = shmat(shmid, NULL, 0);
        printf("Process ID: %d; Sorted %d integers: ", getpid(), high - min3);
        mergesort_4_way_rec(shmg, min3, high);
        for(int i = min3; i < high; i++){
          printf("%d ", shmg[i]);
        }
        printf("\n");
        while ((wpid = wait(&status)) > 0);
        merge_4_way(shmg, low, min1, min2, min3, high);
        printf("Process ID: %d; Merged %d integers: ", getpid(), high - low);
        for(int i = 0; i < high; i++){
          printf("%d ", shmg[i]);
        }
        printf("\n");
        for(int i = 0; i < high; i++){
          array[i] = shmg[i]; 
        }
        shmdt(shmg);
        shmctl(shmid, IPC_RMID, NULL);
      }
    }
  }

	

}

void recursiveMergesort(int* array, int low, int high, int max_num)
{
	// max_num: the maximum number of integers a process can handle
	// Q2.2 Write your solution
  //for the base case
  if((high - low)/4 <= max_num){
    pid_t child2, child3, child4;
    int size_data = (high - low) * sizeof(int);
    int shmid = shmget(IPC_PRIVATE, size_data, 0666|IPC_CREAT);
    int *shmc = shmat(shmid, NULL, 0);
    int j = 0;
    for(int i = low; i < high; i++){
      shmc[j] = array[i];
      j++; 
    }
    //min of shmc
    int min2 = (0 + (high - low))/2; 
    int min1 = (0 + min2)/2;
    int min3 = (min2 + (high - low))/2;
    shmdt(shmc);
    child4 = fork();
    if(child4 == 0){
      int* shmg = shmat(shmid, NULL, 0);
      printf("Process ID: %d; Sorted %d integers: ", getpid(), min1 - 0);
      mergesort_4_way_rec(shmg, 0, min1);
      for(int i = 0; i < min1; i++){
        printf("%d ", shmg[i]);
      }
      printf("\n");
      shmdt(shmg);
      exit(0);
    }else if(child4 > 0){
      child3 = fork();
      if(child3 == 0){
        int* shmg = shmat(shmid, NULL, 0);
        printf("Process ID: %d; Sorted %d integers: ", getpid(), min2 - min1);
        mergesort_4_way_rec(shmg, min1, min2);
        for(int i = min1; i < min2; i++){
          printf("%d ", shmg[i]);
        }
        printf("\n");
        shmdt(shmg);
        exit(0);
      }else if(child3 > 0){
        child2 = fork();
        if(child2 == 0){
          int* shmg = shmat(shmid, NULL, 0);
          printf("Process ID: %d; Sorted %d integers: ", getpid(), min3 - min2);
          mergesort_4_way_rec(shmg, min2, min3);
          for(int i = min2; i < min3; i++){
            printf("%d ", shmg[i]);
          }
          printf("\n");
          shmdt(shmg);
          exit(0);
        }else if(child2 > 0){
          int* shmg = shmat(shmid, NULL, 0);
          printf("Process ID: %d; Sorted %d integers: ", getpid(), (high - low) - min3);
          mergesort_4_way_rec(shmg, min3, (high - low));
          for(int i = min3; i < (high - low); i++){
            printf("%d ", shmg[i]);
          }
          printf("\n");
          waitpid(child2, 0, 0);
          waitpid(child3, 0, 0);
          waitpid(child4, 0, 0);
          merge_4_way(shmg, 0, min1, min2, min3, (high - low));
          printf("Process ID: %d; Merged %d integers: ", getpid(), high - low);
          for(int i = 0; i < (high - low); i++){
            printf("%d ", shmg[i]);
          }
          printf("\n");
          int j = 0;
          for(int i = low; i < high; i++){
            array[i] = shmg[j];
            j++; 
          }
          shmdt(shmg);
          shmctl(shmid, IPC_RMID, NULL);
        }
      }
    }
  }else if((high - low)/4 != max_num){
    //recursion
    pid_t child2, child3, child4;
    int size_data = (high - low) * sizeof(int);
    int shmid = shmget(IPC_PRIVATE, size_data, 0666|IPC_CREAT);
    int *shmc = shmat(shmid, NULL, 0);
    int j = 0;
    for(int i = low; i < high; i++){
      shmc[j] = array[i];
      j++; 
    }
    int min2 = (0 + high - low)/2; 
    int min1 = (low + min2)/2;
    int min3 = (min2 + high - low)/2;
    shmdt(shmc);
    child4 = fork();
    if(child4 == 0){
      //child4
      int *shmg = shmat(shmid, NULL, 0);
      int child4_arr[min1 - 0];
      int j = 0;
      for(int i = 0; i < min1 - 0; i++){
        child4_arr[i] = shmg[j];
        j++;
      }
      j = 0;
      recursiveMergesort(child4_arr, 0, min1 - 0, max_num);
      for(int i = 0; i < min1 - 0; i++){
        shmg[j] = child4_arr[i];
        j++;
      }
      shmdt(shmg);
      exit(0);
    }else if(child4>0){
      child3 = fork();
      if(child3 == 0){
        int *shmg = shmat(shmid, NULL, 0);
        int child3_arr[min2 - min1];
        int j = min1;
        for(int i = 0; i < min2 - min1; i++){
          child3_arr[i] = shmg[j];
          j++;
        }
        j = min1;
        recursiveMergesort(child3_arr, 0, min2 - min1, max_num);
        for(int i = 0; i < min2 - min1; i++){
          shmg[j] = child3_arr[i];
          j++;
        }
        shmdt(shmg);
        exit(0);
      }else if(child3 > 0){
        child2 = fork();
        if(child2 == 0){
          int *shmg = shmat(shmid, NULL, 0);
          int child2_arr[min3 - min2];
          int j = min2;
          for(int i = 0; i < min3 - min2; i++){
            child2_arr[i] = shmg[j];
            j++;
          }
          j = min2;
          recursiveMergesort(child2_arr, 0, min3 - min2, max_num);
          for(int i = 0; i < min3 - min2; i++){
            shmg[j] = child2_arr[i];
            j++;
          }
          shmdt(shmg);
          exit(0);
        }else if(child2 > 0){
          int *shmg = shmat(shmid, NULL, 0);
          int parent_arr[high - low - min3];
          int j = min3;
          for(int i = 0; i < high - low - min3; i++){
            parent_arr[i] = shmg[j];
            j++;
          }
          j = min3;
          recursiveMergesort(parent_arr, 0, high - low - min3, max_num);
          for(int i = 0; i < high - low - min3; i++){
            shmg[j] = parent_arr[i];
            j++;
          }
          waitpid(child2, 0, 0);
          waitpid(child3, 0, 0);
          waitpid(child4, 0, 0);
          merge_4_way(shmg, 0, min1, min2, min3, high - low);
          printf("Process ID: %d; Merged %d integers: ", getpid(), high - low);
          for(int i = 0; i < high - low; i++){
            printf("%d ", shmg[i]);
          }
          printf("\n");
          j = 0;
          for(int i = low; i < high; i++){
            array[i] = shmg[j];
            j++; 
          }
          shmdt(shmg);
          shmctl(shmid, IPC_RMID, NULL);
        }

      }
    }    
  }
	

}


// Plase use the following lines to print related information if needed.
/*
printf("Process ID: %d; Sorted %d integers: ", getpid(), ***);
printf("Process ID: %d; Merged %d integers: ", getpid(), ***);
*/