//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2001 Agog Labs Inc.,
// All rights reserved.
//
// SkookumScript common declarations.  [Included in all SkookumScript files]
// Author(s):   Conan Reis
// Notes:          
//=======================================================================================


#ifndef __SKOOKUMSCRIPT_HPP
#define __SKOOKUMSCRIPT_HPP


//=======================================================================================
// Includes
//=======================================================================================

#include <AgogCore/AgogCore.hpp>
#include <AgogCore/AString.hpp>

//=======================================================================================
// Global Macros / Defines
//=======================================================================================

#ifndef SKOOKUM
  #error The 'SKOOKUM' define must be present and be set to an appropriate value
#endif

// These are the possible flags that may be set for the SKOOKUM define:

// Include extra SkookumScript debugging code.
#define SK_DEBUG     (1 << 0)  // 1

// Include ability to parse SkookumScript code string/text into internal data structures.
#define SK_CODE_IN   (1 << 1)  // 2

// Include ability to generate SkookumScript code string/text from internal data structures.
#define SK_CODE_OUT  (1 << 2)  // 4

// Include ability to read in SkookumScript compiled binary code and convert it to internal data structures.
#define SK_COMPILED_IN  (1 << 3)  // 8

// Include ability to generate SkookumScript compiled binary code from internal data structures.
#define SK_COMPILED_OUT (1 << 4)  // 16

// Here are the normal configuration values for the SKOOKUM define:
//   SkookumScript IDE      = 31  [SK_DEBUG | SK_CODE_IN | SK_CODE_OUT | SK_COMPILED_IN | SK_COMPILED_OUT]
//   Game (Debug & Dev)     = 15  [SK_DEBUG | SK_CODE_IN | SK_CODE_OUT | SK_COMPILED_IN]
//   Game (Release / Final) = 8   [SK_COMPILED_IN]


// If this is defined then check for conditions that would normally be caught and fixed at
// compile-time, but that may slip through to runtime.  For example a bad cast can fool
// the compiler into thinking that an object is of a type that it ends up not being at 
// runtime.  Assert and describe the error if (SKOOKUM & SK_DEBUG) is defined.  Then
// recover from the error state as best as possible.  These checks should be redundant
// if all the errors were caught when (SKOOKUM & SK_DEBUG) is defined, but in big complex
// games lots of things can't be tested rigorously enough to warrant removing the extra
// checks.
// $Revisit - CReis It might be a good idea to have this as another flag in the SKOOKUM
// define.
#define SK_RUNTIME_RECOVER

//---------------------------------------------------------------------------------------
// Debug hooks for notifying when scripts start/stop various tasks so that things like
// tracing, profiling, breakpoints, etc. can be added.
// See SkDebug.hpp - put this here due to problems with order of includes
#if (SKOOKUM & SK_DEBUG) && !defined(SKDEBUG_HOOKS_DISABLE)
  #define SKDEBUG_HOOKS
#endif


//---------------------------------------------------------------------------------------
// Debug stuff common to both regular debugging and execution hooks
#if (SKOOKUM & SK_DEBUG) || defined(SKDEBUG_HOOKS)
  #define SKDEBUG_COMMON
#endif


//---------------------------------------------------------------------------------------
// When present, methods used to visualize/convert data-structures to code scripts will be
// present in the compile.
#if (SKOOKUM & SK_DEBUG) || (SKOOKUM & SK_CODE_OUT) || defined(SKDEBUG_HOOKS)
  #define SK_AS_STRINGS
#endif


//---------------------------------------------------------------------------------------
// When present, data structures and routines to visualize/convert data-structures to
// code scripts or to read in code scripts will be present in the compile.
#if defined(SK_AS_STRINGS) || (SKOOKUM & SK_CODE_IN)
  #define SK_CODE
#endif


//---------------------------------------------------------------------------------------
#if (SKOOKUM & SK_DEBUG) && !defined(SK_IGNORE_DPRINT)
  #define SK_KEEP_DPRINT
#endif

//---------------------------------------------------------------------------------------
// String is empty for 64 bit and "32" for 32 bit systems
#ifdef A_BITS64
#define SK_BITS_ID ""
#else
#define SK_BITS_ID "32"
#endif

//---------------------------------------------------------------------------------------
// Initial/starting values for Skookum classes.
struct SkookumVals
  {
  // Methods

    SkookumVals();

  // Public Data Members

    // 'true' when initially constructed.  If an app needs to override the default values,
    // check for 'true', set any desired custom values, and then set to 'false'.
    bool m_using_defaults;

    // A string describing the engine SkookumScript is hooked up to, e.g. "Unreal Engine 4.7.6"
    AString m_engine_id_string;

    // A string describing the platform/device SkookumScript is running on, e.g. "Windows"
    AString m_platform_id_string;

    // If to use the built-in actor class. Otherwise you have to implement your own.
    bool m_use_builtin_actor;

    // If you do not use the built-in actor class, here is where you specify the name of yours.
    // Or set to ASymbol::ms_null if you do not want an actor class at all.
    AString m_custom_actor_class_name;

    // Object pool size for SkBoolean objects.  Initial size and amount to increment when
    // more objects are needed than are available.
    uint m_pool_init_boolean;
    uint m_pool_incr_boolean;

    // Object pool size for SkData objects.  Initial size and amount to increment when
    // more objects are needed than are available.
    uint m_pool_init_data;
    uint m_pool_incr_data;

    // Object pool size for SkDataInstance objects.  Initial size and amount to increment
    // when more objects are needed than are available.
    uint m_pool_init_data_instance;
    uint m_pool_incr_data_instance;

    // Object pool size for SkInstance objects.  Initial size and amount to increment
    // when more objects are needed than are available.
    uint m_pool_init_instance;
    uint m_pool_incr_instance;

    // Object pool size for SkInvokedExpression objects.  Initial size and amount to
    // increment when more objects are needed than are available.
    uint m_pool_init_iexpr;
    uint m_pool_incr_iexpr;

    // Object pool size for SkInvokedMethod objects.  Initial size and amount to
    // increment when more objects are needed than are available.
    uint m_pool_init_imethod;
    uint m_pool_incr_imethod;

    // Object pool size for SkInvokedCoroutine objects.  Initial size and amount to
    // increment when more objects are needed than are available.
    uint m_pool_init_icoroutine;
    uint m_pool_incr_icoroutine;

  };


namespace Skookum
  {

  // This must be defined somewhere by the application.
  SkookumVals & get_lib_vals();
  // Its body would look something like this:
  //  {
  //  static SkookumVals s_values;
  //  if (s_values.m_using_defaults)
  //    {
  //    // Set custom initial values
  //    s_values.m_using_defaults = false;
  //    }
  //  return s_values;
  //  }

  }

//=======================================================================================
// Global Types
//=======================================================================================

typedef int32_t   SkIntegerType;
typedef f32       SkRealType;
typedef char      SkCharType;
typedef bool      SkBooleanType;
typedef uint32_t  SkEnumType;

//=======================================================================================
// Global Structures
//=======================================================================================

// Pre-declarations
class ASymbol;
class SkClassDescBase;
class SkClass;
class SkMind;


// Constant indicating that the interval should be as often as possible - i.e. every call
// of SkMind::on_update().
const f32 SkCall_interval_always = 0.0f;

// Constant indicating that the interval should be as small as possible - i.e. never call
const f32 SkCall_interval_never = FLT_MAX;

//---------------------------------------------------------------------------------------
// Indicates whether an invocation/call should return immediately - like methods, after a
// duration (including immediately) - like coroutines or either.
enum eSkInvokeTime
  {
  SkInvokeTime_immediate  = 1 << 0,
  SkInvokeTime_durational = 1 << 1,
  SkInvokeTime_any        = SkInvokeTime_immediate | SkInvokeTime_durational
  };

//---------------------------------------------------------------------------------------
// Invocation type - it is stored in the compiled binary code so modify with care
enum eSkInvokeType
  {
  SkInvokeType__invalid  = 0,
  SkInvokeType_method    = 1,
  SkInvokeType_coroutine = 2
  };

//---------------------------------------------------------------------------------------
// This is used to differentiate between different types of invokables when it is only
// known that an instance is of type SkInvokableBase, but not the specific subclass.
// It is returned by the method SkInvokableBase::get_invoke_type();
enum eSkInvokable
  {
  SkInvokable_method         = 0,  // Custom method
  SkInvokable_method_func    = 1,  // Atomic method (C++ function)
  SkInvokable_method_mthd    = 2,  // Atomic method (C++ method)
  SkInvokable_coroutine      = 3,  // Custom coroutine
  SkInvokable_coroutine_func = 4,  // Atomic coroutine (C++ function)
  SkInvokable_coroutine_mthd = 5   // Atomic coroutine (C++ method)
  };

//---------------------------------------------------------------------------------------
enum eSkMember
  {
  SkMember_method         = SkInvokable_method,          // Custom method
  SkMember_method_func    = SkInvokable_method_func,     // Atomic method (C++ function)
  SkMember_method_mthd    = SkInvokable_method_mthd,     // Atomic method (C++ method)
  SkMember_coroutine      = SkInvokable_coroutine,       // Custom coroutine
  SkMember_coroutine_func = SkInvokable_coroutine_func,  // Atomic coroutine (C++ function)
  SkMember_coroutine_mthd = SkInvokable_coroutine_mthd,  // Atomic coroutine (C++ method)

  // Members <= this can show disassembly
  SkMember__disassembly = SkMember_coroutine_mthd,  

  SkMember_data,
  SkMember_class_meta,        // Class meta information
  SkMember_object_ids,        // Class object id validation list
  SkMember_object_ids_defer,  // Class object id validation list (deferred)

  SkMember__length,     // The position of these enumerations are important
  SkMember__invalid,    // Not a valid member and can be ignored
  SkMember__error       // Not a valid member, but considered an error rather than ignored
  };


//---------------------------------------------------------------------------------------
enum eSkScope
  {
  SkScope__none,     // Variable does not exist in current scope
  SkScope_class,     // Class members
  SkScope_instance,  // Instance members
  SkScope_local      // Temporary variables, arguments [& perhaps variables higher on call stack?]
  };


//---------------------------------------------------------------------------------------
// Notes      SkookumScript main class
// Author(s)  Conan Reis
class SkookumScript
  {
  friend class SkActor;
  friend class SkBrain;

  public:

  enum eFlag
    {
    Flag_evaluate    = 1 << 0,  // Scripts can be evaluated - scripts paused if not set
    Flag_need_update = 1 << 1,  // There are scripts that need updating (set by system)
    Flag_updating    = 1 << 2,  // In the middle of an update (set by system)
    Flag_trace       = 1 << 3,  // Trace scripts

    // Masks and combinations
    Flag__none        = 0,
    Flag__eval_update = Flag_evaluate | Flag_need_update  // Can evaluate and an update is needed
    };

  // Class Methods

    static void enable_flag(eFlag flag, bool enable_b = true);
    static bool is_flag_set(eFlag flag);
    static bool is_flags_set(uint32_t flag);

    static void register_bind_atomics_func(void (*bind_atomics_f)());
    static void initialize_pre_load();
    static void initialize_post_load();
    static void initialize_session(bool create_master_mind = true);

    static void pools_reserve();
    static void deinitialize_session();
    static void deinitialize();

    static SkMind * get_master_mind()                                          { return ms_master_mind_p; }


    // Update methods (in order of preference) - just use most convenient version once an update

      static void update(uint64_t sim_ticks, f64 sim_time, f32 sim_delta);
      static void update_ticks(uint64_t sim_ticks);
      static void update_delta(f32 sim_delta);

      static void register_update_request_func(void (*on_update_request_f)(bool update_req_b))   { ms_on_update_request_f = on_update_request_f; }
      static void update_request(bool update_req_b = true);

    // Time Methods

      static uint64_t get_sim_ticks()                                     { return ms_sim_ticks; }
      static f64      get_sim_time()                                      { return ms_sim_time; }
      static f32      get_sim_delta()                                     { return ms_sim_delta; }
      static void     register_update_time_func(void (*update_time_f)())  { ms_update_time_f = update_time_f; }
      static void     reset_time();
      static void     update_time();
      static void     update_time_ticks(uint64_t sim_ticks);

    // Memory Methods

      static void register_script_linear_bytes_func(void (*on_script_linear_bytes_f)(uint32_t bytes_needed))   { ms_on_script_linear_bytes_f = on_script_linear_bytes_f; }
      static void notify_script_linear_bytes(uint32_t bytes_needed)       { if (ms_on_script_linear_bytes_f) { ms_on_script_linear_bytes_f(bytes_needed); } }

  protected:

  // Class Data Members

    static SkClass * ms_startup_class_p;
    static SkMind *  ms_master_mind_p;   // created in `SkookumScript::initialize_session()`

    static uint32_t ms_flags;

    static void (* ms_on_update_request_f)(bool update_req_b);
    static void (* ms_update_time_f)();
    static void (* ms_on_script_linear_bytes_f)(uint32_t bytes_needed);


    // Simulation Time Data Members - The simulation time should only be updated when the
    // game is being simulated - i.e. it should not change when the game is paused or
    // potentially when in the game's front end.

      // Simulation time (in seconds) since game / level start.  It may have been scaled
      // to be faster or slower (even paused) compared to real time.  Most people find it
      // more intuitive and less error prone to think in terms of seconds rather than
      // milliseconds or hundredths of seconds.  A 64-bit float is used rather than a
      // 32-bit float for greater accuracy - it will only have millisecond rounding
      // errors after several tens of thousands of years, whereas a 32-bit float can
      // begin to have rounding errors after only about 3 hours have passed and after
      // about 9 hours (specifically 2^15 = 32768 seconds) the precision will have
      // deteriorated to the point that adding a millisecond would be ignored.
      static f64 ms_sim_time;

      // Simulation ticks (in milliseconds) since game / level start.  It may have been
      // scaled to be faster or slower (even paused) compared to real time.  If time is
      // being represented to users then ms_sim_time might be more appropriate since most
      // people find it more intuitive and less error prone to think in seconds rather
      // than milliseconds.  A 64-bit integer is used rather than a 32-bit integer for
      // greater accuracy - it will only overflow every 585 million years, whereas a
      // 32-bit integer will overflow every 49.71 days.
      static uint64_t ms_sim_ticks;

      // Amount of time that has passed (in seconds) since the last update / frame of the
      // simulation.  It may be scaled to be faster or slower (even paused) compared to
      // real time.  ms_sim_delta should *only* be used for calculations that take place
      // in a single update iteration - an event that occurs in the future should use
      // ms_sim_time or ms_sim_ticks to trigger it rather than accumulating or decrementing
      // ms_sim_delta since it will accumulate millisecond errors over time.
      static f32 ms_sim_delta;

  };  // SkookumScript


//=======================================================================================
// Inline Methods
//=======================================================================================

//---------------------------------------------------------------------------------------
// Determines if particular flag is enabled or not.
// Returns:    true if flag enabled, false if not
// See:        enable_flag()
// Modifiers:   static
// Author(s):   Conan Reis
inline bool SkookumScript::is_flag_set(eFlag flag)
  {
  return (ms_flags & flag) != 0u;
  }

//---------------------------------------------------------------------------------------
// Determines if particular flags are *all* enabled or not.
// Returns:    true if flags enabled, false if not
// See:        enable_flag()
// Modifiers:   static
// Author(s):   Conan Reis
inline bool SkookumScript::is_flags_set(uint32_t flags)
  {
  return (ms_flags & flags) == flags;
  }

//---------------------------------------------------------------------------------------
//  Reset the time variables
// Modifiers:    static
// Author(s):    Conan Reis
inline void SkookumScript::reset_time()
  {
  ms_sim_ticks = UINT64_C(0);
  ms_sim_time  = 0.0;
  ms_sim_delta = 0.0f;
  }


#endif  // __SKOOKUMSCRIPT_HPP
