#include <stdio.h>
#include <string.h>
int main () {
   const char str[] = "This-is-just-a-test-string"; 
   const char ch = '-'; 
   char *p, *p2;

   p = strrchr(str, ch); 
   printf("String starting from last occurrence of %c is: %s\n", ch, p);

   p2 = strrchr(str, 'i'); 
   printf("String starting from last occurrence of 'i' is: %s\n", p2);

   return 0;
}
