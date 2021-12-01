import os, sys, unittest
from tools import SamplesTestCase

OUTPUT = '''\
// Begin function foo returning void
void foo(custom_type* a, custom_type *b) {
  if (a[0] > 1) // the 'if' part
  {
    b[0] = 2;
  }
}
// End function foo
Type is : int *
found you!
Type is : int *
found you!
'''

PROG = 'rewritersample'

class TestRewriterSample(SamplesTestCase):
    def test_live(self):
        self.assertSampleOutput([PROG], 'cfunc_type.c', OUTPUT)

if __name__ == '__main__':
  unittest.main()
