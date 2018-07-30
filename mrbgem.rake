MRuby::Gem::Specification.new('mruby-rwlock') do |spec|
  spec.license = 'MIT'
  spec.authors = 'takumakume'

  if build.toolchains.include?("androideabi")
    spec.cc.flags << '-DHAVE_PTHREADS'
  else
    spec.linker.libraries << ['pthread']
  end
end
