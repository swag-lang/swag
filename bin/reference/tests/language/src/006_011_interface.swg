/**
Interfaces in Swag are virtual tables (lists of function pointers) that can be associated with a struct.

Unlike C++, the virtual table is not embedded in the struct; it is a separate object.
This lets you implement an interface for a struct without changing the struct.
*/

struct Point2 { x, y: f32 }

struct Point3 { x, y, z: f32 }

/**
# Interface Declaration
Declare interface 'IReset' with two functions 'set' and 'reset'.
*/
interface IReset
{
    // First parameter must be 'me' when using 'func'
    func set(me, val: f32);

    // 'mtd' avoids specifying 'me'
    mtd reset();
}

/**
# Implementing an Interface
Implement interface 'IReset' for 'Point2'.
*/
impl IReset for Point2
{
    // Mark with 'impl' to implement an interface function
    mtd impl set(val: f32)
    {
        me.x = val
        me.y = val + 1
    }

    // 'mtd' is sugar; 'func' also works
    func impl reset(me)
    {
        me.x, me.y = 0
    }

    // Regular methods may also appear in this 'impl'
    mtd myOtherMethod() {}
}

/**
# Implementing the Interface for Another Struct
Implement 'IReset' for 'Point3'.
*/
impl IReset for Point3
{
    mtd impl set(val: f32)
    {
        me.x = val
        me.y = val + 1
        me.z = val + 2
    }

    mtd impl reset()
    {
        me.x, me.y, me.z = 0
    }
}

/**
# Using the Interface
Cast to the interface and call its methods.
*/
#test
{
    var pt2: Point2
    var pt3: Point3

    var itf = cast(IReset) pt2
    itf.set(10)
    @assert(pt2.x == 10)
    @assert(pt2.y == 11)

    itf = cast(IReset) pt3
    itf.set(10)
    @assert(pt3.x == 10)
    @assert(pt3.y == 11)
    @assert(pt3.z == 12)
    itf.reset()
    @assert(pt3.x == 0 and pt3.y == 0 and pt3.z == 0)
}

/**
# Accessing Interface Methods Directly
Implementation functions live under a scope named after the interface.
*/
#test
{
    var pt2: Point2
    var pt3: Point3

    pt2.IReset.set(10)
    pt2.IReset.reset()
    pt3.IReset.set(10)
    pt3.IReset.reset()
}

/**
# Interface as a Type
An interface occupies two pointers: object pointer + virtual table pointer.
*/
#test
{
    var pt2: Point2
    var pt3: Point3
    var itf  = cast(IReset) pt2

    #assert(#sizeof(itf) == 2 * #sizeof(*void))

    // Retrieve the concrete type with '@kindof'
    itf = cast(IReset) pt2
    @assert(@kindof(itf) == Point2)
    itf = cast(IReset) pt3
    @assert(@kindof(itf) == Point3)

    // Retrieve the concrete data with '@dataof'
    itf = cast(IReset) pt2
    @assert(@dataof(itf) == &pt2)
    itf = cast(IReset) pt3
    @assert(@dataof(itf) == &pt3)
}

/**
# Default Implementation in Interfaces
Provide default bodies directly in the interface. If a struct does not override,
the default is used.
*/
interface ITest
{
    mtd isImplemented()->bool
    {
        return false
    }
}

/**
Override on 'Point2'; no override on 'Point3'.
*/
impl ITest for Point2
{
    mtd impl isImplemented()->bool
    {
        return true
    }
}

impl ITest for Point3
{
}

/**
For 'Point3', 'isImplemented()' returns 'false' (the default).
*/
#test
{
    var v2: Point2
    var v3: Point3

    let i2 = cast(ITest) v2
    @assert(i2.isImplemented())

    let i3 = cast(ITest) v3
    @assert(!i3.isImplemented())
}
