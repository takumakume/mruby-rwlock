/*
 * Based on https://github.com/matsumotory/mruby-mutex,
 * Please see https://github.com/matsumotory/mruby-mutex/blob/master/LICENSE.
 */

#include "mruby.h"
#include "mruby/data.h"
#include "mruby/class.h"
#include "mrb_rwlock.h"
#include <sys/shm.h>
#include <pthread.h>

#define DONE mrb_gc_arena_restore(mrb, 0);

pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;

typedef struct {
  pthread_rwlock_t *rwlock;
  pthread_rwlockattr_t rwlock_attr;
  int shmid;
  unsigned int global:1;
} mrb_rwlock_data;

static void mrb_rwlock_data_free(mrb_state *mrb, void *p)
{
  mrb_rwlock_data *data = p;

  if (data->global) {
    shmdt(data->rwlock);
  } else {
    pthread_rwlock_destroy(data->rwlock);
  }
  mrb_free(mrb, data);
}

static const struct mrb_data_type mrb_rwlock_data_type = {
  "mrb_rwlock_data", mrb_rwlock_data_free,
};

static mrb_value mrb_rwlock_init(mrb_state *mrb, mrb_value self)
{
  mrb_rwlock_data *data;
  pthread_rwlock_t *rt = NULL;
  pthread_rwlockattr_t rat;
  int shmid;
  unsigned int global = 0;

  mrb_get_args(mrb, "|b", &global);

  data = (mrb_rwlock_data *)DATA_PTR(self);
  if (data) {
    mrb_free(mrb, data);
  }
  DATA_TYPE(self) = &mrb_rwlock_data_type;
  DATA_PTR(self) = NULL;
  data = (mrb_rwlock_data *)mrb_malloc(mrb, sizeof(mrb_rwlock_data));

  if (global) {
    shmid = shmget(IPC_PRIVATE, sizeof(pthread_rwlock_t), 0600);
    if (shmid < 0) {
      mrb_raise(mrb, E_RUNTIME_ERROR, "shmget failed");
    }

    rt = shmat(shmid, NULL, 0);
    if ((int)rt == -1) {
      mrb_raise(mrb, E_RUNTIME_ERROR, "shmat failed");
    }

    if (shmctl(shmid, IPC_RMID, NULL) != 0) {
      mrb_raise(mrb, E_RUNTIME_ERROR, "shmctl failed");
    }

    pthread_rwlockattr_init(&rat);

    if (pthread_rwlockattr_setpshared(&rat, PTHREAD_PROCESS_SHARED) != 0) {
      mrb_raise(mrb, E_RUNTIME_ERROR, "pthread_rwlockattr_setpshared failed");
    }

    pthread_rwlock_init(rt, &rat);
  } else {
    shmid = -1;
    pthread_rwlock_init(&rwlock, NULL);
    rt = &rwlock;
  }

  data->rwlock = rt;
  data->rwlock_attr = rat;
  data->shmid = shmid;
  data->global = global;
  DATA_PTR(self) = data;

  return self;
}

static mrb_value mrb_rwlock_rdlock(mrb_state *mrb, mrb_value self)
{
  mrb_rwlock_data *data = DATA_PTR(self);

  if (pthread_rwlock_rdlock(data->rwlock) != 0) {
    mrb_raise(mrb, E_RUNTIME_ERROR, "pthread_rwlock_rdlock failed");
  }

  return mrb_fixnum_value(0);
}

static mrb_value mrb_rwlock_tryrdlock(mrb_state *mrb, mrb_value self)
{
  mrb_rwlock_data *data = DATA_PTR(self);

  if (pthread_rwlock_tryrdlock(data->rwlock) != 0) {
    mrb_raise(mrb, E_RUNTIME_ERROR, "pthread_rwlock_tryrdlock failed");
  }

  return mrb_fixnum_value(0);
}

static mrb_value mrb_rwlock_wrlock(mrb_state *mrb, mrb_value self)
{
  mrb_rwlock_data *data = DATA_PTR(self);

  if (pthread_rwlock_wrlock(data->rwlock) != 0) {
    mrb_raise(mrb, E_RUNTIME_ERROR, "pthread_rwlock_wrlock failed");
  }

  return mrb_fixnum_value(0);
}

static mrb_value mrb_rwlock_trywrlock(mrb_state *mrb, mrb_value self)
{
  mrb_rwlock_data *data = DATA_PTR(self);

  if (pthread_rwlock_trywrlock(data->rwlock) != 0) {
    mrb_raise(mrb, E_RUNTIME_ERROR, "pthread_tryrwlock_wrlock failed");
  }

  return mrb_fixnum_value(0);
}

static mrb_value mrb_rwlock_unlock(mrb_state *mrb, mrb_value self)
{
  mrb_rwlock_data *data = DATA_PTR(self);

  if (pthread_rwlock_unlock(data->rwlock) != 0) {
    mrb_raise(mrb, E_RUNTIME_ERROR, "pthread_rwlock_unlock failed");
  }

  return mrb_fixnum_value(0);
}

static mrb_value mrb_rwlock_destroy(mrb_state *mrb, mrb_value self)
{
  mrb_rwlock_data *data = DATA_PTR(self);

  if (pthread_rwlock_destroy(data->rwlock)  != 0) {
    mrb_raise(mrb, E_RUNTIME_ERROR, "pthread_rwlock_destroy failed");
  }

  return mrb_fixnum_value(0);
}

void mrb_mruby_rwlock_gem_init(mrb_state *mrb)
{
  struct RClass *rwlock;
  rwlock = mrb_define_class(mrb, "RWLock", mrb->object_class);
  mrb_define_method(mrb, rwlock, "new2",          mrb_rwlock_init,      MRB_ARGS_REQ(1));
  mrb_define_method(mrb, rwlock, "read_lock",      mrb_rwlock_rdlock,    MRB_ARGS_NONE());
  mrb_define_method(mrb, rwlock, "rdlock",         mrb_rwlock_rdlock,    MRB_ARGS_NONE());
  mrb_define_method(mrb, rwlock, "try_read_lock",  mrb_rwlock_tryrdlock, MRB_ARGS_NONE());
  mrb_define_method(mrb, rwlock, "tryrdlock",      mrb_rwlock_tryrdlock, MRB_ARGS_NONE());
  mrb_define_method(mrb, rwlock, "write_lock",     mrb_rwlock_wrlock,    MRB_ARGS_NONE());
  mrb_define_method(mrb, rwlock, "wrlock",         mrb_rwlock_wrlock,    MRB_ARGS_NONE());
  mrb_define_method(mrb, rwlock, "try_write_lock", mrb_rwlock_trywrlock, MRB_ARGS_NONE());
  mrb_define_method(mrb, rwlock, "trywrlock",      mrb_rwlock_trywrlock, MRB_ARGS_NONE());
  mrb_define_method(mrb, rwlock, "unlock",         mrb_rwlock_unlock,    MRB_ARGS_NONE());
  mrb_define_method(mrb, rwlock, "destroy",        mrb_rwlock_destroy,   MRB_ARGS_NONE());
  DONE;
}

void mrb_mruby_rwlock_gem_final(mrb_state *mrb)
{
}
