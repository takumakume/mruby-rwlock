# Based on https://github.com/matsumotory/mruby-mutex,
# Please see https://github.com/matsumotory/mruby-mutex/blob/master/LICENSE.
class RWLock
  def initialize(obj = nil)
    unless obj.nil?
      if obj[:global] == true || obj[:global] == false
        self.new2(obj[:global])
      else
        raise ":global => true or false"
      end
    else
      self.new2
    end
  end

  def read_try_lock_loop(retrytime = nil, timeout = nil, &b)
    _try_lock_loop("read", retrytime, timeout, &b)
  end

  def write_try_lock_loop(retrytime = nil, timeout = nil, &b)
    _try_lock_loop("write", retrytime, timeout, &b)
  end

  private

  def _try_lock_loop(rw, retrytime = 100000, timeout = 500000 * 10, &b)
    count = 0
    self_unlock = false
    loop do
      count += 1
      if self.send("#{rw}_try_lock")
        b.call
        break
      elsif timeout < retrytime * count
        self.unlock
        self_unlock = true
      else
        usleep(retrytime)
      end
    end
    self_unlock
  end
end
