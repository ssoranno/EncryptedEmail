// How to compile: g++ main.cpp -std=c++11 -lsqlite3 -lgcrypt -o main

#include <stdio.h>
#include <sqlite3.h>
#include <gcrypt.h>
#include <iostream>
#include <string>
#include <fstream>
#include <bits/stdc++.h>
using namespace std; 

bool *exists = new bool;
string *salt = new string;
string *passwordHash = new string;

string encryptMessage(string message, string nonce, string password);

static int callback2(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    //cout<< *exists << endl;
        //cout<< "argc:" <<argc << endl; 
    for(i=0; i<argc; i++){
        //printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        //cout<< argv[i] ? argv[i] : "NULL" << endl;
        if(argv[i]!= "NULL"){
            *exists = true;
        }
    }
    /*if(argc >0){
    *exists = true;
    } else {
         *exists = false;
    }*/
    
    return 0;
}

static int callback3(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    for(i=0; i<argc; i++){
        //printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        //cout<< argv[i] ? argv[i] : "NULL" << endl;
        //NotUsed = argv[i] ? argv[i] : "NULL";
        
        *salt = argv[i];
    }
    //cout << "salt:"<< salt<< endl;
    cout<< endl;
    return 0;
}

static int callback5(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    for(i=0; i<argc; i++){
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        //cout<< argv[i] ? argv[i] : "NULL" << endl;
        //NotUsed = argv[i] ? argv[i] : "NULL";
        *passwordHash = argv[i];
    }
    //cout << "hash:"<< passwordHash<< endl;
    cout<< endl;
    return 0;
}

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    for(i=0; i<argc; i++){
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        //cout<< argv[i] ? argv[i] : "NULL" << endl;
        //NotUsed = argv[i] ? argv[i] : "NULL";
        
    }
    cout<< endl;
    return 0;
}

static int createUserTable(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    for(i =0; i<argc; i++){
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
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
    /*for(i=0; i<argc; i++){
        printf("%s %d = %s\n", azColName[i], i+1, argv[i] ? argv[i] : "NULL");
        
        cout<< "argv:"<< argv[i]<< endl;
        cout<< "argv2:" << argv[i+1] << endl;
        //string decryptedMessage = encryptMessage(argv[i]);
        //cout<< decryptedMessage << endl;
    }*/
    cout<< endl;
    string decryptedMessage = encryptMessage(message,nonce,password);
    cout<< decryptedMessage << endl;
    return 0;
}

static int getUsersSQL(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    for(i=0; i<argc; i++){
        printf("%s %d = %s\n", azColName[i], i+1, argv[i] ? argv[i] : "NULL");
    }
    cout<< endl;
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

string createHash(string name, string password){
    //cout<< "h3" << endl;
    //cout<< *salt << endl;
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
    //printf("\n");
    cout<< "hash2:" << HB3 << endl;
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

    string sql_s = "CREATE TABLE "+username+"(ID integer primary key autoincrement, MESSAGE text, NONCE text, SENDER text)";
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

void storeInfo(string name, string password){
    
    /*size_t index;
    size_t txtLength = password.length()+salt.length();
    char * hashBuffer = new char[33];
    char * textBuffer = new char[txtLength+1];
    memset(hashBuffer, 0, 33);
    string pass_s = password+salt;
    const char *passsalt = pass_s.c_str();
    strncpy(textBuffer, passsalt, txtLength);

    gcry_md_hash_buffer(
        GCRY_MD_SHA256, // gcry_cipher_hd_t
        hashBuffer,    // void *
        textBuffer,    // const void *
        txtLength);   // size_t

    printf("hashBuffer = ");
    for (index = 0; index<32; index++)
        printf("%02X", (unsigned char)hashBuffer[index]);
    printf("\n");
    string hashBuff(hashBuffer);
    cout<< hashBuff << endl;*/
    
    string hashBuff = createHash(name, password);

    sqlite3* db;
    char *zErrMsg = 0;
    int rc;
    
    rc = sqlite3_open("mail.db", &db);
    if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    } else{

    string sql_s = "INSERT INTO users (NAME, HASH, SALT) VALUES('"+name+"', '"+hashBuff+"', '"+*salt+"');";
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
    //bool rc;
    //bool exists;
    int rc;
    rc = sqlite3_open("mail.db", &db);
    if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    } else{

    string sql_s = "SELECT name FROM users where name='"+username+"';";
    const char *sql = sql_s.c_str();
    //string temp;
    //bool *exists = new bool;
    rc = sqlite3_exec(db, sql, callback2, 0, &zErrMsg);
    //cout<< "rc:" <<rc << endl;
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
    //cout<< "Temp:" <<temp << endl;
    return temp;
}

string getSalt(string username){
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
    //cout<< "h1" << endl;
    string expHash = createHash(username, password);
    //cout<< "h2" << endl;
    rc = sqlite3_open("mail.db", &db);
    if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    } else{
    
        string sql_s = "SELECT hash FROM users where name='"+username+"';";
        const char *sql = sql_s.c_str();
        //cout<< "h5" << endl;
        rc = sqlite3_exec(db, sql, callback5, 0, &zErrMsg);
        //cout<< "h6" << endl;
        //cout<< "rc:" << rc<< endl;
        if( rc != SQLITE_OK ){
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }
        sqlite3_close(db);
    }
    //cout<< "h4" << endl;
    cout<<"expHash::" <<expHash<< endl;
    cout<<"passHash:::" <<*passwordHash<< endl;
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
            std::cout << "Read random value: " << random_value << std::endl;
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
    cout<< ss.str()<< endl;
    //string randval = std::to_string(random_value);
    string randval = ss.str();
    return randval;
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
    cout<< "h2"<< endl;
    //string temp;
    //bool *exists = new bool;
    rc = sqlite3_exec(db, sql, selectMessage, 0, &zErrMsg);
    cout<< "h3" << endl;
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
    //cout<< encBuffer << endl;
    char *encryptedMess = new char;
    for (index = 0; index<txtLength; index++){
        //char * temp = new char;
        //cout<< (unsigned char)encBuffer[index] << endl;
        //cout<< message[index] << endl;
        encryptedMess[index] = (unsigned char)encBuffer[index] ^ (unsigned char)message[index];
        //sprintf(temp,"%02X",(unsigned char)encBuffer[index]);
        //RandChars = RandChars+ temp;
    }
    //cout << encryptedMess << endl;
    //cout<< "encBuffer2 = " << RandChars << endl;
    //string RandChars = "";
    for (index = 0; index<txtLength; index++){
        char * temp = new char;
        sprintf(temp,"%02X",(unsigned char)encryptedMess[index]);
        RandChars = RandChars+ temp;
    }
    //printf("\n");
    //cout<< "encMessage = " << RandChars << endl;
    //char *encryptedMess = cMessage ^ encBuffer;
    //cout<< "encrypted Message: " << encryptedMess << endl;
    //return encryptedMess;
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
    //bool rc;
    //bool exists;
    int rc;
    rc = sqlite3_open("mail.db", &db);
    if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    } else{

    string sql_s = "SELECT name FROM users";
    const char *sql = sql_s.c_str();
    //string temp;
    //bool *exists = new bool;
    rc = sqlite3_exec(db, sql, getUsersSQL, 0, &zErrMsg);
    //cout<< "rc:" <<rc << endl;
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
    //bool rc;
    //bool exists;
    int rc;
    rc = sqlite3_open("mail.db", &db);
    if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    } else{

    string sql_s = "INSERT INTO "+receiver+" (MESSAGE, NONCE, SENDER) VALUES('"+message+"', '"+nonce+"', '"+sender+"');";
    const char *sql = sql_s.c_str();
    //string temp;
    //bool *exists = new bool;
    rc = sqlite3_exec(db, sql, storeEncrypted, 0, &zErrMsg);
    //cout<< "rc:" <<rc << endl;
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

int writeMessage(string username){
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
}


int main(){
    
    cout<< "enter 1 to login and 2 to register " << endl;
    int lorr;
    cin >> lorr;
    if(lorr == 2){
       string username;
       string password;
       *salt = getRandom();
       cout<< "Enter username:" << endl;
       cin >> username;
       while(username.find("'") != std::string::npos || username.find(";") != std::string::npos){
           cout<< "Please do not use character ' or ;" << endl;
           cin >> username;
       }
       bool usernameExists;
       usernameExists = checkUsernameExists(username);
       while(usernameExists){
           cout<< "Username Already Exits enter new username" << endl;
           cin >> username;
           usernameExists = checkUsernameExists(username); 
       }
       cout<< "Enter password:" << endl;
       cin >> password;
       while(password.find("'") != std::string::npos || password.find(";") != std::string::npos){
            cout<< "Please do not use character ' or ;" << endl;
            cin >> password;
       }
       storeInfo(username, password); 
    } else if(lorr == 1){
       string username;
       string password;
       //string salt = getSalt(username);
       cout<< "Enter username:"<< endl;
       cin >> username;
       while(username.find("'") != std::string::npos || username.find(";") != std::string::npos){
           cout<< "Please do not use character ' or ;" << endl;
           cin >> username;
       }
       bool usernameExist = checkUsernameExists(username);
       cout<< "Enter password:"<< endl;
       cin >> password;
       while(password.find("'") != std::string::npos || password.find(";") != std::string::npos){
           cout<< "Please do not use character ' or ;" << endl;
           cin >> password;
       }
       if(usernameExist){
           bool passwordCorrect = checkPassword(username,password);
           if(passwordCorrect){
               loginMenu(username);
               cout<< "Logged In!!" << endl;
           } else{
               cout<< "Incorrect Username or password" << endl;
           }
       }else{
           cout<< "Incorrect Username or password" << endl;
       }
    }
    /*string name;
    // TODO: hashing
    string password;
    string salt;  //urandom
    // TODO: learn urandom
    cout<< "username" << endl;
    cin >> name;
    cout<< "password:" << endl;
    cin >> password;
    cout<< name << " " << password << endl;
    salt = "1234";*/
    
     

    return 0;
};