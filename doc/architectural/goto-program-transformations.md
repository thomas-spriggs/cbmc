\ingroup module_hidden

\page goto-program-transformations Goto Program Transformations

\section required-transforms Core Transformation Passes

This section lists the transformation passes that must have been applied for the
goto model to be in core goto format.

Note that the passes are listed below in the order they are currently called in
CBMC. While all dependencies on the ordering are not fully documented, the
following order should be used.

\subsection assembly-transform Removal/Lowering of Assembly

This pass goes through the goto model and removes or lowers instances of
assembly intructions. Assembly instructions are stored in instances of the
`other` instruction.

The implementation of this pass is called via the \ref remove_asm(goto_modelt &)
function. For full documentation of this pass see \ref remove_asm.h

_This pass has no predeccesor._

\subsection linking-transform Linking of Standard Libraries

This pass links the function definitions from the Cprover standard library
models to the current goto model.

The implementation of this pass is called via the \ref
link_to_library(goto_modelt &, message_handlert &, const std::function<void(const std::set<irep_idt> &, const symbol_tablet &, symbol_tablet &, message_handlert &)> &) "link_to_library function."

_Predecessor pass is \ref assembly-transform ._

\subsection function-pointer-transform Removal/Lowering of Function Pointers

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

\subsection mmio-transform Memory Mapped IO Instrumentation

This pass finds pointer dereferences and adds corresponding calls to the
`__CPROVER_mm_io_r` and `__CPROVER_mm_io_w` modelling functions if they exist.
See [modeling-mmio.md](Device-behavior) for details of
modeling memory-mapped I/O regions of device interfaces. This pass is always
carried out but will only make changes if one of the modelling functions exist.

The implementation of this pass is called via the \ref mm_io(goto_modelt &)
"mm_io" function. Further documentation of this pass can be found in \ref
mm_io.h

_Predecessor pass is [Removal/Lowering of Function Pointers]._


\subsection precondition-transform Instrument/Remove Preconditions

This pass moves preconditions associated with functions to call sites rather
than at the head of the function. Note that functions may have multiple call
sites and in this case the instrumentation is copied to these multiple call
sites.

The implementation of this is called via \ref
instrument_preconditions(goto_modelt &goto_model) . Documentation of this pass
belongs in \ref instrument_preconditions.h

_Predecessor pass is [Memory Mapped IO Instrumentation]._


\subsection returns-transform Removal/Lowering of Return Statements

This pass replaces returns of values with writes and reads to global variables.

The implementation of this is called via \ref remove_returns(goto_modelt &) .
Detailed documentation of this pass can be found in \ref remove_returns.h

_Predecessor passes are [Instrument/Remove Preconditions] or [Partial Inlining]._


\subsection vector-transform Remove/Lower Vector Typed Expressions

This pass removes/lowers expressions corresponding to CPU instruction sets such
as MMX, SSE and AVX. For more details on how this is done see vector_typet and
remove_vector.cpp. The implementation of this is called via \ref
remove_vector(goto_modelt &goto_model)

_Predecessor pass is [Removal/Lowering of Return Statements]._


\subsection complex-transform Remove/Lower Complex Typed Expressions

This pass is for the handling of complex numbers in the mathematical sense of a
number consisting of paired real and imaginary parts. These are removed/lowered
in this pass. The implementation of this is called via \ref
remove_complex(goto_modelt &) . Documentation for this pass belongs in \ref
remove_complex.h

_Predecessor pass is [Remove/Lower Vector Typed Expressions]._


\subsection unions-transform Rewrite Unions

This pass converts union member access into byte extract and byte update
operations. 

The implementation of this pass is called via \ref rewrite_union(goto_modelt &)

_Predecessor pass is [Remove/Lower Complex Typed Expressions]._


\subsection check-c-transform goto_check_c

This is a pass that can add many checks and instrumentation largely related to
the definition of the C language. Note that this transformation pass is
mandatory in the current implementation, however it may have no effect depending
on the command line options specified.

The implementation of this pass is called via \ref
goto_check_c(const optionst &, goto_modelt &, message_handlert &)

_Predecessor pass is [Rewrite Unions]._


\subsection floats-transform Adjust Float Expressions

This is a transform from general mathematical operations over floating point
types into floating point specific operator variations which include a rounding
mode.

The implementation of this pass is called via \ref
adjust_float_expressions(goto_modelt &) . Documentation of this pass can be
found in \ref adjust_float_expressions.h

_Predecessor passes are [goto_check_c] or [Transform Assertions Assumptions]._


\subsection update-transform Goto Functions Update

This transformation updates in memory data goto program fields which may
have been invalidated by previous passes. Note that the following are performed
by this pass:
- Incoming edges
- Target numbers
- location numbers
- loop numbers

The implementation of this pass is called via \ref goto_functionst::update()

_Predecessor passes are [Adjust Float Expressions] or [String Abstraction]._


\subsection failed-symbols-transform Add Failed Symbols

This pass adds failed symbols to the symbol table. See
`src/pointer-analysis/add_failed_symbols.h` for details. The implementation of
this pass is called via \ref add_failed_symbols(symbol_table_baset &) . The
purpose of failed symbols is explained in the documentation of the function \ref
goto_symext::dereference(exprt &, statet &, bool)

_Predecessor passes are [Goto Functions Update] or
[Add Non-Deterministic Initialisation of Global Scoped Variables]._


\subsection remove-skip-transform Remove Skip Instructions

This transformation removes skip instructions. Note that this transformation is
called in many places and may be called as part of other transformations. This
instance here is part of the mandatory transformation to reach core goto format.
If there is a use case where it is desirable to preserve a "no operation"
instruction, a `LOCATION` type instruction may be used in place of a `SKIP`
instruction. The `LOCATION` instruction has the same semantics as the `SKIP`
instruction, but is not removed by the remove skip instructions pass.

The implementation of this pass is called via \ref remove_skip(goto_modelt &)

_Predecessor passes are [Add Failed Symbols] or [Remove Unused Functions]._



\subsection properties-transform Label Properties

This transformation adds information to assert instructions. This includes
- Adding source locations to asserts where they are missing
- Populating the locations's property ID

The implementation of this pass is called via \ref
label_properties(goto_modelt &)

_Predecessor passes are [Remove Skip Instructions] or [Add Coverage Goals]._



\section optional-transforms Optional Transformation Passes

The sections lists the optional transformation passes that are optional and will
modify a goto model. Note that these are documented here for consistency, but
not required for core goto format.

Note for each optional pass there is a listed predeceesor pass. This is the pass
currently called before the listed pass in CBMC. While the ordering may not be
fully documented, this should be followed.

\subsection string-instrument-transform String Instrumentation

This (optional) pass adds checks on calls to some of the C standard library
string functions. It uses the tracking information added by the
[string-abstraction](#String Abstraction) pass. It is switched on by the
`--string-abstraction` command line option.

The implementation of this pass is called via the
\ref string_instrumentation(goto_modelt &goto_model) function. Detailed
documentation of this pass belongs in \ref string_instrumentation.h

_The predecessor pass is
[Linking of Standard Libraries](#Linking-of-Standard-Libraries)._

\subsection inlining-transform Partial Inlining

This pass does partial inlining when this option is switched on. Partal inlining
is inlining of functions either: marked as inline, or smaller than a specified
limit. For further detail see the implementation function \ref
goto_partial_inline(goto_modelt &, message_handlert &, unsigned, bool)

_Predecessor pass is [Instrument/Remove Preconditions]._


\subsection assertions-transform Transform Assertions Assumptions
 
This pass removes user provided assertions and assumptions when user has opted
to do so. Note that this pass removes skip instructions if any other changes
are made. The implementation of this pass is called via the \ref
transform_assertions_assumptions(const optionst &, goto_modelt &) function.

_Predecessor pass is [goto_check_c]._


\subsection string-abstraction-transform String Abstraction

This (optional) transformation pass adds tracking of length of C strings. It is
switched on by the `--string-abstraction` command line option. This changes made
by this pass are documented as part of the documentation for the \ref
string_abstractiont class. The implementation of this pass is called via the
\ref string_abstraction(goto_modelt &, message_handlert &) function.

_Predecessor pass is [Adjust Float Expressions]._


\subsection nondet-transform Add Non-Deterministic Initialisation of Global Scoped Variables

This transformation adds non-deterministic initialisation of global scoped
variables including static variables. For details see
`src/goto-instrument/nondet_static.h`. The initialisation code is added to the
`CPROVER_initialize` function in the goto model. The implementation of this pass
is called via the \ref nondet_static(goto_modelt &) function.

_Predecessor pass is [Goto Functions Update]._


\subsection unused-functions-transform Remove Unused Functions

This pass removes unused functions from the goto model. In practice this builds
a collection of all the functions that are potentially called, and then removes
any function not in this collection.

The implementation of this pass is called via the \ref
remove_unused_functions(goto_modelt &, message_handlert &) function.

_Predecessor pass is [Add Failed Symbols]._


\subsection coverage-transform Add Coverage Goals

This transformation adds coverage goals instrumentation and is only applied if
the `--cover` option has been specified. The implementation of this pass is
called via the \ref instrument_cover_goals(const cover_configt &, goto_modelt &, message_handlert &)
function.

_Predecessor pass is [Remove Skip Instructions]._


\subsection slicing-transforms Slicing

This includes several slicing passes as specified by various slicing command
line options. The reachability slicer is enabled by the `--reachability-slice`
command line option. The implementation of this pass is called via the \ref
reachability_slicer(goto_modelt &, message_handlert &) function. The full slicer
is enabled by the `--full-slice` command line option. The implementation of this
pass is called via the \ref full_slicer(goto_modelt &) function.

_Predecessor pass is [Label Properties]._
