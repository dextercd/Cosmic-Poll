# Cosmic Poll

Set aside a bit of memory to detect random memory corruption.

ECC memory is supposed to protect against soft errors caused by radiation..
but have you ever wondered often those kind of errors actually occur?
When you look online you'll find a lot of conflicting numbers,
and really it will depend on your elevation and the amount of natural background radiation in your area.

This program allows you to reserve some memory on your system and regularly scan it for a bit flip,
so now you can find out how often these memory errors happen for you!

## Usage

```console
$ ./cosmic_poll --help
Allocate some memory and periodically check it for cosmic ray bit flips.
Usage: ./cosmic_poll [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  --alloc-size UINT:SIZE [b, kb(=1000b), kib(=1024b), ...]
  --check-interval <number> <unit (seconds, minutes, ..)>
  --db-location TEXT=/var/lib/cosmic_poll/activity.sqlite

$ ./cosmic_poll --alloc-size 256MiB
...
Anomaly detected at offset 0000000007628aec value 02
```

You can run the program on as much memory as you like. By default, the program checks 256MiB of memory every 5 minutes.
Before every check it prints a ".", and once it detects an anomaly it will print out some information and exit.

## Database

The program tracks information into an sqlite3 database. 
A lot of people online repeat the figure "One bit flip per 256MB per month",
here's an sqlite3 session where I compare my db against this figure:

How many anomalies should I have according to that "1 / 256MiB/Month" figure?

```console
$ sqlite3 /var/lib/cosmic_poll/activity.sqlite

sqlite> SELECT SUM((bytes / (1024.0 * 1024 * 256)) * (seconds / (60 * 60 * 24 * 30))) FROM active_times;
SUM( (bytes / (1024.0 * 1024 * 256)) * (seconds / (60 * 60 *
------------------------------------------------------------
24.1281936431086                                            
```

24!? And how many do I actually have?

```console
sqlite> SELECT COUNT(*) FROM anomalies;
COUNT(*)
--------
2      

```

Much fewer, only 2! Let's looks at all data about these anomalies.

```console
sqlite> SELECT * FROM anomalies;
time_submitted       offset      value  bytes       mask
-------------------  ----------  -----  ----------  ----
2022-04-26 13:50:00  4955736812  2      5368709120  15  
2022-05-09 00:08:20  5026409196  2      5368709120  15  
```

(The `bytes` column is how much total memory the program was monitoring when it detected the anomaly,
the `offset` is the byte position where the anomaly was found.

## Engineering

The program is extremely over-engineered. It could've been **a lot** simpler
but I had fun making it more complicated than it needed to be. :smile:

The test suite runs the program in a debugger that is used to write to the memory, thereby emulating a bit flip.
This verifies that the compiler did not optimise away the memory monitoring code.
