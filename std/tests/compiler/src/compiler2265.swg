#module "compiler2265"
#unittest error

#test
{
    var x: struct{v: f32;}
    x[0] = x
}