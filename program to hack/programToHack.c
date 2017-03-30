/***********************************************************
 * Author: Pawel Drapiewski                                *
 * This program only validate user inputs as username and  *
 * password and return 0 if everything is ok               *
 ***********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>

#define USERNAME "drapek"
#define PASSWORD "g8"
#define BUFF_SIZE 1024

int authorization( char * input_usrnm, char * input_psswd) ;

int main( int argc, char ** argv ) {
	char * input_username;
	char * input_password;

	if( argc < 3 ) {
		printf("Error, you need to 2 parametres: username and password!\n");
		return EXIT_FAILURE;
	}

	input_username = argv[1];
	input_password = argv[2];

	if( !authorization(input_username, input_password) ){
		printf("You gained access!\n");
		return EXIT_SUCCESS;
	}
	else {
	    printf("Wrong username or password.\n");
		return EXIT_FAILURE;
	}
}

int authorization( char * input_usrnm, char * input_psswd) {
	if( strcmp( input_usrnm, USERNAME ) == 0 &&
	    strcmp( input_psswd, PASSWORD ) == 0 ) {
	 	return 0;
	 }
	else {
		return -1;
	}
}
