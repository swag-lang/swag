#module "compiler1835"
#unittest error

struct Point2
{
    using R = s32
    x, y: s32
}
/*
//#[swag.printbc]
func(T) norm(vec: T)-> @typeof(T.R)
{

}*/