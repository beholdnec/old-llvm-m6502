// TODO: header stuff

#ifndef LLVM_LIB_TARGET_MOS6502_MOS6502TARGETOBJECTFILE_H
#define LLVM_LIB_TARGET_MOS6502_MOS6502TARGETOBJECTFILE_H

#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"

namespace llvm {

// FIXME: We don't actually know what kind of Object File to generate
// The linker will take Object Files and generate a ROM or memory image.
class Mos6502TargetObjectFile : public TargetLoweringObjectFileELF {
public:
};

} // end namespace llvm

#endif
