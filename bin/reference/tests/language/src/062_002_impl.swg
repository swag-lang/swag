/**
Like for an enum, `impl` is used to declare some stuff in the scope of a struct.
*/

#[Swag.ExportType("methods")] // See later, used to export 'methods' in type reflection
struct MyStruct
{
    x:     s32 = 5
    y:     s32 = 10
    z:     s32 = 20
}

impl MyStruct
{
    // Declare a constant in the scope of the struct
    const MyConst = true

    // Declare a function in the scope of the struct
    func returnTrue() => true
}

/**
So to access the constant and the function, you have to use the `MyStruct` namespace.
*/
#test
{
    @assert(MyStruct.MyConst)
    @assert(MyStruct.returnTrue())
}

/**
You can have multiple `impl` blocks. The difference with a namespace is that `self` and `Self` are defined inside an `impl` block. They refere to the corresponding type.
*/

impl MyStruct
{
    // 'self' is an alias for 'var self: Self'
    func returnX(using self) => x
    func returnY(self)       => self.y

    // 'Self' is the corresponding type, in that case 'MyStruct'
    func returnZ(me: Self) => me.z
}

/**
If you declare your function with `mtd` (method) instead of `func`, then the first parameter is forced to be `using self`.
If you declare your function with `mtd const` (method const) instead of `func`, then the first parameter is forced to be `const using self`.
Other than that, it's exactly the same. So this is just **syntaxic sugar** to avoid repeating the `using self`.
*/

impl MyStruct
{
    mtd methodReturnX()          => x
    func funcReturnX(using self) => x
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
All functions in an `impl` block can be retrieved by reflection, as long as the struct is declared with `#[Swag.ExportType("methods")]` (by default, methods are not exported).
*/
#test
{
    // Creates a type alias named 'Lambda'
    typealias Lambda = func(MyStruct)->s32

    var fnX: Lambda
    var fnY: Lambda
    var fnZ: Lambda

    // The 'typeinfo' of a struct contains a field 'methods' which is a slice
    let t = MyStruct
    visit p: t.methods
    {
        // When visiting 'methods', the 'value' field contains the function pointer,
        // which can be casted to the correct type
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

    // These are now valid functions, which can be called
    var v: MyStruct
    @assert(fnX(v) == 5)
    @assert(fnY(v) == 10)
    @assert(fnZ(v) == 20)
}
