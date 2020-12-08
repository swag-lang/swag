#module "tests_call_compiler_foreign"
#import "tests_compiler_foreign"
using tests_compiler_foreign

using swag

//#[swag.printbc, swag.optim(false)]
#test
{
    tcf_join8("titi", 1, 2, 3, 4)
    tcf_join16("titi", 1, 2, 3, 4)
    tcf_join32("titi", 1, 2, 3, 4)
}