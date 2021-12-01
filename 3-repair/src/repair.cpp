//------------------------------------------------------------------------------
//author: Qian Zhang
//time: Jul 20, 2020
//------------------------------------------------------------------------------
#include <cstdio>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Basic/TargetOptions.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Parse/ParseAST.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Rewrite/Frontend/Rewriters.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;
using namespace std;

std::string funname;
static int flag = 1;
static int type_transformation=0;


// By implementing RecursiveASTVisitor, we can specify which AST nodes
// we're interested in by overriding relevant methods.
std::string decl2str(clang::Decl *d, SourceManager &sm) {
  // (T, U) => "T,,"
  string text = Lexer::getSourceText(CharSourceRange::getTokenRange(d->getSourceRange()), sm, LangOptions(), 0);
  if (text.size() > 0 && (text.at(text.size()-1) == ',')) //the text can be ""
    return Lexer::getSourceText(CharSourceRange::getCharRange(d->getSourceRange()), sm, LangOptions(), 0);
  return text;
}

class MyASTVisitor : public RecursiveASTVisitor<MyASTVisitor>
{
public:
    MyASTVisitor(Rewriter &R, SourceManager &S)
            : TheRewriter(R), sourceManager(S)
    {}
    int count = 0;
    bool VisitStmt(Stmt *s) {
        // If statements.
        if (isa<IfStmt>(s)) {
            IfStmt *IfStatement = cast<IfStmt>(s);
            Stmt *Then = IfStatement->getThen();

            TheRewriter.InsertText(Then->getLocStart(),
                    //TheRewriter.InsertText(Then->getLocStart(),
                                   "// the 'if' part\n",
                                   true, true);

            Stmt *Else = IfStatement->getElse();
            if (Else)
                TheRewriter.InsertText(Else->getLocStart(),
                                       "// the 'else' part\n",
                                       true, true);
            printf("Now I will try to remove this part.\n\n");
            TheRewriter.RemoveText(s->getSourceRange());
        }
        return true;
    }

    bool VisitFunctionDecl(FunctionDecl *f) {
        // Only function definitions (with bodies), not declarations.
      // Might return until the beginning of the last token though


        if (f->hasBody()) {
            Stmt *FuncBody = f->getBody();
            // Type name as string
            //QualType QT = f->getResultType();
            QualType QT = f->getReturnType();
            string TypeStr = QT.getAsString();

            // Function name
            DeclarationName DeclName = f->getNameInfo().getName();
            string FuncName = DeclName.getAsString();
            std::cout<<"Visiting Function: "<<FuncName<<std::endl;
            SourceLocation ST = f->getSourceRange().getBegin();
            // Add comment before
            /*stringstream SSBefore;
            SSBefore << "// Begin function " << FuncName << " returning "
                     << TypeStr << "\n";
            SourceLocation ST = f->getSourceRange().getBegin();
            TheRewriter.InsertText(ST, SSBefore.str(), true, true);

            // And after
            stringstream SSAfter;
            SSAfter << "\n// End function " << FuncName << "\n";
            ST = FuncBody->getLocEnd().getLocWithOffset(1);
            TheRewriter.InsertText(ST, SSAfter.str(), true, true);*/

            SourceRange exprRange = f->getSourceRange();
            string exprString;

            int rangeSize = TheRewriter.getRangeSize(exprRange);

            SourceLocation startLoc = exprRange.getBegin();
            const char *strStart = sourceManager.getCharacterData(startLoc);

            exprString.assign(strStart, rangeSize);
            // int out the function body
            //std::cout<<"1111"<<std::endl;
            //std::cout<<exprString<<std::endl;
            int pos0 = funname.find("::");
            std::string tempfunname = funname;
            if(pos0>-1) {
              tempfunname = funname.substr(pos0+2);
            }
            //std::cout<<" you!"<<tempfunname<<std::endl;
            int pos1 = exprString.find(tempfunname);
            int pos2 = exprString.find_first_of(";", pos1);
            //std::cout<<pos1<<" "<<pos2<<std::endl;

            TheRewriter.RemoveText(f->getSourceRange());

            std::string ss = exprString.substr(0, pos1)+"static If2 a(in, out);\n" + "a.do1();"+exprString.substr(pos2+1)+"\n";
            TheRewriter.InsertText(ST, ss, true, true);
        }

        return true;
    }
    bool VisitNamedDecl(clang::NamedDecl *NamedDecl) {
      std::string FuncName = NamedDecl->getQualifiedNameAsString();
      std::cout << "Visiting NameDecl " << FuncName <<std::endl;
      if(FuncName==funname) {
        std::cout<<"Delete function "<<funname<<std::endl;

        // And after
        //stringstream SSAfter;
        //Stmt *FuncBody = NamedDecl->getBody();
        //SSAfter << "\n// End function " << FuncName << "\n";
        //ST = FuncBody->getLocEnd().getLocWithOffset(1);
        //TheRewriter.InsertText(ST, SSAfter.str(), true, true);
        //TheRewriter.RemoveText(NamedDecl->getSourceRange().getBegin(), 50);
      }
      return true;
    }

  bool VisitDecl(clang::Decl *d) {
    std::cout<<"VisitDecl: "<<d->getDeclKindName()<<std::endl;
    //std::cout<<"VisitDecl" ""<<decl2str(f, sourceManager)<<std::endl;
    if (isa<RecordDecl>(d)) {
        std::string f = decl2str(d, sourceManager);
        int pos0 = funname.find("::");
        std::string tempfunname = funname;
        if(pos0>-1) {
          tempfunname = funname.substr(pos0+2);
        }
        int pos1 = f.find_first_of("(")-tempfunname.size();
        int pos2 = f.find_first_of("}");
        length_ = pos2 - pos1+1;
    }
    if(isa<FieldDecl>(d)) {
      NamedDecl *nd  = cast<NamedDecl>(d);
      std::string FuncName = nd->getQualifiedNameAsString();
      std::cout << "Visiting FieldDecl " << FuncName <<std::endl;
      if(FuncName==funname) {
        std::cout<<"Delete "<<length_<<std::endl;
        TheRewriter.RemoveText(d->getSourceRange().getBegin(), length_);
      }
    }
    return true;
  }
private:
    void AddBraces(Stmt *s);

    Rewriter &TheRewriter;
    SourceManager &sourceManager;
    int length_ = 0;
};


// Implementation of the ASTConsumer interface for reading an AST produced
// by the Clang parser.
class MyASTConsumer : public ASTConsumer
{
public:
    MyASTConsumer(Rewriter &R, SourceManager &S)
            : Visitor(R, S)
    {}

    // Override the method that gets called for each parsed top-level
    // declaration.
    virtual bool HandleTopLevelDecl(DeclGroupRef DR) {
        for (DeclGroupRef::iterator b = DR.begin(), e = DR.end();
             b != e; ++b)
            // Traverse the declaration using our AST visitor.
            Visitor.TraverseDecl(*b);
        return true;
    }

private:
    MyASTVisitor Visitor;
};


class MyASTVisitor_var : public RecursiveASTVisitor<MyASTVisitor>
{
public:
  MyASTVisitor_var(Rewriter &R, SourceManager &S)
      : TheRewriter(R), sourceManager(S)
  {}
  int count = 0;
  bool VisitStmt(Stmt *s) {
    // If statements.
    if (isa<IfStmt>(s)) {
      IfStmt *IfStatement = cast<IfStmt>(s);
      Stmt *Then = IfStatement->getThen();

      TheRewriter.InsertText(Then->getLocStart(),
          //TheRewriter.InsertText(Then->getLocStart(),
                             "// the 'if' part\n",
                             true, true);

      Stmt *Else = IfStatement->getElse();
      if (Else)
        TheRewriter.InsertText(Else->getLocStart(),
                               "// the 'else' part\n",
                               true, true);
      TheRewriter.RemoveText(s->getSourceRange());
    }
    return true;
  }

  bool VisitFunctionDecl(FunctionDecl *f) {

    // Only function definitions (with bodies), not declarations.
    if (f->hasBody()) {
      Stmt *FuncBody = f->getBody();
      // Type name as string
      //QualType QT = f->getResultType();
      QualType QT = f->getReturnType();
      string TypeStr = QT.getAsString();

      // Function name
      DeclarationName DeclName = f->getNameInfo().getName();
      string FuncName = DeclName.getAsString();
      std::cout<<"Visiting Function: "<<FuncName<<std::endl;

      // Add comment before
      stringstream SSBefore;
      SSBefore << "// Begin function " << FuncName << " returning "
               << TypeStr << "\n";
      SourceLocation ST = f->getSourceRange().getBegin();
      TheRewriter.InsertText(ST, SSBefore.str(), true, true);

      // And after
      stringstream SSAfter;
      SSAfter << "\n// End function " << FuncName << "\n";
      ST = FuncBody->getLocEnd().getLocWithOffset(1);
      TheRewriter.InsertText(ST, SSAfter.str(), true, true);
    }

    return true;
  }
  bool VisitNamedDecl(clang::NamedDecl *NamedDecl) {
    std::string FuncName = NamedDecl->getQualifiedNameAsString();

    std::cout << "Visiting NameDecl " << FuncName <<std::endl;
    //if (NamedDecl->hasBody()) {
      std::string f = decl2str(NamedDecl, sourceManager);
      std::cout<<"hahahaha "<<f<<std::endl;
    //}
//    if(FuncName==funname) {
//      std::cout<<"Delete function "<<funname<<std::endl;

      // And after
      //stringstream SSAfter;
      //Stmt *FuncBody = NamedDecl->getBody();
      //SSAfter << "\n// End function " << FuncName << "\n";
      //ST = FuncBody->getLocEnd().getLocWithOffset(1);
      //TheRewriter.InsertText(ST, SSAfter.str(), true, true);
      //TheRewriter.RemoveText(NamedDecl->getSourceRange().getBegin(), 50);
//    }
    return true;
  }


  bool VisitDecl(clang::Decl *d) {
    std::cout<<"Visiting Decl: "<<d->getDeclKindName()<<std::endl;

    //std::cout<<"VisitDecl" ""<<decl2str(f, sourceManager)<<std::endl;
    if (isa<RecordDecl>(d)) {
      std::string f = decl2str(d, sourceManager);
      int pos0 = funname.find("::");
      std::string tempfunname = funname;
      if(pos0>-1) {
        tempfunname = funname.substr(pos0+2);
      }
      int pos1 = f.find_first_of("(")-tempfunname.size();
      int pos2 = f.find_first_of("}");
      length_ = pos2 - pos1+1;
    }
    if(isa<FieldDecl>(d)) {
      NamedDecl *nd  = cast<NamedDecl>(d);
      std::string FuncName = nd->getQualifiedNameAsString();
      std::cout << "Visiting Field decl " << FuncName <<std::endl;
      if(FuncName==funname) {
        std::cout<<"Delete "<<length_<<std::endl;
        TheRewriter.RemoveText(d->getSourceRange().getBegin(), length_);
      }
    }
    return true;
  }
private:
  void AddBraces(Stmt *s);

  Rewriter &TheRewriter;
  SourceManager &sourceManager;
  int length_ = 0;
};


// Implementation of the ASTConsumer interface for reading an AST produced
// by the Clang parser.
class MyASTConsumer_var : public ASTConsumer
{
public:
  MyASTConsumer_var(Rewriter &R, SourceManager &S)
      : Visitor(R, S)
  {}

  // Override the method that gets called for each parsed top-level
  // declaration.
  virtual bool HandleTopLevelDecl(DeclGroupRef DR) {
    for (DeclGroupRef::iterator b = DR.begin(), e = DR.end();
         b != e; ++b)
      // Traverse the declaration using our AST visitor.
      Visitor.TraverseDecl(*b);
    return true;
  }

private:
  MyASTVisitor_var Visitor;
};

std::string FindErrorFunction(const std::string &filename) {
  std::ifstream file(filename);
  std::string str;
  std::string funname_;
  bool stack_start = false;
  while (std::getline(file, str)){
    if(str.substr(0, 5)=="ERROR" && !stack_start) {
      int pos = str.find("in function");
      if(pos>-1) {
        stack_start = true;
        int pos2 = str.find("(");
        funname_ = str.substr(pos+13, pos2-pos-13);
        std::cout<<"Error with "<<funname_<<std::endl<<std::endl;
      }
    } else if(str.substr(0, 5)!="ERROR" && stack_start){
      stack_start = false;
    }
  }

  return funname_;
}

void delete_const(string file_name) {
  ::cout<<"+++++visiting AST..."<<std::endl;
  // CompilerInstance will hold the instance of the Clang compiler for us,
  // managing the various objects needed to run the compiler.
  CompilerInstance TheCompInst;
  //TheCompInst.createDiagnostics(0, 0);
  TheCompInst.createDiagnostics();

  // Initialize target info with the default triple for our platform.
  //TargetOptions TO;
  auto TO = std::make_shared<clang::TargetOptions>();
  TO->Triple = llvm::sys::getDefaultTargetTriple();
  TargetInfo *TI = TargetInfo::CreateTargetInfo(
      TheCompInst.getDiagnostics(), TO);
  TheCompInst.setTarget(TI);

  TheCompInst.createFileManager();
  FileManager &FileMgr = TheCompInst.getFileManager();
  TheCompInst.createSourceManager(FileMgr);
  SourceManager &SourceMgr = TheCompInst.getSourceManager();
  TheCompInst.createPreprocessor(TU_Module);
  TheCompInst.createASTContext();

  // A Rewriter helps us manage the code rewriting task.
  Rewriter TheRewriter;
  TheRewriter.setSourceMgr(SourceMgr, TheCompInst.getLangOpts());

  // Set the main file handled by the source manager to the input file.
  const FileEntry *FileIn = FileMgr.getFile(file_name);
  //SourceMgr.createMainFileID(FileIn);
  SourceMgr.setMainFileID(
      SourceMgr.createFileID(FileIn, SourceLocation(), SrcMgr::C_User));
  TheCompInst.getDiagnosticClient().BeginSourceFile(
      TheCompInst.getLangOpts(),
      &TheCompInst.getPreprocessor());

  // Create an AST consumer instance which is going to get called by
  // ParseAST.
  MyASTConsumer TheConsumer(TheRewriter, SourceMgr);

  // Parse the file to AST, registering our consumer as the AST consumer.
  ParseAST(TheCompInst.getPreprocessor(), &TheConsumer,
           TheCompInst.getASTContext());

  // At this point the rewriter's buffer should be full with the rewritten
  // file contents.
  const RewriteBuffer *RewriteBuf =
      TheRewriter.getRewriteBufferFor(SourceMgr.getMainFileID());
  std::cout << string(RewriteBuf->begin(), RewriteBuf->end());
}

void static_var(string file_name) {
  std::cout<<"+++++visiting AST..."<<std::endl;
  // CompilerInstance will hold the instance of the Clang compiler for us,
  // managing the various objects needed to run the compiler.
  CompilerInstance TheCompInst;
  //TheCompInst.createDiagnostics(0, 0);
  TheCompInst.createDiagnostics();

  // Initialize target info with the default triple for our platform.
  //TargetOptions TO;
  auto TO = std::make_shared<clang::TargetOptions>();
  TO->Triple = llvm::sys::getDefaultTargetTriple();
  TargetInfo *TI = TargetInfo::CreateTargetInfo(
      TheCompInst.getDiagnostics(), TO);
  TheCompInst.setTarget(TI);

  TheCompInst.createFileManager();
  FileManager &FileMgr = TheCompInst.getFileManager();
  TheCompInst.createSourceManager(FileMgr);
  SourceManager &SourceMgr = TheCompInst.getSourceManager();
  TheCompInst.createPreprocessor(TU_Module);
  TheCompInst.createASTContext();

  // A Rewriter helps us manage the code rewriting task.
  Rewriter TheRewriter;
  TheRewriter.setSourceMgr(SourceMgr, TheCompInst.getLangOpts());

  // Set the main file handled by the source manager to the input file.
  const FileEntry *FileIn = FileMgr.getFile(file_name);
  //SourceMgr.createMainFileID(FileIn);
  SourceMgr.setMainFileID(
      SourceMgr.createFileID(FileIn, SourceLocation(), SrcMgr::C_User));
  TheCompInst.getDiagnosticClient().BeginSourceFile(
      TheCompInst.getLangOpts(),
      &TheCompInst.getPreprocessor());

  // Create an AST consumer instance which is going to get called by
  // ParseAST.
  MyASTConsumer_var TheConsumer(TheRewriter, SourceMgr);

  // Parse the file to AST, registering our consumer as the AST consumer.
  ParseAST(TheCompInst.getPreprocessor(), &TheConsumer,
           TheCompInst.getASTContext());

  // At this point the rewriter's buffer should be full with the rewritten
  // file contents.
  const RewriteBuffer *RewriteBuf =
      TheRewriter.getRewriteBufferFor(SourceMgr.getMainFileID());
  std::cout << string(RewriteBuf->begin(), RewriteBuf->end());
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        llvm::errs() << "Usage: repair <filename> <logname>\n";
        return 1;
    }
    std::cout<<"Analyzing error messages..."<<std::endl;
    funname = FindErrorFunction(argv[2]);
    std::cout<<"++Now I will apply some edits..."<<std::endl;
    int iter=1;
    if(flag){
      std::cout<<"+++Attempt "<<iter<<std::endl;
      std::cout<<"++++Now I will try the deletion edit..."<<std::endl;
      delete_const(argv[1]);
      //invoke C synthesis and differential execution
      iter++;
      std::cout<<"+++Attempt "<<iter<<std::endl;
      std::cout<<"++++Now I will try to make the variables static..."<<std::endl;
      static_var(argv[1]);
      //invoke C synthesis and differential execution
      if(!type_transformation)
        std::cout<<"Type transformation is not detected..."<<std::endl;
      // type_transformer(argv[1]);
      flag=0;
    }
      //invoke C synthesis and differential execution
      //flag=0;

    if(!flag)
      std::cout<<"It seems no more edits are applicable for this error type."<<std::endl;
    std::cout<<"Have a nice day!"<<std::endl;
    return 0;
}
