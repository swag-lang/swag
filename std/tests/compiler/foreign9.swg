#module "tests_call_compiler_foreign"
#import "tests_compiler_foreign"
using tests_compiler_foreign

using swag

//#[swag.printbc, swag.optim(false)]
#test
{
    tcf_alloc(10)
}