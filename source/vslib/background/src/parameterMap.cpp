//! @file
//! @brief Source file containing library-side background task code for creating and uploading the parameter
//! map.
//! @author Dominik Arominski

#include "parameterMap.h"
#include "versions.h"
#include "vslibMessageQueue.h"

namespace vslib
{
    //! Creates and uploads the parameter map to the shared memory. The memory is reinitialized each time
    //! this method is called.
    void ParameterMap::uploadParameterMap()
    {
        auto parameter_map = fgc4::utils::StaticJsonFactory::getJsonObject();
        parameter_map      = nlohmann::json::array();
        parameter_map.push_back(
            {{"version",
              {
                  version::json_parameter_map.major,
                  version::json_parameter_map.minor,
                  version::json_parameter_map.revision,
              }}}
        );
        parameter_map.push_back(m_root_component.serialize());
        utils::writeJsonToMessageQueue(parameter_map, m_write_parameter_map_queue);
    }
}   // namespace vslib
