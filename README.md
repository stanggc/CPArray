# CPArray

CPArray is a contiguous array C library, where each array member
holds a character pointer (`char *`).

This is somewhat similar to C++'s `std::vector`.

## Usage

Simply drop `CPArray.c` and `CPArray.h` into your C project.

In order to compile CPArray successfully, you will need to define the
following macros during compilation:

* For Windows: `-DWINDOWS`
* For MacOS: `-DMACOS`
* For Linux: `-DLINUX`
* For Unix-based OSes other than MacOS and Linux: `-DUNIX`

Refer to `Test_CPArray.c` for examples on how to use the library.

## Building

### Linux

```shell
$ gcc -DLINUX -D_LARGEFILE64_SOURCE -o Test_CPArray Test_CPArray.c CPArray.c
```

### MacOS

```shell
$ gcc -DMACOS -o Test_CPArray Test_CPArray.c CPArray.c
```

### Windows

```shell
$ gcc -o Test_CPArray.exe Test_CPArray.c CPArray.c
```

## Testing

The tests are in `Test_CPArray.c`.

Run `Test_CPArray` / `Test_CPArray.exe`.

Run `./Test_CPArray all` to include running a large-scale usage test.

A return code of zero indicates tests pass; non-zero otherwise.

## Licence

MIT licence. See LICENSE.txt.
