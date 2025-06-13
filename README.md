# miscounts

Miscounts is a cross-platform CLI based app written in C++ that logs all the miscounts you've experienced
or dealt with in your life.

## Usage

Make your first miscount like this

```commandline
$ miscount -m <Miscount> -n <Name> -d <Short Description>
```

This will create a file by the name of `miscount.csv` on your Documents folder if there wasn't any, and
then log the miscount.

## Installation

I've written my very own build system using Python for use in my projects.

Invoke this command and a binary will be built for you.

```commandline
$ python recipe.py
```

And then, install it to a directory in your `$PATH`.

```commandline
$ install -m775 miscount /usr/bin/miscount
```

## License
Miscounts is free and open source software abiding with the terms of the MIT License.