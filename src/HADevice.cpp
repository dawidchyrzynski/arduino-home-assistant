#include "HADevice.h"

HADevice::HADevice(const char* uniqueId) :
    _uniqueId(uniqueId),
    _manufacturer(nullptr),
    _model(nullptr),
    _name(nullptr),
    _softwareVersion(nullptr),
    _viaDevice(nullptr)
{

}

String HADevice::serialize() const
{
    String output;
    output.concat("{");
    output.concat("{\"identifiers\":\"");
    output.concat(_uniqueId);
    output.concat("\"}");

    if (_manufacturer != nullptr) {
        output.concat(",{\"manufacturer\":\"");
        output.concat(_manufacturer);
        output.concat("\"}");
    }

    if (_model != nullptr) {
        output.concat(",{\"model\":\"");
        output.concat(_model);
        output.concat("\"}");
    }

    if (_name != nullptr) {
        output.concat(",{\"name\":\"");
        output.concat(_name);
        output.concat("\"}");
    }

    if (_softwareVersion != nullptr) {
        output.concat(",{\"sw_version\":\"");
        output.concat(_softwareVersion);
        output.concat("\"}");
    }

    if (_viaDevice != nullptr) {
        output.concat(",{\"via_device\":\"");
        output.concat(_viaDevice);
        output.concat("\"}");
    }

    output += "}";
    return output;
}
