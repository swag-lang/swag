#global #[Swag.Safety("overflow", true)]
//#global testerror

const x0 = 1'u8 << 8
const x1 = 1'u32 << 32
const x2 = 1'u64 << 64
const x3 = Swag.U32.Max << 1
const x4 = Swag.U64.Max << 1
const x5 = 1'u32 >> 1
const x6 = 1'u64 >> 1
