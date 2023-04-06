# Config

## Config Format

* The config file must be in the root directory of your application.
* The config file must be named `ys.conf`
* Each line in the config file may not exceed 256 characters
* If not present, or if missing configuration keys, defaults will be used.

```sh
NUM_THREADS=4
PORT=8000
LOG_LEVEL=debug
LOG_FILE=server.log
```

## Options

|Key|Type|Purpose|Default|
|-|-|-|-|
|`NUM_THREADS`|number|Configures the number of threads to use in the thread pool. Threads are used to handle client connections.|4|
|`PORT`|number|Sets the port number on which the server listens. If a port number is passed to `server_init`, it will override the config value.|5000|
|`LOG_LEVEL`|string|The maximum level of log messages that will be displayed. TODO:|"info"|
|`LOG_FILE`|string|A file path where logs will be written. If this value is not set, logs will be printed to stderr|null|
