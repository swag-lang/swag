
using Core, Core.File

#test
{
    const TXT = "a 666\nb $a"
    var tf: TweakFile

    struct A { a, b: s32; }

    var v: A
    tf.registerFolder(&v)
    try tf.parse(TXT)

    @assert(v.a == 666)
    @assert(v.b == 666)
}
