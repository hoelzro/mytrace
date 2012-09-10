TODO
====

  * Actually execute a different command from test.pl
  * Error checking (waitpid, malloc, etc)
  * Add logging (for when a connection is no longer considered because it's "not MySQL", etc)
  * Detect when a MySQL connection is closed and cleanup
  * Add filter rules for connect() to certain sockets (most servers listen on 3306, for example)
  * Check for recv()/send() calls in addition to read()/write()
    * This is low-priority, since libmysqlclient only seems to use read()/write()
  * Detect SSL connection; complain if found
  * Implement interception of compressed MySQL wire protocol
  * Try to do without intermediary buffers when parsing the MySQL wire protocol
  * Verify that authentication succeeds when keeping a connection around
  * Support attaching to an existing process (tricky)
  * Support parsing handshake/command if we haven't received the whole packet yet
    * Low-priority; libmysqlclient doesn't do this
  * We have no bounds checks =(
  * Break syscall\_info implementation into separate, arch-specific files
  * Add syscall\_info implementation for non x86-64
  * Make sure we can trace MySQL queries submitted by child processes
