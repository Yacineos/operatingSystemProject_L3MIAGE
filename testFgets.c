//BINOMES :Oukkal Yacine N° étudiant: 12213245 || Monachon Vivian N° étudiant: 11907866

#include <stdio.h>
#include <string.h>

int main() {
    char input[128]; // Define a character array to store the user's input

    printf("Enter a string (less than 128 characters): ");
    fgets(input, sizeof(input), stdin); // Use fgets to read the input

    // Remove the trailing newline character, if present
    if (strlen(input) < 128) {
        printf("You entered a valid string with less than 128 characters: %s\n", input);
    } else {
        printf("The entered string is too long (128 characters or more).\n");
    }
    printf("%s",input);

    return 0;
}
