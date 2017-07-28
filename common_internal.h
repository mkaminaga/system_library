  // @file common_internal.h
  // @brief Declaration of sound related structures and functions.
  // @author Mamoru Kaminaga
  // @date 2017-07-27 21:04:42
  // Copyright 2017 Mamoru Kaminaga
#ifndef COMMON_INTERNAL_H_
#define COMMON_INTERNAL_H_
#include <memory>
#include "./common.h"
  //
  // These are internal macros related to sound
  //
#define GAME_ID_NOT_FOUND   (-1)
#define GAME_ID_MAX         (1024)

  //
  // These are internal enumerations and constants related to sound
  //

namespace sys {
  //
  // These are internal structures related to sound
  //
class IdServer {
 public:
  IdServer();
  ~IdServer();
  int CreateId();
  int ReleaseId(int id);
 private:
  struct Impl;
  std::unique_ptr<Impl> impl_;
};

  //
  // These are internal functions related to sound
  //
}  // namespace sys
#endif  // COMMON_INTERNAL_H_
