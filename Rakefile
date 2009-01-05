require 'rake'
require 'rake/testtask'
require 'rake/clean'
require 'rake/gempackagetask'
require 'rake/rdoctask'
require 'tools/rakehelp'
require 'fileutils'
include FileUtils

setup_tests
setup_clean ["ext/flix_engine/*.{bundle,so,obj,pdb,lib,def,exp}", "ext/flix_engine/Makefile", "pkg", "lib/*.bundle", "*.gem", ".config"]

setup_rdoc ['lib/**/*.rb', 'doc/**/*.rdoc', 'ext/flix_engine/flix_engine.c']

desc "Does a full compile, test run"
task :default => [:compile, :test]

desc "Compiles all extensions"
task :compile => [:flix_engine] do
  if Dir.glob(File.join("lib","flix_engine.*")).length == 0
    STDERR.puts "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
    STDERR.puts "Gem actually failed to build.  Your system is"
    STDERR.puts "NOT configured properly to build On2 gem."
    STDERR.puts "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
    exit(1)
  end
end

task :package => [:clean,:compile,:test,:rerdoc]

setup_extension("flix_engine", "flix_engine")

name="on2"
version="0.0.1"

setup_gem(name, version) do |spec|
  spec.summary = "Ruby interface into the On2 Flix Engine."
  spec.description = spec.summary
  spec.test_files = Dir.glob('test/test_*.rb')
  spec.author="Matt Bauer"
  spec.files += %w(README Rakefile setup.rb)

  spec.required_ruby_version = '>= 1.8.4'

  if RUBY_PLATFORM =~ /mswin/
    spec.files += ['lib/flix_engine.so']
    spec.extensions.clear
    spec.platform = Gem::Platform::WIN32
  end  
end

task :install => :package do
  sh %{gem install pkg/on2-#{version}}
end

task :uninstall => [:clean] do
  sh %{gem uninstall on2}
end
