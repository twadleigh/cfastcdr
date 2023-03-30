#include "cfastcdr.h"

#include <memory>

#include <fastcdr/Cdr.h>
#include <fastcdr/FastBuffer.h>
#include <fastcdr/exceptions/BadParamException.h>

using namespace eprosima::fastcdr;

/* constants */
const uint8_t corba_cdr = static_cast<uint8_t>(Cdr::CORBA_CDR);
const uint8_t dds_cdr = static_cast<uint8_t>(Cdr::DDS_CDR);
const uint8_t dds_cdr_without_pl =
    static_cast<uint8_t>(Cdr::DDS_CDR_WITHOUT_PL);
const uint8_t dds_cdr_with_pl = static_cast<uint8_t>(Cdr::DDS_CDR_WITH_PL);
const uint8_t big_endianness = static_cast<uint8_t>(Cdr::BIG_ENDIANNESS);
const uint8_t little_endianness = static_cast<uint8_t>(Cdr::LITTLE_ENDIANNESS);
const uint8_t default_endianness = static_cast<uint8_t>(Cdr::DEFAULT_ENDIAN);

/* fast buffer */
void *fast_buffer_make0() { return static_cast<void *>(new FastBuffer()); }

void *fast_buffer_make(char *c, size_t s) {
  return static_cast<void *>(new FastBuffer(c, s));
}

void fast_buffer_destroy(void *fb) { delete static_cast<FastBuffer *>(fb); }

char *fast_buffer_get_buffer(void *fb) {
  return static_cast<FastBuffer *>(fb)->getBuffer();
}

size_t fast_buffer_get_buffer_size(void *fb) {
  return static_cast<FastBuffer *>(fb)->getBufferSize();
}

bool fast_buffer_reserve(void *fb, size_t s) {
  return static_cast<FastBuffer *>(fb)->reserve(s);
}

bool fast_buffer_resize(void *fb, size_t s) {
  return static_cast<FastBuffer *>(fb)->resize(s);
}

/* cdr */
struct CdrWrapper {
  std::unique_ptr<Cdr> cdr;
  std::string last_exception_message;

  CdrWrapper(void *fb, uint8_t e, uint8_t t)
      : cdr(new Cdr(*static_cast<FastBuffer *>(fb), Cdr::Endianness(e),
                    Cdr::CdrType(t))) {}
};

#define FASTCDR_TRY(c, body)                                                   \
  CdrWrapper *wrapper_ptr = static_cast<CdrWrapper *>(c);                      \
  try {                                                                        \
    wrapper_ptr->cdr->body;                                                    \
    return true;                                                               \
  } catch (const exception::Exception &exn) {                                  \
    wrapper_ptr->last_exception_message = std::string(exn.what());             \
    return false;                                                              \
  }

void *cdr_make(void *fb, uint8_t e, uint8_t t) {
  return static_cast<void *>(new CdrWrapper(fb, e, t));
};

void cdr_destroy(void *c) { delete static_cast<CdrWrapper *>(c); }

const char *cdr_get_last_exception_message(void *c) {
  return static_cast<CdrWrapper *>(c)->last_exception_message.c_str();
}

bool cdr_read_encapsulation(void *c) { FASTCDR_TRY(c, read_encapsulation()) }

bool cdr_serialize_encapsulation(void *c){
    FASTCDR_TRY(c, serialize_encapsulation())}

uint8_t cdr_get_dds_cdr_pl_flag(void *c) {
  return static_cast<uint8_t>(static_cast<Cdr *>(c)->getDDSCdrPlFlag());
}

void cdr_set_dds_cdr_pl_flag(void *c, uint8_t f) {
  static_cast<CdrWrapper *>(c)->cdr->setDDSCdrPlFlag(Cdr::DDSCdrPlFlag(f));
}

uint16_t cdr_get_dds_cdr_options(void *c) {
  return static_cast<CdrWrapper *>(c)->cdr->getDDSCdrOptions();
}

void cdr_set_dds_cdr_options(void *c, uint16_t o) {
  static_cast<CdrWrapper *>(c)->cdr->setDDSCdrOptions(o);
}

uint8_t cdr_get_endianness(void *c) {
  return static_cast<uint8_t>(static_cast<CdrWrapper *>(c)->cdr->endianness());
}

void cdr_set_endianness(void *c, uint8_t e) {
  static_cast<CdrWrapper *>(c)->cdr->changeEndianness(Cdr::Endianness(e));
}

bool cdr_jump(void *c, size_t s) {
  return static_cast<CdrWrapper *>(c)->cdr->jump(s);
}

void cdr_reset(void *c) { static_cast<CdrWrapper *>(c)->cdr->reset(); }

char *cdr_get_buffer_pointer(void *c) {
  return static_cast<CdrWrapper *>(c)->cdr->getBufferPointer();
}

char *cdr_get_current_position(void *c) {
  return static_cast<CdrWrapper *>(c)->cdr->getCurrentPosition();
}

size_t cdr_get_serialized_data_length(void *c) {
  return static_cast<CdrWrapper *>(c)->cdr->getSerializedDataLength();
}

size_t cdr_alignment(size_t a, size_t s) { return Cdr::alignment(a, s); }

bool cdr_move_alignment_forward(void *c, size_t s) {
  return static_cast<CdrWrapper *>(c)->cdr->moveAlignmentForward(s);
}

void cdr_reset_alignment(void *c) {
  static_cast<CdrWrapper *>(c)->cdr->resetAlignment();
}

#define FASTCDR_SERIALIZATION_FOR_BITS_TYPE(TYP)                               \
  bool cdr_serialize_##TYP(void *c, TYP d) { FASTCDR_TRY(c, serialize(d)) }    \
  bool cdr_serialize_with_endianness_##TYP(void *c, TYP d, uint8_t e) {        \
    FASTCDR_TRY(c, serialize(d, Cdr::Endianness(e)))                           \
  }                                                                            \
  bool cdr_deserialize_##TYP(void *c, TYP *d) {                                \
    FASTCDR_TRY(c, deserialize(*d))                                            \
  }                                                                            \
  bool cdr_deserialize_with_endianness_##TYP(void *c, TYP *d, uint8_t e) {     \
    FASTCDR_TRY(c, deserialize(*d, Cdr::Endianness(e)))                        \
  }                                                                            \
  bool cdr_serialize_array_##TYP(void *c, const TYP *d, size_t s) {            \
    FASTCDR_TRY(c, serializeArray(d, s))                                       \
  }                                                                            \
  bool cdr_serialize_array_with_endianness_##TYP(void *c, const TYP *d,        \
                                                 size_t s, uint8_t e) {        \
    FASTCDR_TRY(c, serializeArray(d, s, Cdr::Endianness(e)))                   \
  }                                                                            \
  bool cdr_deserialize_array_##TYP(void *c, TYP *d, size_t s) {                \
    FASTCDR_TRY(c, deserializeArray(d, s))                                     \
  }                                                                            \
  bool cdr_deserialize_array_with_endianness_##TYP(void *c, TYP *d, size_t s,  \
                                                   uint8_t e) {                \
    FASTCDR_TRY(c, deserializeArray(d, s, Cdr::Endianness(e)))                 \
  }

FASTCDR_SERIALIZATION_FOR_BITS_TYPES

#undef FASTCDR_SERIALIZATION_FOR_BITS_TYPE

bool cdr_serialize_cstring(void *c, const char *d) {
  FASTCDR_TRY(c, serialize(d))
}

bool cdr_serialize_with_endianness_cstring(void *c, const char *d, uint8_t e) {
  FASTCDR_TRY(c, serialize(d, Cdr::Endianness(e)))
}

bool cdr_deserialize_cstring(void *c, char **d) {
  FASTCDR_TRY(c, deserialize(*d))
}

bool cdr_deserialize_with_endianness_cstring(void *c, char **d, uint8_t e) {
  FASTCDR_TRY(c, deserialize(*d, Cdr::Endianness(e)))
}

bool cdr_serialize_wcstring(void *c, const wchar_t *d) {
  FASTCDR_TRY(c, serialize(d))
}

bool cdr_serialize_with_endianness_wcstring(void *c, const wchar_t *d,
                                            uint8_t e) {
  FASTCDR_TRY(c, serialize(d, Cdr::Endianness(e)))
}

bool cdr_deserialize_cwstring(void *c, wchar_t **d) {
  FASTCDR_TRY(c, deserialize(*d))
}

bool cdr_deserialize_with_endianness_cwstring(void *c, wchar_t **d, uint8_t e) {
  FASTCDR_TRY(c, deserialize(*d, Cdr::Endianness(e)))
}
