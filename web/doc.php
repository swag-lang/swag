<style>code br {display: none;}</style><?php include('html_start.php'); ?><?php include('header.php'); ?><div class="lg:m-auto lg:w-[70rem] pt-2 lg:pt-10 overflow-hidden p-5"><div class="mb-10"><div class="text-2xl mb-5 border-b">introduction</div><code style="white-space: break-spaces;">/*

This 'swag/bin/reference/language' module is a short explanation of the Swag language, the basic syntax and usage.
It will only describe the language itself, without the need of the Swag standard libraries (Std).

Things like dynamic arrays, dynamic strings, hash maps and so on are written in the 'Std.Core' module, so
you won't find them in the examples to come.

As 'reference/language' is written as a test module, you can run it with 'swag run -w:/reference'

*/</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">source</div><code style="white-space: break-spaces;">/*

All swag source files have the '.swg' extension.
They must be encoded in UTF8.

In Swag you cannot compile a single file (with the exception of '.swgs' files which are single script files)
A source tree is organized in a workspace which contains modules.

For example the Swag standard library is in fact a collection of modules in the 'Std' workspace.

A module is a dll (windows) or an executable.

A workspace can contain as many executables as you want, and as many modules as you want.
You will typically compile the full workspace.

*/</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">comments</div><code style="white-space: break-spaces;">// One line comment

/*
    Multi lines comment
*/

/*
    /* Nested multi lines comment are fine */
*/

const A = 0 // One line comment

const B = /* Comment */ true</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">identifiers</div><code style="white-space: break-spaces;">// User identifiers start with '_' or an ascii letter.
// Identifiers can contain '_', an ascii letter or a digit number.

const thisIsAValidIdentifier0       = 0
const this_is_also_valid            = 0
const this_1_is_2_also__3_valid     = 0

// User identifiers cannot start with two underscores. This is reserved by the compiler.
// const __invalid = 0

/*

Some identifiers start with '#'. This indicates a compiler special keyword.

#assert
#run
#main
...

Some identifiers start with '@'. This indicates an intrinsic function.

@min()
@max()
@sqrt()
...

*/</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">keywords</div><code style="white-space: break-spaces;">#global skip // At the beginning of a file, this tells Swag to NOT take care of the rest.

// This is the list of all keywords:

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
#semerror
#swagbuildnum
#swagos
#swagrevision
#swagversion

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
enum
false
func
impl
interface
namespace
null
private
public
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
</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">semicolon</div><code style="white-space: break-spaces;">#test
{
    // No need for ';' to end a statement like in C/C++
    // Most of the time a end of line is enough.
    {
        var x, y = 0
        x += 1
        y += 1
        @assert(x == 1)
        @assert(y == x)
    }

    // But ';' is possible too
    {
        var x, y = 0;
        x += 1;
        y += 1;
    }

    // This is usefull if you want to do multiple things on the same line
    {
        var x = 0; var y = 0
        x += 1; y += 1
        @assert(x == 1); @assert(y == 1)
    }
}</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">order</div><code style="white-space: break-spaces;">// Order of top level declarations does not matter
const A = B
const B = C
const C = 1 // C could also be defined in another file.

#test
{
    funcDeclaredLater()
}

func funcDeclaredLater() {}</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">types</div><code style="white-space: break-spaces;">#test
{
    // This are all signed integers types
    {
        var a: s8 = -1      // 8 bits signed integer
        var b: s16 = -2     // 16 bits signed integer
        var c: s32 = -3     // 32 bits signed integer
        var d: s64 = -4     // 64 bits signed integer
        var e: int = -4     // 64 bits signed integer

        // The instrinsic '@assert' will panic at runtime if the enclosed expression is false
        @assert(a == -1)
        @assert(b == -2)
        @assert(c == -3)
        @assert(d == -4)

        // The instrinsic '@sizeof' gives the size, in bytes, of a variable
        @assert(@sizeof(a) == 1)
        @assert(@sizeof(b) == 2)
        @assert(@sizeof(c) == 4)
        @assert(@sizeof(d) == 8)
        @assert(@sizeof(e) == 8)
    }

    // This are all unsigned integers types
    {
        var a: u8 = 1       // 8 bits unsigned integer
        var b: u16 = 2      // 16 bits unsigned integer
        var c: u32 = 3      // 32 bits unsigned integer
        var d: u64 = 4      // 64 bits unsigned integer
        var e: uint = 4     // 64 bits unsigned integer

        @assert(a == 1)
        @assert(b == 2)
        @assert(c == 3)
        @assert(d == 4)

        @assert(@sizeof(a) == 1)
        @assert(@sizeof(b) == 2)
        @assert(@sizeof(c) == 4)
        @assert(@sizeof(d) == 8)
        @assert(@sizeof(e) == 8)
    }

    // This are all float types
    {
        var a: f32 = 3.14       // 32 bits floating point value
        var b: f64 = 3.14159    // 64 bits floating point value

        @assert(a == 3.14)
        @assert(b == 3.14159)

        @assert(@sizeof(a) == 4)
        @assert(@sizeof(b) == 8)
    }

    // The boolean type
    {
        var a: bool = true      // Stored in 1 byte
        var b: bool = false     // Stored in 1 byte

        @assert(a == true)
        @assert(b == false)

        @assert(@sizeof(a) == 1)
        @assert(@sizeof(b) == 1)
    }

    // The string type.
    // Strings are UTF8, and are stored as two 64 bits (the pointer to the value and the string length in bytes)
    {
        var a: string = "string 是"
        @assert(a == "string 是")
        @assert(@sizeof(a) == 2 * @sizeof(*void))
    }

    // The 'rune' type uses the string syntax, postfix with the type 'rune'.
    // It's a 32 bits unicode code point.
    {
        var a: rune = "是"'rune
        @assert(a == "是"'rune)
        @assert(@sizeof(a) == 4)
    }

    // Swag has type reflection.
    // You can use '@typeof' to get the type of an expression.
    {
        var a = 0
        var b: @typeof(a) = 1   // 'b' will have the same type as 'a'
        @assert(@typeof(a) == @typeof(b))
        @assert(@typeof(a) == s32)

        // As the types of 'a' and 'b' are known at compile time, we can use '#assert' instead of '@assert'
        // The #assert will be done by the compiler, and will not generate runtime code (unlike '@assert')
        #assert @typeof(a) == @typeof(b)
        #assert @typeof(a) == s32
    }

    // Types are values, at compile time and at runtime.
    {
        x := @typeof(s32)
        @assert(x == s32)
        @assert(x.name == "s32")
    }
}</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">literals</div><code style="white-space: break-spaces;">#test
{
    // Integers in decimal, hexadecimal or binary forms
    {
        const a: u32 = 123456           // Decimal
        const b: u32 = 0xFFFF           // Hexadecimal, with '0x'
        const c: u32 = 0b00001111       // Binary, with '0b'
        @assert(a == 123456)
        @assert(b == 65535)
        @assert(c == 15)
    }

    // You can separate digits with the '_' character
    {
        const a: u32 = 123_456
        const b: u32 = 0xF_F_F_F
        const c: u32 = 0b0000_1111
        @assert(a == 123456)
        @assert(b == 65_535)
        @assert(c == 15)
    }

    // The default type of an hexadecimal number or a binary number is 'u32' or 'u64' depending on its value
    {
        const a = 0xFF
        #assert @typeof(a) == u32
        const b = 0xFFFFFF_FFFFFF
        #assert @typeof(b) == u64
        const c = 0b00000001
        #assert @typeof(c) == u32
        const d = 0b00000001_00000001_00000001_00000001_00000001
        #assert @typeof(d) == u64
    }

    // A boolean is 'true' or 'false'
    // Note that as constants are known at compile time, we can use '#assert' to check the values
    {
        const a = true
        #assert a == true

        const b, c = false
        #assert b == false
        #assert c == false
    }

    // A float value has the usual C/C++ form
    {
        var a = 1.5
        @assert(a == 1.5)
        #assert @typeof(a) == f32

        var b = 0.11
        @assert(b == 0.11)

        var c = 15e2
        @assert(c == 15e2)

        var d = 15e+2
        @assert(d == 15e2)

        var e = -1E-1
        @assert(e == -0.1)
    }

    // By default, a floating point value is 'f32', not 'f64' (aka double) like in C++
    {
        var a = 1.5
        @assert(a == 1.5)
        #assert @typeof(a) == f32
        #assert @typeof(a) != f64
    }

    // You can postfix a number literal by a type, to force it
    {
        var a = 1.5'f64
        @assert(a == 1.5)
        @assert(a == 1.5'f64)
        #assert @typeof(a) == f64

        b := 10'u8
        @assert(b == 10)
        #assert @typeof(b) == u8

        c := 1'u32
        #assert @typeof(c) == u32
    }
}</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">string</div><code style="white-space: break-spaces;">#test
{
    // Strings are UTF8, and can be compared
    {
        const a = "this is a chinese character: 是"
        #assert a == "this is a chinese character: 是"

        const b = "this are some cyrillic characters: ӜИ"
        #assert b == "this are some cyrillic characters: ӜИ"
    }

    // A rune is an unicode codepoint, and is 32 bits
    {
        const a = "是"'rune
        #assert a == "是"'rune
        #assert @sizeof(a) == @sizeof(u32)
    }

    // You *cannot* index a string to get a rune, except in ascii strings. This is because i didn't want the
    // runtime to come with UTF8 decoding.
    // So in that case you will retrieve a byte.
    {
        const a = "this is a chinese character: 是"
        const b = a[0]
        #assert b == "t"'u8
        #assert @typeof(b) == @typeof(u8)

        // Here, the 'X' character in the middle does not have the index '1', because the chinese character before is
        // encoded in UTF8 with more than 1 byte
        const c = "是X是"
        #assert c[1] != "X"'u8 // False because the byte number 1 is not the character 'X'
    }

    // Multiple strings are compiled as one
    {
        const a = "this is "   "a"   " string"
        #assert a == "this is a string"
    }

    // You can concatenate some values if the values are known at compile time.
    // Use the '++' operator for that
    {
        const a = "the devil number is " ++ 666
        #assert a == "the devil number is 666"

        const b = 666
        var c = "the devil number is not " ++ (b + 1) ++ "!"
        @assert(c == "the devil number is not 667!")

        var d = "they are " ++ 4 ++ " apples in " ++ (2*2) ++ " baskets"
        @assert(d == "they are 4 apples in 4 baskets")
    }

    // A string and a rune can contain escape sequences for special characters
    // An escape sequence starts with '\'
    {
        const a = "this is code ascii 0x00:   \0"
        const b = "this is code ascii 0x07:   \a"
        const c = "this is code ascii 0x08:   \b"
        const d = "this is code ascii 0x09:   \t"
        const e = "this is code ascii 0x0A:   \n"
        const f = "this is code ascii 0x0B:   \v"
        const g = "this is code ascii 0x0C:   \f"
        const h = "this is code ascii 0x0D:   \r"
        const i = "this is code ascii 0x22:   \""
        const j = "this is code ascii 0x27:   \'"
        const k = "this is code ascii 0x5C:   \\"
    }

    // An escape sequence can be used to specify an ascii or an unicode value
    {
        const a = "\x26"        // 1 byte, hexadecimal, extended ascii
        const b = "\u2626"      // 2 bytes, hexadecimal, unicode 16 bits
        const c = "\U26262626"  // 4 bytes, hexadecimal, unicode 32 bits

        const d = "\u2F46"
        #assert d == "⽆"
    }

    // A raw string does not transform the escape sequences
    // A raw string starts and ends with the character '@'
    {
        const a = @"\u2F46"@
        #assert a != "⽆"
        #assert a == @"\u2F46"@
    }

    // A raw string can be declared on several lines because <eol> is now
    // part of the string
    {
        const a = @"this is
                    a
                    string
                    "@

        // Every blanks before the ending mark '"@' will be removed from every other lines,
        // so the string before is equivalent to :
        //
        // this is
        // a
        // string
    }

    // A multiline string starts and ends with """
    // Unlike raw strings, they still evaluate escape sequences
    {
        const a = """this is
                     a
                     string
                     """

        // Equivalent to :
        // this is
        // a
        // string
    }

    // In a multiline or raw string, if you end a line with '\', the
    // following eol will *NOT* be part of the string
    {
        const a = """\
                     this is
                     a
                     string
                     """
        // Equivalent to :
        // this is
        // a
        // string
    }

    // A single character can be casted to every unsigned int type
    {
        var a = "0"'u8
        @assert(a == 48)
        @assert(@typeof(a)== @typeof(u8))

        var b = "1"'u16
        @assert(b == 49)
        @assert(@typeof(b)== @typeof(u16))

        var c = "2"'u32
        @assert(c == 50)
        @assert(@typeof(c)== @typeof(u32))

        var d = "3"'u64
        @assert(d == 51)
        @assert(@typeof(d)== @typeof(u64))

        var e = "4"'rune
        @assert(e == 52)
        @assert(@typeof(e)== @typeof(rune))
    }

    // A string can be null if not defined
    {
        var a: string
        @assert(a == null)
        a = "null"
        @assert(a != null)
        a = null
        @assert(a == null)
    }

    // You can also use '@stringof' to return at compile time the result of a
    // constant expression as a string
    {
        const X = 1
        #assert @stringof(X) == "1"
        #assert @stringof(X + 10) == "11"
    }

    // You can also use '@nameof' to return the name of a variable, function etc.
    {
        const X = 1
        #assert @nameof(X) == "X"
    }
}</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">variables</div><code style="white-space: break-spaces;">#test
{
    // To declare a variable, use the 'var' keyword, followed by ':' and then the type
    {
        // Variable 'a' is of type 'u32' and its value is '1'
        var a: u32 = 1
        @assert(a == 1)

        var b: string = "string"
        @assert(b == "string")
    }

    // You can also declare multiple variables on the same line
    {
        var a, b: u32 = 123
        @assert(a == 123)
        @assert(b == 123)
    }

    // Or
    {
        var a: u32 = 12, b: f32 = 1.5
        @assert(a == 12)
        @assert(b == 1.5)
    }

    // If you don't assign a value, then the variable will be initialized with its default value (0)
    {
        var a: bool
        @assert(a == false)

        var b: string
        @assert(b == null)

        var c: f64
        @assert(c == 0)
    }

    // If you do not want the variable to be initialized, you can assign it with '?'
    {
        var a: bool = ?
        var b: string = ?
    }

    // Type is optional if it can be deduced from the assignment
    {
        var a = 1.5
        @assert(a == 1.5)
        #assert @typeof(a) == f32

        var b = "string"
        @assert(b == "string")
        #assert @typeof(b) == string

        var c = 1.5'f64
        @assert(c == 1.5)
        #assert @typeof(c) == f64
    }

    // Same for multiple variables
    {
        var a, b = true
        @assert(a == true)
        @assert(b == true)
        #assert @typeof(a) == @typeof(true)
        #assert @typeof(b) == @typeof(a)

        var c = 1.5, d = "string"
        @assert(c == 1.5)
        @assert(d == "string")
        #assert @typeof(c) == f32
        #assert @typeof(d) == string
    }

    // Even 'var' is optional if you use ':='
    {
        a := 1.5
        @assert(a == 1.5)
        #assert @typeof(a) == f32

        b := "string"
        @assert(b == "string")
        #assert @typeof(b) == string

        c := 1.5'f64
        @assert(c == 1.5)
        #assert @typeof(c) == f64
    }
}</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">const</div><code style="white-space: break-spaces;">#test
{
    // If you use 'const' instead of 'var', the value must be known by the compiler
    // There's no memory footprint if the type is a value or a string
    {
        const a = 666
        #assert a == 666

        const b: string = "string"
        #assert b == "string"
    }

    // Const can be more than just a simple type.
    // In that case, there's a memory footprint, because the constant is stored in the data segment.
    // That means that you can take the address of such a constant at runtime
    {
        const a: [3] s32 = @[0, 1, 2]   // static array of size 3, type s32

        ptr := &a[0] // Take the address of the first element
        @assert(ptr[0] == 0)
        @assert(ptr[2] == 2)

        // But as this is a constant, we can also test it at compile time
        #assert a[0] == 0
        #assert a[1] == 1
        #assert a[2] == 2
    }

    // A multidimensional array as a constant
    {
        const M4x4: [4, 4] f32 = @[
            @[1, 0, 0, 0],
            @[0, 1, 0, 0],
            @[0, 0, 1, 0],
            @[0, 0, 0, 1]
        ]
    }
}</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">cast</div><code style="white-space: break-spaces;">#test
{
    // use 'cast(type)' to cast from one type to another
    {
        x := 1.0
        #assert @typeof(x) == f32

        y := cast(s32) x
        #assert @typeof(y) == s32
        @assert(y == 1)
    }

    // use 'acast' to automatically cast to the left expression
    {
        var x: f32 = 1.0
        var y: s32 = acast x    // cast to 's32'
        #assert @typeof(y) == s32
        @assert(y == 1)
    }

    // use 'bitcast' to convert a native type to another without converting the value
    // Works only if the two types are of the same size
    {
        var x: f32 = 1.0
        var y: u32 = bitcast(u32) x
        @assert(y == 0x3f800000)
        #assert bitcast(u32) 1.0 == 0x3f800000
        #assert bitcast(f32) 0x3f800000 == 1.0
    }

    // An implicit cast is done if there's no loss of precision.
    // In that case, you can affect different types.
    {
        var x: s16 = 1's8   // 8 bits to 16 bits, fine
        var y: s32 = 1's16  // 16 bits to 32 bits, fine

        // But the following would generate an error as you cannot affect 's16' to 's8' without
        // an explicit cast(s16)

        //var z0: s16 = 1
        //var z1: s8 = z0
    }

    // To avoid overflow with operators + - * /, operations are promoted to 32 or 64 bits
    {
        @assert(@typeof(1's8 + 2's8)   == s32)
        @assert(@typeof(1's8 + 2's16)  == s32)
        @assert(@typeof(1'u8 + 2'u8)   == u32)
        @assert(@typeof(1'u8 + 2'u16)  == u32)
        @assert(@typeof(1'u8 + 2'f64)  == f64)
    }

    // But in that case, you can still affect to the original type.
    // The compiler will add an implicit cast, and a safety check
    {
        // Even if the resulting type of the '+' operator is 's32', you can still
        // affect to a 's8' value. Swag will panic at runtime if the conversion is invalid.
        var x: s8 = 1's8 + 2's8
    }
}</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">operators</div><code style="white-space: break-spaces;">#test
{
    // Basic binary operators
    {
        var x: s32 = 10

        x = x + 1
        x = x - 1
        x = x * 2
        x = x / 2
        x = x % 2 // Modulo
        x = x ^ 2 // Xor
        x = x & 0b0000_0001's32 // Bitmask and
        x = x | cast(s32) 0b0000_0001 // Bitmask or
        x = x << 1 // Shift bits left
        x = x >> 1 // Shift bits right
    }

    // Affect operators
    {
        var x: s32 = 10

        x += 1
        x -= 1
        x *= 2
        x /= 2
        x %= 2
        x ^= 2
        x |= 0b0000_0001's32
        x &= cast(s32) 0b0000_0001
        x <<= 1
        x >>= 1
    }

    // Unary operator
    {
        x := true
        x = !x      // Invert boolean
        @assert(x == false)

        y := 0b0000_0001'u8
        y = ~y      // Invert bits
        @assert(y == 0b1111_1110)

        z := 1
        z = -z      // Negative
        @assert(z == -1)
    }

    // Ternary operator
    {
        var x = true ? 1 : 666
        @assert(x == 1)
        var y = (x == 1) ? 1 : 666
        @assert(y == 1)
    }

    // Comparison operators
    {
        a := false

        a = 1 == 1 ? true : false
        a = 1 != 1 ? true : false
        a = 1 <= 1 ? true : false
        a = 1 >= 1 ? true : false
        a = 1 < 1 ? true : false
        a = 1 > 1 ? true : false
    }

    // Two ways comparison operator, which returns a s32 of -1, 0 or 1
    // A <=> B == -1 if A < B
    // A <=> B == 0  if A == B
    // A <=> B == 1  if A > B
    {
        a := -1.5 <=> 2.31
        #assert @typeof(a) == s32
        @assert(a == -1)

        @assert(-1 <=>  1 == -1)
        @assert( 1 <=> -1 == 1)
        @assert( 1 <=>  1 == 0)
    }

    {
        x1 := 10 <=> 20
        @assert(x1 == -1)
        x2 := 20 <=> 10
        @assert(x2 == 1)
        x3 := 20 <=> 20
        @assert(x3 == 0)
    }

    // Logical operator. This is not '&&' and '||' like in C/C++.
    {
        a := false
        a = (1 > 10) and (10 < 1)
        a = (1 > 10) or (10 < 1)
    }

    // Null conditional operator 'orelse'
    // Works with strings, pointers and structures with the 'opData' special function (we'll see later)
    {
        a := "string1"
        b := "string2"
        c := a orelse b     // c = a if a is not null, else c = b
        @assert(c == "string1")

        a = null
        c = a orelse b
        @assert(c == "string2")
    }

    {
        a := 0
        b := 1
        c := a orelse b
        @assert(c == b)
    }

    // Basic comparison operators
    {
        x := 5
        y := 10
        @assert(x == 5)
        @assert(x != 10)
        @assert(x <= 5)
        @assert(x < 10)
        @assert(x >= 5)
        @assert(x > 0)
    }
}</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">overflow</div><code style="white-space: break-spaces;">#test
{
    {
        // By default, swag will panic if some operators overflow (it depends on the build configuration)

        x := 255'u8
        // x += 1      // This would overflow, and panic, because we lose informations
        // x <<= 1     // Same here, we lose some bits
    }

    {
        // Operators that can overflow are :
        // + - * << >> and their equivalent += -= *= <<= >>=
        //
        // If you know what your are doing, you can use a special version of those operators, which will not panic.
        // Add the ',safe' modifier after the operation. This will disable safety checks.

        x := 255'u8
        x +=,safe 1     // wrap around
        @assert(x == 0)

        y := 255'u8
        y >>=,safe 1
        @assert(y == 127)
    }

    // Note that you can remove safety checks with a special Swag 'attribute'
    // This can be defined for a file, a function, or just a piece of code.
    #[Swag.Safety("overflow", false)]
    {
        x := 255'u8
        x += 255    // 254
        x += 1      // 255
        x >>= 1     // 127
        @assert(x == 127)
    }

    // Note that safety checks can be changed for a specific build configuration (--cfg:configuration).
    // Swag comes with four predefined configurations : 'debug', 'fast-debug', 'fast-compile' and 'release'
    // Safety checks are disabled in 'release'.
}</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">array</div><code style="white-space: break-spaces;">// Remember that dynamic arrays are part of the 'Std.Core' module.
// Here we are only talking about native static arrays.

#test
{
    // A static array is declared with [] followed by the type
    {
        var array: [2] s32  // Static array of two s32
        array[0] = 1
        array[1] = 2
    }

    // You can get the number of elements of an array with '@countof'
    {
        var array: [2] s32
        #assert @countof(array) == 2
        #assert @sizeof(array) == 2 * @sizeof(s32)
    }

    // You can get the address of the array with '@dataof'
    {
        var array: [2] s32
        ptr0 := @dataof(array)
        ptr0[0] = 1

        // This is equivalent of taking the address of the first element
        ptr1 := &array[0]
        ptr1[1] = 2

        @assert(array[0] == 1)
        @assert(array[1] == 2)
    }

    // An array literal has the form @[...]
    {
        arr := @[1, 2, 3, 4]
        #assert @countof(arr) == 4
        #assert @typeof(arr) == [4] s32
    }

    // The size of the array can be deduced from the initialisation expression
    {
        var array: [] s32 = @[1, 2]
        @assert(array[0] == 1)
        @assert(array[1] == 2)
        #assert @countof(array) == 2

        array1 := @["10", "20", "30"]
        @assert(array1[0] == "10")
        @assert(array1[1] == "20")
        @assert(array1[2] == "30")
        #assert @countof(array1) == 3
    }

    // Like every other types, an array is initialized by default to 0
    {
        var array: [2] s32
        @assert(array[0] == 0)
        @assert(array[1] == 0)
    }

    // But for speed, you can force the array to be not initialized with '?'
    {
        var array: [100] s32 = ?
    }

    // An array can have multiple dimensions
    // Syntax is *not* [X][Y] like in C/C++, but is [X, Y]
    {
        var array: [2, 2] s32
        array[0, 0] = 1
        array[0, 1] = 2
        array[1, 0] = 3
        array[1, 1] = 4
    }

    // The sizes can be deduced from the initialization expression too
    {
        array  := @[1, 2, 3, 4]
        array1 := @[[1, 2], [3, 4]]

        #assert @countof(array) == 4
        #assert @countof(array) == 4
    }

    // A static array (with compile time values) can be stored as a constant
    {
        const array = @[1, 2, 3, 4]
        #assert array[0] == 1   // Dereference is done at compile time
        #assert array[3] == 4
    }

    // If the type of the array is not specified, the type of the first literal value will be used
    // for all other members
    {
        arr := @[1'f64, 2, 3, 4]    // Every values are forced to be 'f64'

        #assert @countof(arr) == 4
        #assert @typeof(arr) == [4] f64
        @assert(arr[3] == 4.0)
    }
}</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">slice</div><code style="white-space: break-spaces;">#test
{
    // A slice is a pointer on a buffer of datas, and a 'uint' to count the number of elements
    // It is declared with [..]
    {
        var a: [..] bool
        #assert @sizeof(a) == @sizeof(*void) + @sizeof(uint)
    }

    // You can initialize a slice like an array
    {
        var a: const [..] u32 = @[10, 20, 30, 40, 50]
        @assert(@countof(a) == 5)
        @assert(a[0] == 10)
        @assert(a[4] == 50)

        // But as this is a slice and not a static array, you can change the value at runtime
        a = @[1, 2]
        @assert(@countof(a) == 2)
        @assert(a[0] == 1)
        @assert(a[1] == 2)
    }

    // At runtime, '@dataof' will return the address of the values, '@countof' the number of elements
    {
        var a: const [..] u32 = @[10, 20, 30, 40, 50]
        count := @countof(a)
        addr  := @dataof(a)
        @assert(count == 5)
        @assert(addr[0] == 10)
        @assert(addr[4] == 50)

        a = @[1, 2]
        @assert(@countof(a) == 2)
    }

    // You can create a slice with your own pointer and count using '@mkslice'
    {
        var array: [4]  u32 = @[10, 20, 30, 40]

        // Creates a slice of 'array' (a view, or subpart) starting at index 2, with 2 elements
        var slice: [..] u32 = @mkslice(&array[0] + 2, 2)

        @assert(@countof(slice) == 2)
        @assert(slice[0] == 30)
        @assert(slice[1] == 40)

        @assert(array[2] == 30)
        slice[0] = 314
        @assert(array[2] == 314)
    }

    // For a string, the slice must be 'const' because a string is immutable
    {
        var str = "string"
        var strSlice: const [..] u8 = @mkslice(@dataof(str), 2)
        @assert(strSlice[0] == "s"'u8)
        @assert(strSlice[1] == "t"'u8)
    }

    // Instead of '@mkslice', you can slice something with the '..' operator
    // For example you can slice a string.
    {
        var str = "string"
        var slice = str[1..3]
        @assert(slice == "tri")
    }

    // You can slice an array
    {
        arr := @[10, 20, 30, 40]
        slice := arr[2..3]
        @assert(slice[0] == 30)
        @assert(slice[1] == 40)
        @assert(@countof(slice) == 2)
    }

    // You can slice a pointer
    {
        arr := @[10, 20, 30, 40]
        ptr := &arr[2]
        slice := ptr[0..1]
        @assert(slice[0] == 30)
        @assert(slice[1] == 40)
        @assert(@countof(slice) == 2)
    }

    // You can slice another slice
    {
        arr := @[10, 20, 30, 40]
        slice1 := arr[1..3]
        @assert(slice1[0] == 20)
        @assert(slice1[1] == 30)
        @assert(slice1[2] == 40)

        slice2 := slice1[1..2]
        @assert(slice2[0] == 30)
        @assert(slice2[1] == 40)
    }
}</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">pointer</div><code style="white-space: break-spaces;">#test
{
    // A pointer is declared with '*'
    {
        var ptr1: *u8
        var ptr2: **u8
    }

    // A pointer can be null (i know some of you will collapse here)
    {
        var ptr1: *u8
        @assert(ptr1 == null)
    }

    // You can take the address of something with '&'
    {
        arr := @[1, 2, 3, 4]
        ptr := &arr[0]
        @assert(ptr[0] == 1)
        @assert(@dataof(arr) == ptr)
    }

    // Pointers can be 'const'
    {
        str := "string"
        var ptr: const *u8 = @dataof(str)
        @assert(dref ptr == "s"'u8)
    }

    // You can be weird, but is this necessary ?
    {
        var ptr:  *const *u8        // Normal pointer to a const pointer
        var ptr1: const *const *u8  // Const pointer to a const pointer
        var ptr2: const **u8        // Const pointer to a normal pointer
    }

    // If you want to enable pointer arithmetic, declare your pointer
    // with ^ instead of *
    {
        var ptr: ^u8
        ptr = ptr - 1
    }

    // Taking the address of an array element enables pointer arithmetic
    {
        var x: [4] s32
        ptr := &x[1]
        ptr = ptr - 1
    }

    // You can dereference a pointer by index if pointer arithmetic is possible, or with 'dref'
    {
        v    := 1
        ptr := &v
        @assert(@typeof(ptr) == *s32)
        @assert(dref ptr == 1)
    }

    {
        arr := @[1, 2, 3, 4]
        ptr := &arr[0]
        @assert(@typeof(ptr) == ^s32)

        // The type of 'ptr' is ^s32, so it can be dereferenced by index
        value1 := ptr[0]
        @assert(value1 == 1)
        #assert @typeof(value1) == s32

        // 'deref' still works for the first element
        value := dref ptr
        @assert(value == 1)
        #assert @typeof(value) == s32
    }
}</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">any</div><code style="white-space: break-spaces;">#test
{
    // 'any' is a specific type that can store every other types
    {
        var a: any

        // Store a s32
        a = 6
        @assert(a == 6)

        // Then store a string instead
        a = "string"
        @assert(a == "string")

        // Then store a bool instead
        a = true
        @assert(a == true)
    }

    // 'any' is in fact a pointer to the value, and a 'typeinfo'
    // '@dataof' can be used to retrieve the pointer to the value.
    {
        var a: any = 6
        ptr := cast(const *s32) @dataof(a)
        @assert(dref ptr == 6)
    }

    // '@typeof' will give you the type 'any', but '@kindof' will give you the real underlying type
    // In that case, '@kindof' is evaluted at runtime.
    {
        var a: any = "string"
        #assert @typeof(a) == any
        @assert(@kindof(a) == string)

        a = true
        @assert(@kindof(a) == bool)
    }

    // You can declare an array with multiple types, with 'any'
    {
        var array: [] any = @[true, 2, 3.0, "4"]
        @assert(@kindof(array[0]) == bool)
        @assert(@kindof(array[1]) == s32)
        @assert(@kindof(array[2]) == f32)
        @assert(@kindof(array[3]) == string)

        @assert(array[0] == true)
        @assert(array[1] == 2)
        @assert(array[2] == 3.0)
        @assert(array[3] == "4")
    }

    // An any can be set to null, and tested against null
    {
        var x: any
        @assert(x == null)
        x = 6
        @assert(x != null)
        @assert(x == 6)
        x = "string"
        @assert(x != null)
        @assert(x == "string")
        x = null
        @assert(x == null)
    }
}</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">tuple</div><code style="white-space: break-spaces;">#test
{
    // A tuple is an anonymous structure, aka a struct literal
    // Syntax is @{}
    {
        tuple1 := @{2, 2}
        tuple2 := @{"string", 2, true}
    }

    // Tuple values have default names to access them, in the form of 'itemX' where 'X' is the field rank
    {
        tuple := @{"string", 2, true}
        @assert(tuple.item0 == "string")
        @assert(tuple.item1 == 2)
        @assert(tuple.item2 == true)
    }

    // But you can specify your own names
    {
        tuple := @{x: 1.0, y: 2.0}
        @assert(tuple.x == 1.0)
        @assert(tuple.item0 == 1.0)
        @assert(tuple.y == 2.0)
        @assert(tuple.item1 == 2.0)
    }

    // A tuple is an expression, where names of fields can be omitted.
    // But the automatic name 'itemX' is always valid
    {
        var tuple: {x: f32, f32}
        tuple.x = 1.0
        @assert(tuple.x == 1.0)
        @assert(tuple.item0 == 1.0)
        @assert(tuple.item1 == 0.0)
    }

    // You can unpack a tuple field by field
    {
        tuple1 := @{x: 1.0, y: 2.0}
        var (value0, value1) = tuple1
        @assert(value0 == 1.0)
        @assert(value1 == 2.0)

        tuple2 := @{"name", true}
        (name, value) := tuple2
        @assert(name == "name")
        @assert(value == true)
    }

    // You can ignore a tuple field by naming the variable '?'
    {
        tuple1 := @{x: 1.0, y: 2.0}
        (x, ?) := tuple1
        @assert(x == 1.0)
        (?, y) := tuple1
        @assert(y == 2.0)
    }

    // When creating a tuple literal with variables, the tuple
    // fields will take the name of the variables (except if specified)
    {
        x := 555
        y := 666
        t := @{x, y}
        @assert(t.x == 555)
        @assert(t.item0 == t.x)
        @assert(t.y == 666)
        @assert(t.item1 == t.y)
    }

    // Even if two tuples do not have the same field names, they can be assigned
    // to each other if the field types are the same
    {
        var x: {a: s32, b: s32}
        var y: {c: s32, d: s32}

        y = @{1, 2}
        x = y
        @assert(x.a == 1)
        @assert(x.b == 2)

        // But note that 'x' and 'y' to not have the same type
        #assert @typeof(x) != @typeof(y)
    }
}
</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">enum</div><code style="white-space: break-spaces;">#test
{
    // Enums values, unlike C/C++, must end with ';' or an <eol> (like a normal Swag declaration)
    {
        enum Values
        {
            A
            B
        }
    }

    // By default, an enum is of type 's32'
    {
        enum Values { A; B; }
        type := @typeof(Values)

        // 'type' here is a 'typeinfo' dedicated to the enum type.
        // In that case, 'type.rawType' is the enum underlying type.
        @assert(type.rawType == s32)
        #assert @typeof(Values) == Values
    }

    // '@kindof' will return the underlying type of the enum
    {
        enum RGB { R; G; B; }
        #assert @typeof(RGB) == RGB
        #assert @kindof(RGB) != RGB
        #assert @kindof(RGB) == s32
    }

    // You can specify your own type after the enum name
    {
        enum Values1: s64
        {
            A
            B
            C
        }

        #assert @kindof(Values1) == s64
        #assert @typeof(Values1.A) == Values1
    }

    // Enum values, if not specified, start at 0 and are increased by 1 at each new value
    {
        enum Value: s64
        {
            A
            B
            C
        }

        #assert Value.A == 0
        #assert Value.B == 1
        #assert Value.C == 2
    }

    // You can specify your own values
    {
        enum Value: s64
        {
            A = 10
            B = 20
            C = 30
        }

        #assert Value.A == 10
        #assert Value.B == 20
        #assert Value.C == 30
    }

    // If you omit one value, it will be assigned to the previous value + 1
    {
        enum Value: u32
        {
            A = 10
            B // 11
            C // 12
        }

        #assert Value.A == 10
        #assert Value.B == 11
        #assert Value.C == 12
    }

    // For non integer types, you *must* specify the values as they cannot be deduced
    {
        enum Value1: string
        {
            A = "string 1"
            B = "string 2"
            C = "string 3"
        }

        #assert Value1.A == "string 1"
        #assert Value1.B == "string 2"
        #assert Value1.C == "string 3"

        enum Value2: f32
        {
            A = 1.0
            B = 3.14
            C = 6
        }

        #assert Value2.A == 1.0
        #assert Value2.B == 3.14
        #assert Value2.C == 6
    }

    // '@countof' returns the number of values of an enum
    {
        enum Value: string
        {
            A = "string 1"
            B = "string 2"
            C = "string 3"
        }

        @assert(@countof(Value) == 3)
        #assert @countof(Value) == 3
    }

    // You can use the keyword 'using' for an enum
    {
        enum Value
        {
            A
            B
            C
        }

        using Value

        // No need to say 'Value.A' thanks to the 'using' above
        @assert(A == 0)
        @assert(B == 1)
        @assert(C == 2)

        // But the normal form is still possible
        @assert(Value.A == 0)
        @assert(Value.B == 1)
        @assert(Value.C == 2)
    }

    // Enum can be a set of flags with the #[Swag.EnumFlags] attribute
    // Type should be u8, u16, u32, u64 or uint
    {
        #[Swag.EnumFlags]
        enum MyFlags: u8
        {
            A   // First value is 1 and *not* 0
            B   // Value is 2
            C   // Value is 4
            D   // Value is 8
        }

        #assert MyFlags.A == 0b00000001
        #assert MyFlags.B == 0b00000010
        #assert MyFlags.C == 0b00000100
        #assert MyFlags.D == 0b00001000

        value := MyFlags.B | MyFlags.C
        @assert(value == 0b00000110)
        @assert(value & MyFlags.B == MyFlags.B)
        @assert(value & MyFlags.C == MyFlags.C)
    }

    // You can have an enum of static arrays
    {
        enum Value: const [2] s32
        {
            A = @[1, 2]
            B = @[10, 20]
        }

        #assert Value.A[0] == 1
        #assert Value.A[1] == 2
        #assert Value.B[0] == 10
        #assert Value.B[1] == 20
    }

    // You can have an enum of const slices
    {
        enum Value: const [..] s32
        {
            A = @[1, 2]
            B = @[10, 20, 30, 40]
        }

        #assert @countof(Value.A) == 2
        #assert @countof(Value.B) == 4

        x := Value.A
        @assert(x[0] == 1)
        @assert(x[1] == 2)
        y := Value.B
        @assert(y[0] == 10)
        @assert(y[1] == 20)
    }

    // The type of the enum is not necessary in the assignement expression when declaring a variable
    {
        enum Values { A; B; }

        // The normal form
        var x: Values = Values.A

        // But in fact 'Values' is not necessary because it can be deduced
        var y: Values = A

        @assert(x == y)
    }

    // The enum type is not necessary in a case expression of a switch block (it is deduced from the switch expression)
    {
        enum Values { A; B; }
        x := Values.A

        // The 'normal' form
        switch x
        {
        case Values.A: break
        case Values.B: break
        }

        // The 'simplified' form
        switch x
        {
        case A: break
        case B: break
        }
    }

    // In an expression, and if the enum name can be deduced, you can omit it and use the '.Value' syntax
    {
        enum Values { A; B; }

        // The normal form
        x := Values.A

        // The simplified form, because 'Values' can be deduced from type of x
        @assert(x == .A)
        @assert(x != .B)
    }

    // Works also for flags
    {
        #[Swag.EnumFlags]
        enum Values { A; B; }

        x := Values.A | Values.B
        @assert((x & .A) and (x & .B))
    }

    // Works also (most of the time), for functions
    {
        enum Values { A; B; }
        func toto(v1, v2: Values) {}
        toto(.A, .B)
    }

    // By type reflection, you can loop/visit all values of an enum
    {
        enum RGB { R; G; B; }

        cpt := 0
        loop idx: RGB
            cpt += 1
        @assert(cpt == 3)

        visit val: RGB
        {
            switch val
            {
            case R:     break
            case G:     break
            case B:     break
            default:    @assert(false)
            }
        }
    }
}</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">impl</div><code style="white-space: break-spaces;">// 'impl' can be used to declare some stuff in the scope of an enum
// The keyword 'self' represents the enum value

enum RGB { R; G; B; }

// Note the 'impl enum' syntax. We'll see later that 'impl' is also used for structs.
impl enum RGB
{
    func isRed(self)       => self == R
    func isRedOrBlue(self) => self == R or self == B
}

#test
{
    @assert(RGB.isRed(RGB.R))
    @assert(RGB.isRedOrBlue(RGB.B))

    using RGB
    @assert(isRedOrBlue(R))
    @assert(isRedOrBlue(B))

    // A first look at 'ufcs' (uniform function call syntax)
    @assert(R.isRedOrBlue())
    @assert(!RGB.G.isRedOrBlue())
}</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">namespace</div><code style="white-space: break-spaces;">// You can create a global scope with a namespace
// All symbols inside the namespace will be in the corresponding global scope.
namespace A
{
    func a() => 1
}

// You can also specify more than one name
// Here 'C' will be a namespace inside 'B' which is itself inside 'A'
namespace A.B.C
{
    func a() => 2
}

#test
{
    @assert(A.a() == 1)
    @assert(A.B.C.a() == 2)
}

// You can also put 'using' in front of the namespace to be able to access
// the content without scoping in the *current* file
using namespace Private
{
    const FileSymbol = 0
}

const B = Private.FileSymbol
const C = FileSymbol // No need to specify 'Private' because of the 'using'

// This is equivalent to 'scopefile', but you don't have to specify a name,
// the compiler will generate it for you

scopefile {
    const OtherSymbol = 0
}

const D = OtherSymbol

// All symbols from a Swag source file are exported to other files of the same module.
// So using 'scopefile' can protect from name conflicts.
</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">if</div><code style="white-space: break-spaces;">#test
{
    // A basic test with 'if'. Curlies are optional.
    // The expression doesn't need to be enclosed with () unlike C/C++
    a := 0
    if a == 1
        @assert(false)
    if (a == 1)
        @assert(false)

    if a == 0 {
        @assert(true)
    }

    // 'else' is of course also a thing
    if a == 0
        a += 1
    else
        a += 2
    @assert(a == 1)

    // 'elif' is equivalent to 'else if'
    if a == 1
        a += 1
    else if a == 2
        @assert(false)
    elif a == 3
        @assert(false)
    elif a == 4
        @assert(false)

    // Boolean expression with 'and' and 'or'
    if a == 0 and a == 1
        @assert(false)
    if a == 0 or a == 1
        @assert(true)
}</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">loop</div><code style="white-space: break-spaces;">#test
{
    // loop are used to iterate a given amount of time
    // The loop expression value is evaluated *once*.
    // The loop expression must be a positive value.
    {
        cpt := 0
        loop 10 // Loops 10 times
            cpt += 1
        @assert(cpt == 10)
    }

    // The intrinsic '@index' returns the current index of the loop (starting at 0)
    {
        cpt := 0'uint
        loop 5
        {
            cpt += @index
        }

        @assert(cpt == 0+1+2+3+4)
    }

    // You can name that index if you want
    {
        cpt  := 0
        cpt1 := 0

        loop i: 5   // index is named 'i'
        {
            cpt  += i
            cpt1 += @index  // @index is always available, even when named
        }

        @assert(cpt  == 0+1+2+3+4)
        @assert(cpt1 == cpt)
    }

    // loop can be used on every types that accept the '@countof' intrinsic
    {
        arr := @[10, 20, 30, 40]
        #assert @countof(arr) == 4

        cpt := 0
        loop arr    // The array contains 4 elements, so the loop count is 4
            cpt += arr[@index]
        @assert(cpt == 10+20+30+40)
    }

    // Warning !
    // On a string, it will loop for each byte, *not* runes (if a rune is encoded in more than one byte)
    // If you want to iterate on all runes, you will have to use the Std.Core module.
    {
        cpt := 0
        loop "⻘"
            cpt += 1

        // cpt is equal to 3 because '⻘' is endoded with 3 bytes
        @assert(cpt == 3)
    }

    // You can exit a loop with 'break'
    {
        cpt := 0
        loop x: 10
        {
            if x == 5
                break
            cpt += 1
        }

        @assert(cpt == 5)
    }

    // You can force to return to the loop evaluation with 'continue'
    {
        cpt := 0
        loop x: 10
        {
            if x == 5
                continue // Do not count 5
            cpt += 1
        }

        @assert(cpt == 9)
    }

    // Loop can also be used to iterate on a range of signed values
    // Syntax is <lower bound>..<upper bound>
    {
        count := 0
        sum   := 0
        loop i: -1..1   // loop from -1 to 1, all included
        {
            count += 1
            sum += i
        }

        @assert(sum == 0)
        @assert(count == 3)
    }

    // With a range, you can loop in reverse order
    {
        // Loop from 5 to 0
        loop 5..0
        {
        }

        // Loop from 1 to -1
        loop 1..-1
        {
        }
    }

    // You can exclude the last value with the ..< syntax
    {
        // Will loop from 1 to 2 and *not* 1 to 3
        cpt := 0
        loop i: 1..<3
        {
            cpt += i
        }

        @assert(cpt == 1+2)
    }

    // A loop without an expression but with a block is infinite.
    // This is equivalent to while true {}
    // @index is still valid in that case (but cannot be renamed)
    loop
    {
        if @index == 4
            break
    }
}
</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">visit</div><code style="white-space: break-spaces;">#test
{
    // 'visit' is used to visit all the elements of a collection
    {
        visit value: "ABC"
        {
            a := @index
            switch a
            {
            case 0: @assert(value == "A"'u8)
            case 1: @assert(value == "B"'u8)
            case 2: @assert(value == "C"'u8)
            }
        }
    }

    // You can name both the value and the loop index, in that order
    {
        visit value, index: "ABC"
        {
            a := index
            switch a
            {
            case 0: @assert(value == "A"'u8)
            case 1: @assert(value == "B"'u8)
            case 2: @assert(value == "C"'u8)
            }
        }
    }

    // Both names are optional. In that case, you can use '@alias0' and '@alias1'
    // '@alias0' for the value, and '@alias1' for the index
    {
        visit "ABC"
        {
            a := @alias1    // This is the index
            @assert(a == @index)
            switch a
            {
            case 0: @assert(@alias0 == "A"'u8)
            case 1: @assert(@alias0 == "B"'u8)
            case 2: @assert(@alias0 == "C"'u8)
            }
        }
    }

    // You can visit arrays or slices
    {
        array := @[10, 20, 30]

        result := 0
        visit it: array
            result += it

        @assert(result == 10+20+30)
    }

    // Works also for multi dimensional arrays
    {
        var array: [2, 2] s32 = @[[10, 20], [30, 40]]

        result := 0
        visit it: array
            result += it

        @assert(result == 10+20+30+40)
    }

    // You can visit with a pointer to the value, and not the value itself, by
    // adding '&' before the value name
    {
        var array: [2, 2] s32 = @[[1, 2], [3, 4]]

        result := 0
        visit &it: array
        {
            result += dref it
            dref it = 555
        }

        @assert(result == 1+2+3+4)

        @assert(array[0, 0] == 555)
        @assert(array[0, 1] == 555)
        @assert(array[1, 0] == 555)
        @assert(array[1, 1] == 555)
    }
}
</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">for</div><code style="white-space: break-spaces;">#test
{
    // 'for' accepts a start statement, an expression to test, and an ending statement
    // This is the same as the C/C++ 'for'
    {
        cpt := 0
        for i := 0; i < 10; i += 1
            cpt += 1
        @assert(cpt == 10)

        // ';' can be replaced by a newline (like always)
        for i := 0
            i < 10
            i += 1
        {
            cpt += 1
        }
        @assert(cpt == 20)

        // Instead of one single expression, you can use a statement
        for { i := 0; cpt = 0; }
            i < 10
            i += 1
        {
            cpt += 1
        }
        @assert(cpt == 10)

        for { i := 0; cpt = 0; }
            i < 10
            { i += 2; i -= 1; }
        {
            cpt += 1
        }
        @assert(cpt == 10)
    }

    // Like 'loop' and 'while', you have access to '@index', the current loop index
    {
        cpt := 0'uint
        for var i: u32 = 10; i < 15; i += 1
            cpt += @index
        @assert(cpt == 0+1+2+3+4)

        cpt1 := 0'uint
        for i := 10; i < 15; i += 1
            cpt1 += @index
        @assert(cpt1 == 0+1+2+3+4)
    }
}</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">while</div><code style="white-space: break-spaces;">#test
{
    // While is a loop that runs until the expression is false
    {
        i := 0
        while i < 10
            i += 1
        @assert(i == 10)
    }

    // You can also break and continue inside a while
    {
        i := 0
        while i < 10
        {
            if i == 5
                break
            i += 1
        }

        @assert(i == 5)
    }
}
</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">switch</div><code style="white-space: break-spaces;">#test
{
    // 'switch' works like in C/C++, except that no 'break' is necessary (except if the 'case' is empty)
    // That means that there's no automatic 'fallthrough' from one case to another
    {
        value := 6
        switch value
        {
        case 0:  @assert(false)
        case 5:  @assert(false)
        case 6:  @assert(true)
        default: @assert(false)
        }

        ch := "A"'rune
        switch ch
        {
        case "B"'rune: @assert(false)
        case "A"'rune: break
        }
    }

    // You can put multiple values on the same 'case'
    {
        value := 6
        switch value
        {
        case 2, 4, 6:   @assert(true)
        default:        @assert(false)
        }

        switch value
        {
        case 2,
             4,
             6:
            @assert(true)
        default:
            @assert(false)
        }
    }

    // switch works with every types that accept the '==' operator
    // So you can switch on strings for example
    {
        value := "myString"
        switch value
        {
        case "myString":    @assert(true)
        case "otherString": @assert(false)
        default:            @assert(false)
        }
    }

    // If you want to pass from one case to another like in C/C++, use 'fallthrough'
    {
        value := 6
        switch value
        {
        case 6:
            fallthrough
        case 7:
            @assert(value == 6)
        default:
            @assert(true)
        }
    }

    // 'break' can be used to exit the current 'case' statement
    {
        value := 6
        switch value
        {
        case 6:
            if value == 6
                break
            @assert(false)
        default:
            @assert(false)
        }
    }

    // A 'case' statement cannot be empty. Use 'break' if you want to do nothing
    {
        value := 6
        switch value
        {
        case 5:     @assert(false)
        case 6:     break
        default:    @assert(false)
        }
    }

    // switch can be marked with 'Swag.Complete' to force all the cases to be covered.
    // If one or more values are missing, an error will be raised by the compiler
    {
        enum Color { Red; Green; Blue; }
        color := Color.Red

        #[Swag.Complete]
        switch color
        {
        case Color.Red:     break
        case Color.Green:   @assert(false)
        case Color.Blue:    @assert(false)
        }
    }

    // If the switch expression is omitted, then it will behave like a serie of if/else,
    // resolved in order
    {
        value  := 6
        value1 := "true"

        switch
        {
        case value == 6 or value == 7:
            @assert(true)
            fallthrough
        case value1 == "true":
            @assert(true)
        default:
            @assert(false)
        }
    }

    // When used on an 'any' variable, switch is done on the underlying variable type
    {
        var x: any = "value"

        switch x
        {
        case string: break
        default:     @assert(false)
        }
    }

    // A switch can also be used with a (constant) range of values
    {
        success := false
        x := 6
        switch x
        {
        case 0..5:  @assert(false)
        case 6..15: success = true
        }

        @assert(success)
    }

    // If they overlap, the first valid range will be used
    {
        success := false
        x := 6
        switch x
        {
        case 0..10:  success = true
        case 5..<15: @assert(false)
        }

        @assert(success)
    }

    // A case expression doesn't need to be constant, except if the switch
    // is marked with 'Swag.Complete'
    {
        test := 2
        a := 0
        b := 1
        switch test
        {
        case a:     // Test with a variable
            @assert(false)
        case b:     // Test with a variable
            @assert(false)
        case b + 1: // Test with an expression
            @assert(true)
        }
    }
}</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">break</div><code style="white-space: break-spaces;">#test
{
    // 'break' is used to exit a loop, while, for, switch
    {
        loop 10
            break
        for i := 0; i < 10; i += 1
            break
        while true
            break
    }

    // By default, 'break' will exit the parent scope only
    {
        cpt := 0
        loop 10
        {
            loop 10
            {
                break   // Exit the inner loop...
            }

            // ...and continue execution here
            cpt += 1
        }

        @assert(cpt == 10)
    }

    // But you can name a scope with the 'scope' keyword, and exit to the end of it with a 'break'
    {
        cpt := 0
        scope BigScope
        {
            loop 10
            {
                cpt += 1
                break BigScope  // Break to the outer scope...
            }

            @assert(false)
        }

        // ...and continue execution here
        @assert(cpt == 1)
    }

    // When used with a scope, a continue is a way to go back
    // to the start of the scope
    {
        cpt := 0
        scope Loop
        {
            cpt += 1
            if cpt == 5
                break
            continue    // Loop
        }

        @assert(cpt == 5)
    }

    // You are not obliged to name the scope, so this can also be used (for example)
    // as an alternative of a bunch of if/else
    {
        cpt := 0
        scope
        {
            if cpt == 1
            {
                @assert(cpt == 1)
                break
            }

            if cpt == 2
            {
                @assert(cpt == 2)
                break
            }

            if cpt == 3
            {
                @assert(cpt == 3)
                break
            }
        }
    }

    // Note that a scope can be followed by a simple statement, not always a block
    {
        scope Up loop 10
        {
            loop 10
            {
                if @index == 5
                    break Up
            }

            @assert(false)
        }
    }
}</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">struct</div><code style="white-space: break-spaces;">#test
{
    // This is a struct declaration. 'var' is not necessary for the fields.
    {
        struct MyStruct
        {
            name: string
        }

        struct MyStruct1
        {
            x:      s32
            y, z:   s32
            val:    bool
            myS:    MyStruct
        }
    }

    // A struct can be anonymous when declared as a variable type
    // Unlike tuples, syntax should be the same as for named structs
    {
        var tuple: struct
        {
            x: f32
            y: f32
        }
        var tuple1: struct{x, y: f32; }

        tuple.x = 1.0
        tuple.y = 2.0
        @assert(tuple.x == 1.0)
        @assert(tuple.y == 2.0)
    }

    {
        struct MyStruct
        {
            rgb: struct {x, y, z: f32; }
            hsl: struct {h, s, l: f32; }
        }
    }

    // You can initialize a struct variable in different ways
    {
        struct MyStruct
        {
            x, y: s32 = 1
        }

        // Default values
        var v0: MyStruct
        @assert(v0.x == 1)
        @assert(v0.y == 1)

        // Init in the order of the fields
        var v1: MyStruct{10, 20}
        @assert(v1.x == 10)
        @assert(v1.y == 20)

        // Named field
        v2 := MyStruct{y: 20}
        @assert(v2.x == 1)
        @assert(v2.y == 20)

        // With a tuple
        var v3: MyStruct = @{10, 20}
        @assert(v3.x == 10)
        @assert(v3.y == 20)
    }

    // The fields of a struct can be initialized at the declaration
    {
        struct MyStruct
        {
            x: s32    = 666
            y: string = "454"
        }

        v := MyStruct{}
        @assert(v.x == 666)
        @assert(v.y == "454")
    }

    // A struct can be affected to a constant, as long as it can be evaluated at compile time
    {
        struct MyStruct
        {
            x: s32    = 666
            y: string = "454"
        }

        const X: MyStruct{50, "value"}
        #assert X.x == 50
        #assert X.y == "value"
    }
}

// A function can take an argument of type struct.
// No copy is done (this is equivalent to a const ref in C++)
struct Struct3
{
    x, y, z: s32 = 666
}

func toto(v: Struct3)
{
    @assert(v.x == 5)
    @assert(v.y == 5)
    @assert(v.z == 666)
}

func titi(v: Struct3)
{
    @assert(v.y == 666)
}

#test
{
    // Call with a struct literal
    toto(Struct3{5, 5, 666})

    // Type can be deduced from the argument
    toto(@{5, 5, 666})

    // You can also just specify some parts of the struct, in the declaration order of the fields
    titi(@{5})      // Initialize x, which is the first field
    titi(@{5, 666}) // Initialize x and y

    // You can also name the fields, and omit some of them
    titi(@{x: 5, z: 5}) // Here y will stay to the default value, which is 666
}</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">impl</div><code style="white-space: break-spaces;">// Like for an enum, 'impl' is used to declare some stuff in the scope of a struct

#[Swag.ExportType("methods")]   // See later, used to export 'methods' in type reflection
struct MyStruct
{
    x: s32 = 5
    y: s32 = 10
    z: s32 = 20
}

impl MyStruct
{
    const MyConst = true
    func returnTrue() => true
}

#test
{
    @assert(MyStruct.returnTrue())
    @assert(MyStruct.MyConst)
}

// You can have multiple 'impl' blocks.
// The difference with a namespace is that 'self' and 'Self' are defined
// inside an impl block. They refere to the corresponding type

impl MyStruct
{
    // 'self' is an alias for 'var self: Self'
    func returnX(using self) => x
    func returnY(self)       => self.y
    // 'Self' is the corresponding type, in that case 'MyStruct'
    func returnZ(me: Self)   => me.z
}

// If you declare your function with 'mtd' (method) instead of 'func', then
// the first parameter is forced to be 'using self'.
// If you declare your function with 'mtdc' (method const) instead of 'func', then
// the first parameter is forced to be 'const using self'.
//
// Other than that, it's exactly the same.
// So this is just syntaxic sugar to avoid repeating the 'using self'.

impl MyStruct
{
    mtd  methodReturnX()          => x
    func funcReturnX(using self)  => x
}

#test
{
    var c: MyStruct
    @assert(c.returnX() == 5)
    @assert(c.methodReturnX() == 5)
    @assert(c.funcReturnX() == 5)
    @assert(c.returnY() == 10)
    @assert(c.returnZ() == 20)
}

// All functions in an impl block can be retrieved by reflection, as long as the struct
// is declared with #[Swag.ExportType("methods")] (by default, methods are not exported)
#test
{
    // Creates a type alias named 'Lambda'
    alias Lambda = func(MyStruct)->s32

    var fnX: Lambda
    var fnY: Lambda
    var fnZ: Lambda

    // The 'typeinfo' of a struct contains a field 'methods' which is a slice
    t := @typeof(MyStruct)
    visit p: t.methods
    {
        // When visiting 'methods', the 'value' field contains the function pointer,
        // which can be casted to the correct type
        switch p.name
        {
        case "returnX": fnX = cast(Lambda) p.value
        case "returnY": fnY = cast(Lambda) p.value
        case "returnZ": fnZ = cast(Lambda) p.value
        }
    }

    // This are now valid functions, which can be called
    var v: MyStruct
    @assert(fnX(v) == 5)
    @assert(fnY(v) == 10)
    @assert(fnZ(v) == 20)
}</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">opspec</div><code style="white-space: break-spaces;">// A struct can have special operations in the 'impl' block.
// This operations are predefined, and known by the compiler.
// This is the way to go to overload operators for example.

struct Struct
{
    x, y: s32
}

alias OneType      = bool
alias AnotherType  = s32
alias WhateverType = bool

impl Struct
{
    // Called each time a variable needs to be dropped
    // This is the destructor in C++
    func opDrop(using self) {}

    // Called after a raw copy operation from one value to another
    func opPostCopy(using self) {}

    // Called after a move semantic operation from one value to another
    func opPostMove(using self) {}

    // Get value by slice [low..up]
    // Must return a string or a slice
    func opSlice(using self, low, up: uint)->string { return "true"; }

    // Get value by index
    func opIndex(using self, index: uint)->WhateverType { return true; }

    // Called by @countof
    // Use in a 'loop' block for example
    func opCount(using self)->uint          { return 0; }
    // Called by @dataof
    func opData(using self)->*WhateverType  { return null; }

    // Called for explicit/implicit casting between struct value and return type
    // Can be overloaded by a different return type
    // Example: x := cast(OneType) v
    func opCast(using self)->OneType      { return true; }
    func opCast(using self)->AnotherType  { return 0; }

    // Called to compare the struct value with something else
    // Can be overloaded
    // Returns true if they are equal, otherwise returns false
    // Called by '==', '!='
    func opEquals(using self, other: OneType)->bool      { return false; }
    func opEquals(using self, other: AnotherType)->bool  { return false; }

    // Called to compare the struct value with something else
    // Can be overloaded
    // Returns -1, 0 or 1
    // Called by '<', '>', '<=', '>=', '<=>'
    func opCmp(using self, other: OneType)->s32      { return 0; }
    func opCmp(using self, other: AnotherType)->s32  { return 0; }

    // Affect struct with another value
    // Can be overloaded
    // Called by '='
    func opAffect(using self, other: OneType) {}
    func opAffect(using self, other: AnotherType) {}

    // Affect struct with a literal value with a specified suffix
    // Generic function, can be overloaded
    // Called by '='
    func(suffix: string) opAffectSuffix(using self, value: OneType) {}
    func(suffix: string) opAffectSuffix(using self, value: AnotherType) {}

    // Affect struct with another value at a given index
    // Can be overloaded
    // Called by '[] ='
    func opIndexAffect(using self, index: uint, value: OneType) {}
    func opIndexAffect(using self, index: uint, value: AnotherType) {}

    // Binary operator. 'op' generic argument contains the operator string
    // Generic function, can be overloaded
    // Called by '+', '-', '*', '/', '%', '|', '&', '^', '<<', '>>'
    func(op: string) opBinary(using self, other: OneType)->Self     { return @{1, 2}; }
    func(op: string) opBinary(using self, other: AnotherType)->Self { return @{1, 2}; }

    // Unary operator. 'op' generic argument contains the operator string (see below)
    // Generic function
    // Called by '!', '-', '~'
    func(op: string) opUnary(using self)->Self { return @{1, 2}; }

    // Affect operator. 'op' generic argument contains the operator string (see below)
    // Generic function, can be overloaded
    // Called by '+=', '-=', '*=', '/=', '%=', '|=', '&=', '^=', '<<=', '>>='
    func(op: string) opAssign(using self, other: OneType) {}
    func(op: string) opAssign(using self, other: AnotherType)  {}

    // Affect operator. 'op' generic argument contains the operator string (see below)
    // Generic function, can be overloaded
    // Called by '+=', '-=', '*=', '/=', '%=', '|=', '&=', '^=', '<<=', '>>='
    func(op: string) opIndexAssign(using self, index: uint, value: OneType) {}
    func(op: string) opIndexAssign(using self, index: uint, value: AnotherType) {}

    // Called by a 'visit' block
    // Can have multiple versions, by adding a name after 'opVisit'
    // This is the way to go for iterators
    #[Swag.Macro]
    {
        func(ptr: bool) opVisit(using self, stmt: code) {}
        func(ptr: bool) opVisitWhatever(using self, stmt: code) {}
        func(ptr: bool) opVisitAnother(using self, stmt: code) {}
    }
}</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">opaffect</div><code style="white-space: break-spaces;">struct Struct
{
    x, y, z: s32 = 666
}

impl Struct
{
    // opAffect is a way of assigning to a struct with '='
    // You can have more the one opAffect with different types
    mtd opAffect(value: s32)  { x, y = value; }
    mtd opAffect(value: bool) { x, y = value ? 1 : 0; }
}

#test
{
    // This will initialize v and then call opAffect(s32) with '4'
    var v: Struct = 4's32
    @assert(v.x == 4)
    @assert(v.y == 4)
    // Note that variable 'v' is also initiliazed with the default values.
    // So here 'z' is still 666.
    @assert(v.z == 666)

    // This will call opAffect(bool) with 'true'
    var v1: Struct = true
    @assert(v1.x == 1)
    @assert(v1.y == 1)

    // This will call opAffect(bool) with 'false'
    v1 = false
    @assert(v1.x == 0)
    @assert(v1.y == 0)
}

impl Struct
{
    // If opAffect is supposed to initialize the full content of the struct, you can add #[Swag.Complete]
    // This will avoid every variables to be initialized to the default values, then changed later
    // with the 'opAffect' call.
    #[Swag.Complete]
    mtd opAffect(value: u64)  { x, y, z = cast(s32) value; }

    // For later
    #[Swag.Implicit]
    mtd opAffect(value: u16)  { x, y = cast(s32) value; }
}

#test
{
    // Here the variable 'v' will not be initialized prior to the affectation
    // This is more optimal, as there's only one initialization
    var v: Struct = 2'u64
    @assert(v.x == 2)
    @assert(v.y == 2)
    @assert(v.z == 2)
}

#test
{
    func toto(v: Struct)
    {
        @assert(v.x == 5)
        @assert(v.y == 5)
        @assert(v.z == 666)
    }

    func titi(v: Struct)
    {
        @assert(v.y == 666)
    }

    // By default, there's no automatic conversion for a function argument, so you must cast.
    // Here, this will create a 'Struct' on the stack, initialized with a call to 'opAffect(s32)'
    toto(cast(Struct) 5's32)

    // But if opAffect is marked with #[Swag.Implicit], automatic conversion can occur.
    // No need for an explicit cast.
    toto(5'u16)
}

// A structure can be converted to a constant by a call to opAffect if the function is
// marked with 'Swag.ConstExpr'
struct Vector2
{
    x, y: f32
}

impl Vector2
{
    #[Swag.ConstExpr]
    mtd opAffect(one: f32)
    {
        x, y = one
    }
}

const One: Vector2 = 1.0    // This will call opAffect(1.0) at compile time
#assert One.x == 1.0
#assert One.y == 1.0</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">opcount</div><code style="white-space: break-spaces;">struct MyStruct
{
}

impl MyStruct
{
    mtd opCount() => 4'uint
}

#test
{
    v := MyStruct{}

    // '@countof' will call 'opCount'
    @assert(@countof(v) == 4)

    // You can then loop on a struct value if 'opCount' has been defined
    cpt := 0
    loop v
        cpt += 1

    @assert(cpt == 4)
}</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">oppost</div><code style="white-space: break-spaces;">// Swag accepts copy and move semantics for structures.

struct Vector3
{
    x, y, z: s32 = 666
}

impl Vector3
{
    // This is used for 'copy semantic'
    mtd opPostCopy()
    {
        x, y, z += 1
    }

    // This is used for 'move semantic'
    mtd opPostMove()
    {
        x, y, z += 2
    }

    mtd opDrop()
    {
    }
}

#test
{
    a := Vector3{}
    b := Vector3{100, 200, 300}

    // "copy semantic"
    // 1. This will call 'opDrop' on 'a' if it exists
    // 2. This will raw copy 'b' to 'a'
    // 3. This will call 'opPostCopy' on 'a' if it exists
    a = b
    @assert(a.x == 101)
    @assert(a.y == 201)
    @assert(a.z == 301)

    // "move semantic"
    // 1. This will call 'opDrop' on 'a' if it exists
    // 2. This will raw copy 'b' to 'a'
    // 3. This will call 'opPostMove' on 'a' if it exists
    // 4. This will reinitialize 'b' to the default values if 'opDrop' exists
    a =,move b
    @assert(a.x == 102)
    @assert(a.y == 202)
    @assert(a.z == 302)

    // 'b' contains an 'opDrop' special function, so it will be initialized to
    // the default values after the 'move'
    @assert(b.x == 666)

    // If you know what you're doing, you can avoid the first call to 'opDrop' with '=,nodrop'
    // Do it when you know the state of 'a' and do not want an extra unnecessary call
    // (for example if 'a' is in an undetermined state)
    a =,nodrop b
    a =,nodrop,move b

    // For the 'move semantic', you can avoid the last reinitialization by using '=,moveraw'
    // instead of '=,move'
    a =,moveraw b
    a =,nodrop,moveraw b
}
</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">opvisit</div><code style="white-space: break-spaces;">// You can visit a struct value if a macro 'opVisit' has been defined
// This is the equivalent of an 'iterator'

struct MyStruct
{
    x: s32 = 10
    y: s32 = 20
    z: s32 = 30
}

impl MyStruct
{
    #[Swag.Macro]
    func(ptr: bool) opVisit(self, stmt: code)
    {
        // 'ptr' is a generic parameter that tells if we want to visit by pointer.
        // We do not use it in this example.
        #assert(!ptr, "visiting myStruct by pointer is not supported")

        // Loop on the 3 fields
        loop idx: 3
        {
            // The '#macro' force its body to be in the scope of the caller
            #macro
            {
                // @alias0 will be the value
                var @alias0: s32 = ?

                // As this code is in the caller scope, with need to add a backtick before 'idx' to reference the
                // variable of this function (and not a potential variable in the caller scope)
                switch `idx
                {
                case 0: @alias0 = `self.x   // Same for function parameter 'self'
                case 1: @alias0 = `self.y
                case 2: @alias0 = `self.z
                }

                // @alias1 will be the index
                @alias1 := @index

                // include user code
                #mixin `stmt
            }
        }
    }
}

#test
{
    myStruct := MyStruct{}
    cpt := 0

    // Visiting each field in declaration order
    // 'v' is an alias for @alias0 (value)
    // 'i' is an alias for @alias1 (index)
    visit v, i: myStruct
    {
        switch i
        {
        case 0: @assert(v == 10)
        case 1: @assert(v == 20)
        case 2: @assert(v == 30)
        }

        cpt += 1
    }

    @assert(cpt == 3)
}

// You can have variants of opVisit by specifying an additional name
impl MyStruct
{
    #[Swag.Macro]
    mtd(ptr: bool) opVisitReverse(stmt: code)   // We add 'Reverse' in the name
    {
        // Visit fields in reverse order (z, y then x)
        loop idx: 3
        {
            #macro
            {
                var @alias0: s32 = ?
                switch `idx
                {
                case 0: @alias0 = `self.z
                case 1: @alias0 = `self.y
                case 2: @alias0 = `self.x
                }

                @alias1 := @index
                #mixin `stmt
            }
        }
    }
}

#test
{
    myStruct := MyStruct{}
    cpt := 0

    // To call a variant, add the extra name between parenthesis
    visit(Reverse) v, i: myStruct
    {
        switch i
        {
        case 0: @assert(v == 30)
        case 1: @assert(v == 20)
        case 2: @assert(v == 10)
        }

        cpt += 1
    }

    @assert(cpt == 3)
}</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">offset</div><code style="white-space: break-spaces;">#test
{
    // You can force the layout of a field with the 'Swag.Offset' attribute
    {
        struct MyStruct
        {
            x: s32

            // 'y' is located at the same offset as 'x', so they share the same space
            #[Swag.Offset("x")]
            y: s32
        }

        #assert @sizeof(MyStruct) == 4
        v := MyStruct{}
        v.x = 666

        // As 'y' and 'x' are sharing the same space, affecting to 'x' also affects to 'y'.
        @assert(v.y == 666)
    }

    // An example to reference a field by index.
    {
        struct MyStruct
        {
            x, y, z:    f32

            #[Swag.Offset("x")]
            idx:        [3] f32
        }

        var v: MyStruct
        v.x = 10; v.y = 20; v.z = 30
        @assert(v.idx[0] == v.x)
        @assert(v.idx[1] == v.y)
        @assert(v.idx[2] == v.z)
    }
}
</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">pack</div><code style="white-space: break-spaces;">// You can also control the struct layout with two attributes: #[Swag.Pack] and #[Swag.Align]

#test
{
    // The default struct packing is the same as in C: each field is aligned to the size of the type.
    // This is the equivalent of #[Swag.Pack(0)]
    {
        struct MyStruct
        {
            x: bool     // offset 0: aligned to 1 byte
            y: s32      // offset 4: s32 is aligned to 4 bytes (so here there's 3 bytes of padding before)
            z: s64      // offset 8: aligned to 8 bytes
        }

        // '@offsetof' can be used to retrieve the offset of a field
        #assert @offsetof(MyStruct.x) == 0
        #assert @offsetof(MyStruct.y) == 4
        #assert @offsetof(MyStruct.z) == 8
        #assert @sizeof(MyStruct)     == 16
    }

    // You can *reduce* the packing of the fields with #[Swag.Pack].
    {
        #[Swag.Pack(1)]
        struct MyStruct1
        {
            x: bool // offset 0: 1 byte
            y: s32  // offset 1: 4 bytes (no padding)
        }

        #assert @offsetof(MyStruct1.x) == 0
        #assert @offsetof(MyStruct1.y) == 1
        #assert @sizeof(MyStruct1)     == 5

        #[Swag.Pack(2)]
        struct MyStruct2
        {
            x: bool // offset 0: 1 byte
            y: s32  // offset 2: 4 bytes (1 byte of padding before)
        }

        #assert @offsetof(MyStruct2.x) == 0
        #assert @offsetof(MyStruct2.y) == 2
        #assert @sizeof(MyStruct2)     == 6

        #[Swag.Pack(4)]
        struct MyStruct3
        {
            x: bool // offset 0: 1 byte
            y: s64  // offset 4: 8 bytes (3 bytes of padding before)
        }

        #assert @offsetof(MyStruct3.x) == 0
        #assert @offsetof(MyStruct3.y) == 4
        #assert @sizeof(MyStruct3)     == 12
    }

    // The total struct size is always a multiple of the biggest alignement of the fields
    {
        struct MyStruct1
        {
            x: s32  // 4 bytes
            y: bool // 1 byte
            // 3 bytes of padding because of 'x', to aligned on 's32'
        }

        #assert @sizeof(MyStruct1) == 8
    }

    // You can force the struct alignement with #[Swag.Align].
    {
        struct MyStruct1
        {
            x: bool     // 1 byte
            y: bool     // 1 byte
        }

        #assert @offsetof(MyStruct1.x) == 0
        #assert @offsetof(MyStruct1.y) == 1
        #assert @sizeof(MyStruct1)     == 2

        #[Swag.Align(8)]
        struct MyStruct2
        {
            x: bool     // 1 byte
            y: bool     // 1 byte
            // 6 bytes of padding to be a multiple of '8'
        }

        #assert @offsetof(MyStruct2.x) == 0
        #assert @offsetof(MyStruct2.y) == 1
        #assert @sizeof(MyStruct2)     == 8
    }

    // You can also force each field to be aligned on a specific value
    {
        struct MyStruct1
        {
            x: bool // offset 0: 1 byte
            #[Swag.Align(8)]
            y: bool // offset 8: aligned to 8 (7 bytes of padding before)
        }

        #assert @sizeof(MyStruct1) == 9

        #[Swag.Align(8)]
        struct MyStruct2
        {
            x: bool // offset 0: 1 byte
            #[Swag.Align(4)]
            y: bool // offset 4: aligned to 4 (3 bytes of padding before)
            // 3 bytes of padding to be a multiple of 8
        }

        #assert @sizeof(MyStruct2) == 8
    }
}
</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">union</div><code style="white-space: break-spaces;">#test
{
    // An union is just a struct where all fields are located at offset 0
    {
        union MyUnion
        {
            x, y, z: f32
        }

        v := MyUnion{x: 666}
        @assert(v.y == 666)
        @assert(v.z == 666)
    }
}
</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">interface</div><code style="white-space: break-spaces;">// Interfaces are 'virtual tables' (a list of function pointers) that can be associated to a struct.
// Unlike c++, the virtual table is not embedded with the struct. It is a separate
// object.
// You can then 'implement' an interface for a given struct without changing the struct
// definition.

struct Point2
{
    x, y: f32
}

struct Point3
{
    x, y, z: f32
}

// Here we declare an interface, with two functions 'set' and 'reset'.
interface IReset
{
    // You must declare a lambda variable for each "virtual" function.
    // As the first parameter is always 'self', you can either declare it
    // yourself or use 'mtd/mtdc'
    set: mtd(f32)

    // You can also use the normal "func/mtd/mtdc" declaration.
    // The compiler will then create the table entry for you.
    mtd reset()
}

// You can implement an interface for any given struct with 'impl' and 'for'
// For example here, we implement interface 'IReset' for struct 'Point2'.
impl IReset for Point2
{
    mtd set(val: f32)
    {
        x = val
        y = val+1
    }

    mtd reset()
    {
        x, y = 0
    }
}

// And we implement interface 'IReset' also for struct 'Point3'.
impl IReset for Point3
{
    mtd set(val: f32)
    {
        x = val
        y = val+1
        z = val+2
    }

    mtd reset()
    {
        x, y, z = 0
    }
}

#test
{
    var pt2: Point2
    var pt3: Point3

    // To get the interface associated to a given struct, use the 'cast' operator.
    // If the compiler does not find the corresponding implementation, it will raise an error.
    itf := cast(IReset) pt2
    itf.set(10)
    @assert(pt2.x == 10)
    @assert(pt2.y == 10+1)

    itf = cast(IReset) pt3
    itf.set(10)
    @assert(pt3.x == 10)
    @assert(pt3.y == 10+1)
    @assert(pt3.z == 10+2)
    itf.reset()
    @assert(pt3.x == 0 and pt3.y == 0)

    // You can also access, with a normal call, all functions declared in an interface
    // implementation block for a given struct.
    // They are located in a dedicated scope.
    pt2.IReset.set(10)
    pt2.IReset.reset()
    pt3.IReset.set(10)
    pt3.IReset.reset()

    // An interface is a real type, with a size equivalent to 2 pointers
    #assert @sizeof(itf) == 2 * @sizeof(*void)

    // You can retrieve the concrete type associated with an interface instance with '@kindof'
    itf = cast(IReset) pt2
    @assert(@kindof(itf) == Point2)
    itf = cast(IReset) pt3
    @assert(@kindof(itf) == Point3)

    // You can retrieve the concrete data associated with an interface instance with '@dataof'
    itf = cast(IReset) pt2
    @assert(@dataof(itf) == &pt2)
    itf = cast(IReset) pt3
    @assert(@dataof(itf) == &pt3)
}</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">function</div><code style="white-space: break-spaces;">// A function starts with the 'func' keyword
func toto()
{
}

// To return a value, you must add -> followed by the type
func toto1()->s32
{
    return 0
}

// The return type can be deduced in case of a simple expression, with '=>' instead of '->'
func sum(x, y: s32) => x + y

// A short version exists, in case of a function returning nothing
func print(val: string) = @print(val)

// Parameters
func sum1(x, y: s32, unused: f32)->s32
{
    return x + y
}

// Parameters can have default values
func sum2(x, y: s32, unused: f32 = 666)->s32
{
    return x + y
}

// Type of parameters can be deduced in case of a default value
func sum3(x, y = 0.0)
{
    #assert @typeof(x) == f32
    #assert @typeof(y) == f32
}

enum Values { A; B; }
func toto(x: s32, y = Values.A)
{
    #assert @typeof(y) == Values
}

#test
{
    // Functions can be nested inside other functions.
    // This are not closure but just functions in a sub scope.
    func sub(x, y: s32) => x - y

    // Simple call
    {
        x := sub(5, 2)
        @assert(x == 3)
    }

    // You can name parameters, and don't have to respect parameters order
    // in that case
    {
        x1 := sub(x: 5, y: 2)
        @assert(x1 == 3)
        x2 := sub(y: 5, x: 2)
        @assert(x2 == -3)
    }

    {
        func returnMe(x, y: s32 = 0) => x + y * 2
        @assert(returnMe(x: 10) == 10)
        @assert(returnMe(y: 10) == 20)
    }
}

#test
{
    // A tuple can be used to return multiple values in a function
    {
        func myFunction() -> {f32, f32}
        {
            return @{1.0, 2.0}
        }

        result := myFunction()
        @assert(result.item0 == 1.0)
        @assert(result.item1 == 2.0)

        var (x, y) = myFunction()
        @assert(x == 1.0)
        @assert(y == 2.0)

        (z, w) := myFunction()
        @assert(z == 1.0)
        @assert(w == 2.0)
    }

    // You can name the fields of the function result too
    func returns2() -> {x: s32, y: s32}
    {
        // You can return a tuple literal as long as the types match
        if false
            return @{1, 2}

        // Or use the specifal type 'retval' which is an alias to the
        // function return type (but not only, we'll see later)
        using var result: retval
        x = 1
        y = 2
        return result
    }

    // You can access the tuple fields with the names (if specified), or with 'item?'
    result := returns2()
    @assert(result.x == 1)
    @assert(result.item0 == 1)
    @assert(result.y == 2)
    @assert(result.item1 == 2)

    // You can deconstruct the returned tuple
    (x, y) := returns2()
    @assert(x == 1)
    @assert(y == 2)
}</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">lambda</div><code style="white-space: break-spaces;">#test
{
    // A lambda is just a pointer to a function
    {
        func myFunction0() {}
        func myFunction1(x: s32) => x * x

        // 'ptr0' is declared as a pointer to a function that takes no parameter, and returns nothing
        var ptr0: func() = &myFunction0
        ptr0()

        // Here type of 'ptr1' is deduced from 'myFunction1'
        ptr1 := &myFunction1
        @assert(myFunction1(2) == 4)
        @assert(ptr1(2) == 4)
    }

    // A lambda can be null
    {
        var lambda: func()->bool
        @assert(lambda == null)
    }

    // You can use a lambda as a function parameter type
    {
        alias callback = func(s32)->s32
        func toDo(value: s32, ptr: callback)->s32 => ptr(value)

        func square(x: s32) => x * x
        @assert(toDo(4, &square) == 16)
    }

    // You can also create anonymous functions with '@' (functions as literals)
    {
        cb := @(x: s32)->s32 => x * x
        @assert(cb(4) == 16)
        cb = @(x: s32)->s32 => x * x * x
        @assert(cb(4) == 64)
    }

    // Anonymous functions can be passed as parameters to another function
    {
        alias callback = func(s32)->s32
        func toDo(value: s32, ptr: callback)->s32 => ptr(value)

        @assert(toDo(4, @(x: s32) => x * x) == 16)
        @assert(toDo(4, @(x: s32) => x + x) == 8)
        @assert(toDo(4, @(x: s32)->s32 { return x - x; }) == 0)
    }

    // Types of parameters can be deduced
    {
        alias callback = func(s32)->s32
        func toDo(value: s32, ptr: callback)->s32 => ptr(value)

        @assert(toDo(4, @(x) => x * x) == 16)
        @assert(toDo(4, @(x) => x + x) == 8)
        @assert(toDo(4, @(x) { return x - x; }) == 0)
    }

    // When you affect a lambda to a variable, the type of parameters and the return type can also be
    // omitted, as they will be deduced from the variable type
    {
        var fct: func(s32, s32)->bool

        fct = @(x, y) => x == y
        @assert(fct(10, 10))

        fct = @(x, y) { return x != y; }
        @assert(fct(20, 120))
    }
}</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">mixin</div><code style="white-space: break-spaces;">#test
{
    // A mixin is declared like a function, with the attribute #[Swag.Mixin]
    {
        #[Swag.Mixin]
        func myMixin()
        {
        }
    }

    // A mixin function is inserted in the scope of the caller
    {
        #[Swag.Mixin]
        func myMixin()
        {
            a += 1
        }

        a := 0
        myMixin()   // Equivalent to 'a += 1'
        myMixin()   // Equivalent to 'a += 1'
        @assert(a == 2)
    }

    // This behaves like a function, so you can add parameters
    {
        #[Swag.Mixin]
        func myMixin(increment: s32 = 1)
        {
            a += increment
        }

        a := 0
        myMixin()   // Equivalent to 'a += 1'
        myMixin(2)  // Equivalent to 'a += 2'
        @assert(a == 3)
    }

    // A mixin accepts parameters of type 'code'
    {
        #[Swag.Mixin]
        func doItTwice(what: code)
        {
            // You can then insert the code with '#mixin'
            #mixin what
            #mixin what
        }

        a := 0
        doItTwice(#code {a += 1;})
        @assert(a == 2)
    }

    // When the last parameter of a mixin is of type 'code', the caller
    // can declare that code in a statement just after the call
    {
        #[Swag.Mixin]
        func doItTwice(value: s32, what: code)
        {
            #mixin what
            #mixin what
        }

        a := 0
        doItTwice(4)
        {
            a += value
        }

        doItTwice(2)
        {
            a += value
        }

        @assert(a == 12)
    }

    // You can use special name '@alias' to create a named alias for an identifier
    {
        #[Swag.Mixin]
        func inc10()
        {
            @alias0 += 10
        }

        a, b := 0
        inc10(|a|)  // Passing alias name 'a'
        inc10(|b|)  // Passing alias name 'b'
        @assert(a == b and b == 10)
    }

    {
        #[Swag.Mixin]
        func setVar(value: s32)
        {
            var @alias0 = value
        }

        setVar(|a| 10)  // Passing alias name 'a'
        setVar(|b| 20)  // Passing alias name 'b'
        @assert(a == 10)
        @assert(b == 20)
        setVar(30)      // No alias, so name is @alias0
        @assert(@alias0 == 30)
    }

    // You can declare special variables named '@mixin?'
    // Those variables will have a unique name each time the mixin is used
    // So the same mixin, even if it declares local variables, can be used
    // multiple time in the same scope
    {
        var total: s32

        #[Swag.Mixin]
        func toScope()
        {
            var @mixin0: s32 = 1
            total += @mixin0
        }

        toScope()
        toScope()
        toScope()

        @assert(total == 3)
    }
}</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">macro</div><code style="white-space: break-spaces;">#test
{
    // A macro, like a mixin, is declared like a function, but with the attribute 'Swag.Macro'
    {
        #[Swag.Macro]
        func myMacro()
        {
        }
    }

    // Unlike a mixin, a macro has its own scope, and cannot conflict with the function
    // it is inserted inside
    {
        #[Swag.Macro]
        func myMacro()
        {
            a := 666    // 'a' is declared in the scope of 'myMacro'
        }

        a := 0
        myMacro()   // no conflict with the 'a' declared above
        @assert(a == 0)
    }

    // But you can force an identifier to be found outside of the scope of the macro
    {
        #[Swag.Macro]
        func myMacro()
        {
            `a += 1 // Back ticked to reference the 'a' of the caller
        }

        a := 0
        myMacro()
        myMacro()
        @assert(a == 2)
    }

    // Like a mixin, macro accepts 'code' parameters
    {
        #[Swag.Macro]
        func myMacro(what: code)
        {
            #mixin what
        }

        a := 0

        myMacro(#code { `a += 1; } )

        myMacro()
        {
            `a += 1
        }

        @assert(a == 2)
    }

    // You can use '#macro' inside a macro to force the code after to be in the
    // same scope of the caller. That is, no back tick is necessary to reference
    // variables of the caller.
    {
        #[Swag.Macro]
        func myMacro(what: code)
        {
            // No conflict, in its own scope
            a := 666

            #macro
            {
                // In the scope of the caller
                #mixin `what
            }
        }

        a := 1
        myMacro()
        {
            a += 2  // will reference the 'a' above because this code has been inserted inside '#macro'
        }

        @assert(a == 3)
    }

    // You can extend the language with macros, without using pointers to functions
    // (no lambda call cost)
    {
        #[Swag.Macro]
        func repeat(count: s32, what: code)
        {
            a := 0
            while a < count
            {
                #macro
                {
                    index := `a
                    #mixin `what
                }

                a += 1
            }
        }

        a := 0
        repeat(5) { a += index; }
        @assert(a == 0+1+2+3+4)
        repeat(3) { a += index; }
        @assert(a == 10+3)
    }

    // When you need 'break' in the user code to break outside of
    // a multi loop
    {
        #[Swag.Macro]
        func repeatSquare(count: u32, what: code)
        {
            scope Up loop count
            {
                loop count
                {
                    #macro
                    {
                        // 'break' in the user code will be replaced by 'break Up'
                        // So it will break outside the outer loop
                        #mixin `what { break = break Up; }
                    }
                }
            }
        }

        a := 0
        repeatSquare(5)
        {
            a += 1
            if a == 10
                break
        }

        @assert(a == 10)
    }

    // In a macro, you can use special variables named '@alias<num>'
    // Note that this is also valid for mixins.
    {
        #[Swag.Macro]
        func call(v: s32, stmt: code)
        {
            @alias0 := v
            @alias1 := v * 2
            #mixin stmt
        }

        call(20)
        {
            @assert(@alias0 == 20)
            @assert(@alias1 == 40)
        }

        // The caller can then name those special variables
        // Use |name0, name1, ...| before the function call parameters
        call(|x| 20)
        {
            @assert(x == 20)        // x is @alias0
            @assert(@alias1 == 40)  // @alias1 is not renamed
        }

        call(|x, y| 20)
        {
            @assert(x == 20)   // x is @alias0
            @assert(y == 40)   // y is @alias1
        }
    }
}</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">ufcs</div><code style="white-space: break-spaces;">#test
{
    // ufcs stands for "uniform function call syntax"
    // It allows every functions to be called with a 'param.func()' form when the
    // first parameter of func() is of the same type as 'param'
    {
        func myFunc(param: bool) => param

        b := false
        @assert(myFunc(b) == b.myFunc())
    }

    // This means that in Swag, there are only 'static' functions, but which
    // can be called like 'methods'
    {
        struct Point { x, y: s32; }
        func set(using pt: *Point, value: s32) { x, y = value; }

        var pt: Point

        // Ufcs call
        pt.set(10)
        @assert(pt.x == 10 and pt.y == 10)

        // Normal call
        set(&pt, 20)
        @assert(pt.x == 20 and pt.y == 20)
    }
}</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">variadic</div><code style="white-space: break-spaces;">#test
{
    // A function can take a variable number of arguments with '...'
    {
        func myFunction(value: bool, parameters: ...)
        {
        }

        myFunction(true, 4, "true", 5.6)
    }

    // In that case, 'parameters' is a slice of 'any'
    {
        func myFunction(parameters: ...)
        {
            // We can know the number of parameters at runtime with '@countof'
            @assert(@countof(parameters) == 3)

            // Each parameter is a type 'any'
            #assert @typeof(parameters[0]) == any
            #assert @typeof(parameters[1]) == any
            #assert @typeof(parameters[2]) == any

            // But you can use '@kindof' to get the real type (at runtime) of the parameter
            @assert(@kindof(parameters[0]) == s32)
            @assert(@kindof(parameters[1]) == string)
            @assert(@kindof(parameters[2]) == f32)
        }

        myFunction(4, "true", 5.6)
    }

    // If all variadic parameters are of the same type, you can force it.
    // Parameters then won't be of type 'any'
    {
        func myFunction(value: bool, parameters: s32...)
        {
            #assert @typeof(parameters[0]).name == "s32"
            #assert @typeof(parameters[1]).name == "s32"
            #assert @typeof(parameters[2]) == s32
            #assert @typeof(parameters[3]) == s32

            @assert(parameters[0] == 10)
            @assert(parameters[1] == 20)
            @assert(parameters[2] == 30)
            @assert(parameters[3] == 40)
        }

        myFunction(true, 10, 20, 30, 40)
    }

    // Variadic parameters can be passed from function to function
    {
        func A(params: ...)
        {
            @assert(@countof(params) == 2)
            @assert(@kindof(params[0]) == string)
            @assert(@kindof(params[1]) == bool)
            @assert(cast(string) params[0] == "value")
            @assert(cast(bool) params[1] == true)
        }

        func B(params: ...)
        {
            A(params)
        }

        B("value", true)
    }

    // You can spread the content of an array or a slice to variadic parameters with '@spread'
    {
        func sum(params: s32...)->s32
        {
            // Note that variadic parameters can be visited, as this is a slice
            total := 0
            visit v: params
                total += v
            return total
        }

        arr := @[1, 2, 3, 4]
        res := sum(@spread(arr)) // is equivalent to sum(1, 2, 3, 4)
        @assert(res == 1+2+3+4)

        res1 := sum(@spread(arr[1..2])) // is equivalent to sum(2, 3)
        @assert(res1 == 2+3)
    }
}</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">constexpr</div><code style="white-space: break-spaces;">// A function marked with 'Swag.ConstExpr' can be executed by the compiler if it can
#[Swag.ConstExpr]
func sum(x, y: f32) => x + y

// Here 'G' will be baked to 3 by the compiler
const G = sum(1, 2)
#assert G == 3

// If a function is not 'ConstExpr', you can force the compile time call with #run
func mul(x, y: f32) => x * y
const G1 = #run mul(3, 6)
#assert G1 == 18</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">overload</div><code style="white-space: break-spaces;">// Functions can have the same names as long as parameters are different
#[Swag.ConstExpr]
{
    func sum(x, y: s32) => x + y
    func sum(x, y, z: s32) => x + y + z
}

#assert sum(1, 2) == 3
#assert sum(1, 2, 3) == 6

// Note that in Swag, there's no implicit cast for function parameters
// So you must always specify the right type
#test
{
    func over(x, y: s32) => x + y
    func over(x, y: s64) => x * y

    // This would generate an error because it's ambiguous, as 1 and 2 are not strong types
    // res0 := over(1, 2)

    res0 := over(1's32, 2's32)
    @assert(res0 == 3)
    res1 := over(1's64, 2's64)
    @assert(res1 == 2)
}
</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">discard</div><code style="white-space: break-spaces;">#test
{
    // By default, you must always use the returned value of a function,
    // otherwise the compiler will generate an error
    func sum(x, y: s32) => x + y

    // This would generated an error, because the return value of 'sum' is not used
    // sum(2, 3)

    // To force the return value to be ignored, you can use 'discard' at the call site
    discard sum(2, 3)

    // If a function authorizes the caller to not use its return value, because
    // it's not that important, it can be marked with 'Swag.Discardable'

    #[Swag.Discardable]
    func mul(x, y: s32)->s32 => x * y

    // This is fine to ignore the return value of 'mul' (even if strange)
    mul(2, 4)
}</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">retval</div><code style="white-space: break-spaces;">#test
{
    // Inside a function, you can use the 'retval' type which is an alias
    // to the function return type
    {
        func toto()->s32
        {
            var result: retval
            result = 10
            return result
        }

        @assert(toto() == 10)
    }

    // But 'retval' will also make a direct reference to the caller storage, to
    // avoid an unnecessary copy (if possible). So this is mostly a hint for the
    // compiler, and usefull when the function returns a complexe type like a
    // struct, a tuple or an array.
    {
        struct RGB { x, y, z: f64; }

        func getWhite()->RGB
        {
            // To avoid the clear of the returned struct, we use = ?
            var result: retval = ?
            result.x = 0.5
            result.y = 0.1
            result.z = 1.0
            return result
        }

        // Here the 'getWhite' function can directly modify r, g and b without storing
        // a temporary value on the stack.
        (r, g, b) := getWhite()
        @assert(r == 0.5)
        @assert(g == 0.1)
        @assert(b == 1.0)
    }

    // This is the preferred way (because optimal) to return a struct, a tuple or an array.
    {
        func toto()->[255] s32
        {
            // To avoid the clear of the array, we use = ?
            var result: retval = ?
            loop i: 255
                result[i] = i
            return result
        }

        arr := toto()
        @assert(arr[0] == 0)
        @assert(arr[100] == 100)
        @assert(arr[254] == 254)
    }
}</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">closure</div><code style="white-space: break-spaces;">// Swag supports a limited set of the 'closure' concept
// Only a given amount of bytes of capture are possible (for now 48 bytes)
// That way there's never an hidden allocation
// Another limitation is that you can only capture 'simple' variables (no struct with 'opDrop', 'opPostCopy', 'opPostMove' for example)

#test
{
    // A closure is declared like a lambda, with the captured variables between |...| before the function parameters
    {
        a := 125
        b := 521

        // Capture 'a' and 'b'
        var fct: closure() = @|a, b|()
        {
            @assert(a == 125)
            @assert(b == 521)
        }

        fct()
    }

    // You can also capture by pointer with '&' (otherwise it's a copy)
    {
        a := 125

        // Capture 'a' by pointer
        var fct: closure() = @|&a|()
        {
            // We can change the value of the local variable 'a'
            dref a += 1
        }

        fct()
        @assert(a == 126)
        fct()
        @assert(a == 127)
    }

    // You can assign a normal lambda (no capture) to a closure variable
    {
        var fct: closure(s32, s32)->s32

        fct = @(x, y) => x + y
        @assert(fct(1, 2) == 3)
    }

    // You can capture arrays, structs, slices etc... as long as it fits in the
    // maximum storage of 'n' bytes (and as long as the struct is a pod)
    {
        x := @[1, 2, 3]

        var fct: closure(s32)->s32

        // Capture the array 'x' by making a copy
        fct = @|x|(toAdd)
        {
            res := 0
            visit v: x
                res += v
            res += toAdd
            return res
        }

        result := fct(4)
        @assert(result == 1 + 2 + 3 + 4)
    }

    // Captured variables are mutable, and part of the closure
    // So you can modify them
    {
        func getInc()->closure()->s32
        {
            x := 10

            return @|x|()->s32 {
                x += 1
                return x
            }
        }

        fct := getInc()
        @assert(fct() == 11)
        @assert(fct() == 12)
        @assert(fct() == 13)
    }
}</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">foreign</div><code style="white-space: break-spaces;">// Swag can interop with external "modules" (dlls under windows), which
// contain exported C functions

alias DWORD = u32
alias UINT  = u32

// Put a special attribute 'Swag.Foreign' before the function prototype, and specify
// the module name where the function is located.
// The module name can be a swag compiled module, or an external system module (where the location
// depends on the OS).
//
// In the case below, the function is located in kernel32.dll (under windows)
#[Swag.Foreign("kernel32")]
func ExitProcess(uExitCode: UINT);

// Like for other attributes, you can use a block.
#[Swag.Foreign("kernel32")]
{
    func ExitProcess(uExitCode: UINT);
    func Sleep(dwMilliseconds: DWORD);
}

// Note that in the case of a system module, you will have to declare somewhere the
// imported library too
#foreignlib "kernel32"

// #foreignlib is here to force a link to the given library (when generating executables)</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">intrinsic</div><code style="white-space: break-spaces;">#global skip

// This is the list of all compiler intrinsics
// All intrinsics start with '@', which is reserved for them

// Base

func @assert(value: bool);
func @breakpoint();
func @getcontext()->*Swag.Context;
func @setcontext(context: const *Swag.Context);
func @isbytecode()->bool;
func @compiler()->Swag.ICompiler;

func @args()->const [..] string;
@panic()
@errormsg()

// Buildin

@spread()
@init()
@drop()
@postmove()
@postcopy()
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
@getpinfos()
@isconstexpr()
@itftableof()

@index
@err

// Memory related

func @alloc(size: uint)->*void;
func @realloc(ptr: *void, size: uint)->*void;
func @free(ptr: *void);
func @memset(dst: *void, value: u8, size: uint);
func @memcpy(dst: *void, src: const *void, size: uint);
func @memmove(dst: *void, src: const *void, size: uint);
func @memcmp(dst, src: const *void, size: uint)->s32;
func @strlen(value: const *u8)->uint;

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
func @byteswap(value: u64)->u64;</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">init</div><code style="white-space: break-spaces;">// @init can be used to reinitialize a variable/array to the default value

#test
{
    // For a simple variable, default value is 0
    {
        x := 666
        @init(&x)
        @assert(x == 0)
    }

    // Work also for an array, as you can specify the number of elements
    // you want to initialize
    {
        x := @[1, 2]
        @init(&x, 2)    // Initialize 2 elements
        @assert(x[0] == 0)
        @assert(x[1] == 0)
    }

    // You can also specify a "value" to initialize, instead of the default one
    {
        x := 666'f32
        @init(&x)(3.14)  // Initialize to 3.14 instead of zero
        @assert(x == 3.14)
    }

    // Same for an array
    {
        x := @[1, 2]
        @init(&x, 2)(555)
        @assert(x[0] == 555)
        @assert(x[1] == 555)
    }

    struct RGB
    {
        r = 1
        g = 2
        b = 3
    }

    // When called on a struct, the struct will be restored to the values defined in it
    {
        var rgb: RGB{10, 20, 30}
        @init(&rgb)
        @assert(rgb.r == 1)
        @assert(rgb.g == 2)
        @assert(rgb.b == 3)
    }

    // But you can also specified the values
    {
        var rgb: RGB{10, 20, 30}
        @init(&rgb)(5, 6, 7)
        @assert(rgb.r == 5)
        @assert(rgb.g == 6)
        @assert(rgb.b == 7)
    }

    // And this works also for array
    {
        var rgb: [4] RGB
        @init(&rgb, 4)(5, 6, 7)
        @assert(rgb[3].r == 5)
        @assert(rgb[3].g == 6)
        @assert(rgb[3].b == 7)
    }

    // Note that for a struct, this will *not* call opDrop, so this is mostly useful to
    // initialize a plain old data
    // But there also '@drop', which works the same, except that it will 'drop' all the
    // content by calling 'opDrop' if it is defind
    {
        var rgb: [4] RGB

        // In fact this is a no op, as struct RGB is plain old data, without a defined 'opDrop'
        @drop(&rgb, 4)

        @init(&rgb, 4)(5, 6, 7)
        @assert(rgb[3].r == 5)
        @assert(rgb[3].g == 6)
        @assert(rgb[3].b == 7)
    }
}</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">generic</div><code style="white-space: break-spaces;">#test
{
    {
        // A function can be generic by specifying parameters after 'func'
        // At the call site, you specify the generic parameters with funcCall'(type1, type2, ...)(parameters)
        // Note that parenthesis can be ommited if there's only one generic parameter

        // Here 'T' is a generic type
        func(T) myFunc(val: T) => 2 * val

        @assert(myFunc's32(2) == 4)
        @assert(myFunc'f32(2.0) == 4.0)
    }

    {
        // Generic types can be deduced from parameters, so func'type() is
        // not always necessary
        func(T) myFunc(val: T) => 2 * val

        @assert(myFunc(2's32) == 4)         // T is deduced to be s32
        @assert(myFunc(2.0'f32) == 4.0)     // T is deduced to be f32
    }

    {
        // Of course you can specify more than one generic parameter
        func(K, V) myFunc(key: K, value: V) => value

        @assert(myFunc(2's32, "value") == "value")              // K and V are deduced
        @assert(myFunc'(s32, string)(2, "value") == "value")    // K and V are explicit

        @assert(myFunc(2's32, true) == true)
        @assert(myFunc'(s32, bool)(2, true) == true)
    }

    // You can also specify constants as generic parameters
    {
        func(N: s32) myFunc() = @assert(N == 10)
        myFunc'10()
    }

    // And you can mix
    {
        func(T, N: s32) myFunc(x: T) => x * N

        alias call = myFunc'(s32, 10)
        @assert(call(2) == 20)
        @assert(call(100) == 1000)
    }

    // Like functions, a struct can be generic
    {
        struct(T) Struct
        {
            val: T
        }

        var x: Struct's32
        @assert(@typeof(x.val) == s32)
        var x1: Struct'f32
        @assert(@typeof(x1.val) == f32)
    }

    {
        struct(T, N: s32) Struct
        {
            val: [N] T
        }

        var x: Struct'(bool, 10)
        @assert(@typeof(x.val) == [10] bool)
    }
}</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">selectif</div><code style="white-space: break-spaces;">#test
{
    // On a function, you can use '#selectif' to constrain the usage of the function.
    // If the #selectif expression returns false, then the function will not be considered for the call match.
    // The #selectif expression is evaluated only once, whatever the call, so it is typically used to
    // check generic parameters.
    {
        func(T) sum(x: T...)->T
            #selectif @typeof(T) == s32 or @typeof(T) == s64
        {
            total := 0'T
            visit it: x
                total += it
            return total
        }

        res1 := sum's32(1, 2)
        @assert(res1 == 3)
        res2 := sum's64(10, 20)
        @assert(res2 == 30)

        // This would generate a compile error because type is 'f32', so there's no match for that type
        // res1 := sum'f32(1, 2)
    }

    // You can use #selectif to make a kind of a generic specialisation
    {
        // s32 version
        func(T) isNull(x: T)->bool
            #selectif @typeof(T) == s32
        {
            return x == 0
        }

        // f32/f64 version
        func(T) isNull(x: T)->bool
            #selectif @typeof(T) == f32 or @typeof(T) == f64
        {
            return @abs(x) < 0.01
        }

        @assert(isNull(0's32))
        @assert(isNull(0.001'f32))
    }

    // #selectif can also be followed by a block that returns a bool instead of an expression
    {
        func(T) sum(x: T...)->T
            #selectif
            {
                if @typeof(T) == s32 or @typeof(T) == s64
                    return true
                return false
            }
        {
            total := 0'T
            visit it: x
                total += it
            return total
        }
    }

    // By using '@errormsg', you can trigger errors at compile time if the
    // type is incorrect for example
    {
        func(T) sum(x, y: T)->T
            #selectif
            {
                if @typeof(T) == s32 or @typeof(T) == s64
                    return true
                @errormsg("invalid type " ++ @stringof(T), #location)
                return false
            }
        {
            return x + y
        }

        // This will trigger an error
        // x := sum'f32(1, 2)
    }

    // Instead of #selectif, you can use #checkif.
    // #checkif is evaluated for each call, so it can be used to check
    // parameters, as long as they can be evaluated at compile time
    {
        func div(x, y: s32)->s32
            #checkif
            {
                // If 'y' cannot be evaluated at compile time, then we can do nothing about it
                if !@isconstexpr(y)
                    return true
                if y == 0
                    @errormsg("division by zero", #location)
                return true
            }
        {
            return x / y
        }

        // Fine
        x1 := div(1, 1)

        // Error at compile time, division by zero
        // x2 := div(1, 0)
    }

    {
        // A version of 'first' where 'x' is known at compile time
        func first(x: s32)->s32
            #checkif @isconstexpr(x)
        {
            return 555
        }

        // A version of 'first' where 'x' is *not* known at compile time
        func first(x: s32)->s32
            #checkif !@isconstexpr(x)
        {
            return 666
        }

        // Will call version 1 because parameter is a literal
        @assert(first(0) == 555)

        // Will call version 2 because parameter is a variable
        var a: s32
        @assert(first(a) == 666)
    }
}</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">defer</div><code style="white-space: break-spaces;">#test
{
    // 'defer' is used to call an expression when the current scope is left.
    // It's purely compile time, so it does not evaluate until the block is left
    {
        v := 0
        defer @assert(v == 1)
        v += 1
        // defer expression will be executed here
    }

    // defer can also be used with a block
    {
        v := 0
        defer
        {
            v += 10
            @assert(v == 15)
        }

        v += 5
        // defer block will be executed here
    }

    // defer expressions are called when leaving the corresponding scope, even
    // with return, break, continue etc., and even inside a loop/while/for etc.
    {
        var G = 0
        loop 10
        {
            defer G += 1
            if G == 2
                break   // will be called here, before breaking the loop
            // will be called here also
        }

        @assert(G == 3)
    }

    // 'defer' are executed in reverse order of their declaration
    {
        x := 1
        defer @assert(x == 2)   // Will be executed second
        defer x *= 2            // Will be executed first
    }

    // It's typically used to unregister/destroy a resource, by putting the release
    // code just after the creation one
    {
        func createResource() => true
        func releaseResource(resource: *bool) = dref resource = false
        func isResourceCreated(b: bool) => b

        resource := false
        loop 10
        {
            resource = createResource()
            defer
            {
                @assert(resource.isResourceCreated())
                releaseResource(&resource)
            }

            if @index == 2
                break
        }

        @assert(!resource.isResourceCreated())
    }
}</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">using</div><code style="white-space: break-spaces;">#test
{
    // using brings the scope of a namespace, struct or enum in the current one
    {
        enum RGB { R; G; B; }
        @assert(RGB.R == 0)

        using RGB
        @assert(G == 1)
    }

    // using can also be used with a variable or a function parameter
    {
        struct Point { x: s32; y: s32; }

        var pt: Point

        using pt
        x = 1 // no need to specify 'pt'
        y = 2 // no need to specify 'pt'

        @assert(pt.x == 1)
        @assert(pt.y == 2)
    }

    // You can declare a variable with 'using' just before
    {
        struct Point { x: s32; y: s32; }

        using var pt: Point
        x = 1 // no need to specify 'pt'
        y = 2 // no need to specify 'pt'

        @assert(pt.x == 1)
        @assert(pt.y == 2)
    }

    // using can be the equivalent of the hidden 'this' in C++
    {
        struct Point { x: s32; y: s32; }

        func setOne(using point: *Point)
        {
            // Here there's no need to specify 'point'
            x, y = 1
        }

        var pt: Point
        setOne(&pt)
        @assert(pt.x == 1)
        @assert(pt.y == 1)

        // ufcs
        pt.setOne()
        @assert(pt.x == 1)
        @assert(pt.y == 1)
    }

    // using can be used with a field inside a struct
    {
        struct Point2
        {
            x, y: s32
        }

        struct Point3
        {
            using base: Point2
            z: s32
        }

        // That way the content of the field can be referenced directly
        var value: Point3
        value.x = 0 // Equivalent to value.base.x = 0
        value.y = 0 // Equivalent to value.base.y = 0
        value.z = 0
        @assert(&value.x == &value.base.x)
        @assert(&value.y == &value.base.y)

        // The compiler can then cast automatically 'Point3' to 'Point2'
        func set1(using ptr: *Point2)
        {
            x, y = 1
        }

        set1(&value) // Here the cast is automatic thanks to the using
        @assert(value.x == 1)
        @assert(value.y == 1)
        @assert(value.base.x == 1)
        @assert(value.base.y == 1)
    }
}</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">with</div><code style="white-space: break-spaces;">struct Point { x, y: s32; }

impl Point
{
    mtd setOne()
    {
        x, y = 1
    }
}

#test
{
    // You can use 'with' to avoid repeating the same variable again and again
    // You can then access fields with a simple '.'
    {
        var pt: Point
        with pt
        {
            .x = 1   // equivalent to pt.x
            .y = 2   // equivalent to pt.y
        }

        @assert(pt.x == 1)
        @assert(pt.y == 2)
    }

    // Work for function calls to
    {
        var pt: Point
        with pt
        {
            .setOne()           // equivalent to pt.setOne() or setOne(pt)
            .y = 2              // equivalent to pt.y
            @assert(.x == 1)    // equivalent to pt.x
            @assert(.y == 2)    // equivalent to pt.y
            @assert(pt.x == 1)
            @assert(pt.y == 2)
        }
    }

    // Works also for enums
    {
        enum RGB{R; G; B;}
        var color: RGB
        with color
        {
            x := .R
            @assert(x == RGB.R)
            @assert(x == .R)
        }
    }

    // Works also with a namespace
    {
        with NameSpace
        {
            .inside0()
            .inside1()
        }
    }

    // Instead of an identifier name, 'with' also accepts a variable declaration
    {
        with pt := Point{1, 2}
        {
            .x = 10
            .y = 20
        }

        @assert(pt.x == 10 and pt.y == 20)
    }

    {
        with var pt: Point
        {
            .x = 10
            .y = 20
        }

        @assert(pt.x == 10 and pt.y == 20)
    }

    {
        enum RGB{R; G; B;}
        with var color: RGB
        {
            x := .R
            @assert(x == RGB.R)
            @assert(x == .R)
        }
    }

    // Or an affect statement
    {
        var pt: Point
        with pt = Point{1, 2}
        {
            .x = 10
            .y = 20
        }

        @assert(pt.x == 10 and pt.y == 20)
    }
}

namespace NameSpace
{
    func inside0() {}
    func inside1() {}
}</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">alias</div><code style="white-space: break-spaces;">#test
{
    // 'alias' is used to make a shortcut to another name or type
    {
        enum RGB { R; G; B; }
        @assert(RGB.R == 0)

        alias Color = RGB
        @assert(Color.G == 1)
    }

    // Types alias
    {
        alias Float32 = f32
        alias Float64 = f64
        var x: Float32 = 1.0
        var y: Float64 = 1.0
        #assert @typeof(Float32) == f32
        #assert @typeof(Float64) == f64
    }

    // A type alias can be marked with the 'Swag.Strict' attribute.
    // In that case, implicit cast is not done, but explicit cast is
    // still possible
    {
        #[Swag.Strict]
        alias MyType = s32
        #assert @typeof(MyType) != s32
        var x: MyType = cast(MyType) 0
    }

    // You can also alias a function
    {
        func thisIsABigFunctionName(x: s32) => x * x
        alias myFunc = thisIsABigFunctionName
        @assert(myFunc(4) == 16)
    }

    // You can also alias a variable
    {
        var myLongVariableName: s32 = 0
        alias short = myLongVariableName
        short += 2
        @assert(myLongVariableName == 2)
    }
}</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">inline</div><code style="white-space: break-spaces;">#test
{
    // #inline is used to create an inlined separated scope
    {
        myVar := 0
        #inline
        {
            myVar := 5
        }

        @assert(myVar == 0)
    }

    // You can reference a variable outside the #inline scope with a backtick
    {
        myVar := 0
        #inline
        {
            `myVar += 1
        }

        @assert(myVar == 1)
    }

    // You can alias an out of scope variable for easy referencing
    {
        myVar := 0
        #inline
        {
            alias myVar = `myVar
            myVar += 1
            myVar += 2
        }

        @assert(myVar == 3)
    }
}</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">var</div><code style="white-space: break-spaces;">// A global variable can be tagged with #[Swag.Tls] to store it
// in the thread local storage (one copy per thread)
#[Swag.Tls]
var G = 0

#test
{
    // A local variable can be tagged with #[Swag.Global] to make it global
    // (aka static in C/C++)
    {
        func toto()->s32
        {
            #[Swag.Global]
            var G1 = 0

            G1 += 1
            return G1
        }

        @assert(toto() == 1)
        @assert(toto() == 2)
        @assert(toto() == 3)
    }
}

// A global variable can also be marked as #[Swag.Compiler]
// That kind of variable will not be exported to the runtime
// and can only be used in compile time code
#[Swag.Compiler]
var G2 = 0

#run
{
    G2 += 5
}</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">error</div><code style="white-space: break-spaces;">// swag contains a *very* simple error system used to deal with function returning errors
// Will probably be changed/improved at some point
// This are *not* exceptions !

// A function that can return an error must be marked with 'throw'
// It can then raise an error with the 'throw' keyword, passing an error message.
// When an error is raised by a function, the return value is always equal to the default value,
// depending on the return type.
func count(name: string)->uint throw
{
    // This function will return 0 in case of an error.
    if name == null
        throw "null pointer"

    return @countof(name)
}

// The caller will then have to deal with the error
// It can 'catch' it, and test (or not) its value with the '@err' intrinsic
// Execution will continue at the call site.
func myFunc()
{
    // Dismiss the eventual error with 'catch'
    cpt := catch count("fileName")

    // And test it with @err, which returns the 'throw' corresponding string
    if @err
    {
        @assert(cpt == 0)
        @print(@err)
        return
    }
}

// The caller can stop the execution with 'try', and return to its own caller with the same error raised.
// The function must then also be marked with 'throw'.
// Here, the caller of 'myFunc1' will have to deal with the error at its turn.
func myFunc1() throw
{
    // If 'count()' raises an error, 'myFunc1' will return with the same error
    cpt := try count("filename")
}

// The caller can also panic if an error is raised, with 'assume'.
// This can be disabled in release builds (in that case the behaviour is undefined).
func myFunc2()
{
    // Here the program will stop with a panic message if 'count()' throws an error
    cpt := assume count("filename")
}

// Note that you can use a block instead of one single statement (this does not create a scope)
func myFunc3() throw
{
    // This is not really necessary, see below, but this is just de show 'try' with a block
    // instead of one single call
    try
    {
        cpt0 := count("filename")
        cpt1 := count("other filename")
    }

    assume
    {
        cpt2 := count("filename")
        cpt3 := count("other filename")
    }

    // Works also for 'catch' if you do not want to deal with the error message.
    // '@err' in that case is not really relevant.
    catch
    {
        cpt4 := count("filename")
        cpt5 := count("other filename")
    }
}

// When a function is marked with "throw", the "try" for a function call is automatic
// if not specified.
// That means that most of the time it's not necessary to specify it.
#test
{
    func mySubFunc2() throw
    {
        throw "error"
    }

    func mySubFunc1() throw
    {
        // In fact there's no need to add a 'try' before the call because 'mySubFunc1' is
        // marked with 'throw'
        // This is less verbose when you do not want to do something special in case
        // of errors (with 'assume' or 'catch')
        mySubFunc2()
    }

    catch mySubFunc1()
    @assert(@err == "error")
}

// 'defer' can have parameters like defer(err) or defer(noerr) to control
// if it should be executed depending on the error status
var g_Defer = 0

func raiseError() throw
{
    throw "error"
}

func testDefer(err: bool) throw
{
    defer(err)      g_Defer += 1    // This will be called in case an error is raised, before exiting
    defer(noerr)    g_Defer += 2    // This will only be called in case an error is not raised
    defer           g_Defer += 3    // This will be called in both cases
    if err
        raiseError()
}

#test
{
    g_Defer = 0
    catch testDefer(true)
    @assert(g_Defer == 4)   // Will call only defer(err) and the normal defer

    g_Defer = 0
    catch testDefer(false)
    @assert(g_Defer == 5)   // Will call only defer(noerr) and the normal defer
}</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">compiler</div><code style="white-space: break-spaces;">// #assert is a static assert (at compile time)
#assert true
#assert(true, "message")

// @defined(SYMBOL) returns true, at compile time, if the given symbol exists
// in the current context
#assert !@defined(DOES_NOT_EXISTS)
#assert @defined(Global)
var Global = 0

// static #if/#else, with an expression that can be evaluated at compile time
const DEBUG = 1
const RELEASE = 0
#if DEBUG
{
}
#elif RELEASE
{
}
#else
{
}

// 'isThisDebug' is marked with 'ConstExpr', so it can be automatically evaluated
// at compile time
#[Swag.ConstExpr]
func isThisDebug() => true

#if isThisDebug() == false
{
    #assert(false, "this should not be called !")
}

// A more complicated #assert
#[Swag.ConstExpr]
func factorial(x: s32)->s32
{
    if x == 1 return 1
    return x * factorial(x - 1)
}

#assert factorial(4) == 24 // Evaluated at compile time</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">functions</div><code style="white-space: break-spaces;">// #test is a special function than can be used in the 'tests/' folder of the workspace.
// They will be executed only if swag is running in test mode.
#test
{
}

// #main is the program entry point
// Can only be defined once per module
#main
{
}

// #init will be called at runtime, during the module initialization
// You can have as many #init as you want
// Order of #init in the same module is undefined.
#init
{
}

// #drop will be called at runtime, when module is unloaded
// You can have as many #drop as you want
// Order of #drop in the same module is undefined (but is always the inverse order of #init)
#drop
{
}

// #premain will be called after all modules have done their #init code,
// but before the #main function is called
#premain
{
}</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">run</div><code style="white-space: break-spaces;">// #run is a special function that will be called at compile time
// It can be used to precompute some global values for example.
var G: [5] f32 = ?

// Initialize G with [1,2,4,8,16] at compile time
#run
{
    value := 1'f32
    loop i: @countof(G)
    {
        G[i] = value
        value *= 2
    }
}

// #test are executed after #run, even at compile time (during testing)
// So we can test the values of G here.
#test
{
    @assert(G[0] == 1)
    @assert(G[1] == 2)
    @assert(G[2] == 4)
    @assert(G[3] == 8)
    @assert(G[4] == 16)
}

// #run can also be used as an expression, to call for example a function not marked with #[Swag.ConstExpr]
const SumValue = #run sum(1, 2, 3, 4) + 10
#assert SumValue == 20

func sum(values: s32...)->s32
{
    result := 0's32
    visit v: values
        result += v
    return result
}

// #run can also be used as an expression block
// The return type is deduced from the 'return' statement
const Value = #run {
    var result: f32
    loop 10
        result += 1
    return result   // 'Value' will be of type 'f32'
}
#assert Value == 10.0</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">global</div><code style="white-space: break-spaces;">#global skip

// A bunch of #global can start a source file

// Skip the content of the file, like this one (but must be a valid swag file)
#global skip

// All symbols in the file will be public/private
#global public
#global private

// All symbols in the file will go in the namespace 'Toto'
#global namespace Toto

// A #if for the whole file
#global if DEBUG == true

// Some attributes can be assigned to the full file
#global #[Swag.Safety("", true)]

// The file will be exported for external usage
// It's like putting everything in public, except that the file will
// be copied in its totality in the public folder
#global export

// Link with a given external library
#foreignlib "windows.lib"
</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">attribute</div><code style="white-space: break-spaces;">using Swag

// Attributes are tags associated with functions, structures etc...
// They are declared like functions, with 'attr' before instead of 'func'
attr AttributeA()

// Attributes can have parameters
attr AttributeB(x, y: s32, z: string)

// Attributes can have default values
attr AttributeBA(x: s32, y: string = "string")

// You can define a usage before the attribute definition to restrict its usage
#[AttrUsage(AttributeUsage.Function)]
attr AttributeC()

// To use an attribute, syntax is #[attribute, attribute...]
#[AttributeA, AttributeB(0, 0, "string")]
func function1()
{
}

// You can declare multiple usages
#[AttrUsage(AttributeUsage.Function | AttributeUsage.Struct)]
attr AttributeD(x: s32);

#[AttributeD(6)]
func function2()
{
}

#[AttributeD(150)]
struct struct1
{
}

#test
{
    // Attributes can be retrieved by reflection
    {
        type := @typeof(function2)
        @assert(@countof(type.attributes) == 1)
    }
}</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">attrlist</div><code style="white-space: break-spaces;">#global skip

// This is the list of predefined attributes
// All are located in the reserved 'Swag' namespace

#[AttrUsage(AttributeUsage.Function)]
attr ConstExpr()

#[AttrUsage(AttributeUsage.Function|AttributeUsage.Struct|AttributeUsage.File)]
attr PrintBc()

#[AttrUsage(AttributeUsage.Function|AttributeUsage.GlobalVariable)]
attr Compiler()

#[AttrUsage(AttributeUsage.Function)]
attr Inline()

#[AttrUsage(AttributeUsage.Function)]
attr Macro()

#[AttrUsage(AttributeUsage.Function)]
attr Mixin()

#[AttrUsage(AttributeUsage.Function)]
attr Test()

#[AttrUsage(AttributeUsage.Function)]
attr Implicit()

// Hardcoded also for switch
#[AttrUsage(AttributeUsage.Function)]
attr Complete()

#[AttrUsage(AttributeUsage.Function)]
attr NoReturn()

#[AttrUsage(AttributeUsage.Function)]
attr Foreign(module: string, function: string = "");

#[AttrUsage(AttributeUsage.Function)]
attr Callback()

#[AttrUsage(AttributeUsage.Function|AttributeUsage.Variable)]
attr Discardable()

#[AttrUsage(AttributeUsage.Function|AttributeUsage.Struct|AttributeUsage.Enum|AttributeUsage.EnumValue)]
attr Deprecated(msg: string = null)

// Hardcoded for type alias
attr Strict()

// Hardcoded for local variables
attr Global()

// Hardcoded for struct and variables
attr Align(value: u8)

#[AttrUsage(AttributeUsage.Struct)]
attr Pack(value: u8)

#[AttrUsage(AttributeUsage.Struct)]
attr NoCopy()

#[AttrUsage(AttributeUsage.StructVariable)]
attr Offset(name: string)

#[AttrUsage(AttributeUsage.Enum)]
attr EnumFlags()

#[AttrUsage(AttributeUsage.Enum)]
attr EnumIndex()

#[AttrUsage(AttributeUsage.All|AttributeUsage.File)]
attr Safety(what: string, value: bool)

#[AttrUsage(AttributeUsage.All)]
attr SelectIf(value: bool)

#[AttrUsage(AttributeUsage.Function|AttributeUsage.File)]
attr Optim(what: string, value: bool)
</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">reflection</div><code style="white-space: break-spaces;">#test
{
    // You can get the type of an expression with '@typeof' or just with the type itself
    // (in Swag types are values)
    {
        ptr1 := @typeof(s8)
        @assert(ptr1.name == "s8")
        @assert(ptr1 == s8)

        ptr2 := @typeof(s16)
        @assert(ptr2.name == "s16")
        @assert(ptr2 == s16)

        ptr3 := s32
        @assert(ptr3.name == "s32")
        @assert(ptr3 == @typeof(s32))

        ptr4 := s64
        @assert(ptr4.name == "s64")
        @assert(ptr4 == s64)
    }

    // The return result of '@typeof' is a const pointer to a Swag.TypeInfo kind of structure
    // This is an alias for the 'typeinfo' type.
    {
        ptr := bool
        @assert(@typeof(ptr) == @typeof(const *Swag.TypeInfoNative))

        ptr1 := [2] s32
        @assert(@typeof(ptr1) == @typeof(const *Swag.TypeInfoArray))
        @assert(ptr1.name == "[2] s32")
    }

    // The TypeInfo structure contains a different enum value for each type
    {
        ptr := f64
        @assert(@typeof(ptr.kind).fullname == "Swag.TypeInfoKind")
        @assert(ptr.sizeof == @sizeof(f64))
    }
}
</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">ast</div><code style="white-space: break-spaces;">#test
{
    // '#ast' is executed at compile time and returns a string that will be compiled inplace
    {
        #ast "x := 666"
        @assert(x == 666)
    }

    {
        cpt := 2
        #ast
        {
            const INC = 5
            return "cpt += " ++ INC   // Equivalent to 'cpt += 5'
        }

        @assert(cpt == 7)
    }

    // '#ast' can be used to generate the content of a struct/enum
    {
        struct MyStruct
        {
            #ast
            {
                return "x, y: s32 = 666"
            }
        }

        var v: MyStruct
        @assert(v.x == 666)
        @assert(v.y == 666)
    }

    // Works with generics too, and can be mixed with static declarations
    {
        struct(T) MyStruct
        {
            #ast
            {
                return "x, y: " ++ @typeof(T).name
            }

            z: string
        }

        var v: MyStruct'bool
        #assert @typeof(v.x) == bool
        #assert @typeof(v.y) == bool
        #assert @typeof(v.z) == string

        var v1: MyStruct'f64
        #assert @typeof(v1.x) == f64
        #assert @typeof(v1.y) == f64
        #assert @typeof(v1.z) == string
    }

    // '#ast' needs to return a 'string like' value, that can be dynamic
    {
        func append(buf: ^u8, val: string)
        {
            len := 0
            while buf[len] len += 1
            @memcpy(buf + len, @dataof(val), cast(uint) @countof(val) + 1)
        }

        struct Vector3
        {
            #ast
            {
                var buf: [256] u8
                append(buf, "x: f32 = 1\n")
                append(buf, "y: f32 = 2\n")
                append(buf, "z: f32 = 3\n")
                return cast(string) buf
            }
        }

        var v: Vector3
        @assert(v.x == 1)
        @assert(v.y == 2)
        @assert(v.z == 3)
    }
}

// #ast can also be called on the global scope
#ast
{
    const value = 666
    return "const myGeneratedConst = " ++ value
}

// You must use '#placeholder' in case you are generating symbols that can be
// used by something else in the code.
#placeholder myGeneratedConst

// Here the #assert will wait for the symbol 'myGeneratedConst' to be replaced with its
// real content.
#assert myGeneratedConst == 666</code></div><div class="mb-10"><div class="text-2xl mb-5 border-b">message</div><code style="white-space: break-spaces;">// '#message' is a special function that will be called by the compiler
// The parameter is a mask that tells the compiler when to call the function.

// Here the function will be called each time a function of the module has been typed
#message(Swag.CompilerMsgMask.SemFunctions)
{
    itf := @compiler()          // Get the interface to communicate with the compiler
    msg := itf.getMessage()     // Get the current message

    // We know the type is a function because of the '#message' mask
    typeFunc := cast(const *Swag.TypeInfoFunc) msg.type
    nameFunc := msg.name

    // For example, we register the function name if it starts with "XX".
    // But we could use custom function attributes instead...
    if @countof(nameFunc) > 2 and nameFunc[0] == "X"'u8 and nameFunc[1] == "X"'u8
        G += 1
}

// The compiler will call the following function after the semantic pass
// So after all the functions of the module have been parsed
#message(Swag.CompilerMsgMask.PassAfterSemantic)
{
    // We should have found 3 functions starting with "XX"
    @assert(G == 3)
}

var G = 0

func XXTestFunc1() {}
func XXTestFunc2() {}
func XXTestFunc3() {}

// This will be called for every global variables of the module
#message(Swag.CompilerMsgMask.SemGlobals)
{
    itf := @compiler()
    msg := itf.getMessage()
}

// This will be called for every global types of the module (structs, enums, interfaces...)
#message(Swag.CompilerMsgMask.SemTypes)
{
    itf := @compiler()
    msg := itf.getMessage()
}</code></div></div><?php include('html_end.php'); ?>