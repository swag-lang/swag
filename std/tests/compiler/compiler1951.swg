struct(A) Foozle
{
}

impl Foozle
{
    func add(self, x: A)
    {
        @print("add")
    }

    /*func(G) add(self, x: G)
    {
        //@print("add1")
        var t: A = x
        self.add(t)
    }*/
}

#test
{
    //var fooz: Foozle's32
    //fooz.add(0'f32)
}