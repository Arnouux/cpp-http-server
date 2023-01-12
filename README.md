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

<code>
gcc trie_t.c -c

g++ server.cpp trie_t.o -o server.exe -lwsock32
</code>

Place the executable on your site folder and launch it, or use it as the proxy to connect to your application.

Run `server.exe <folder> <service_type>`.