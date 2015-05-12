//
//  print_data.c
//  AirView
//
//  Created by Łukasz Przytuła on 12.05.2015.
//
//

#include "print_data.h"

void _print_data(uint8_t data[], int size) {
    const int row_width = 16;
    int rows = (size / row_width) + ((size % row_width == 0) ? 0 : 1);
    
    for (int row = 0; row < rows; ++row) {
        printf("| ");
        for (int column = 0; column < row_width; ++column) {
            int index = (row * row_width) + column;
            if (index < size) {
                if (column > 0) {
                    printf(":");
                }
                printf("%02X", data[index]);
            } else {
                printf("   ");
            }
        }
        printf(" | ");
        for (int column = 0; column < row_width; ++column) {
            int index = (row * row_width) + column;
            if (index < size) {
                char c = (char)data[index];
                if (c < 32 || c == 127) {
                    c = ' ';
                }
                printf("%c", c);
            } else {
                printf(" ");
            }
        }
        printf(" |\n");
    }
    printf("\n");
}