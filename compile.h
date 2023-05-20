#pragma once
// built in headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>  // for directory
#include <stdarg.h>  // to accept variable number of arguments
#include <stdbool.h> // for boolean

// user defined headers
#include "y.tab.h"
#include "parser.h"

/**
 * @brief Set the Quadrable File Path object & open the file
 * @param filePath the path of the file
 */
void setQuadrableFilePath(char * filePath , FILE * file);

/**
 * @brief Clear the Quadrable File Path object & close the file
 *
 */
void clearQuadrableFilePath();

/**
 * @brief Function that handles the constant nodes whether , integer , float , bool , char and string
 *
 * @param p the node to be handled
 * @return struct conNodeType*
 */
struct conNodeType *handle_const_node(nodeType *p);

/**
 * @brief Function that handles the type definition nodes
 *
 * @param p the node to be handled
 * @return struct conNodeType*
 */
struct conNodeType *handle_def_node(nodeType *p);

/**
 * @brief Function that handles the identifier nodes & variables operations
 *
 * @param p the node to be handled
 * @return struct conNodeType*
 */
struct conNodeType *handle_identifier_node(nodeType *p);

bool is_binary_operation(int operation);

char *get_operation_string(int operand);

struct conNodeType *get_operand_value(nodeType *p);

/**
 * @brief Function that handles the operation nodes
 *
 * @param p the node to be handled
 * @return struct conNodeType*
 */
struct conNodeType *handle_operation_node(nodeType *p);

/**
 * @brief Function that handles most of nodes & their operations
 *
 * @param p the node to be handled
 * @return struct conNodeType* the result of the operation
 */
struct conNodeType *ex(nodeType *p);

/**
 * @brief Entry point of the compiler that executes the program
 *
 * @param p the root node of the AST
 */
void execute(nodeType *p);