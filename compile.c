#include "compile.h"

char *quadrableFilePath = NULL;
FILE *quadrableFile = NULL;

typedef struct functionLinkedList
{
  char *name;
  struct functionLinkedList *next;
} functionLinkedList;

functionLinkedList *functionListHeader = NULL;

static int lbl;
static int switch_lbl;
static int case_lbl;
static int reg;

char *var;               // identifier name
char *global_error = ""; // global variable to store any error message

/**
 * @brief Set the Quadrable File Path object & open the file
 * @param filePath the path of the file
 */
// void setQuadrableFilePath(char *filePath)
void setQuadrableFilePath(char * filePath , FILE *file)
{
  quadrableFilePath = filePath;
  quadrableFile = file;
}

/**
 * @brief Clear the Quadrable File Path object & close the file
 *
 */
void clearQuadrableFilePath()
{
  quadrableFilePath = NULL;
  quadrableFile = NULL;
}

/**
 * @brief Function that handles the constant nodes whether , integer , float , bool , char and string
 *
 * @param p the node to be handled
 * @return struct conNodeType*
 */
struct conNodeType *handle_const_node(nodeType *p)
{
  struct conNodeType *resultNode = malloc(sizeof(struct conNodeType *));

  conEnum currentConType = p->con.type;
  char *string_to_write = malloc(sizeof(char) * 100);

  switch (currentConType)
  {
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
struct conNodeType *handle_def_node(nodeType *p)
{
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
struct conNodeType *handle_identifier_node(nodeType *p)
{
  char *identifierName = p->id.id;
  struct conNodeType *resultNode = getVariable(identifierName, &global_error);

  fprintf(quadrableFile, "\tpush\t%s\n", identifierName); // push the identifier name to the stack
  bool hasError = strcmp(global_error, "") != 0 || resultNode == NULL;
  if (hasError)
  {
    printf("ERROR: %s\n", global_error);
    yyerror(global_error);
    global_error = "";
  }

  return resultNode;
}

bool is_binary_operation(int operation)
{
  int operationsArr[] = {
      ADD,
      SUB,
      MUL,
      DIV,
      MOD,
      LT,
      LTE,
      GT,
      GTE,
      EQUAL_TO,
      NOT_EQUAL,
      AND,
      OR};

  int arrSize = sizeof(operationsArr) / sizeof(int);
  for (int i = 0; i < arrSize; i++)
  {
    if (operation == operationsArr[i])
      return true;
  }

  return false;
}

char *get_operation_string(int operand)
{
  switch (operand)
  {
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

struct conNodeType *get_operand_value(nodeType *p)
{
  switch (p->type)
  {
  case typeCon:
    printf("const node\n");
    return handle_const_node(p);
    break;
  case typeId:
    return handle_identifier_node(p);
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
struct conNodeType *handle_operation_node(nodeType *p)
{
  struct conNodeType *resultNode = malloc(sizeof(struct conNodeType *));
  int currOperation = p->opr.oper;
  
  
  if (is_binary_operation(currOperation))
  {
    struct conNodeType *operand1 = get_operand_value(p->opr.op[0]);
    struct conNodeType *operand2 = get_operand_value(p->opr.op[1]);
    handle_operations(operand1, operand2, currOperation , resultNode);

    fprintf(quadrableFile, "%s", get_operation_string(currOperation));
    return resultNode;
  }

  bool isUnaryOperation = currOperation == UMINUS || currOperation == NOT;
  if (isUnaryOperation)
  {
    struct conNodeType *operand1 = get_operand_value(p->opr.op[0]);
    resultNode = handle_unary_operations(operand1, currOperation);

    fprintf(quadrableFile, "%s", get_operation_string(currOperation));
    return resultNode;
  }

  switch (currOperation)
  {
  case WHILE:
    break;

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
struct conNodeType *ex(nodeType *p)
{
  if (!p)
  {
    printf("ERROR: empty node\n");
    return NULL;
  }

  nodeEnum currentNodeType = p->type;
  switch (currentNodeType)
  {
  case typeCon:
    return handle_const_node(p);
    break;
  case typeDef:
    return handle_def_node(p);
    break;
  case typeId:
    return handle_identifier_node(p);
    break;
  case typeOpr:

    printf("opr in ex %d\n", p->opr.oper);
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
void execute(nodeType *p)
{
  if(p->type == typeOpr ){
    printf("typeOpr\n");
    printf("opr: %d\n", p->opr.oper);
  }else if(p->type == typeCon){
    printf("typeCon\n");
  }else if(p->type == typeId){
    printf("typeId\n");
  }else if(p->type == typeDef){
    printf("typeDef\n");
  }

  if(!p){
    printf("ERROR: empty node\n");
    return;
  }

  printf("entering ex \n-------------------\n");
  ex(p);
}
