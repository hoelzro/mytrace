mytrace - strace for MySQL
==========================

strace is a tool familiar to many Linux administrators and developers; I know
it's saved my skin many a time.  I wanted to see what queries a script I was
running was submitting to MySQL, and I wanted to know more about Linux's ptrace
facility, so I decided to play around and write this tool.

Goals
=====

As of the "1.0 release" of this tool, I would like it to:

  * be able to trace MySQL queries submitted via TCP and UNIX sockets
  * be able to trace across fork() invocations
  * be able to attach to an existing process and start tracing its MySQL queries
