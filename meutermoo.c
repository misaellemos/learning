#include <stdlib.h>
#include <stdio.h>
#include <string.h> 
#include <ctype.h>
#include <time.h>

enum colors {Gray = 1, Yellow, Green};

#define C_GREEN "\x1b[48;5;2m"
#define C_YELLOW "\x1b[43m"
#define C_GRAY "\x1b[48;5;239m"
#define C_SPACE "\x1b[40m"
#define C_STD "\x1b[0m"

#define LEN_WORD 5

#define DONTPRINT 0
#define YESPRINT 75
#define IGNORE 0

void play(char* filename);
int num_words_file(char* filename);
char* selecting_word_day(char* filename,int num);
int* occurrence_char(char* word_day);
void coloring_output(char letter, int color);
void printing_output(char* input, char* word_day, int* occurr_char);
void show_possible_keys(char c, int color, int print);

int main(int agrc, char** argv){
    char* filename = {"five_words.txt"};
    printf("\n\tMY TERMOOO");
    play(filename);
}

char* uppercase(char* str){
    for(int i = 0; i < strlen(str); i++){
        str[i] = toupper(str[i]);
    }
    
    return str;
}
/*
 * process with two uses:
 * 	First: to print the "keyboard" showing what keys were already used with different colors 
 * 	(green: has in the word of the day and the used guessed the correct position one time, yellow: has the letter in the word but the user still did'nt guess it right, gray: this character it isn't in the word of the day, black: didn't guessed this key yet)
 * 	Second: each time the user send an input this function will update the case color if it's necessary
 * I could split this procediment in two but I wanted to use static int and I was tired, so i left in this way
 */
void show_possible_keys(char inputc, int color, int print){
    static int possi_keyboard[26] = {0};
    
    if(print == 75){
        printf("\n\n\tkeyboard");
        for(int i = 0; i < 26; i++){
            if(i % 9 == 0) printf("\n" C_SPACE " ");
            coloring_output('A' + i, possi_keyboard[i]);
            printf(C_SPACE" " C_STD);
        }
	printf("\n");
	return;
    }
    
    // 3 verde, 2 amarelo, 1 cinza, 0 normal 
    switch(color){
        case Green:
            possi_keyboard[inputc - 'A'] = 3;
            break;
        case Yellow:
            if(possi_keyboard[inputc - 'A'] < Yellow){
                possi_keyboard[inputc - 'A'] = Yellow;
            }
            break;
        case Gray:
            if(possi_keyboard[inputc - 'A'] < Gray){
                possi_keyboard[inputc - 'A'] = Gray;
            }
            break;
    }
}

/*
 * function returns the number of words in the file so we can use it to randomize the selection of the word of the day
 */
int num_words_file(char* filename){
    FILE* arq = fopen(filename, "r");
    int num;
    char aux[LEN_WORD];

    if(arq == NULL){
        printf("\nerror open file num words\n");
    }

    while(fscanf(arq, "%s", aux) != - 1){
        num++;
    }
    fclose(arq);

    return num;
}

/*
 * function that randomly choose a word from the .txt file and than put it in uppercase
 */
char* selecting_word_day(char* filename, int num){
    FILE* arq = fopen(filename, "r");
    char* word_day = (char*) malloc(sizeof(char) * LEN_WORD);

    if(arq == NULL){
        printf("\nerror open file selectingwordday\n");
    }

    if(word_day == NULL){
        printf("\nerror allocating memory selectinwordday\n");
    }
    
    srand(time(NULL));
    int word_indice = rand() % num;

    do{
        fscanf(arq, "%s", word_day);
        word_indice--;
    }while(word_indice > 0);
    fclose(arq);
    
    uppercase(word_day);

    return word_day;
}

/*
 * this function creates an integer array witch each index corresponding to a letter of the alphabet and its value correspond to the number of times that letter shows up in the word of the day
 * this is the way I found out to be able to try to restrain the hint color, so if one user try to send a word with repeated character(ex: "aaaaa") the program will update the coloring output while printing it ( so in the "aaaaa" exemple we don't run the risk of printing 5 green a's)
 */
int* occurrence_char(char* word_day){
    int* occur_char = (int*) calloc(26, sizeof(int));

    if(occur_char == NULL){
        printf("\nerror allocating memory occurrencechar\n");
    }

    for(int i = 0; i < strlen(word_day); i++){
        occur_char[word_day[i] - 'A']++;
    }

    return occur_char;
}

/*
 * function responsible for printing the output and selecting its color
 * here we copy the occurrence_char array so we're able to decrement the values locally, don't changing its real value
 * and with this everytime we know that an input char belongs to the word of the day we look through the occurrence_char array to know if still have occurrence of that letter in the word of the day, then deciding what color to give
 */
/*
 * BEFORE, I'LL TRY TO IMPROVE IT
void printing_output(char* input, char* word_day, int* occurr_char){
    int color;
    int occur_local[26];
    memcpy(occur_local, occurr_char, 26 * sizeof(int));

    printf("\n\t" C_SPACE " ");
    for(int i = 0; i < strlen(input); i++){
        if(strchr(word_day, input[i]) != NULL){
            
            if(input[i] == word_day[i]){
                color = Green;
            }else{
                
                if(occur_local[input[i] - 'A'] > 0){
                    color = Yellow;
                }else{
                    color = Gray;
                } 
            }
            
            occur_local[input[i] - 'A']--;
        }else{
            color = Gray;
        }
        
        coloring_output(input[i], color);
        show_possible_keys(input[i], color, 0);
    }
}
 */
void printing_output(char* input, char* word_day, int* occurr_char){
    int color[LEN_WORD] = {0};
    int occur_local[26];
    memcpy(occur_local, occurr_char, 26 * sizeof(int));

    printf("\n\t" C_SPACE " ");
    for(int i = 0; i < strlen(input); i++){
        if(strchr(word_day, input[i]) != NULL){
            
            if(input[i] == word_day[i]){
                color[i] = Green;

		if(occur_local[input[i] - 'A'] <= 0){
			int j = 0;
			while(input[j] != input[i]){
				j++;
			}
			color[j] = Gray;
		}
		
            }else{
                
                if(occur_local[input[i] - 'A'] > 0){
                    color[i] = Yellow;
                }else{
                    color[i] = Gray;
                } 
            }
            
            occur_local[input[i] - 'A']--;
        }else{
            color[i] = Gray;
        }
    }

    for(int i = 0; i < strlen(input); i++){
	    coloring_output(input[i], color[i]);
	    show_possible_keys(input[i], color[i], DONTPRINT);
    }
}

void coloring_output(char letter, int color){
    switch(color){
        case Green:
            printf(C_GREEN "%c" C_SPACE " ", letter);
            break;
        case Yellow:
            printf(C_YELLOW "%c" C_SPACE " ", letter);
            break;
        case Gray:
            printf(C_GRAY "%c" C_SPACE " ", letter);
            break;
        case 0:
            printf(C_STD "%c" C_SPACE " ", letter);
            break;
    }
    printf(C_STD);
}

/*
 * game core function
 * responsible for getting the user input and working with the others functions to give the desirable output
 */
void play(char* filename){
        char input[LEN_WORD];
        int num_tries = 6;
        int compare; 
        
        int num = num_words_file(filename);
        char* word_day = selecting_word_day(filename, num);
        int* occurr_char = occurrence_char(word_day);
        
        do{
            do{
                printf("\nword with 5 letters:\n ");
                scanf("%s", input);
                
                if(strlen(input) != LEN_WORD) printf("\nwrong length");
            }while(strlen(input) != LEN_WORD);
            
            uppercase(input);
            
            printing_output(input, word_day, occurr_char);
            show_possible_keys(IGNORE, IGNORE, YESPRINT);
            
            compare = strcmp(input, word_day);
            
            if(compare == 0){
                printf("\nyou won\n");
                return;
            }
            
            num_tries--;
        }while(compare != 0 && num_tries > 1);
        
        printf("\n\n\tperdeu\n burrao a resposta era: %s\n", word_day);
}
