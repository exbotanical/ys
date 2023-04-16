# Platform Support

Ys currently supports UNIX-like environments and has been tested on the following operating systems.

## Linux

On Linux systems, you will need to ensure openssl is symlinked to `/usr/include/openssl/`.

### Arch Linux

Full support for usage + development.

### Ubuntu

Full support for usage.

For development, we need to fix the integration tests. If someone on Ubuntu wants to contribute, let me know and I will expedite this.

#### Everything you need for general usage

```sh
apt update && apt install libpcre3-dev make gcc libssl-dev
```

#### Everything you need for development

```sh
apt install curl
```

```sh
sh -c "`curl -L https://raw.githubusercontent.com/rylnd/shpec/master/install.sh`"
```

### Fedora

Full support for usage + development.

#### Everything you need for general usage

```sh
yum install make gcc pcre-devel openssl-devel redhat-lsb-core
```

#### Everything you need for development

```sh
yum install curl
```

```sh
sh -c "`curl -L https://raw.githubusercontent.com/rylnd/shpec/master/install.sh`"
```

### AL2 Linux

Partial support for usage (TLS support not working yet). Development support needs work.

#### Everything you need for general usage

```sh
yum install pcre-devel openssl-devel
```

#### Everything you need for development

```sh
yum install curl
```

```sh
sh -c "`curl -L https://raw.githubusercontent.com/rylnd/shpec/master/install.sh`"
```

## Other Unix-Like Systems

### macos

Yes, Ys works on macos. You will need to symlink the following dependencies to `/usr/local/include`:

- `libcrypto`
- `libssl`
- `pcre.h`
- `openssl`

For example, if you've installed openssl with homebrew:

```sh
sudo ln -s /opt/homebrew/opt/openssl/lib/libcrypto.dylib /usr/local/include
# ...
```

### Don't See Your System?

We're working on expanding cross-platform support and need your help!

[Improve Cross-platform Support](https://github.com/exbotanical/ys/issues/1)

<!-- ### AL2 Linux

1. const in server_conf
2. /usr/include/openssl (not working) -->
