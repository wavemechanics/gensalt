# gensalt
Generate crypt() Salt Strings

# What does gensalt do

gensalt generates random `crypt()` salt strings based on a supplied template.

The template names the set of characters allowed in the salt, and the number of characters needed.

For example, a template for a short MD5 salt looks like this:
```
$1$[A-Za-z0-9./]{8}
```

An optional random generator can be supplied to override the default `rand()` calls.

# How to use it

1. compile gensalt.c 
2. include gensalt.h in your program

3. call gensalt like this:

```c
char *salt = gensalt("$1$[A-Za-z0-9./]{8}", NULL);
if salt == NULL {
    fprintf(
        stderr,
        "could not generate salt for pattern %s: %s\n"
        pattern,
        gensalt_msg()
    );
}
```

# More information

See http://perfec.to/gensalt/

# TODO

* Make a CLI
* Get rid of the statics and make it reentrant.


