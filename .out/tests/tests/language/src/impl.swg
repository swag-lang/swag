// 'impl' is used to declare some stuff in the scope of a struct/enum
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
    func returnY(self) => self.y
    // 'Self' is the corresponding type
    func returnZ(me: Self) => me.z
}

#test
{
    var c: MyStruct
    @assert(c.returnX() == 5)
    @assert(c.returnY() == 10)
    @assert(c.returnZ() == 20)
}

// 'impl' can also be used for an enum
enum RGB { R; G; B; }
impl enum RGB
{
    func isRed(self) => self == R
    func isRedOrBlue(self) => self == R || self == B
}

#test
{
    @assert(RGB.isRed(RGB.R))
    @assert(RGB.isRedOrBlue(RGB.B))
    using RGB
    @assert(isRedOrBlue(R))
    @assert(isRedOrBlue(B))
    @assert(R.isRedOrBlue())
    @assert(!RGB.G.isRedOrBlue())
}