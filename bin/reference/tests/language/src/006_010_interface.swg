/**
Interfaces in Swag are **virtual tables** (a list of function pointers) that can be associated with a struct.

Unlike C++, the virtual table is not embedded within the struct. It is a **separate** object.
This allows for implementing an interface for a given struct without altering the struct's definition.
*/

struct Point2
{
    x, y: f32  // Represents the coordinates of a point in a 2D space
}

struct Point3
{
    x, y, z: f32  // Represents the coordinates of a point in a 3D space
}

/**
# Interface Declaration
Here we declare an interface 'IReset', with two functions `set` and `reset`.
*/
interface IReset
{
    // The first parameter must be 'self'
    func set(self, val: f32);

    // You can also use the 'mtd' declaration to avoid specifying the 'self' yourself
    mtd reset();
}

/**
# Implementing an Interface
You can implement an interface for any given struct with `impl` and `for`.
For example, here we implement interface `IReset` for struct `Point2`.
*/

impl IReset for Point2
{
    // You must add 'impl' to indicate that you want to implement a function of the interface.
    mtd impl set(val: f32)
    {
        x = val              // Set x to the given value
        y = val + 1          // Set y to the value incremented by 1
    }

    // Don't forget that 'mtd' is just syntactic sugar. 'func' still works.
    func impl reset(self)
    {
        self.x, self.y = 0   // Reset x and y to 0
    }

    // Note that you can also declare 'normal' functions or methods in an 'impl' block.
    mtd myOtherMethod() {}   // Example of an additional method
}

/**
# Implementing the Interface for Another Struct
Similarly, we implement the `IReset` interface for struct `Point3`.
*/
impl IReset for Point3
{
    mtd impl set(val: f32)
    {
        x = val              // Set x to the given value
        y = val + 1          // Set y to the value incremented by 1
        z = val + 2          // Set z to the value incremented by 2
    }

    mtd impl reset()
    {
        x, y, z = 0          // Reset x, y, and z to 0
    }
}

/**
# Using the Interface
We can then use these interfaces on either `Point2` or `Point3`.
*/
#test
{
    var pt2: Point2
    var pt3: Point3

    // To get the interface associated with a given struct, use the 'cast' operator.
    // If the compiler does not find the corresponding implementation, it will raise an error.
    var itf = cast(IReset) pt2
    itf.set(10)
    @assert(pt2.x == 10)
    @assert(pt2.y == 10 + 1)

    itf = cast(IReset) pt3
    itf.set(10)
    @assert(pt3.x == 10)
    @assert(pt3.y == 10 + 1)
    @assert(pt3.z == 10 + 2)
    itf.reset()
    @assert(pt3.x == 0 and pt3.y == 0 and pt3.z == 0)
}

/**
# Accessing Interface Methods Directly
You can also access all functions declared in an interface implementation block for a given struct with a normal call.
They are located in a dedicated scope.
*/
#test
{
    var pt2: Point2
    var pt3: Point3

    // The scope where all functions are located has the same name as the interface.
    pt2.IReset.set(10)
    pt2.IReset.reset()
    pt3.IReset.set(10)
    pt3.IReset.reset()
}

/**
# Interface as a Type
An interface is a real type, with a size equivalent to 2 pointers: a pointer to the *object* and a pointer to the *virtual table*.
*/
#test
{
    var pt2: Point2
    var pt3: Point3
    var itf  = cast(IReset) pt2

    #assert @sizeof(itf) == 2 * @sizeof(*void)

    // You can retrieve the concrete type associated with an interface instance with '@kindof'.
    itf = cast(IReset) pt2
    @assert(@kindof(itf) == Point2)
    itf = cast(IReset) pt3
    @assert(@kindof(itf) == Point3)

    // You can retrieve the concrete data associated with an interface instance with '@dataof'
    itf = cast(IReset) pt2
    @assert(@dataof(itf) == &pt2)
    itf = cast(IReset) pt3
    @assert(@dataof(itf) == &pt3)
}

/**
# Default Implementation in Interfaces
When you declare an interface, you can define a default implementation for each function. 
If a struct does not redefine the function, then the default implementation will be called instead.
*/

/**
Just declare a body in the interface function to provide a default implementation.
*/
interface ITest
{
    mtd isImplemented()->bool { return false; }  // Default implementation returns false
}

/**
Here we define a specific version of 'isImplemented' for `Point2`, and no specific implementation for `Point3`.
*/
impl ITest for Point2
{
    mtd impl isImplemented()->bool { return true; }  // Override to return true for Point2
}

impl ITest for Point3
{
    // No override, so the default implementation will be used.
}

/**
For `Point3`, `isImplemented()` will return `false` because this is the default implementation.
*/
#test
{
    var v2: Point2
    var v3: Point3

    // 'isImplemented' has been redefined, and will return 'true' for Point2.
    let i2 = cast(ITest) v2
    @assert(i2.isImplemented())

    // 'isImplemented' is not redefined, it will return false for Point3.
    let i3 = cast(ITest) v3
    @assert(!i3.isImplemented())
}
