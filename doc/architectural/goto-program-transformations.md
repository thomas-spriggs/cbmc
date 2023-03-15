\ingroup module_hidden

\page goto-program-transformations Goto Program Transformations

TODO:
- Add link for each pass to the function that implements it.
- Add to each title the name of the function that implements it.
- Add link points and cross-linking
- Fix consistency of Removal/Remove etc.

## Core Transformation Passes

This section lists the transformation passes that must have been applied for the
goto model to be in core goto format.

Note that the passes are listed below in the order they are currently called in
CBMC. While all dependencies on the ordering are not fully documented, the
following order should be used.

### Removal/Lowering of Assembly

This pass goes through the goto model and removes or lowers instances of
assembly intructions. Assembly instructions are stored in instances of the
`other` instruction.

The implementation of this pass is called via the \ref remove_asm(goto_modelt &)
function. For full documentation of this pass see \ref remove_asm.h

_This pass has no predeccesor._

### Linking of Standard Libraries

This pass links the function definitions from the Cprover standard library
models to the current goto model.

The implementation of this pass is called via the \ref
link_to_library(goto_modelt &, message_handlert &, const std::function<void(const std::set<irep_idt> &, const symbol_tablet &, symbol_tablet &, message_handlert &)> &) "link_to_library function."

_Predecessor pass is [Removal/Lowering of Assembly]._

### Removal/Lowering of Function Pointers

This pass finds all the instructions which are function calls to the value of a
function pointer. Each instruction is then replaced with a switch block. The
switch block contains a case for each of the potential targets of the  function
pointer. The targets are found by looking for all functions in the goto program
that approximately match the type of the function pointer.

Note that for this pass to work as intended, all potential targets of calls to
function pointers must be in the model. Otherwise they will not be added to the
relevant switch blocks. This may cause the assertion that the switch blocks'
fallthrough is unreachable to be shown to be violated via an invalid counter
example.

The implementation of this pass is called via the \ref
remove_function_pointers(message_handlert &, goto_modelt &, bool)
"remove_function_pointers" function. Detailed documentation of this pass belongs
in \ref remove_function_pointers.h

_Predecessor passes are [String Instrumentation] /
[Removal/Lowering of Assembly](#Removal/Lowering-of-Assembly)._

### Memory Mapped IO Instrumentation

This pass finds pointer dereferences and adds corresponding calls to the
`__CPROVER_mm_io_r` and `__CPROVER_mm_io_w` modelling functions if they exist.
See [modeling-mmio.md](Device-behavior) for details of
modeling memory-mapped I/O regions of device interfaces. This pass is always
carried out but will only make changes if one of the modelling functions exist.

The implementation of this pass is called via the \ref mm_io(goto_modelt &)
"mm_io" function. Further documentation of this pass can be found in \ref
mm_io.h

_Predecessor pass is [Removal/Lowering of Function Pointers]._


### Instrument/Remove Preconditions

This pass moves preconditions associated with functions to call sites rather
than at the head of the function. Note that functions may have multiple call
sites and in this case the instrumentation is copied to these multiple call
sites.

The implementation of this is called via \ref
instrument_preconditions(goto_modelt &goto_model) . Documentation of this pass
belongs in \ref instrument_preconditions.h

_Predecessor pass is [Memory Mapped IO Instrumentation]._


### Removal/Lowering of Return Statements

This pass replaces returns of values with writes and reads to global variables.

The implementation of this is called via \ref remove_returns(goto_modelt &) .
Detailed documentation of this pass can be found in \ref remove_returns.h

_Predecessor passes are [Instrument/Remove Preconditions] or [Partial Inlining]._


### Remove/Lower Vector Typed Expressions

This pass removes/lowers expressions corresponding to CPU instruction sets such
as MMX, SSE and AVX. For more details on how this is done see vector_typet and
remove_vector.cpp. The implementation of this is called via \ref
remove_vector(goto_modelt &goto_model)

_Predecessor pass is [Removal/Lowering of Return Statements]._


### Remove/Lower Complex Typed Expressions

This pass is for the handling of complex numbers in the mathematical sense of a
number consisting of paired real and imaginary parts. These are removed/lowered
in this pass.

_Predecessor pass is [Remove/Lower Vector Typed Expressions]._


### Rewrite Unions

This pass converts union member access into byte extract and byte update
operations. 

**TODO: Pointer/type related operations require closer implementation
examination.**

_Predecessor pass is [Remove/Lower Complex Typed Expressions]._


### goto_check_c

This is a pass that can add many checks and instrumentation largely related to
the definition of the C language. Note that this transformation pass is
mandatory in the current implementation, however it may have no effect depending
on the command line options specified.

**TODO: add some more details/hints on what the options of note are**

_Predecessor pass is [Rewrite Unions]._


### Adjust Float Expressions

This is a transform from general mathematical operations over floating point
types into floating point specific operator variations which include a rounding
mode.

_Predecessor passes are [goto_check_c] or [Transform Assertions Assumptions]._


### Goto Functions Update

This transformation updates **(in memory?)** data goto program fields which may
have been invalidated by previous passes. Note that the following are performed
by this pass:
- Incoming edges
- Target numbers
- location numbers
- loop numbers

_Predecessor passes are [Adjust Float Expressions] or [String Abstraction]._


### Add Failed Symbols

This pass adds failed symbols to the symbol table only. See
`src/pointer-analysis/add_failed_symbols.h` for details.

_Predecessor passes are [Goto Functions Update] or
[Add Non-Deterministic Initialisation of Global Scoped Variables]._


### Remove Skip Instructions

This transformation removes skip instructions. Note that this transformation is
called in many places and may be called as part of other transformations. This
instance here is part of the mandatory transformation to reach core goto format.

_Predecessor passes are [Add Failed Symbols] or [Remove Unused Functions]._



### Label Properties

This transformation adds information to assert instructions. This includes
- Adding source locations to asserts where they are missing
- Populating the locations's property ID

_Predecessor passes are [Remove Skip Instructions] or [Add Coverage Goals]._



## Optional Transformation Passes

The sections lists the optional transformation passes that are optional and will
modify a goto model. Note that these are documented here for consistency, but
not required for core goto format.

Note for each optional pass there is a listed predeceesor pass. This is the pass
currently called before the listed pass in CBMC. While the ordering may not be
fully documented, this should be followed.

### String Instrumentation

This (optional) pass adds checks on calls to some of the C standard library
string functions. It uses the tracking information added by the
[string-abstraction](#String Abstraction) pass. It is switched on by the
`--string-abstraction` command line option.

The implementation of this pass is called via the
\ref string_instrumentation(goto_modelt &goto_model) function. Detailed
documentation of this pass belongs in \ref string_instrumentation.h

_The predecessor pass is
[Linking of Standard Libraries](#Linking-of-Standard-Libraries)._

### Partial Inlining

This pass does partial inlining when this option is switched on. Partal inlining
is inlining of functions either: marked as inline, or smaller than a specified
limit. For further detail see the implementation XXXXXXX

_Predecessor pass is [Instrument/Remove Preconditions]._


### Transform Assertions Assumptions
 
 This pass removes user provided assertions and assumptions when user has opted
 to do so. Note that this pass removes skip instructions if any other changes
 are made.

_Predecessor pass is [goto_check_c]._


### String Abstraction

This (optional) transformation pass adds tracking of length of C strings. It is
switched on by the `--string-abstraction` command line option.

_Predecessor pass is [Adjust Float Expressions]._


### Add Non-Deterministic Initialisation of Global Scoped Variables

This transformation adds non-deterministic initialisation of global scoped
variables including static variables. For details see
`src/goto-instrument/nondet_static.h`. The initialisation code is added to the
`CPROVER_initialize` function in the goto model.

_Predecessor pass is [Goto Functions Update]._


### Remove Unused Functions

This pass removes unused functions from the goto model. In practice this builds
a collection of all the functions that are potentially called, and then removes
any function not in this collection.

_Predecessor pass is [Add Failed Symbols]._


### Add Coverage Goals

This transformation adds coverage goals instrumentation and is only applied if
the `--cover` option has been specified.

_Predecessor pass is [Remove Skip Instructions]._


### Slicing

This includes various slicing passes as specified by various slicing command
line options. For full details see the various slicing options and source code
XXXXXXX.

_Predecessor pass is [Label Properties]._
