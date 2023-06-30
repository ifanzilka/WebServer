#ifndef WEBSERVHEADER
# define WEBSERVHEADER


#define PAGE_START "<!DOCTYPE html>\
<html>\
<head>\
<title>Welcome to ifanzilka WebServ!</title>\
<style>\
html { color-scheme: light dark; }\
body { width: 35em; margin: 0 auto;\
font-family: Tahoma, Verdana, Arial, sans-serif; }\
</style>\
</head>\
<body>\
<h1>Welcome to ifanzilka WebServ!</h1>\
<p>If you see this page, the  web server is successfully installed and \
working. Further configuration is required.</p>\
<p>For online documentation and support please refer to\
<a href=\"https://github.com/ifanzilka/WebServer/\">   github/ifanzilka/webserv</a>.<br/>\
<p><em>Thank you for using my WebServ.</em></p>\
</body>\
</html>"


#define PAGE_404 "<html>\
    <head><title>404 Not Found</title></head>\
    <body>\
    <center><h1>404 Not Found</h1></center>\
    <hr><center>WebServ/v1</center>\
    </body>\
    </html>"

#endif