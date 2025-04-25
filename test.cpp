#include <iostream>
#include <cstring>
#include <vector>
#include <algorithm>


int main() {
     char buffer[256] = {0};
     char sat[3] = {'s','a'};
     strcpy(buffer,"haii");
     int num = 2;
     strcat(buffer, sat);
     std::cout<<buffer<<std::endl;
    return 0;
}
