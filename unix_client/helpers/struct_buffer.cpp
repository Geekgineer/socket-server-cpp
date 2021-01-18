#include <stdio.h>
#include <string.h>
#include <iostream>
struct data{
  int num1;
  int num2;
  int num3;
};

int main(int argc, char** argv)
{
   data my_data;
   my_data.num1 = 64;
   my_data.num2 = 65;
   my_data.num3 = 66;

   char buffer[20];
   memcpy(buffer, &my_data, sizeof(data));

   data copy_data;
   memcpy(&copy_data, buffer, sizeof(data));

   printf("the numbers : %d - %d - %d \n", copy_data.num1, copy_data.num2, copy_data.num3);

    std::cout << copy_data.num1 << std::endl;

   return 0;
}