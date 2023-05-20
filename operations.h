#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "parser.h"
#include "y.tab.h"

// void yyerror(char *);

/**
 * @brief this functions handles all the math and logical operations between two operands
 * 
 * @param operand1 the node of the first operand
 * @param operand2 the node of the second operand
 * @param operator_type the type of the operation whether ADD, SUB, MUL, DIV, MOD, OR, AND, EQUAL_TO, NOT_EQUAL, GT, GTE, LT, LTE
 * @return struct conNodeType* the node of the result of the operation
 */
struct conNodeType* handle_operations(
  struct conNodeType* operand1, 
  struct conNodeType* operand2, 
  int operator_type
) {

    if(operand1 == NULL && operand2 == NULL) {
        yyerror("ERROR: NULL operand!\n");
        return NULL;
    }

    if(
      (operand1->type != typeInt && operand1->type != typeFloat && operand1->type != typeBool) || 
      (operand2->type != typeInt && operand2->type != typeFloat && operand2->type != typeBool)
    ){
        yyerror("ERROR: can't make math/logical operations to non-int, non-float operands\n");
        return NULL;
    }
    
  
    struct conNodeType* resultNode = malloc(sizeof(struct conNodeType*));
    bool isFloat = false;
    if(operand1->type == typeFloat || operand2->type == typeFloat) {
      isFloat = true;
      resultNode->type = typeFloat;
    }else {
      resultNode->type = typeInt;
    }

    float opr1_value = operand1->type == typeFloat ? 
      operand1->fValue : 
      (int)operand1->iValue;
    
    float opr2_value = operand2->type == typeFloat ? 
      operand2->fValue : 
      (int)operand2->iValue;

    switch (operator_type)
    {
      case ADD:
        if(isFloat) {
          resultNode->fValue = opr1_value + opr2_value;
        }else {
          resultNode->iValue = (int)opr1_value + (int) opr2_value;
        }
        break;

      case SUB:
        if(isFloat) {
          resultNode->fValue = opr1_value - opr2_value;
        }else {
          resultNode->iValue = (int)opr1_value - (int) opr2_value;
        }

        if(isFloat){
          resultNode->fValue = opr1_value - opr2_value;
        }else{
          resultNode->iValue = (int)opr1_value - (int)opr2_value;
        }
        break;

      case MUL:
        if(isFloat){
          resultNode->fValue = opr1_value * opr2_value;
        }else{
          resultNode->iValue = (int)opr1_value * (int)opr2_value;
        }
        break;

      case DIV:
        if(opr2_value == 0) {
          yyerror("ERROR: division by zero!\n");
          return NULL;
        }

        if(isFloat){
          resultNode->fValue = opr1_value / opr2_value;
        }else {
          resultNode->iValue = (int)opr1_value / (int)opr2_value;
        }
        break;

      case MOD:
        if(opr2_value == 0) {
          yyerror("ERROR: division by zero!\n");
          return NULL;
        }

        if(isFloat){
          resultNode->fValue = (int)opr1_value % (int)opr2_value;
        }else {
          resultNode->iValue = (int)opr1_value % (int)opr2_value;
        }
        break;


      case OR:
        //TODO: check if we need this check or we can allow logical operations on int
        if(operand1->type != typeBool || operand2->type != typeBool) {
          yyerror("ERROR: can't make logical operations to non-bool operands\n");
          return NULL;
        }
        resultNode->type = typeBool;
        resultNode->iValue = (int)opr1_value || (int)opr2_value;
        break;

      case AND:
        //TODO: check if we need this check or we can allow logical operations on int
        if(operand1->type != typeBool || operand2->type != typeBool) {
          yyerror("ERROR: can't make logical operations to non-bool operands\n");
          return NULL;
        }
        resultNode->type = typeBool;
        resultNode->iValue = (int)opr1_value && (int)opr2_value;
        break;


      case EQUAL_TO:
        resultNode->type = typeBool;
        resultNode->iValue = (opr1_value == opr2_value);
        break;

      case NOT_EQUAL:
        resultNode->type = typeBool;
        resultNode->iValue = (opr1_value != opr2_value);
        break;

      case GT:
        resultNode->type = typeBool;
        resultNode->iValue = (opr1_value > opr2_value);
        break;

      case GTE:
        resultNode->type = typeBool;
        resultNode->iValue = (opr1_value >= opr2_value);
        break;

      case LT:
        resultNode->type = typeBool;
        resultNode->iValue = (opr1_value < opr2_value);
       break;

      case LTE:
        resultNode->type = typeBool;
        resultNode->iValue = (opr1_value <= opr2_value);
        break;

      default:
        break;
    }

    return resultNode;
}

/**
 * @brief handle unary operations
 * 
 * @param operand1 the operand node
 * @param operator_type the type of the operator whether it's UMINUS or NOT
 * @return struct conNodeType* 
 */
struct conNodeType* handle_unary_operations( struct conNodeType* operand1, int operator_type) {
    if(operand1 == NULL) {
      yyerror("ERROR: NULL operand!\n");
      return NULL;
    }

    if(operand1->type != typeInt && operand1->type != typeFloat && operand1->type != typeBool) {
      yyerror("ERROR: can't make math/logical operations to non-int, non-float , non-bool operand\n");
      return NULL;
    }
    struct conNodeType* resultNode = malloc(sizeof(struct conNodeType*));
    bool isFloat = (operand1->type == typeFloat);

    switch (operator_type)
    {
      case UMINUS:
        // only one operand
        if(isFloat){
          resultNode->fValue = -operand1->fValue;
        }else {
          resultNode->iValue = -operand1->iValue;
        }
        break;

      case NOT:
        // only one operand
        if(isFloat){
          resultNode->fValue = !operand1->fValue;
        }else {
          resultNode->iValue = !operand1->iValue;
        }
        break;

      default:
        break;
    }

    return resultNode;
} 