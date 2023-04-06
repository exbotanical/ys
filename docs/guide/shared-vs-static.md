# A Primer on Shared Objects and Static Libraries

## Shared Objects

### Pros
* Are smaller in size, as they are not linked with the application at compile time.
* Are loaded into memory at runtime, meaning multiple applications can use the same shared object, reducing memory usage.
* If updated, all applications that use it will benefit from the updates.

### Cons
* There is a performance cost to loading a shared object into memory at runtime.
* Shared objects can be more complex to manage, as they may have dependencies on other shared objects.
* There is a potential security risk if a shared object is compromised, as it can be accessed by multiple applications.

## Static Libraries

### Pros

* Static libraries are linked with the application at compile time, resulting in faster load times.
* There are no external dependencies, as all the required code is included in the executable.
* There is no runtime performance cost associated with loading the library.

### Cons
* Static libraries can result in larger executable files.
* If a static library is updated, the application must be recompiled to take advantage of the update.
* If multiple applications use the same static library, each application must include its own copy of the library, resulting in increased memory usage.
