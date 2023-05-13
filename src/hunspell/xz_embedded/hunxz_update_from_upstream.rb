# A script for tweaking the upstream xz-embedded sources
#
# Authors: Siarhei Siamashka <siarhei.siamashka@gmail.com>
#
# This file has been put into the public domain.
# You can do whatever you want with this file.

prefix = "hun"
tmpdir = "upstream-xz-embedded"
upstreamgit = "https://git.tukaani.org/xz-embedded.git"

unless File.exists?("#{tmpdir}/.git")
  STDERR.puts "Please do \"git clone #{upstreamgit} #{tmpdir}\"\n"
  exit 1
end

# The list of source files
src_files = "
        linux/include/linux/xz.h
        linux/lib/xz/xz_crc32.c
        linux/lib/xz/xz_dec_lzma2.c
        linux/lib/xz/xz_dec_stream.c
        linux/lib/xz/xz_lzma2.h
        linux/lib/xz/xz_private.h
        linux/lib/xz/xz_stream.h
        userspace/xz_config.h
".strip.split

# Update the commit hash in README.md
commit_hash = `git -C #{tmpdir} rev-parse HEAD`.strip
updatedreadmetext = File.read("README.md").gsub(/(commit )[0-9a-f]+/, "\\1#{commit_hash}")
File.write("README.md", updatedreadmetext)

src_files.each do |fname|
  data = File.read(tmpdir + "/" + fname)
  # uncomment XZ_DEC_CONCATENATED and add XZ_DEC_ANY_CHECK
  data.gsub!(/(\/\*\s*(\#\s*define\s+(XZ_DEC_CONCATENATED))\s*\*\/)/, "\\2\n#define XZ_DEC_ANY_CHECK")
  # update include path
  data.gsub!("<linux/xz.h>", "\"xz.h\"")
  # change the "xz_" prefix to a different one
  data.gsub!(/([^\w\"])xz_/, "\\1#{prefix}xz_")
  # these tweaks improve performance
  data.gsub!(/static void lzma_len\(/, "static __always_inline void lzma_len(")
  data.gsub!(/static bool dict_repeat\(/, "static __always_inline bool dict_repeat(")
  # save the patched file
  File.write(File.basename(fname), data)
end
