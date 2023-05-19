#pragma once

/**
 * @brief Node Type Enum
 * whether this node is a constant, identifier, operation or type definition
 */
typedef enum { 
    typeCon,  // const 
    typeId,   // identifier
    typeOpr,  // operation
    typeDef   // data type
} nodeEnum;


/**
 * @brief Constant Type Enum
 * used to identify the type of the constant con_node type
 * 
 */
typedef enum { 
    typeInt, 
    typeFloat, 
    typeString, 
    typeChar, 
    typeBool, 
    typeConst, 
    typeNotDefined,     // not defined
    typeVoid            // void
} conEnum;


/**
 * @brief the const node type
 * used to store values of expressions 
 * 
 */
struct conNodeType{
    // this type is used to identify which variable in the union is used
    conEnum type;        

    union 
    {
        int iValue;     // integer value
        float fValue;   // float value
        char* sValue;   // string value
        char cValue;    // char value
    };
};


/**
 * @brief Identifiers Node Type
 * id the name of the identifier  
 * 
 */
struct idNodeType{
    // the name of the identifier used to access the symbol table
    char* id;   
};


/**
 * @brief Operators Node Type
 * oper the operator
 * nops the number of operands
 * op the operands
 * 
 */
struct oprNodeType {
    int oper;                 // operator 
    int nops;                 // num of operands
    struct nodeTypeTag *op[1];// operands, extended at runtime
};


/**
 * @brief Types Node Type -> used in the variable declaration like
 * int x;   -> int is the type here
 * float y; -> float is the type here
 */
struct typeNodeType {
    conEnum type;                     
};


typedef struct nodeTypeTag {
    nodeEnum type;              /* type of node */

    union {
        struct conNodeType con;        /* constants */
        struct idNodeType id;          /* identifiers */
        struct oprNodeType opr;        /* operators */
        struct typeNodeType typ;       /* types */
    };
} nodeType;
