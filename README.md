## Customized c++ HTTP server

For a fast web server responding to simple http requests, featuring:
- all files serving (css,imgs)
- logging
- counting visits
- anti dos (todo)
  

For now made for windows (using wsock32)
### How to use

Compile with the command `g++ server.cpp -o server.exe -lwsock32`, place the executable on your site folder and launch it.