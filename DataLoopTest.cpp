#include "DataLoop.h"
#include <iostream>
#include <sstream>
#include <stdlib.h> // abs function

using std::cout;
using std::endl;

#ifndef ASSERT
#include <csignal>  // signal handler 
#include <cstring>  // memset
#include <string>
char programName[128];

void segFaultHandler(int, siginfo_t*, void* context) {
  char cmdbuffer[1024];
  char resultbuffer[128];
#ifdef __APPLE__
  sprintf(cmdbuffer, "addr2line -Cfip -e %s %p", programName,
      (void*)((ucontext_t*)context)->uc_mcontext->__ss.__rip);
#else
  sprintf(cmdbuffer, "addr2line -Cfip -e %s %p", programName,
      (void*)((ucontext_t*)context)->uc_mcontext.gregs[REG_RIP]);
#endif
  std::string result = "";
  FILE* pipe = popen(cmdbuffer, "r");
  if (!pipe) throw std::runtime_error("popen() failed!");
  try {
    while (fgets(resultbuffer, sizeof resultbuffer, pipe) != NULL) {
      result += resultbuffer;
    }
  } catch (...) {
    pclose(pipe);
    throw;
  }
  pclose(pipe);
  cout << "Segmentation fault occured in " << result;
#ifdef __APPLE__
  ((ucontext_t*)context)->uc_mcontext->__ss.__rip += 2;  // skip the seg fault
#else
  ((ucontext_t*)context)->uc_mcontext.gregs[REG_RIP] += 2;  // skip the seg fault
#endif
}

#define ASSERT(cond) if (!(cond)) { \
    cout << "failed ASSERT " << #cond << " at line " << __LINE__ << endl; \
  } else { \
    cout << __func__ << " - (" << #cond << ")" << " passed!" << endl; \
  }
#endif

/**
 * \struct DataLoopTest
 * \defgroup DataLoopTest
 * \brief Test cases for the DataLoop class
 */
struct DataLoopTest {
  /**
   * \brief A test function for default constructor
   */
  static void DefaultConstructorTest() {
    DataLoop *q = new DataLoop();
    // check if the structure is as expected
    ASSERT(q->start == nullptr);
    ASSERT(q->count == 0);
    delete q;
  }
   
  /**
   * \brief A test function for non-default constructor
   */
  static void NonDefaultConstructorTest() {
    DataLoop *q = new DataLoop(10);
    // check if the structure is as expected
    ASSERT(q->start != nullptr);
    ASSERT(q->start->next == q->start);
    ASSERT(q->start->prev == q->start);
    ASSERT(q->start->data == 10);
    ASSERT(q->count == 1);
    delete q;

    // my tests
    DataLoop *m = new DataLoop(5);
    // check if the structure is as expected
    ASSERT(m->start != nullptr);
    ASSERT(m->start->next == m->start);
    ASSERT(m->start->prev == m->start);
    ASSERT(m->start->data == 5);
    ASSERT(m->count == 1);
    delete m;

  }
  
  /**
   * \brief A test function for operator+=
   */
  static void OperatorPlusGetsTest() {
    // create a dataloop ->10<->30<->20<-
    DataLoop *q = new DataLoop();

    *q += 10;        // test single use
    // check if the structure is as expected
    ASSERT(q->start != nullptr);
    ASSERT(q->start->next == q->start);
    ASSERT(q->start->prev == q->start);
    ASSERT(q->start->data == 10);
    ASSERT(q->count == 1);
   
    *q += 30;
    ASSERT(q->start != nullptr);
    ASSERT(q->start->data == 10);  // check not changed
    ASSERT(q->start->next != nullptr);
    ASSERT(q->start->next->data == 30);
    ASSERT(q->start->prev != nullptr);
    ASSERT(q->start->prev->data == 30);

    *q += 20;  
    ASSERT(q->count == 3);

    ASSERT(q->start->next->next != nullptr);
    ASSERT(q->start->next->next->data == 20);
    ASSERT(q->start->next->next->next == q->start);
    ASSERT(q->start->prev->prev != nullptr);
    ASSERT(q->start->prev->prev->data == 30);
    ASSERT(q->start->prev->prev->prev == q->start);

    DataLoop *r = new DataLoop();
    ((*r += 10) += 30) += 20;    // test chaining (function return)
    ASSERT(r->count == 3);
    ASSERT(r->start != nullptr);
    ASSERT(r->start->data == 10);
    ASSERT(r->start->next != nullptr);
    ASSERT(r->start->next->data == 30);
    ASSERT(r->start->prev != nullptr);
    ASSERT(r->start->prev->data == 20);
    ASSERT(r->start->next->next != nullptr);
    ASSERT(r->start->next->next->data == 20);
    ASSERT(r->start->next->next->next == r->start);
    ASSERT(r->start->prev->prev != nullptr);
    ASSERT(r->start->prev->prev->data == 30);
    ASSERT(r->start->prev->prev->prev == r->start);

    delete q;
    delete r;

    // my tests
    DataLoop *m = new DataLoop();
    *m += 15;
    ASSERT(m->start != nullptr);
    ASSERT(m->start->next == m->start);
    ASSERT(m->start->prev == m->start);
    ASSERT(m->start->data == 15);
    ASSERT(m->count == 1);

    delete m;
    
  }
  

  /**
   * \brief A test function for copy constructor
   */
  static void CopyConstructorTest() {
    DataLoop *r = new DataLoop(10);
    *r += 30;
    *r += 20;
    DataLoop *q = new DataLoop(*r);
    // check if the structure is as expected
    ASSERT(q->count == 3);

    ASSERT(q->start != nullptr);
    ASSERT(q->start->data == 10);  // check not changed
    ASSERT(q->start->next != nullptr);
    ASSERT(q->start->next->data == 30);
    ASSERT(q->start->next->next != nullptr);
    ASSERT(q->start->next->next->data == 20);
    ASSERT(q->start->next->next->next == q->start);
    ASSERT(q->start->prev != nullptr);
    ASSERT(q->start->prev->data == 20);
    ASSERT(q->start->prev->prev != nullptr);
    ASSERT(q->start->prev->prev->data == 30);
    ASSERT(q->start->prev->prev->prev == q->start);

    // check that updating r doesn't affect q
    *r += 15;
    ASSERT(r->count == 4);
    ASSERT(q->count == 3);
    ASSERT(r->start->prev->data == 15);
    ASSERT(q->start->prev->data == 20);

    // my tests
    DataLoop *m = new DataLoop();
    DataLoop *k = new DataLoop(*m);

    ASSERT(k->count == 0);
    ASSERT(k->start == nullptr);

    std::cout << "after copy constructor" << std::endl;
    std::cout << *k << std::endl;
    std::cout << *m << std::endl;

    delete r;
    delete q;
    delete m;
    delete k;
  }
  
  /**
   * \brief A test function for assignment operator
   */
  static void OperatorAssignmentTest() {
    DataLoop *r = new DataLoop(10);
    *r += 30; *r += 20;
    DataLoop *q = new DataLoop(15);

    std::cout << "before assignment" << std::endl;
    std::cout << "r: " << *r << std::endl;
    std::cout << "q: " << *q << std::endl;

    *q = *r;
    // check if the structure is as expected
    ASSERT(q->count == 3);

    ASSERT(q->start != nullptr);
    ASSERT(q->start->data == 10);  // check not changed
    ASSERT(q->start->next != nullptr);
    ASSERT(q->start->next->data == 30);
    ASSERT(q->start->next->next != nullptr);
    ASSERT(q->start->next->next->data == 20);
    ASSERT(q->start->next->next->next == q->start);
    ASSERT(q->start->prev != nullptr);
    ASSERT(q->start->prev->data == 20);
    ASSERT(q->start->prev->prev != nullptr);
    ASSERT(q->start->prev->prev->data == 30);
    ASSERT(q->start->prev->prev->prev == q->start);

    // check that updating r doesn't affect q
    // TODO: add your own tests here
    std::cout << "after assignment" << std::endl;
    std::cout << "r: " << *r << std::endl;
    std::cout << "q: " << *q << std::endl;

    *r += 100; // updating r
    ASSERT(q->count == 3);
    ASSERT(q->start != nullptr);
    ASSERT(q->start->data == 10);  // check not changed
    ASSERT(q->start->next != nullptr);
    ASSERT(q->start->next->data == 30);
    ASSERT(q->start->next->next != nullptr);
    ASSERT(q->start->next->next->data == 20);
    ASSERT(q->start->next->next->next == q->start);
    ASSERT(q->start->prev != nullptr);
    ASSERT(q->start->prev->data == 20);
    ASSERT(q->start->prev->prev != nullptr);
    ASSERT(q->start->prev->prev->data == 30);
    ASSERT(q->start->prev->prev->prev == q->start);

    std::cout << "after change to r" << std::endl;
    std::cout << "r: " << *r << std::endl;
    std::cout << "q: " << *q << std::endl;

    // other tests
    DataLoop *m = new DataLoop();
    DataLoop *a = new DataLoop(1);
    std::cout << "before assignment" << std::endl;
    std::cout << "a: " << *a << std::endl;
    std::cout << "m: " << *m << std::endl;

    *a = *m;

    ASSERT(m->count == 0);
    ASSERT(m->start == nullptr);
    ASSERT(a->count == 0);
    ASSERT(a->start == nullptr);

    std::cout << "after assignment" << std::endl;
    std::cout << "a: " << *a << std::endl;
    std::cout << "m: " << *m << std::endl;

    delete r;
    delete q;
    delete a;
    delete m;
  }
  
  
  /**
   * \brief A test function for equality operator
   */
  static void OperatorEqualityTest() {
    DataLoop *q = new DataLoop();
    *q += 10; *q += 30; *q += 20;    
    DataLoop *r = new DataLoop();
    *r += 10; *r += 30; *r += 20;
    // check if they are the same
    ASSERT(*q == *r);

    q->start->data = 15;   // change node
    ASSERT(!(*q == *r));

    q->start->data = 10;   // reset to same
    q->start = q->start->next->next;   // move start
    ASSERT(!(*q == *r));

    q->start = q->start->prev->prev;  // reset to same
    *q += 15;               // add node
    ASSERT(!(*q == *r));

    // my tests
    DataLoop *m = new DataLoop();
    DataLoop *k = new DataLoop(1);
    ASSERT(!(*m == *k));

    DataLoop *a = new DataLoop();
    DataLoop *w = new DataLoop();
    ASSERT(*a == *w);
    
    delete r;
    delete q;
    delete m;
    delete k;
    delete a;
    delete w;
  }
  
  /**
   * \brief A test function for concatenate operator
   */
  static void OperatorConcatenateTest() {
    DataLoop *q = new DataLoop(10);
    *q += 30;
    DataLoop *r = new DataLoop(15);
    *r += 5;
    DataLoop *s = new DataLoop(100);

    std::cout << "before concat: " << std::endl;
    std::cout << "q: " << *q << std::endl;
    std::cout << "r: " << *r << std::endl;
    std::cout << "s: " << *s << std::endl;

    *s = *q + *r;

    ASSERT(s->count == 4);
    // check if the structure is as expected
    ASSERT(s->start != nullptr);
    ASSERT(s->start->data == 10);
    ASSERT(s->start->next != nullptr);
    ASSERT(s->start->next->data == 30);
    ASSERT(s->start->next->next != nullptr);
    ASSERT(s->start->next->next->data == 15);
    ASSERT(s->start->next->next->next != nullptr);
    ASSERT(s->start->next->next->next->data == 5);
    ASSERT(s->start->next->next->next->next == s->start);

    // TODO: add prev tests
    ASSERT(s->start != nullptr);
    ASSERT(s->start->data == 10);
    ASSERT(s->start->prev != nullptr);
    ASSERT(s->start->prev->data == 5);
    ASSERT(s->start->prev->prev != nullptr);
    ASSERT(s->start->prev->prev->data == 15);
    ASSERT(s->start->prev->prev->prev != nullptr);
    ASSERT(s->start->prev->prev->prev->data == 30);
    ASSERT(s->start->prev->prev->prev->prev == s->start);


    // TODO: test that q and r have not changed
    std::cout << "after concat" << std::endl;
    std::cout << "q: " << *q << std::endl;
    std::cout << "r: " << *r << std::endl;
    std::cout << "s: " << *s << std::endl;
    ASSERT(q->start != nullptr);
    ASSERT(q->start->data == 10);
    ASSERT(q->start->next != nullptr);
    ASSERT(q->start->next->data == 30);
    ASSERT(q->start->next->next != nullptr);
    ASSERT(q->start->next->next->data == 10);
      
    ASSERT(r->start != nullptr);
    ASSERT(r->start->data == 15);
    ASSERT(r->start->next != nullptr);
    ASSERT(r->start->next->data == 5);
    ASSERT(r->start->next->next != nullptr);
    ASSERT(r->start->next->next->data == 15);

    // other tests
    DataLoop *m = new DataLoop(1);
    DataLoop *a = new DataLoop();
    DataLoop *k = new DataLoop(100);

    *k = *m + *a;
    std::cout << "after concat" << std::endl;
    std::cout << "m: " << *m << std::endl;
    std::cout << "a: " << *a << std::endl;
    std::cout << "k: " << *k << std::endl;
    ASSERT(k->start != nullptr);
    ASSERT(k->start->data == 1);
    ASSERT(k->start->next == k->start);

    delete s;
    delete r;
    delete q;
    delete m;
    delete a;
    delete k;
  }
  
  /**
   * \brief A test function for stream insertion operator
   */
  static void OperatorStreamInsertionTest() {
    DataLoop *q = new DataLoop();
    std::stringstream empty_ss;
    empty_ss << *q;
    ASSERT(empty_ss.str() == ">no values<");
    *q += 10; *q += 30; *q += 20;
    std::stringstream ss;
    ss << *q;
    ASSERT(ss.str() == "-> 10 <--> 30 <--> 20 <-");
    delete q;

    // my tests
    DataLoop *m = new DataLoop(1);
    std::stringstream ss2;
    ss2 << *m;
    ASSERT(ss2.str() == "-> 1 <-");
    delete m;
  }

  /**
   * \brief A test function for shift operator
   */
  static void OperatorShiftTest() {
    DataLoop *q = new DataLoop();
    for (int i = 1; i <= 10; i++) {
      *q += i;
    }
    *q ^ 0;
    ASSERT(q->start->data == 1);
    *q ^ 1;
    ASSERT(q->start->data == 2);
    *q ^ 5;
    ASSERT(q->start->data == 7);
    *q ^ 20;
    ASSERT(q->start->data == 7);
    *q ^ -1;
    ASSERT(q->start->data == 6);
    *q ^ -8;
    ASSERT(q->start->data == 8);
    *q ^ -49;
    ASSERT(q->start->data == 9);
    
    delete q;

    // my tests
    DataLoop *m = new DataLoop(10);
    *m += 5;

    *m ^ 208;
    ASSERT(m->start->data == 10);
    *m ^ 319;
    ASSERT(m->start->data == 5);

    delete m;

    DataLoop *r = new DataLoop(8);
    *r ^ 300;
    ASSERT(r->start->data == 8);

    delete r;

  }

  /**
   * \brief A test function for length function
   */
  static void FunctionLengthTest() {
    DataLoop *q = new DataLoop();
    ASSERT(q->length() == 0);
    *q += 10;
    ASSERT(q->length() == 1);
    *q += 30; *q += 20;
    ASSERT(q->length() == 3);
    delete q;

    // my tests
    DataLoop *m = new DataLoop(10);
    ASSERT(m->length() == 1);
    delete m;
  }

  /**
   * \brief A test function for splice function, depends on operator<< and operator+=
   */
  static void FunctionSpliceTest() {

    DataLoop *q = new DataLoop();
    for (int i = 1; i <= 4; i++) {
      *q += i;
    }
    DataLoop *r = new DataLoop();
    for (int j = 10; j <= 12; j++) {
      *r += j;
    }

    q->splice(*r, 0);
    ASSERT(q->count == 7);
    ASSERT(r->count == 0);
    ASSERT(r->start == nullptr);

    std::stringstream ss1;
    ss1 << *q;
    // std::cout << ss1.str() << std::endl; 
    ASSERT(ss1.str() == "-> 10 <--> 11 <--> 12 <--> 1 <--> 2 <--> 3 <--> 4 <-");

    std::stringstream ss2;
    ss2 << *r;
    ASSERT(ss2.str() == ">no values<");
    
    DataLoop *p = new DataLoop();
    *p += 20;
    *p += 25;
    q->splice(*p, 9);
    ASSERT(q->count == 9);
    std::stringstream ss3;
    ss3 << *q;
    // std::cout << ss3.str() << std::endl;
    ASSERT(ss3.str() == "-> 10 <--> 11 <--> 20 <--> 25 <--> 12 <--> 1 <--> 2 <--> 3 <--> 4 <-");

    delete p;   // empty list
    // std::cout << "p count: " << p->count << std::endl;
    // std::stringstream ss4;
    // ss4 << *p;
    // std::cout << "p: " << ss4.str() << std::endl;
    delete r;   // empty list
    delete q; 

    // my tests
  
    // test 1
    DataLoop *k = new DataLoop();
    DataLoop *w = new DataLoop();
    for (int i = 1; i <= 3; i++) {
      *k += i;
    }

    k->splice(*w, 9);
    ASSERT(k->count == 3);
    ASSERT(w->count == 0);
    ASSERT(w->start == nullptr);
    
    std::stringstream ss;
    ss << *k;
    std::cout << ss.str() << std::endl;
    ASSERT(ss.str() == "-> 1 <--> 2 <--> 3 <-");
    
    // test 2
    DataLoop *m = new DataLoop();
    DataLoop *a = new DataLoop();

    *m += 10; // one element data loop
    for (int i = 1; i <= 3; i++) {
      *a += i;
    }

    a->splice(*m, 8);
    ASSERT(a->count == 4);
    ASSERT(m->count == 0);
    ASSERT(m->start == nullptr);
    std::stringstream ss4;
    ss4 << *a;
    std::cout << ss4.str() << std::endl;
    ASSERT(ss4.str() == "-> 1 <--> 2 <--> 10 <--> 3 <-");

    delete m;
    delete a;
    delete k;
    delete w;

  }

};

// call our test functions in the main
int main(int, char* argv[]) {
  cout << "Testing DataLoop" << endl;
  // register a seg fault handler
  sprintf(programName, "%s", argv[0]);
  struct sigaction signalAction;
  memset(&signalAction, 0, sizeof(struct sigaction));
  signalAction.sa_flags = SA_SIGINFO;
  signalAction.sa_sigaction = segFaultHandler;
  sigaction(SIGSEGV, &signalAction, NULL);
  
  DataLoopTest::DefaultConstructorTest();
  DataLoopTest::NonDefaultConstructorTest();
  DataLoopTest::OperatorPlusGetsTest();    // needed for below tests

  DataLoopTest::CopyConstructorTest();
  DataLoopTest::OperatorAssignmentTest();
  DataLoopTest::OperatorEqualityTest();
  DataLoopTest::OperatorConcatenateTest();
  DataLoopTest::OperatorStreamInsertionTest();

  DataLoopTest::OperatorShiftTest();
  DataLoopTest::FunctionLengthTest();
  DataLoopTest::FunctionSpliceTest(); 
  
  return 0;
}
