/**
Interfaces are **virtual tables** (a list of function pointers) that can be associated to a struct.

Unlike C++, the virtual table is not embedded with the struct. It is a separate object.
You can then *implement* an interface for a given struct without changing the struct definition.
*/

struct Point2
{
    x, y: f32
}

struct Point3
{
    x, y, z: f32
}

/**
Here we declare an interface, with two functions `set` and `reset`.
*/
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

/**
You can implement an interface for any given struct with `impl` and `for`.
For example here, we implement interface `IReset` for struct `Point2`.
*/

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

/**
And we implement interface `IReset` also for struct `Point3`.
*/
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
}