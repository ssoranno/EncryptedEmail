#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <cstdlib>
#include <stdio.h>
#include <sqlite3.h>
#include <gcrypt.h>
#include <fstream>
#include <bits/stdc++.h>

using namespace std;

int messNum = 0;
string username = "";
string password = "";
string messagePassword = "";

static int selectMessage(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    string nonce = argv[1];
    string message = argv[0];
    messNum +=1;
    string sender = argv[2];
    string messNum_s = to_string(messNum);
    cout << "<form method=\"POST\" action=\"decryptMessage.cgi\" id=\"decrypt\">\n";
    cout<< "<Label for=viewMessage>Message "+messNum_s+" from "+sender+"</Label>\n";
    //cout << "<input type=\"text\" name=\"messPass\" id=\"messPass\" placeholder=\"Enter Message Password\"/>\n";
    cout << "<input type=\"submit\" value=\"View Message\" name=\"viewMessage\" />\n";
    cout<< "<input type=\"hidden\" name=\"username\" id=\"username\" value="+username+">\n";
    cout<< "<input type=\"hidden\" name=\"password\" id=\"password\" value="+password+">\n";
    cout<< "<input type=\"hidden\" name=\"message\" id=\"message\" value="+message+">\n";
    cout<< "<input type=\"hidden\" name=\"nonce\" id=\"nonce\" value="+nonce+">\n";
    cout<< "<input type=\"hidden\" name=\"messNum\" id=\"messNum\" value="+messNum_s+"/0"+" />\n";
    cout << "</form>\n";
    //cout<< "Enter password to decrypt message from "<< sender << endl;
    //string password;
    //cin >> password;
    /*for(i=0; i<argc; i++){
        printf("%s %d = %s\n", azColName[i], i+1, argv[i] ? argv[i] : "NULL");
        
        cout<< "argv:"<< argv[i]<< endl;
        cout<< "argv2:" << argv[i+1] << endl;
        //string decryptedMessage = encryptMessage(argv[i]);
        //cout<< decryptedMessage << endl;
    }*/
    //cout<< endl;
    //string decryptedMessage = encryptMessage(message,nonce,password);
    //cout<< decryptedMessage << endl;
    return 0;
}

int readMessages(string username){
    sqlite3* db;
    char *zErrMsg = 0;
    //bool rc;
    //bool exists;
    int rc;
    rc = sqlite3_open("mail.db", &db);
    if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    } else{
    string sql_s = "SELECT MESSAGE, NONCE, SENDER FROM "+username+";";
    const char *sql = sql_s.c_str();
    //cout<< "h2"<< endl;
    //string temp;
    //bool *exists = new bool;
    rc = sqlite3_exec(db, sql, selectMessage, 0, &zErrMsg);
    //cout<< "h3" << endl;
    if( rc != SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
      //exists = false;
    }/*else{
      //fprintf(stdout, "Username exists\n");
      //exists = true;
    }*/
    sqlite3_close(db);
    }
    return 0;
}

const char HEX2DEC[256] = 
{
    /*       0  1  2  3   4  5  6  7   8  9  A  B   C  D  E  F */
    /* 0 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* 1 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* 2 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* 3 */  0, 1, 2, 3,  4, 5, 6, 7,  8, 9,-1,-1, -1,-1,-1,-1,
    
    /* 4 */ -1,10,11,12, 13,14,15,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* 5 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* 6 */ -1,10,11,12, 13,14,15,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* 7 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    
    /* 8 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* 9 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* A */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* B */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    
    /* C */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* D */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* E */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* F */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1
};


string UriDecode(const std::string & sSrc)
{
   // Note from RFC1630: "Sequences which start with a percent
   // sign but are not followed by two hexadecimal characters
   // (0-9, A-F) are reserved for future extension"
 
   const unsigned char * pSrc = (const unsigned char *)sSrc.c_str();
   const int SRC_LEN = sSrc.length();
   const unsigned char * const SRC_END = pSrc + SRC_LEN;
   // last decodable '%' 
   const unsigned char * const SRC_LAST_DEC = SRC_END - 2;
 
   char * const pStart = new char[SRC_LEN];
   char * pEnd = pStart;
 
   while (pSrc < SRC_LAST_DEC)
   {
      if (*pSrc == '%')
      {
         char dec1, dec2;
         if (-1 != (dec1 = HEX2DEC[*(pSrc + 1)])
            && -1 != (dec2 = HEX2DEC[*(pSrc + 2)]))
         {
            *pEnd++ = (dec1 << 4) + dec2;
            pSrc += 3;
            continue;
         }
      }
 
      *pEnd++ = *pSrc++;
   }
 
   // the last 2- chars
   while (pSrc < SRC_END)
      *pEnd++ = *pSrc++;
 
   std::string sResult(pStart, pEnd);
   delete [] pStart;
   return sResult;
}

string replacePlus(string s){
   for(int i =0; i<s.length(); i++){
      if(s[i] == '+'){
         s[i] = ' ';
      }
   }
   return s;
}

int main () {
    
   cout << "Content-type:text/html\r\n\r\n";
   cout << "<html>\n";
   cout << "<head>\n";
   cout << "<title>Encrypted Email</title>\n";
   cout << "</head>\n";
   cout << "<body>\n";
   
   char query[1024];
   int len, startNumberLocation, endNumberLocation;
   string data;
   char strnum[20];
   messNum = 0;
   if(getenv("CONTENT_LENGTH"))
   {
      len = atoi(getenv("CONTENT_LENGTH"));
      //cout<<"<p>"<<len<<"</p>\n";
      cin.read(query, len);
      data = replacePlus(UriDecode(query));
      //cout<<replacePlus(data) <<"\n";
      
      startNumberLocation = data.find("username")+9;
      endNumberLocation = data.find("&password");
      //cout<<"<p>"<< startNumberLocation <<" " <<endNumberLocation <<"</p>\n";
      username = data.substr(startNumberLocation,endNumberLocation-startNumberLocation);
      //cout<<"<p>"<< username << "</p>\n";
      
      startNumberLocation = endNumberLocation+10;
      endNumberLocation = data.find("&getMessages");
      //cout<<"<p>"<< startNumberLocation <<" " <<endNumberLocation <<"</p>\n";
      password = data.substr(startNumberLocation,endNumberLocation-startNumberLocation);
      //cout<<"<p>"<< password << "</p>\n";
      
   }
   
   readMessages(username);
   
   cout << "<form method=\"POST\" action=\"webLogic.cgi\" id=\"goBack\">\n";
   cout<< "<input type=\"hidden\" name=\"inputUsername\" id=\"username\" value="+username+">\n";
   cout<< "<input type=\"hidden\" name=\"inputPassword\" id=\"password\" value="+password+">\n";
   cout << "<input type=\"submit\" name=\"Login\" value=\"Back\"/>\n";
   cout<< "</form>\n";
   
   return 0;
}