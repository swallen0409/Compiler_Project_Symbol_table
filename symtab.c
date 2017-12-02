#include "symtab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct SymTable SymbolTable;

/*search the symbol table for the pattern*/
struct SymTableEntry* findSymbol(char *s) {
    for(int i=0; i<SymbolTable.size; i++) {
        if(strcmp(s, SymbolTable.entries[i].name) == 0) {
            return &SymbolTable.entries[i];
        }
    }

    return 0;
}
//add another child
struct SymTableEntry* addVariable(char *s, enum StdType type) {
    if(findSymbol(s) != 0) {
        printf("Error: duplicate declaration of variable %s\n", s);
        exit(0);
    }

    int index = SymbolTable.size;
    SymbolTable.size++;

    strcpy(SymbolTable.entries[index].name, s);
    SymbolTable.entries[index].type = type;
    
    return &SymbolTable.entries[index];
}
//find and return nth child in an node
struct node* nthChild(int n, struct node *node) {
    struct node *child = node->child;
    for(int i=1; i<n; i++) {
        child = child->rsibling;
    }
    return child;
}

void semanticCheck(struct node *node) {
    switch(node->nodeType) {
        case NODE_VAR_DECL: {
            /* We only implement integer and real type here,
               you should implement array type by yourself */
            struct node *typeNode = nthChild(2, node);
            enum StdType valueType;
            if(typeNode->nodeType == NODE_TYPE_INT)
                valueType = TypeInt;
            else if(typeNode->nodeType == NODE_TYPE_REAL)
                valueType = TypeReal;
            else
                valueType = TypeString;

            struct node *idList = nthChild(1, node);
            struct node *idNode = idList->child;
            do {
                addVariable(idNode->string, valueType);
                idNode = idNode->rsibling;
            } while(idNode != idList->child);

            return;
        }

        /* This case is simplified, actually you should check
           the symbol is a variable or a function with no parameter */
        case NODE_VAR_OR_PROC: 
        case NODE_SYM_REF: {
            struct SymTableEntry *entry = findSymbol(node->string);
            if(entry == 0) {
                printf("Error: undeclared variable %s\n", node->string);
                exit(0);
            }

            node->entry = entry;
            node->valueType = entry->type;

            return;
        }

        case NODE_INT: {
            node->valueType = TypeInt;
            return;
        }

        case NODE_REAL: {
            node->valueType = TypeReal;
            return;
        }

        /* Only implemented binary op here, you should implement unary op */
        case NODE_OP:

        /* You should check the LHS of assign stmt is assignable
           You should also report error if LHS is a function with no parameter 
           (function is not implemented in this sample, you should implement it) */ 
        case NODE_ASSIGN_STMT: {
            struct node *child1 = nthChild(1, node);
            struct node *child2 = nthChild(2, node);
            semanticCheck(child1);
            semanticCheck(child2);

            /* We only implement the checking for integer and real types
               you should implement the checking for array type by yourself */
            if(child1->valueType != child2->valueType) {
                if(node->nodeType == NODE_OP)
                    printf("Error: type mismatch for operator\n");
                else
                    printf("Error: type mismatch for assignment\n");
                exit(0);
            }

            node->valueType = child1->valueType;

            return;
        }
    }

    /* Default action for other nodes not listed in the switch-case */
    struct node *child = node->child;
    if(child != 0) {
        do {
            semanticCheck(child);
            child = child->rsibling;
        } while(child != node->child);
    }
}

