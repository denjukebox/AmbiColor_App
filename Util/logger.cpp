#include "logger.h"

AC::Logger::Logger()
{

}

void AC::Logger::WriteActivity(const std::type_info& caller, string text){
    WriteLine(caller, text + " {Activity}");
}

void AC::Logger::WriteError(const std::type_info& caller, string text){
    WriteLine(caller, text + " {Error}");
}

void AC::Logger::WriteDebug(const std::type_info& caller, string text){
    WriteLine(caller, text + " {Debug}");
}

void AC::Logger::WriteLine(const std::type_info& caller, string text){
    auto name = caller.name();
    if(type_index(caller) == type_index(typeid(AC::DivisionThreader))){
        name = "DivisionThreader";
    }
    if(type_index(caller) == type_index(typeid(AC::FrameGrabber))){
        name = "FrameGrabber";
    }
    if(type_index(caller) == type_index(typeid(AC::TeensyLightConnector))){
        name = "TeensyLightConnector";
    }
    WriteLine(name, text);
}

void AC::Logger::WriteLine(string caller, string text){
    std::cout << '[' << caller << "] : " << text << std::endl;
}
