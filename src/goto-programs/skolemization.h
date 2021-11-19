/// \file skolemization.h
/// Rename variables in existentially quantified statements.
/// Author: Diffblue Ltd.

#ifndef CPROVER_GOTO_PROGRAMS_SKOLEMIZATION_H
#define CPROVER_GOTO_PROGRAMS_SKOLEMIZATION_H

class exprt;
class goto_modelt;

/// Rename bound variables in existential quantifiers. This is the entry
/// point to the module.
/// \param goto_model The goto_model on which we operate. Needed
///    for making changes to the symbol table at variable renaming time.
void rename_variables_in_existential_quantifiers(goto_modelt &goto_model);

#endif
