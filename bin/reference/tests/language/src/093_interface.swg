struct Point2
{
    x, y: f32
}

struct Point3
{
    x, y, z: f32
}

// Interfaces are virtual tables that can be associated to a struct
interface IReset
{
    set: func(self, f32)
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
}

#test
{
    var pt2: Point2
    var pt3: Point3

    // To get the interface associated to a given struct, use the 'cast' operator
    itf := cast(IReset) pt2
    itf.set(10)
    @assert(pt2.x == 10)
    @assert(pt2.y == 10+1)

    itf = cast(IReset) pt3
    itf.set(10)
    @assert(pt3.x == 10)
    @assert(pt3.y == 10+1)
    @assert(pt3.z == 10+2)

    // An interface is a real type, with a size
    #assert @sizeof(itf) == 2 * @sizeof(*void)

    // You can retrieve the concrete type associated with an interface with '@kindof'
    itf = cast(IReset) pt2
    @assert(@kindof(itf) == Point2)
    itf = cast(IReset) pt3
    @assert(@kindof(itf) == Point3)

    // You can retrieve the concrete data associated with an interface with '@dataof'
    itf = cast(IReset) pt2
    @assert(@dataof(itf) == &pt2)
    itf = cast(IReset) pt3
    @assert(@dataof(itf) == &pt3)
}