#include <stdio.h>
#include <stdlib.h>

void printReverse( char* string ){
    int len = 0;
    while( string[len] != '\0' ){
        len++;
    }
    for( int i = len - 1; i >= 0; i-- ){
        printf("%c\n", string[i]);
    }
}

char* reverseString( char* input ){
    int number_of_chars_in_input = 0;
    while( input[number_of_chars_in_input] != '\0' ){
        number_of_chars_in_input++;
    }

    char* output = (char*)malloc( number_of_chars_in_input+1 );

    for( int i = 0; i < number_of_chars_in_input; i++ ){
        output[i] = input[number_of_chars_in_input - 1 - i];
    }
    output[number_of_chars_in_input] = '\0';

    return output;
}

int main(){
    char *messagePtr = "HELLOWORLD!";

    printf("%s\n", messagePtr);

    for( int i = 0; i < 11; i++ ){
        printf("%c\n", messagePtr[i]);
    }

    printf("%c\n", *messagePtr);

    printf("%c\n", *(messagePtr + 1));
    printf("%c\n", *(messagePtr + 2));
    printf("%c\n", *(messagePtr + 3));

    for( int i = 0; i < 11; i++ ){
        printf("%c\n", *(messagePtr + i));
    }

    int i = 0;
    while( messagePtr[i] != '\0' ){
        printf("%c\n", messagePtr[i]);
        i++;
    }

    printReverse(messagePtr);

    char* reversedMessage = reverseString( messagePtr );
    printf("Reversed string: %s\n", reversedMessage);

    free(reversedMessage);

    return 0;
}
