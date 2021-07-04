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

// If you declare your function with 'method' instead of 'func', then
// the first parameter is automatically 'using self'. Other than that,
// it's exactly the same.
// So this is just syntaxic sugar to avoid repeating the 'using self'.

impl MyStruct
{
    method methodReturnX()          => x
    func   funcReturnX(using self)  => x
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

// All functions in an impl block can be retreived by reflection
#test
{
    alias Lambda = func(MyStruct)->s32
    var fnX: Lambda
    var fnY: Lambda
    var fnZ: Lambda

    t := @typeof(MyStruct)
    visit p: t.methods
    {
        // When visiting 'methods', the 'value' field contains the function pointer,
        // that can be casted to the correct type
        switch p.name
        {
        case "returnX": fnX = cast(Lambda) p.value
        case "returnY": fnY = cast(Lambda) p.value
        case "returnZ": fnZ = cast(Lambda) p.value
        }
    }

    // This are now valid functions, that can be called
    var v: MyStruct
    @assert(fnX(v) == 5)
    @assert(fnY(v) == 10)
    @assert(fnZ(v) == 20)
}

// 'impl' can also be used for an enum
enum RGB { R; G; B; }
impl enum RGB
{
    func isRed(self) => self == R
    func isRedOrBlue(self) => self == R or self == B
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