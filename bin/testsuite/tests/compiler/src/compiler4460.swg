struct A { x: f32; }
const V: [] A = [{1.0}]

func toto(v: [..] A)
{
}

#test
{
    a := V
    toto(a)
}