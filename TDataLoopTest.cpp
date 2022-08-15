#include "TDataLoop.h"
#include <iostream>
#include <sstream>
#include <string>

using std::cout;
using std::endl;
using std::string;

#ifndef ASSERT
#include <csignal>  // signal handler 
#include <cstring>  // memset

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
 * \struct TDataLoopTest
 * \defgroup TDataLoopTest
 * \brief Test cases for the TDataLoop class
 */
struct TDataLoopTest {
  // aliases
  using CTDataLoop = TDataLoop<char>;
  using STDataLoop = TDataLoop<string>;
  using DTDataLoop = TDataLoop<double>;
  
  /**
   * \brief A test function for default constructor
   */
  static void DefaultConstructorTest() {
    CTDataLoop *q = new CTDataLoop();
    // check if the structure is as expected
    ASSERT(q->start == nullptr);
    ASSERT(q->count == 0);
    delete q;

    STDataLoop *s = new STDataLoop();
    // check if the structure is as expected
    ASSERT(s->start == nullptr);
    ASSERT(s->count == 0);
    delete s;

    // my test
    DTDataLoop *m = new DTDataLoop();
    // check if the structure is as expected
    ASSERT(m->start == nullptr);
    ASSERT(m->count == 0);
    delete m;

  }
   
  /**
   * \brief A test function for non-default constructor
   */
  static void NonDefaultConstructorTest() {
    CTDataLoop *q = new CTDataLoop('1');
    // check if the structure is as expected
    ASSERT(q->start != nullptr);
    ASSERT(q->start->next == q->start);
    ASSERT(q->start->prev == q->start);
    ASSERT(q->start->data == '1');
    ASSERT(q->count == 1);
    delete q;

    STDataLoop *s = new STDataLoop("10");
    // check if the structure is as expected
    ASSERT(s->start != nullptr);
    ASSERT(s->start->next == s->start);
    ASSERT(s->start->prev == s->start);
    ASSERT(s->start->data == "10");
    ASSERT(s->count == 1);
    delete s;

    // my tests
    DTDataLoop *f = new DTDataLoop(1.5);
    // check if the structure is as expected
    ASSERT(f->start != nullptr);
    ASSERT(f->start->next == f->start);
    ASSERT(f->start->prev == f->start);
    ASSERT(f->start->data == 1.5);
    ASSERT(f->count == 1);
    delete f;
}
  
  /**
   * \brief A test function for operator+= using char
   */
  static void OperatorPlusGetsTestChar() {

    CTDataLoop *q = new CTDataLoop();

    *q += '1';        // test single use
    // check if the structure is as expected
    ASSERT(q->start != nullptr);
    ASSERT(q->start->next == q->start);
    ASSERT(q->start->prev == q->start);
    ASSERT(q->start->data == '1');
    ASSERT(q->count == 1);
   

    *q += '3';
    ASSERT(q->start != nullptr);
    ASSERT(q->start->data == '1');  // check not changed
    ASSERT(q->start->next != nullptr);
    ASSERT(q->start->next->data == '3');
    ASSERT(q->start->prev != nullptr);
    ASSERT(q->start->prev->data == '3');

    *q += '2';  
    ASSERT(q->count == 3);

    ASSERT(q->start->next->next != nullptr);
    ASSERT(q->start->next->next->data == '2');
    ASSERT(q->start->next->next->next == q->start);
    ASSERT(q->start->prev->prev != nullptr);
    ASSERT(q->start->prev->prev->data == '3');
    ASSERT(q->start->prev->prev->prev == q->start);

    CTDataLoop *r = new CTDataLoop();
    ((*r += '1') += '3') += '2';    // test chaining (function return)
    ASSERT(r->count == 3);
    ASSERT(r->start != nullptr);
    ASSERT(r->start->data == '1');
    ASSERT(r->start->next != nullptr);
    ASSERT(r->start->next->data == '3');
    ASSERT(r->start->prev != nullptr);
    ASSERT(r->start->prev->data == '2');
    ASSERT(r->start->next->next != nullptr);
    ASSERT(r->start->next->next->data == '2');
    ASSERT(r->start->next->next->next == r->start);
    ASSERT(r->start->prev->prev != nullptr);
    ASSERT(r->start->prev->prev->data == '3');
    ASSERT(r->start->prev->prev->prev == r->start);

    delete q;
    delete r;

    // my test
    CTDataLoop *m = new CTDataLoop();
    *m += '5';
    ASSERT(m->start != nullptr);
    ASSERT(m->start->next == m->start);
    ASSERT(m->start->prev == m->start);
    ASSERT(m->start->data == '5');
    ASSERT(m->count == 1);

    delete m;

  }

    /**
   * \brief A test function for operator+= using double
   */
  static void OperatorPlusGetsTestDouble() {
    
    // my test
    DTDataLoop *m = new DTDataLoop();
    ((*m += 5.7) += 10.9);
    ASSERT(m->count == 2);
    ASSERT(m->start != nullptr);
    ASSERT(m->start->data == 5.7);
    ASSERT(m->start->next != nullptr);
    ASSERT(m->start->next->data == 10.9);
    ASSERT(m->start->prev != nullptr);
    ASSERT(m->start->prev->data == 10.9);
    ASSERT(m->start->next->next != nullptr);
    ASSERT(m->start->next->data == 10.9);
    ASSERT(m->start->next->next == m->start);

    delete m;
  }

    /**
   * \brief A test function for operator+= using string
   */
  static void OperatorPlusGetsTestString() {

    STDataLoop *q = new STDataLoop();

    *q += "10";        // test single use
    // check if the structure is as expected
    ASSERT(q->start != nullptr);
    ASSERT(q->start->next == q->start);
    ASSERT(q->start->prev == q->start);
    ASSERT(q->start->data == "10");
    ASSERT(q->count == 1);
   

    *q += "30";
    ASSERT(q->start != nullptr);
    ASSERT(q->start->data == "10");  // check not changed
    ASSERT(q->start->next != nullptr);
    ASSERT(q->start->next->data == "30");
    ASSERT(q->start->prev != nullptr);
    ASSERT(q->start->prev->data == "30");

    *q += "20";  
    ASSERT(q->count == 3);

    ASSERT(q->start->next->next != nullptr);
    ASSERT(q->start->next->next->data == "20");
    ASSERT(q->start->next->next->next == q->start);
    ASSERT(q->start->prev->prev != nullptr);
    ASSERT(q->start->prev->prev->data == "30");
    ASSERT(q->start->prev->prev->prev == q->start);

    STDataLoop *r = new STDataLoop();
    ((*r += "10") += "30") += "20";    // test chaining (function return)
    ASSERT(r->count == 3);
    ASSERT(r->start != nullptr);
    ASSERT(r->start->data == "10");
    ASSERT(r->start->next != nullptr);
    ASSERT(r->start->next->data == "30");
    ASSERT(r->start->prev != nullptr);
    ASSERT(r->start->prev->data == "20");
    ASSERT(r->start->next->next != nullptr);
    ASSERT(r->start->next->next->data == "20");
    ASSERT(r->start->next->next->next == r->start);
    ASSERT(r->start->prev->prev != nullptr);
    ASSERT(r->start->prev->prev->data == "30");
    ASSERT(r->start->prev->prev->prev == r->start);

    delete q;
    delete r;

    // my test
    STDataLoop *m = new STDataLoop();
    *m += "15";
    ASSERT(m->start != nullptr);
    ASSERT(m->start->next == m->start);
    ASSERT(m->start->prev == m->start);
    ASSERT(m->start->data == "15");
    ASSERT(m->count == 1);

    delete m;
  }


  /**
   * \brief A test function for copy constructor using char
   */
  static void CopyConstructorTest() {
    CTDataLoop *r = new CTDataLoop('1');
    *r += '3';
    *r += '2';
    CTDataLoop *q = new CTDataLoop(*r);
    // check if the structure is as expected
    ASSERT(q->count == 3);

    ASSERT(q->start != nullptr);
    ASSERT(q->start->data == '1');  // check not changed
    ASSERT(q->start->next != nullptr);
    ASSERT(q->start->next->data == '3');
    ASSERT(q->start->next->next != nullptr);
    ASSERT(q->start->next->next->data == '2');
    ASSERT(q->start->next->next->next == q->start);
    ASSERT(q->start->prev != nullptr);
    ASSERT(q->start->prev->data == '2');
    ASSERT(q->start->prev->prev != nullptr);
    ASSERT(q->start->prev->prev->data == '3');
    ASSERT(q->start->prev->prev->prev == q->start);

    // check that updating r doesn't affect q
    *r += '5';
    ASSERT(r->count == 4);
    ASSERT(q->count == 3);
    ASSERT(r->start->prev->data == '5');
    ASSERT(q->start->prev->data == '2');

    delete r;
    delete q;

    // my tests
    DTDataLoop *m = new DTDataLoop();
    DTDataLoop *k = new DTDataLoop(*m);

    ASSERT(k->count == 0);
    ASSERT(k->start == nullptr);

    std::cout << "after copy constructor" << std::endl;
    std::cout << *k << std::endl;
    std::cout << *m << std::endl;

    delete m;
    delete k;

  }
  
  /**
   * \brief A test function for assignment operator using string
   */
  static void OperatorAssignmentTest() {
    STDataLoop *r = new STDataLoop("10");
    *r += "30"; *r += "20";
    STDataLoop *q = new STDataLoop("15");

    std::cout << "before assignment" << std::endl;
    std::cout << "r: " << *r << std::endl;
    std::cout << "q: " << *q << std::endl;

    *q = *r;
    // check if the structure is as expected
    ASSERT(q->count == 3);

    ASSERT(q->start != nullptr);
    ASSERT(q->start->data == "10");  // check not changed
    ASSERT(q->start->next != nullptr);
    ASSERT(q->start->next->data == "30");
    ASSERT(q->start->next->next != nullptr);
    ASSERT(q->start->next->next->data == "20");
    ASSERT(q->start->next->next->next == q->start);
    ASSERT(q->start->prev != nullptr);
    ASSERT(q->start->prev->data == "20");
    ASSERT(q->start->prev->prev != nullptr);
    ASSERT(q->start->prev->prev->data == "30");
    ASSERT(q->start->prev->prev->prev == q->start);

    // check that updating r doesn't affect q
    // TODO: add your own tests here
    std::cout << "after assignment" << std::endl;
    std::cout << "r: " << *r << std::endl;
    std::cout << "q: " << *q << std::endl;

    *r += "100"; // updating r
    ASSERT(q->count == 3);
    ASSERT(q->start != nullptr);
    ASSERT(q->start->data == "10");  // check not changed
    ASSERT(q->start->next != nullptr);
    ASSERT(q->start->next->data == "30");
    ASSERT(q->start->next->next != nullptr);
    ASSERT(q->start->next->next->data == "20");
    ASSERT(q->start->next->next->next == q->start);
    ASSERT(q->start->prev != nullptr);
    ASSERT(q->start->prev->data == "20");
    ASSERT(q->start->prev->prev != nullptr);
    ASSERT(q->start->prev->prev->data == "30");
    ASSERT(q->start->prev->prev->prev == q->start);

    std::cout << "after change to r" << std::endl;
    std::cout << "r: " << *r << std::endl;
    std::cout << "q: " << *q << std::endl;

    // other tests
    STDataLoop *m = new STDataLoop();
    STDataLoop *a = new STDataLoop("1");
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
   * \brief A test function for equality operator using char
   */
  static void OperatorEqualityTest() {
    CTDataLoop *q = new CTDataLoop();
    *q += '1'; *q += '3'; *q += '2';    
    CTDataLoop *r = new CTDataLoop();
    *r += '1'; *r += '3'; *r += '2';
    // check if they are the same
    ASSERT(*q == *r);

    q->start->data = '5';   // change node
    ASSERT(!(*q == *r));

    q->start->data = '1';   // reset to same
    q->start = q->start->next->next;   // move start
    ASSERT(!(*q == *r));

    q->start = q->start->prev->prev;  // reset to same
    *q += '8';               // add node
    ASSERT(!(*q == *r));
    
    delete r;
    delete q;

    // my tests
    CTDataLoop *m = new CTDataLoop();
    CTDataLoop *k = new CTDataLoop('1');
    ASSERT(!(*m == *k));

    CTDataLoop *a = new CTDataLoop();
    CTDataLoop *w = new CTDataLoop();
    ASSERT(*a == *w);

    delete m;
    delete k;
    delete a;
    delete w;

  }
  
  /**
   * \brief A test function for concatenate operator using string
   */
  static void OperatorConcatenateTest() {
    STDataLoop *q = new STDataLoop("10");
    *q += "30";
    STDataLoop *r = new STDataLoop("15");
    *r += "5";
    STDataLoop *s = new STDataLoop("100");
    *s = *q + *r;
    ASSERT(s->count == 4);

    // check if the structure is as expected
    ASSERT(s->start != nullptr);
    ASSERT(s->start->data == "10");
    ASSERT(s->start->next != nullptr);
    ASSERT(s->start->next->data == "30");
    ASSERT(s->start->next->next != nullptr);
    ASSERT(s->start->next->next->data == "15");
    ASSERT(s->start->next->next->next != nullptr);
    ASSERT(s->start->next->next->next->data == "5");
    ASSERT(s->start->next->next->next->next == s->start);

    // TODO: add prev tests
    ASSERT(s->start != nullptr);
    ASSERT(s->start->data == "10");
    ASSERT(s->start->prev != nullptr);
    ASSERT(s->start->prev->data == "5");
    ASSERT(s->start->prev->prev != nullptr);
    ASSERT(s->start->prev->prev->data == "15");
    ASSERT(s->start->prev->prev->prev != nullptr);
    ASSERT(s->start->prev->prev->prev->data == "30");
    ASSERT(s->start->prev->prev->prev->prev == s->start);

    // TODO: test that q and r have not changed
    std::cout << "after concat" << std::endl;
    std::cout << "q: " << *q << std::endl;
    std::cout << "r: " << *r << std::endl;
    ASSERT(q->start != nullptr);
    ASSERT(q->start->data == "10");
    ASSERT(q->start->next != nullptr);
    ASSERT(q->start->next->data == "30");
    ASSERT(q->start->next->next != nullptr);
    ASSERT(q->start->next->next->data == "10");
      
    ASSERT(r->start != nullptr);
    ASSERT(r->start->data == "15");
    ASSERT(r->start->next != nullptr);
    ASSERT(r->start->next->data == "5");
    ASSERT(r->start->next->next != nullptr);
    ASSERT(r->start->next->next->data == "15");

    delete s;
    delete r;
    delete q;

    // other tests
    STDataLoop *m = new STDataLoop("1");
    STDataLoop *a = new STDataLoop();
    STDataLoop *k = new STDataLoop("100");

    *k = *m + *a;
    std::cout << "after concat" << std::endl;
    std::cout << "m: " << *m << std::endl;
    std::cout << "a: " << *a << std::endl;
    std::cout << "k: " << *k << std::endl;
    ASSERT(k->start != nullptr);
    ASSERT(k->start->data == "1");
    ASSERT(k->start->next == k->start);

    delete m;
    delete a;
    delete k;

  }
  
  /**
   * \brief A test function for stream insertion operator using char
   */
  static void OperatorStreamInsertionTestChar() {
    CTDataLoop *q = new CTDataLoop();
    std::stringstream empty_ss;
    empty_ss << *q;
    ASSERT(empty_ss.str() == ">no values<");
    *q += 'A'; *q += 'B'; *q += 'C';
    std::stringstream ss;
    ss << *q;
    ASSERT(ss.str() == "-> A <--> B <--> C <-");
    delete q;

    // my tests
    CTDataLoop *m = new CTDataLoop('1');
    std::stringstream ss2;
    ss2 << *m;
    ASSERT(ss2.str() == "-> 1 <-");
    delete m;

  }

  /**
   * \brief A test function for stream insertion operator using string
   */
  static void OperatorStreamInsertionTestString() {
    STDataLoop *q = new STDataLoop();
    std::stringstream empty_ss;
    empty_ss << *q;
    ASSERT(empty_ss.str() == ">no values<");
    *q += "10"; *q += "30"; *q += "20";
    std::stringstream ss;
    ss << *q;
    ASSERT(ss.str() == "-> 10 <--> 30 <--> 20 <-");
    delete q;

    // my tests
    STDataLoop *m = new STDataLoop("1");
    std::stringstream ss2;
    ss2 << *m;
    ASSERT(ss2.str() == "-> 1 <-");
    delete m;
  }

  /**
   * \brief A test function for shift operator using char
   */
  static void OperatorShiftTest() {
    CTDataLoop *q = new CTDataLoop();
    for (char i = 'A'; i <= 'J'; i++) {
      *q += i;
    }
    *q ^ 0;
    ASSERT(q->start->data == 'A');
    *q ^ 1;
    ASSERT(q->start->data == 'B');
    *q ^ 5;
    ASSERT(q->start->data == 'G');
    *q ^ 20;
    ASSERT(q->start->data == 'G');
    *q ^ -1;
    ASSERT(q->start->data == 'F');
    *q ^ -8;
    ASSERT(q->start->data == 'H');
    *q ^ -49;
    ASSERT(q->start->data == 'I');
    
    delete q;

    // my tests
    STDataLoop *m = new STDataLoop("10");
    *m += "5";

    *m ^ 208;
    ASSERT(m->start->data == "10");
    *m ^ 319;
    ASSERT(m->start->data == "5");

    delete m;

    STDataLoop *r = new STDataLoop("8");
    *r ^ 300;
    ASSERT(r->start->data == "8");

    delete r;
  }

  /**
   * \brief A test function for length function using string
   */
  static void FunctionLengthTest() {
    STDataLoop *q = new STDataLoop();
    ASSERT(q->length() == 0);
    *q += "10";
    ASSERT(q->length() == 1);
    *q += "30"; *q += "20";
    ASSERT(q->length() == 3);
    delete q;

    // my tests
    STDataLoop *m = new STDataLoop("10");
    ASSERT(m->length() == 1);
    delete m;
  }

  /**
   * \brief A test function for splice function, depends on operator<< and operator+=
   */
  static void FunctionSpliceTest() {
    TDataLoop<int> *q = new TDataLoop<int>();
    for (int i = 1; i <= 4; i++) {
      *q += i;
    }
    TDataLoop<int> *r = new TDataLoop<int>();
    for (int j = 10; j <= 12; j++) {
      *r += j;
    }

    q->splice(*r, 0);
    ASSERT(q->count == 7);
    ASSERT(r->count == 0);
    ASSERT(r->start == nullptr);

    std::stringstream ss1;
    ss1 << *q;
    ASSERT(ss1.str() == "-> 10 <--> 11 <--> 12 <--> 1 <--> 2 <--> 3 <--> 4 <-");

    std::stringstream ss2;
    ss2 << *r;
    ASSERT(ss2.str() == ">no values<");
    
    TDataLoop<int> *p = new TDataLoop<int>();
    *p += 20;
    *p += 25;
    q->splice(*p, 9);
    ASSERT(q->count == 9);
    std::stringstream ss3;
    ss3 << *q;
    ASSERT(ss3.str() == "-> 10 <--> 11 <--> 20 <--> 25 <--> 12 <--> 1 <--> 2 <--> 3 <--> 4 <-");

    delete p;  // empty list 
    delete r;  // empty list
    delete q;  

    // my tests

    // test 1
    STDataLoop *k = new STDataLoop();
    STDataLoop *w = new STDataLoop();
    *k += "1";
    *k += "2";
    *k += "3";

    k->splice(*w, 9);
    ASSERT(k->count == 3);
    ASSERT(w->count == 0);
    ASSERT(w->start == nullptr);
    
    std::stringstream ss;
    ss << *k;
    std::cout << ss.str() << std::endl;
    ASSERT(ss.str() == "-> 1 <--> 2 <--> 3 <-");
    
    // test 2
    CTDataLoop *m = new CTDataLoop();
    CTDataLoop *a = new CTDataLoop();

    *m += '5'; // one element data loop
    *a += '1';
    *a += '2';
    *a += '3';

    a->splice(*m, 8);
    ASSERT(a->count == 4);
    ASSERT(m->count == 0);
    ASSERT(m->start == nullptr);
    std::stringstream ss4;
    ss4 << *a;
    std::cout << ss4.str() << std::endl;
    ASSERT(ss4.str() == "-> 1 <--> 2 <--> 5 <--> 3 <-");

    delete m;
    delete a;
    delete k;
    delete w;
  }

};

// call our test functions in the main
int main(int, char* argv[]) {
  cout << "Testing TDataLoop" << endl;
  // register a seg fault handler
  sprintf(programName, "%s", argv[0]);
  struct sigaction signalAction;
  memset(&signalAction, 0, sizeof(struct sigaction));
  signalAction.sa_flags = SA_SIGINFO;
  signalAction.sa_sigaction = segFaultHandler;
  sigaction(SIGSEGV, &signalAction, NULL);
  
  TDataLoopTest::DefaultConstructorTest();
  TDataLoopTest::NonDefaultConstructorTest();
  TDataLoopTest::OperatorPlusGetsTestChar();    // needed for below tests
  TDataLoopTest::OperatorPlusGetsTestString();    // needed for below tests
  TDataLoopTest::OperatorPlusGetsTestDouble();

  TDataLoopTest::CopyConstructorTest();
  TDataLoopTest::OperatorAssignmentTest();
  TDataLoopTest::OperatorEqualityTest();
  TDataLoopTest::OperatorConcatenateTest();
  TDataLoopTest::OperatorStreamInsertionTestChar();
  TDataLoopTest::OperatorStreamInsertionTestString();

  TDataLoopTest::OperatorShiftTest();  // char
  TDataLoopTest::FunctionLengthTest();  // string
  TDataLoopTest::FunctionSpliceTest();   // int
  
  return 0;
}
