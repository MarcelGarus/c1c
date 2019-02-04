# c1c

A compiler for `.c1` files - just like regular C files, but array indexes start at 1.

## Motivation

Did you ever feel the need to torture a software engineer?
The c1 programming language is the perfect fit for you!

## How to install

Just download c1c and add it to your path.
There's a linux executable in the repo, or you can build the compiler from source (`c1c.c`).

## Usage

Just execute `c1c test.c1` to compile the given file to an executable.

### Options

There's the option to provide a custom start index for arrays using the `--start-index` option.
It's not limited to integer values, so you can even let your arrays start at 0.5 or pi. How awesome is that!

**Warning**:
Almost all array start indexes are permitted, except 0.
Because that would be crazy.
Also, your indexes are casted to `int`s.

### Examples

Here's a simple program that's supposed to be compiled using an array start of 1:

```c
#include <stdio.h>

int main(void) {
  int a[] = { 2, 3, 1 };
  printf("First element is %d.\n", a[a[3]]);
}
```

You can find it in the repo as `test.c1`.

---

**Disclaimer**:
Do not use this compiler in a production environment.
In fact, just don't use it at all.
I do not support array indexes starting at 1.
Of course, pi is the only correct start index.
