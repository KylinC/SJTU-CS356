# Problem 4:
## Description：

Caesar cipher, is one of the simplest and most widely known encryption techniques. During encryption, each letter in the plaintext is replaced by a letter some fixed number of positions down the alphabet. In this problem, we set the number=3.

- For example:
  - Plain: ABCDEFGHIJKLMNOPQRSTUVWXYZ
  - Cipher: DEFGHIJKLMNOPQRSTUVWXYZABC
  - Plain: abcdefghijklmnopqrstuvwxyz
  - Cipher: defghijklmnopqrstuvwxyzabc

## Requirement:

- Please develop a Caesar Encryption Server, which receives plaintext from clients and sends the corresponding ciphertext to clients.
- The Server can serve at most 2 clients concurrently, more clients coming have to wait.
- The server-side program must be concurrent multi-threaded.
- Client input :q to end the service.
- For simplicity, you can execute one server and multiple clients in one host.

## Tips:

For Linux Server Programming, you can refer to:

- [http://www.yolinux.com/TUTORIALS/LinuxTutorialPosixThreads.html](http://www.yolinux.com/TUTORIALS/LinuxTutorialPosixThreads.html)

- [http://www.linuxhowtos.org/C_C++/socket.htm](http://www.linuxhowtos.org/C_C++/socket.htm)

## Pictures:

![prob1](https://ws2.sinaimg.cn/large/006tNc79ly1g29idd8l9vj30zj0qeqcr.jpg)

## Files:

After building the project, we can get module name **Server** and **Client**, how to upload it can refer to [my blog](<http://kylinchen.top/2019/04/06/Ubuntu-Android_init/>).