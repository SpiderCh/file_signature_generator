### Using

Application hash file by blocks.

To hash file call binary file with parameter:

```
signature_generator --input_file="/path/to/file" --output_file="/path/to/output/file"
```

Result will be written to the "/path/to/output/file"

By default block size equal 1Mb.

If you want change this call binary file with parameter:

```
-b desired_block_size_in_bytres
```

For hashing by default used md5 algorithm. If you want to use crc algorithm call binary with parameter:

```
--algorithm="crc"
```

### Testing

Tests written for each hashing algorithm. They are placed in unit_test folder of each algorithm.

For md5 examples taken from [RFC](https://tools.ietf.org/html/rfc1321).

For CRC used some default strings of text.

### Building

For building you will need cmake version 3.14 or higher, compiler with support of C++17 and boost version 1.70.

If you don't have such a compiler you may change using `optional` on `boost::optional` and lower required C++ standard to c++14

Boost also may be lowered.

