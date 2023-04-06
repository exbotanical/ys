# What is Ys (pronounced EEs)?

Ys is a minimal web application framework for the C programming language. Ys is designed to help you bootstrap functional APIs with minimal dev effort using a rich feature-set of high-level primitives.

## At a Glance

- Out-of-the-box HTTP multiplexing (routing)
- TLS support
- Logging
- Request-level metadata e.g. pre-parsed URL and query parameters
- Middleware
- Zero-config CORS
- Cookies

## Why?

C programs tend to be verbose. You seldom see C servers in the wild because the development overhead is absolutely massive and mired in complexity; before you can think about building an API, you'll need to sift through syscalls, parse raw requests, handle socket descriptors, and become intimately familiar with the HTTP specification.

Shipped with a robust feature-set inspired by the Express JavaScript framework, Ys provides the building blocks you'll need for most use-cases and takes care of the complexity so you can focus on what matters — building your application.


## What Ys is *Not* For

- **Ys is not designed for building enterprise-grade servers.**

  If you plan on launching your next product, you probably don't want to be building it in C and moreover Ys. While we aim to carry Ys to a production-ready state, we don't recommend it for your business needs.

- **Ys is performant but only insofar as C itself is performant.**

  We prioritize developer experience and ease-of-use over ever-optimized performance.