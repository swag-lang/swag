#global #[Swag.Safety(.All, .Overflow, true)]
//#global testerror

const x1 = 10'u32 >> 32
const x2 = 10'u64 >> 64
const x3 = 10'u32 << 32
const x4 = 10'u64 << 64
