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
#include <inttypes.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum { CFASTCDR_CORBA_CDR, CFASTCDR_DDS_CCR } CFASTCDR_CDR_TYPE;

typedef enum { CFASTCDR_DDS_CDR_WITHOUT_PL = 0, CFASTCDR_DDS_CDR_WITH_PL = 2 } CFASTCDR_DDS_CDR_PL_FLAG;

typedef enum { CFASTCDR_BIG_ENDIANNESS = 0, CFASTCDR_LITTLE_ENDIANNESS = 1 } CFASTCDR_ENDIANNESS;

typedef enum {
  CFASTCDR_EXN_SUCCESS,
  CFASTCDR_EXN_NOT_ENOUGH_MEMORY,
  CFASTCDR_EXN_BAD_PARAM,
  CFASTCDR_EXN_UNKNOWN
} CFASTCDR_EXN_TYPE;

extern const CFASTCDR_ENDIANNESS CFASTCDR_DEFAULT_ENDIANNESS;

struct cfastcdr_exn;
/* exn functions may be safely called on `NULL` values */
CFASTCDR_EXN_TYPE cfastcdr_exn_type(cfastcdr_exn *exn);
const char *cfastcdr_exn_message(cfastcdr_exn *exn);
void cfastcdr_exn_destroy(cfastcdr_exn *exn);

struct cfastcdr_fastbuffer;
cfastcdr_fastbuffer *cfastcdr_fastbuffer_make_variable();                 /* owns and manages its memory */
cfastcdr_fastbuffer *cfastcdr_fastbuffer_make_fixed(char *buf, size_t s); /* does not own its memory */
char *cfastcdr_fastbuffer_get_buffer(cfastcdr_fastbuffer *fastbuffer);
size_t cfastcdr_fastbuffer_get_buffer_size(cfastcdr_fastbuffer *fastbuffer);
bool cfastcdr_fastbuffer_reserve(cfastcdr_fastbuffer *fastbuffer, size_t size);
bool cfastcdr_fastbuffer_resize(cfastcdr_fastbuffer *fastbuffer, size_t size);
void cfastcdr_fastbuffer_destroy(cfastcdr_fastbuffer *fastbuffer);

struct cfastcdr_cdr;
cfastcdr_cdr *cfastcdr_cdr_make(cfastcdr_fastbuffer *fastbuffer, CFASTCDR_ENDIANNESS endianness,
                                CFASTCDR_CDR_TYPE cdr_type);
cfastcdr_exn *cfastcdr_cdr_read_encapsulation(cfastcdr_cdr *cdr);
cfastcdr_exn *cfastcdr_cdr_serialize_encapsulation(cfastcdr_cdr *cdr);
CFASTCDR_DDS_CDR_PL_FLAG cfastcdr_cdr_get_dds_cdr_pl_flag(cfastcdr_cdr *cdr);
void cfastcdr_cdr_set_dds_cdr_pl_flag(cfastcdr_cdr *cdr, CFASTCDR_DDS_CDR_PL_FLAG dds_cdr_pl_flag);
uint16_t cfastcdr_cdr_get_dds_cdr_options(cfastcdr_cdr *cdr);
void cfastcdr_cdr_set_dds_cdr_options(cfastcdr_cdr *cdr, uint16_t dds_cdr_options);
CFASTCDR_ENDIANNESS cfastcdr_cdr_get_endianness(cfastcdr_cdr *cdr);
void cfastcdr_cdr_set_endianness(cfastcdr_cdr *cdr, CFASTCDR_ENDIANNESS endianness);
bool cfastcdr_cdr_jump(cfastcdr_cdr *cdr, size_t size);
void cfastcdr_cdr_reset(cfastcdr_cdr *cdr);
char *cfastcdr_cdr_get_buffer_pointer(cfastcdr_cdr *cdr);
char *cfastcdr_cdr_get_current_position(cfastcdr_cdr *cdr);
size_t cfastcdr_cdr_get_serialized_data_length(cfastcdr_cdr *cdr);
size_t cfastcdr_cdr_alignment(size_t current_alignment, size_t size);
bool cfastcdr_cdr_move_alignment_forward(cfastcdr_cdr *cdr, size_t size);
void cfastcdr_cdr_reset_alignment(cfastcdr_cdr *cdr);
void cfastcdr_cdr_destroy(cfastcdr_cdr *cdr);

#define CFASTCDR_SERIALIZATION_FOR_BITS_TYPE(TYPE)                                                                     \
  cfastcdr_exn *cfastcdr_serialize_##TYPE(cfastcdr_cdr *cdr, TYPE value);                                              \
                                                                                                                       \
  cfastcdr_exn *cfastcdr_serialize_endian_##TYPE(cfastcdr_cdr *cdr, CFASTCDR_ENDIANNESS endianness, TYPE value);       \
                                                                                                                       \
  cfastcdr_exn *cfastcdr_deserialize_##TYPE(cfastcdr_cdr *cdr, TYPE *value);                                           \
                                                                                                                       \
  cfastcdr_exn *cfastcdr_deserialize_endian_##TYPE(cfastcdr_cdr *cdr, CFASTCDR_ENDIANNESS endianness, TYPE *value);    \
                                                                                                                       \
  cfastcdr_exn *cfastcdr_serialize_array_##TYPE(cfastcdr_cdr *cdr, const TYPE *array, size_t size);                    \
                                                                                                                       \
  cfastcdr_exn *cfastcdr_serialize_endian_array_##TYPE(cfastcdr_cdr *cdr, CFASTCDR_ENDIANNESS endianness,              \
                                                       const TYPE *array, size_t size);                                \
                                                                                                                       \
  cfastcdr_exn *cfastcdr_deserialize_array_##TYPE(cfastcdr_cdr *cdr, TYPE *array, size_t size);                        \
                                                                                                                       \
  cfastcdr_exn *cfastcdr_deserialize_endian_array_##TYPE(cfastcdr_cdr *cdr, CFASTCDR_ENDIANNESS endianness,            \
                                                         TYPE *array, size_t size);

#define CFASTCDR_SERIALIZATION_FOR_BITS_TYPES                                                                          \
  CFASTCDR_SERIALIZATION_FOR_BITS_TYPE(bool)                                                                           \
  CFASTCDR_SERIALIZATION_FOR_BITS_TYPE(char)                                                                           \
  CFASTCDR_SERIALIZATION_FOR_BITS_TYPE(int8_t)                                                                         \
  CFASTCDR_SERIALIZATION_FOR_BITS_TYPE(uint8_t)                                                                        \
  CFASTCDR_SERIALIZATION_FOR_BITS_TYPE(int16_t)                                                                        \
  CFASTCDR_SERIALIZATION_FOR_BITS_TYPE(uint16_t)                                                                       \
  CFASTCDR_SERIALIZATION_FOR_BITS_TYPE(int32_t)                                                                        \
  CFASTCDR_SERIALIZATION_FOR_BITS_TYPE(uint32_t)                                                                       \
  CFASTCDR_SERIALIZATION_FOR_BITS_TYPE(int64_t)                                                                        \
  CFASTCDR_SERIALIZATION_FOR_BITS_TYPE(uint64_t)                                                                       \
  CFASTCDR_SERIALIZATION_FOR_BITS_TYPE(float)                                                                          \
  CFASTCDR_SERIALIZATION_FOR_BITS_TYPE(double)

CFASTCDR_SERIALIZATION_FOR_BITS_TYPES

#undef CFASTCDR_SERIALIZATION_FOR_BITS_TYPE

#ifdef __cplusplus
} /* extern "C" */
#endif