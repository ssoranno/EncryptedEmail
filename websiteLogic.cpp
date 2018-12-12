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

bool *exists = new bool;
string *salt = new string;
string *passwordHash = new string;

string encryptMessage(string message, string nonce, string password);

// Check if username exists function
static int callback2(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    for(i=0; i<argc; i++){
        if(argv[i]!= "NULL"){
            *exists = true;
        }
    }
    
    return 0;
}

// Get salt callback function
static int callback3(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    for(i=0; i<argc; i++){
        *salt = argv[i];
    }
    return 0;
}

// Check Password sql callback
static int callback5(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    for(i=0; i<argc; i++){
        *passwordHash = argv[i];
    }
    cout<< endl;
    return 0;
}

// Create table callback
static int callback(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    for(i=0; i<argc; i++){
        
    }
    cout<< endl;
    return 0;
}

static int createUserTable(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    for(i =0; i<argc; i++){
        //printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
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

static int getUsersSQL(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    cout<<"<p>";
    cout<<argv[0];
    cout<< "</p>\n";
    return 0;
}

static int storeEncrypted(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    for(i=0; i<argc; i++){
        //printf("%s %d = %s\n", azColName[i], i+1, argv[i] ? argv[i] : "NULL");
    }
    cout<< endl;
    return 0;
}

string createHash(string name, string password){
    size_t index;
    string tempSalt = *salt;
    size_t txtLength = password.length()+tempSalt.length();
    char * hashBuffer = new char[33];
    char * textBuffer = new char[txtLength+1];
    memset(hashBuffer, 0, 33);
    string pass_s = password+*salt;
    const char *passsalt = pass_s.c_str();
    strncpy(textBuffer, passsalt, txtLength);

    gcry_md_hash_buffer(
        GCRY_MD_SHA256, // gcry_cipher_hd_t
        hashBuffer,    // void *
        textBuffer,    // const void *
        txtLength);   // size_t

    //printf("hashBuffer = ");
    char * HB2 = new char[33]; 
    string HB3 = "";
    for (index = 0; index<32; index++){
        char * temp = new char;
        //printf("%02X", (unsigned char)hashBuffer[index]);
        sprintf(temp,"%02X",(unsigned char)hashBuffer[index]);
        HB3 = HB3+ temp;
    }
    
    return HB3;
}

void createTable(string username){
    sqlite3* db;
    char *zErrMsg = 0;
    int rc;
    
    rc = sqlite3_open("mail.db", &db);
    if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    } else{

    string sql_s = "CREATE TABLE "+username+"(ID integer primary key autoincrement, MESSAGE blob, NONCE text, SENDER text)";
    const char *sql = sql_s.c_str();
    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }else{
      fprintf(stdout, "Message table successfully created\n");
    }    
    sqlite3_close(db);
    }
}

void storeInfo(string name, string password, string salt2){
    
    string hashBuff = createHash(name, password);

    sqlite3* db;
    char *zErrMsg = 0;
    int rc;
    
    rc = sqlite3_open("mail.db", &db);
    if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    } else{

    string sql_s = "INSERT INTO users (NAME, HASH, SALT) VALUES('"+name+"', '"+hashBuff+"', '"+salt2+"');";
    const char *sql = sql_s.c_str();
    rc = sqlite3_exec(db, sql, createUserTable, 0, &zErrMsg);
    if( rc != SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }else{
      fprintf(stdout, "User successfully added\n");
    }    
    sqlite3_close(db);
    }
    createTable(name);
}

bool checkUsernameExists(string username){
    
    sqlite3* db;
    char *zErrMsg = 0;
    int rc;
    rc = sqlite3_open("mail.db", &db);
    if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    } else{

    string sql_s = "SELECT name FROM users where name='"+username+"';";
    const char *sql = sql_s.c_str();
    rc = sqlite3_exec(db, sql, callback2, 0, &zErrMsg);
    
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
    bool temp = *exists;
    *exists = false;
    return temp;
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

bool checkPassword(string username, string password){
    sqlite3* db;
    char *zErrMsg = 0;
    bool rc;
    *salt = getSalt(username);
    string expHash = createHash(username, password);
    rc = sqlite3_open("mail.db", &db);
    if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    } else{
    
        string sql_s = "SELECT hash FROM users where name='"+username+"';";
        const char *sql = sql_s.c_str();
        rc = sqlite3_exec(db, sql, callback5, 0, &zErrMsg);
        if( rc != SQLITE_OK ){
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }
        sqlite3_close(db);
    }
    if(expHash == *passwordHash){
        return true;
    } else{
        return false;
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
    string randval = ss.str();
    return randval;
}

int readMessages(string username){
    sqlite3* db;
    char *zErrMsg = 0;
    int rc;
    rc = sqlite3_open("mail.db", &db);
    if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    } else{
    string sql_s = "SELECT MESSAGE, NONCE, SENDER FROM "+username+";";
    const char *sql = sql_s.c_str();
    rc = sqlite3_exec(db, sql, selectMessage, 0, &zErrMsg);
    
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

char *xorMessage(char *encBuffer, char * message, int length){
    size_t index;
    size_t txtLength = length;
    string RandChars = "";
    
    char *encryptedMess = new char;
    for (index = 0; index<txtLength; index++){
        
        encryptedMess[index] = (unsigned char)encBuffer[index] ^ (unsigned char)message[index];
        
    }
    
    for (index = 0; index<txtLength; index++){
        char * temp = new char;
        sprintf(temp,"%02X",(unsigned char)encryptedMess[index]);
        RandChars = RandChars+ temp;
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
    char salsaKey[password.length()];// = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"; // 32 bytes
    char iniVector[9]; // 8 bytes
    strncpy(iniVector,randvalue.c_str(),9);
    strncpy(salsaKey,password.c_str(),password.length());
    //cout<< iniVector<< endl;

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
    cout<< "encBuffer = " << RandChars << endl;
    cout<< message.length() << endl;
    char cMessage[message.length()];
    strncpy(cMessage,message.c_str(),message.length());
    cout<< cMessage << endl;
    
    char *encrypted = xorMessage(encBuffer,cMessage,message.length());
    cout<<"encrypted:" <<hex << encrypted << endl;
    
    // To decrypt:
    /*char *decrypted = xorMessage(encBuffer,encrypted,message.length());
    //cout<< hex << decrypted << endl;
    cout<<"decrypted:" <<decrypted << endl;
    */
    
    return encrypted;
}

void printUsers(){
    sqlite3* db;
    char *zErrMsg = 0;
    
    int rc;
    rc = sqlite3_open("mail.db", &db);
    if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    } else{

    string sql_s = "SELECT name FROM users";
    const char *sql = sql_s.c_str();

    rc = sqlite3_exec(db, sql, getUsersSQL, 0, &zErrMsg);

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
      //exists = false;
    }/*else{
      //fprintf(stdout, "Username exists\n");
      //exists = true;
    }*/
    sqlite3_close(db);
    }
}

/*int writeMessage(string username){
    printUsers();
    cout<< "Enter name of user to send message to." << endl;
    string receiver;
    cin >> receiver;
    cout<< "Enter Message with no new line characters:" << endl;
    string message;
    cin >> message;
    string password;
    cout<< "Enter password to encrypt message" << endl;
    cin >> password;
    string nonce = getRandom();
    string encrypted= encryptMessage(message, nonce,password);
    storeMessageInDB(encrypted,receiver,nonce,username);
    
}

int loginMenu(string username){
    cout<< "Enter 1 to write message 2 to view message and 3 to go back" << endl;
    int rwb = 0;
    cin >> rwb;
    if(rwb == 1){
        writeMessage(username);
    } else if(rwb == 2){
        readMessages(username);
    } else{
        return 1;
    }
}*/

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
   string username;
   string password;
   string regOrLogin;
   string message;
   string receiver;
   if(getenv("CONTENT_LENGTH"))
   {
      len = atoi(getenv("CONTENT_LENGTH"));
      //cout<<"<p>"<<len<<"</p>\n";
      cin.read(query, len);
      data = replacePlus(UriDecode(query));
      
      startNumberLocation = data.find("inputUsername")+14;
      endNumberLocation = data.find("&input");
      //cout<<"<p>"<< startNumberLocation <<" " <<endNumberLocation <<"</p>\n";
      username = data.substr(startNumberLocation,endNumberLocation-startNumberLocation);
      //cout<<"<p>"<< username << "</p>\n";
      startNumberLocation = endNumberLocation+15;
      //cout<<"<p>"<< data.find("&stuff") << "</p>\n";
      endNumberLocation = len;
      password = data.substr(startNumberLocation,endNumberLocation-startNumberLocation);
      //cout<<"<p>p1:"<< password << "</p>\n";
      string password2 = password;
      startNumberLocation = 0;
      //cout<<"<p>"<< data.find("&stuff") << "</p>\n";
      endNumberLocation = password.find("&");
      password = password.substr(startNumberLocation,endNumberLocation-startNumberLocation);
      //cout<<"<p>p2:"<< password << "</p>\n";
      startNumberLocation = password2.find("&")+1;
      endNumberLocation = password2.length();
      regOrLogin = password2.substr(startNumberLocation,endNumberLocation-startNumberLocation);
      //cout<<"<p>r1:"<< regOrLogin << "</p>\n";
      startNumberLocation = 0;
      endNumberLocation = regOrLogin.find("=");
      regOrLogin = regOrLogin.substr(startNumberLocation,endNumberLocation-startNumberLocation);
      //cout<<"<p>r2:"<< regOrLogin << "</p>\n";
      //startNumberLocation = data.find("message")+8;
      //endNumberLocation = data.find("");
   }
   
   if(username.find("'") != std::string::npos || username.find(";") != std::string::npos 
      || password.find("'") != std::string::npos || password.find(";") != std::string::npos){
         cout<< "<p>Please do not use character ' or ;</p>\n";
         cout << "<form method=\"POST\" action=\"index.cgi\" id=\"credentials\">\n";
         cout << "<input type=\"submit\" name=\"inputUsername\" value=\"Back\"/>\n";
   } else{
   
      
      if(regOrLogin.compare("Register")== 0){
         
         *salt = getRandom();
         bool usernameExists;
         usernameExists = checkUsernameExists(username);
         if(usernameExists){
           cout<< "<p>Username Already Exists enter new username</p>" << endl;
           cout << "<form method=\"POST\" action=\"index.cgi\" id=\"goBack3\">\n";
           cout << "<input type=\"submit\" name=\"inputUsername\" value=\"Back\"/>\n";
         } else{
             
            storeInfo(username, password,*salt);
            cout<< "<p>Registered Account</p>" << endl;
            cout << "<form method=\"POST\" action=\"index.cgi\" id=\"goBack4\">\n";
            cout << "<input type=\"submit\" name=\"inputUsername\" value=\"Back\"/>\n";
         }
      } else{
         bool usernameExist = checkUsernameExists(username);
         if(usernameExist){
             
              bool passwordCorrect = checkPassword(username,password);
              if(passwordCorrect){
                  cout<< "<h1>"<< username <<" Logged In</h1>" << endl;
                  cout<< "<p>Other Users:</p>\n";
                  printUsers();
                  cout<< "<p>Send Message:</p>\n";
                  cout<< "<div>\n";
                  cout<< "<form method=\"POST\" action=\"messageSent.cgi\" id=\"messageForm\">\n";
                  cout<< "<input type=\"hidden\" name=\"username\" id=\"username\" value="+username+">\n";
                  cout<< "<input type=\"hidden\" name=\"password\" id=\"password\" value="+password+">\n";
                  cout << "<input type=\"text\" name=\"receiver\" id=\"receiver\" placeholder=\"Enter Receiver\"/>\n";
                  cout<< "<input type=\"text\" name=\"message\" id=\"message\" placeholder=\"Enter Message\">\n";
                  //cout << "<input type=\"password\" name=\"messPassword\" id=\"messPassword\" placeholder=\"Enter Message Password\"/>\n";
                  cout<< "<input type=\"submit\" name=\"send\" value=\"Send Message\"/>\n";
                  cout<< "</form>\n";
                  cout<< "</div>\n";
                  cout << "<form method=\"POST\" action=\"printMessages.cgi\" id=\"goToMessages\">\n";
                  cout<< "<input type=\"hidden\" name=\"username\" id=\"username\" value="+username+">\n";
                  cout<< "<input type=\"hidden\" name=\"password\" id=\"password\" value="+password+">\n";
                  cout << "<input type=\"submit\" name=\"getMessages\" value=\"Get Messages\"/>\n";
                  cout<< "</form>\n";
                  cout << "<form method=\"POST\" action=\"index.cgi\" id=\"logoff\">\n";
                  cout << "<input type=\"submit\" name=\"inputUsername\" value=\"Logout\"/>\n";
                  cout<< "</form>\n";
                  
              } else{
                  cout<< "<p>Incorrect Username or password</p>" << endl;
                  cout << "<form method=\"POST\" action=\"index.cgi\" id=\"goBack1\">\n";
                  cout << "<input type=\"submit\" name=\"inputUsername\" value=\"Back\"/>\n";
              }
          }else{
               cout<< "<p>Incorrect Username or password</p>" << endl;
               cout << "<form method=\"POST\" action=\"index.cgi\" id=\"goBack2\">\n";
               cout << "<input type=\"submit\" name=\"inputUsername\" value=\"Back\"/>\n";
          }
      }
   }
   cout << "</body>\n";
   cout << "</html>\n";
   
   return 0;
}