struct S
{
    down:   s32 = 10
    up:     s32 = 20
}

impl S
{
    #[swag.macro]
    func opVisit(using self, stmt: code)
    {
        x := down
        @assert(x == 10)
    }
}

#test
{
    var x: S    
    visit x
    {
        @print("AAA\n")
    }   
}