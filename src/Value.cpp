// C++ header files
#include <string>
// My header files
#include "../include/Value.hpp"

namespace Yun::VM::Primitives {

    [[nodiscard]] auto Reference::ToString() const -> std::string {
        std::string retVal{ "(HeapID: " };
        retVal += std::to_string(HeapID) + ", ArrayIndex: ";
        retVal += std::to_string(ArrayIndex) + ")";
        return retVal;
    }

    [[nodiscard]] auto Value::ToString(bool verbose) const -> std::string {
        std::string retVal{ verbose? "(" : "" };

        switch (_type) {
        case Type::Uninit:
            retVal.append("null");
            break;
        case Type::Int8:
            retVal.append(std::to_string(_as.int8));
            break;
        case Type::Int16:
            retVal.append(std::to_string(_as.int16));
            break;
        case Type::Int32:
            retVal.append(std::to_string(_as.int32));
            break;
        case Type::Int64:
            retVal.append(std::to_string(_as.int64));
            break;
        case Type::Uint8:
            retVal.append(std::to_string(_as.uint8));
            break;
        case Type::Uint16:
            retVal.append(std::to_string(_as.uint16));
            break;
        case Type::Uint32:
            retVal.append(std::to_string(_as.uint32));
            break;
        case Type::Uint64:
            retVal.append(std::to_string(_as.uint64));
            break;
        case Type::Float32:
            retVal.append(std::to_string(_as.float32));
            break;
        case Type::Float64:
            retVal.append(std::to_string(_as.float64));
            break;
        case Type::Reference:
            retVal.append(_as.ref.ToString());
            break;
        }
        if (verbose) {
            retVal.append(": ");
            retVal.append(TypeToString(_type));
            retVal.append(")");
        }
        return retVal;
    }
}