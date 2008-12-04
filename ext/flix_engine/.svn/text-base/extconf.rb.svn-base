require 'mkmf'

dir_config("flix_engine")

inc, lib = dir_config('', '/usr/local')
libs = ['m', 'z']
while not find_library('flixengine2', 'Flix2_Version', lib, "#{lib}/flixengine") do
  exit 1 if libs.empty?
  have_library(libs.shift)
end

create_makefile("flix_engine")
