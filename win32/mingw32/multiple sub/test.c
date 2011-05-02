//#include <math.h>
#include <stdio.h>
#include "sub/mod1.h"
#include "sub/mod2.h"


void func0(void) {
	printf("this is function 0\n");
}

int main (void) {
	
  //double x = sqrt (2.0);
  //printf ("The square root of 2.0 is %f\n", x);
  func0();
  func1();
  func2();
  
  return 0;

}