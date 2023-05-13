/*
 * A simple example
 *
 * Authors: Siarhei Siamashka <siarhei.siamashka@gmail.com>
 *
 * This file has been put into the public domain.
 * You can do whatever you want with this file.
 */
#include <fstream>
#include <iostream>
#include "hunxz_streambuf.hxx"

int main(int argc, char *argv[])
{
  if (argc < 3) {
    printf("Usage: hunxzdec [infile.xz] [outfile.txt]");
    return 10;
  }

  const char *infile = argv[1];
  const char *outfile = argv[2];

  std::ofstream fout(outfile, std::ios_base::binary);
  std::ifstream fin(infile, std::ios_base::in | std::ios_base::binary);
  hunxz_decoder_streambuf sbuf(fin.rdbuf());
  std::istream is(&sbuf);
  std::string str;

  while (getline(is, str))
    fout << str << "\n";

  return sbuf.ret_code();
}
