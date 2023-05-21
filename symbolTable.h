#pragma once
// built in headers
#include <dirent.h>   // for directory
#include <stdarg.h>   // to accept variable number of arguments
#include <stdbool.h>  // for boolean
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// user defined headers
#include "parser.h"

// we want to use the code with c language so we we used this
#ifdef __cplusplus
extern "C" {
#endif

typedef enum { down = -1, up = 1 } scopeEnum;

/**
 * @brief Set the Table Path object & opens the file to write to it
 * @param path the path of the symbol table file
 */
void setTablePath(char* path);

/**
 * @brief Clear the Table Path object & closes the file of the symbol table
 */
void clearTablePath();

/**
 * @brief function used to move the scope of the symbol table
 * to handle the nested scopes & functions
 * @param scope_dir integer value to indicates whether we will go up or down in
 * the scope
 */
void changeScope(scopeEnum scope_dir);

/**
 * @brief insert a new variable to the symbol table
 *
 * @param var_name  the name of the variable
 * @param var_type  the type of the variable
 * @param var_value the value of the variable
 * @param is_const  whether the variable is constant or not
 * @param has_value whether the variable is initialized or not
 * @param error     the error message if there is an error
 * @return  struct conNodeType* the created node for the variable
 */
struct conNodeType* insertNewVariable(char* var_name, conEnum var_type,
                                      struct conNodeType var_value,
                                      bool is_const, bool has_value,
                                      char** error);

/**
 * @brief get the value of a variable from the symbol table
 *
 * @param var_name the name of the variable
 * @param error    the error message if there is an error
 * @return struct conNodeType* the node of the variable
 */
struct conNodeType* getVariable(char* var, char** error);

/**
 * @brief print the symbol table
 */
void printSymbolTable();

#ifdef __cplusplus
}  // end extern "C"
#endif