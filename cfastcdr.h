#include <inttypes.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

extern const uint8_t corba_cdr;
extern const uint8_t dds_cdr;
extern const uint8_t dds_cdr_without_pl;
extern const uint8_t dds_cdr_with_pl;
extern const uint8_t big_endianness;
extern const uint8_t little_endianness;
extern const uint8_t default_endianness;

/* fast buffer */
void *fast_buffer_make0();
void *fast_buffer_make(char *, size_t);
void fast_buffer_destroy(void *);
char *fast_buffer_get_buffer(void *);
size_t fast_buffer_get_buffer_size(void *);
bool fast_buffer_reserve(void *, size_t);
bool fast_buffer_resize(void *, size_t);

/* cdr */
void *cdr_make(void *, uint8_t, uint8_t);
void cdr_destroy(void *);
const char *cdr_get_last_exception_message(void *);
bool cdr_read_encapsulation(void *);
bool cdr_serialize_encapsulation(void *);
uint8_t cdr_get_dds_cdr_pl_flag(void *);
void cdr_set_dds_cdr_pl_flag(void *, uint8_t);
uint16_t cdr_get_dds_cdr_options(void *);
void cdr_set_dds_cdr_options(void *, uint16_t);
uint8_t cdr_get_endianness(void *);
void cdr_set_endianness(void *, uint8_t);
bool cdr_jump(void *, size_t);
void cdr_reset(void *);
char *cdr_get_buffer_pointer(void *);
char *cdr_get_current_position(void *);
size_t cdr_get_serialized_data_length(void *);
size_t cdr_alignment(size_t, size_t);
bool cdr_move_alignment_forward(void *, size_t);
void cdr_reset_alignment(void *);

#define FASTCDR_SERIALIZATION_FOR_BITS_TYPE(TYP)                                                                       \
  bool cdr_serialize_##TYP(void *, TYP);                                                                               \
  bool cdr_serialize_with_endianness_##TYP(void *, TYP, uint8_t);                                                      \
  bool cdr_deserialize_##TYP(void *, TYP *);                                                                           \
  bool cdr_deserialize_with_endianness_##TYP(void *, TYP *, uint8_t);                                                  \
  bool cdr_serialize_array_##TYP(void *, const TYP *, size_t);                                                         \
  bool cdr_serialize_array_with_endianness_##TYP(void *, const TYP *, size_t, uint8_t);                                \
  bool cdr_deserialize_array_##TYP(void *, TYP *, size_t);                                                             \
  bool cdr_deserialize_array_with_endianness_##TYP(void *, TYP *, size_t, uint8_t);

typedef long double long_double;

#define FASTCDR_SERIALIZATION_FOR_BITS_TYPES                                                                           \
  FASTCDR_SERIALIZATION_FOR_BITS_TYPE(uint8_t)                                                                         \
  FASTCDR_SERIALIZATION_FOR_BITS_TYPE(char)                                                                            \
  FASTCDR_SERIALIZATION_FOR_BITS_TYPE(int8_t)                                                                          \
  FASTCDR_SERIALIZATION_FOR_BITS_TYPE(uint16_t)                                                                        \
  FASTCDR_SERIALIZATION_FOR_BITS_TYPE(int16_t)                                                                         \
  FASTCDR_SERIALIZATION_FOR_BITS_TYPE(uint32_t)                                                                        \
  FASTCDR_SERIALIZATION_FOR_BITS_TYPE(int32_t)                                                                         \
  FASTCDR_SERIALIZATION_FOR_BITS_TYPE(wchar_t)                                                                         \
  FASTCDR_SERIALIZATION_FOR_BITS_TYPE(uint64_t)                                                                        \
  FASTCDR_SERIALIZATION_FOR_BITS_TYPE(int64_t)                                                                         \
  FASTCDR_SERIALIZATION_FOR_BITS_TYPE(float)                                                                           \
  FASTCDR_SERIALIZATION_FOR_BITS_TYPE(double)                                                                          \
  FASTCDR_SERIALIZATION_FOR_BITS_TYPE(long_double)                                                                     \
  FASTCDR_SERIALIZATION_FOR_BITS_TYPE(bool)

FASTCDR_SERIALIZATION_FOR_BITS_TYPES

#undef FASTCDR_SERIALIZATION_FOR_BITS_TYPE

/* string (de-)serialization */
bool cdr_serialize_cstring(void *, const char *);
bool cdr_serialize_with_endianness_cstring(void *, const char *, uint8_t);
bool cdr_deserialize_cstring(void *, char **);
bool cdr_deserialize_with_endianness_cstring(void *, char **, uint8_t);
bool cdr_serialize_wcstring(void *, const wchar_t *);
bool cdr_serialize_with_endianness_cwstring(void *, const wchar_t *, uint8_t);
bool cdr_deserialize_wcstring(void *, wchar_t **);
bool cdr_deserialize_with_endianness_cwstring(void *, wchar_t **, uint8_t);

#ifdef __cplusplus
} /* extern "C" */
#endif