#module "tests_call_compiler_foreign"
#import "tests_compiler_foreign"
using tests_compiler_foreign

#test
{
    tcf_join("titi", 1, 2, 3, 4)
}