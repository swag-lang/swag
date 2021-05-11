enum A
{
    B
}

impl enum A
{
    #[swag.constexpr]
    func toto(self) => 6
}

func(T: A) titi()
{
    x := T.toto()
}

#test
{
    titi'A.B()
}