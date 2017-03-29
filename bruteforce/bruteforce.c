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
int start_brutforce(const char * program_path, const char * username, int password_length, char * brutforce_char_set);
int generate_next_password_pattern(char ** password_pattern, char * brutforce_char_set, int password_length);
void init_password_pattern(char ** password_pattern, int password_length);

struct password_pattern {
  char actual_char;
  int actual_char_index;
  int char_max_index;
}

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


  char * brutforce_char_set = init_char_set();
  #if (DEBUG)
    print_array(brutforce_char_set);
  #endif

  start_brutforce(argv[1], argv[2], max_pswd_length, brutforce_char_set);

  /*test - delete after it*/
  printf("Result: %s\n", (check_password(argv[1], argv[2], "xcw") == CORRECT_PASSWORD) ? "passed" : "not passed");

  return 0;
}


int start_brutforce(const char * program_path, const char * username, int password_length, char * brutforce_char_set) {
  char ** actual_pswd_pattern = (char **) malloc((password_length + 1) * sizeof(char *));
  actual_pswd_pattern[password_length] = '\0';

  actual_pswd_pattern[0] = &brutforce_char_set[0]; /* make 'a' as password pattern */

  int i;
  for(i = 0; i < 100; i++) {
    #if (DEBUG)
      printf("Actual proceed password: %s\n", actual_pswd_pattern[0]);

      int i = 0;
      while(actual_pswd_pattern[i] != NULL){
        printf("\f%c (addr: %d)\n", *actual_pswd_pattern[i], actual_pswd_pattern[i]);
        i ++;
      }
    #endif
    generate_next_password_pattern(actual_pswd_pattern, brutforce_char_set, password_length);

  }

}

/* generate next password pattern. Returns 1 when there is no next pattern, 0 if OK*/
int generate_next_password_pattern(char ** password_pattern, char * brutforce_char_set, int password_length) {
   int i = 0;
   while( password_pattern[i] != NULL) {
      i++;
    }

    i--; /* this is index of the last char in password pattern */

    password_pattern[i] = password_pattern[i] + 1;

    if(password_pattern[i] == '\0') {
      if(i == password_length)
        return 1; /* we can't generete next pattern because our password has already max lenght */

      int j = i + 1;
      for(; j >= 0; j--) {
        password_pattern[j] = &brutforce_char_set[0];
      }
    }

    return 0;
}

void init_password_pattern(char ** password_pattern, int password_length) {
  int i;
  for(i = 0; i <= password_length; i++)
    password_pattern[i] = NULL;
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

char * init_char_set() {
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
