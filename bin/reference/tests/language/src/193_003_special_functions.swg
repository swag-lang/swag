#global skip

/**
# #main
`#main` is the program entry point. It can only be defined **once** per module, and has meaning only for an executable.
*/
#main
{
}

/**
Unlike the C function `main()`, there's no argument, but you can use the intrinsic `@args` to get a slice of all the specified program arguments.
*/
#main
{
    var myArgs = @args()
    var count = @countof(myArgs)
    if myArgs[0] == "fullscreen"
    {
        ...
    }
}

/**
# #premain
`#premain` will be called after all the modules have done their `#init` code, but before the `#main` function is called.
*/
#premain
{
}

/**
# #init
`#init` will be called at runtime, during the module initialization.
You can have as many `#init` as you want, but the execution order in the same module is undefined.
*/
#init
{
}

/**
# #drop
`#drop` will be called at runtime, when module is unloaded.
You can have as many `#drop` as you want.
The execution order in the same module is undefined, but is always the inverse order of `#init`.
*/
#drop
{
}

/**
# #test
`#test` is a special function than can be used in the `tests/` folder of the workspace.
All `#test` will be executed only if swag is running in test mode.
*/
#test
{
}
