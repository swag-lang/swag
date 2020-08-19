#module "compiler1461"
#unittest pass syntax

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
        for x := down; x <= up; x += 1
        {
            #mixin stmt
        }
    }
}

#test
{
    var x: S    
    visit i, j: x
    {
        @print("AAA\n")
    }   
}