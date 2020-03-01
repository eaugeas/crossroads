// Copyright (c) 2019, tlblanc <tlblanc1490 at gmail dot com>

#include "status.hpp"

Status ConfigLineNoValue = new StatusClass (1, "[ConfigLineNoValue]: config line does not have value property");
Status ConfigNotFoundKey = new StatusClass (1, "[ConfigNotFoundKey]: key in configuration file is not defined");
Status ConfigParseValue = new StatusClass (1, "[ConfigNotFoundKey]: failed to parse value for key");
