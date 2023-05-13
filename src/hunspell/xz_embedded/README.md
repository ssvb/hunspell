From https://git.tukaani.org/xz-embedded.git commit d89ad8130128d71c773f5e50e356562a506f843e
The `hunxz_update_from_upstream.rb` script can be used for trying to upgrade.

Example:
```c++
#include <fstream>
#include <iostream>
#include "hunxz_decoder_streambuf.h"

int main(int argc, char *argv[])
{
	if (argc < 2) {
		fprintf(stderr, "Usage: hunxztest [infile.xz]\n");
		return 1;
	}

	// Open the input file
	std::ifstream infile(argv[1], std::ios_base::in | std::ios_base::binary);
	if (!infile.is_open()) {
		fprintf(stderr, "Can't open the input file.\n");
		return 1;
	}

	// Initialize the XZ decompressor
	hunxz_decoder_streambuf xzstreambuf(infile.rdbuf());
	std::istream xzstream(&xzstreambuf);

	// Mimic the "getline()" behaviour of Hunspell. This is primarily
	// done this way for a fair apples-to-apples performance comparison
	// against the "hunzip" tool. Note: the use of printf will mishandle
	// the possible '\0' characters if decompressing a binary file and
	// may add an extra '\n' at the end.
	std::string str;
	while (getline(xzstream, str))
		printf("%s\n", str.c_str());

	switch (xzstreambuf.ret_code()) {
		case XZ_OK:
		case XZ_STREAM_END:
			return 0;
		case XZ_FORMAT_ERROR:
			fprintf(stderr, "File format is not XZ.\n");
			break;
		case XZ_MEMLIMIT_ERROR:
			fprintf(stderr, "This XZ file requires too much memory (--best abused?).\n");
			break;
		case XZ_OPTIONS_ERROR:
			fprintf(stderr, "This XZ file uses unsupported features (such as BCJ).\n");
			break;
		case XZ_DATA_ERROR:
			fprintf(stderr, "This XZ file is corrupted.\n");
			break;
		default:
			fprintf(stderr, "Exit code %d\n", xzstreambuf.ret_code());
			break;
	}
	return xzstreambuf.ret_code();
}
```
