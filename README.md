# mmgrep

This program searches files for a binary string given as a hex pattern.

Usage:

```
./mmgrep deadbeef file1 file2
```

It uses `mmap` and `memmem` so it should be very fast. It will not work if the files are not seekable.
