**Based on [matsumotory/mruby-mutex](https://github.com/matsumotory/mruby-mutex), Please see [LICENCE](https://github.com/matsumotory/mruby-mutex/blob/master/LICENSE).**

# mruby-rwlock   [![Build Status](https://travis-ci.org/takumakume/mruby-rwlock.svg?branch=master)](https://travis-ci.org/takumakume/mruby-rwlock)
RWLock class

## install by mrbgems
- add conf.gem line to `build_config.rb`

```ruby
MRuby::Build.new do |conf|

    # ... (snip) ...

    conf.gem :github => 'takumakume/mruby-rwlock'
end
```

## example

```ruby
rwlock = RWLock.new

# for global
rwlock = RWLock.new(:global => true)

# Read lock
rwlock.read_lock # or rwlock.rdlock
rwlock.try_read_lock # or rwlock.tryrdlock
rwlock.try_read_lock_loop do
  # code
end

# Read lock
rwlock.write_lock # or rwlock.wrlock
rwlock.try_write_lock # or rwlock.tryrwlock
rwlock.try_write_lock_loop(50000, 10000) do
  # code
end

# Unlock
rwlock.unlock

# Destroy
rwlock.destroy
```
