#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <omp.h>

void merge(std::vector<int>& arr, int left, int mid, int right) {
    int size_left = mid - left + 1;
    int size_right = right - mid;

    std::vector<int> left_arr(size_left), right_arr(size_right);

    for (int i = 0; i < size_left; i++)
        left_arr[i] = arr[left + i];
    for (int j = 0; j < size_right; j++)
        right_arr[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;
    while (i < size_left && j < size_right) {
        if (left_arr[i] <= right_arr[j]) {
            arr[k] = left_arr[i];
            i++;
        }
        else {
            arr[k] = right_arr[j];
            j++;
        }
        k++;
    }

    while (i < size_left) {
        arr[k] = left_arr[i];
        i++;
        k++;
    }

    while (j < size_right) {
        arr[k] = right_arr[j];
        j++;
        k++;
    }
}

void merge_sort(std::vector<int>& arr, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        merge_sort(arr, left, mid);
        merge_sort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}

std::vector<int> parallel_merge_sort(std::vector<int> arr) {
    int size = arr.size();
    if (size <= 1)
        return arr;

    int num_threads = omp_get_max_threads();
    int chunk_size = size / num_threads;

    std::vector<int> sorted_arr(size);
#pragma omp parallel num_threads(num_threads)
    {
        int tid = omp_get_thread_num();
        int start = tid * chunk_size;
        int end = (tid + 1) * chunk_size - 1;
        if (tid == num_threads - 1)
            end = size - 1;
        merge_sort(arr, start, end);
#pragma omp barrier
#pragma omp single
        merge(arr, 0, (num_threads / 2) * chunk_size - 1, size - 1);
    }

    return arr;
}

int main() {
    const int size = 100000;
    std::vector<int> arr(size);

    srand(time(nullptr));
    for (int i = 0; i < size; i++) {
        arr[i] = rand() % 1000;
    }

    std::vector<int> arr_copy(arr);
    double start_time = omp_get_wtime();
    merge_sort(arr_copy, 0, size - 1);
    double end_time = omp_get_wtime();
    std::cout << "Size of array: " << size << std::endl;
    //std::cout << "Sorted array without OpenMP: ";
    //for (int i = 0; i < size; ++i)
    //    std::cout << arr_copy[i] << " ";
    //std::cout << std::endl;
    std::cout << "Time without OpenMP: " << (end_time - start_time) << " seconds" << std::endl;

    start_time = omp_get_wtime();
    parallel_merge_sort(arr);
    end_time = omp_get_wtime();
    //std::cout << "Sorted array using OpenMP: ";
    //for (int i = 0; i < size; i++)
    //    std::cout << arr[i] << " ";
    //std::cout << std::endl;
    std::cout << "Time using OpenMP: " << (end_time - start_time) << " seconds" << std::endl;

    return 0;
}
