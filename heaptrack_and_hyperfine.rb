#!/usr/bin/env ruby

require "shellwords"
require "open3"
require "tempfile"

heapdata = []
ARGV.each do |cmd|
  heap = "?"
  rss  = "?"
  result, stderr, status = Open3.capture3("heaptrack --record-only " + cmd)
  if result =~ /heaptrack\s+\-\-analyze\s+(.*)$/
    result2 = `heaptrack_print #{$1}`
    if result2 =~ /peak heap memory consumption\: (\S+)/
      heap = $1
    end
    if result2 =~ /peak RSS \(including heaptrack overhead\)\: (\S+)/
      rss = $1
    end
  end
  heapdata.push({heap: heap, rss: rss})
end

tmp = Tempfile.new
system("hyperfine --export-markdown #{tmp.path.shellescape} --warmup 1 #{ARGV.shelljoin}")

puts
puts "CPU: #{$1}\n" if File.read("/proc/cpuinfo") =~ /model name\s+: (.*$)/

md = File.open(tmp.path)
puts md.gets.strip + " Peak heap usage | Peak RSS |"
puts md.gets.strip + "---:|---:|"
while (md_l = md.gets)
  mu_l = heapdata.shift
  puts md_l.strip + " #{mu_l[:heap]} | #{mu_l[:rss]} |"
end
