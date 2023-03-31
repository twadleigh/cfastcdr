/*
MIT License

Copyright (c) 2023 Tracy Wadleigh

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include "cfastcdr.h"

#include <memory>

#include <fastcdr/Cdr.h>
#include <fastcdr/FastBuffer.h>
#include <fastcdr/exceptions/BadParamException.h>

using namespace eprosima::fastcdr;

/* constants */
const uint8_t corba_cdr = static_cast<uint8_t>(Cdr::CORBA_CDR);
const uint8_t dds_cdr = static_cast<uint8_t>(Cdr::DDS_CDR);
const uint8_t dds_cdr_without_pl = static_cast<uint8_t>(Cdr::DDS_CDR_WITHOUT_PL);
const uint8_t dds_cdr_with_pl = static_cast<uint8_t>(Cdr::DDS_CDR_WITH_PL);
const uint8_t big_endianness = static_cast<uint8_t>(Cdr::BIG_ENDIANNESS);
const uint8_t little_endianness = static_cast<uint8_t>(Cdr::LITTLE_ENDIANNESS);
const uint8_t default_endianness = static_cast<uint8_t>(Cdr::DEFAULT_ENDIAN);
const uint8_t exn_success = 0;
const uint8_t exn_not_enough_memory = 1;
const uint8_t exn_bad_param = 2;
const uint8_t exn_unknown = 3;
const char empty_message[] = {0};

/* exception */
struct Exn {
  uint8_t type;
  std::string message;

  Exn(const std::exception &e) : type(exn_unknown), message(e.what()) {}
  Exn(const exception::NotEnoughMemoryException &e) : type(exn_not_enough_memory), message(e.what()) {}
  Exn(const exception::BadParamException &e) : type(exn_bad_param), message(e.what()) {}
};

uint8_t exn_type(void *exn) {
  if (nullptr == exn) {
    return exn_success;
  }
  return static_cast<Exn *>(exn)->type;
}

const char *exn_message(void *exn) {
  if (nullptr == exn) {
    return empty_message;
  }
  return static_cast<Exn *>(exn)->message.c_str();
}

void exn_destroy(void *exn) {
  if (nullptr != exn) {
    delete static_cast<Exn *>(exn);
  }
}

/* fast buffer */
void *fb_make0() { return static_cast<void *>(new FastBuffer()); }

void *fb_make(char *buf, size_t s) {
  try {
    return static_cast<void *>(new FastBuffer(buf, s));
  } catch (const std::exception &e) {
    return nullptr;
  }
}

void fb_destroy(void *fb) { delete static_cast<FastBuffer *>(fb); }
char *fb_get_buffer(void *fb) { return static_cast<FastBuffer *>(fb)->getBuffer(); }
size_t fb_get_buffer_size(void *fb) { return static_cast<FastBuffer *>(fb)->getBufferSize(); }
bool fb_reserve(void *fb, size_t s) { return static_cast<FastBuffer *>(fb)->reserve(s); }
bool fb_resize(void *fb, size_t s) { return static_cast<FastBuffer *>(fb)->resize(s); }

#define FASTCDR_TRY(c, body)                                                                                           \
  Cdr *ptr = static_cast<Cdr *>(c);                                                                                    \
  try {                                                                                                                \
    ptr->body;                                                                                                         \
    return nullptr;                                                                                                    \
  } catch (const exception::Exception &exn) {                                                                          \
    return new Exn(exn);                                                                                               \
  } catch (const std::exception &exn) {                                                                                \
    return new Exn(exn);                                                                                               \
  }

void *cdr_make(void *fb, uint8_t e, uint8_t t) {
  try {
    return static_cast<void *>(new Cdr(*static_cast<FastBuffer *>(fb), Cdr::Endianness(e), Cdr::CdrType(t)));
  } catch (const std::exception &e) {
    return nullptr;
  }
}

void cdr_destroy(void *c) { delete static_cast<Cdr *>(c); }

void *cdr_read_encapsulation_exn(void *c) { FASTCDR_TRY(c, read_encapsulation()) }

void *cdr_serialize_encapsulation_exn(void *c){FASTCDR_TRY(c, serialize_encapsulation())}

uint8_t cdr_get_dds_cdr_pl_flag(void *c) {
  return static_cast<uint8_t>(static_cast<Cdr *>(c)->getDDSCdrPlFlag());
}

void cdr_set_dds_cdr_pl_flag(void *c, uint8_t f) { static_cast<Cdr *>(c)->setDDSCdrPlFlag(Cdr::DDSCdrPlFlag(f)); }

uint16_t cdr_get_dds_cdr_options(void *c) { return static_cast<Cdr *>(c)->getDDSCdrOptions(); }

void cdr_set_dds_cdr_options(void *c, uint16_t o) { static_cast<Cdr *>(c)->setDDSCdrOptions(o); }

uint8_t cdr_get_endianness(void *c) { return static_cast<uint8_t>(static_cast<Cdr *>(c)->endianness()); }

void cdr_set_endianness(void *c, uint8_t e) { static_cast<Cdr *>(c)->changeEndianness(Cdr::Endianness(e)); }

bool cdr_jump(void *c, size_t s) { return static_cast<Cdr *>(c)->jump(s); }

void cdr_reset(void *c) { static_cast<Cdr *>(c)->reset(); }

char *cdr_get_buffer_pointer(void *c) { return static_cast<Cdr *>(c)->getBufferPointer(); }

char *cdr_get_current_position(void *c) { return static_cast<Cdr *>(c)->getCurrentPosition(); }

size_t cdr_get_serialized_data_length(void *c) { return static_cast<Cdr *>(c)->getSerializedDataLength(); }

size_t cdr_alignment(size_t a, size_t s) { return Cdr::alignment(a, s); }

bool cdr_move_alignment_forward(void *c, size_t s) { return static_cast<Cdr *>(c)->moveAlignmentForward(s); }

void cdr_reset_alignment(void *c) { static_cast<Cdr *>(c)->resetAlignment(); }

#define FASTCDR_SERIALIZATION_FOR_BITS_TYPE(TYP)                                                                       \
  void *cdr_serialize_##TYP##_exn(void *c, TYP *d) { FASTCDR_TRY(c, serialize(*d)) }                                   \
  void *cdr_serialize_with_endianness_##TYP##_exn(void *c, TYP *d, uint8_t e) {                                        \
    FASTCDR_TRY(c, serialize(*d, Cdr::Endianness(e)))                                                                  \
  }                                                                                                                    \
  void *cdr_deserialize_##TYP##_exn(void *c, TYP *d) { FASTCDR_TRY(c, deserialize(*d)) }                                     \
  void *cdr_deserialize_with_endianness_##TYP##_exn(void *c, TYP *d, uint8_t e) {                                      \
    FASTCDR_TRY(c, deserialize(*d, Cdr::Endianness(e)))                                                                \
  }                                                                                                                    \
  void *cdr_serialize_array_##TYP##_exn(void *c, const TYP *d, size_t s) { FASTCDR_TRY(c, serializeArray(d, s)) }            \
  void *cdr_serialize_array_with_endianness_##TYP##_exn(void *c, const TYP *d, size_t s, uint8_t e) {                  \
    FASTCDR_TRY(c, serializeArray(d, s, Cdr::Endianness(e)))                                                           \
  }                                                                                                                    \
  void *cdr_deserialize_array_##TYP##_exn(void *c, TYP *d, size_t s) { FASTCDR_TRY(c, deserializeArray(d, s)) }              \
  void *cdr_deserialize_array_with_endianness_##TYP##_exn(void *c, TYP *d, size_t s, uint8_t e) {                      \
    FASTCDR_TRY(c, deserializeArray(d, s, Cdr::Endianness(e)))                                                         \
  }

FASTCDR_SERIALIZATION_FOR_BITS_TYPES

#undef FASTCDR_SERIALIZATION_FOR_BITS_TYPE

void *cdr_serialize_cstring_exn(void *c, const char *d) { FASTCDR_TRY(c, serialize(d)) }

void *cdr_serialize_with_endianness_cstring_exn(void *c, const char *d, uint8_t e) {
  FASTCDR_TRY(c, serialize(d, Cdr::Endianness(e)))
}

void *cdr_serialize_wcstring_exn(void *c, const wchar_t *d) { FASTCDR_TRY(c, serialize(d)) }

void *cdr_serialize_with_endianness_wcstring_exn(void *c, const wchar_t *d, uint8_t e) {
  FASTCDR_TRY(c, serialize(d, Cdr::Endianness(e)))
}

void *cdr_deserialize_cstring_exn(void *c, char **d) { FASTCDR_TRY(c, deserialize(*d)) }

void *cdr_deserialize_with_endianness_cstring_exn(void *c, char **d, uint8_t e) {
  FASTCDR_TRY(c, deserialize(*d, Cdr::Endianness(e)))
}

void *cdr_deserialize_cwstring_exn(void *c, wchar_t **d) { FASTCDR_TRY(c, deserialize(*d)) }

void *cdr_deserialize_with_endianness_cwstring_exn(void *c, wchar_t **d, uint8_t e) {
  FASTCDR_TRY(c, deserialize(*d, Cdr::Endianness(e)))
}
