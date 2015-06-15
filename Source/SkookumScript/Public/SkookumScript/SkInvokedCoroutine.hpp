//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2001 Agog Labs Inc.,
// All rights reserved.
//
// Class wrapper for executed/called/invoked coroutines declaration file
// Author(s):   Conan Reis
// Notes:          
//=======================================================================================

#ifndef __SKINVOKEDCOROUTINE_HPP
#define __SKINVOKEDCOROUTINE_HPP


//=======================================================================================
// Includes
//=======================================================================================

#include <SkookumScript/SkInvokedBase.hpp>


//=======================================================================================
// Global Macros / Defines
//=======================================================================================

// Default value indicating that invoked coroutine is not in its actors update list.
const uint32_t SkInvokedCoroutine_not_listed = ADef_uint32;

//=======================================================================================
// Global Structures
//=======================================================================================

// Pre-declarations
class SkCoroutineBase;
class SkCoroutineCall;


//---------------------------------------------------------------------------------------
// Notes      Wrapper for executed/called/invoked coroutines
// Author(s)  Conan Reis
class SkInvokedCoroutine : public SkInvokedContextBase
  {
  // For fast access.
  friend class SkInstance;
  friend class SkMind;
  friend class SkCoroutineCall;

  public:

  // Nested Structures

    enum eFlag
      {
      Flag_none             = 0,
      Flag_tracked_updating = 1 << 0,
      Flag_tracked_pending  = 1 << 1,
      Flag_tracked_mask     = Flag_tracked_updating | Flag_tracked_pending,

      Flag_suspended        = 1 << 2,

      Flag__default = Flag_none
      };

  // Public Data Members

    // Number of times this coroutine has been updated.  This will always be 0u on the
    // first update.  m_update_count * m_update_interval will give an approximate
    // elapsed time of how long this coroutine has been invoked.
    uint32_t m_update_count;

    // Specifies how many seconds should elapse between each update of this coroutine.
    // Since the update interval is stored in this invoked coroutine rather than the
    // coroutine itself, the same coroutine can have different update rates and a specific
    // invoked coroutine can change its update interval from one update to the next.
    f32 m_update_interval;

    // Simulation time (in seconds) when this coroutine should next be updated.  The next
    // update time is incremented prior to the update, so it may be modified freely
    // within the update without concern that it will be additionally modified.
    // The simulation time should only be updated when the game is being simulated - i.e.
    // it should not change when the game is paused or when in the game's front end.
    // Also note that since the simulation time is being represented by a f32, there
    // will be rounding errors after the simulation has been running straight for more
    // than 3 days.  This could instead be represented by an integer using milliseconds,
    // but most people find it more intuitive and less error prone to think in terms of
    // seconds rather than milliseconds or hundredths of seconds.
    f64 m_update_next;

  // Common Methods

    SK_NEW_OPERATORS(SkInvokedCoroutine);

    SkInvokedCoroutine();
    SkInvokedCoroutine(const SkCoroutineBase & coroutine, f32 update_interval = SkCall_interval_always, SkInvokedBase * caller_p = nullptr, SkObjectBase * scope_p = nullptr);

  // Comparison Methods

    bool operator==(const SkInvokedCoroutine & icoroutine) const;
    bool operator<(const SkInvokedCoroutine & icoroutine) const;

  // Converter Methods

    operator const ASymbol & () const;

  // Accessor Methods
    
    const ASymbol &         get_name() const;
    const SkCoroutineBase & get_coroutine() const;
    virtual SkMind *        get_updater() const;
    void                    reset(f32 update_interval, SkInvokedBase * caller_p, SkInstance * scope_p, SkMind * updater_p = nullptr, const APCompactArray<SkIdentifier> * rargs_p = nullptr);
    void                    set_coroutine(const SkCoroutineBase & coroutine);

    // Overriding from SkInvokedBase

    virtual SkInstance *      new_instance() const;
    virtual void              abort_invoke(eSkNotify notify_caller = SkNotify_fail, eSkNotifyChild notify_child = SkNotifyChild_abort);
    virtual void              pending_return(bool completed = true);
    virtual void              pending_schedule(bool completed = true);
    virtual eSkMember         get_invoke_type() const;
    virtual SkInvokableBase * get_invokable() const;

    #if (SKOOKUM & SK_DEBUG)
      virtual bool is_in_use() const                        { return is_valid_id() && ((m_flags & Flag_tracked_mask) != 0u); }
    #endif

    #if defined(SK_AS_STRINGS)
      virtual AString as_string_debug() const;
    #endif

  // Methods
    
    bool is_suspended() const								{ return (m_flags & Flag_suspended) != 0u; }
    void suspend();
    void resume();
    bool on_update();

  // Class Methods

    static SkInstance *         new_instance(SkInvokedCoroutine * icoroutine_p);
    static void                 pool_delete(SkInvokedCoroutine * icoroutine_p);
    static SkInvokedCoroutine * pool_new(SkCoroutineBase * coroutine_p);
    static SkInvokedCoroutine * pool_new(const SkCoroutineCall & cocall, f32 update_interval, SkInvokedBase * caller_p, SkInstance * scope_p, SkMind * updater_p = nullptr);
    static AObjReusePool<SkInvokedCoroutine> & get_pool();

  // SkookumScript Atomic Methods

    static void register_bindings();

    static void mthd_String(SkInvokedMethod * scope_p, SkInstance ** result_pp);

  protected:

  // Internal Methods

    void stop_tracking();


  // Data Members

    // The coroutine that is invoked/running
    const SkCoroutineBase * m_coroutine_p;

    // Variable identifiers to bind any return argument results to or nullptr if no return
    // arguments used.
    const APCompactArray<SkIdentifier> * m_return_args_p;

    // The mind that is updating/tracking this invoked coroutine.  It may be the same as
    // m_scope_p or it may not.
    // $Revisit - CReis This *could* be a simple hard pointer SkMind *, but being paranoid
    // at the moment.
    mutable AIdPtr<SkMind> m_mind_p;

    // See SkInvokedCoroutine::eFlag
    uint32_t m_flags;

    // Future: priority - to resolve conflicting coroutines

  };  // SkInvokedCoroutine


//=======================================================================================
// Inline Methods
//=======================================================================================

//---------------------------------------------------------------------------------------
// Storage specialization - SkInvokedCoroutine stored indirectly as smart pointer in
// SkUserData rather than whole structure
template<> inline SkInvokedCoroutine *  SkUserDataBase::as<SkInvokedCoroutine>() const           { return static_cast<SkInvokedCoroutine *>(as_stored<AIdPtr<SkInvokedBase>>()->get_obj()); }
template<> inline void                  SkUserDataBase::set(SkInvokedCoroutine * const & value)  { *as_stored<AIdPtr<SkInvokedBase>>() = value; }

#ifndef A_INL_IN_CPP
  #include <SkookumScript/SkInvokedCoroutine.inl>
#endif


#endif  // __SKINVOKEDCOROUTINE_HPP
