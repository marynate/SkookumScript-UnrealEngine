//=======================================================================================
// Agog Labs C++ library.
// Copyright (c) 2001 Agog Labs Inc.,
// All rights reserved.
//
//  AStringRef class declaration header
// Author(s):    Conan Reis
// Create Date:   2001-09-19
// Notes:          
//=======================================================================================


#ifndef __ASTRINGREF_HPP
#define __ASTRINGREF_HPP
#pragma once
             

//=======================================================================================
// Includes
//=======================================================================================

#include <AgogCore/AgogCore.hpp>

//=======================================================================================
// Global Structures
//=======================================================================================

// Pre-declarations
template<class _ObjectType> class AObjReusePool;
template<class _ObjectType> class AObjBlock;

//---------------------------------------------------------------------------------------
// Author   Conan Reis
struct AStringRef
  {
  // Common methods

    // $Note - CReis Use pool_new() instead of constructor unless just used temporarily on the stack.

    AStringRef(const char * cstr_p, uint32_t length, uint32_t size, uint16_t ref_count, bool deallocate, bool read_only);

    AStringRef * reuse_or_new(const char * cstr_p, uint32_t length, uint32_t size, bool deallocate = true);

  // Comparison Methods

    eAEquate compare(const AStringRef & str_ref) const;
    bool     is_equal(const AStringRef & str_ref) const;
    bool     is_equal(const char * cstr_p) const;
    bool     is_equal(const char * cstr_p, uint32_t cstr_length) const;

  // Modifying Methods

    void dereference();


  // Class Methods

    static uint32_t     request_char_count(uint32_t needed_chars);
    static char *       alloc_buffer(uint32_t needed);
	static void         free_buffer(char * buffer);
    static void         pool_delete(AStringRef * str_ref_p);
    static AStringRef * pool_new(const char * cstr_p, uint32_t length, uint32_t size, uint16_t ref_count, bool deallocate, bool read_only);
    static AStringRef * pool_new_copy(const char * cstr_p, uint32_t length, uint16_t ref_count = 1u, bool read_only = false);
    static AStringRef * get_empty();

    static AObjReusePool<AStringRef> & get_pool();


  // Data Members

    char *   m_cstr_p;      // Pointer to C-String buffer
    uint32_t m_length;      // Current length (number of characters)
    uint32_t m_size;        // Allocated size of m_cstr_p
    uint16_t m_ref_count;   // Number of references to this AStringRef
    bool     m_deallocate;  // Specifies whether m_cstr_p should be deallocated or not
    bool     m_read_only;   // Indicates whether m_cstr_p is read-only

    // $Revisit - CReis [Efficiency] Note that 'm_deallocate' and 'm_read_only' could be
    // combined into one enumerated type (using just a uint8_t or uint16_t) with three possible
    // states: writable_deallocate, writable, and read_only.
    // This may be faster since one less value assignment would be needed for the
    // creation of AStringRef objects.  Note that this should *not* save any space since
    // two bools should use the same space as one uint16_t - i.e. 2 bytes.

  AStringRef() {}  // Intentionally uninitialized - use AStringRef::pool_new()

  protected:  // Internal Stuff

  // Internal Class Methods

    // Hide except for internal use
    friend class AObjBlock<AStringRef>;

  };  // AStringRef


//=======================================================================================
// Inline Methods
//=======================================================================================

#ifndef A_INL_IN_CPP
  #include <AgogCore/AStringRef.inl>
#endif

 
#endif  // __ASTRINGREF_HPP



