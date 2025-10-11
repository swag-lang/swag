#global testerror "Err0484"
struct A
{
    fn:  func(*A)
}

impl A
{
    func toto(me)
    {
        //   fn = null
        //if me.fn:
        fn(me)
    }
}

#test
{
    var a: A
    a.toto()
}
