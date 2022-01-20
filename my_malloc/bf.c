#include <stdlib.h>
#include <stdio.h>
#include "my_malloc.c"

int main(int argc, char *argv[]) {
    const unsigned NUM_ITEMS = 5;
    int i;
    int size;
    int sum = 0;
    int expected_sum = 0;
    int *array[NUM_ITEMS];

    size = 25;
    expected_sum += size * size;
    printf("1th malloc in main:\n");
    array[0] = (int *) bf_malloc(size * sizeof(int));
    for (i = 0; i < size; i++) {
        array[0][i] = size;
    } //for i
    for (i = 0; i < size; i++) {
        sum += array[0][i];
    } //for i

    size = 1;
    expected_sum += size * size;
    printf("2th malloc in main:\n");
    array[1] = (int *) bf_malloc(size * sizeof(int));
    for (i = 0; i < size; i++) {
        array[1][i] = size;
    } //for i
    for (i = 0; i < size; i++) {
        sum += array[1][i];
    } //for i

    size = 8;
    expected_sum += size * size;
    printf("3th malloc in main:\n");
    array[2] = (int *) bf_malloc(size * sizeof(int));
    for (i = 0; i < size; i++) {
        array[2][i] = size;
    } //for i
    for (i = 0; i < size; i++) {
        sum += array[2][i];
    } //for i

    size = 1;
    expected_sum += size * size;
    printf("4th malloc in main:\n");
    array[3] = (int *) bf_malloc(size * sizeof(int));
    for (i = 0; i < size; i++) {
        array[3][i] = size;
    } //for i
    for (i = 0; i < size; i++) {
        sum += array[3][i];
    } //for i

    size = 12;
    expected_sum += size * size;
    printf("5th malloc in main:\n");
    array[4] = (int *) bf_malloc(size * sizeof(int));
    for (i = 0; i < size; i++) {
        array[4][i] = size;
    } //for i
    for (i = 0; i < size; i++) {
        sum += array[4][i];
    } //for i

    printf("free array[0] in main: \n");
    bf_free(array[0]);
    printf("free array[2] in main: \n");
    bf_free(array[2]);
    printf("free array[4] in main: \n");
    bf_free(array[4]);

    size = 9;
    expected_sum += size * size;
    printf("6th malloc in main:\n");
    array[5] = (int *) bf_malloc(size * sizeof(int));
    for (i = 0; i < size; i++) {
        array[5][i] = size;
    } //for i
    for (i = 0; i < size; i++) {
        sum += array[5][i];
    } //for i

    printf("free array[3] in main: \n");
    bf_free(array[3]);
    printf("free array[1] in main: \n");
    bf_free(array[1]);
    printf("free array[5] in main: \n");
    bf_free(array[5]);


    if (sum == expected_sum) {
        printf("Calculated expected value of %d\n", sum);
        printf("Test passed\n");
    } else {
        printf("Expected sum=%d but calculated %d\n", expected_sum, sum);
        printf("Test failed\n");
    } //else

    return 0;
}
