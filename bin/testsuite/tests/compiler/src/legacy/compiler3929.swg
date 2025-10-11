var g: func||(s32)

#test
{
    #[Swag.Mixin]
    func toto()
    {
        g = func(btn) {}
    }

    toto()
}
