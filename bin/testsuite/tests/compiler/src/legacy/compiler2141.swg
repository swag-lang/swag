// A simple point with 2 float coordinates X and Y
struct(T) PointBase { x, y: T }

impl PointBase
{
    func(op: string) opAssign(me, value: T)
    {
        #if op == "+="
        {
            me.x += value; me.y += value; }
        #elif op == "-="
        {
            me.x -= value; me.y -= value; }
        #elif op == "*="
        {
            me.x *= value; me.y *= value; }
        #else do
            #error("unsupported assign operation '" ++ op ++ "'")
    }

    func(op: string) opBinary(me, other: PointBase)->PointBase
    {
        #if op == "+" do
            return PointBase{me.x + other.x, me.y + other.y}
        #elif op == "-" do
            return PointBase{me.x - other.x, me.y - other.y}
        #elif op == "*" do
            return PointBase{me.x * other.x, me.y * other.y}
        #else do
            #error("unsupported binary operation '" ++ op ++ "'")
    }

    // Offset this Point by a given value
    #[Swag.Overload]
    func offset(me, value: T)
    {
        me.x, me.y += value
    }

    // Offset this Point by a given value
    #[Swag.Overload]
    func offset(me, valueX, valueY: T)
    {
        me.x += valueX
        me.y += valueY
    }
}

alias Point2141 = PointBase'f32

#test
{
    var pt0, pt1, pt2: Point2141
    pt1.x = 1; pt1.y = 1
    pt2.x = 0.5; pt2.y = 1.5
    pt0   = pt1 + pt2
    @assert(pt0.x == 1.5)
    @assert(pt0.y == 2.5)
}
