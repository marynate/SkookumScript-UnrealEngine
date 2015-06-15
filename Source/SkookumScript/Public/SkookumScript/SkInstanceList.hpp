//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2001-2015 Agog Labs Inc. All rights reserved.
//
// List of SkInstances
//
// Author: Conan Reis
//=======================================================================================

#pragma once

//=======================================================================================
// Includes
//=======================================================================================

#include <AgogCore/APArray.hpp>
#include <SkookumScript/SkInstance.hpp>

//---------------------------------------------------------------------------------------
// List of SkInstances
class SkInstanceList
  {
  public:
	  SK_NEW_OPERATORS(SkInstanceList);

  // Enumerated constants
    enum
      {
      Item_str_length_def = 8
      };

  // Methods

    explicit              SkInstanceList(uint32_t ensure_size = 0u) : m_items(nullptr, 0u, ensure_size) {}
                          SkInstanceList(const SkInstanceList & other);
                          SkInstanceList(const SkInstanceList && other);
                          ~SkInstanceList();

    SkInstanceList &      operator = (const SkInstanceList & other);

    SkInstance **         get_array() const       { return m_items.get_array(); }
    uint32_t              get_length() const      { return m_items.get_length(); }
    APArray<SkInstance> & get_instances() const   { return const_cast<SkInstanceList *>(this)->m_items; }

    void                  append(SkInstance & instance, bool reference_b = true);
    void                  append_all(const SkInstanceList & other, uint32_t start_pos = 0, uint32_t count = ALength_remainder);
    void                  append_all(uint32_t elem_count, SkInstance & instance);
    void                  at_set(uint32_t index, SkInstance & instance);
    void                  crop(uint32_t pos, uint32_t count);
    void                  empty();
    void                  insert(SkInstance & instance, uint32_t pos, uint32_t count = 1);
    void                  insert_all(const SkInstanceList & list, uint32_t pos);
    bool                  remove(SkInstance & instance, uint32_t match = AMatch_first_found, uint32_t * find_pos_p = nullptr, uint32_t start_pos = 0u, uint32_t end_pos = ALength_remainder);
    void                  remove_all(uint32_t pos, uint32_t count = ALength_remainder);
    void                  set_all(SkInstance & instance, uint32_t pos = 0, uint32_t count = ALength_remainder);

    void                  append_elems_as_strings(AString * str_p, SkInvokedBase * caller_p = nullptr) const;

  protected:

  // Data Members

    // Ordered array of items/elements.
    APArray<SkInstance> m_items;

  };  // SkInstanceList

