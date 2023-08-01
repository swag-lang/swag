#global export

typealias int8_t    = s8
typealias int16_t   = s16
typealias int32_t   = s32
typealias int64_t   = s64
typealias uint8_t   = u8
typealias uint16_t  = u16
typealias uint32_t  = u32
typealias uint64_t  = u64

typealias int_least8_t   = s8
typealias int_least16_t  = s16
typealias int_least32_t  = s32
typealias int_least64_t  = s64
typealias uint_least8_t  = u8
typealias uint_least16_t = u16
typealias uint_least32_t = u32
typealias uint_least64_t = u64

typealias int_fast8_t   = s8
typealias int_fast16_t  = s32
typealias int_fast32_t  = s32
typealias int_fast64_t  = s64
typealias uint_fast8_t  = u8
typealias uint_fast16_t = u32
typealias uint_fast32_t = u32
typealias uint_fast64_t = u64

typealias intptr_t  = s64
typealias uintptr_t = u64
typealias ptrdiff_t = s64
typealias intmax_t  = s64
typealias uintmax_t = u64

const SIZE_MAX    = Swag.U64.Max
const PTRDIFF_MIN = Swag.S64.Min
const PTRDIFF_MAX = Swag.S64.Max

const INT8_MAX   = Swag.S8.Max
const INT16_MAX  = Swag.S16.Max
const INT32_MAX  = Swag.S32.Max
const INT64_MAX  = Swag.S64.Max

const INT8_MIN   = Swag.S8.Min
const INT16_MIN  = Swag.S16.Min
const INT32_MIN  = Swag.S32.Min
const INT64_MIN  = Swag.S64.Min

const UINT8_MAX  = Swag.U8.Max
const UINT16_MAX = Swag.U16.Max
const UINT32_MAX = Swag.U32.Max
const UINT64_MAX = Swag.U64.Max