//! @file
//! @brief  Helper functions for the manager
//! @author Martin Cejp

#pragma once

#include <filesystem>

#include "bmboot.hpp"
#include "bmboot/domain.hpp"

namespace bmboot
{

    void                     displayOutputContinuously(IDomain& domain);
    void                     loadPayloadFromFileOrThrow(IDomain& domain, std::filesystem::path const& path);
    std::unique_ptr<IDomain> throwOnError(DomainInstanceOrErrorCode maybe_domain, const char* function_name);
    void                     throwOnError(MaybeError err, const char* function_name);

}
