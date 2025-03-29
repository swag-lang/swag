SCBE_MICRO_OP(End,                         MOF_ZERO,                                                                   MOF_ZERO)
SCBE_MICRO_OP(Enter,                       MOF_ZERO,                                                                   MOF_ZERO)
SCBE_MICRO_OP(Leave,                       MOF_ZERO,                                                                   MOF_ZERO)
SCBE_MICRO_OP(Ignore,                      MOF_ZERO,                                                                   MOF_ZERO)
SCBE_MICRO_OP(Label,                       MOF_ZERO,                                                                   MOF_ZERO)
SCBE_MICRO_OP(Debug,                       MOF_ZERO,                                                                   MOF_ZERO)
                                                                                                                       
SCBE_MICRO_OP(Push,                        MOF_REG_A | MOF_READ_REG,                                                   MOF_ZERO)
SCBE_MICRO_OP(Pop,                         MOF_REG_A | MOF_READ_REG,                                                   MOF_ZERO)
SCBE_MICRO_OP(Nop,                         MOF_ZERO,                                                                   MOF_ZERO)
SCBE_MICRO_OP(Ret,                         MOF_ZERO,                                                                   MOF_ZERO)
                                                                                                                       
SCBE_MICRO_OP(SymbolRelocationRef,         MOF_NAME,                                                                   MOF_ZERO)
SCBE_MICRO_OP(SymbolRelocationAddress,     MOF_REG_A | MOF_WRITE_REG,                                                  MOF_VALUE_A | MOF_VALUE_B)
SCBE_MICRO_OP(SymbolRelocationValue,       MOF_REG_A | MOF_WRITE_REG,                                                  MOF_VALUE_A | MOF_VALUE_B)
SCBE_MICRO_OP(SymbolGlobalString,          MOF_REG_A | MOF_WRITE_REG,                                                  MOF_NAME)
SCBE_MICRO_OP(SymbolRelocationPtr,         MOF_REG_A | MOF_WRITE_REG,                                                  MOF_NAME)
                                                                                                                       
SCBE_MICRO_OP(LoadCallerParam,             MOF_REG_A | MOF_OPBITS_A | MOF_WRITE_REG,                                   MOF_VALUE_A)
SCBE_MICRO_OP(LoadCallerAddressParam,      MOF_REG_A | MOF_WRITE_REG,                                                  MOF_VALUE_A)
SCBE_MICRO_OP(LoadCallerZeroExtendParam,   MOF_REG_A | MOF_OPBITS_A | MOF_WRITE_REG,                                   MOF_VALUE_B | MOF_OPBITS_B)
SCBE_MICRO_OP(StoreCallerParam,            MOF_VALUE_A,                                                                MOF_REG_A | MOF_READ_REG)
                                                                                                                       
SCBE_MICRO_OP(CallLocal,                   MOF_NAME,                                                                   MOF_ZERO)
SCBE_MICRO_OP(CallExtern,                  MOF_NAME,                                                                   MOF_ZERO)
SCBE_MICRO_OP(CallIndirect,                MOF_REG_A | MOF_READ_REG | MOF_READ_MEM,                                    MOF_ZERO)
                                                                                                                       
SCBE_MICRO_OP(JumpTable,                   MOF_ZERO,                                                                   MOF_ZERO)
SCBE_MICRO_OP(JumpCC,                      MOF_JUMP_TYPE | MOF_OPBITS_A,                                               MOF_ZERO)
SCBE_MICRO_OP(JumpM,                       MOF_REG_A | MOF_READ_REG | MOF_READ_MEM,                                    MOF_ZERO)
SCBE_MICRO_OP(JumpCI,                      MOF_JUMP_TYPE | MOF_VALUE_A,                                                MOF_ZERO)
SCBE_MICRO_OP(PatchJump,                   MOF_ZERO,                                                                   MOF_ZERO)
                                                                                                                       
SCBE_MICRO_OP(LoadRR,                      MOF_REG_A | MOF_OPBITS_A | MOF_WRITE_REG,                                   MOF_REG_B | MOF_OPBITS_A | MOF_READ_REG)
SCBE_MICRO_OP(LoadRI64,                    MOF_REG_A | MOF_WRITE_REG,                                                  MOF_VALUE_A)
SCBE_MICRO_OP(LoadRI,                      MOF_REG_A | MOF_OPBITS_A | MOF_WRITE_REG,                                   MOF_VALUE_A)
SCBE_MICRO_OP(LoadRM,                      MOF_REG_A | MOF_OPBITS_A | MOF_WRITE_REG,                                   MOF_REG_B | MOF_VALUE_A | MOF_READ_REG | MOF_READ_MEM)
SCBE_MICRO_OP(LoadMR,                      MOF_REG_A | MOF_VALUE_A | MOF_READ_REG | MOF_WRITE_MEM,                     MOF_REG_B | MOF_READ_REG | MOF_OPBITS_A)
SCBE_MICRO_OP(LoadMI,                      MOF_REG_A | MOF_VALUE_A | MOF_READ_REG | MOF_WRITE_MEM,                     MOF_VALUE_B | MOF_OPBITS_A)
                                                                                                                       
SCBE_MICRO_OP(LoadSignedExtendRM,          MOF_REG_A | MOF_OPBITS_A | MOF_WRITE_REG,                                   MOF_REG_B | MOF_VALUE_A | MOF_READ_REG | MOF_READ_MEM | MOF_OPBITS_B)
SCBE_MICRO_OP(LoadZeroExtendRM,            MOF_REG_A | MOF_OPBITS_A | MOF_WRITE_REG,                                   MOF_REG_B | MOF_VALUE_A | MOF_READ_REG | MOF_READ_MEM | MOF_OPBITS_B)
SCBE_MICRO_OP(LoadSignedExtendRR,          MOF_REG_A | MOF_OPBITS_A | MOF_WRITE_REG,                                   MOF_REG_B | MOF_READ_REG | MOF_OPBITS_B)
SCBE_MICRO_OP(LoadZeroExtendRR,            MOF_REG_A | MOF_OPBITS_A | MOF_WRITE_REG,                                   MOF_REG_B | MOF_READ_REG | MOF_OPBITS_B)
                                                                                                                       
SCBE_MICRO_OP(LoadAddressM,                MOF_REG_A | MOF_WRITE_REG,                                                  MOF_REG_B | MOF_READ_REG | MOF_VALUE_A | MOF_READ_MEM | MOF_WRITE_MEM)
SCBE_MICRO_OP(LoadAddressAddMul,           MOF_REG_A | MOF_WRITE_REG,                                                  MOF_REG_B | MOF_REG_C | MOF_VALUE_A | MOF_OPBITS_A | MOF_READ_REG)
                                                                                                                                                                                                                                             
SCBE_MICRO_OP(CmpRR,                       MOF_REG_A | MOF_READ_REG | MOF_OPBITS_A,                                    MOF_REG_B | MOF_READ_REG | MOF_OPBITS_A)
SCBE_MICRO_OP(CmpRI,                       MOF_REG_A | MOF_READ_REG | MOF_OPBITS_A,                                    MOF_VALUE_A | MOF_OPBITS_A)
SCBE_MICRO_OP(CmpMR,                       MOF_REG_A | MOF_VALUE_A | MOF_READ_REG | MOF_READ_MEM,                      MOF_REG_B | MOF_READ_REG | MOF_OPBITS_A)
SCBE_MICRO_OP(CmpMI,                       MOF_REG_A | MOF_VALUE_A | MOF_READ_REG | MOF_READ_MEM,                      MOF_VALUE_B | MOF_OPBITS_A)
                                                                                                                       
SCBE_MICRO_OP(SetCC,                       MOF_CPU_COND | MOF_REG_A | MOF_READ_REG,                                    MOF_ZERO)                                                                                                                      
SCBE_MICRO_OP(ClearR,                      MOF_REG_A | MOF_WRITE_REG | MOF_OPBITS_A,                                   MOF_ZERO)

SCBE_MICRO_OP(OpUnaryM,                    MOF_CPU_OP | MOF_REG_A | MOF_VALUE_A | MOF_READ_REG | MOF_WRITE_MEM,        MOF_ZERO)
SCBE_MICRO_OP(OpUnaryR,                    MOF_CPU_OP | MOF_REG_A | MOF_WRITE_REG | MOF_OPBITS_A,                      MOF_ZERO)

SCBE_MICRO_OP(OpBinaryRR,                  MOF_CPU_OP | MOF_REG_A | MOF_WRITE_REG | MOF_OPBITS_A,                      MOF_REG_B | MOF_READ_REG | MOF_OPBITS_A)
SCBE_MICRO_OP(OpBinaryRI,                  MOF_CPU_OP | MOF_REG_A | MOF_WRITE_REG | MOF_OPBITS_A,                      MOF_VALUE_A | MOF_OPBITS_A)
SCBE_MICRO_OP(OpBinaryRM,                  MOF_CPU_OP | MOF_REG_A | MOF_WRITE_REG | MOF_OPBITS_A,                      MOF_REG_B | MOF_VALUE_A | MOF_READ_REG | MOF_READ_MEM)
SCBE_MICRO_OP(OpBinaryMR,                  MOF_CPU_OP | MOF_REG_A | MOF_VALUE_A | MOF_READ_REG | MOF_WRITE_MEM,        MOF_REG_B | MOF_READ_REG | MOF_OPBITS_A)
SCBE_MICRO_OP(OpBinaryMI,                  MOF_CPU_OP | MOF_REG_A | MOF_VALUE_A | MOF_READ_REG | MOF_WRITE_MEM,        MOF_VALUE_B | MOF_OPBITS_A)

SCBE_MICRO_OP(OpTernaryRRR,                MOF_REG_A | MOF_WRITE_REG | MOF_OPBITS_A,                                   MOF_REG_A | MOF_REG_B | MOF_REG_C | MOF_OPBITS_A | MOF_READ_REG)
