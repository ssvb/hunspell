/*
 * Wrap the XZ Embedded decoder into a C++ std::streambuf
 *
 * Authors: Siarhei Siamashka <siarhei.siamashka@gmail.com>
 *
 * This file has been put into the public domain.
 * You can do whatever you want with this file.
 */
#ifndef HUNXZ_DECODER_STREAMBUF_H_
#define HUNXZ_DECODER_STREAMBUF_H_

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
	char *out;
	size_t outbufsize;
	struct hunxz_buf b;
	struct hunxz_dec *s;
	enum hunxz_ret ret;
protected:
	virtual int underflow() {
		if (!s)
			return EOF;
		if (gptr() == egptr()) {
			reinterpret_cast<hunxz_reader_streambuf *>(srcbuf)->to_hunxz_buf(&b);
			b.out = (uint8_t *)out;
			b.out_pos = 0;
			b.out_size = outbufsize;
			ret = hunxz_dec_catrun(s, &b, b.in_size == 0);
			reinterpret_cast<hunxz_reader_streambuf *>(srcbuf)->from_hunxz_buf(&b);
			setg(out, out, out + b.out_pos);
		}
		return gptr() == egptr() ? EOF : *gptr();
	}
public:
	hunxz_decoder_streambuf(std::streambuf* srcbuf,
				size_t mem_usage_limit = 8 << 20, // 8MB (default xz compression settings)
				size_t outbufsize = 65536) {      // 64K is good enough for buffering
		this->srcbuf = srcbuf;
		this->outbufsize = outbufsize;
		out = new char[outbufsize];
		hunxz_crc32_init(); // FIXME? This initializes global arrays and should be done only once at start
		hunxz_crc64_init(); // FIXME? This initializes global arrays and should be done only once at start
		s = hunxz_dec_init(XZ_DYNALLOC, mem_usage_limit);
	}
	virtual ~hunxz_decoder_streambuf() {
		hunxz_dec_end(s);
		delete [] out;
	}
	// Return the XZ decoder exit code (in case if there was an error)
	enum hunxz_ret ret_code() const {
		return ret;
	};
};

#endif
