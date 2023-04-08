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
const CFASTCDR_ENDIANNESS CFASTCDR_DEFAULT_ENDIANNESS = static_cast<CFASTCDR_ENDIANNESS>(Cdr::DEFAULT_ENDIAN);
const char empty_message[] = {0};

/* exception */
struct cfastcdr_exn {
  CFASTCDR_EXN_TYPE type;
  std::string message;

  cfastcdr_exn(const std::exception &e) : type(CFASTCDR_EXN_UNKNOWN), message(e.what()) {}
  cfastcdr_exn(const exception::NotEnoughMemoryException &e)
      : type(CFASTCDR_EXN_NOT_ENOUGH_MEMORY), message(e.what()) {}
  cfastcdr_exn(const exception::BadParamException &e) : type(CFASTCDR_EXN_BAD_PARAM), message(e.what()) {}
};

CFASTCDR_EXN_TYPE cfastcdr_exn_type(cfastcdr_exn *exn) {
  if (nullptr == exn) {
    return CFASTCDR_EXN_SUCCESS;
  }
  return exn->type;
}

const char *cfastcdr_exn_message(cfastcdr_exn *exn) {
  if (nullptr == exn) {
    return empty_message;
  }
  return exn->message.c_str();
}

void cfastcdr_exn_destroy(cfastcdr_exn *exn) {
  if (nullptr != exn) {
    delete exn;
  }
}

/* fast buffer */
struct cfastcdr_fastbuffer : public FastBuffer {};
cfastcdr_fastbuffer *cfastcdr_fastbuffer_make_variable() {
  return static_cast<cfastcdr_fastbuffer *>(new FastBuffer());
}

cfastcdr_fastbuffer *cfastcdr_fastbuffer_make_fixed(char *buf, size_t s) {
  try {
    return static_cast<cfastcdr_fastbuffer *>(new FastBuffer(buf, s));
  } catch (const std::exception &e) {
    return nullptr;
  }
}

void cfastcdr_fastbuffer_destroy(cfastcdr_fastbuffer *fb) { delete static_cast<FastBuffer *>(fb); }

char *cfastcdr_fastbuffer_get_buffer(cfastcdr_fastbuffer *fb) { return static_cast<FastBuffer *>(fb)->getBuffer(); }

size_t cfastcdr_fastbuffer_get_buffer_size(cfastcdr_fastbuffer *fb) {
  return static_cast<FastBuffer *>(fb)->getBufferSize();
}

bool cfastcdr_fastbuffer_reserve(cfastcdr_fastbuffer *fb, size_t s) {
  return static_cast<FastBuffer *>(fb)->reserve(s);
}

bool cfastcdr_fastbuffer_resize(cfastcdr_fastbuffer *fb, size_t s) { return static_cast<FastBuffer *>(fb)->resize(s); }

#define CFASTCDR_TRY(c, body)                                                                                          \
  Cdr *ptr = static_cast<Cdr *>(c);                                                                                    \
  try {                                                                                                                \
    ptr->body;                                                                                                         \
    return static_cast<cfastcdr_exn *>(nullptr);                                                                       \
  } catch (const exception::Exception &exn) {                                                                          \
    return new cfastcdr_exn(exn);                                                                                      \
  } catch (const std::exception &exn) {                                                                                \
    return new cfastcdr_exn(exn);                                                                                      \
  }

struct cfastcdr_cdr : public Cdr {};
cfastcdr_cdr *cfastcdr_cdr_make(cfastcdr_fastbuffer *fb, uint8_t e, uint8_t t) {
  try {
    return static_cast<cfastcdr_cdr *>(new Cdr(*static_cast<FastBuffer *>(fb), Cdr::Endianness(e), Cdr::CdrType(t)));
  } catch (const std::exception &e) {
    return nullptr;
  }
}

void cfastcdr_cdr_destroy(cfastcdr_cdr *c) { delete static_cast<Cdr *>(c); }

cfastcdr_exn *cfastcdr_cdr_read_encapsulation(cfastcdr_cdr *c){CFASTCDR_TRY(c, read_encapsulation())}

cfastcdr_exn *cfastcdr_cdr_serialize_encapsulation(cfastcdr_cdr *c){CFASTCDR_TRY(c, serialize_encapsulation())}

CFASTCDR_DDS_CDR_PL_FLAG cfastcdr_cdr_get_dds_cdr_pl_flag(cfastcdr_cdr *c) {
  return static_cast<CFASTCDR_DDS_CDR_PL_FLAG>(static_cast<Cdr *>(c)->getDDSCdrPlFlag());
}

void cfastcdr_cdr_set_dds_cdr_pl_flag(cfastcdr_cdr *c, CFASTCDR_DDS_CDR_PL_FLAG f) {
  static_cast<Cdr *>(c)->setDDSCdrPlFlag(Cdr::DDSCdrPlFlag(f));
}

uint16_t cfastcdr_cdr_get_dds_cdr_options(cfastcdr_cdr *c) { return static_cast<Cdr *>(c)->getDDSCdrOptions(); }

void cfastcdr_cdr_set_dds_cdr_options(cfastcdr_cdr *c, uint16_t o) { static_cast<Cdr *>(c)->setDDSCdrOptions(o); }

CFASTCDR_ENDIANNESS cfastcdr_cdr_get_endianness(cfastcdr_cdr *c) {
  return static_cast<CFASTCDR_ENDIANNESS>(static_cast<Cdr *>(c)->endianness());
}

void cfastcdr_cdr_set_endianness(cfastcdr_cdr *c, CFASTCDR_ENDIANNESS e) {
  static_cast<Cdr *>(c)->changeEndianness(Cdr::Endianness(e));
}

bool cfastcdr_cdr_jump(cfastcdr_cdr *c, size_t s) { return static_cast<Cdr *>(c)->jump(s); }

void cfastcdr_cdr_reset(cfastcdr_cdr *c) { static_cast<Cdr *>(c)->reset(); }

char *cfastcdr_cdr_get_buffer_pointer(cfastcdr_cdr *c) { return static_cast<Cdr *>(c)->getBufferPointer(); }

char *cfastcdr_cdr_get_current_position(cfastcdr_cdr *c) { return static_cast<Cdr *>(c)->getCurrentPosition(); }

size_t cfastcdr_cdr_get_serialized_data_length(cfastcdr_cdr *c) {
  return static_cast<Cdr *>(c)->getSerializedDataLength();
}

size_t cfastcdr_cdr_alignment(size_t a, size_t s) { return Cdr::alignment(a, s); }

bool cfastcdr_cdr_move_alignment_forward(cfastcdr_cdr *c, size_t s) {
  return static_cast<Cdr *>(c)->moveAlignmentForward(s);
}

void cfastcdr_cdr_reset_alignment(cfastcdr_cdr *c) { static_cast<Cdr *>(c)->resetAlignment(); }

#define CFASTCDR_SERIALIZATION_FOR_BITS_TYPE(TYPE)                                                                     \
  cfastcdr_exn *cfastcdr_serialize_##TYPE(cfastcdr_cdr *cdr, TYPE value){CFASTCDR_TRY(cdr, serialize(value))}          \
                                                                                                                       \
  cfastcdr_exn *cfastcdr_serialize_endian_##TYPE(cfastcdr_cdr *cdr, CFASTCDR_ENDIANNESS endianness, TYPE value){       \
      CFASTCDR_TRY(cdr, serialize(value, Cdr::Endianness(endianness)))}                                                \
                                                                                                                       \
  cfastcdr_exn *cfastcdr_deserialize_##TYPE(cfastcdr_cdr *cdr, TYPE *value){CFASTCDR_TRY(cdr, deserialize(*value))}    \
                                                                                                                       \
  cfastcdr_exn *cfastcdr_deserialize_endian_##TYPE(cfastcdr_cdr *cdr, CFASTCDR_ENDIANNESS endianness, TYPE *value){    \
      CFASTCDR_TRY(cdr, deserialize(*value, Cdr::Endianness(endianness)))}                                             \
                                                                                                                       \
  cfastcdr_exn *cfastcdr_serialize_array_##TYPE(cfastcdr_cdr *cdr, const TYPE *array,                                  \
                                                size_t size){CFASTCDR_TRY(cdr, serializeArray(array, size))}           \
                                                                                                                       \
  cfastcdr_exn *cfastcdr_serialize_endian_array_##TYPE(cfastcdr_cdr *cdr, CFASTCDR_ENDIANNESS endianness,              \
                                                       const TYPE *array, size_t size){                                \
      CFASTCDR_TRY(cdr, serializeArray(array, size, Cdr::Endianness(endianness)))}                                     \
                                                                                                                       \
  cfastcdr_exn *cfastcdr_deserialize_array_##TYPE(cfastcdr_cdr *cdr, TYPE *array,                                      \
                                                  size_t size){CFASTCDR_TRY(cdr, deserializeArray(array, size))}       \
                                                                                                                       \
  cfastcdr_exn *cfastcdr_deserialize_endian_array_##TYPE(cfastcdr_cdr *cdr, CFASTCDR_ENDIANNESS endianness,            \
                                                         TYPE *array, size_t size) {                                   \
    CFASTCDR_TRY(cdr, deserializeArray(array, size, Cdr::Endianness(endianness)))                                      \
  }

CFASTCDR_SERIALIZATION_FOR_BITS_TYPES

#undef CFASTCDR_SERIALIZATION_FOR_BITS_TYPE
