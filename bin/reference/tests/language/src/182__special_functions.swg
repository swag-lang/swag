/**
# #test
`#test` is a special function than can be used in the 'tests/' folder of the workspace.
They will be executed only if swag is running in test mode.
*/
#test
{
}

/**
# #main
`#main` is the program entry point.
Can only be defined once per module.
*/
#main
{
}

/**
# #init
`#init` will be called at runtime, during the module initialization.
You can have as many #init as you want.
Order of #init in the same module is undefined.
*/
#init
{
}

/**
# #drop
`#drop` will be called at runtime, when module is unloaded.
You can have as many #drop as you want.
Order of #drop in the same module is undefined (but is always the inverse order of #init).
*/
#drop
{
}

/**
# #premain
`#premain` will be called after all modules have done their #init code, but before the #main function is called.
*/
#premain
{
}