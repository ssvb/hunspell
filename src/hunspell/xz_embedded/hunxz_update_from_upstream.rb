# A script for tuning the upstream xz-embedded sources. The necessary
# options are enabled and the "xz_*" prefix is changed to "hunxz_*".
#
# Authors: Siarhei Siamashka <siarhei.siamashka@gmail.com>
#
# This file has been put into the public domain.
# You can do whatever you want with this file.

tmpdir = "upstream-xz-embedded"
upstreamgit = "https://git.tukaani.org/xz-embedded.git"

unless File.exists?("#{tmpdir}/.git")
  STDERR.puts "Please do \"git clone #{upstreamgit} #{tmpdir}\"\n"
  exit 1
end

# The recommended list of source files
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

# These options will be uncommented in xz_config.h
use_options = "
  XZ_DEC_CONCATENATED
".strip.split

upstreamrev = `git -C #{tmpdir} rev-parse HEAD`.strip

updatedreadmetext = File.read("README.md").gsub(/(commit )[0-9a-f]+/, "\\1#{upstreamrev}")
File.write("README.md", updatedreadmetext)


readme = sprintf("From %s commit %s\n", upstreamgit, upstreamrev) +
         "The update-from-upstream.rb script can be used for trying to upgrade.\n"
File.write("readme.txt", readme)

src_files.each do |fname|
  data = File.read(tmpdir + "/" + fname)
  data.gsub!(/(\/\*\s*(\#\s*define\s+(\w+))\s*\*\/)/) { use_options.include?($3) ? $2 : $1 }
  data.gsub!("<linux/xz.h>", "\"xz.h\"")
  data.gsub!(/([^\w\"])xz_/, "\\1hunxz_")
  File.write(File.basename(fname), data)
end
