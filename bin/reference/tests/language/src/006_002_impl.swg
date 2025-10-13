/**
# Struct Methods and Constants

In Swag, structs can encapsulate methods and constants within them using the 'impl' block.
This keeps related functionality close to the data it operates on.
*/

#[Swag.ExportType(.Methods)] // Enable method reflection for this struct type
struct MyStruct
{
    x:     s32 = 5      // Default 5
    y:     s32 = 10     // Default 10
    z:     s32 = 20     // Default 20
}

impl MyStruct
{
    const MyConst = true // Constant in the struct's namespace

    func returnTrue() => true
}

/**
To access the constant or the function, use the 'MyStruct' namespace.
*/
#test
{
    @assert(MyStruct.MyConst)
    @assert(MyStruct.returnTrue())
}

/**
# Multiple 'impl' Blocks

Swag allows multiple 'impl' blocks for the same struct. Inside methods,
'me' refers to the current instance. You can also name the receiver explicitly.
*/

impl MyStruct
{
    // 'me' is implicitly 'var me: MyStruct'
    func returnX(me) => me.x
    func returnY(me) => me.y

    // Explicit receiver name and type
    func returnZ(self: MyStruct) => self.z
}

/**
# Method Syntax Sugar

Using 'mtd' makes the first parameter implicitly 'me'.
Using 'mtd const' makes it 'const me'. This simplifies common patterns.
*/

impl MyStruct
{
    mtd methodReturnX()  => me.x // Equivalent to 'func methodReturnX(me) => me.x'
    func funcReturnX(me) => me.x
    func funcReturnY(me) => .x // 'me' can be omitted
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

/**
# Method Reflection

To enable reflection on methods in an 'impl' block, annotate the struct with
'#[Swag.ExportType(.Methods)]'. The 'typeinfo' for the struct then exposes
a 'methods' array that you can traverse to retrieve function pointers.
*/
#test
{
    // Type alias for a function pointer taking 'MyStruct' and returning 's32'
    alias Lambda = func(MyStruct)->s32

    var fnX: Lambda
    var fnY: Lambda
    var fnZ: Lambda

    let t = MyStruct
    foreach p in t.methods
    {
        // 'p.value' is the function pointer; cast it to the expected type
        switch p.name
        {
        case "returnX":
            fnX = cast(Lambda) p.value
        case "returnY":
            fnY = cast(Lambda) p.value
        case "returnZ":
            fnZ = cast(Lambda) p.value
        }
    }

    let v: MyStruct
    @assert(fnX(v) == 5)
    @assert(fnY(v) == 10)
    @assert(fnZ(v) == 20)
}
