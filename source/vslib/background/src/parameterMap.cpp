//! @file
//! @brief Source file containing library-side background task code for creating and uploading the parameter
//! map.
//! @author Dominik Arominski

#include "componentRegistry.h"
#include "parameterMap.h"
#include "sharedMemory.h"

namespace vslib
{
    //! Creates and uploads the parameter map to the shared memory. The memory is reinitialized each time
    //! this method is called.
    void ParameterMap::uploadParameterMap()
    {
        auto json_component_registry = fgc4::utils::StaticJsonFactory::getJsonObject();
        json_component_registry      = ComponentRegistry::instance().createParameterMap();
        writeJsonToMessageQueue(json_component_registry, m_write_parameter_map_queue);
    }
}   // namespace vslib
