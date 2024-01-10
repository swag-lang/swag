#global skip
struct(T) Array
{
    buf: ^T
}

impl Array
{
    mtd add(x: T) {}
    mtd opCast()->const [..] T
    {
        return @mkslice(buf,1)
    }
}

func(T, R) map(values: const [..] T, cb: func(T)->R)->Array'R
{
    var result: retval
    visit v: values do
        result.add(cb(v))
    return result
}

#test
{
    let a: Array's32

    const A = [0, 1, 0, 0, 1]

    let res = map'(s32,bool)(A[..], func(x) => x != 0)
}