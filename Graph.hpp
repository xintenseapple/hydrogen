/**
 * @author Ashwin K J
 * @file
 * Graph Class: Graph Data-structure
 */
#ifndef GRAPH_H
#define GRAPH_H

/* #include "Graph_Function.hpp" */
#include <fstream>
#include <iostream>
#include <list>
#include <llvm/IR/CallSite.h>
#include <llvm/IR/DebugInfoMetadata.h>
#include <llvm/IR/InstrTypes.h>
#include <llvm/IR/Intrinsics.h>
#include <regex>
namespace hydrogen_framework {
/* Forward declaration */
class Graph_Edge;
class Graph_Function;
class Graph_Instruction;
class Graph_Line;

/**
 * Graph Class: Class for generating Graphs
 */
class Graph {
public:
  /**
   * Constructor
   * Initialize ID to zero
   */
  explicit Graph(unsigned ver)
      : graphID(0), graphVersion(ver), graphEntryID(std::numeric_limits<unsigned int>::max() - 1),
        graphExitID(std::numeric_limits<unsigned int>::max() - 2) {
    whiteList.emplace_back("__isoc99_scanf");
    whiteList.emplace_back("printf");
    whiteList.emplace_back("malloc");
    whiteList.emplace_back("strlen");
    whiteList.emplace_back("strcpy");
    whiteList.emplace_back("strcmp");
    whiteList.emplace_back("free");
    whiteList.emplace_back("getpwnam");
    whiteList.emplace_back("__ctype_b_loc");
    whiteList.emplace_back("tolower");
    whiteList.emplace_back("setpwent");
    whiteList.emplace_back("getpwent");
    whiteList.emplace_back("strchr");
    whiteList.emplace_back("strcasecmp");
    whiteList.emplace_back("perror");
    whiteList.emplace_back("toupper");
    whiteList.emplace_back("malloc");
    whiteList.emplace_back("strlen");
    whiteList.emplace_back("strcpy");
    whiteList.emplace_back("strcmp");
    whiteList.emplace_back("free");
    whiteList.emplace_back("getpwnam");
    whiteList.emplace_back("__ctype_b_loc");
    whiteList.emplace_back("tolower");
    whiteList.emplace_back("setpwent");
    whiteList.emplace_back("getpwent");
    whiteList.emplace_back("strchr");
    whiteList.emplace_back("strcasecmp");
    whiteList.emplace_back("perror");
    whiteList.emplace_back("snprintf");
    whiteList.emplace_back("toupper");
  }

  /**
   * Destructor
   */
  ~Graph() {
    graphEdges.clear();
    graphFunctions.clear();
  }

  /**
   * Get next ID
   */
  unsigned getNextID() { return ++graphID; }

  /**
   * Return graphVersion
   */
  [[nodiscard]] unsigned getGraphVersion() const { return graphVersion; }

  /**
   * Set graphVersion
   */
  void setGraphVersion(unsigned ver) { graphVersion = ver; }

  /**
   * Push Graph_Edge into graphEdges
   */
  void pushGraphEdges(Graph_Edge *edge) { graphEdges.push_back(edge); }

  /**
   * Push Graph_Function into graphFunctions
   */
  void pushGraphFunction(Graph_Function *func);

  /**
   * Add sequential edges for the instructions in a Graph_Line
   */
  void addSeqEdges(Graph_Line *line);

  /**
   * Add branch type edges for ICFG
   */
  void addBranchEdges();

  /**
   * Add virtual nodes and corresponding edges to the Graph_Function
   */
  void addVirtualNodes(Graph_Function *func);

  /**
   * Add function call edges
   * Call only after addVirtualNodes
   */
  void addFunctionCallEdges();

  /**
   * Function to add Graph_Edge to both graphEdges and corresponding Graph_Instruction
   */
  void addEdge(Graph_Instruction *from, Graph_Instruction *to, Graph_Edge *edge);

  /**
   * Print the graph in DOT format
   */
  void printGraph(const std::string& graphName);

  /**
   * Find matching instruction in the ICFG
   * Can return NULL if no match is found
   */
  Graph_Instruction *findMatchedInstruction(llvm::Instruction *matchInst);

  /**
   * Find virtual entry for the given function name
   * Can return NULL if no match is found
   */
  Graph_Instruction *findVirtualEntry(const std::string& funcName);

  /**
   * Find virtual entry for the given function name
   * Can return NULL if no match is found
   */
  Graph_Instruction *findVirtualExit(const std::string& funcName);

  /**
   * Return graphFunctions
   */
  std::list<Graph_Function *> getGraphFunctions() { return graphFunctions; }

  /**
   * Return TRUE if it is a virtual node
   */
  [[nodiscard]] bool isVirtualNodeLineNumber(unsigned lineNumber) const;

  /**
   * Return graphEdges
   */
  std::list<Graph_Edge *> getGraphEdges() { return graphEdges; }

  /**
   * Return whiteList
   */
  std::list<std::string> getWhiteList() { return whiteList; }

private:
  unsigned graphID;                           /**< Unique Graph ID */
  unsigned graphVersion;                      /**< Version of graph. */
  unsigned graphEntryID;                      /**< ID for all virtual entry Node. Set to max -1 */
  unsigned graphExitID;                       /**< ID for all virtual exit Node. Set to max -2 */
  std::list<Graph_Edge *> graphEdges;         /**< Container for Edges in the graph */
  std::list<Graph_Function *> graphFunctions; /**< Container for function containers */
  std::list<std::string> whiteList;           /**< Container for white-listed functions */
};                                            // End Graph Class

/**
 * Find the line number and file name of the given LLVM instruction
 * Will return 0 if no information found
 */
void getLocationInfo(llvm::Instruction &I, unsigned int &DILocLine, std::string &DIFile);
} // namespace hydrogen_framework
#endif
