The goal is to build a functional http server in steps.

1) Minspec - Single threaded - blocking on IO

2) Thread pool - threads block on IO

3) Thread pool - threads don't block on IO.
<br>
<br>

Probably I will leave it there, I have no desire to implement things like:
- HTTPS with SSL/TLS
- CGI support
- Virtual hosts
- Authentication

Resources:
- https://pages.cs.wisc.edu/~remzi/OSTEP/
- https://www.jmarshall.com/easy/http/#sample