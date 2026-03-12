#include <criterion/criterion.h>
#include <signal.h>
#include <common.h>
#include <time.h>

// Include src headers
#include "defines.h"
#include "student_code.h"

// Define any testing parameters
#define NUMBER_OF_REPEATS 10

// Include unit tests
#include "tests/unittests_base_bounds.c"
#include "tests/unittests_paging.c"
#include "tests/unittests_end2end.c"
#include "tests/unittests_reserved.c"

TestSuite(BaseBounds, .disabled=false, .timeout=60.0);
TestSuite(Paging, .disabled=false, .timeout=60.0);
TestSuite(End2End, .disabled=true, .timeout=60.0);

// From: https://github.com/codewars/criterion-hooks/blob/main/criterion-hooks.c
// PRE_TEST: occurs after the test initialization, but before the test is run.
ReportHook(PRE_TEST)(struct criterion_test *test) {
  log_info("Starting test: %s\n", test->name)
}

// From: https://github.com/codewars/criterion-hooks/blob/main/criterion-hooks.c
// TEST_CRASH: occurs when a test crashes unexpectedly.
ReportHook(TEST_CRASH)(struct criterion_test_stats *stats) {
  log_error("Test Crashed.  Caught unexpected signal: ");
  switch (stats->signal) {
  case SIGILL:
    log_error("SIGILL (%d). %s\n", stats->signal, "Invalid instruction.");
    break;

  case SIGFPE:
    log_error("SIGFPE (%d). %s\n", stats->signal, "Erroneous arithmetic operation.");
    break;

  case SIGSEGV:
    log_error("SIGSEGV (%d). %s\n", stats->signal, "Invalid memory access.");
    break;

  default:
    log_error("%d\n", stats->signal);
  }
}
