﻿  // @file common
  // @brief Definitions of common related structures and functions.
  // @author Mamoru Kaminaga
  // @date 2017-07-27 21:04:42
  // Copyright 2017 Mamoru Kaminaga
#include <assert.h>
#include "./common.h"
#include "./common_internal.h"
#include "./system_internal.h"
#define SYS_BIT_NOT_FOUND (-1)
namespace sys {
  //
  // These are internal structures related to common
  //
struct IdServer::Impl {
  uint32_t m_flag;  // If m_flag's bit m is set, all n_flag[m]'s bits are set.
  uint32_t n_flag[32];
  // The number of set bit are counted.
  int CountSetBits32(uint32_t v) {
    uint32_t count = (v & 0x55555555) + ((v >> 1) & 0x55555555);
    count = (count & 0x33333333) + ((count >> 2) & 0x33333333);
    count = (count & 0x0f0f0f0f) + ((count >> 4) & 0x0f0f0f0f);
    count = (count & 0x00ff00ff) + ((count >> 8) & 0x00ff00ff);
    return ((count & 0x0000ffff) + ((count >> 16) & 0x0000ffff));
  }
  // LSB is acquired.
  int GetLSB32(uint32_t v) {
    if (v == 0) return SYS_BIT_NOT_FOUND;
    v |= (v << 1);
    v |= (v << 2);
    v |= (v << 4);
    v |= (v << 8);
    v |= (v << 16);
    return (32 - CountSetBits32(v));
  }
  // Finds the first clear bit from lower class.
  int SearchClearBitFoward32(int n, uint32_t v) {
    v = ~v & (0xffffffff << n);
    if (v) return GetLSB32(v);
    return SYS_BIT_NOT_FOUND;
  }
  // Nth bit of v is set by this function.
  void SetBit32(int n, uint32_t* v) {
    *v = *v | (0x00000001 << n);
  }
  // Nth bit of v is cleared by this function.
  void ClearBit32(int n, uint32_t* v) {
    *v = *v & ~(0x00000001 << n);
  }
  // A not used id is searched.
  int FindFreeId() {
    if (m_flag == 0xffffffff) return SYS_BIT_NOT_FOUND;  // All bits are set.
    const int m = SearchClearBitFoward32(0, m_flag);
    const int n = SearchClearBitFoward32(0, n_flag[m]);
    return ((m << 5) | n);
  }
  // A flag bit for id use is set.
  void BusyId(int id) {
    const int m = id >> 5;  // 5 bits set.
    const int n = id & 0x001f;  // 5 bits set.
    SetBit32(n, &n_flag[m]);
    if (n_flag[m] == 0xffffffff) SetBit32(m, &m_flag);
  }
  // A flag bit for id use is cleared.
  void FreeId(int id) {
    const int m = id >> 5;  // 5 bits set.
    const int n = id & 0x001f;  // 5 bits set.
    ClearBit32(n, &n_flag[m]);
    ClearBit32(m, &m_flag);
  }
};
IdServer::IdServer() : impl_(new Impl()) {
  memset(&impl_->m_flag, 0, sizeof(uint32_t));
  memset(impl_->n_flag, 0, sizeof(uint32_t) * 32);
}
IdServer::~IdServer() = default;
int IdServer::CreateId() {
  const int id = impl_->FindFreeId();
  if (id == SYS_BIT_NOT_FOUND) return SYS_ID_SERVER_EXCEEDS_LIMIT;
  impl_->BusyId(id);
  return id;
}
int IdServer::ReleaseId(int id) {
  impl_->FreeId(id);
  return id;
}

  //
  // These are public structures related to common
  //
ResourceDesc::ResourceDesc() : file_name(), mem_ptr(nullptr), mem_size(0),
    use_mem(false) { }

  //
  // These are internal functions related to common
  //

  //
  // These are public functions related to common
  //
bool ErrorDialogBox(const wchar_t* format, ...) {
  wchar_t buffer[256] = {0};
  va_list args;
  va_start(args, format);
  vswprintf_s(buffer, 256, format, args);
  MessageBox(system_data.hwnd, buffer, L"System Error", MB_ICONEXCLAMATION);
  return true;
}
}  // namespace sys
