//
// Created by qzhang on 3/10/21.
//

#ifndef LLVM_CLANG_SAMPLES_MACRO_H
#define LLVM_CLANG_SAMPLES_MACRO_H

/*elementrary edits*/
enum {
  INSERT;
  MOVE;
  MAKE_STATIC;
  UPDATE;
  DELETE;
  RESIZE;
  TYPECHANGE;
  CONFIG;
};

/*Six kinds of common errors*/
enum {
  /*01*/ DYNAMIC;
  /*02*/ DATAFLOW;
  /*03*/ TYPE;
  /*04*/ LOOP;
  /*05*/ TOP;
  /*06*/ STRUCT;
};

#endif // LLVM_CLANG_SAMPLES_MACRO_H

