/**
 * @author Ashwin K J
 * @file
 */
#include "Diff_Mapping.hpp"
#include "Get_Input.hpp"
#include "Graph.hpp"
#include "Graph_Line.hpp"
#include "MVICFG.hpp"
#include "Graph_Function.hpp"
#include "Module.hpp"
#include <chrono>

using namespace hydrogen_framework;

/**
 * Main function
 */
int main(int argc, char *argv[]) {
  /* Getting the input */
  if (argc < 2) {
    std::cerr << "Insufficient arguments\n"
              << "The correct format is as follows:\n"
              << "<Path-to-Module1> <Path-to-Module2> .. <Path-to-ModuleN> :: "
              << "<Path-to-file1-for-Module1> .. <Path-to-fileN-for-Module1> :: "
              << "<Path-to-file2-for-Module2> .. <Path-to-fileN-for-Module2> ..\n"
              << "Note that '::' is the demarcation\n";
    return 1;
  } // End check for min argument
  Hydrogen framework;
  if (!framework.validateInputs(argc, argv)) {
    return 2;
  } // End check for valid Input
  if (!framework.processInputs(argc, argv)) {
    return 3;
  } // End check for processing Inputs
  std::list<Module *> mod = framework.getModules();
  /* Create ICFG */
  unsigned graphVersion = 1;
  Module *firstMod = mod.front();
  Graph *MVICFG = buildICFG(firstMod, graphVersion);
  /* Start timer */
  auto mvicfgStart = std::chrono::high_resolution_clock::now();
  /* Create MVICFG */
  for (auto iterModule = mod.begin(), iterModuleEnd = mod.end(); iterModule != iterModuleEnd; ++iterModule) {
    auto iterModuleNext = std::next(iterModule);
    /* Proceed as long as there is a next module */
    if (iterModuleNext != iterModuleEnd) {
      /* Container for added and deleted MVICFG lines */
      std::list<Graph_Line *> addedLines;
      std::list<Graph_Line *> deletedLines;
      std::map<Graph_Line *, Graph_Line *> matchedLines; /**<Map From ICFG Graph_Line to MVICFG Graph_Line */
      std::list<Diff_Mapping> diffMap = generateLineMapping(*iterModule, *iterModuleNext);
      Graph *ICFG = buildICFG(*iterModuleNext, ++graphVersion);
      for (const auto& iter : diffMap) {
        /* iter.printFileInfo(); */
        std::list<Graph_Line *> iterAdd = addToMVICFG(MVICFG, ICFG, iter, graphVersion);
        std::list<Graph_Line *> iterDel = deleteFromMVICFG(MVICFG, ICFG, iter, graphVersion);
        std::map<Graph_Line *, Graph_Line *> iterMatch = matchedInMVICFG(MVICFG, ICFG, iter, graphVersion);
        addedLines.insert(addedLines.end(), iterAdd.begin(), iterAdd.end());
        deletedLines.insert(deletedLines.end(), iterDel.begin(), iterDel.end());
        matchedLines.insert(iterMatch.begin(), iterMatch.end());
      } // End loop for diffMap
      std::cout << "Code Churn: " << addedLines.size() + deletedLines.size() << std::endl;
      /* Update Map Edges */
      getEdgesForAddedLines(MVICFG, ICFG, addedLines, diffMap);
      /* Update the matched lines to get new temporary variable mapping for old lines */
      updateMVICFGVersion(MVICFG, addedLines, deletedLines, diffMap, graphVersion);
      /* Update Map Version */
      MVICFG->setGraphVersion(graphVersion);
    } // End check for iterModuleEnd
  }   // End loop for Module

  /* Stop timer */
  auto mvicfgStop = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> mvicfgBuildTime = mvicfgStop - mvicfgStart;
  MVICFG->printGraph("MVICFG");
  std::cout << "Finished Building MVICFG in " << mvicfgBuildTime.count() << "ms" << std::endl;;

  unsigned int correct_version = MVICFG->getGraphVersion();
  for (unsigned int i = 1; i < correct_version; i++) {
    auto paths = calculateChangedPaths(MVICFG, i, correct_version);
    std::cout << "Changed Path Between V" << i << " and V" << correct_version << ":" << std::endl;
    std::cout << "\tAdded Paths: " << paths.first << std::endl;
    std::cout << "\tDeleted Paths: " << paths.second << std::endl;
  }

  unsigned long long graph_instructions = 0;
  for (auto graph_function : MVICFG->getGraphFunctions()) {
    for (auto graph_line : graph_function->getFunctionLines()) {
      graph_instructions += graph_line->getLineInstructions().size();
    }
  }

  std::cout << "Size of MVICFG (nodes, edges): " << graph_instructions << ", " << MVICFG->getGraphEdges().size() << std::endl;

  /* Write output to file */
  std::ofstream rFile("Result.txt", std::ios::trunc);
  if (!rFile.is_open()) {
    std::cerr << "Unable to open file for printing the output\n";
    return 5;
  } // End check for Result file
  rFile << "Input Args:\n";
  for (auto i = 0; i < argc; ++i) {
    rFile << argv[i] << "  ";
  } // End loop for writing arguments
  rFile << "\n";
  rFile << "Finished Building MVICFG in " << mvicfgBuildTime.count() << "ms\n";
  rFile.close();
  return 0;
} // End main
