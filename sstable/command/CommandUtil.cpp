//
// Created by ChistaData on 06/08/22.
//

#include <CommandUtil.h>

std::shared_ptr<Command>  convertToCommand(nlohmann::json j){
    switch (j["commandType"].get<int>())
    {
        case SET:
            return std::make_shared<SetCommand>(SetCommand(j));
        default:
            return std::make_shared<RmCommand>(RmCommand(j));
    }
}
