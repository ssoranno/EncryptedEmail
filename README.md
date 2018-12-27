# EncryptedEmail

This program allows users to register accounts with a username and password, login to their accounts with their password.  Once logged in, the user can view a list of all the other users on the platform that he or she can send messages to.  Finally, the program allows users to send and recieve encrypted messages.

The program is written in C++ and uses CGI and apache to allow the client to communicate to the server using with the https protocol.

## How to Run:

Compile all .cpp file on a your local host or a cloud server.  To compile use the command:<br>
g++ "filename" -lgcrypt -lsqlite3

Navigate to https://"servername".com/index.cgi to run the program in brower.  "servername" is to be replaced with the server or local host name.
