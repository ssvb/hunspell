/*
 * C++ wrapper, which provides XZ decoder as std::streambuf
 *
 * Authors: Siarhei Siamashka <siarhei.siamashka@gmail.com>
 *
 * This file has been put into the public domain.
 * You can do whatever you want with this file.
 */

#ifndef HUNXZ_STREAMBUF_H
#define HUNXZ_STREAMBUF_H

#include <iostream>
#include "xz.h"

class hunxz_reader_streambuf : public std::streambuf {
public:
  inline void to_hunxz_buf(struct hunxz_buf *b) {
    if (gptr() == egptr())
      underflow();
    b->in = (uint8_t *)eback();
    b->in_pos = gptr() - eback();
    b->in_size = egptr() - eback();
  }
  inline void from_hunxz_buf(struct hunxz_buf *b) {
    setg((char *)b->in, (char *)b->in + b->in_pos, (char *)b->in + b->in_size);
  }
};

class hunxz_decoder_streambuf : public std::streambuf {
    std::streambuf *srcbuf;
    char out[65536];
    struct hunxz_buf b;
    struct hunxz_dec *s;
    enum hunxz_ret ret;
protected:
    int underflow() {
        if (!s)
          return EOF;
        if (gptr() == egptr()) {
            reinterpret_cast<hunxz_reader_streambuf *>(srcbuf)->to_hunxz_buf(&b);
            b.out = (uint8_t *)out;
            b.out_pos = 0;
            b.out_size = sizeof(out);
            ret = hunxz_dec_catrun(s, &b, b.in_size == 0);
            reinterpret_cast<hunxz_reader_streambuf *>(srcbuf)->from_hunxz_buf(&b);
            setg(out, out, out + b.out_pos);
        }
        return gptr() == egptr() ? EOF : *gptr();
    }
public:
    hunxz_decoder_streambuf(std::streambuf* srcbuf) : srcbuf(srcbuf) {
      // FIXME? This initializes global arrays and should be done only once at start
      hunxz_crc32_init();
      hunxz_crc64_init();
      // Set 8 MB memory usage limit and reject XZ files created with extreme settings
      s = hunxz_dec_init(XZ_DYNALLOC, 8 << 20);
    }
    ~hunxz_decoder_streambuf() {
      hunxz_dec_end(s);
    }
    // Return XZ decoder exit code
    enum hunxz_ret ret_code() const {
      return ret;
    };
};

#endif
