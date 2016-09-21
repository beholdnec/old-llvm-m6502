// TODO: header stuff

// the llvm configuration program activates Asm Printer for the target if it
// finds this file in the target subdirectory.

#include "M6502.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

namespace llvm {
class MCStreamer;

class LLVM_LIBRARY_VISIBILITY M6502AsmPrinter : public AsmPrinter {
public:
  explicit M6502AsmPrinter(TargetMachine &TM,
                           std::unique_ptr<MCStreamer> Streamer)
      : AsmPrinter(TM, std::move(Streamer)) {}

  const char *getPassName() const override {
    return "M6502 Assembly Printer";
  }

  void EmitInstruction(const MachineInstr *MI) override;

private:
  MCOperand LowerSymbolOperand(MCSymbol *Sym, int64_t Offset);
  void LowerMachineInstrToMCInst(const MachineInstr *MI, MCInst &OutMI);
};
} // end namespace llvm

extern "C" void LLVMInitializeM6502AsmPrinter() {
  RegisterAsmPrinter<M6502AsmPrinter> X(TheM6502Target);
}

void M6502AsmPrinter::EmitInstruction(const MachineInstr *MI) {
  // TODO
  MCInst TmpInst;
  LowerMachineInstrToMCInst(MI, TmpInst);
  EmitToStreamer(*OutStreamer, TmpInst);
}

// See WebAssemblyMCInstLower.cpp (and similar functions in other backends)
MCOperand M6502AsmPrinter::LowerSymbolOperand(MCSymbol *Sym, int64_t Offset) {
  const MCExpr *Expr = MCSymbolRefExpr::create(Sym, OutContext);
  if (Offset != 0) {
    Expr = MCBinaryExpr::createAdd(Expr,
                                   MCConstantExpr::create(Offset, OutContext),
                                   OutContext);
  }
  return MCOperand::createExpr(Expr);
}

void M6502AsmPrinter::LowerMachineInstrToMCInst(const MachineInstr *MI, MCInst &OutMI) {
  // TODO
  // XXX: copied from BPFMCInstLower.cpp
  OutMI.setOpcode(MI->getOpcode());

  for (unsigned i = 0, e = MI->getNumOperands(); i != e; ++i) {
    const MachineOperand &MO = MI->getOperand(i);

    MCOperand MCOp;
    switch (MO.getType()) {
    default:
      MI->dump();
      llvm_unreachable("unknown operand type");
      break;
    case MachineOperand::MO_Register:
      // Ignore all implicit register operands.
      if (MO.isImplicit())
        continue;
      MCOp = MCOperand::createReg(MO.getReg());
      break;
    case MachineOperand::MO_Immediate:
      MCOp = MCOperand::createImm(MO.getImm());
      break;
    case MachineOperand::MO_MachineBasicBlock:
      MCOp = MCOperand::createExpr(
          MCSymbolRefExpr::create(MO.getMBB()->getSymbol(), OutContext));
      break;
    case MachineOperand::MO_RegisterMask:
      continue;
    case MachineOperand::MO_GlobalAddress:
      assert(MO.getTargetFlags() == 0 &&
             "M6502 does not use target flags on GlobalAddresses");
      MCOp = LowerSymbolOperand(getSymbol(MO.getGlobal()), MO.getOffset());
      break;
    // TODO: case MachineOperand::MO_ExternalSymbol
    }

    OutMI.addOperand(MCOp);
  }
}
