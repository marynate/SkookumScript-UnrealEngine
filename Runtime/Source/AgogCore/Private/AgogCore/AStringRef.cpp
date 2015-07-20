//=======================================================================================
// Agog Labs C++ library.
// Copyright (c) 2001 Agog Labs Inc.,
// All rights reserved.
//
//  AStringRef class definition module
// Author(s):    Conan Reis
// Create Date:   2001-09-19
// Notes:          
//=======================================================================================

//=======================================================================================
// Includes
//=======================================================================================

#include <AgogCore/AStringRef.hpp>
#ifdef A_INL_IN_CPP
  #include <AgogCore/AStringRef.inl>
#endif
#include <AgogCore/AObjReusePool.hpp>
#include <AgogCore/AConstructDestruct.hpp>


//=======================================================================================
// Method Definitions
//=======================================================================================

//---------------------------------------------------------------------------------------
// Reuses this string reference if it can - otherwise retrieves a string reference object
// from the dynamic pool and initializes it for use.
// This should be used instead of 'new' because it prevents unnecessary allocations by
// reusing previously allocated objects.
// 
// Notes:  
//   To 'deallocate' an object that was retrieved with this method, use `dereference()`
//   or `pool_delete()` rather than `delete`.
//   
// Returns:     a dynamic AStringRef
// See:         pool_new_copy(), pool_delete(), reuse_or_new()
// Modifiers:   static
// Author(s):   Conan Reis
AStringRef * AStringRef::reuse_or_new(
  const char * cstr_p,
  uint32_t     length,
  uint32_t     size,
  bool         deallocate // = true
  )
  {
  // If unique
  if (m_ref_count == 1u)
    {
    if (m_deallocate)
      {
      AStringRef::free_buffer(m_cstr_p);
      }

    m_cstr_p     = const_cast<char *>(cstr_p);
    m_size       = size;
    m_length     = length;
    m_deallocate = deallocate;
    m_read_only  = false;

    return this;
    }

  // Shared
  
  // Decrement prior to getting a new AStringRef
  dereference();
  return AStringRef::pool_new(cstr_p, length, size, 1u, deallocate, false);
  }

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Class Methods
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//---------------------------------------------------------------------------------------
// Retrieves a string reference object from the dynamic pool and initializes it for use.
// This should be used instead of 'new' because it prevents unnecessary allocations by
// reusing previously allocated objects.
// 
// Notes:  
//   To 'deallocate' an object that was retrieved with this method, use `pool_delete()`
//   rather than `delete`.
//   
// Returns:     a dynamic AStringRef
// See:         pool_new_copy(), pool_delete(), reuse_or_new()
// Modifiers:   static
// Author(s):   Conan Reis
AStringRef * AStringRef::pool_new(
  const char * cstr_p,
  uint32_t     length,
  uint32_t     size,
  uint16_t     ref_count,
  bool         deallocate,
  bool         read_only
  )
  {
  AStringRef * str_ref_p = get_pool().pop();

  str_ref_p->m_cstr_p     = const_cast<char *>(cstr_p);
  str_ref_p->m_length     = length;
  str_ref_p->m_size       = size;
  str_ref_p->m_ref_count  = ref_count;
  str_ref_p->m_deallocate = deallocate;
  str_ref_p->m_read_only  = read_only;

  return str_ref_p;
  }

//---------------------------------------------------------------------------------------
//  Retrieves a string reference object from the dynamic pool and initializes
//              it for use with a copy of cstr_p.  This should be used instead of 'new'
//              because it prevents unnecessary allocations by reusing previously
//              allocated objects.
// Returns:     a dynamic AStringRef
// See:         pool_new(), pool_delete()
// Notes:       To 'deallocate' an object that was retrieved with this method, use
//              'pool_delete()' rather than 'delete'.
// Modifiers:    static
// Author(s):    Conan Reis
AStringRef * AStringRef::pool_new_copy(
  const char * cstr_p,
  uint32_t     length,
  uint16_t     ref_count, // = 1u
  bool         read_only  // = false
  )
  {
  AStringRef * str_ref_p   = get_pool().pop();
  uint32_t     size        = request_char_count(length);
  char *       copy_cstr_p = alloc_buffer(size);

  memcpy(copy_cstr_p, cstr_p, length);
  copy_cstr_p[length] = '\0';  // Put in null-terminator

  str_ref_p->m_cstr_p     = copy_cstr_p;
  str_ref_p->m_length     = length;
  str_ref_p->m_size       = size;
  str_ref_p->m_ref_count  = ref_count;
  str_ref_p->m_deallocate = true;
  str_ref_p->m_read_only  = read_only;

  return str_ref_p;
  }

//---------------------------------------------------------------------------------------
// Returns common empty AStringRef Reference used by the empty AString and null symbol.
// 0-length empty AString, i.e. ""
// 
// #Notes
//   Uses Scott Meyers' tip "Make sure that objects are initialized before they're used"
//   from "Effective C++" [Item 47 in 1st & 2nd Editions and Item 4 in 3rd Edition]
//   This is instead of using a non-local static object for a singleton.
//   
// #Modifiers  static
// #Author(s)  Conan Reis
AStringRef * AStringRef::get_empty()
  {
  // Note that the initial reference count is 2 to ensure that it not be added to the
  // pool once it is dereferenced since it is not dynamically allocated.
  static AStringRef s_empty("", 0u, 1u, 2u, false, true);
  //A_DSINGLETON_GUARD;

  return &s_empty;
  }

//---------------------------------------------------------------------------------------
// Returns dynamic reference pool. Pool created first call and reused on successive calls.
// 
// #Notes
//   Uses Scott Meyers' tip "Make sure that objects are initialized before they're used"
//   from "Effective C++" [Item 47 in 1st & 2nd Editions and Item 4 in 3rd Edition]
//   This is instead of using a non-local static object for a singleton.
//   
// #Modifiers  static
// #Author(s)  Conan Reis
AObjReusePool<AStringRef> & AStringRef::get_pool()
  {
  static AObjReusePool<AStringRef> s_pool(Agog::get_agog_core_vals().m_pool_init_string_ref, Agog::get_agog_core_vals().m_pool_incr_string_ref);
  //A_DSINGLETON_GUARD;
  return s_pool;
  }


