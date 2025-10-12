/**
# Special Keywords

Special keywords are predefined, reserved identifiers that have specific meanings
within the Swag compiler. These cannot be used as identifiers in your code, as they
are integral to the language's structure.

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
is
do

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
mtd
namespace
alias
attr
var
let
const
moveref
````

*/

/**

# Reserved Keywords

These keywords are reserved by the language and cannot be used by developers,
even though they may not currently serve an active role in the syntax. They are
reserved for potential future use or to prevent conflicts with language features.

```swag
not
```

*/

/**

# Basic Types

These are the fundamental data types provided by the language. They are reserved
keywords and form the core building blocks for variables, constants, and function
return types.

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
cstring
cvarargs
```

*/

/**

# Compiler Instructions

Compiler instructions are prefixed with '#'. They are reserved for specific operations
within the Swag compiler and control various aspects of compilation and code generation.
User-defined identifiers cannot start with '#', ensuring there are no naming conflicts.

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
#load
#foreignlib
#assert
#print
#error
#warning
#import
#if
#else
#elif
#inject
#macro
#scope

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
#safety
#include

#cfg
#os
#arch
#cpu
#backend
#module
#file
#line
#me
#curlocation
#callerlocation
#callerfunction
#swagversion
#swagrevision
#swagbuildnum
#swagos

#code
#type
#up

#alias0
#alias1
#alias2 // and more generally #aliasN
#uniq0
#uniq1
#uniq2 // and more generally #mixinN
```

*/

/**

# Intrinsic Values

Intrinsic values begin with '@' and are provided by the compiler for introspection
or internal state access.

```swag
@err
@index
@args
@pinfos
@bytecode
@compiler
@modules
@gvtd
@rtflags
```

*/

/**

# Miscellaneous Intrinsics

Intrinsic functions are prefixed with '@' and provide low-level operations directly
supported by the compiler or hardware. They can be used at both compile time and runtime.

```swag
@kindof
@countof
@dataof
@mkslice
@mkstring
@mkcallback
@mkany
@mkinterface
@dbgalloc
@sysalloc
@stringcmp
@typecmp
@is
@as
@getcontext
@tableof

@assert
@breakpoint
@init
@drop
@postcopy
@postmove
@compilererror
@compilerwarning
@panic
@print
@setcontext
```

*/

/**

# Intrinsics from 'libc'

These intrinsic functions expose standard 'libc' functionalities, allowing developers
to perform common mathematical, memory, and string operations. They are also prefixed
with '@' to prevent conflicts with user-defined identifiers.

```swag
@abs
@acos
@asin
@atan
@atan2
@alloc
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
@cvaarg
@cvaend
@cvastart
@ceil
@cos
@cosh
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
@min
@muladd
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

*/

/**

# Modifiers

Modifiers can be applied to specific keywords or operators to adjust their behavior.
They give developers finer control over how code is interpreted or executed.

```swag
#prom
#wrap
#nodrop
#move
#moveraw
#reverse
#ref
#constref
#err
#noerr
#null
```

*/
