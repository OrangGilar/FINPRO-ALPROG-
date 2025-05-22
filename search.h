#ifndef SEARCH_SORT_H
#define SEARCH_SORT_H

#include <vector>
#include <algorithm>

// Quick Sort
void quickSort(std::vector<int>& arr, int low, int high) {
    if (low < high) {
        int pivot = arr[high];
        int i = low - 1;
        for (int j = low; j < high; j++) {
            if (arr[j] > pivot) { // Descending sort
                i++;
                std::swap(arr[i], arr[j]);
            }
        }
        std::swap(arr[i + 1], arr[high]);
        int pi = i + 1;
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

// Binary Search
bool binarySearchScore(const std::vector<int>& sortedScores, int target) {
    int low = 0, high = sortedScores.size() - 1;
    while (low <= high) {
        int mid = (low + high) / 2;
        if (sortedScores[mid] == target) return true;
        else if (sortedScores[mid] < target) high = mid - 1;
        else low = mid + 1;
    }
    return false;
}

#endif // SEARCH_SORT_H
