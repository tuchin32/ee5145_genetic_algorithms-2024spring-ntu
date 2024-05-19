#include "myrand.h"
#include <iostream>

using namespace std;

int main()
{
    MyRand mr;
    
    // Test uniformArray
    int n = 10;

    int *arr = new int[n];
    mr.uniformArray(arr, n, 0, n - 1);

    for (int i = 0; i < n; i++){
        cout << arr[i] << endl;
    }

    return 0;
}