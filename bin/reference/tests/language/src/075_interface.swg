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
    // The first parameter must be 'self'
    func set(self, val: f32)

    // You can also use the 'mtd' declaration to avoid specifying the 'self' yourself
    mtd reset()
}

/**
You can implement an interface for any given struct with `impl` and `for`.
For example here, we implement interface `IReset` for struct `Point2`.
*/

impl IReset for Point2
{
    // You must add 'impl' to indicate that you want to implement a function of the interface.
    mtd impl set(val: f32)
    {
        x = val
        y = val+1
    }

    // Don't forget that 'mtd' is just syntaxic sugar. 'func' still works.
    func impl reset(self)
    {
        self.x, self.y = 0
    }

    // Not that you can also declare 'normal' functions or methods in an 'impl block'.
    mtd myOtherMethod()
    {
    }
}

/**
And we implement interface `IReset` also for struct `Point3`.
*/
impl IReset for Point3
{
    mtd impl set(val: f32)
    {
        x = val
        y = val+1
        z = val+2
    }

    mtd impl reset()
    {
        x, y, z = 0
    }
}

/**
We can then use these interfaces on either `Vector2` or `Vector3`.
*/
#test
{
    var pt2: Point2
    var pt3: Point3

    // To get the interface associated to a given struct, use the 'cast' operator.
    // If the compiler does not find the corresponding implementation, it will raise an error.
    var itf = cast(IReset) pt2
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
}

/**
You can also access, with a normal call, all functions declared in an interface implementation block for a given struct.
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
An interface is a real type, with a size equivalent to 2 pointers. A pointer to the *object* and a pointer to the *virtual table*.
*/
#test
{
    var pt2: Point2
    var pt3: Point3
    var itf = cast(IReset) pt2
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