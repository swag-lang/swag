struct Vector3 { x, y, z: s32 = 666 }

#test
{
    func assign(toI: &Vector3, from: Vector3)
    {
        toI = from
    }

    var a  = Vector3{1, 2, 3}
    var b: Vector3

    // The 'copy' version
    assign(&b, a)
    @assert(a.x == 1 and a.y == 2 and a.z == 3)
    @assert(b.x == 1 and b.y == 2 and b.z == 3)
}
