# Configuration and Logging

Ys supports an optional config file, `ys.conf`, which you can use to tune different aspects of your server.

Suppose we create a `ys.conf`. It should be in the root directory of the application using Ys.

```
NUM_THREADS=4
PORT=8000
LOG_LEVEL=debug
LOG_FILE=server.log
```

We've specified that the server should use a thread pool with 4 threads. It will listen on port 8000. Further, it will print logs up to and including the `debug` log-level, and will write said logs to a log file `server.log`.

If we omit the log file, the log messages will be printed to stderr.

For logging, there are three log-levels: `info`, `debug`, and `verbose` (in order of verbosity). By default, Ys will use `info`.
