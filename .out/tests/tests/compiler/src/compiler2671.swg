#global skip
struct AA
{
    x: f64 = 1
    y: f64 = 2
    z: f64 = 3
}

/*#[swag.printbc]
func toto(x: AA)
{
    //@print(x.x)
    v := @{x}
    @print(v.item0.x)
    //@print(v.item0.y)
    //@print(v.item0.z)
    //@assert(@sizeof(v) == 3 * @sizeof(f64))
}
*/

#test
{
    var v: AA
    var ref: const&AA = v
    @assert(ref.x == 1)
    @assert(ref.y == 2)
    @assert(ref.z == 3)
    //toto(v)
}