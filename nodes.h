#pragma once
// built in headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h> // for directory
#include <stdarg.h> // to accept variable number of arguments
#include <stdbool.h> // for boolean

// user defined headers
#include "y.tab.h"
#include "parser.h"

void yyerror(char *);

/**
 * @brief Create a type node object
 * 
 * @param type the type of the node
 * @return nodeType* 
 */
nodeType * create_type_node(conEnum type){
    nodeType *p;

    // allocate the node
    if ((p = malloc(sizeof(nodeType))) == NULL)
        yyerror("out of memory");

    /* copy information */
    p->type = typeDef;
    p->typ.type = type;

    return p;
}


/**
 * @brief Create a const node object
 * @return nodeType* 
 */
nodeType * create_const_node(){
    nodeType *p;

    // allocate the node
    if ((p = malloc(sizeof(nodeType))) == NULL)
        yyerror("out of memory");

    /* copy information */
    p->type = typeCon;
    return p;
}


/**
 * @brief Create a int node object
 * 
 * @param value the value of the integer
 * @return nodeType* 
 */
nodeType * create_int_node(int value) {
    nodeType *p = create_const_node();
    p->con.type = typeInt;
    p->con.iValue = value;
    
    return p;
}


/**
 * @brief Create a float node object
 * 
 * @param value the value of the float
 * @return nodeType* 
 */
nodeType * create_float_node(float value) {
    nodeType *p = create_const_node();

    p->con.type = typeFloat;
    p->con.fValue = value;

    return p;
}


/**
 * @brief Create a bool node object
 * 
 * @param value the value of the bool
 * @return nodeType* 
 */
nodeType * create_bool_node(bool value) {
    nodeType *p = create_const_node();

    p->con.type = typeBool;
    p->con.iValue = value;

    return p;
}


/**
 * @brief Create a char node object
 * 
 * @param value the value of the char
 * @return nodeType* 
 */
nodeType * create_char_node(char value) {
    nodeType *p = create_const_node();

    p->con.type = typeChar;
    p->con.cValue = value;

    return p;
}


/**
 * @brief Create a string node object
 * 
 * @param value the value of the string
 * @return nodeType* 
 */
nodeType * create_string_node(char* value) {
    nodeType *p = create_const_node();

    p->con.type = typeString;
    p->con.sValue = value;

    return p;
}


/**
 * @brief Create a identifier node object
 * 
 * @param id the name of the identifier
 * @return nodeType* 
 */
nodeType * create_identifier_node(char* id) {

    nodeType *p;

    // allocate the node
    if ((p = malloc(sizeof(nodeType))) == NULL)
        yyerror("out of memory");

    /* copy information */
    p->type = typeId;
    p->id.id = id;

    return p;
}


/**
 * @brief Create a oper node object
 * 
 * @param oper the operator
 * @param nops the number of operands
 * @param ...  all the operands
 * @return nodeType* 
 */
nodeType * create_oper_node(int oper, int nops, ...) {

    va_list ap;
    nodeType *p;
    int i;

    /* allocate node, extending op array */
    if ((p = malloc(sizeof(nodeType) + (nops-1) * sizeof(nodeType *))) == NULL)
        yyerror("out of memory");

    // assign values
    p->type = typeOpr;
    p->opr.oper = oper;
    p->opr.nops = nops;

    // copy all the operands
    va_start(ap, nops);
    for (i = 0; i < nops; i++)
        p->opr.op[i] = va_arg(ap, nodeType*);
    va_end(ap);
    return p;
}


/**
 * @brief free the node from memory
 * @param p the node to be freed
 */
void freeNode(nodeType *p) {
    
    int i;

    if (!p) return;
    if (p->type == typeOpr) {
        for (i = 0; i < p->opr.nops; i++)
            freeNode(p->opr.op[i]);
    }
    free (p);
}