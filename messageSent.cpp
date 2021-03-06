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

string *salt = new string;

string encryptMessage(string message, string nonce, string password);

static int callback3(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    for(i=0; i<argc; i++){
        
        *salt = argv[i];
    }
    
    cout<< endl;
    return 0;
}

static int selectMessage(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    string nonce = argv[1];
    string message = argv[0];
    string sender = argv[2];
    cout<< "Enter password to decrypt message from "<< sender << endl;
    string password;
    cin >> password;
    cout<< endl;
    string decryptedMessage = encryptMessage(message,nonce,password);
    cout<< decryptedMessage << endl;
    return 0;
}

static int storeEncrypted(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    for(i=0; i<argc; i++){
        printf("%s %d = %s\n", azColName[i], i+1, argv[i] ? argv[i] : "NULL");
    }
    cout<< endl;
    return 0;
}

string getSalt(string username){
    sqlite3* db;
    char *zErrMsg = 0;
    
    int rc;
    rc = sqlite3_open("mail.db", &db);
    if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    } else{
        string sql_s = "SELECT salt from users where name='"+username+"';";
        const char *sql = sql_s.c_str();
        rc = sqlite3_exec(db, sql, callback3, 0, &zErrMsg);
        if( rc != SQLITE_OK ){
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }
        sqlite3_close(db);
    }
    return *salt;
}

char *xorMessage(char *encBuffer, char * message, int length){
    size_t index;
    size_t txtLength = length;
    
    char *encryptedMess = new char;
    for (index = 0; index<txtLength; index++){
        
        encryptedMess[index] = (unsigned char)encBuffer[index] ^ (unsigned char)message[index];
        
    }
    
    return encryptedMess;
}

string encryptMessage(string message, string nonce, string password) {
    gcry_error_t     gcryError;
    gcry_cipher_hd_t gcryCipherHd;
    size_t           index;
    //string message = "To bob:\nHello Poop Head\nFrom: Steven";
    //cin >> message;
    int textSize = message.length();
    //cout << textSize << endl;
    //int textSize;
    //cin >> textSize;
    //cout<< textSize << endl;
    string randvalue = nonce;
    char salsaKey[33] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"; // 32 bytes
    char iniVector[9]; // 8 bytes
    strncpy(iniVector,randvalue.c_str(),9);
    //strncpy(salsaKey,password.c_str(),password.length());
    //cout<< iniVector<< endl;
    //cout<< "<p>Password in Encrypt"+password+"</p>\n";

    gcryError = gcry_cipher_open(
        &gcryCipherHd, // gcry_cipher_hd_t *
        GCRY_CIPHER_SALSA20,   // int
        GCRY_CIPHER_MODE_STREAM,   // int
        0);            // unsigned int
    if (gcryError)
    {
        /*printf("gcry_cipher_open failed:  %s/%s\n",
               gcry_strsource(gcryError),
               gcry_strerror(gcryError));
        */return 0;
    }
    //printf("gcry_cipher_open worked\n");
    
    gcryError = gcry_cipher_setkey(gcryCipherHd, salsaKey, 32);
    if (gcryError)
    {
        /*printf("gcry_cipher_setkey failed:  %s/%s\n",
               gcry_strsource(gcryError),
               gcry_strerror(gcryError));
        */return 0;
    }
    //printf("gcry_cipher_setkey worked\n");
    
    gcryError = gcry_cipher_setiv(gcryCipherHd, iniVector, 8);
    if (gcryError)
    {
        /*printf("gcry_cipher_setiv failed:  %s/%s\n",
               gcry_strsource(gcryError),
               gcry_strerror(gcryError));
        */return 0;
    }
    //printf("gcry_cipher_setiv worked\n");
    
    size_t txtLength = textSize;
    char * encBuffer = new char[txtLength];
    char * textBuffer = new char[txtLength];
    memset(textBuffer, 0, txtLength);

    gcryError = gcry_cipher_encrypt(
        gcryCipherHd, // gcry_cipher_hd_t
        encBuffer,    // void *
        txtLength,    // size_t
        textBuffer,    // const void *
        txtLength);   // size_t
    if (gcryError)
    {
        /*printf("gcry_cipher_decrypt failed:  %s/%s\n",
               gcry_strsource(gcryError),
               gcry_strerror(gcryError));
        */return 0;
    }
    //printf("gcry_cipher_decrypt worked\n");
    
    
    string RandChars = "";
    for (index = 0; index<txtLength; index++){
        char * temp = new char;
        sprintf(temp,"%02X",(unsigned char)encBuffer[index]);
        RandChars = RandChars+ temp;
    }
    //printf("\n");
    //cout<< "encBuffer = " << RandChars << endl;
    //cout<< message.length() << endl;
    char cMessage[message.length()];
    strncpy(cMessage,message.c_str(),message.length());
    //cout<< "cMessage:" <<cMessage << endl;
    
    char *encrypted = xorMessage(encBuffer,cMessage,message.length());
    
    // To decrypt:
    /*char *decrypted = xorMessage(encBuffer,encrypted,message.length());
    //cout<< hex << decrypted << endl;
    cout<<"<p>decrypted:";
    cout<< decrypted;
    cout<< "</p>\n";*/
    
    
    return encrypted;
}

void storeMessageInDB(string message, string receiver, string nonce, string sender){
    sqlite3* db;
    char *zErrMsg = 0;
    
    int rc;
    rc = sqlite3_open("mail.db", &db);
    if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    } else{

        string sql_s = "INSERT INTO "+receiver+" (MESSAGE, NONCE, SENDER) VALUES('"+message+"', '"+nonce+"', '"+sender+"');";
        
        const char *sql = sql_s.c_str();
        
        rc = sqlite3_exec(db, sql, storeEncrypted, 0, &zErrMsg);
        
        if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
          sqlite3_free(zErrMsg);
          cout<< "<p>Error:";
          cout<< zErrMsg;
          cout<< "</p>\n";
          //exists = false;
        }else{
            //cout<< "<p>Did it!</p>\n";
          //fprintf(stdout, "Username exists\n");
          //exists = true;
        }
    sqlite3_close(db);
    }
}


string getRandom(){
    unsigned int random_value = 0; //Declare value to store data into
    size_t size = sizeof(random_value); //Declare size of data
    ifstream urandom("/dev/urandom", ios::in|ios::binary); //Open stream
    if(urandom) //Check if stream is open
    {
        urandom.read(reinterpret_cast<char*>(&random_value), size); //Read from urandom
        if(urandom) //Check if stream is ok, read succeeded
        {
            //std::cout << "Read random value: " << random_value << std::endl;
        }
        else //Read failed
        {
            std::cerr << "Failed to read from /dev/urandom" << std::endl;
        }
        urandom.close(); //close stream
    }
    else //Open failed
    {
        std::cerr << "Failed to open /dev/urandom" << std::endl;
    }
    stringstream ss;
    ss << hex << random_value;
    //cout<< ss.str()<< endl;
    //string randval = std::to_string(random_value);
    string randval = ss.str();
    return randval;
}


int readMessages(string username, int messID){
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

        string id = to_string(messID);
        string sql_s = "SELECT MESSAGE, NONCE FROM "+username+" where id="+id+";";
    
        const char *sql = sql_s.c_str();
        
        rc = sqlite3_exec(db, sql, selectMessage, 0, &zErrMsg);
        
        if( rc != SQLITE_OK ){
        fprintf(stdout, "SQL error: %s\n", zErrMsg);
            //printf()
            //cout<<"<p>"+*zErrMsg+"</p>\n";
          sqlite3_free(zErrMsg);
          //exists = false;
        }else{
          //fprintf(stdout, "Username exists\n");
          //exists = true;
        }
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
   string message;
   string receiver;
   string messPassword = "none";
   string username;
   string password;
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
      endNumberLocation = data.find("&receiver");
      //cout<<"<p>"<< startNumberLocation <<" " <<endNumberLocation <<"</p>\n";
      password = data.substr(startNumberLocation,endNumberLocation-startNumberLocation);
      //cout<<"<p>"<< password << "</p>\n";
      
      startNumberLocation = endNumberLocation+10;
      endNumberLocation = data.find("&message");
      //cout<<"<p>"<< startNumberLocation <<" " <<endNumberLocation <<"</p>\n";
      receiver = data.substr(startNumberLocation,endNumberLocation-startNumberLocation);
      //cout<<"<p>"<< receiver << "</p>\n";
      
      startNumberLocation = endNumberLocation+9;
      endNumberLocation = data.find("&send");
      //cout<<"<p>"<< startNumberLocation <<" " <<endNumberLocation <<"</p>\n";
      message = data.substr(startNumberLocation,endNumberLocation-startNumberLocation);
      //cout<<"<p>"<< message << "</p>\n";
      
   }
   
   string nonce = getRandom();
   string encrypted= encryptMessage(message, nonce,messPassword);
   
   storeMessageInDB(encrypted,receiver,nonce,username);
   
   cout<<"<p>Message Sent.</p>\n";
   
   cout << "<form method=\"POST\" action=\"webLogic.cgi\" id=\"goBack\">\n";
   cout<< "<input type=\"hidden\" name=\"inputUsername\" id=\"username\" value="+username+">\n";
   cout<< "<input type=\"hidden\" name=\"inputPassword\" id=\"password\" value="+password+">\n";
   cout << "<input type=\"submit\" name=\"Login\" value=\"Back\"/>\n";
   cout<< "</form>\n";
   
   return 0;
}