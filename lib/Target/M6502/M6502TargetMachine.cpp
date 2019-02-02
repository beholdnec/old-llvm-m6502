// TODO: license stuff

#include "M6502TargetMachine.h"
#include "M6502.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/Twine.h"
using namespace llvm;

#define DEBUG_TYPE "m6502";

namespace llvm {
extern Target TheM6502Target;
}

extern "C" void LLVMInitializeM6502Target() {
  RegisterTargetMachine<M6502TargetMachine> X(TheM6502Target);
}

M6502TargetMachine::M6502TargetMachine(const Target &T, const Triple &TT,
                                       StringRef CPU, StringRef FS,
                                       const TargetOptions &Options,
                                       Optional<Reloc::Model> RM,
                                       Optional<CodeModel::Model> CM,
                                       CodeGenOpt::Level OL, bool JIT)
    : LLVMTargetMachine(T, "e-p:16:8-n8", TT, CPU, FS, Options,
                        RM.getValueOr(Reloc::Static),
                        CM.getValueOr(CodeModel::Small), OL),
      TLOF(llvm::make_unique<M6502TargetObjectFile>()),
      Subtarget(TT, CPU, FS, *this) {

  initAsmInfo();
}

namespace {
class M6502PassConfig : public TargetPassConfig {
public:
  M6502PassConfig(M6502TargetMachine &TM, PassManagerBase &PM)
    : TargetPassConfig(TM, PM) {}

  M6502TargetMachine &getM6502TargetMachine() const {
    return getTM<M6502TargetMachine>();
  }

  /// Add common target configurable passes that perform LLVM IR to IR
  /// transforms following machine independent optimization.
  void addIRPasses() override;
  /// Add pass to prepare the LLVM IR for code generation. This should be done
  /// before exception handling preparation passes.
  void addCodeGenPrepare() override;
  /// Add common passes that perform LLVM IR to IR transforms in preparation for
  /// instruction selection.
  void addISelPrepare() override;
  /// addInstSelector - This method should install an instruction selector pass,
  /// which converts from LLVM code to machine instructions.
  bool addInstSelector() override;
  void addPreSched2() override;
};
} // namespace

TargetPassConfig *M6502TargetMachine::createPassConfig(PassManagerBase &PM) {
  return new M6502PassConfig(*this, PM);
}

void M6502PassConfig::addIRPasses() {
  addPass(createPrinterPass(dbgs(), "Hello. IR Passes go here."));
  TargetPassConfig::addIRPasses();
}

void M6502PassConfig::addCodeGenPrepare() {
  addPass(createPrinterPass(dbgs(), "Hello. CodeGen Prepare Passes go here."));
  TargetPassConfig::addCodeGenPrepare();
}

void M6502PassConfig::addISelPrepare() {
  addPass(createPrinterPass(dbgs(), "Hello. ISel Prepare Passes go here."));
  TargetPassConfig::addISelPrepare();
}

bool M6502PassConfig::addInstSelector() {
  TargetPassConfig::addInstSelector();
  addPass(createM6502ISelDag(getM6502TargetMachine(), getOptLevel()));
  return false;
}

void M6502PassConfig::addPreSched2() {
  addPass(createM6502Expand16BitPseudoPass());
}
