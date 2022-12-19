/**
This is the list of all keywords:
*/

#global skip // At the beginning of a file, this tells Swag to NOT take care of the rest.

// Compiler keywords
// -----------------

#arch
#backend
#callerfunction
#callerlocation
#cfg
#code
#file
#line
#location
#module
#os
#self
#swagbuildnum
#swagos
#swagrevision
#swagversion
#type

#assert
#checkif
#elif
#else
#global
#if
#import
#inline
#include
#load
#macro
#mixin
#placeholder
#print
#selectif

#ast
#drop
#init
#main
#message
#run
#test

// Intrinsics libc
// ---------------

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
@min
@pow
@realloc
@round
@sin
@sinh
@sqrt
@strcmp
@strlen
@tan
@tanh
@trunc

// Intrinsics
// ----------

@index
@err
@alias0
@alias1
@alias2 ...

@alignof
@args
@assert
@breakpoint
@compiler
@countof
@dataof
@dbgalloc
@defined
@drop
@errormsg
@getcontext
@getpinfos
@gettag
@hastag
@init
@interfaceof
@isbytecode
@isconstexpr
@itftableof
@kindof
@mkany
@mkcallback
@mkinterface
@mkslice
@mkstring
@modules
@nameof
@offsetof
@panic
@postcopy
@postmove
@print
@rtflags
@safety
@setcontext
@sizeof
@spread
@stringcmp
@stringof
@typecmp
@typeof

// Modifiers
// ---------

,move
,nodrop
,moveraw
,safe
,small

// Language keywords
// -----------------

acast
alias
assume
attr
bitcast
cast
catch
closure
const
discard
dref
enum
false
func
impl
interface
namespace
null
private
public
ref
retval
struct
throw
true
try
union
using
var

and
break
case
continue
default
defer
elif
else
fallthrough
for
if
scope
loop
or
return
switch
visit
while

// Basic types
// -----------

any
bool
code
cstring
cvarargs
f32
f64
int
rune
s16
s32
s64
s8
string
typeinfo
u16
u32
u64
u8
uint
void
