#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>

#include <chrono>
#include <omp.h>

#ifdef __cplusplus
extern "C" {
#endif
  void generateMergeSortData (int* arr, size_t n);
  void checkMergeSortResult (int* arr, size_t n);
#ifdef __cplusplus
}
#endif

void mergesort(int *arr, int left, int right)
  {
    if(left<right)
      {
	int middle = (left+right)/2;

	#pragma omp parallel
	{
	  #pragma omp single
	  {
	    #pragma omp task
	    mergesort(arr,left,middle);

	    #pragma omp task
	    mergesort(arr,middle+1,right);

	    #pragma omp taskwait
	    merge(arr, left, middle+1, right);
	  }
	}
      }
  }

void merge(int *arr, int left, int middle, int right)
{
  int i,j,k;
  int n = middle - left;
  int *temp = new int[n];
  
  if(left == right)
    return;
  
  if(right - left == 1)
    {
      if(arr[left]>arr[right])
	{
	  int temp=arr[left];
	  arr[left]=arr[right];
	  arr[right]=temp;
	}
      return;
    }
  
  for(i=0;i<n;i++)
    temp[i]=arr[left+i];
  
  i=0;
  j=middle;
  k=left;

  while(i<n && j<=right)
    {
      if(temp[i]<=arr[j])
	{
	  arr[k++]=temp[i++];
	}
      else
	arr[k++]=arr[j++];
    }

  while(i<n)
    arr[k++]=temp[i++];

  delete[] temp;
}

int main (int argc, char* argv[])
{
  
  if (argc < 3) { std::cerr<<"Usage: "<<argv[0]<<" <n> <nbthreads>"<<std::endl;
    return -1;
  }

  int n = atoi(argv[1]);
  
  // get arr data
  int * arr = new int [n];
  generateMergeSortData (arr, n);

  //insert sorting code here.

  auto startTime= std::chrono::system_clock::now();

  #pragma omp parallel
  {
    #pragma omp single
    {
      mergesort(arr,0,n-1);
    }
  }

  auto endTime = std::chrono::system_clock::now();
  std::chrono::duration<double> totalTime = endTime - startTime;
  
  checkMergeSortResult (arr, n);
  std::cerr<<totalTime.count()<<std::endl;
  
  delete[] arr;

  return 0;
}
