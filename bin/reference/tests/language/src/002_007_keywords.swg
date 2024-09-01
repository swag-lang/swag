/**
# Special Keywords
Special keywords are predefined, reserved identifiers that have specific meanings within the Swag compiler. These cannot be used as identifiers in your code as they are integral to the language's structure.

```swag
if
else
elif
while
switch
defer
for
foreach
break
fallthrough
return
case
continue
default
and
or
orelse
unreachable
to
until
where
in
as

true
false
null
undefined

using
with
cast
dref
retval
try
trycatch
catch
assume
throw
discard

public
internal
private

enum
struct
union
impl
interface
func
closure
mtd
namespace
typealias
namealias
attr
var
let
const
moveref
```

# Reserved Keywords
These keywords are reserved by the language and cannot be used by developers, even though they may not currently serve an active role in the syntax. They are reserved for potential future use or to avoid conflicts with language features.

```swag
is
not
do
```

# Basic Types
These are the fundamental data types provided by the language. They are reserved keywords and form the core types that can be used to define variables, constants, and function return types.

```swag
s8
s16
s32
s64
u8
u16
u32
u64
f32
f64
bool
string
rune
any
typeinfo
void
code
cstring
cvarargs
```

# Compiler Instructions
Compiler instructions are prefixed with `#` and are reserved for specific operations within the Swag compiler. These keywords are used to control various aspects of compilation and code generation. User-defined identifiers cannot start with `#`, ensuring that compiler keywords do not conflict with user-defined names.

```swag
#run
#ast
#test
#init
#drop
#main
#premain
#message
#dependencies

#global
#include
#load
#assert
#print
#error
#warning
#foreignlib
#import
#mixin
#macro
#placeholder
#if
#else
#elif
#code
#scope
#up
#type

#defined
#offsetof
#alignof
#sizeof
#typeof
#stringof
#nameof
#isconstexpr
#location
#decltype
#gettag
#hastag
#runes

#cfg
#os
#arch
#cpu
#backend
#module
#file
#line
#self
#curlocation
#callerlocation
#callerfunction
#swagversion
#swagrevision
#swagbuildnum
#swagos

#index
#alias0
#alias1
#alias2 // and more generally #aliasN
#mix0
#mix1
#mix2 // and more generally #mixinN
```

# Miscellaneous Intrinsics
Intrinsic functions are prefixed with `@` and provide low-level operations that are often directly supported by the compiler or underlying hardware. These keywords offer specialized functionality that can be used during both compile time and runtime, depending on the context.

```swag
@err
@args
@assert
@breakpoint
@compiler
@countof
@dataof
@dbgalloc
@drop
@compilererror
@compilerwarning
@getcontext
@pinfos
@init
@isbytecode
@itftableof
@kindof
@mkany
@mkcallback
@mkinterface
@mkslice
@mkstring
@modules
@panic
@postcopy
@postmove
@print
@rtflags
@safety
@setcontext
@spread
@stringcmp
@sysalloc
@is
@typecmp
```

# Intrinsics from libc
These intrinsic functions provide access to standard libc functionalities, allowing developers to perform common mathematical, memory, and string operations. They are also prefixed with `@` to avoid conflicts with user-defined identifiers.

```swag
@abs
@acos
@alloc
@asin
@atan
@atomadd
@atomand
@atomcmpxchg
@atomor
@atomxchg
@atomxor
@bitcountlz
@bitcountnz
@bitcounttz
@byteswap
@ceil
@cos
@cosh
@cvaarg
@cvaend
@cvastart
@exp
@exp2
@floor
@free
@log
@log10
@log2
@max
@memcmp
@memcpy
@memmove
@memset
@muladd
@min
@pow
@realloc
@rol
@ror
@round
@sin
@sinh
@sqrt
@strcmp
@strlen
@tan
@tanh
@trunc
```

# Modifiers
Modifiers can be applied to specific keywords or operators to alter their behavior. These modifiers allow developers to fine-tune operations, giving more control over how certain code constructs are executed.

```swag
#prom
#over
#nodrop
#move
#moveraw
#unconst
#back
#ref
#constref
```
*/