
#global testpass syntax

struct S
{
    down:   s32 = 10
    up:     s32 = 20
}

impl S
{
    #[Swag.Macro]
    func opVisit(using self, stmt: code)
    {
        for var x = down; x <= up; x += 1
        {
            #inject stmt
        }
    }
}

#test
{
    var x: S
    foreach i, j in x
    {
        @print("AAA\n")
    }
}