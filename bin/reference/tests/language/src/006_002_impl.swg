/**
# Struct Methods and Constants

In Swag, structs can encapsulate methods and constants within them using the 'impl' block.
This encapsulation allows for better organization and modularity of the code, keeping
the functionality related to the struct within the struct itself.
*/

#[Swag.ExportType("methods")]           // This attribute enables method reflection for this struct type
struct MyStruct
{
    x:     s32 = 5   // Field 'x' with a default value of 5
    y:     s32 = 10  // Field 'y' with a default value of 10
    z:     s32 = 20  // Field 'z' with a default value of 20
}

impl MyStruct
{
    const MyConst = true                // Constant defined within the struct's scope

    func returnTrue() => true           // Function defined within the struct's scope, returns 'true'
}

/**
To access the constant or the function, use the 'MyStruct' namespace.
*/
#test
{
    @assert(MyStruct.MyConst)           // Accessing the constant 'MyConst' within the struct's scope
    @assert(MyStruct.returnTrue())      // Invoking the function 'returnTrue' within the struct's scope
}

/**
# Multiple 'impl' Blocks

Swag allows multiple 'impl' blocks for a single struct. This feature helps organize
methods logically. Within an 'impl' block, 'me' are defined to refer to
the current instance and the struct type, respectively.
*/

impl MyStruct
{
    // 'me' is implicitly defined as 'var me: MyStruct'
    func returnX(using me) => x         // Method to return 'x', using 'me' implicitly

    func returnY(me)       => me.y      // Method to return 'y', explicitly using 'me'

    func returnZ(self: MyStruct) => self.z  // Method to return 'z'
}

/**
# Method Syntax Sugar

Swag provides syntactic sugar for method definitions. If you use 'mtd' (method) instead
of 'func', the first parameter is implicitly 'me'. If you use 'mtd const',
it becomes 'const me'. This feature simplifies method definitions,
especially for common cases where 'me' is required.
*/

impl MyStruct
{
    mtd methodReturnX()  => me.x  // Equivalent to 'func methodReturnX(using me) => x'
    func funcReturnX(me) => me.x  // Using 'me' to return 'x'
}

#test
{
    var c: MyStruct
    @assert(c.returnX() == 5)          // Calling 'returnX' method, expected to return 'x'
    @assert(c.methodReturnX() == 5)    // Using the 'mtd' syntax, expected to return 'x'
    @assert(c.funcReturnX() == 5)      // Explicitly using 'func' syntax, expected to return 'x'
    @assert(c.returnY() == 10)         // Calling 'returnY' method, expected to return 'y'
    @assert(c.returnZ() == 20)         // Calling 'returnZ' method, expected to return 'z'
}

/**
# Method Reflection

To enable reflection on methods within an 'impl' block, the struct must be annotated with
'#[Swag.ExportType("methods")]'. By default, methods are not exported for reflection,
but with this annotation, they can be accessed programmatically.
*/
#test
{
    // Define a type alias 'Lambda' for a function pointer taking 'MyStruct' and returning 's32'
    alias Lambda = func(MyStruct)->s32

    var fnX: Lambda  // Function pointer for 'returnX'
    var fnY: Lambda  // Function pointer for 'returnY'
    var fnZ: Lambda  // Function pointer for 'returnZ'

    // The 'typeinfo' of a struct contains a 'methods' field, which is an array of method pointers
    let t = MyStruct
    foreach p in t.methods
    {
        // The 'value' field of 'methods' contains the function pointer, which we cast to the correct type
        switch p.name
        {
        case "returnX":
            fnX = cast(Lambda) p.value  // Cast the method pointer to 'Lambda' and assign to 'fnX'
        case "returnY":
            fnY = cast(Lambda) p.value  // Cast the method pointer to 'Lambda' and assign to 'fnY'
        case "returnZ":
            fnZ = cast(Lambda) p.value  // Cast the method pointer to 'Lambda' and assign to 'fnZ'
        }
    }

    // The function pointers 'fnX', 'fnY', and 'fnZ' are now callable
    var v: MyStruct
    @assert(fnX(v) == 5)   // Calling the function pointer 'fnX', expected to return 5
    @assert(fnY(v) == 10)  // Calling the function pointer 'fnY', expected to return 10
    @assert(fnZ(v) == 20)  // Calling the function pointer 'fnZ', expected to return 20
}
