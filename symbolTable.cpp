// built in headers
#include <string.h>

#include <fstream>
#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>
using namespace std;

// user defined headers
#include "parser.h"
#include "symbolTable.h"

// type definitions
typedef pair<bool, bool> PAIR_ISCONST_ISINITIALIZED;
typedef pair<conNodeType, PAIR_ISCONST_ISINITIALIZED> PAIR_TYPE_VALUE;

struct symbol_table {
  unordered_map<string, PAIR_TYPE_VALUE> symtable;
  symbol_table *parent = NULL;
};

// global variables
string symbol_table_path = "";
ofstream symbol_table_file;
symbol_table curr_global_table;

/**
 * @brief Set the Table Path object & opens the file to write to it
 * @param path the path of the symbol table file
 */
void setTablePath(char *path) {
  symbol_table_path = string(path);
  symbol_table_file.open(symbol_table_path);
}

/**
 * @brief Clear the Table Path object & closes the file
 *
 */
void clearTablePath() {
  symbol_table_path = "";
  symbol_table_file.close();
}

/**
 * @brief print the symbol table
 */
void printSymbolTable(bool is_print_nest = false) {
  symbol_table *table_iterator = &curr_global_table;

  is_print_nest = is_print_nest || table_iterator->parent != NULL;

  if (!is_print_nest) {
    while (table_iterator->parent != NULL) {
      table_iterator = table_iterator->parent;
    }
  }

  if (is_print_nest) {
    cout << "\t\tNested Symbol Table" << endl << "\t\t";
    symbol_table_file << "\t\tNested Symbol Table" << endl << "\t\t";
  }
  cout << "------------------- Start SYMBOL TABLE ------------------" << endl;
  symbol_table_file
      << "------------------ Start SYMBOL TABLE -------------------" << endl;

  if (!table_iterator) return;
  for (auto &it : table_iterator->symtable) {
    string var_name = it.first;
    conEnum var_type = it.second.first.type;
    string is_const = it.second.second.first ? "true" : "false";
    // string is_const       = it.second.second.first  ? "Yes ✔" : "No ✖";
    string is_initialized = it.second.second.second ? "true" : "false";
    string var_type_str = "";
    string var_value_str = "";

    switch (var_type) {
      case typeInt:
        var_type_str = "Integer";
        var_value_str = to_string(it.second.first.iValue);
        break;

      case typeFloat:
        var_type_str = "Float";
        var_value_str = to_string(it.second.first.fValue);
        break;

      case typeBool:
        var_type_str = "Bool";
        var_value_str = to_string(it.second.first.iValue);
        break;

      case typeChar:
        var_type_str = "Char";
        var_value_str = (it.second.first.cValue);
        var_value_str = "'" + var_value_str + "'";
        break;

      case typeString:
        var_type_str = "String";
        var_value_str = '"' + string(it.second.first.sValue) + '"';
        break;

      case typeVoid:
        var_type_str = "Void";
        break;
      default:
        break;
    }
    if (var_name == "main") {
      // since function are saved in the symbol table also && we need to
      // neglect the main
      continue;
    }

    if (is_print_nest) {
      cout << "\t\t";
      symbol_table_file << "\t\t";
    }
    cout << "variable_name:\t" << var_name << "\tValue : " << var_value_str
         << " | type:\t" << var_type_str << " | is_constant:\t" << is_const
         << " | is_initialized\t" << is_initialized << " \n";

    symbol_table_file << "variable Name:\t" << var_name
                      << "\tValue : " << var_value_str << "\ttype:\t"
                      << var_type_str << "\tis_constant:\t" << is_const
                      << "\tis_initialized\t" << is_initialized << " \n";
  }

  if (is_print_nest) {
    cout << "\t\t";
    symbol_table_file << "\t\t";
  }
  symbol_table_file
      << "------------------ END   SYMBOL TABLE -------------------\n"
      << endl;
  cout << "------------------ END   SYMBOL TABLE -------------------\n" << endl;
}

/**
 * @brief function used to move the scope of the symbol table
 * to handle the nested scopes & functions
 * @param scope_dir integer value to indicates whether we will go up or down in
 * the scope
 */
void changeScope(scopeEnum scope_dir) {
  if (scope_dir == up) {
    // go up in the scope
    symbol_table *prev_table = new symbol_table{curr_global_table};

    symbol_table *new_table = new symbol_table;
    new_table->parent = prev_table;
    curr_global_table = *new_table;
  } else {
    // go down in the scope
    symbol_table *parent = curr_global_table.parent;
    curr_global_table = *parent;
  }
}

/**
 * @brief insert a new variable to the symbol table
 *
 * @param var_name the name of the variable
 * @param var_type the type of the variable
 * @param var_value    the value of the variable
 * @param is_const whether the variable is constant or not
 * @param has_value whether the variable is initialized or not
 * @param error the error message if there is an error
 * @return struct conNodeType* the created node for the variable
 */
struct conNodeType *insertNewVariable(char *var_name, conEnum var_type,
                                      struct conNodeType var_value,
                                      bool is_const, bool has_value,
                                      char **error) {
  bool isVoid = (var_value.type == typeVoid);
  if (isVoid) {
    *error = (char *)malloc(sizeof(char) * 100);
    strcpy(*error, "Error: void type is not allowed");
    return NULL;
  }

  symbol_table *table_iterator = &curr_global_table;
  while (table_iterator != NULL) {
    bool is_found = table_iterator->symtable.find(var_name) !=
                    table_iterator->symtable.end();
    if (is_found) {
      bool is_const = table_iterator->symtable[var_name].second.first;
      if (is_const) {
        *error = (char *)malloc(sizeof(char) * 100);
        strcpy(*error, "Error: constant variables can't be updated");
        return NULL;
      }

      bool is_type_missmatch =
          table_iterator->symtable[var_name].first.type != var_value.type;
      if (is_type_missmatch) {
        *error = (char *)malloc(sizeof(char) * 100);
        strcpy(*error, "Error: type missmatch for the variable");
        return NULL;
      }

      // we use notDefined to indicate updating
      bool is_redeclared = (var_type != typeNotDefined);
      if (is_redeclared) {
        *error = (char *)malloc(sizeof(char) * 100);
        strcpy(*error,
               "Error: re-declaration of the variable in the same scope");
        return NULL;
      }

      // update the value of the variable
      table_iterator->symtable[var_name].first =
          var_value;  // the value of the variable
      table_iterator->symtable[var_name].second =
          make_pair(false, true);  // is_const= false, has_value = true

      conNodeType *variableNode = &(table_iterator->symtable[var_name].first);
      printSymbolTable(false);  // false to not print nested scopes
      return variableNode;
    }

    // check if nested scope & new declaration (adding var to this nested scope)
    bool is_nested_scope = (table_iterator->parent != NULL);
    bool is_new_declare_in_scope = (var_type != typeNotDefined);

    // inserting new var to this scope , so don't need to go up
    if (is_nested_scope && is_new_declare_in_scope) {
      printf("\nnew declaration in nested scope\n");
      break;
    }
    table_iterator = table_iterator->parent;
  }

  bool is_not_declared = (var_type == typeNotDefined);
  if (is_not_declared) {
    *error = (char *)malloc(sizeof(char) * 100);
    strcpy(*error,
           "Error: variable is not declared in the current scope , can't "
           "update it");
    return NULL;
  }

  bool is_type_missmatch =
      (has_value && var_type != var_value.type && var_type != typeVoid);
  if (is_type_missmatch) {
    *error = (char *)malloc(sizeof(char) * 100);
    strcpy(*error, "Error: type missmatch for the variable");
    return NULL;
  }

  // insert the variable to the symbol table
  PAIR_ISCONST_ISINITIALIZED isconst_isinit = make_pair(is_const, has_value);
  PAIR_TYPE_VALUE type_value = make_pair(var_value, isconst_isinit);
  curr_global_table.symtable.insert({var_name, type_value});

  conNodeType *variableNode = &(curr_global_table.symtable[var_name].first);
  printSymbolTable(false);  // false to not print nested scopes
  return variableNode;
}

/**
 * @brief get the value of a variable from the symbol table
 *
 * @param var_name the name of the variable
 * @param error    the error message if there is an error
 * @return struct conNodeType* the node of the variable
 */
struct conNodeType *getVariable(char *var_name, char **error,
                                struct conNodeType **resultNode) {
  // intialize next table pointer
  symbol_table *table_iterator = &curr_global_table;

  while (table_iterator != NULL) {
    // check if the variable is declared in the current scope
    bool is_found = table_iterator->symtable.find(var_name) !=
                    table_iterator->symtable.end();
    if (is_found) {
      conNodeType *variableNode = &(table_iterator->symtable[var_name].first);
      *resultNode = variableNode;
      bool has_value = table_iterator->symtable[var_name].second.second;
      if (!has_value) {
        *error = (char *)malloc(sizeof(char) * 100);
        strcpy(*error, "variable is not initialized");
        printf("variable %s is not initialized\n", var_name);
      }

      return variableNode;
    }
    table_iterator = table_iterator->parent;
  }
  *error = (char *)malloc(sizeof(char) * 100);
  strcpy(*error, "variable is not declared");
  return NULL;
}
