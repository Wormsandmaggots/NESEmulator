//
// Created by Radek on 05.11.2024.
//

#ifndef LOGMESSAGES_H
#define LOGMESSAGES_H

namespace error {
    constexpr const char* memoryOutOfBounds = "Out of bounds memory";
    constexpr const char* unsupportedTemplateType = "Unsupported template type";
    constexpr const char* unsupportedAddressingModeWithU16 = "Invalid address mode with U16";
    constexpr const char* unsupportedAddressingModeWithU8 = "Invalid address mode with U8";
    constexpr const char* fetchProblem = "Fetch only applies to U8 or U16";
    constexpr const char* unimplementedOpcode = "Use of unimplemented opcode";
    constexpr const char* illegalOpcode = "Use of illegal opcode";
    constexpr const char* cantOpenFile = "Can't open file";
    constexpr const char* unsupportedFileFormat = "Unsupported file format";
    constexpr const char* invalidVariantTypeGet = "Invalid variant type format";
    constexpr const char* unsupportedMapper = "Unsupported mapper";
}

#endif //LOGMESSAGES_H
