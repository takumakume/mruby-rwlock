# Based on https://github.com/matsumotory/mruby-mutex,
# Please see https://github.com/matsumotory/mruby-mutex/blob/master/LICENSE.

assert("RWLock") do
  RWLock.class == Class
end

assert("RWLock init") do
  t = RWLock.new
  t.class == RWLock
end

assert("RWLock init for global") do
  t = RWLock.new(:global => true)
  t.class == RWLock
end

assert("RWLock#read_lock") do
  t = RWLock.new
  assert_equal(0, t.read_lock)
  t.unlock
end

assert("RWLock#rdlock") do
  t = RWLock.new
  assert_equal(0, t.rdlock)
  t.unlock
end

assert("RWLock#write_lock") do
  t = RWLock.new
  assert_equal(0, t.write_lock)
  t.unlock
end

assert("RWLock#wrlock") do
  t = RWLock.new
  assert_equal(0, t.wrlock)
  t.unlock
end

assert("RWLock#try_read_lock") do
  t = RWLock.new
  assert_equal(0, t.try_read_lock)
  t.unlock
end

assert("RWLock#tryrdlock") do
  t = RWLock.new
  assert_equal(0, t.tryrdlock)
  t.unlock
end

assert("RWLock#try_write_lock") do
  t = RWLock.new
  assert_equal(0, t.try_write_lock)
  t.unlock
end

assert("RWLock#trywrlock") do
  t = RWLock.new
  assert_equal(0, t.trywrlock)
  t.unlock
end

assert("RWLock#unlock") do
  t = RWLock.new
  t.read_lock
  assert_equal(0, t.unlock)
end

assert("RWLock#destroy") do
  t = RWLock.new
  assert_equal(0, t.destroy)
end

assert("RWLock#read_lock -> read_lock") do
  t = RWLock.new
  assert_equal(0, t.read_lock)
  assert_equal(0, t.read_lock)
  t.unlock
end
