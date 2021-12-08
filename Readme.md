### Building

You will need [conan](https://conan.io), cmake version 3.20.3 or higher, compiler with support of C++17.

### Using

Application hash file by blocks.

To hash file call binary file with parameter:

```
signature_generator --input_file="/path/to/file" --output_file="/path/to/output/file"
```

Result will be written to the "/path/to/output/file"

By default block size equal 1Mb. If you want to change block size, call binary with parameter:

```
-b desired_block_size_in_bytres
```

Default hashing algorithm is md5. If you want to use crc algorithm, call binary with parameter:

```
--algorithm="crc"
```

### Testing

Tests written for each hashing algorithm. They are placed in unit_test folder of each algorithm.

For md5 examples taken from [RFC](https://tools.ietf.org/html/rfc1321).

For CRC used some default strings of text.
