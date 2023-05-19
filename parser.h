#pragma once

typedef enum { 
    typeCon,  // const 
    typeId,   // identifier
    typeOpr,  // operation
    typeDef   // data type
} nodeEnum;

typedef enum { 
    typeInt, 
    typeFloat, 
    typeString, 
    typeChar, 
    typeBool, 
    typeConst, 
    typeND,     // not defined
    typeVoid    // void
} conEnum; 

/* constants */

struct conNodeType{
    conEnum type;        // type of the constant value ( used to assign value for the identifiers)

    /* constant values */
    union 
    {
        int iValue; 
        float fValue;
        char* sValue;
        char cValue;
    };
    
};

/* identifiers */
struct idNodeType{
    char* id;   // pointer to the identifiers table
};


/* operators */
struct oprNodeType {
    int oper;                 // operator 
    int nops;                 // num of operands
    struct nodeTypeTag *op[1];// operands, extended at runtime
};

/* types */
struct typeNodeType {
    conEnum type;                     
} ;


typedef struct nodeTypeTag {
    nodeEnum type;              /* type of node */

    union {
        struct conNodeType con;        /* constants */
        struct idNodeType id;          /* identifiers */
        struct oprNodeType opr;        /* operators */
        struct typeNodeType typ;       /* types */
    };
} nodeType;
