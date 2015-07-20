//=======================================================================================
// Agog Labs C++ library.
// Copyright (c) 2013 Agog Labs Inc.,
// All rights reserved.
//
//  Bit flag set class.
// Author(s):    Conan Reis
// Notes:          Simple wrapper around an unsigned integer to represent bit-flags.
//=======================================================================================


#ifndef __AFLAGSET_HPP
#define __AFLAGSET_HPP


//=======================================================================================
// Includes
//=======================================================================================

#include <AgogCore/AgogCore.hpp>


//=======================================================================================
// Global Structures
//=======================================================================================

//---------------------------------------------------------------------------------------
// Bit flag set class.  Simple wrapper around an unsigned integer to represent bit-flags.
struct AFlagSet32
  {
  // Public Data Members

    uint32_t m_flagset;

  // Common Methods

    AFlagSet32()                          : m_flagset(0u) {}
    AFlagSet32(uint32_t flags)            : m_flagset(flags) {}
    AFlagSet32(const AFlagSet32 & flags)  : m_flagset(flags.m_flagset) {}

    uint32_t get_flags() const                { return m_flagset; }
    void assign(uint32_t flags)               { m_flagset = flags; }

    AFlagSet32 & operator=(const AFlagSet32 & flags)  { m_flagset = flags.m_flagset; return *this; }
    AFlagSet32 & operator=(uint32_t flags)            { m_flagset = flags; return *this; }

    bool operator==(const AFlagSet32 & flags)         { return m_flagset == flags.m_flagset; }
    bool operator!=(const AFlagSet32 & flags)         { return m_flagset != flags.m_flagset; }


  // Methods

    bool     is_set_all(uint32_t flags) const     { return (m_flagset & flags) == flags; }
    bool     is_set_any(uint32_t flags) const     { return (m_flagset & flags) != 0u; }
    bool     is_clear_all(uint32_t flags) const   { return (m_flagset & flags) == 0u; }
    bool     is_clear_any(uint32_t flags) const   { return (m_flagset & flags) != flags; }
    uint32_t get_masked(uint32_t mask) const      { return m_flagset & mask; }

    void set(uint32_t flags)                      { m_flagset |= flags; }
    void set_all()                                { m_flagset = UINT32_MAX; }
    void clear(uint32_t flags)                    { m_flagset &= ~flags; }
    void clear_all()                              { m_flagset = 0u; }
    void toggle(uint32_t flags)                   { m_flagset ^= flags; }
    void enable(uint32_t flags, bool set_flag = true)       { if (set_flag) { m_flagset |= flags; } else { m_flagset &= ~flags; } }
    void set_and_clear(uint32_t to_set, uint32_t to_clear)  { m_flagset |= to_set; m_flagset &= ~to_clear; }


  // Bit Index Methods

    bool is_set_bit(uint32_t bit) const           { return (m_flagset & (1u << bit)) != 0u; }
    bool is_clear_bit(uint32_t bit) const         { return (m_flagset & (1u << bit)) == 0u; }
    void clear_bit(uint32_t bit)                  { m_flagset &= ~(1u << bit); }
    void set_bit(uint32_t bit)                    { m_flagset |= (1u << bit); }
    void toggle_bit(uint32_t bit)                 { m_flagset ^= (1u << bit); }
    void enable_bit(uint32_t bit, bool set_flag = true)  { if (set_flag) { m_flagset |= (1u << bit); } else { m_flagset &= ~(1u << bit); } }
  };


//=======================================================================================
// Inline Functions
//=======================================================================================


#endif // __AFLAGSET_HPP


