/**
Like for an enum, `impl` is used to declare functions and constants within the scope of a struct.
*/

#[Swag.ExportType("methods")] // This attribute exports the methods for type reflection
struct MyStruct
{
    x:     s32 = 5   // Field 'x' with default value 5
    y:     s32 = 10  // Field 'y' with default value 10
    z:     s32 = 20  // Field 'z' with default value 20
}

impl MyStruct
{
    // Declare a constant within the struct's scope
    const MyConst = true

    // Declare a function within the struct's scope
    func returnTrue() => true
}

/**
To access the constant and the function, use the `MyStruct` namespace.
*/
#test
{
    @assert(MyStruct.MyConst)           // Accessing the constant within the struct's scope
    @assert(MyStruct.returnTrue())      // Calling the function within the struct's scope
}

/**
You can have multiple `impl` blocks for a struct. The difference between an `impl` block and a namespace is that `self` and `Self` are defined within an `impl` block. They refer to the corresponding type.
*/

impl MyStruct
{
    // 'self' is an alias for 'var self: Self', where 'Self' refers to 'MyStruct'
    func returnX(using self) => x       // Accessing 'x' through 'self'
    func returnY(self)       => self.y  // Accessing 'y' using 'self' explicitly

    // 'Self' refers to the type 'MyStruct'
    func returnZ(me: Self) => me.z      // Accessing 'z' using 'Self' as the type alias
}

/**
If you declare your function with `mtd` (method) instead of `func`, the first parameter is implicitly `using self`.
If you declare your function with `mtd const` (method const) instead of `func`, the first parameter is implicitly `const using self`.
This is **syntactic sugar** to avoid explicitly writing `using self`.
*/

impl MyStruct
{
    mtd methodReturnX()          => x  // Equivalent to 'func methodReturnX(using self) => x'
    func funcReturnX(using self) => x  // Explicitly using 'self'
}

#test
{
    var c: MyStruct
    @assert(c.returnX() == 5)          // Calling the method to return 'x'
    @assert(c.methodReturnX() == 5)    // Using the 'mtd' syntax to return 'x'
    @assert(c.funcReturnX() == 5)      // Explicitly calling with 'func' syntax
    @assert(c.returnY() == 10)         // Calling the method to return 'y'
    @assert(c.returnZ() == 20)         // Calling the method to return 'z'
}

/**
All functions in an `impl` block can be accessed via reflection, provided the struct is declared with `#[Swag.ExportType("methods")]` (by default, methods are not exported).
*/
#test
{
    // Create a type alias named 'Lambda' for a function pointer
    typealias Lambda = func(MyStruct)->s32

    var fnX: Lambda  // Function pointer for 'returnX'
    var fnY: Lambda  // Function pointer for 'returnY'
    var fnZ: Lambda  // Function pointer for 'returnZ'

    // The 'typeinfo' of a struct contains a 'methods' field, which is a slice of methods
    let t = MyStruct
    visit p in t.methods
    {
        // When visiting 'methods', the 'value' field contains the function pointer,
        // which can be cast to the correct type
        switch p.name
        {
        case "returnX":
            fnX = cast(Lambda) p.value  // Casting the function pointer to 'Lambda'
        case "returnY":
            fnY = cast(Lambda) p.value  // Casting the function pointer to 'Lambda'
        case "returnZ":
            fnZ = cast(Lambda) p.value  // Casting the function pointer to 'Lambda'
        }
    }

    // These are now valid functions that can be called
    var v: MyStruct
    @assert(fnX(v) == 5)   // Calling the function pointer 'fnX'
    @assert(fnY(v) == 10)  // Calling the function pointer 'fnY'
    @assert(fnZ(v) == 20)  // Calling the function pointer 'fnZ'
}
