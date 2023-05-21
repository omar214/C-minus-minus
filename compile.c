#include "compile.h"

char *quadrableFilePath = NULL;
FILE *quadrableFile = NULL;

typedef struct functionLinkedList {
  char *name;
  struct functionLinkedList *next;
} functionLinkedList;

functionLinkedList *functionListHeader = NULL;

static int lbl;
static int switch_lbl;
static int case_lbl;
static int reg;

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
struct conNodeType *handle_identifier_node(nodeType *p, struct conNodeType **resultNode) {
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
      break;
    }
    case DO: {
      break;
    }
    case FOR: {
      break;
    }
    case IF: {
      break;
    }
    case SWITCH: {
      break;
    }
    case CASE: {
      break;
    }
    case DEFAULT: {
      break;
    }
    case BREAK: {
      break;
    }
    case FUNCTION: {
      break;
    }
    case VOIDFUNCTION: {
      break;
    }

    case STATMENT_SEPARATOR: {
      break;
    }
    case NEW_SCOPE: {
      break;
    }
      // TODO: handle all below
    case 't': {
      break;
    }
    case 'r': {
      break;
    }
    case 'q': {
      break;
    }
    case ':': {
      break;
    }

    default:
      break;
  }
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
