#module "tests.call.compiler.foreign"
#import "tests.compiler.foreign"

using swag

//#[swag.printbc, swag.optim(false)]
#test
{
    tcf_alloc(10)
}