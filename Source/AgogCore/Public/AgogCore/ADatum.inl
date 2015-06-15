//=======================================================================================
// Agog Labs C++ library.
// Copyright (c) 2000 Agog Labs Inc.,
// All rights reserved.
//
// ADatum class - Objects that need to be persistent (saved and loaded), or converted to
// to a contiguous memory block form should have conversion methods to and from ADatum.
// 
// Author(s):  Conan Reis
//=======================================================================================


//=======================================================================================
// Includes
//=======================================================================================

#include <AgogCore/ABinaryParse.hpp>
#include <AgogCore/AString.hpp>
#include <AgogCore/AObjReusePool.hpp>


//=======================================================================================
// ADatum::Reference Inline Methods
//=======================================================================================

//---------------------------------------------------------------------------------------
// Author(s):  Conan Reis
A_INLINE ADatum::Reference::Reference(uint32_t data_length) :
  m_references(1u)
  {
  if (data_length)
    {
    uint32_t size = ADatum_header_size + data_length;

    m_term     = ATerm_short;
    m_size     = size;
    m_buffer_p = alloc_buffer(size);
    A_BYTE_STREAM32(m_buffer_p, &size);
    }
  else
    {
    m_term     = ATerm_long;
    m_size     = 4u;
    m_buffer_p = reinterpret_cast<uint8_t *>(&ms_bytes4);
    }
  }

//---------------------------------------------------------------------------------------
// Author(s):  Conan Reis
A_INLINE ADatum::Reference::Reference(
  void *   buffer_p,
  uint32_t size,
  eATerm   term // = ATerm_short
  ) :
  m_references(1u),
  m_size(size),
  m_buffer_p(buffer_p ? reinterpret_cast<uint8_t *>(buffer_p) : alloc_buffer(size)),
  m_term(term)
  {}


//=======================================================================================
// ADatum Inline Methods
//=======================================================================================

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Common Methods
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//---------------------------------------------------------------------------------------
// Copy constructor
// 
// Params: 
//   datum: ADatum to copy
//   
// Returns:   itself
// Author(s):  Conan Reis
A_INLINE ADatum::ADatum(const ADatum & datum) :
  m_dref_p(datum.m_dref_p)
  {
  m_dref_p->m_references++;
  }

//---------------------------------------------------------------------------------------
// Destructor - decrements reference count to buffer.
// 
// Author(s):  Conan Reis
A_INLINE ADatum::~ADatum()
  {
  m_dref_p->decrement();
  }

//---------------------------------------------------------------------------------------
// Assignment operator
// 
// Params: 
//   datum: ADatum to copy
//   
// Returns:    itself
// Examples:   `datum1 = datum2 = datum3;`
// Author(s):  Conan Reis
A_INLINE ADatum & ADatum::operator=(const ADatum & datum)
  {
  datum.m_dref_p->m_references++;
  m_dref_p->decrement();
  m_dref_p = datum.m_dref_p;

  return *this;
  }


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Converter Methods
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//---------------------------------------------------------------------------------------
// Cast / convert a ADatum to a constant pointer to the buffer (prefixed with the size
// header).
// 
// Returns:   constant pointer to the buffer.
// 
// Notes:     Same as get_buffer()
// 
// See: 
//   get_buffer(), get_buffer_writable(), get_data(), get_data_writable(), set_data()
//   
// Author(s):  Conan Reis
A_INLINE ADatum::operator const uint8_t * () const
  {
  return m_dref_p->m_buffer_p;
  }

//---------------------------------------------------------------------------------------
// Creates a string representation of itself
// 
// Returns:    itself as a AString 
// Author(s):  Conan Reis
A_INLINE AString ADatum::as_string() const
  {
  return AString(64u, "ADatum(%u bytes)[ref# %u]", m_dref_p->m_size, m_dref_p->m_references);
  }


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Accessor Methods
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//---------------------------------------------------------------------------------------
// Returns a constant pointer to the buffer (prefixed with the size header).
// 
// Returns:    constant pointer to the buffer.
// See:        get_data(), get_data_writable(), get_buffer_writable(), set_data(),
// Author(s):  Conan Reis
A_INLINE const uint8_t * ADatum::get_buffer() const
  {
  return m_dref_p->m_buffer_p;
  }

//---------------------------------------------------------------------------------------
// Returns a pointer to its buffer (prefixed with the size header) and ensures that this
// ADatum is the only ADatum object referring to its buffer.
// 
// Returns:  pointer to its buffer.
// Params: 
//   keep_data:
//     if a new buffer is allocated this indicates whether the current data should be
//     preserved (true) or if it is permissible for the data to be lost making the values
//     of the buffer - including the size header - undefined (false).  If keep_data is
//     true and min_length is less than the currently stored data and a new buffer needs
//     to be allocated due to this ADatum sharing a buffer with other Datums then only
//     the min_length will be allocated and part of the data will be truncated.
//   min_length:
//     minimum required data length in bytes (i.e. does not include the size header).  If
//     min_length is set to ADatum_use_current_size, the size of the currently allocated
//     buffer is used.
//     
// See:        get_buffer(), get_data(), get_data_writable(), set_data(), set_data()
// Author(s):  Conan Reis
A_INLINE uint8_t * ADatum::get_buffer_writable(
  bool     keep_data, // = true
  uint32_t min_length // = ADatum_use_current_size
  )
  {
  if (min_length == ADatum_use_current_size)
    {
    min_length = m_dref_p->m_size - ADatum_header_size;
    }

  ensure_size(min_length, keep_data);

  return m_dref_p->m_buffer_p;
  }

//---------------------------------------------------------------------------------------
// Returns a read-only pointer to the data portion of the buffer.
// 
// Returns:  constant pointer to the data portion of the buffer.
// See:  get_buffer(), get_data_writable(), get_buffer_writable(), set_data(), set_data()
// Author(s):  Conan Reis
A_INLINE const uint8_t * ADatum::get_data() const
  {
  return m_dref_p->m_buffer_p + ADatum_header_size;
  }

//---------------------------------------------------------------------------------------
// Returns a pointer to the data portion of its buffer and ensures that this ADatum is
// the only ADatum object referring to its buffer.
// 
// Returns:   pointer to the data portion of its buffer
// Params: 
//   keep_data:
//     if a new buffer is allocated this indicates whether the current data should be
//     preserved (true) or if it is permissible for the data to be lost making the values
//     of the buffer - including the size header - undefined (false).  If keep_data is
//     true and min_length is less than the currently stored data and a new buffer needs
//     to be allocated due to this ADatum sharing a buffer with other Datums then only
//     the min_length will be allocated and part of the data will be truncated.
//   min_length:
//     minimum required data length in bytes (i.e. does not include the size header).  If
//     min_length is set to ADatum_use_current_size, the size of the currently allocated
//     buffer is used.
//     
// See:        get_buffer(), get_data(), get_buffer_writable(), set_data(),  set_data()
// Author(s):  Conan Reis
A_INLINE uint8_t * ADatum::get_data_writable(
  bool     keep_data, // = true
  uint32_t min_length // = ADatum_use_current_size
  )
  {
  if (min_length == ADatum_use_current_size)
    {
    min_length = m_dref_p->m_size - ADatum_header_size;
    }

  ensure_size(min_length, keep_data);

  return m_dref_p->m_buffer_p + ADatum_header_size;
  }

//---------------------------------------------------------------------------------------
// Returns a pointer to the current end of the data portion of its buffer and ensures
// that this ADatum is the only ADatum object referring to its buffer and that there is
// enough space in the buffer for the current data + append_data_length.  It also
// increases the current data length by append_data_length.
// 
// Returns:  pointer to the current end of the data portion of its buffer
// Params: 
//   append_data_length:
//     ensures that this additional space is available at the end of the current data.
//   min_expand:
//     if the size needs to be increased and min_expand = true then the new size should be
//     just large enough, if min_expand = false then the new size is more than is
//     minimally needed (it doubles the current size until it is large enough) to allow
//     for additional expansion in the future without needing to reallocate.
//     
// See:        get_buffer(), get_data(), get_buffer_writable(), set_data(),  set_data()
// Author(s):  Conan Reis
A_INLINE uint8_t * ADatum::get_data_end_writable(
  uint32_t append_data_length,
  bool     min_expand // = true
  )
  {
  uint32_t length     = get_length();
  uint32_t new_length = length + append_data_length;

  ensure_size(new_length, true, min_expand);

  // Adjust length to include data to be appended
  A_BYTE_STREAM32(m_dref_p->m_buffer_p, &new_length);

  return m_dref_p->m_buffer_p + length;
  }

//---------------------------------------------------------------------------------------
// Returns length of filled data buffer.
// 
// Author(s):  Conan Reis
A_INLINE uint32_t ADatum::get_data_length() const
  {
  return A_BYTE_STREAM_UI32(m_dref_p->m_buffer_p) - ADatum_header_size;
  }

//---------------------------------------------------------------------------------------
// Returns length of filled data buffer including prefixed buffer length.
// 
// Author(s):  Conan Reis
A_INLINE uint32_t ADatum::get_length() const
  {
  return A_BYTE_STREAM_UI32(m_dref_p->m_buffer_p);
  }

//---------------------------------------------------------------------------------------
// Returns available size of complete buffer
// 
// Author(s):  Conan Reis
A_INLINE uint32_t ADatum::get_size() const
  {
  return m_dref_p->m_size;
  }

//---------------------------------------------------------------------------------------
// Returns true if data buffer is empty, false if not.
// 
// Author(s):  Conan Reis
A_INLINE bool ADatum::is_empty() const
  {
  return (get_length() == ADatum_header_size);
  }

//---------------------------------------------------------------------------------------
// Increments this ADatum's reference count once to indicate that the memory buffer used
// in the ADatum is owned elsewhere and that this ADatum may only read and not write to it
// and that this ADatum does not need to deallocate it.
// 
// Author(s):  Conan Reis
A_INLINE void ADatum::ref_count_increment()
  {
  m_dref_p->m_references++;
  }

//---------------------------------------------------------------------------------------
// Ensures that this ADatum is the only ADatum object referring to its buffer, copies
// data_p to the data portion, and resets the length in the size header of the buffer.
// 
// Params: 
//   data_p: pointer to data (excluding the size header)
//   data_length: length in bytes of data_p.
//   
// Author(s):  Conan Reis
A_INLINE void ADatum::set_data(
  const void * data_p,
  uint32_t     data_length
  )
  {
  ensure_size(data_length, false);

  // Set length
  set_data_length(data_length);

  // Copy data
  memcpy(m_dref_p->m_buffer_p + ADatum_header_size, data_p, data_length);
  }

//---------------------------------------------------------------------------------------
// Sets length of filled data buffer.
// 
// Author(s):  Conan
A_INLINE void ADatum::set_data_length(uint32_t data_length)
  {
  uint32_t full_length = ADatum_header_size + data_length;

  #ifdef A_BOUNDS_CHECK
    A_VERIFY(full_length <= m_dref_p->m_size, "ADatum::set_data_length() - attempted to set length longer than size", AErrId_invalid_index, ADatum);
  #endif

  A_BYTE_STREAM32(m_dref_p->m_buffer_p, &full_length);
  }


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Class Methods
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//---------------------------------------------------------------------------------------
// Frees up a data block and puts it into the dynamic pool ready for its next use.  This
// should be used instead of 'delete' because it prevents unnecessary deallocations by
// saving previously allocated objects.
// 
// Params: 
//   datum_p:
//     pointer to data block to free up and return to the dynamic pool for its next use.
//     
// Notes:      To "allocate" an object use 'pool_new' rather than 'new'.
// Examples:   `ADatum::pool_delete(datum_p);`
// Author(s):  Conan Reis
A_INLINE void ADatum::pool_delete(ADatum * datum_p)
  {
  datum_p->m_dref_p->decrement();  // $Revisit - CReis Could keep this if this ADatum is the only one referencing it.
  datum_p->m_dref_p = nullptr;

  get_pool().append(datum_p);
  }

//---------------------------------------------------------------------------------------
// Retrieves a data block object from the dynamic pool and initializes it for use. This
// should be used instead of 'new' because it prevents unnecessary allocations by reusing
// previously allocated objects.
// 
// Returns:  a dynamic ADatum
// Params: 
//   data_p:
//     pointer to data (excluding the size header) or a pointer to a buffer (which
//     includes space for the size header).  It may point to nullptr in which case no data
//     is copied.  See "Notes" for the possible argument permutations and their
//     interpretations.
//   data_length:
//     length in bytes of the data part of the data_p.  If data_length is 
//     ALength_in_header, then data_p points to a complete buffer and contains a correct
//     length in its header.  See "Notes" for the possible argument permutations and their
//     interpretations.
//   size:
//     If size is not 0 and data_p is not nullptr, data_p is a buffer of size bytes to be
//     taken over (and eventually freed when no longer used) rather than allocating a new
//     buffer and copying the data.  The buffer is of the indicated size and contains
//     enough space for both the size header and any data. See "Notes" for the possible
//     argument permutations and their interpretations.
//     
// Examples: 
//   ```
//   ADatum * datum1_p = ADatum::pool_new(data_p, 7);  // allocate buffer (header_size + 7), set buffer_length to header_size + 7, and copy data_p to buffer
//   ADatum * datum2_p = ADatum::pool_new(buffer_p, ALength_in_header, ADatum_header_size + 7)  // take buffer_p over and use it directly
//   ```
//   
// Notes: 
//   To "deallocate" an object that was retrieved with this method, use pool_delete()
//   rather than 'delete'.
//
//   These are the possible permutations of the arguments and their interpretations:
//   ```
//                              Buffer size
//   # data_p data_length         or Data    Interpretation
//   1  nullptr   ALength_in_header  0   n/a     allocate(header_size) & buffer_length = header_size (Default constructor)
//   2 !nullptr   ALength_in_header  0   buffer  allocate(buffer_length) & copy buffer
//   3  nullptr  !ALength_in_header  0   n/a     allocate(length + header_size) & buffer_length = header_size + length
//   4 !nullptr  !ALength_in_header  0   data    allocate(length + header_size) & buffer_length = header_size + length & copy data
//   5  nullptr   ALength_in_header !0   n/a     allocate(size) & buffer_length = header_size
//   6 !nullptr   ALength_in_header !0   buffer  take over buffer
//   7  nullptr  !ALength_in_header !0   n/a     allocate(size) & buffer_length = header_size + length
//   8 !nullptr  !ALength_in_header !0   buffer  take over buffer & buffer_length = header_size + length
//   ```
//
// See:  ADatum(data_length_pair_count, ...)
// Author(s):  Conan Reis
A_INLINE ADatum * ADatum::pool_new(
  const void * data_p,      // = nullptr
  uint32_t     data_length, // = ALength_in_header
  uint32_t     size         // = 0u
  )
  {
  ADatum * datum_p = get_pool().pop();

  datum_p->set_buffer(data_p, data_length, size);

  return datum_p;
  }


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Class Internal Methods
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//---------------------------------------------------------------------------------------
// Doubles current_size until it is larger than needed and returns the result
// 
// Returns:   a size greater than or equal to needed
// Params: 
//   needed: amount of bytes that need to be stored
//   
// See:        alloc_buffer()
// Modifiers:  protected, static
// Author(s):  Conan Reis
A_INLINE uint32_t ADatum::calc_size(
  uint32_t needed,
  uint32_t current_size
  )
  {
  uint32_t size = current_size;

  while (size <= needed)
    {
    size <<= 1;  // Double size
    }

  return size;
  }

//---------------------------------------------------------------------------------------
// Returns a memory buffer.
// 
// See:        calc_size()
// Modifiers:  protected, static
// Author(s):  Conan Reis
A_INLINE uint8_t * ADatum::alloc_buffer(uint32_t buffer_size)
  {
  uint8_t * buffer_p = new ("ADatum::alloc_buffer") uint8_t[buffer_size];

  A_VERIFY_MEMORY(buffer_p != nullptr, ADatum);

  return buffer_p;
  }


