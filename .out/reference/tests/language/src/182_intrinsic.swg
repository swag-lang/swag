#global skip

// This is the list of all compiler intrinsics
// All intrinsics start with '@', which is reserved for them

// Base

func @assert(value: bool);
func @dbgbreak();
func @getcontext()->*Swag.Context;
func @setcontext(context: const *Swag.Context);
func @isbytecode()->bool;
func @compiler()->Swag.ICompiler;

func @args()->const [..] string;
func @seterr(err: string);
@panic()
@errormsg()

// Buildin

@spread()
@init()
@drop()
@postmove()
@postcopy()
@reloc()
@sizeof()
@alignof()
@offsetof()
@typeof()
@kindof()
@countof()
@stringof()
@dataof()
@mkslice()
@mkstring()
@mkany()
@mkinterface()
@mkcallback()
@mkforeign()
@index
@isconstexpr
@err

// Memory related

func @alloc(size: uint)->*void;
func @realloc(ptr: *void, size: uint)->*void;
func @free(ptr: *void);
func @memset(dst: *void, value: u8, size: uint);
func @memcpy(dst: *void, src: const *void, size: uint);
func @memmove(dst: *void, src: const *void, size: uint);
func @memcmp(dst, src: const *void, size: uint)->s32;
func @cstrlen(value: const *u8)->uint;
@strcmp()
@typecmp()

// Atomic operations

func @atomadd(addr: *s8, value: s8)->s8;
func @atomadd(addr: *s16, value: s16)->s16;
func @atomadd(addr: *s32, value: s32)->s32;
func @atomadd(addr: *s64, value: s64)->s64;
func @atomadd(addr: *u8, value: u8)->u8;
func @atomadd(addr: *u16, value: u16)->u16;
func @atomadd(addr: *u32, value: u32)->u32;
func @atomadd(addr: *u64, value: u64)->u64;

func @atomand(addr: *s8, value: s8)->s8;
func @atomand(addr: *s16, value: s16)->s16;
func @atomand(addr: *s32, value: s32)->s32;
func @atomand(addr: *s64, value: s64)->s64;
func @atomand(addr: *u8, value: u8)->u8;
func @atomand(addr: *u16, value: u16)->u16;
func @atomand(addr: *u32, value: u32)->u32;
func @atomand(addr: *u64, value: u64)->u64;

func @atomor(addr: *s8, value: s8)->s8;
func @atomor(addr: *s16, value: s16)->s16;
func @atomor(addr: *s32, value: s32)->s32;
func @atomor(addr: *s64, value: s64)->s64;
func @atomor(addr: *u8, value: u8)->u8;
func @atomor(addr: *u16, value: u16)->u16;
func @atomor(addr: *u32, value: u32)->u32;
func @atomor(addr: *u64, value: u64)->u64;

func @atomxor(addr: *s8, value: s8)->s8;
func @atomxor(addr: *s16, value: s16)->s16;
func @atomxor(addr: *s32, value: s32)->s32;
func @atomxor(addr: *s64, value: s64)->s64;
func @atomxor(addr: *u8, value: u8)->u8;
func @atomxor(addr: *u16, value: u16)->u16;
func @atomxor(addr: *u32, value: u32)->u32;
func @atomxor(addr: *u64, value: u64)->u64;

func @atomxchg(addr: *s8,  exchangeWith: s8)->s8;
func @atomxchg(addr: *s16, exchangeWith: s16)->s16;
func @atomxchg(addr: *s32, exchangeWith: s32)->s32;
func @atomxchg(addr: *s64, exchangeWith: s64)->s64;
func @atomxchg(addr: *u8,  exchangeWith: u8)->u8;
func @atomxchg(addr: *u16, exchangeWith: u16)->u16;
func @atomxchg(addr: *u32, exchangeWith: u32)->u32;
func @atomxchg(addr: *u64, exchangeWith: u64)->u64;

func @atomcmpxchg(addr: *s8,  compareTo, exchangeWith: s8)->s8;
func @atomcmpxchg(addr: *s16, compareTo, exchangeWith: s16)->s16;
func @atomcmpxchg(addr: *s32, compareTo, exchangeWith: s32)->s32;
func @atomcmpxchg(addr: *s64, compareTo, exchangeWith: s64)->s64;
func @atomcmpxchg(addr: *u8,  compareTo, exchangeWith: u8)->u8;
func @atomcmpxchg(addr: *u16, compareTo, exchangeWith: u16)->u16;
func @atomcmpxchg(addr: *u32, compareTo, exchangeWith: u32)->u32;
func @atomcmpxchg(addr: *u64, compareTo, exchangeWith: u64)->u64;

// Math

func @sqrt(value: f32)->f32;
func @sqrt(value: f64)->f64;

func @sin(value: f32)->f32;
func @sin(value: f64)->f64;
func @cos(value: f32)->f32;
func @cos(value: f64)->f64;
func @tan(value: f32)->f32;
func @tan(value: f64)->f64;

func @sinh(value: f32)->f32;
func @sinh(value: f64)->f64;
func @cosh(value: f32)->f32;
func @cosh(value: f64)->f64;
func @tanh(value: f32)->f32;
func @tanh(value: f64)->f64;

func @asin(value: f32)->f32;
func @asin(value: f64)->f64;
func @acos(value: f32)->f32;
func @acos(value: f64)->f64;
func @atan(value: f32)->f32;
func @atan(value: f64)->f64;

func @log(value: f32)->f32;
func @log(value: f64)->f64;
func @log2(value: f32)->f32;
func @log2(value: f64)->f64;
func @log10(value: f32)->f32;
func @log10(value: f64)->f64;

func @floor(value: f32)->f32;
func @floor(value: f64)->f64;
func @ceil(value: f32)->f32;
func @ceil(value: f64)->f64;
func @trunc(value: f32)->f32;
func @trunc(value: f64)->f64;
func @round(value: f32)->f32;
func @round(value: f64)->f64;

func @abs(value: s8)->s8;
func @abs(value: s16)->s16;
func @abs(value: s32)->s32;
func @abs(value: s64)->s64;
func @abs(value: f32)->f32;
func @abs(value: f64)->f64;

func @exp(value: f32)->f32;
func @exp(value: f64)->f64;
func @exp2(value: f32)->f32;
func @exp2(value: f64)->f64;
func @pow(value1, value2: f32)->f32;
func @pow(value1, value2: f64)->f64;

func @min(value1, value2: s8)->s8;
func @min(value1, value2: s16)->s16;
func @min(value1, value2: s32)->s32;
func @min(value1, value2: s64)->s64;
func @min(value1, value2: u8)->u8;
func @min(value1, value2: u16)->u16;
func @min(value1, value2: u32)->u32;
func @min(value1, value2: u64)->u64;
func @min(value1, value2: f32)->f32;
func @min(value1, value2: f64)->f64;

func @max(value1, value2: s8)->s8;
func @max(value1, value2: s16)->s16;
func @max(value1, value2: s32)->s32;
func @max(value1, value2: s64)->s64;
func @max(value1, value2: u8)->u8;
func @max(value1, value2: u16)->u16;
func @max(value1, value2: u32)->u32;
func @max(value1, value2: u64)->u64;
func @max(value1, value2: f32)->f32;
func @max(value1, value2: f64)->f64;

func @bitcountnz(value: u8)->u8;
func @bitcountnz(value: u16)->u16;
func @bitcountnz(value: u32)->u32;
func @bitcountnz(value: u64)->u64;
func @bitcounttz(value: u8)->u8;
func @bitcounttz(value: u16)->u16;
func @bitcounttz(value: u32)->u32;
func @bitcounttz(value: u64)->u64;
func @bitcountlz(value: u8)->u8;
func @bitcountlz(value: u16)->u16;
func @bitcountlz(value: u32)->u32;
func @bitcountlz(value: u64)->u64;

func @byteswap(value: u16)->u16;
func @byteswap(value: u32)->u32;
func @byteswap(value: u64)->u64;