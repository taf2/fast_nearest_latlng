Gem::Specification.new do |s|
  s.name    = "fast_nearest_latlng"
  s.authors = ["Todd A. Fisher"]
  s.version = '0.0.1'
  s.date    = '2012-04-15'
  s.description = %q{Locate the nearest location within a set to a given location}
  s.email   = 'todd.fisher@gmail.com'
  s.extra_rdoc_files = ['LICENSE', 'README']
  s.files = ['ext/fastloc.c', 'ext/extconf.rb', 'ext/test.rb']
  #### Load-time details
  s.require_paths = ['ext']
  s.rubyforge_project = 'fast_nearest_latlng'
  s.summary = %q{Nearest lat lng lookup}
  s.test_files = ['ext/test.rb']
  s.extensions << 'ext/extconf.rb'

  #### Documentation and testing.
  s.homepage = 'http://github.com/'
  s.rdoc_options = ['--main', 'README']

  s.platform = Gem::Platform::RUBY
end
