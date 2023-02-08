// Copyright (c) 2023 Fotis Koutoulakis for Diffblue Ltd.

/// \file Provide a convenience function whose aim is to initialise the verification
///       engine and its dependencies, and run it against a passed in goto-model,
///       in the end returning a verification result type so that we can make assertions
///       against its structure.

#include <sstream>

#include <goto-checker/multi_path_symex_checker.h>
#include <goto-checker/all_properties_verifier_with_trace_storage.h>
#include <goto-checker/properties.h>
#include <goto-checker/verification_result.h>

#include "message.h"

#include "run_verification_engine.h"

verification_resultt run_verification_engine(goto_modelt &model) {
    // Initialise dependencies of verification engine
    std::ostringstream out;
    test_ui_message_handlert_plain ui_message_handler(out);
    
    all_properties_verifier_with_trace_storaget<multi_path_symex_checkert> verifier(optionst{}, ui_message_handler, model);
    
    return verifier.produce_results();
}

