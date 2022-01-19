#include <stdlib.h>
#include <stdio.h>
#include "my_malloc.c"

int main(int argc, char *argv[]) {
    const unsigned NUM_ITEMS = 10;
    int i;
    int size;
    int sum = 0;
    int expected_sum = 0;
    int *array[NUM_ITEMS];

    size = 4;
    expected_sum += size * size;
    printf("1th malloc in main:\n");
    array[0] = (int *) ff_malloc(size * sizeof(int));
    for (i = 0; i < size; i++) {
        array[0][i] = size;
    } //for i
    for (i = 0; i < size; i++) {
        sum += array[0][i];
    } //for i

    size = 16;
    expected_sum += size * size;
    printf("2th malloc in main:\n");
    array[1] = (int *) ff_malloc(size * sizeof(int));
    for (i = 0; i < size; i++) {
        array[1][i] = size;
    } //for i
    for (i = 0; i < size; i++) {
        sum += array[1][i];
    } //for i

    size = 8;
    expected_sum += size * size;
    printf("3th malloc in main:\n");
    array[2] = (int *) ff_malloc(size * sizeof(int));
    for (i = 0; i < size; i++) {
        array[2][i] = size;
    } //for i
    for (i = 0; i < size; i++) {
        sum += array[2][i];
    } //for i

    size = 32;
    expected_sum += size * size;
    printf("4th malloc in main:\n");
    array[3] = (int *) ff_malloc(size * sizeof(int));
    for (i = 0; i < size; i++) {
        array[3][i] = size;
    } //for i
    for (i = 0; i < size; i++) {
        sum += array[3][i];
    } //for i

    printf("free array[0] in main: \n");
    ff_free(array[0]);
    printf("free array[2] in main: \n");
    ff_free(array[2]);

    size = 7;
    expected_sum += size * size;
    printf("5th malloc in main:\n");
    array[4] = (int *) ff_malloc(size * sizeof(int));
    for (i = 0; i < size; i++) {
        array[4][i] = size;
    } //for i
    for (i = 0; i < size; i++) {
        sum += array[4][i];
    } //for i

    size = 256;
    expected_sum += size * size;
    printf("6th malloc in main:\n");
    array[5] = (int *) ff_malloc(size * sizeof(int));
    for (i = 0; i < size; i++) {
        array[5][i] = size;
    } //for i
    for (i = 0; i < size; i++) {
        sum += array[5][i];
    } //for i

    printf("free array[5] in main: \n");
    ff_free(array[5]);
    printf("free array[1] in main: \n");
    ff_free(array[1]);
    printf("free array[3] in main: \n");
    ff_free(array[3]);

    size = 23;
    expected_sum += size * size;
    printf("7th malloc in main:\n");
    array[6] = (int *) ff_malloc(size * sizeof(int));
    for (i = 0; i < size; i++) {
        array[6][i] = size;
    } //for i
    for (i = 0; i < size; i++) {
        sum += array[6][i];
    } //for i

    size = 4;
    expected_sum += size * size;
    printf("8th malloc in main:\n");
    array[7] = (int *) ff_malloc(size * sizeof(int));
    for (i = 0; i < size; i++) {
        array[7][i] = size;
    } //for i
    for (i = 0; i < size; i++) {
        sum += array[7][i];
    } //for i

    printf("free array[4] in main: \n");
    ff_free(array[4]);

    size = 10;
    expected_sum += size * size;
    printf("9th malloc in main:\n");
    array[8] = (int *) ff_malloc(size * sizeof(int));
    for (i = 0; i < size; i++) {
        array[8][i] = size;
    } //for i
    for (i = 0; i < size; i++) {
        sum += array[8][i];
    } //for i

    size = 32;
    expected_sum += size * size;
    printf("10th malloc in main:\n");
    array[9] = (int *) ff_malloc(size * sizeof(int));
    for (i = 0; i < size; i++) {
        array[9][i] = size;
    } //for i
    for (i = 0; i < size; i++) {
        sum += array[9][i];
    } //for i

    printf("free array[6] in main: \n");
    ff_free(array[6]);
    printf("free array[7] in main: \n");
    ff_free(array[7]);
    printf("free array[8] in main: \n");
    ff_free(array[8]);
    printf("free array[9] in main: \n");
    ff_free(array[9]);

    if (sum == expected_sum) {
        printf("Calculated expected value of %d\n", sum);
        printf("Test passed\n");
    } else {
        printf("Expected sum=%d but calculated %d\n", expected_sum, sum);
        printf("Test failed\n");
    } //else

    return 0;
}
