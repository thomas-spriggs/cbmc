/*******************************************************************\

Module: Goto Trace Storage

Author: Daniel Kroening, Peter Schrammel

\*******************************************************************/

/// \file
/// Goto Trace Storage

#ifndef CPROVER_GOTO_CHECKER_GOTO_TRACE_STORAGE_H
#define CPROVER_GOTO_CHECKER_GOTO_TRACE_STORAGE_H

#include <goto-programs/goto_trace.h>

#include <util/merge_irep.h>

#include <list>

class goto_trace_storaget
{
public:
  explicit goto_trace_storaget(const namespacet &);
  /// @brief Copy constructor for the goto_trace_storaget class.
  /// \warning Be advised that goto_traces can be large objects, so
  ///          copying is not recommended, unless you know what you're doing.
  goto_trace_storaget(const goto_trace_storaget &) = default;
  /// @brief Move constructor for the goto_trace_storaget class.
  /// \details Exists so that moving of traces into API consumers who
  ///          then peruse the traces is facilitated.
  goto_trace_storaget(goto_trace_storaget &&o) = default;

  /// Store trace that ends in a violated assertion
  const goto_tracet &insert(goto_tracet &&);

  /// Store trace that contains multiple violated assertions
  /// \note Only property IDs that are not part of any already stored trace
  ///   are mapped to the given trace.
  const goto_tracet &insert_all(goto_tracet &&);

  const std::list<goto_tracet> &all() const;
  const goto_tracet &operator[](const irep_idt &property_id) const;

  const namespacet &get_namespace() const;

protected:
  /// the namespace related to the traces
  const namespacet &ns;

  /// stores the traces
  std::list<goto_tracet> traces;

  // maps property ID to index in traces
  std::unordered_map<irep_idt, std::size_t> property_id_to_trace_index;

  /// irep container for shared ireps
  merge_irept merge_ireps;
};

#endif // CPROVER_GOTO_CHECKER_GOTO_TRACE_STORAGE_H
