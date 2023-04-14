# Platform Support

Ys currently supports UNIX-like environments and has been tested on the following operating systems

## Linux

### Arch Linux

Full support for usage + development.

### Ubuntu

Full support for usage.

For development, we need to fix the integration tests. If someone on Ubuntu wants to contribute, let me know and I will expedite this.

#### Everything you need

##### General Usage

```sh
apt update && apt install libpcre3-dev make gcc libssl-dev
```

##### Development

```sh
apt install curl
```

```sh
sh -c "`curl -L https://raw.githubusercontent.com/rylnd/shpec/master/install.sh`"
```

### Fedora

Full support for usage + development.

#### Everything you need

##### General Usage

```sh
yum install make gcc pcre-devel openssl-devel redhat-lsb-core
```

##### Development

```sh
yum install curl
```

```sh
sh -c "`curl -L https://raw.githubusercontent.com/rylnd/shpec/master/install.sh`"
```

### AL2 Linux

Partial support for usage (TLS support not working yet). Development support needs work.

#### Everything you need

##### General Usage

```sh
yum install pcre-devel openssl-devel
```

##### Development

```sh
yum install curl
```

```sh
sh -c "`curl -L https://raw.githubusercontent.com/rylnd/shpec/master/install.sh`"
```

We're working on expanding cross-platform support and need your help!

[Improve Cross-platform Support](https://github.com/exbotanical/ys/issues/1)


## Other Unix-Like Systems

### macos

Yes, Ys works on macos.

<!-- ### AL2 Linux

1. const in server_conf
2. /usr/include/openssl (not working) -->
