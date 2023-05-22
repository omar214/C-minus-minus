#include "compile.h"

char *quadrableFilePath = NULL;
FILE *quadrableFile = NULL;

typedef struct functionLinkedList {
  char *name;
  struct functionLinkedList *next;
} functionLinkedList;

functionLinkedList *functionListHeader = NULL;

static int global_curr_label = 0;
static int global_curr_reg = 0;
static int global_switch_lbl = 0;
static int global_case_lbl = 0;
static int is_inside_loop = 0;
static int is_inside_switch = 0;

char *var;                // identifier name
char *global_error = "";  // global variable to store any error message

/**
 * @brief Set the Quadrable File Path object & open the file
 * @param filePath the path of the file
 */
// void setQuadrableFilePath(char *filePath)
void setQuadrableFilePath(char *filePath, FILE *file) {
  quadrableFilePath = filePath;
  quadrableFile = file;
}

/**
 * @brief Clear the Quadrable File Path object & close the file
 *
 */
void clearQuadrableFilePath() {
  quadrableFilePath = NULL;
  quadrableFile = NULL;
}

/**
 * @brief Function that handles the constant nodes whether , integer , float ,
 * bool , char and string
 *
 * @param p the node to be handled
 * @return struct conNodeType*
 */
struct conNodeType *handle_const_node(nodeType *p) {
  struct conNodeType *resultNode = malloc(sizeof(struct conNodeType *));

  conEnum currentConType = p->con.type;
  char *string_to_write = malloc(sizeof(char) * 100);

  switch (currentConType) {
    case typeInt:
      resultNode->iValue = p->con.iValue;
      resultNode->type = typeInt;
      sprintf(string_to_write, "\tpush\t%d\n", p->con.iValue);
      break;

    case typeFloat:
      resultNode->fValue = p->con.fValue;
      resultNode->type = typeFloat;
      sprintf(string_to_write, "\tpush\t%f\n", p->con.fValue);
      break;

    case typeBool:
      resultNode->iValue = p->con.iValue;
      resultNode->type = typeBool;
      sprintf(string_to_write, "\tpush\t%d\n", p->con.iValue);
      break;

    case typeChar:
      resultNode->cValue = p->con.cValue;
      resultNode->type = typeChar;
      sprintf(string_to_write, "\tpush\t\'%c\'\n", p->con.cValue);
      break;

    case typeString:
      resultNode->sValue = p->con.sValue;
      resultNode->type = typeString;
      sprintf(string_to_write, "\tpush\t\"%s\"\n", p->con.sValue);
      break;

    default:
      break;
  }

  fprintf(quadrableFile, "%s", string_to_write);
  return resultNode;
}

/**
 * @brief Function that handles the type definition nodes
 *
 * @param p the node to be handled
 * @return struct conNodeType*
 */
struct conNodeType *handle_def_node(nodeType *p) {
  struct conNodeType *resultNode = malloc(sizeof(struct conNodeType *));

  // set the type of node , whether int, float ...
  resultNode->type = p->typ.type;

  return resultNode;
}

/**
 * @brief Function that handles the identifier nodes & variables operations
 *
 * @param p the node to be handled
 * @return struct conNodeType*
 */
struct conNodeType *handle_identifier_node(nodeType *p,
                                           struct conNodeType **resultNode) {
  char *identifierName = p->id.id;

  //  get the variable from the symbol table
  struct conNodeType *temp = malloc(sizeof(struct conNodeType *));
  getVariable(identifierName, &global_error, &temp);

  // assign the result node
  *resultNode = temp;

  fprintf(quadrableFile, "\tpush\t%s\n",
          identifierName);  // push the identifier name to the stack
  bool hasError = strcmp(global_error, "") != 0 || resultNode == NULL;
  if (hasError) {
    printf("ERROR: %s\n", global_error);
    yyerror(global_error);
    global_error = "";
  }

  return NULL;
}

/**
 * @brief function that checks if this operation is binary or not
 *
 * @param operation the operation to be checked
 * @return true if binary operation
 * @return false if not binary operation
 */
bool is_binary_operation(int operation) {
  int operationsArr[] = {ADD, SUB, MUL,      DIV,       MOD, LT, LTE,
                         GT,  GTE, EQUAL_TO, NOT_EQUAL, AND, OR};

  int arrSize = sizeof(operationsArr) / sizeof(int);
  for (int i = 0; i < arrSize; i++) {
    if (operation == operationsArr[i]) return true;
  }

  return false;
}

/**
 * @brief Get the operation string object
 *
 * @param operand the operation to be converted to string
 * @return char* the string of the operation
 */
char *get_operation_string(int operand) {
  switch (operand) {
    case ADD:
      return "\tplus\n";
      break;
    case SUB:
      return "\tsubtract\n";
      break;
    case MUL:
      return "\tmul\n";
      break;
    case DIV:
      return "\tdiv\n";
      break;
    case MOD:
      return "\tmod\n";
      break;
    case LT:
      return "\tcompLT\n";
      break;
    case LTE:
      return "\tcompLTE\n";
      break;
    case GT:
      return "\tcompGT\n";
      break;
    case GTE:
      return "\tcompGTE\n";
      break;
    case EQUAL_TO:
      return "\tcompEQ\n";
      break;
    case NOT_EQUAL:
      return "\tcompNEQ\n";
      break;
    case AND:
      return "\tand\n";
      break;
    case OR:
      return "\tor\n";
      break;
    case NOT:
      return "\tnot\n";
      break;
    case UMINUS:
      return "\tneg\n";
      break;
    default:
      return "";
      break;
  }
}

/**
 * @brief Get the operand value object , whether it's a constant or an
 * identifier or nested operation
 *
 * @param p the node to be handled
 * @return struct conNodeType* the value of the operand
 */
struct conNodeType *get_operand_value(nodeType *p) {
  struct conNodeType *resultNode = malloc(sizeof(struct conNodeType *));

  switch (p->type) {
    case typeCon:
      return handle_const_node(p);
      break;
    case typeId:
      handle_identifier_node(p, &resultNode);
      return resultNode;
      break;
    case typeOpr:
      // in case of nested operations
      // like 2 + 3 * 4
      return ex(p);
      break;

    default:
      break;
  }
}

char *get_case_value_to_compare_as_string(nodeType *p) {
  char *result = malloc(sizeof(char *));

  switch (p->con.type) {
    case typeInt:
      sprintf(result, "%d", p->con.iValue);
      break;
    case typeChar:
      sprintf(result, "\'%c\'", p->con.cValue);
      break;
    case typeString:
      sprintf(result, "\"%s\"", p->con.sValue);
      break;
    default:
      break;
  }

  return result;
}

/**
 * @brief Function that handles the operation nodes
 *
 * @param p the node to be handled
 * @return struct conNodeType*
 */
struct conNodeType *handle_operation_node(nodeType *p) {
  struct conNodeType *resultNode = malloc(sizeof(struct conNodeType *));
  int currOperation = p->opr.oper;

  if (is_binary_operation(currOperation)) {
    struct conNodeType *operand1 = get_operand_value(p->opr.op[0]);
    struct conNodeType *operand2 = get_operand_value(p->opr.op[1]);
    handle_operations(operand1, operand2, currOperation, resultNode);

    fprintf(quadrableFile, "%s", get_operation_string(currOperation));
    return resultNode;
  }

  bool isUnaryOperation = currOperation == UMINUS || currOperation == NOT;
  if (isUnaryOperation) {
    struct conNodeType *operand1 = get_operand_value(p->opr.op[0]);
    resultNode = handle_unary_operations(operand1, currOperation);

    fprintf(quadrableFile, "%s", get_operation_string(currOperation));
    return resultNode;
  }

  switch (currOperation) {
      // int identifier;
    case DECLARE: {
      // get the type of the variable
      struct conNodeType *declareTypeNode = ex(p->opr.op[0]);
      conEnum varType = declareTypeNode->type;

      // get the variable name
      nodeType *declareIdNode = p->opr.op[1];
      char *varName = declareIdNode->id.id;

      // insert the variable in the symbol table
      resultNode = insertNewVariable(varName, varType, *declareTypeNode, false,
                                     false, &global_error);
      break;
    }
    case ASSIGNMENT: {
      int numOfOps = p->opr.nops;

      // updating var
      // var = expr;
      if (numOfOps == 2) {
        // get the variable name
        char *varName = p->opr.op[0]->id.id;

        // get the variable value (expr)
        struct conNodeType *varValue = ex(p->opr.op[1]);

        if (!varValue) {
          yyerror("ERROR: Variable value is not valid");
          return NULL;
        }

        // we used typeNotDefined to indicate this is updating
        // & we don't the type of the variable
        resultNode = insertNewVariable(varName, typeNotDefined, *varValue,
                                       false, true, &global_error);

        if (!resultNode || global_error != "") {
          yyerror(global_error);
          global_error = "";
          return NULL;
        }

        fprintf(quadrableFile, "\tpop %s\n", varName);
        return resultNode;
      } else if (numOfOps == 3) {
        // declare with assignment
        // int var = 5;

        // get the variable type
        struct conNodeType *declareTypeNode = ex(p->opr.op[0]);
        conEnum varType = declareTypeNode->type;

        // get the variable name
        char *varName = p->opr.op[1]->id.id;

        // get the variable value (expr)
        struct conNodeType *varValue = ex(p->opr.op[2]);

        if (!varValue) {
          yyerror("ERROR: Variable value is not valid");
          return NULL;
        }

        resultNode = insertNewVariable(varName, varType, *varValue, false, true,
                                       &global_error);

        if (!resultNode || global_error != "") {
          yyerror(global_error);
          global_error = "";
          return NULL;
        }

        fprintf(quadrableFile, "\tpop %s\n", varName);
        return resultNode;
      } else if (numOfOps == 4) {
        // const declare with assignment
        // const int var = 5;

        // get the variable type
        struct conNodeType *declareTypeNode = ex(p->opr.op[1]);
        conEnum varType = declareTypeNode->type;

        // get the variable name
        char *varName = p->opr.op[2]->id.id;

        // get the variable value (expr)
        struct conNodeType *varValue = ex(p->opr.op[3]);

        if (!varValue) {
          yyerror("ERROR: Variable value is not valid");
          return NULL;
        }

        resultNode = insertNewVariable(varName, varType, *varValue, true, true,
                                       &global_error);

        if (!resultNode || global_error != "") {
          yyerror(global_error);
          global_error = "";
          return NULL;
        }

        fprintf(quadrableFile, "\tpop %s\n", varName);
        return resultNode;
      }

      break;
    }
    case WHILE: {
      int start_while_label = global_curr_label++;
      int end_while_label = global_curr_label++;
      struct conNodeType *while_condition = p->opr.op[0];
      struct conNodeType *while_body = p->opr.op[1];

      // set the inside loop flag to true
      is_inside_loop = true;

      // print the start label
      fprintf(quadrableFile, "L%03d:\n", start_while_label);

      // execute the condition
      ex(while_condition);

      // jumb to end_while if the condition is false
      fprintf(quadrableFile, "\tjz L%03d\n", end_while_label);

      // execute the body
      ex(while_body);

      // print the jump to the start
      fprintf(quadrableFile, "\tjmp L%03d\n", start_while_label);

      // print the end label
      fprintf(quadrableFile, "L%03d:\n", end_while_label);

      // set the inside loop flag to false
      is_inside_loop = false;

      return NULL;
      break;
    }
    case DO: {
      int start_do_while_label = global_curr_label++;
      int end_do_while_label = global_curr_label++;
      struct conNodeType *do_while_body = p->opr.op[0];
      struct conNodeType *do_while_condition = p->opr.op[1];

      // set the inside loop flag to true
      is_inside_loop = true;

      // print the start label
      fprintf(quadrableFile, "L%03d:\n", start_do_while_label);

      // execute the  body
      ex(do_while_body);

      // execute the condition
      ex(do_while_condition);

      // jumb to end_while if the condition is false
      fprintf(quadrableFile, "\tjz L%03d\n", end_do_while_label);

      // print the jump to the start
      fprintf(quadrableFile, "\tjmp L%03d\n", start_do_while_label);

      // print the end label
      fprintf(quadrableFile, "L%03d:\n", end_do_while_label);

      // clear the inside loop flag
      is_inside_loop = false;

      return NULL;
      break;
    }
    case FOR: {
      int start_for_label = global_curr_label++;
      int end_for_label = global_curr_label++;

      struct conNodeType *for_init_statement = p->opr.op[0];
      struct conNodeType *for_condition_statement = p->opr.op[1];
      struct conNodeType *for_update_statement = p->opr.op[2];
      struct conNodeType *for_body_statement = p->opr.op[3];

      // set the inside loop flag to true
      is_inside_loop = true;

      // execute the init statement
      ex(for_init_statement);

      // print the start label
      fprintf(quadrableFile, "L%03d:\n", start_for_label);

      // execute the condition
      ex(for_condition_statement);

      // jumb to end_for if the condition is false
      fprintf(quadrableFile, "\tjz\tL%03d\n", end_for_label);

      // execute the body
      ex(for_body_statement);

      // execute the update statement
      ex(for_update_statement);

      // jump to the start
      fprintf(quadrableFile, "\tjmp\tL%03d\n", start_for_label);

      // print the end label
      fprintf(quadrableFile, "L%03d:\n", end_for_label);

      // clear the inside loop flag
      is_inside_loop = false;

      return NULL;
      break;
    }
    case IF: {
      struct conNodeType *if_condition = p->opr.op[0];
      struct conNodeType *if_body = p->opr.op[1];
      int end_if_label = global_curr_label++;

      // execute the condition
      ex(if_condition);

      // jumb to end_if if the condition is false
      fprintf(quadrableFile, "\tjz\tL%03d\n", end_if_label);

      // execute the body
      ex(if_body);

      // check if has eles
      bool hasElse = p->opr.nops > 2;
      if (!hasElse) {
        // print the end label
        fprintf(quadrableFile, "L%03d:\n", end_if_label);

      } else {
        int end_else_label = global_curr_label++;

        // print the jump to the end_else
        fprintf(quadrableFile, "\tjmp\tL%03d\n", end_else_label);

        // print the end_if label
        fprintf(quadrableFile, "L%03d:\n", end_if_label);

        // execute the else body
        ex(p->opr.op[2]);

        // print the end_else label
        fprintf(quadrableFile, "L%03d:\n", end_else_label);
      }

      return NULL;
      break;
    }
    case SWITCH: {
      // get the variable name
      char *varName = p->opr.op[0]->id.id;
      getVariable(varName, &global_error, &resultNode);

      // check if variable exists
      if (!resultNode || global_error != "") {
        printf("ERROR: %s\n", global_error);
        yyerror(global_error);
        global_error = "";
        break;
      }

      // set the inside switch flag to true
      is_inside_switch = true;

      // get the labels & register
      int start_switch_label = global_curr_label++;
      int end_switch_label = global_switch_lbl++;
      int switch_var_reg = global_curr_reg++;

      struct conNodeType *switch_body_statement = p->opr.op[1];
      struct conNodeType *switch_default_statement = p->opr.op[2];

      // print the start label
      fprintf(quadrableFile, "L%03d:  ", start_switch_label);

      // store the variable in a register
      fprintf(quadrableFile, "\tpush\t%s\n", varName);
      fprintf(quadrableFile, "\tpop\tR%03d\n", switch_var_reg);

      // execute the body
      ex(switch_body_statement);

      // execute the default
      ex(switch_default_statement);

      // print the end label for Switch (S000 , S001 , ...)
      fprintf(quadrableFile, "S%03d:\n", end_switch_label);

      // clear the inside switch flag
      is_inside_switch = false;
      break;
    }
    case CASE: {
      struct conNodeType *other_case_list = p->opr.op[0];
      struct conNodeType *case_value_to_compare = p->opr.op[1];
      struct conNodeType *case_body_statement = p->opr.op[2];

      int case_reg = global_curr_reg - 1;
      int end_case_label = global_case_lbl++;
      // int end_case_label = global_switch_lbl++;

      is_inside_switch = true;

      // execute the other case list , as we used left recursion
      ex(other_case_list);

      // push the register to the stack
      fprintf(quadrableFile, "\tpush\tR%03d\n", case_reg);

      // push the value to compare with (int , float , char , string)
      char *temp = get_case_value_to_compare_as_string(case_value_to_compare);
      fprintf(quadrableFile, "\tpush\t%s\n", temp);

      // compare the two values
      fprintf(quadrableFile, "\tcompEQ\n");

      // jumb to end_case if the condition is false
      fprintf(quadrableFile, "\tjz\tC%03d\n", end_case_label);

      // execute the body
      ex(case_body_statement);

      // jump to the end of the whole switch
      fprintf(quadrableFile, "\tjmp\tS%03d\n", global_switch_lbl - 1);

      // print the end label
      fprintf(quadrableFile, "C%03d:\n", end_case_label);

      is_inside_switch = false;

      break;
    }
    case DEFAULT: {
      is_inside_switch = true;

      struct conNodeType *default_body_statement = p->opr.op[0];

      // execute the body
      ex(default_body_statement);

      is_inside_switch = false;
      break;
    }
    case BREAK: {
      // if (is_inside_switch) {
      //   printf("inside switch\n");
      //   break;
      // }

      // if (!is_inside_loop && !is_inside_switch) {
      //   printf("ERROR: break statement must be inside a loop or a switch\n");
      //   yyerror("ERROR: break statement must be inside a loop or a switch");
      //   break;
      // }

      // if (is_inside_loop) {
      //   // print the jump to the end of the loop
      //   fprintf(quadrableFile, "\tjmp\tL%03d\n", global_curr_label - 1);
      // }

      // handled in the switch case
      // if (is_inside_switch) {
      //   print the jump to the end of the switch
      //   fprintf(quadrableFile, "\tjmp\tS%03d\n", global_switch_lbl - 1);
      // }

      break;
    }
    case RETURN: {
      // get the return expression
      struct conNodeType *return_expression = p->opr.op[0];

      // return the value
      return ex(return_expression);
    }
    case FUNCTION: {
      // get the function type
      struct conNodeType *temp = ex(p->opr.op[0]);
      conEnum function_type = temp->type;

      // get the function name
      char *funcName = p->opr.op[1]->id.id;

      // get the function param , body
      struct conNodeType *function_param = p->opr.op[2];
      struct conNodeType *function_body = p->opr.op[3];

      // insert the function in the symbol table
      insertNewVariable(funcName, function_type, *temp, false, true,
                        &global_error);

      // check if the function is already defined
      if (global_error != "") {
        printf("ERROR: %s\n", global_error);
        yyerror(global_error);
        global_error = "";
        break;
      }

      // print the function name
      fprintf(quadrableFile, "%s:\n", funcName);

      // append the function to the function linked list
      functionLinkedList *newFunction = malloc(sizeof(functionLinkedList));
      newFunction->name = funcName;
      newFunction->next = functionListHeader;
      functionListHeader = newFunction;

      // change the scope
      changeScope(1);

      // execute the function param
      ex(function_param);

      // execute the function body
      ex(function_body);

      // change the scope
      changeScope(-1);

      // print the end of the function
      fprintf(quadrableFile, "\tEND\t%s\n", funcName);

      break;
    }
    case VOIDFUNCTION: {
      // get the function type
      conEnum function_type = typeVoid;

      // get the function name
      char *funcName = p->opr.op[0]->id.id;

      // get the function param , body
      struct conNodeType *function_param = p->opr.op[1];
      struct conNodeType *function_body = p->opr.op[2];

      // insert the function in the symbol table
      struct conNodeType *temp = malloc(sizeof(struct conNodeType));
      insertNewVariable(funcName, function_type, *temp, false, true,
                        &global_error);

      // check if the function is already defined
      if (global_error != "") {
        printf("ERROR: %s\n", global_error);
        yyerror(global_error);
        global_error = "";
        break;
      }

      // print the function name
      fprintf(quadrableFile, "%s:\n", funcName);

      // append the function to the function linked list
      functionLinkedList *newFunction = malloc(sizeof(functionLinkedList));
      newFunction->name = funcName;
      newFunction->next = functionListHeader;
      functionListHeader = newFunction;

      // check if main
      bool is_main = strcmp(funcName, "main") == 0;

      // change the scope
      if (!is_main) {
        changeScope(1);
      }

      // execute the function param
      ex(function_param);

      // execute the function body
      ex(function_body);

      // change the scope
      if (!is_main) {
        changeScope(-1);
      }

      // print the end of the function
      if (is_main) {
        fprintf(quadrableFile, "\tHLT\n");
      } else {
        fprintf(quadrableFile, "\tEND\t%s\n", funcName);
      }

      break;
    }
    case STATMENT_SEPARATOR: {
      struct conNodeType *first_statement = p->opr.op[0];
      struct conNodeType *second_statement = p->opr.op[1];

      // execute the first statement
      ex(first_statement);

      // execute the second statement
      return ex(second_statement);

      break;
    }
    case NEW_SCOPE: {
      printf("inside new scope\n");

      // change the scope
      changeScope(1);

      // execute the body
      struct conNodeType *statement_body = p->opr.op[0];
      ex(statement_body);

      // change the scope
      changeScope(-1);

      return NULL;
      break;
    }
    case FUNCTION_CALL: {
      // get the function name
      char *funcName = p->opr.op[0]->id.id;

      // check if the function is defined
      functionLinkedList *func_iterator = functionListHeader;
      bool is_defined = false;
      while (func_iterator != NULL) {
        is_defined = strcmp(func_iterator->name, funcName) == 0;
        if (is_defined) break;

        func_iterator = func_iterator->next;
      }

      // check if the function is defined
      if (!is_defined) {
        printf("ERROR: function %s is not defined\n", funcName);
        yyerror("function is not defined");
        break;
      }

      // get the function param
      struct conNodeType *function_param = p->opr.op[1];

      // execute the function param
      struct conNodeType *param_result = ex(function_param);

      // print the call to the function
      fprintf(quadrableFile, "\tCALL\t%s\n", funcName);

      return param_result;
      break;
    }
    case SINGLE_PARAM_DECLARATION: {
      // get the param name
      struct conNodeType *temp_param_type = ex(p->opr.op[0]);
      conEnum param_type = temp_param_type->type;

      printf("param type: %d\n", param_type);

      // get the param name
      char *param_name = p->opr.op[1]->id.id;

      // insert the param in the symbol table
      insertNewVariable(param_name, param_type, *temp_param_type, false, true,
                        &global_error);

      // check if error occured
      if (global_error != "") {
        printf("ERROR: %s\n", global_error);
        yyerror(global_error);
        global_error = "";
        break;
      }

      // print the param name
      fprintf(quadrableFile, "\tPOP\t%s\n", param_name);

      return temp_param_type;

      break;
    }
    case MULTI_PARAM_DECLARATION: {
      // get the param type
      struct conNodeType *temp_param_type = ex(p->opr.op[0]);
      conEnum param_type = temp_param_type->type;

      // get the param name
      char *param_name = p->opr.op[1]->id.id;

      // insert the param in the symbol table
      insertNewVariable(param_name, param_type, *temp_param_type, false, true,
                        &global_error);

      // check if error occured
      if (global_error != "") {
        printf("ERROR: %s\n", global_error);
        yyerror(global_error);
        global_error = "";
        break;
      }

      // print the param name
      fprintf(quadrableFile, "\tPOP\t%s\n", param_name);

      // get the second argument
      struct conNodeType *second_statement = p->opr.op[2];

      // execute the second statement
      return ex(second_statement);

      break;
    }
    case SINGLE_PARAM_CALL: {
      // execute the expression
      return ex(p->opr.op[0]);

      break;
    }
    case MULTI_PARAM_CALL: {
      // if more than 2 params it's handled by the left recursion
      nodeType *first_param = p->opr.op[0];
      nodeType *second_param = p->opr.op[1];

      // execute the first param
      ex(first_param);

      // execute the second param
      return ex(second_param);

      break;
    }

    default:
      break;
  }

  return resultNode;
}

/**
 * @brief Function that handles most of nodes & their operations
 *
 * @param p the node to be handled
 * @return struct conNodeType* the result of the operation
 */
struct conNodeType *ex(nodeType *p) {
  if (!p) {
    return NULL;
  }

  struct conNodeType *resultNode = malloc(sizeof(struct conNodeType *));
  nodeEnum currentNodeType = p->type;
  switch (currentNodeType) {
    case typeCon:
      return handle_const_node(p);
      break;
    case typeDef:
      return handle_def_node(p);
      break;
    case typeId:
      handle_identifier_node(p, &resultNode);
      return resultNode;
      break;
    case typeOpr:

      return handle_operation_node(p);
      break;
    default:
      printf("ERROR: unknown node type\n");
      break;
  }
  // in case of no node type matched
  return NULL;
}

/**
 * @brief Entry point of the compiler that executes the program
 *
 * @param p the root node of the AST
 */
void execute(nodeType *p) {
  // if(p->type == typeOpr ){
  //   // printf("typeOpr\n");
  //   // printf("opr: %d\n", p->opr.oper);
  // }else if(p->type == typeCon){
  //   // printf("typeCon\n");
  // }else if(p->type == typeId){
  //   // printf("typeId\n");
  // }else if(p->type == typeDef){
  //   // printf("typeDef\n");
  // }

  if (!p) {
    // printf("ERROR: empty node\n");
    return;
  }

  printf("entering ex \n-------------------\n");
  ex(p);
}
