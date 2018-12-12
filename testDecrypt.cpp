#include <stdio.h>
#include <sqlite3.h>
#include <gcrypt.h>
#include <iostream>
#include <string>
#include <fstream>
#include <bits/stdc++.h>
using namespace std;

char *xorMessage(char *encBuffer, char * message, int length){
    size_t index;
    size_t txtLength = length;
    //string RandChars = "";
    /*cout<<encBuffer << endl;
    cout<<"\n";
    cout<<message;
    cout<< "\n";*/
    char *encryptedMess = new char;
    for (index = 0; index<txtLength; index++){
        //char * temp = new char;
        //cout<< (unsigned char)encBuffer[index] << endl;
        //cout<< message[index] << endl;
        encryptedMess[index] = (unsigned char)encBuffer[index] ^ (unsigned char)message[index];
        //sprintf(temp,"%02X",(unsigned char)encBuffer[index]);
        //RandChars = RandChars+ temp;
    }
    //cout << "<p>Got Here</p>\n";
    //cout<< "encBuffer2 = " << RandChars << endl;
    //string RandChars = "";
    /*for (index = 0; index<txtLength; index++){
        char * temp = new char;
        sprintf(temp,"%02X",(unsigned char)encryptedMess[index]);
        RandChars = RandChars+ temp;
    }*/
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
    cout<< textSize << endl;
    string randvalue = nonce;
    char salsaKey[password.length()];// = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"; // 32 bytes
    char iniVector[9]; // 8 bytes
    strncpy(iniVector,randvalue.c_str(),9);
    strncpy(salsaKey,password.c_str(),password.length());
    cout<< iniVector<< endl;
    cout<<"Password in Encrypt: "<< password<< endl;

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
    cout<< "cMessage:" <<cMessage << endl;
    
    char *encrypted = xorMessage(encBuffer,cMessage,message.length());
    //char newEncrypt = *encrypted;
    string str(encrypted);
    cout<<"encrypted:" <<hex << encrypted << endl;
    
    // To decrypt:
    /*char *decrypted = xorMessage(encBuffer,encrypted,message.length());
    //cout<< hex << decrypted << endl;
    cout<<"decrypted:" <<decrypted << endl;
    */
    
    return encrypted;
}

static int selectMessage(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    cout<<"<p>Gothere222</p>"<< endl;
    string nonce = argv[1];
    string message = argv[0];
    cout<< "<p>"+nonce+"</p>\n";
    cout<< "<p>Messgae:"+message+"</p\n>";
    //string sender = argv[2];
    
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
    string decryptedMessage = encryptMessage(message,nonce,"pooping");
    cout<<"<p>decryptMessage:"+ decryptedMessage+ "</p>"<< endl;
    return 0;
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
        //int temp = stoi(messID);
        //string str(temp);
        //cout<<"<p>"+ temp+"</p>"<< endl;
        string id = to_string(messID);
    string sql_s = "SELECT MESSAGE, NONCE FROM "+username+" where id="+id+";";
    cout<< "<p>"+sql_s+"</p>\n";
    const char *sql = sql_s.c_str();
    cout<< "h2"<< endl;
    //string temp;
    //bool *exists = new bool;
    rc = sqlite3_exec(db, sql, selectMessage, 0, &zErrMsg);
    cout<< "h3" << endl;
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

int main(){
    readMessages("noah",1);
    string s = "sssssssssssssssssssss";
    cout<<s.length()<<endl;
    encryptMessage(s,"ele8f965","pooping");
    return 0;
}