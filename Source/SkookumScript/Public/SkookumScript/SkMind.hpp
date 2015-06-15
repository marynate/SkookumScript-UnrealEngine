//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2015 Agog Labs Inc.,
// All rights reserved.
//
// Mind object - tracks and updates coroutines.
// 
// Author(s): Conan Reis
//=======================================================================================


#ifndef __SKMIND_HPP
#define __SKMIND_HPP


//=======================================================================================
// Includes
//=======================================================================================

#include <AgogCore/APArray.hpp>
#include <AgogCore/AList.hpp>
#include <SkookumScript/SkDataInstance.hpp>
#include <SkookumScript/SkClassBindingAbstract.hpp>


//=======================================================================================
// Global Structures
//=======================================================================================

// Pre-declarations
class SkMind; 


//---------------------------------------------------------------------------------------
// This is passed as a third argument to various template classes such as Array and
// APSorted to provide a mechanism for logical sorting of SkInvokedCoroutine elements which
// may be nullptr.
//class CompareIRespNULL
//  {
//  public:
//    // Returns true if elements are equal - allows for null elements
//    static bool  equals(const ASymbol & lhs, const SkInvokedCoroutine & rhs) {return &rhs ? (lhs == rhs) : false;}
//  };


// Some shorthand for sorted arrays
typedef APArray<SkInvokedCoroutine>       tSkICoroutines;  // SkMind::on_update() assumes non-sorted / indexes persistent
//typedef APArray<SkInvokedCoroutine, ASymbol, CompareIRespNULL> tSkICoroutines;  // SkMind::on_update() assumes non-sorted / indexes persistent


// Project/user specific flags can be added from this shift position and higher - see SkMind::eFlag
const int SkMind_flag_user_shift = 16;


//---------------------------------------------------------------------------------------
// Instance of Mind - used to track and update coroutines.
// Acts as home for concurrently updating behaviours.
// 
// Most if not all Mind objects will be singletons and so the name of the class is used
// for the string representation of a mind object.
class SkMind : public SkClassBindingAbstract<SkMind>, public SkDataInstance, public AListNode<SkMind>
  {
  friend class SkInstance;
  friend class SkInvokedCoroutine;

  public:

  // Nested Structures

    // These flags are stored in m_mind_flags.
    enum eFlag
      {
      Flag_behaving         = 1<<0,   // Indicates whether the mind is behaving or not (toggling clears & restarts behaviour)
      Flag_on_update_list   = 1<<1,   // Indicates whether the mind is in the update list or not - i.e. it has active coroutines and it's on_update() method being called.
      Flag_updating         = 1<<2,   // If set, the mind is updated as needed (its durational invocations are called)
      Flag_during_update    = 1<<3,   // Set when coroutines updated in the middle of an on_update() call

      // Debugging flags
      Flag_trace            = 1<<8    // If set, the mind's script is traced

      // User/Project Specific flags - make a new enum with enumerations similar to:
      //Flag_user_this  = 1 << (SkMind_flag_user_shift + 0),
      //Flag_user_that  = 1 << (SkMind_flag_user_shift + 1)
      };

  // Common Methods

    SK_NEW_OPERATORS(SkMind);

    SkMind(SkClass * class_p = nullptr);
    virtual ~SkMind();

  // Accessor Methods

    bool is_active() const                              { return (m_icoroutines_to_update.is_filled() || m_icoroutines_pending.is_filled()); }
    bool is_behaving() const                            { return (m_mind_flags & Flag_behaving) != 0u; }
    bool is_during_update() const                       { return (m_mind_flags & Flag_during_update) != 0u; }
    bool is_on_update_list() const                      { return (m_mind_flags & Flag_on_update_list) != 0u; }
    bool is_update_needed() const                       { return m_icoroutines_to_update.is_filled(); }
    bool is_updating() const                            { return (m_mind_flags & Flag_updating) != 0u; }

    void clear_mind_flags(uint32_t flags)               { m_mind_flags &= ~flags; }
    void set_mind_flags(uint32_t flags)                 { m_mind_flags |= flags; }
    bool is_mind_flags(uint32_t flags) const            { return (m_mind_flags & flags) == flags; }

    #if (SKOOKUM & SK_DEBUG)
      bool is_tracing() const                           { return SkookumScript::is_flag_set(SkookumScript::Flag_trace) || (m_mind_flags & Flag_trace); }
    #endif

  // Methods

    const ASymbol & get_name() const;
    AString         as_string() const;
    void            enable_behavior(bool activate = true);
    void            enable_on_update(bool activate = true);
    void            enable_updating(bool activate = true)  { if (activate) { m_mind_flags |= Flag_updating; } else { m_mind_flags &= ~Flag_updating; } }

    // Coroutine Methods

      tSkICoroutines get_invoked_coroutines()            { return m_icoroutines_to_update; }

      void clear_coroutines();
      void suspend_coroutines();
      void resume_coroutines();

    // Overriding from SkInstance -> SkDataInstance

      virtual void delete_this() override;

    // Overriding from SkInstance

       #if defined(SK_AS_STRINGS)
         virtual AString         as_string_debug() const override  { return as_string(); }
         virtual const ASymbol & get_name_debug() const override   { return get_name(); }
       #endif

    // Overriding from SkObjectBase

      virtual uint32_t get_obj_type() const override     { return SkObjectType_mind; } 
      virtual bool     is_mind() const override          { return true; }

 // Class Methods

    static SkMind *              find(const ASymbol & actor_name);
    static const AList<SkMind> & get_updating_minds()    { return ms_minds_to_update; }
    static void                  update_all();


 // SkookumScript Bindings

      static void register_bindings();

    // Script Methods

      static void mthd_ctor(SkInvokedMethod * scope_p, SkInstance ** result_pp);
      static void mthd_op_equals(SkInvokedMethod * scope_p, SkInstance ** result_pp);
      static void mthd_op_not_equal(SkInvokedMethod * scope_p, SkInstance ** result_pp);
      static void mthd_name(SkInvokedMethod * scope_p, SkInstance ** result_pp);
      static void mthd_string(SkInvokedMethod * scope_p, SkInstance ** result_pp);

    // Script Class Methods

      static void mthdc_instance(SkInvokedMethod * scope_p, SkInstance ** result_pp);
      static void mthdc_instance_or_nil(SkInvokedMethod * scope_p, SkInstance ** result_pp);
      static void mthdc_instanceQ(SkInvokedMethod * scope_p, SkInstance ** result_pp);
      static void mthdc_instances_length(SkInvokedMethod * scope_p, SkInstance ** result_pp);


  protected:

  // Internal Methods

    void coroutine_track(SkInvokedCoroutine * icoro_p);
    void coroutine_track_stop(SkInvokedCoroutine * icoro_p);
    void coroutine_track_updating(SkInvokedCoroutine * icoro_p);
    void coroutine_track_updating_stop(SkInvokedCoroutine * icoro_p);
    void coroutine_track_pending(SkInvokedCoroutine * icoro_p);
    void coroutine_track_pending_stop(SkInvokedCoroutine * icoro_p);

    //---------------------------------------------------------------------------------------
    // Finds first mind object of the specified class (same class or derived subclass).
    static SkMind * find_by_class(const SkClass & mind_class);

  // Event Methods

    virtual void on_update();


  // Data Members

    uint32_t m_mind_flags;

    // List of invoked coroutines updated/managed by this mind.  Invoked co-routines that
    // are suspended/waiting on operations are placed on the m_icoroutines_pending list.
    // ***Note that these invoked coroutines may have a different receiver than this mind
    // - sub-coroutines that are started by other coroutines in this update list use this
    // mind as their updater by default unless they are called within a "divert" block.
    // This allows contextually similar coroutines to be grouped together under a single
    // updater mind for "group" management.
    // $Revisit - CReis This might be better as a intrusive list
    tSkICoroutines m_icoroutines_to_update;

    // List of invoked coroutines updated by this mind though currently suspended/waiting
    // until pending [sub] operations are completed.  Once a coroutine resumes it will
    // either complete, spawn new sub-operations and be put back on this list or it will
    // be transferred to the m_invoked_coroutine list.
    // $Revisit - CReis This might be better as a intrusive list
    tSkICoroutines m_icoroutines_pending;


  // Class Data Members

    // List of minds that are currently "active" (have tracked invoked coroutines) and
    // need to have on_update() called during update_all().
    // Every mind object with either be in `ms_minds_to_update` (and if mid-update
    // `ms_minds_updating`) or `ms_minds_no_update`.
    static AList<SkMind> ms_minds_to_update;

    // List of minds that are either waiting for tracked suspended invoked coroutines to
    // be resumed or have no invoked coroutines.
    // Every mind object with either be in `ms_minds_to_update` (and if mid-update
    // `ms_minds_updating`) or `ms_minds_no_update`.
    static AList<SkMind> ms_minds_no_update;

    // Used during SkMind::update_all()

      // List of minds that are being updated in update_all() *at this moment*.
      // Every mind object with either be in `ms_minds_to_update` (and if mid-update
      // `ms_minds_updating`) or `ms_minds_no_update`.
      static AList<SkMind> ms_minds_updating;

      // List of coroutines that are currently being updated by a particular mind in
      // method SkMind::on_update() which is called by SkMind::update_all().
      // $Revisit - CReis This might be better as a intrusive list.  Also might be better
      // as a list of newly scheduled coroutines that occurred during an update rather
      // than a snapshot of the coroutines to update.
      static tSkICoroutines ms_icoroutines_updating;

  };  // SkMind


//=======================================================================================
// Inline Methods
//=======================================================================================

#ifndef A_INL_IN_CPP
  #include <SkookumScript/SkMind.inl>
#endif


#endif  // __SKMIND_HPP
