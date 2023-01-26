## Customized c++ HTTP server

For a fast web server responding to simple http requests, featuring:
- all files serving (css,imgs)
- logging
- counting visits
- thread pool (todo)
- anti dos
- multi platform (todo)
  

For now made for windows (using wsock32)
### How to use

Compile with the commands using:

`gcc trie_t.c -c`

`g++ server.cpp whitelist.cpp trie_t.o -o server.exe -lwsock32`

Place the executable on your site folder and launch it, or use it as the proxy to connect to your application.

Run `server.exe <folder> <service_type>`.


### How whitelisting works
In `whitelist.lst`, 1 line per accepted url:
+ ./website/html/*
+ ./website/css/*
+ ./resources/**

The above `whitelist.lst` will accept requests such as `website/html/x.html`, `website/css/y.css`, but not `website/html/other-folder/x.html`. It accepts any url starting with `resources/`.