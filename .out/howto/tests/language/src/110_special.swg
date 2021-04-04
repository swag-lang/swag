// #test is a special function than can be used in the 'tests' folder of the workspace.
// They will be executed only if swag is run in test mode.
#test
{
}

// #run is a special function that will be called at compile time only
// It can be used to precompute some global values for example.
var G: f32 = ?
#run
{
    // Dummy example, but at runtime, G will be equal to 10
    loop 10 G += 1
}

// #main is the program entry point
// Can only be defined once per module
#main
{
}