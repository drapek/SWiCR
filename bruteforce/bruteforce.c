/*************************************************************
 * Author: Pawel Drapiewski                                  *
 * This program make simply brutforce attack on program      *
 * specified by user in program parameter                    *
 * Execution: brutforce [program path] [username] [password] *
 *************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define WRONG_PASSWORD 0
#define CORRECT_PASSWORD 1
#define FILE_EXISTS 1
#define FILE_DOESNT_EXISTS 0
#define BUFF_SIZE 1024
#define DEBUG 1

char * create_execution_command(const char * program_path, const char * username, char * password );
int check_password(const char * program_path, const char * username, char * password );
int check_if_file_exist(const char * program_path);
char * init_char_set();
void print_array(char * array);
int start_brutforce(const char * program_path, const char * username, int password_length, char * brutforce_char_set, int charset_length);
void init_actual_pswd_pattern(int * actual_pswd_pattern, int charset_length);
void transform_pswd_to_string(int * actual_pswd_pattern, int password_length, char * charset, char * store_pswd_here);
int generate_next_password_pattern(int * actual_password_indexes, int password_length, int charset_length);

int main(int argc, char const *argv[]) {
  if( argc < 4 ) {
    printf("[ERROR] You need 3 parametres: [program_path] [user_login] [password_max_lengts]\n");
    return EXIT_FAILURE;
  }

  int max_pswd_length = atoi(argv[3]);
  printf("%d\n", max_pswd_length);

  if( (max_pswd_length < 1) || (max_pswd_length > 1024)) {
    printf("[ERROR] Maximum password length must be greater than 0 and smaller than 1025\n");
    return EXIT_FAILURE;
  }

  if( check_if_file_exist(argv[1]) == FILE_DOESNT_EXISTS ) {
    printf("[ERROR] Program \'%s\' doesn't exists!\n", argv[1]);
    return EXIT_FAILURE;
  }

  int charset_length = 0;
  char * brutforce_char_set = init_char_set(&charset_length);
  #if (DEBUG)
    printf("Charset length: %d\n", charset_length);
    print_array(brutforce_char_set);
  #endif

  start_brutforce(argv[1], argv[2], max_pswd_length, brutforce_char_set, charset_length);

  /*test - delete after it*/
  printf("Result: %s\n", (check_password(argv[1], argv[2], "xcw") == CORRECT_PASSWORD) ? "passed" : "not passed");

  return 0;
}


int start_brutforce(const char * program_path, const char * username, int password_length, char * brutforce_char_set, int charset_length) {
  int * actual_pswd_pattern = (int *) malloc((password_length) * sizeof(int)); /*stores password as indexes of brutforce_char_set array */
  init_actual_pswd_pattern(actual_pswd_pattern, password_length);
  char * password_to_check = (char *) malloc((password_length + 1) * sizeof(char)); /* stores password as string */

  //TODO analize code below - beacuse for sure it going to trash :D
  // int i;
  // for(i = 0; i < 100; i++) {
  //   #if (DEBUG)
  //     printf("Actual proceed password: %s\n", actual_pswd_pattern[0]);
  //
  //     int i = 0;
  //     while(actual_pswd_pattern[i] != NULL){
  //       printf("\f%c (addr: %d)\n", *actual_pswd_pattern[i], actual_pswd_pattern[i]);
  //       i ++;
  //     }
  //   #endif
  //   generate_next_password_pattern(actual_pswd_pattern, brutforce_char_set, password_length);

  //}
  int all_patterns_checked = 0;
  int i = 0;
  while( !all_patterns_checked && i < 100) {
    transform_pswd_to_string(actual_pswd_pattern, password_length, brutforce_char_set, password_to_check);
    #if( DEBUG )
      printf("\tActual checked password: %s\n", password_to_check);
    #endif
    i++;
    all_patterns_checked = generate_next_password_pattern(actual_pswd_pattern, password_length, charset_length);
}


}

/* generate next password pattern. Returns 1 when there is no next pattern, 0 if OK*/
int generate_next_password_pattern(int * actual_password_indexes, int password_length, int charset_length) {
   int i = 1;
   while( actual_password_indexes[i] != -1 && i < password_length) {
      i++;
    }

    i--; /* this is index of the last char in password pattern */

    actual_password_indexes[i] = actual_password_indexes[i] + 1;

    //TODO repair this part of code!
    // /* check if it is the last char in charset */
    // if(actual_password_indexes[i] == charset_length - 1) {
    //   if(i == (password_length - 1))
    //     return 1; /* we can't generete next pattern because our password has already max lenght */
    //
    //   int k = i;
    //   while( actual_password_indexes[k] == (charset_length - 1)) {
    //     if (k != 0 ) k--;
    //     /*if all chars are the last char of charset than expand password and initialized it. */
    //     if(k == 0) {
    //       int j = i + 1;
    //       for(; j >= 0; j--) {
    //         actual_password_indexes[j] = 0; /*assign index of first char in char set array */
    //       }
    //     }
    //    }
    // actual_password_indexes[k] = actual_password_indexes[k] + 1;
    // }

    return 0;
}

void init_actual_pswd_pattern(int * actual_pswd_pattern, int charset_length){
  int i;
  for(i = 0; i < charset_length; i++){
    actual_pswd_pattern[i] = -1;
  }
}


void transform_pswd_to_string(int * actual_pswd_pattern, int password_length, char * charset, char * store_pswd_here) {
  int i = 0;
  while( actual_pswd_pattern[i] != -1 && i < password_length ) {
    store_pswd_here[i] = charset[actual_pswd_pattern[i]];
    i++;
  }

  store_pswd_here[i] = '\0';
}

/* Check if username and password for given program is correct */
int check_password(const char * program_path, const char * username, char * password ) {
  char * command = create_execution_command(program_path, username, password);

  if (system(command) == 0) {
    return CORRECT_PASSWORD;
  }
  else {
    return WRONG_PASSWORD;
  }
}

/* Creates command with parameters to execute it in system() function */
char * create_execution_command(const char * program_path, const char * username, char * password ) {
  char * command_to_execute = malloc(BUFF_SIZE * sizeof(char));
  char * space = " ";
  char * program_runner = "./";

  strcat(command_to_execute, program_runner);
  strcat(command_to_execute, program_path);
  strcat(command_to_execute, space);
  strcat(command_to_execute, username);
  strcat(command_to_execute, space);
  strcat(command_to_execute, password);
  strcat(command_to_execute, " 1> /dev/null"); /*ignores stdout of hacking program*/

  return command_to_execute;
}

/* chec if exists program under giver program path */
int check_if_file_exist(const char * program_path) {
  if(access(program_path, F_OK) != -1) {
      return FILE_EXISTS;
  }
  else {
    return FILE_DOESNT_EXISTS;
  }
}

char * init_char_set(int * charset_length) {
  char * result_char_set = (char *) malloc(BUFF_SIZE * sizeof(char));
  int array_elem_nmb = 0;

  /*put small letters into array*/
  int char_a_code = 97;
  int char_z_code = 122;
  int range = char_z_code - char_a_code;
  int i;
  for (i = 0; i <= range; i++){
    result_char_set[i] = (char)(char_a_code + i);
    array_elem_nmb++;
  }

  /*put big letters into array*/
  char_a_code = 65;
  char_z_code = 90;
  range = char_z_code - char_a_code;

  int start_index = array_elem_nmb;

  for (i = 0; i <= range; i++){
    result_char_set[i + start_index] = (char)(char_a_code + i);
    array_elem_nmb++;
  }

  /*put numbers into array*/
  char_a_code = 48;
  char_z_code = 57;
  range = char_z_code - char_a_code;

  start_index = array_elem_nmb;

  for (i = 0; i <= range; i++){
    result_char_set[i + start_index] = (char)(char_a_code + i);
    array_elem_nmb++;
  }

  *charset_length = array_elem_nmb;
  result_char_set[array_elem_nmb] = '\0'; /*the last element of array shoud be null*/

  return result_char_set;
}

void print_array(char * array) {
  printf("Charset for brutforce attack:\n");
  int i = 0;
  while(array[i] != '\0') {
    printf("[%d] %c (addr: %d)\n", i, array[i], &array[i]);
    i++;
  }
}
