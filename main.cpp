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
    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    if( rc != SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }else{
      fprintf(stdout, "User successfully added\n");
    }    
    sqlite3_close(db);
    }
     
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



/*int loginMenu(){
    cout<< "Enter 1 to write message 2 to view message and 3 to go back" << endl;
    int rwb = 0;
    cin >> rwb;
    if(rwb == 1){
        writeMessage();
    } else if(rwb == 2){
        readMessages();
    } else{
        return 1;
    }
}*/


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
               //loginMenu();
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