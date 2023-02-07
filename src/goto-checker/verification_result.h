#include "properties.h"

struct verification_resultt {
    propertiest properties;
    resultt verifier_result;

    verification_resultt(propertiest &_properties, resultt _verifier_result)
      : properties(_properties), verifier_result(_verifier_result)
    {} 
};
