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

/* Cdr::CdrType */
extern const uint8_t corba_cdr;
extern const uint8_t dds_cdr;

/* Cdr::DDSCdrPlFlag */
extern const uint8_t dds_cdr_without_pl;
extern const uint8_t dds_cdr_with_pl;

/* Cdr::Endianness */
extern const uint8_t big_endianness;
extern const uint8_t little_endianness;
extern const uint8_t default_endianness;

/* possible values returned from `exn_type` */
extern const uint8_t exn_success;
extern const uint8_t exn_not_enough_memory;
extern const uint8_t exn_bad_param;
extern const uint8_t exn_unknown;

/*
exception

Certain functions in this interface have C++ implementations that may throw a C++ exception. These functions all return
a `void*` and have `_exn` appended to their name.

If the return value  is `NULL` then the call was successful.

Otherwise, a handle to an exception object is returned. It can be queried with the following interface, after which it
MUST BE DESTROYED by calling `exn_destroy`.

All `exn_*` functions may be safely invoked on `NULL` values.
*/
uint8_t exn_type(void *exn);
const char *exn_message(void *exn);
void exn_destroy(void *exn);

/*
`FastBuffer` constructors

It is the caller's responsibility to ensure:
 - a non-zero pointer has been returned.
*/
void *fb_make0();                   /* owns and manages its memory */
void *fb_make(char *buf, size_t s); /* does not own its memory */

/*
For the following functions it is the caller's responsibility to ensure the first argument is a pointer to a valid
`FastBuffer` object. I.e., it is a value:
 - returned by `fb_make[0]`,
 - is not `NULL`,
 - has not yet been passed to `fb_destroy`.
*/
void fb_destroy(void *fb);
char *fb_get_buffer(void *fb);
size_t fb_get_buffer_size(void *fb);
bool fb_reserve(void *fb, size_t s);
bool fb_resize(void *fb, size_t s);

/*
`Cdr` constructor

It is the caller's responsibility to ensure:
 - the first argument is a valid pointer to a `FastBuffer` object;
 - the second argument is a valid `Cdr::Endianness` value;
 - the third argument is a valid `Cdr::CdrType` value;
 - a non-zero pointer has been returned.
*/
void *cdr_make(void *fb, uint8_t e, uint8_t t);

/*
For all remaining functions declared in this header, it is the caller's responsibility to ensure:
 - the first argument is a pointer to a valid `Cdr` object, i.e., it is a value:
    - returned by `cdr_make`,
    - is not `NULL`,
    - has not yet been passed to `cdr_destroy`.
 - an appropriate value is passed whereever an enumeration type is indicated with a comment.
*/
void cdr_destroy(void *cdr);
void *cdr_read_encapsulation_exn(void *cdr);
void *cdr_serialize_encapsulation_exn(void *cdr);
uint8_t cdr_get_dds_cdr_pl_flag(void *cdr);
void cdr_set_dds_cdr_pl_flag(void *cdr, uint8_t f /* `Cdr::DDSCdrPlFlag` */);
uint16_t cdr_get_dds_cdr_options(void *cdr);
void cdr_set_dds_cdr_options(void *cdr, uint16_t o);
uint8_t cdr_get_endianness(void *cdr);
void cdr_set_endianness(void *cdr, uint8_t e /* `Cdr::Endianness` */);
bool cdr_jump(void *cdr, size_t s);
void cdr_reset(void *cdr);
char *cdr_get_buffer_pointer(void *cdr);
char *cdr_get_current_position(void *cdr);
size_t cdr_get_serialized_data_length(void *cdr);
size_t cdr_alignment(size_t a, size_t s);
bool cdr_move_alignment_forward(void *cdr, size_t s);
void cdr_reset_alignment(void *cdr);

/*
[de]serialization for bits types

In addition to responsibilities previously mentioned, it is the responsibility of the caller to ensure:
 - the second argument points to a sufficient amount of validly allocated memory.

The [de]serialization operations exposed in this minimal interface are for arrays only. [De]serialization of a scalar
can be achieved by calling cdr_[de]serialize_array_* with size 1.
*/

#define FASTCDR_SERIALIZATION_FOR_BITS_TYPE(TYP)                                                                       \
  void *cdr_serialize_array_##TYP##_exn(void *cdr, const TYP *d, size_t s);                                            \
  void *cdr_serialize_array_with_endianness_##TYP##_exn(void *cdr, const TYP *d, size_t s,                             \
                                                        uint8_t e /* `Cdr::Endianness` */);                            \
  void *cdr_deserialize_array_##TYP##_exn(void *cdr, TYP *d, size_t s);                                                \
  void *cdr_deserialize_array_with_endianness_##TYP##_exn(void *cdr, TYP *d, size_t s,                                 \
                                                          uint8_t e /* `Cdr::Endianness` */);

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

/*
serialization for strings

The null-terminator is explicitly serialized with a `cstring` but not a `cwstring`. That is, serialization for a
`cwstring` is semantically equivalent to invoking `cdr_serialize_array_wchar_t_exn` on the non-null characters, while
serializing a `cstring` is equivalent to invoking `cdr_serialize_array_char_exn` on all the characters, including the
trailing null.

`c[w]string`s may be [de]serialized by:
  1. invoking `cdr_[de]serialize[_with_endianness]_uint32_t_exn` for string length (explicitly counting the null
     character in the case of `cstring`);
  2. invoking `cdr_[de]serialize_array[_with_endianness]_[char|wchar_t]_exn` with appropriate arguments.

[de]serialization of other structured values

 - A record (e.g., `class` or `struct`) is [de]serialized field-wise in the order declared in the IDL.
 - The new fields declared in a child record are [de]serialized after those of its parent record.
 - A [fixed-length] sequence (e.g., `std::array`) is [de]serialized in a way equivalent to invoking the serialization on
   each value in succession, i.e., equivalent to the semantics of `cdr_[de]serialize_array_*`.
 - A variable-length sequence (e.g., `std::vector`) is [de]serialized by [de]serializing its length as a `uint32_t`
   followed by [de]serializing its values as though it were a fixed-length sequence.
 - A dictionary (e.g., `std::map`) is [de]serialized as though it were a variable-length sequence of key/value pairs,
   where a pair is [de]serialized as its key followed by its value.
*/

#ifdef __cplusplus
} /* extern "C" */
#endif