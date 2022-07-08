#include <glib.h>
#include <locale.h>
#include <stdlib.h>

void test_exit_non_zero_without_gpio() {
  int exitstatus = system("../src/mausberry-switch");
  g_assert_cmpint(WEXITSTATUS(exitstatus), ==, 1);
}

int main(int argc, char *argv[]) {
  setlocale(LC_ALL, "");

  g_test_init(&argc, &argv, NULL);
  g_test_bug_base("https://github.com/t-richards/mausberry-switch/issues");

  g_test_add_func("/libmaus/test_exit_nonzero",
                  test_exit_non_zero_without_gpio);

  return g_test_run();
}
