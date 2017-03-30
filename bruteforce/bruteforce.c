/*************************************************************
 * Author: Pawel Drapiewski                                  *
 * This program make simply brutforce attack on program      *
 * specified by user in program parameter                    *
 * Execution: brutforce [program path] [username] [password] *
 *************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
  int max_pswd_length;
  int charset_length;
  char * brutforce_char_set;
  int result;

  if( argc < 4 ) {
    printf("[ERROR] You need 3 parametres: [program_path] [user_login] [password_max_lengts]\n");
    return EXIT_FAILURE;
  }

  max_pswd_length = atoi(argv[3]);
  printf("%d\n", max_pswd_length);

  if( (max_pswd_length < 1) || (max_pswd_length > 1024)) {
    printf("[ERROR] Maximum password length must be greater than 0 and smaller than 1025\n");
    return EXIT_FAILURE;
  }

  if( check_if_file_exist(argv[1]) == FILE_DOESNT_EXISTS ) {
    printf("[ERROR] Program \'%s\' doesn't exists!\n", argv[1]);
    return EXIT_FAILURE;
  }

  charset_length = 0;
  brutforce_char_set = init_char_set(&charset_length);
  #if (DEBUG)
    printf("Charset length: %d\n", charset_length);
    print_array(brutforce_char_set);
  #endif

  result = start_brutforce(argv[1], argv[2], max_pswd_length, brutforce_char_set, charset_length);

  return result;
}


int start_brutforce(const char * program_path, const char * username, int password_length, char * brutforce_char_set, int charset_length) {
  int * actual_pswd_pattern;
  char * password_to_check;
  int is_all_patterns_checked;
  int i;

  actual_pswd_pattern = (int *) malloc((password_length) * sizeof(int)); /*stores password as indexes of brutforce_char_set array */
  init_actual_pswd_pattern(actual_pswd_pattern, password_length);
  password_to_check = (char *) malloc((password_length + 1) * sizeof(char)); /* stores password as string */

  is_all_patterns_checked = 0;
  i = 0;
  while( !is_all_patterns_checked) {
    transform_pswd_to_string(actual_pswd_pattern, password_length, brutforce_char_set, password_to_check);
    if( check_password(program_path, username, password_to_check) == CORRECT_PASSWORD ) {
      printf("Found password: %s\n", password_to_check);
      return 0;
    }
    #if( DEBUG )
      printf("\tActual checked password: %s\n", password_to_check);
    #endif
    i++;
    is_all_patterns_checked = generate_next_password_pattern(actual_pswd_pattern, password_length, charset_length);
  }

  printf("Sorry, password not found. May try with longer password.\n");
  return 1;
}

/* generate next password pattern. Returns 1 when there is no next pattern, 0 if OK*/
int generate_next_password_pattern(int * actual_password_indexes, int password_length, int charset_length) {
   int i;
   int j;
   int k;


   i = 1; /*TODO here could be problems */
   while( actual_password_indexes[i] != -1 && i < password_length) {
      i++;
    }

    i--; /* this is index of the last char in password pattern */

    if(actual_password_indexes[i] == charset_length) {
      /* if end char or chars have maximum value than find previous element who isn't*/
      k = i;
      while( actual_password_indexes[k] == charset_length) {
        k--;
        /*if all elemntes have maximum value than exapnd password */
        if( k == -1)  {
            /* when password have maxium length with all maxiumum values than there is no more options */
            if(i == (password_length - 1))
                return 1; /* we can't generete next pattern because our password has already max lenght */

            j = i + 1;
            for(; j >= 0; j--) {
              actual_password_indexes[j] = 0; /*assign index of first char in char set array */
           }
        } else {
          /* add value of first non maximum elements, and from this char to end assign first char value*/
          actual_password_indexes[k] =+ actual_password_indexes[k] + 1;
          j = k + 1;
          while(actual_password_indexes[j] != -1 && j < password_length) {
            actual_password_indexes[j] = 0;
            j++;
          }
        }
      } /*while closing*/
    } else {
      actual_password_indexes[i] = actual_password_indexes[i] + 1;
    }

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
  char * result_char_set;
  int array_elem_nmb;
  int char_a_code;
  int char_z_code;
  int range;
  int i;
  int start_index;


  result_char_set = (char *) malloc(BUFF_SIZE * sizeof(char));
  array_elem_nmb = 0;

  /*put small letters into array*/
  char_a_code = 97;
  char_z_code = 122;
  range = char_z_code - char_a_code;
  i;
  for (i = 0; i <= range; i++){
    result_char_set[i] = (char)(char_a_code + i);
    array_elem_nmb++;
  }

  /*put big letters into array*/
  char_a_code = 65;
  char_z_code = 90;
  range = char_z_code - char_a_code;

  start_index = array_elem_nmb;

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
  int i;
  printf("Charset for brutforce attack:\n");
  i = 0;
  while(array[i] != '\0') {
    printf("[%d] %c (addr: %d)\n", i, array[i], &array[i]);
    i++;
  }
}
