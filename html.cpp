#include <iostream>
#include <stdlib.h>
using namespace std;

int main () {
    
   cout << "Content-type:text/html\r\n\r\n";
   cout << "<html>\n";
   cout << "<head>\n";
   cout << "<title>Encrypted Email</title>\n";
   cout << "</head>\n";
   cout << "<body>\n";
   cout<< "<h1>Login</h1>\n";
   cout << "<form method=\"POST\" action=\"webLogic.cgi\" id=\"credentials\">\n";
   cout << "<div>\n";
   cout << "<Label for=\"inputUsername\">Username</label>\n";
   cout << "<input type=\"text\" name=\"inputUsername\" id=\"inputUsername\" placeholder=\"Enter Username\"/>\n";
   cout << "</div>\n";
   cout << "<div>\n";
   cout << "<Label for=\"inputPassword\">Password</label>\n";
   cout << "<input type=\"password\" id=\"inputPassword\" name=\"inputPassword\" placeholder=\"Enter Password\"/>\n";
   cout << "</div>\n";
   cout << "<input type=\"submit\" value=\"Login\" name=\"Login\"/>\n";
   cout << "<input type=\"submit\" value=\"Register\" name=\"Register\"/>\n";
   cout << "</form>\n";
  
   cout << "</body>\n";
   cout << "</html>\n";
   
   
   return 0;
}