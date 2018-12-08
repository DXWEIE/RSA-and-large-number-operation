# largenumber
RSA and large numer operation library write in c/c++
===

1.rsa.cpp is the main part, and it relys on bignum.h<br>

2.The code style and some parts are referenced from the book- Cryptography in C and C++ Second Edition<br>

3.The efficiency is not so good, but if you compile it use the optimization in VS, it seems so good!<br>

4.If you want to test rsa.cpp,you need a file called 'plain.txt' in your current filefolder. And to avoid errors, the content of plain.txt is suggested to be a SHA-1 result of another file.<br>
