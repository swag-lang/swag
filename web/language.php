<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta http-equiv="X-UA-Compatible" content="IE=edge">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<?php include('common/start-head.php'); ?><title>Swag Language Reference</title>
<link rel="icon" type="image/x-icon" href="favicon.ico">
<link rel="stylesheet" type="text/css" href="css/style.css">
<script src="https://kit.fontawesome.com/f76be2b3ee.js" crossorigin="anonymous"></script>
<style>

    .container  { display: flex; flex-wrap: nowrap; flex-direction: row; margin: 0px auto; padding: 0px; }
    .left       { display: block; overflow-y: scroll; width: 500px; }
    .left-page  { margin: 10px; }
    .right      { display: block; width: 100%; }
    .right-page { max-width: 1024px; margin: 10px auto; }
    
    @media(min-width: 640px)  { .container { max-width: 640px; }}
    @media(min-width: 768px)  { .container { max-width: 768px; }}
    @media(min-width: 1024px) { .container { max-width: 1024px; }}
    @media(min-width: 1280px) { .container { max-width: 1280px; }}
    @media(min-width: 1536px) { .container { max-width: 1536px; }}
    
    @media screen and (max-width: 600px) {
        .left       { display: none; }
        .right-page { margin:  10px; }
    }

            .container { height: 100vh; }
            .right     { overflow-y: scroll; }
        
    html { font-family: ui-sans-serif, system-ui, "Segoe UI", Roboto, "Helvetica Neue", Arial, "Noto Sans", sans-serif; }
    body { margin: 0px; line-height: 1.3em; }     

    .container a        { color: DoggerBlue; }
    .container a:hover  { text-decoration: underline; }
    .container img      { margin: 0 auto; }
   
    .left a     { text-decoration: none; }
    .left ul    { list-style-type: none; margin-left: -20px; }
    .left h3    { background-color: Black; color: White; padding: 6px; }
    .right h1   { margin-top: 50px; margin-bottom: 50px; }
    .right h2   { margin-top: 35px; }

    .right ol li { margin-bottom: 10px; }

    .strikethrough-text { text-decoration: line-through; }
    .swag-watermark     { text-align:right; font-size: 80%; margin-top: 30px; }
    .swag-watermark a   { text-decoration: none; color: inherit; }

    .blockquote               { border-radius: 5px; border: 1px solid; margin: 20px; padding: 10px; }
    .blockquote-default       { border-color: Orange; border-left: 6px solid Orange; background-color: LightYellow; }
    .blockquote-note          { border-color: #ADCEDD; background-color: #CDEEFD; }
    .blockquote-tip           { border-color: #BCCFBC; background-color: #DCEFDC; }
    .blockquote-warning       { border-color: #DFBDB3; background-color: #FFDDD3; }
    .blockquote-attention     { border-color: #DDBAB8; background-color: #FDDAD8; }
    .blockquote-example       { border: 2px solid LightGrey; }
    .blockquote-title-block   { margin-bottom: 10px; }
    .blockquote-title         { font-weight: bold; }
    .blockquote-default       p:first-child { margin-top: 0px; }
    .blockquote-default       p:last-child  { margin-bottom: 0px; }
    .blockquote               p:last-child  { margin-bottom: 0px; }

    .description-list-title   { font-weight: bold; font-style: italic; }
    .description-list-block   { margin-left: 30px; }

    .container table          { border: 1px solid LightGrey; border-collapse: collapse; font-size: 90%; margin-left: 20px; margin-right: 20px; }
    .container td             { border: 1px solid LightGrey; border-collapse: collapse; padding: 6px; min-width: 100px; }
    .container th             { border: 1px solid LightGrey; border-collapse: collapse; padding: 6px; min-width: 100px; background-color: #eeeeee; }

    table.api-item            { border-collapse: separate; background-color: Black; color: White; width: 100%; margin-top: 70px; margin-right: 0px; font-size: 110%; }
    .api-item td              { font-size: revert; border: 0; }
    .api-item td:first-child  { width: 33%; white-space: nowrap; }
    .api-item-title-src-ref   { text-align:  right; }
    .api-item-title-src-ref a { color:       inherit; }
    .api-item-title-kind      { font-weight: normal; font-size: 80%; }
    .api-item-title-light     { font-weight: normal; }
    .api-item-title-strong    { font-weight: bold; font-size: 100%; }
    .api-additional-infos     { font-size: 90%; white-space: break-spaces; overflow-wrap: break-word; }

    table.table-enumeration           { width: calc(100% - 40px); }
    .table-enumeration td:first-child { background-color: #f8f8f8; white-space: nowrap; }
    .table-enumeration td:last-child  { width: 100%; }
    .table-enumeration td.code-type   { background-color: #eeeeee; }
    .table-enumeration a              { text-decoration: none; color: inherit; }

    .code-inline  { background-color: #eeeeee; border-radius: 5px; border: 1px dotted #cccccc; padding: 0px 8px; font-size: 110%; font-family: monospace; display: inline-block; }
    .code-block   { background-color: #eeeeee; border-radius: 5px; border: 1px solid LightGrey; padding: 10px; margin: 20px; white-space: pre; overflow-x: auto; }
    .code-block   { font-family: ui-monospace, SFMono-Regular, Menlo, Monaco, Consolas, "Liberation Mono", "Courier New", monospace; }
    .code-block a { color: inherit; }
    .SCde { color: #222222; }
    .SCmt { color: #71a35b; }
    .SCmp { color: #7f7f7f; }
    .SFct { color: #ff6a00; }
    .SCst { color: #3bc3a7; }
    .SItr { color: #b4b44a; }
    .STpe { color: #ed9a11; }
    .SKwd { color: #3186cd; }
    .SLgc { color: #b040be; }
    .SNum { color: #74a35b; }
    .SStr { color: #bb6643; }
    .SAtr { color: #7f7f7f; }
    .SBcR { color: #b5af49; }
    .SInv { color: #ff0000; }
</style>
<?php include('common/end-head.php'); ?>
</head>
<body>
<?php include('common/start-body.php'); ?>
<div class="container">
<div class="left">
<div class="left-page">
<h2>Table of Contents</h2>
<ul>
<li><a href="#_000_introduction_swg">Introduction</a></li>
<li><a href="#_001_hello_mad_world_swg">Hello mad world</a></li>
<li><a href="#_002_source_code_organization_swg">Source code organization</a></li>
<li><a href="#_003_comments_swg">Comments</a></li>
<li><a href="#_004_identifiers_swg">Identifiers</a></li>
<li><a href="#_005_keywords_swg">Keywords</a></li>
<ul>
<li><a href="#_005_keywords_swg_Special_keywords">Special keywords</a></li>
<li><a href="#_005_keywords_swg_Reserved_keywords">Reserved keywords</a></li>
<li><a href="#_005_keywords_swg_Basic_types">Basic types</a></li>
<li><a href="#_005_keywords_swg_Compiler_keywords">Compiler keywords</a></li>
<li><a href="#_005_keywords_swg_Misc_intrinsics">Misc intrinsics</a></li>
<li><a href="#_005_keywords_swg_Intrinsics_libc">Intrinsics libc</a></li>
<li><a href="#_005_keywords_swg_Modifiers">Modifiers</a></li>
</ul>
<li><a href="#_006_semicolon_swg">Semicolon</a></li>
<li><a href="#_007_global_declaration_order_swg">Global declaration order</a></li>
<li><a href="#_010_basic_types_swg">Basic types</a></li>
<ul>
<li><a href="#_010_basic_types_swg_Type_Reflection">Type Reflection</a></li>
</ul>
<li><a href="#_011_number_literals_swg">Number literals</a></li>
<ul>
<li><a href="#_011_number_literals_swg_Suffix">Suffix</a></li>
</ul>
<li><a href="#_012_string_swg">String</a></li>
<ul>
<li><a href="#_012_string_swg_Character_literals">Character literals</a></li>
<li><a href="#_012_string_swg_Escape_sequences">Escape sequences</a></li>
<li><a href="#_012_string_swg_Raw_strings">Raw strings</a></li>
<li><a href="#_012_string_swg_Multiline_strings">Multiline strings</a></li>
<li><a href="#_012_string_swg_@stringof_and_@nameof">@stringof and @nameof</a></li>
</ul>
<li><a href="#_013_variables_swg">Variables</a></li>
<ul>
<li><a href="#_013_variables_swg_Special_variables">Special variables</a></li>
</ul>
<li><a href="#_014_const_swg">Const</a></li>
<li><a href="#_015_operators_swg">Operators</a></li>
<ul>
<li><a href="#_015_operators_swg_Arithmetic_operators">Arithmetic operators</a></li>
<li><a href="#_015_operators_swg_Bitwise_operators">Bitwise operators</a></li>
<li><a href="#_015_operators_swg_Assignment_operators">Assignment operators</a></li>
<li><a href="#_015_operators_swg_Unary_operators">Unary operators</a></li>
<li><a href="#_015_operators_swg_Comparison_operators">Comparison operators</a></li>
<li><a href="#_015_operators_swg_Logical_operators">Logical operators</a></li>
<li><a href="#_015_operators_swg_Ternary_operator">Ternary operator</a></li>
<li><a href="#_015_operators_swg_Spaceshift_operator">Spaceshift operator</a></li>
<li><a href="#_015_operators_swg_Null-coalescing_operator">Null-coalescing operator</a></li>
<li><a href="#_015_operators_swg_Type_promotion">Type promotion</a></li>
<li><a href="#_015_operators_swg_Operator_precedence">Operator precedence</a></li>
</ul>
<li><a href="#_016_cast_swg">Cast</a></li>
<ul>
<li><a href="#_016_cast_swg_Explicit_Cast">Explicit Cast</a></li>
<li><a href="#_016_cast_swg_acast_(Automatic_Cast)">acast (Automatic Cast)</a></li>
<li><a href="#_016_cast_swg_Bitcast">Bitcast</a></li>
<li><a href="#_016_cast_swg_Implicit_Casts">Implicit Casts</a></li>
<ul>
<li><a href="#_016_cast_swg_Implicit_Casts_Implicit_Cast_Rules">Implicit Cast Rules</a></li>
<li><a href="#_016_cast_swg_Implicit_Casts_Examples_of_Implicit_Casts">Examples of Implicit Casts</a></li>
<li><a href="#_016_cast_swg_Implicit_Casts_Examples_Where_Implicit_Casts_Are_Not_Allowed">Examples Where Implicit Casts Are Not Allowed</a></li>
</ul>
</ul>
<li><a href="#_020_array_swg">Array</a></li>
<li><a href="#_021_slice_swg">Slice</a></li>
<li><a href="#_022_pointers_swg">Pointers</a></li>
<ul>
<li><a href="#_022_pointers_swg_Single_value_pointers">Single value pointers</a></li>
<li><a href="#_022_pointers_swg_Multiple_values_pointers">Multiple values pointers</a></li>
</ul>
<li><a href="#_023_references_swg">References</a></li>
<li><a href="#_024_any_swg">Any</a></li>
<li><a href="#_025_tuple_swg">Tuple</a></li>
<ul>
<li><a href="#_025_tuple_swg_Tuple_unpacking">Tuple unpacking</a></li>
</ul>
<li><a href="#_030_enum_swg">Enum</a></li>
<ul>
<li><a href="#_030_enum_swg_Enum_as_flags">Enum as flags</a></li>
<li><a href="#_030_enum_swg_Enum_of_arrays">Enum of arrays</a></li>
<li><a href="#_030_enum_swg_Enum_of_slices">Enum of slices</a></li>
<li><a href="#_030_enum_swg_Nested_enums">Nested enums</a></li>
<li><a href="#_030_enum_swg_Specific_attributes">Specific attributes</a></li>
<li><a href="#_030_enum_swg_Enum_type_inference">Enum type inference</a></li>
<li><a href="#_030_enum_swg_Visiting_Enum_Values">Visiting Enum Values</a></li>
<ul>
<li><a href="#_030_enum_swg_Visiting_Enum_Values_Loop">Loop</a></li>
<li><a href="#_030_enum_swg_Visiting_Enum_Values_Visit">Visit</a></li>
</ul>
</ul>
<li><a href="#_031_impl_swg">Impl</a></li>
<li><a href="#_035_namespace_swg">Namespace</a></li>
<li><a href="#_050_if_swg">If</a></li>
<ul>
<li><a href="#_050_if_swg_Variable_declaration">Variable declaration</a></li>
</ul>
<li><a href="#_051_loop_swg">Loop</a></li>
<ul>
<li><a href="#_051_loop_swg_break,_continue">break, continue</a></li>
<li><a href="#_051_loop_swg_Ranges">Ranges</a></li>
<li><a href="#_051_loop_swg_Infinite_loop">Infinite loop</a></li>
<li><a href="#_051_loop_swg_where">where</a></li>
</ul>
<li><a href="#_052_visit_swg">Visit</a></li>
<ul>
<li><a href="#_052_visit_swg_where">where</a></li>
</ul>
<li><a href="#_053_for_swg">For</a></li>
<ul>
<li><a href="#_053_for_swg_Introduction_to_`for`_Loops">Introduction to `for` Loops</a></li>
<li><a href="#_053_for_swg_Accessing_Loop_Index_with_`#index`">Accessing Loop Index with `#index`</a></li>
<li><a href="#_053_for_swg_Using_`break`_and_`continue`_in_`for`_Loops">Using `break` and `continue` in `for` Loops</a></li>
<li><a href="#_053_for_swg_Nested_`for`_Loops">Nested `for` Loops</a></li>
<li><a href="#_053_for_swg_Iterating_Over_Arrays_with_`for`">Iterating Over Arrays with `for`</a></li>
</ul>
<li><a href="#_054_while_swg">While</a></li>
<ul>
<li><a href="#_054_while_swg_Introduction_to_`while`_Loops">Introduction to `while` Loops</a></li>
<li><a href="#_054_while_swg_Breaking_Out_of_a_`while`_Loop">Breaking Out of a `while` Loop</a></li>
<li><a href="#_054_while_swg_Skipping_Iterations_with_`continue`">Skipping Iterations with `continue`</a></li>
<li><a href="#_054_while_swg_Nested_`while`_Loops">Nested `while` Loops</a></li>
<li><a href="#_054_while_swg_Using_`while`_with_Complex_Conditions">Using `while` with Complex Conditions</a></li>
</ul>
<li><a href="#_055_switch_swg">Switch</a></li>
<ul>
<li><a href="#_055_switch_swg_The_'Swag_Complete'_Attribute">The 'Swag.Complete' Attribute</a></li>
<li><a href="#_055_switch_swg_Specific_Types">Specific Types</a></li>
<li><a href="#_055_switch_swg_Using_Ranges">Using Ranges</a></li>
<li><a href="#_055_switch_swg_The_'where'_clause">The 'where' clause</a></li>
<li><a href="#_055_switch_swg_Switch_Without_an_Expression">Switch Without an Expression</a></li>
</ul>
<li><a href="#_056_break_swg">Break</a></li>
<li><a href="#_060_struct_swg">Struct</a></li>
<ul>
<li><a href="#_060_struct_swg__061_001_declaration_swg">Declaration</a></li>
<li><a href="#_060_struct_swg__062_002_impl_swg">Impl</a></li>
<li><a href="#_060_struct_swg__063_003_special_functions_swg">Special functions</a></li>
<li><a href="#_060_struct_swg__064_004_affectation_swg">Affectation</a></li>
<li><a href="#_060_struct_swg__064_005_count_swg">Count</a></li>
<li><a href="#_060_struct_swg__064_006_post_copy_and_post_move_swg">Post copy and post move</a></li>
<ul>
<li><a href="#_060_struct_swg__064_006_post_copy_and_post_move_swg_Move_Semantics_in_Functions">Move Semantics in Functions</a></li>
</ul>
<li><a href="#_060_struct_swg__064_007_visit_swg">Visit</a></li>
<li><a href="#_060_struct_swg__067_008_offset_swg">Offset</a></li>
<li><a href="#_060_struct_swg__068_009_packing_swg">Packing</a></li>
</ul>
<li><a href="#_070_union_swg">Union</a></li>
<li><a href="#_075_interface_swg">Interface</a></li>
<ul>
<li><a href="#_075_interface_swg_Default_implementation">Default implementation</a></li>
</ul>
<li><a href="#_100_function_swg">Function</a></li>
<ul>
<li><a href="#_100_function_swg__101_001_declaration_swg">Declaration</a></li>
<ul>
<li><a href="#_100_function_swg__101_001_declaration_swg_Introduction_to_Function_Declarations">Introduction to Function Declarations</a></li>
<li><a href="#_100_function_swg__101_001_declaration_swg_Returning_Values_from_Functions">Returning Values from Functions</a></li>
<li><a href="#_100_function_swg__101_001_declaration_swg_Inferring_Return_Types">Inferring Return Types</a></li>
<li><a href="#_100_function_swg__101_001_declaration_swg_Shorter_Syntax_for_Functions_Without_Return_Values">Shorter Syntax for Functions Without Return Values</a></li>
<li><a href="#_100_function_swg__101_001_declaration_swg_Defining_Parameters_in_Functions">Defining Parameters in Functions</a></li>
<li><a href="#_100_function_swg__101_001_declaration_swg_Using_Default_Parameter_Values">Using Default Parameter Values</a></li>
<li><a href="#_100_function_swg__101_001_declaration_swg_Inferred_Parameter_Types">Inferred Parameter Types</a></li>
<li><a href="#_100_function_swg__101_001_declaration_swg_Overloading_Functions">Overloading Functions</a></li>
<li><a href="#_100_function_swg__101_001_declaration_swg_Nested_Functions">Nested Functions</a></li>
<li><a href="#_100_function_swg__101_001_declaration_swg_Named_Parameters_and_Parameter_Order">Named Parameters and Parameter Order</a></li>
<li><a href="#_100_function_swg__101_001_declaration_swg_Multiple_Return_Values">Multiple Return Values</a></li>
<ul>
<li><a href="#_100_function_swg__101_001_declaration_swg_Multiple_Return_Values_Returning_Multiple_Values_with_Anonymous_Structs">Returning Multiple Values with Anonymous Structs</a></li>
<li><a href="#_100_function_swg__101_001_declaration_swg_Multiple_Return_Values_Using_`retval`_to_Structure_Return_Statements">Using `retval` to Structure Return Statements</a></li>
</ul>
</ul>
<li><a href="#_100_function_swg__102_002_lambda_swg">Lambda</a></li>
<ul>
<li><a href="#_100_function_swg__102_002_lambda_swg_Introduction_to_Lambdas_in_Swag">Introduction to Lambdas in Swag</a></li>
<li><a href="#_100_function_swg__102_002_lambda_swg_Null_Lambdas">Null Lambdas</a></li>
<li><a href="#_100_function_swg__102_002_lambda_swg_Using_Lambdas_as_Function_Parameters">Using Lambdas as Function Parameters</a></li>
<li><a href="#_100_function_swg__102_002_lambda_swg_Anonymous_Functions">Anonymous Functions</a></li>
<li><a href="#_100_function_swg__102_002_lambda_swg_Passing_Anonymous_Functions_as_Parameters">Passing Anonymous Functions as Parameters</a></li>
<li><a href="#_100_function_swg__102_002_lambda_swg_Inferred_Parameter_Types_in_Anonymous_Functions">Inferred Parameter Types in Anonymous Functions</a></li>
<li><a href="#_100_function_swg__102_002_lambda_swg_Omitting_Types_When_Assigning_Lambdas">Omitting Types When Assigning Lambdas</a></li>
<li><a href="#_100_function_swg__102_002_lambda_swg_Lambdas_with_Default_Parameter_Values">Lambdas with Default Parameter Values</a></li>
</ul>
<li><a href="#_100_function_swg__103_003_closure_swg">Closure</a></li>
<ul>
<li><a href="#_100_function_swg__103_003_closure_swg_Introduction_to_Closures_in_Swag">Introduction to Closures in Swag</a></li>
<li><a href="#_100_function_swg__103_003_closure_swg_Declaring_a_Closure">Declaring a Closure</a></li>
<li><a href="#_100_function_swg__103_003_closure_swg_Capturing_Variables_by_Reference">Capturing Variables by Reference</a></li>
<li><a href="#_100_function_swg__103_003_closure_swg_Assigning_Lambdas_to_Closure_Variables">Assigning Lambdas to Closure Variables</a></li>
<li><a href="#_100_function_swg__103_003_closure_swg_Capturing_Complex_Types">Capturing Complex Types</a></li>
<li><a href="#_100_function_swg__103_003_closure_swg_Modifying_Captured_Variables">Modifying Captured Variables</a></li>
</ul>
<li><a href="#_100_function_swg__104_004_mixin_swg">Mixin</a></li>
<ul>
<li><a href="#_100_function_swg__104_004_mixin_swg_Introduction_to_Swag_Mixins">Introduction to Swag Mixins</a></li>
<li><a href="#_100_function_swg__104_004_mixin_swg_Basic_Example_of_a_Mixin">Basic Example of a Mixin</a></li>
<li><a href="#_100_function_swg__104_004_mixin_swg_Mixins_with_Parameters">Mixins with Parameters</a></li>
<li><a href="#_100_function_swg__104_004_mixin_swg_Mixins_with_Code_Blocks">Mixins with Code Blocks</a></li>
<li><a href="#_100_function_swg__104_004_mixin_swg_Mixing_Code_Blocks_in_Separate_Statements">Mixing Code Blocks in Separate Statements</a></li>
<li><a href="#_100_function_swg__104_004_mixin_swg_Creating_Aliases_with_Mixins">Creating Aliases with Mixins</a></li>
<li><a href="#_100_function_swg__104_004_mixin_swg_Unique_Variable_Names_with_`#mix?`">Unique Variable Names with `#mix?`</a></li>
</ul>
<li><a href="#_100_function_swg__105_005_macro_swg">Macro</a></li>
<ul>
<li><a href="#_100_function_swg__105_005_macro_swg_Introduction_to_Swag_Macros">Introduction to Swag Macros</a></li>
<li><a href="#_100_function_swg__105_005_macro_swg_Macro_Scope">Macro Scope</a></li>
<li><a href="#_100_function_swg__105_005_macro_swg_Resolving_Identifiers_Outside_the_Macro_Scope">Resolving Identifiers Outside the Macro Scope</a></li>
<li><a href="#_100_function_swg__105_005_macro_swg_Macros_with_Code_Parameters">Macros with Code Parameters</a></li>
<li><a href="#_100_function_swg__105_005_macro_swg_Forcing_Code_into_the_Caller’s_Scope_with_`#macro`">Forcing Code into the Caller’s Scope with `#macro`</a></li>
<li><a href="#_100_function_swg__105_005_macro_swg_Performance_Considerations_with_Macros">Performance Considerations with Macros</a></li>
<li><a href="#_100_function_swg__105_005_macro_swg_Handling_Breaks_in_User_Code_with_Macros">Handling Breaks in User Code with Macros</a></li>
<li><a href="#_100_function_swg__105_005_macro_swg_Using_Aliases_in_Macros">Using Aliases in Macros</a></li>
</ul>
<li><a href="#_100_function_swg__105_006_variadic_parameters_swg">Variadic parameters</a></li>
<ul>
<li><a href="#_100_function_swg__105_006_variadic_parameters_swg_Introduction_to_Variadic_Functions">Introduction to Variadic Functions</a></li>
<li><a href="#_100_function_swg__105_006_variadic_parameters_swg_Working_with_Variadic_Parameters_as_Slices">Working with Variadic Parameters as Slices</a></li>
<li><a href="#_100_function_swg__105_006_variadic_parameters_swg_Forcing_Variadic_Parameters_to_a_Specific_Type">Forcing Variadic Parameters to a Specific Type</a></li>
<li><a href="#_100_function_swg__105_006_variadic_parameters_swg_Passing_Variadic_Parameters_Between_Functions">Passing Variadic Parameters Between Functions</a></li>
<li><a href="#_100_function_swg__105_006_variadic_parameters_swg_Spreading_Arrays_or_Slices_to_Variadic_Parameters">Spreading Arrays or Slices to Variadic Parameters</a></li>
<li><a href="#_100_function_swg__105_006_variadic_parameters_swg_Advanced_Example:_Combining_Variadic_and_Non-Variadic_Parameters">Advanced Example: Combining Variadic and Non-Variadic Parameters</a></li>
<li><a href="#_100_function_swg__105_006_variadic_parameters_swg_Example:_Handling_Different_Types_in_Variadic_Parameters">Example: Handling Different Types in Variadic Parameters</a></li>
</ul>
<li><a href="#_100_function_swg__106_007_ufcs_swg">Ufcs</a></li>
<ul>
<li><a href="#_100_function_swg__106_007_ufcs_swg_Introduction_to_Uniform_Function_Call_Syntax_(UFCS)">Introduction to Uniform Function Call Syntax (UFCS)</a></li>
<li><a href="#_100_function_swg__106_007_ufcs_swg_Static_Functions_as_Methods">Static Functions as Methods</a></li>
<li><a href="#_100_function_swg__106_007_ufcs_swg_UFCS_with_Multiple_Parameters">UFCS with Multiple Parameters</a></li>
<li><a href="#_100_function_swg__106_007_ufcs_swg_UFCS_and_Function_Overloading">UFCS and Function Overloading</a></li>
</ul>
<li><a href="#_100_function_swg__107_008_constexpr_swg">Constexpr</a></li>
<li><a href="#_100_function_swg__108_009_function_overloading_swg">Function overloading</a></li>
<li><a href="#_100_function_swg__109_010_discard_swg">Discard</a></li>
<li><a href="#_100_function_swg__110_011_retval_swg">Retval</a></li>
<li><a href="#_100_function_swg__111_012_foreign_swg">Foreign</a></li>
</ul>
<li><a href="#_120_intrinsics_swg">Intrinsics</a></li>
<ul>
<li><a href="#_120_intrinsics_swg_Base">Base</a></li>
<li><a href="#_120_intrinsics_swg_Buildin">Buildin</a></li>
<li><a href="#_120_intrinsics_swg_Memory_related">Memory related</a></li>
<li><a href="#_120_intrinsics_swg_Atomic_operations">Atomic operations</a></li>
<li><a href="#_120_intrinsics_swg_Math">Math</a></li>
</ul>
<li><a href="#_121_init_swg">Init</a></li>
<ul>
<li><a href="#_121_init_swg_@init">@init</a></li>
<li><a href="#_121_init_swg_@drop">@drop</a></li>
</ul>
<li><a href="#_130_generic_swg">Generic</a></li>
<ul>
<li><a href="#_130_generic_swg__131_001_declaration_swg">Declaration</a></li>
<li><a href="#_130_generic_swg__132_002_where_swg">Where</a></li>
<ul>
<li><a href="#_130_generic_swg__132_002_where_swg_Single_evaluation">Single evaluation</a></li>
<li><a href="#_130_generic_swg__132_002_where_swg_Multiple_evaluations">Multiple evaluations</a></li>
</ul>
</ul>
<li><a href="#_140_attributes_swg">Attributes</a></li>
<ul>
<li><a href="#_140_attributes_swg__141_001_user_attributes_swg">User attributes</a></li>
<li><a href="#_140_attributes_swg__142_002_predefined_attributes_swg">Predefined attributes</a></li>
</ul>
<li><a href="#_160_scoping_swg">Scoping</a></li>
<ul>
<li><a href="#_160_scoping_swg__161_001_defer_swg">Defer</a></li>
<li><a href="#_160_scoping_swg__162_002_using_swg">Using</a></li>
<ul>
<li><a href="#_160_scoping_swg__162_002_using_swg_For_a_function_parameter">For a function parameter</a></li>
<li><a href="#_160_scoping_swg__162_002_using_swg_For_a_field">For a field</a></li>
</ul>
<li><a href="#_160_scoping_swg__163_003_with_swg">With</a></li>
</ul>
<li><a href="#_164_alias_swg">Alias</a></li>
<ul>
<li><a href="#_164_alias_swg_Type_alias">Type alias</a></li>
<li><a href="#_164_alias_swg_Name_alias">Name alias</a></li>
</ul>
<li><a href="#_170_error_management_swg">Error management</a></li>
<ul>
<li><a href="#_170_error_management_swg_throw">throw</a></li>
<li><a href="#_170_error_management_swg_catch">catch</a></li>
<li><a href="#_170_error_management_swg_trycatch">trycatch</a></li>
<li><a href="#_170_error_management_swg_try">try</a></li>
<li><a href="#_170_error_management_swg_assume">assume</a></li>
<ul>
<li><a href="#_170_error_management_swg_assume_Implicit_assume">Implicit assume</a></li>
</ul>
<li><a href="#_170_error_management_swg_Blocks">Blocks</a></li>
<li><a href="#_170_error_management_swg_Implicit_try">Implicit try</a></li>
<li><a href="#_170_error_management_swg_The_error_struct">The error struct</a></li>
<li><a href="#_170_error_management_swg_defer">defer</a></li>
</ul>
<li><a href="#_175_safety_swg">Safety</a></li>
<ul>
<li><a href="#_175_safety_swg_overflow">overflow</a></li>
<li><a href="#_175_safety_swg_any">any</a></li>
<li><a href="#_175_safety_swg_boundcheck">boundcheck</a></li>
<li><a href="#_175_safety_swg_math">math</a></li>
<li><a href="#_175_safety_swg_switch">switch</a></li>
<li><a href="#_175_safety_swg_bool">bool</a></li>
<li><a href="#_175_safety_swg_nan">nan</a></li>
</ul>
<li><a href="#_180_type_reflection_swg">Type reflection</a></li>
<ul>
<li><a href="#_180_type_reflection_swg_@decltype">@decltype</a></li>
</ul>
<li><a href="#_190_compile-time_evaluation_swg">Compile-time evaluation</a></li>
<ul>
<li><a href="#_190_compile-time_evaluation_swg__191_001_constexpr_swg">Constexpr</a></li>
<li><a href="#_190_compile-time_evaluation_swg__192_002_run_swg">Run</a></li>
<ul>
<li><a href="#_190_compile-time_evaluation_swg__192_002_run_swg_Force_compile-time_call">Force compile-time call</a></li>
<li><a href="#_190_compile-time_evaluation_swg__192_002_run_swg_#run_block">#run block</a></li>
<li><a href="#_190_compile-time_evaluation_swg__192_002_run_swg_#run_expression">#run expression</a></li>
</ul>
<li><a href="#_190_compile-time_evaluation_swg__193_003_special_functions_swg">Special functions</a></li>
<ul>
<li><a href="#_190_compile-time_evaluation_swg__193_003_special_functions_swg_#main">#main</a></li>
<li><a href="#_190_compile-time_evaluation_swg__193_003_special_functions_swg_#premain">#premain</a></li>
<li><a href="#_190_compile-time_evaluation_swg__193_003_special_functions_swg_#init">#init</a></li>
<li><a href="#_190_compile-time_evaluation_swg__193_003_special_functions_swg_#drop">#drop</a></li>
<li><a href="#_190_compile-time_evaluation_swg__193_003_special_functions_swg_#test">#test</a></li>
</ul>
<li><a href="#_190_compile-time_evaluation_swg__194_004_compiler_instructions_swg">Compiler instructions</a></li>
<ul>
<li><a href="#_190_compile-time_evaluation_swg__194_004_compiler_instructions_swg_#assert">#assert</a></li>
<li><a href="#_190_compile-time_evaluation_swg__194_004_compiler_instructions_swg_#if/#else">#if/#else</a></li>
<li><a href="#_190_compile-time_evaluation_swg__194_004_compiler_instructions_swg_#error/#warning">#error/#warning</a></li>
<li><a href="#_190_compile-time_evaluation_swg__194_004_compiler_instructions_swg_#global">#global</a></li>
<li><a href="#_190_compile-time_evaluation_swg__194_004_compiler_instructions_swg_#foreignlib">#foreignlib</a></li>
</ul>
</ul>
<li><a href="#_210_code_inspection_swg">Code inspection</a></li>
<li><a href="#_220_meta_programming_swg">Meta programming</a></li>
<ul>
<li><a href="#_220_meta_programming_swg__221_001_ast_swg">Ast</a></li>
<ul>
<li><a href="#_220_meta_programming_swg__221_001_ast_swg_Struct_and_enums">Struct and enums</a></li>
<li><a href="#_220_meta_programming_swg__221_001_ast_swg_For_example">For example</a></li>
<li><a href="#_220_meta_programming_swg__221_001_ast_swg_At_global_scope">At global scope</a></li>
</ul>
<li><a href="#_220_meta_programming_swg__222_002_compiler_interface_swg">Compiler interface</a></li>
</ul>
<li><a href="#_230_documentation_md">Documentation</a></li>
<ul>
<li><a href="#_230_documentation_md_Markdown_files">Markdown files</a></li>
<li><a href="#_230_documentation_md_Format_of_comments">Format of comments</a></li>
<ul>
<li><a href="#_230_documentation_md_Format_of_comments_Paragraphs">Paragraphs</a></li>
<li><a href="#_230_documentation_md_Format_of_comments_Lists">Lists</a></li>
<li><a href="#_230_documentation_md_Format_of_comments_Definition_Lists">Definition Lists</a></li>
<li><a href="#_230_documentation_md_Format_of_comments_Quotes">Quotes</a></li>
<li><a href="#_230_documentation_md_Format_of_comments_Tables">Tables</a></li>
<li><a href="#_230_documentation_md_Format_of_comments_Code">Code</a></li>
<li><a href="#_230_documentation_md_Format_of_comments_Titles">Titles</a></li>
<li><a href="#_230_documentation_md_Format_of_comments_References">References</a></li>
<li><a href="#_230_documentation_md_Format_of_comments_Images">Images</a></li>
<li><a href="#_230_documentation_md_Format_of_comments_Markdown">Markdown</a></li>
</ul>
<li><a href="#_230_documentation_md__231_001_Api_swg">Api</a></li>
<ul>
<li><a href="#_230_documentation_md__231_001_Api_swg_References">References</a></li>
<li><a href="#_230_documentation_md__231_001_Api_swg_NoDoc">NoDoc</a></li>
</ul>
<li><a href="#_230_documentation_md__231_002_Examples_md">Examples</a></li>
<li><a href="#_230_documentation_md__231_003_Pages_md">Pages</a></li>
</ul>
</div>
</div>
<div class="right">
<div class="right-page">
<h1>Swag Language Reference</h1>

<h2 id="_000_introduction_swg">Introduction</h2><p>The <span class="code-inline">swag-lang/swag/bin/reference/language</span> module offers a foundational introduction to the syntax and usage of the Swag programming language, separate from the <a href="std.php">Swag standard modules</a> (<span class="code-inline">Std</span>). This documentation is auto-generated from the module's source code. </p>
<p>For more advanced features, such as dynamic arrays, dynamic strings, or hash maps, please consult the <a href="std.core.php">Std.Core</a> module documentation, as these topics are beyond the scope of the examples covered here. This guide focuses exclusively on the core elements of the Swag language. </p>
<p>Since <span class="code-inline">reference/language</span> is implemented as a test module, you can run it using the following commands: </p>
<div class="code-block"><span class="SCde">swag test --workspace:c:/swag-lang/swag/bin/reference
swag test -w:c:/swag-lang/swag/bin/reference</span></div>
<p>These commands will execute all test modules within the specified workspace, including this one. If you are running Swag from the workspace directory, the <span class="code-inline">--workspace</span> flag (or shorthand <span class="code-inline">-w</span>) can be omitted. </p>
<p>To compile and execute a specific module within the workspace, use the <span class="code-inline">--module</span> (or <span class="code-inline">-m</span>) flag: </p>
<div class="code-block"><span class="SCde">swag test -w:c:/swag-lang/swag/bin/reference -m:test_language</span></div>

<h2 id="_001_hello_mad_world_swg">Hello mad world</h2><p>Let's start with the most simple version of the "hello world" example. This is a version that does not require external dependencies like the <a href="std.php">Swag standard modules</a>. </p>
<p><span class="code-inline">#main</span> is the <b>program entry point</b>, a special compiler function (that's why the name starts with <span class="code-inline">#</span>). It must be defined only once for a native executable. <span class="code-inline">@print</span> is an <b>intrinsic</b>, a special built-in function (that's why the name starts with <span class="code-inline">@</span>). </p>
<p>All intrinsics are part of the <a href="std/swag.runtime.php">compiler runtime</a> which comes with the compiler. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#main</span>
{
    <span class="SItr">@print</span>(<span class="SStr">"Hello mad world !\n"</span>)
}</span></div>
<p>Next, a version that this time uses the <span class="code-inline">Core.Console.print</span> function in the <a href="std.core.html">Std.Core</a> module. The <span class="code-inline">Std.Core</span> module would have to be imported in order to be used, but let's keep it simple. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#main</span>
{
    <span class="SCst">Core</span>.<span class="SCst">Console</span>.<span class="SFct">print</span>(<span class="SStr">"Hello mad world !"</span>, <span class="SStr">"\n"</span>)
    <span class="SCst">Core</span>.<span class="SCst">Console</span>.<span class="SFct">printf</span>(<span class="SStr">"%\n"</span>, <span class="SStr">"Hello mad world again !"</span>)
}</span></div>
<p>A <span class="code-inline">#run</span> block is executed at <b>compile time</b>, and can make Swag behaves like a kind of a <b>scripting language</b>. So in the following example, the famous message will be printed by the compiler during compilation. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#run</span>
{
    <span class="SKwd">const</span> <span class="SCst">Msg</span> = <span class="SStr">"Hello mad world !\n"</span>   <span class="SCmt">// Creates a compiler constant of type 'string'</span>
    <span class="SCst">Core</span>.<span class="SCst">Console</span>.<span class="SFct">print</span>(<span class="SCst">Msg</span>)             <span class="SCmt">// And call 'Console.print' at compile time</span>
}</span></div>
<p>A version that calls a <b>nested function</b> at compile time (only) to initialize the string constant to print. </p>
<div class="code-block"><span class="SCde"><span class="SCmt">// Brings the 'Core' namespace into scope, to avoid repeating it again and again</span>
<span class="SKwd">using</span> <span class="SCst">Core</span>

<span class="SFct">#main</span>
{
    <span class="SAtr">#[Swag.ConstExpr]</span>
    <span class="SKwd">func</span> <span class="SFct">nestedFunc</span>() =&gt; <span class="SStr">"Hello mad world !\n"</span>   <span class="SCmt">// Function short syntax</span>

    <span class="SCmt">// nestedFunc() can be called at compile time because it is marked with</span>
    <span class="SCmt">// the 'Swag.ConstExpr' attribute.</span>
    <span class="SKwd">const</span> <span class="SCst">Msg</span> = <span class="SFct">nestedFunc</span>()
    <span class="SCst">Console</span>.<span class="SFct">print</span>(<span class="SCst">Msg</span>)
}</span></div>
<p>Now a stupid version that generates the code to: the print thanks to <b>meta programming</b>. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">using</span> <span class="SCst">Core</span>

<span class="SFct">#main</span>
{
    <span class="SKwd">const</span> <span class="SCst">Msg</span> = <span class="SStr">"Hello mad world !\n"</span>

    <span class="SCmt">// The result of an '#ast' block is a string that will be compiled in place.</span>
    <span class="SCmt">// So this whole thing is equivalent to a simple 'Console.print(Msg)'.</span>
    <span class="SFct">#ast</span>
    {
        <span class="SKwd">var</span> sb = <span class="SCst">StrConv</span>.<span class="SCst">StringBuilder</span>{}
        sb.<span class="SFct">appendString</span>(<span class="SStr">"Console.print(Msg)"</span>)
        <span class="SLgc">return</span> sb.<span class="SFct">toString</span>()
    }
}</span></div>
<p>And finally let's be more and more crazy. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">using</span> <span class="SCst">Core</span>

<span class="SFct">#main</span>
{
    <span class="SCmt">// #run will force the call of mySillyFunction() at compile time even if it's not marked</span>
    <span class="SCmt">// with #[Swag.ConstExpr]</span>
    <span class="SKwd">const</span> <span class="SCst">Msg</span> = <span class="SFct">#run</span> <span class="SFct">mySillyFunction</span>()
    <span class="SCst">Console</span>.<span class="SFct">print</span>(<span class="SCst">Msg</span>)
}

<span class="SCmt">// The attribute #[Swag.Compiler] tells Swag that this is a compile time function only.</span>
<span class="SCmt">// So this function will not be exported to the final executable or module.</span>
<span class="SAtr">#[Swag.Compiler]</span>
<span class="SKwd">func</span> <span class="SFct">mySillyFunction</span>()-&gt;<span class="STpe">string</span>
{
    <span class="SCst">Console</span>.<span class="SFct">print</span>(<span class="SStr">"Hello mad world at compile time !\n"</span>)

    <span class="SCmt">// This creates a constant named 'MyConst'</span>
    <span class="SFct">#ast</span>
    {
        <span class="SKwd">var</span> sb = <span class="SCst">StrConv</span>.<span class="SCst">StringBuilder</span>{}
        sb.<span class="SFct">appendString</span>(<span class="SStr">"const MyConst = \"Hello "</span>)
        sb.<span class="SFct">appendString</span>(<span class="SStr">"mad world "</span>)
        sb.<span class="SFct">appendString</span>(<span class="SStr">"at runtime !\""</span>)
        <span class="SLgc">return</span> sb.<span class="SFct">toString</span>()
    }

    <span class="SLgc">return</span> <span class="SCst">MyConst</span>
}</span></div>
<p>This whole piece of code is equivalent to... </p>
<div class="code-block"><span class="SCde"><span class="SFct">#main</span>
{
    <span class="SCst">Core</span>.<span class="SCst">Console</span>.<span class="SFct">print</span>(<span class="SStr">"Hello mad world at runtime !"</span>)
}</span></div>

<h2 id="_002_source_code_organization_swg">Source code organization</h2><p>In Swag, all source files use the <span class="code-inline">.swg</span> extension, except for simple scripts, which use the <span class="code-inline">.swgs</span> extension. All files must be encoded in <b>UTF-8</b>. </p>
<p>Swag does not support compiling individual source files (with the exception of <span class="code-inline">.swgs</span> script files). Instead, source code is organized within a <b>workspace</b> that contains one or more <b>modules</b>. </p>
<p>For instance, <span class="code-inline">Std</span> is a workspace that includes all the Swag standard modules. </p>
<p>A module can be either a <span class="code-inline">dll</span> (on Windows) or an executable, and a workspace may encompass multiple modules. Therefore, a workspace typically contains both the modules you develop (such as your main executable) and all your dependencies (including any external modules you use). </p>
<p>In most cases, the entire workspace is compiled together. </p>

<h2 id="_003_comments_swg">Comments</h2><ul>
<li>Swag supports both single-line and multi-line comments.</li>
</ul>
<div class="code-block"><span class="SCde"><span class="SCmt">// Example of a single-line comment</span>

<span class="SCmt">/*
    Example of a multi-line comment
    that spans several lines.
*/</span>

<span class="SKwd">const</span> <span class="SCst">A</span> = <span class="SNum">0</span>                 <span class="SCmt">// A constant assigned the value '0'</span>
<span class="SKwd">const</span> <span class="SCst">B</span> = <span class="SCmt">/* false */</span> <span class="SKwd">true</span>  <span class="SCmt">// A constant assigned the value 'true', with an inline comment indicating an alternative value</span></span></div>
<ul>
<li>Swag also supports nested comments within multi-line comments.</li>
</ul>
<div class="code-block"><span class="SCmt">/*
    */ Example of a nested multi-line comment */
*/</span></div>

<h2 id="_004_identifiers_swg">Identifiers</h2><p>User-defined identifiers (such as variables, constants, and function names) must begin with either an underscore or an ASCII letter. These identifiers can then include underscores, ASCII letters, and digits. </p>
<p>Examples: </p>
<div class="code-block"><span class="SCde"><span class="SKwd">const</span> thisIsAValidIdentifier0   = <span class="SNum">0</span>
<span class="SKwd">const</span> this_is_also_valid        = <span class="SNum">0</span>
<span class="SKwd">const</span> this_1_is_2_also__3_valid = <span class="SNum">0</span></span></div>
<p>However, identifiers cannot start with two underscores, as this prefix is reserved by the compiler. </p>
<div class="code-block"><span class="SCmt">// const __this_is_invalid = 0</span></div>
<p>Additionally, some identifiers may begin with <span class="code-inline">#</span>, indicating a <b>compiler special keyword</b>. </p>
<div class="code-block"><span class="SCde"><span class="SCmp">#assert</span>
<span class="SFct">#run</span>
<span class="SFct">#main</span></span></div>
<p>Identifiers that start with <span class="code-inline">@</span> represent intrinsic functions. These functions may be available only at compile time or at both compile time and runtime, depending on the specific intrinsic. </p>
<div class="code-block"><span class="SCde"><span class="SItr">@min</span>()
<span class="SItr">@max</span>()
<span class="SItr">@sqrt</span>()
<span class="SItr">@print</span>()</span></div>

<h2 id="_005_keywords_swg">Keywords</h2><p>This is the list of all keywords in the language. </p>
<h3 id="_005_keywords_swg_Special_keywords">Special keywords </h3>
<p>Keywords are predefined, reserved identifiers that have special meanings to the compiler. They can't be used as identifiers in your program. </p>
<div class="code-block"><span class="SCde"><span class="SLgc">if</span>
<span class="SLgc">else</span>
<span class="SLgc">elif</span>
<span class="SLgc">for</span>
<span class="SLgc">while</span>
<span class="SLgc">switch</span>
<span class="SLgc">defer</span>
<span class="SLgc">loop</span>
<span class="SLgc">visit</span>
<span class="SLgc">break</span>
<span class="SLgc">fallthrough</span>
<span class="SLgc">return</span>
<span class="SLgc">case</span>
<span class="SLgc">continue</span>
<span class="SLgc">default</span>
<span class="SLgc">and</span>
<span class="SLgc">or</span>
<span class="SLgc">orelse</span>
<span class="SLgc">unreachable</span>
<span class="SLgc">to</span>
<span class="SLgc">until</span>
<span class="SInv">do</span>
<span class="SLgc">where</span>
<span class="SLgc">in</span>

<span class="SKwd">true</span>
<span class="SKwd">false</span>
<span class="SKwd">null</span>
<span class="SKwd">undefined</span>

<span class="SKwd">using</span>
<span class="SKwd">with</span>
<span class="SKwd">cast</span>
<span class="SKwd">acast</span>
<span class="SKwd">dref</span>
<span class="SKwd">retval</span>
<span class="SKwd">try</span>
<span class="SKwd">trycatch</span>
<span class="SKwd">catch</span>
<span class="SKwd">assume</span>
<span class="SKwd">throw</span>
<span class="SKwd">discard</span>

<span class="SKwd">public</span>
<span class="SKwd">internal</span>
<span class="SKwd">private</span>

<span class="SKwd">enum</span>
<span class="SKwd">struct</span>
<span class="SKwd">union</span>
<span class="SKwd">impl</span>
<span class="SKwd">interface</span>
<span class="SKwd">func</span>
<span class="SKwd">closure</span>
<span class="SKwd">mtd</span>
<span class="SKwd">namespace</span>
<span class="SKwd">typealias</span>
<span class="SKwd">namealias</span>
<span class="SKwd">attr</span>
<span class="SKwd">var</span>
<span class="SKwd">let</span>
<span class="SKwd">const</span>
<span class="SKwd">moveref</span></span></div>
<h3 id="_005_keywords_swg_Reserved_keywords">Reserved keywords </h3>
<p>These keywords are reserved by the language, just in case... </p>
<div class="code-block"><span class="SCde"><span class="SInv">is</span>
<span class="SInv">not</span></span></div>
<h3 id="_005_keywords_swg_Basic_types">Basic types </h3>
<p>These are all the native types of the language. All are reserved keywords too. </p>
<div class="code-block"><span class="SCde"><span class="STpe">s8</span>
<span class="STpe">s16</span>
<span class="STpe">s32</span>
<span class="STpe">s64</span>
<span class="STpe">u8</span>
<span class="STpe">u16</span>
<span class="STpe">u32</span>
<span class="STpe">u64</span>
<span class="STpe">f32</span>
<span class="STpe">f64</span>
<span class="STpe">bool</span>
<span class="STpe">string</span>
<span class="STpe">rune</span>
<span class="STpe">any</span>
<span class="STpe">typeinfo</span>
<span class="STpe">void</span>
<span class="STpe">code</span>
<span class="STpe">cstring</span>
<span class="STpe">cvarargs</span></span></div>
<h3 id="_005_keywords_swg_Compiler_keywords">Compiler keywords </h3>
<p>Compiler keywords always start with <span class="code-inline">#</span>. As user identifiers cannot start the same way, compiler keywords will never collide with user identifiers. </p>
<div class="code-block"><span class="SCde"><span class="SCmp">#global</span>
<span class="STpe">#type</span>

<span class="SFct">#run</span>
<span class="SFct">#ast</span>
<span class="SFct">#test</span>
<span class="SFct">#init</span>
<span class="SFct">#drop</span>
<span class="SFct">#main</span>
<span class="SFct">#premain</span>
<span class="SFct">#message</span>
<span class="SFct">#dependencies</span>

<span class="SCmp">#include</span>
<span class="SCmp">#load</span>
<span class="SCmp">#assert</span>
<span class="SCmp">#print</span>
<span class="SCmp">#error</span>
<span class="SCmp">#warning</span>
<span class="SCmp">#foreignlib</span>
<span class="SCmp">#import</span>
<span class="SCmp">#mixin</span>
<span class="SCmp">#macro</span>
<span class="SCmp">#placeholder</span>
<span class="SCmp">#if</span>
<span class="SCmp">#else</span>
<span class="SCmp">#elif</span>
<span class="SCmp">#code</span>
<span class="SCmp">#scope</span>
<span class="SCmp">#up</span>

<span class="SCmp">#cfg</span>
<span class="SCmp">#os</span>
<span class="SCmp">#arch</span>
<span class="SCmp">#cpu</span>
<span class="SCmp">#backend</span>
<span class="SCmp">#module</span>
<span class="SCmp">#file</span>
<span class="SCmp">#line</span>
<span class="SCmp">#self</span>
<span class="SCmp">#location</span>
<span class="SCmp">#callerlocation</span>
<span class="SCmp">#callerfunction</span>
<span class="SCmp">#swagversion</span>
<span class="SCmp">#swagrevision</span>
<span class="SCmp">#swagbuildnum</span>
<span class="SCmp">#swagos</span>

<span class="SItr">#index</span>
<span class="SItr">#alias0</span>
<span class="SItr">#alias1</span>
<span class="SItr">#alias2</span> <span class="SCmt">// and more generally #aliasN</span>
<span class="SItr">#mix0</span>
<span class="SItr">#mix1</span>
<span class="SItr">#mix2</span> <span class="SCmt">// and more generally #mixinN</span></span></div>
<h3 id="_005_keywords_swg_Misc_intrinsics">Misc intrinsics </h3>
<p>Intrisic keywords always start with <span class="code-inline">@</span>. As user identifiers cannot start the same way, intrinsics keywords will never collide with user identifiers. </p>
<div class="code-block"><span class="SCde"><span class="SItr">@err</span>
<span class="SItr">@alignof</span>
<span class="SItr">@args</span>
<span class="SItr">@assert</span>
<span class="SItr">@breakpoint</span>
<span class="SItr">@compiler</span>
<span class="SItr">@countof</span>
<span class="SItr">@dataof</span>
<span class="SItr">@dbgalloc</span>
<span class="SItr">@defined</span>
<span class="SItr">@drop</span>
<span class="SItr">@compilererror</span>
<span class="SItr">@compilerwarning</span>
<span class="SItr">@getcontext</span>
<span class="SItr">@pinfos</span>
<span class="SItr">@gettag</span>
<span class="SItr">@hastag</span>
<span class="SItr">@init</span>
<span class="SItr">@isbytecode</span>
<span class="SItr">@isconstexpr</span>
<span class="SItr">@itftableof</span>
<span class="SItr">@kindof</span>
<span class="SItr">@location</span>
<span class="SItr">@mkany</span>
<span class="SItr">@mkcallback</span>
<span class="SItr">@mkinterface</span>
<span class="SItr">@mkslice</span>
<span class="SItr">@mkstring</span>
<span class="SItr">@decltype</span>
<span class="SItr">@modules</span>
<span class="SItr">@nameof</span>
<span class="SItr">@offsetof</span>
<span class="SItr">@panic</span>
<span class="SItr">@postcopy</span>
<span class="SItr">@postmove</span>
<span class="SItr">@print</span>
<span class="SItr">@rtflags</span>
<span class="SItr">@safety</span>
<span class="SItr">@setcontext</span>
<span class="SItr">@sizeof</span>
<span class="SItr">@spread</span>
<span class="SItr">@stringcmp</span>
<span class="SItr">@stringof</span>
<span class="SItr">@sysalloc</span>
<span class="SItr">@typecmp</span>
<span class="SItr">@typeof</span></span></div>
<h3 id="_005_keywords_swg_Intrinsics_libc">Intrinsics libc </h3>
<div class="code-block"><span class="SCde"><span class="SItr">@abs</span>
<span class="SItr">@acos</span>
<span class="SItr">@alloc</span>
<span class="SItr">@asin</span>
<span class="SItr">@atan</span>
<span class="SItr">@atomadd</span>
<span class="SItr">@atomand</span>
<span class="SItr">@atomcmpxchg</span>
<span class="SItr">@atomor</span>
<span class="SItr">@atomxchg</span>
<span class="SItr">@atomxor</span>
<span class="SItr">@bitcountlz</span>
<span class="SItr">@bitcountnz</span>
<span class="SItr">@bitcounttz</span>
<span class="SItr">@byteswap</span>
<span class="SItr">@ceil</span>
<span class="SItr">@cos</span>
<span class="SItr">@cosh</span>
<span class="SItr">@cvaarg</span>
<span class="SItr">@cvaend</span>
<span class="SItr">@cvastart</span>
<span class="SItr">@exp</span>
<span class="SItr">@exp2</span>
<span class="SItr">@floor</span>
<span class="SItr">@free</span>
<span class="SItr">@log</span>
<span class="SItr">@log10</span>
<span class="SItr">@log2</span>
<span class="SItr">@max</span>
<span class="SItr">@memcmp</span>
<span class="SItr">@memcpy</span>
<span class="SItr">@memmove</span>
<span class="SItr">@memset</span>
<span class="SItr">@muladd</span>
<span class="SItr">@min</span>
<span class="SItr">@pow</span>
<span class="SItr">@realloc</span>
<span class="SItr">@rol</span>
<span class="SItr">@ror</span>
<span class="SItr">@round</span>
<span class="SItr">@sin</span>
<span class="SItr">@sinh</span>
<span class="SItr">@sqrt</span>
<span class="SItr">@strcmp</span>
<span class="SItr">@strlen</span>
<span class="SItr">@tan</span>
<span class="SItr">@tanh</span>
<span class="SItr">@trunc</span></span></div>
<h3 id="_005_keywords_swg_Modifiers">Modifiers </h3>
<p>Modifiers can be applied to some specific keywords or operators to change their behaviours. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">#prom</span>
<span class="SKwd">#over</span>
<span class="SKwd">#nodrop</span>
<span class="SKwd">#move</span>
<span class="SKwd">#moveraw</span>
<span class="SKwd">#bit</span>
<span class="SKwd">#unconst</span>
<span class="SKwd">#back</span>
<span class="SKwd">#ref</span>
<span class="SKwd">#constref</span></span></div>

<h2 id="_006_semicolon_swg">Semicolon</h2><div class="code-block"><span class="SCde"><span class="SCmp">#global</span> skipfmt</span></div>
<p>In Swag, unlike in C/C++, there's no requirement to end statements with a semicolon (<span class="code-inline">;</span>). Typically, the end of a line (<span class="code-inline">end of line</span>) is sufficient to terminate a statement, resulting in cleaner and more concise syntax. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SCmt">// Declare two variables, x and y, of type s32 (signed 32-bit integer) and initialize them both to 1.</span>
    <span class="SKwd">var</span> x, y: <span class="STpe">s32</span> = <span class="SNum">1</span>

    <span class="SCmt">// Increment both x and y by 1.</span>
    x += <span class="SNum">1</span>
    y += <span class="SNum">1</span>

    <span class="SCmt">// Use the '@assert' intrinsic to verify the correctness of the values of x and y.</span>
    <span class="SItr">@assert</span>(x == <span class="SNum">2</span>)     <span class="SCmt">// Ensure that x equals 2, raising an error if it does not.</span>
    <span class="SItr">@assert</span>(y == x)     <span class="SCmt">// Ensure that y equals x (which should be 2).</span>
}</span></div>
<p>Although semicolons are optional, you can still use them when preferred or when they enhance readability in certain situations. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SCmt">// Note that type inference is used here, so the types of x and y are not explicitly specified.</span>
    <span class="SKwd">var</span> x, y = <span class="SNum">0</span>;  <span class="SCmt">// Both variables are initialized to 0, with their types inferred automatically.</span>

    <span class="SCmt">// The short syntax below applies the same operation to both variables.</span>
    x, y += <span class="SNum">1</span>;     <span class="SCmt">// Both x and y are incremented by 1.</span>
}</span></div>
<p>Semicolons can be particularly useful when writing multiple statements on the same line. This can make the code more compact, though it should be used sparingly to maintain readability. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SCmt">// Two statements on the same line, separated by a semicolon.</span>
    <span class="SKwd">var</span> x = <span class="SNum">0</span>; <span class="SKwd">var</span> y = <span class="SNum">0</span>

    <span class="SCmt">// Increment both x and y on the same line.</span>
    x += <span class="SNum">1</span>; y += <span class="SNum">1</span>

    <span class="SCmt">// Validate the values of x and y on the same line.</span>
    <span class="SItr">@assert</span>(x == <span class="SNum">1</span>); <span class="SItr">@assert</span>(y == <span class="SNum">1</span>)
}</span></div>

<h2 id="_007_global_declaration_order_swg">Global declaration order</h2><p>The order of all <b>top-level</b> declarations in Swag does not matter. This means that you can reference constants, variables, or functions before they are defined within the same file or even across multiple files. </p>
<div class="code-block"><span class="SCde"><span class="SCmt">// Here we declare a constant 'A' and initialize it with 'B', </span>
<span class="SCmt">// even though 'B' has not been declared or defined yet.</span>
<span class="SKwd">const</span> <span class="SCst">A</span> = <span class="SCst">B</span>

<span class="SCmt">// Now we declare a constant 'B' and initialize it with 'C',</span>
<span class="SCmt">// which is also not yet declared or defined.</span>
<span class="SKwd">const</span> <span class="SCst">B</span> = <span class="SCst">C</span>

<span class="SCmt">// Finally, we declare and define 'C' as a constant of type 'u64' </span>
<span class="SCmt">// (an unsigned 64-bit integer) with a value of 1.</span>
<span class="SCmt">// This retroactively defines 'A' and 'B' as well, based on the earlier assignments.</span>
<span class="SKwd">const</span> <span class="SCst">C</span>: <span class="STpe">u64</span> = <span class="SNum">1</span></span></div>
<p>In this test, we call the function <span class="code-inline">functionDeclaredLater</span> before it is defined. This is acceptable in Swag, demonstrating the flexibility of top-level declaration order. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#run</span>
{
    <span class="SCmt">// Here, we call the function 'functionDeclaredLater' before it is declared.</span>
    <span class="SCmt">// This works fine because Swag allows functions to be called before their definition.</span>
    <span class="SFct">functionDeclaredLater</span>()
}

<span class="SCmt">// The function is declared here, after it was called.</span>
<span class="SKwd">func</span> <span class="SFct">functionDeclaredLater</span>() {}</span></div>
<p>Note that the order of declarations is not only irrelevant within the same file but also across multiple files. For example, you can call a function in one file and define it in another. The global order of declarations does not matter! </p>

<h2 id="_010_basic_types_swg">Basic types</h2><div class="code-block"><span class="SCde"><span class="SCmp">#global</span> skipfmt</span></div>
<p>Swag provides various signed integer types: <span class="code-inline">s8</span>, <span class="code-inline">s16</span>, <span class="code-inline">s32</span>, and <span class="code-inline">s64</span>. These types represent signed integers with different bit widths. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">let</span> a: <span class="STpe">s8</span> = -<span class="SNum">1</span>      <span class="SCmt">// 8-bit signed integer</span>
    <span class="SKwd">let</span> b: <span class="STpe">s16</span> = -<span class="SNum">2</span>     <span class="SCmt">// 16-bit signed integer</span>
    <span class="SKwd">let</span> c: <span class="STpe">s32</span> = -<span class="SNum">3</span>     <span class="SCmt">// 32-bit signed integer</span>
    <span class="SKwd">let</span> d: <span class="STpe">s64</span> = -<span class="SNum">4</span>     <span class="SCmt">// 64-bit signed integer</span>

    <span class="SCmt">// The '@assert' intrinsic checks that the following conditions hold true.</span>
    <span class="SCmt">// If any of these conditions is false, an error will be raised at runtime.</span>
    <span class="SItr">@assert</span>(a == -<span class="SNum">1</span>)    <span class="SCmt">// Verifies that 'a' holds the value -1.</span>
    <span class="SItr">@assert</span>(b == -<span class="SNum">2</span>)    <span class="SCmt">// Verifies that 'b' holds the value -2.</span>
    <span class="SItr">@assert</span>(c == -<span class="SNum">3</span>)    <span class="SCmt">// Verifies that 'c' holds the value -3.</span>
    <span class="SItr">@assert</span>(d == -<span class="SNum">4</span>)    <span class="SCmt">// Verifies that 'd' holds the value -4.</span>

    <span class="SCmt">// The '@sizeof' intrinsic returns the size of a variable in bytes.</span>
    <span class="SItr">@assert</span>(<span class="SItr">@sizeof</span>(a) == <span class="SNum">1</span>) <span class="SCmt">// 'a' is an s8, so its size is 1 byte.</span>
    <span class="SItr">@assert</span>(<span class="SItr">@sizeof</span>(b) == <span class="SNum">2</span>) <span class="SCmt">// 'b' is an s16, so its size is 2 bytes.</span>
    <span class="SItr">@assert</span>(<span class="SItr">@sizeof</span>(c) == <span class="SNum">4</span>) <span class="SCmt">// 'c' is an s32, so its size is 4 bytes.</span>
    <span class="SItr">@assert</span>(<span class="SItr">@sizeof</span>(d) == <span class="SNum">8</span>) <span class="SCmt">// 'd' is an s64, so its size is 8 bytes.</span>
}</span></div>
<p>Swag also supports various unsigned integer types: <span class="code-inline">u8</span>, <span class="code-inline">u16</span>, <span class="code-inline">u32</span>, and <span class="code-inline">u64</span>. These types represent unsigned integers with different bit widths. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">let</span> a: <span class="STpe">u8</span> = <span class="SNum">1</span>      <span class="SCmt">// 8-bit unsigned integer</span>
    <span class="SKwd">let</span> b: <span class="STpe">u16</span> = <span class="SNum">2</span>     <span class="SCmt">// 16-bit unsigned integer</span>
    <span class="SKwd">let</span> c: <span class="STpe">u32</span> = <span class="SNum">3</span>     <span class="SCmt">// 32-bit unsigned integer</span>
    <span class="SKwd">let</span> d: <span class="STpe">u64</span> = <span class="SNum">4</span>     <span class="SCmt">// 64-bit unsigned integer</span>

    <span class="SItr">@assert</span>(a == <span class="SNum">1</span>)    <span class="SCmt">// Verifies that 'a' holds the value 1.</span>
    <span class="SItr">@assert</span>(b == <span class="SNum">2</span>)    <span class="SCmt">// Verifies that 'b' holds the value 2.</span>
    <span class="SItr">@assert</span>(c == <span class="SNum">3</span>)    <span class="SCmt">// Verifies that 'c' holds the value 3.</span>
    <span class="SItr">@assert</span>(d == <span class="SNum">4</span>)    <span class="SCmt">// Verifies that 'd' holds the value 4.</span>

    <span class="SItr">@assert</span>(<span class="SItr">@sizeof</span>(a) == <span class="SNum">1</span>) <span class="SCmt">// 'a' is a u8, so its size is 1 byte.</span>
    <span class="SItr">@assert</span>(<span class="SItr">@sizeof</span>(b) == <span class="SNum">2</span>) <span class="SCmt">// 'b' is a u16, so its size is 2 bytes.</span>
    <span class="SItr">@assert</span>(<span class="SItr">@sizeof</span>(c) == <span class="SNum">4</span>) <span class="SCmt">// 'c' is a u32, so its size is 4 bytes.</span>
    <span class="SItr">@assert</span>(<span class="SItr">@sizeof</span>(d) == <span class="SNum">8</span>) <span class="SCmt">// 'd' is a u64, so its size is 8 bytes.</span>
}</span></div>
<p>Swag supports floating-point types <span class="code-inline">f32</span> and <span class="code-inline">f64</span>. These types represent single-precision and double-precision floating-point numbers, respectively. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">let</span> a: <span class="STpe">f32</span> = <span class="SNum">3.14</span>        <span class="SCmt">// 32-bit floating-point value</span>
    <span class="SKwd">let</span> b: <span class="STpe">f64</span> = <span class="SNum">3.14159</span>     <span class="SCmt">// 64-bit floating-point value</span>

    <span class="SItr">@assert</span>(a == <span class="SNum">3.14</span>)       <span class="SCmt">// Verifies that 'a' holds the value 3.14.</span>
    <span class="SItr">@assert</span>(b == <span class="SNum">3.14159</span>)    <span class="SCmt">// Verifies that 'b' holds the value 3.14159.</span>

    <span class="SItr">@assert</span>(<span class="SItr">@sizeof</span>(a) == <span class="SNum">4</span>) <span class="SCmt">// 'a' is an f32, so its size is 4 bytes.</span>
    <span class="SItr">@assert</span>(<span class="SItr">@sizeof</span>(b) == <span class="SNum">8</span>) <span class="SCmt">// 'b' is an f64, so its size is 8 bytes.</span>
}</span></div>
<p>The boolean type <span class="code-inline">bool</span> is used to represent true or false values. In Swag, a boolean is stored as a 1-byte value. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">let</span> a: <span class="STpe">bool</span> = <span class="SKwd">true</span>      <span class="SCmt">// Boolean value, stored in 1 byte</span>
    <span class="SKwd">let</span> b: <span class="STpe">bool</span> = <span class="SKwd">false</span>     <span class="SCmt">// Boolean value, stored in 1 byte</span>

    <span class="SItr">@assert</span>(a == <span class="SKwd">true</span>)      <span class="SCmt">// Verifies that 'a' is true.</span>
    <span class="SItr">@assert</span>(b == <span class="SKwd">false</span>)     <span class="SCmt">// Verifies that 'b' is false.</span>

    <span class="SItr">@assert</span>(<span class="SItr">@sizeof</span>(a) == <span class="SNum">1</span>) <span class="SCmt">// The size of a boolean is 1 byte.</span>
    <span class="SItr">@assert</span>(<span class="SItr">@sizeof</span>(b) == <span class="SNum">1</span>) <span class="SCmt">// The size of a boolean is 1 byte.</span>
}</span></div>
<p>The <span class="code-inline">string</span> type represents text. In Swag, strings are <b>UTF-8</b> encoded and are stored as two 64-bit values (one for the pointer to the data and one for the length in bytes). Note that a string literal also ends with a null byte, similar to C strings. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">let</span> a: <span class="STpe">string</span> = <span class="SStr">"string 是"</span> <span class="SCmt">// A string containing UTF-8 characters</span>

    <span class="SItr">@assert</span>(a == <span class="SStr">"string 是"</span>)   <span class="SCmt">// Verifies that 'a' holds the correct string.</span>
    <span class="SItr">@assert</span>(<span class="SItr">@sizeof</span>(a) == <span class="SNum">2</span> * <span class="SItr">@sizeof</span>(*<span class="STpe">void</span>)) <span class="SCmt">// A string is stored as two 64-bit values (pointer and length).</span>
}</span></div>
<p>The <span class="code-inline">rune</span> type in Swag represents a 32-bit Unicode code point. It is used to store individual Unicode characters. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">let</span> a: <span class="STpe">rune</span> = <span class="SStr">`是`</span>           <span class="SCmt">// A single Unicode character</span>

    <span class="SItr">@assert</span>(a == <span class="SStr">`是`</span>)            <span class="SCmt">// Verifies that 'a' holds the correct Unicode code point.</span>
    <span class="SItr">@assert</span>(<span class="SItr">@sizeof</span>(a) == <span class="SNum">4</span>)      <span class="SCmt">// The size of a rune is 4 bytes (32 bits).</span>
}</span></div>
<h3 id="_010_basic_types_swg_Type_Reflection">Type Reflection </h3>
<p>Swag supports <b>type reflection</b> both at <b>compile time</b> and at <b>runtime</b>. This powerful feature allows the inspection and manipulation of types dynamically. More details on this will be provided later. </p>
<p>You can use <span class="code-inline">@decltype</span> to create a type based on an expression. This is useful for cases where you want to infer or mirror the type of a variable dynamically. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">let</span> a  = <span class="SNum">0</span>                  <span class="SCmt">// The type of 'a' is inferred to be 's32'.</span>
    <span class="SKwd">let</span> b: <span class="SItr">@decltype</span>(a) = <span class="SNum">1</span>     <span class="SCmt">// 'b' is declared with the same type as 'a' (which is 's32').</span>

    <span class="SCmt">// '@typeof' is used to check the type of a variable.</span>
    <span class="SItr">@assert</span>(<span class="SItr">@typeof</span>(a) == <span class="SItr">@typeof</span>(b)) <span class="SCmt">// Verifies that 'a' and 'b' have the same type.</span>
    <span class="SItr">@assert</span>(<span class="SItr">@typeof</span>(a) == <span class="STpe">s32</span>)        <span class="SCmt">// Verifies that the type of 'a' is 's32'.</span>

    <span class="SCmt">// Since the types of 'a' and 'b' are known at compile time, we can use '#assert' for compile-time validation.</span>
    <span class="SCmt">// '#assert' checks the condition during compilation and does not generate runtime code.</span>
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(a) == <span class="SItr">@typeof</span>(b)
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(a) == <span class="STpe">s32</span>
}</span></div>
<p>Types in Swag are also treated as values, both at compile time and at runtime. This allows types to be manipulated, compared, and inspected just like any other value. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">let</span> x = <span class="SItr">@typeof</span>(<span class="STpe">s32</span>) <span class="SCmt">// 'x' is now a variable that holds a type (in this case, 's32').</span>
    <span class="SItr">@assert</span>(x == <span class="STpe">s32</span>)    <span class="SCmt">// Verifies that 'x' is equal to the type 's32'.</span>

    <span class="SCmt">// Types in Swag are predefined structs with fields that can be inspected.</span>
    <span class="SCmt">// For example, you can retrieve the name of the type.</span>
    <span class="SItr">@assert</span>(x.name == <span class="SStr">"s32"</span>) <span class="SCmt">// Retrieves and verifies the name of the type 's32'.</span>

    <span class="SCmt">// '@typeof' is often unnecessary when there is no ambiguity in the expression.</span>
    <span class="SKwd">let</span> y = <span class="STpe">bool</span>
    <span class="SItr">@assert</span>(y == <span class="STpe">bool</span>)       <span class="SCmt">// Verifies that 'y' holds the type 'bool'.</span>
}</span></div>

<h2 id="_011_number_literals_swg">Number literals</h2><p>In Swag, integers can be written in <i>decimal</i>, <i>hexadecimal</i>, or <i>binary</i> forms. These different representations allow you to express numbers in the format that best suits your needs. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">const</span> a: <span class="STpe">u32</span> = <span class="SNum">123456</span>         <span class="SCmt">// Decimal format</span>
    <span class="SKwd">const</span> b: <span class="STpe">u32</span> = <span class="SNum">0xFFFF</span>         <span class="SCmt">// Hexadecimal format, prefixed with '0x'</span>
    <span class="SKwd">const</span> c: <span class="STpe">u32</span> = <span class="SNum">0b00001111</span>     <span class="SCmt">// Binary format, prefixed with '0b'</span>

    <span class="SItr">@assert</span>(a == <span class="SNum">123456</span>)          <span class="SCmt">// Verifies that 'a' holds the correct decimal value</span>
    <span class="SItr">@assert</span>(b == <span class="SNum">65535</span>)           <span class="SCmt">// Verifies that 'b' holds the correct hexadecimal value</span>
    <span class="SItr">@assert</span>(c == <span class="SNum">15</span>)              <span class="SCmt">// Verifies that 'c' holds the correct binary value</span>
}</span></div>
<p>You can separate digits within numeric literals using the <span class="code-inline">_</span> character for better readability, especially with large numbers. This does not affect the value of the number. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">const</span> a: <span class="STpe">u32</span> = <span class="SNum">123_456</span>         <span class="SCmt">// Decimal with digit separators</span>
    <span class="SKwd">const</span> b: <span class="STpe">u32</span> = <span class="SNum">0xF_F_F_F</span>       <span class="SCmt">// Hexadecimal with digit separators</span>
    <span class="SKwd">const</span> c: <span class="STpe">u32</span> = <span class="SNum">0b0000_1111</span>     <span class="SCmt">// Binary with digit separators</span>

    <span class="SItr">@assert</span>(a == <span class="SNum">123456</span>)           <span class="SCmt">// Verifies that 'a' holds the correct value</span>
    <span class="SItr">@assert</span>(b == <span class="SNum">65535</span>)            <span class="SCmt">// Verifies that 'b' holds the correct value</span>
    <span class="SItr">@assert</span>(c == <span class="SNum">15</span>)               <span class="SCmt">// Verifies that 'c' holds the correct value</span>
}</span></div>
<p>The default type of a hexadecimal or binary number in Swag is <span class="code-inline">u32</span> if it fits within 32 bits. If the value exceeds 32 bits, the type is automatically inferred as <span class="code-inline">u64</span>. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SCmt">// The compiler will deduce that the type of 'a' is 'u32' since the value fits within 32 bits.</span>
    <span class="SKwd">const</span> a = <span class="SNum">0xFF</span>
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(a) == <span class="STpe">u32</span>      <span class="SCmt">// Verifies that 'a' is of type 'u32'</span>

    <span class="SCmt">// The compiler will deduce that the type of 'b' is 'u64' because the constant</span>
    <span class="SCmt">// is too large to fit within 32 bits.</span>
    <span class="SKwd">const</span> b = <span class="SNum">0xF_FFFFF_FFFFFF</span>
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(b) == <span class="STpe">u64</span>      <span class="SCmt">// Verifies that 'b' is of type 'u64'</span>

    <span class="SKwd">const</span> c = <span class="SNum">0b00000001</span>           <span class="SCmt">// Binary value within 32 bits</span>
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(c) == <span class="STpe">u32</span>      <span class="SCmt">// Verifies that 'c' is of type 'u32'</span>

    <span class="SKwd">const</span> d = <span class="SNum">0b00000001_00000001_00000001_00000001_00000001</span>  <span class="SCmt">// Binary value that exceeds 32 bits</span>
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(d) == <span class="STpe">u64</span>      <span class="SCmt">// Verifies that 'd' is of type 'u64'</span>
}</span></div>
<p>A boolean in Swag can be either <span class="code-inline">true</span> or <span class="code-inline">false</span>. Since constants are known at compile time, we can use <span class="code-inline">#assert</span> to check their values directly during compilation. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">const</span> a = <span class="SKwd">true</span>
    <span class="SCmp">#assert</span> a == <span class="SKwd">true</span>              <span class="SCmt">// Compile-time check that 'a' is true</span>

    <span class="SKwd">const</span> b, c = <span class="SKwd">false</span>
    <span class="SCmp">#assert</span> b == <span class="SKwd">false</span>             <span class="SCmt">// Compile-time check that 'b' is false</span>
    <span class="SCmp">#assert</span> c == <span class="SKwd">false</span>             <span class="SCmt">// Compile-time check that 'c' is false</span>
}</span></div>
<p>Floating point values in Swag follow the standard C/C++ notation for floating-point literals. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">let</span> a = <span class="SNum">1.5</span>
    <span class="SItr">@assert</span>(a == <span class="SNum">1.5</span>)              <span class="SCmt">// Verifies that 'a' holds the value 1.5</span>
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(a) == <span class="STpe">f32</span>      <span class="SCmt">// The type of 'a' is inferred to be 'f32'</span>

    <span class="SKwd">let</span> b = <span class="SNum">0.11</span>
    <span class="SItr">@assert</span>(b == <span class="SNum">0.11</span>)             <span class="SCmt">// Verifies that 'b' holds the value 0.11</span>

    <span class="SKwd">let</span> c = <span class="SNum">15e2</span>
    <span class="SItr">@assert</span>(c == <span class="SNum">15e2</span>)             <span class="SCmt">// Verifies that 'c' holds the value 15e2 (1500)</span>

    <span class="SKwd">let</span> d = <span class="SNum">15e+2</span>
    <span class="SItr">@assert</span>(d == <span class="SNum">15e2</span>)             <span class="SCmt">// Verifies that 'd' holds the same value as 'c'</span>

    <span class="SKwd">let</span> e = -<span class="SNum">1E-1</span>
    <span class="SItr">@assert</span>(e == -<span class="SNum">0.1</span>)             <span class="SCmt">// Verifies that 'e' holds the value -0.1</span>
}</span></div>
<p>By default, floating point literals in Swag are of type <span class="code-inline">f32</span>, unlike in C/C++, where they default to <span class="code-inline">double</span> (<span class="code-inline">f64</span>).  </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">let</span> a = <span class="SNum">1.5</span>
    <span class="SItr">@assert</span>(a == <span class="SNum">1.5</span>)              <span class="SCmt">// Verifies that 'a' holds the value 1.5</span>
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(a) == <span class="STpe">f32</span>      <span class="SCmt">// Verifies that 'a' is of type 'f32'</span>
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(a) != <span class="STpe">f64</span>      <span class="SCmt">// Verifies that 'a' is not of type 'f64'</span>
}</span></div>
<h3 id="_011_number_literals_swg_Suffix">Suffix </h3>
<p>You can explicitly set the type of a literal number by adding a <b>suffix</b>. This is useful when you need a specific type, such as <span class="code-inline">f64</span> or <span class="code-inline">u8</span>. We will explore how this syntax is used for generic arguments later on. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SCmt">// Declare 'a' to be of type 'f64' and assign it the value '1.5'</span>
    <span class="SKwd">let</span> a = <span class="SNum">1.5</span>'<span class="STpe">f64</span>
    <span class="SItr">@assert</span>(a == <span class="SNum">1.5</span>)              <span class="SCmt">// Verifies that 'a' holds the value 1.5</span>
    <span class="SItr">@assert</span>(a == <span class="SNum">1.5</span>'<span class="STpe">f64</span>)          <span class="SCmt">// Verifies that 'a' holds the value 1.5 explicitly as 'f64'</span>
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(a) == <span class="STpe">f64</span>      <span class="SCmt">// Verifies that 'a' is of type 'f64'</span>

    <span class="SCmt">// 'b' is a variable of type 'u8', initialized with the value '10'</span>
    <span class="SKwd">let</span> b = <span class="SNum">10</span>'<span class="STpe">u8</span>
    <span class="SItr">@assert</span>(b == <span class="SNum">10</span>)               <span class="SCmt">// Verifies that 'b' holds the value 10</span>
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(b) == <span class="STpe">u8</span>       <span class="SCmt">// Verifies that 'b' is of type 'u8'</span>

    <span class="SKwd">let</span> c = <span class="SNum">1</span>'<span class="STpe">u32</span>
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(c) == <span class="STpe">u32</span>      <span class="SCmt">// Verifies that 'c' is of type 'u32'</span>
}</span></div>

<h2 id="_012_string_swg">String</h2><p>In Swag, strings are encoded in UTF-8, allowing them to support a wide range of characters from different languages and symbol sets. </p>
<p>Strings can be compared directly for equality. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">const</span> a = <span class="SStr">"this is a Chinese character: 是"</span>
    <span class="SCmp">#assert</span> a == <span class="SStr">"this is a Chinese character: 是"</span>

    <span class="SKwd">const</span> b = <span class="SStr">"these are some Cyrillic characters: ӜИ"</span>
    <span class="SCmp">#assert</span> b == <span class="SStr">"these are some Cyrillic characters: ӜИ"</span>
}</span></div>
<p>A <span class="code-inline">rune</span> in Swag represents a Unicode code point and is stored as a 32-bit value. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">const</span> a: <span class="STpe">rune</span> = <span class="SStr">`是`</span>
    <span class="SCmp">#assert</span> a == <span class="SStr">`是`</span>
    <span class="SCmp">#assert</span> <span class="SItr">@sizeof</span>(a) == <span class="SItr">@sizeof</span>(<span class="STpe">u32</span>)  <span class="SCmt">// Ensures that a `rune` is the same size as a 32-bit integer.</span>
}</span></div>
<div class="blockquote blockquote-warning">
<div class="blockquote-title-block"><i class="fa fa-exclamation-triangle"></i>  <span class="blockquote-title">Warning</span></div><p> You cannot index a string to directly retrieve a <span class="code-inline">rune</span>, except in ASCII strings. This is because Swag does not include the runtime cost of UTF-8 encoding/decoding in its string operations. However, the <span class="code-inline">Std.Core</span> module provides utilities to handle UTF-8 strings effectively. </p>
</div>
<p>When you index a string, you retrieve a byte rather than a <span class="code-inline">rune</span>. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">const</span> a = <span class="SStr">"this is a Chinese character: 是"</span>

    <span class="SCmt">// Retrieves the first byte of the string.</span>
    <span class="SKwd">const</span> b = a[<span class="SNum">0</span>]
    <span class="SCmp">#assert</span> b == <span class="SStr">`t`</span>
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(b) == <span class="SItr">@typeof</span>(<span class="STpe">u8</span>)  <span class="SCmt">// Confirms that the result is a byte (u8).</span>

    <span class="SCmt">// The 'X' character here does not have the index '1' because the preceding Chinese character</span>
    <span class="SCmt">// is encoded in UTF-8, which uses multiple bytes.</span>
    <span class="SKwd">const</span> c = <span class="SStr">"是X是"</span>
    <span class="SCmp">#assert</span> c[<span class="SNum">1</span>] != <span class="SStr">`X`</span> <span class="SCmt">// False because byte 1 is not the character 'X'</span>
}</span></div>
<p>You can concatenate strings and other values at compile time using the <span class="code-inline">++</span> operator. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">const</span> a = <span class="SStr">"the devil's number is "</span> ++ <span class="SNum">666</span>
    <span class="SCmp">#assert</span> a == <span class="SStr">"the devil's number is 666"</span>

    <span class="SKwd">const</span> b = <span class="SNum">666</span>
    <span class="SKwd">let</span> c   = <span class="SStr">"the devil's number is not "</span> ++ (b + <span class="SNum">1</span>) ++ <span class="SStr">"!"</span>
    <span class="SItr">@assert</span>(c == <span class="SStr">"the devil's number is not 667!"</span>)

    <span class="SKwd">let</span> d = <span class="SStr">"there are "</span> ++ <span class="SNum">4</span> ++ <span class="SStr">" apples in "</span> ++ (<span class="SNum">2</span> * <span class="SNum">2</span>) ++ <span class="SStr">" baskets"</span>
    <span class="SItr">@assert</span>(d == <span class="SStr">"there are 4 apples in 4 baskets"</span>)
}</span></div>
<p>A string can be <span class="code-inline">null</span> if it has not been defined. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> a: <span class="STpe">string</span>
    <span class="SItr">@assert</span>(a == <span class="SKwd">null</span>)  <span class="SCmt">// Initially, 'a' is null.</span>
    a = <span class="SStr">"not null"</span>
    <span class="SItr">@assert</span>(a != <span class="SKwd">null</span>)  <span class="SCmt">// Now 'a' is not null.</span>
    a = <span class="SKwd">null</span>
    <span class="SItr">@assert</span>(a == <span class="SKwd">null</span>)  <span class="SCmt">// 'a' is null again.</span>
}</span></div>
<h3 id="_012_string_swg_Character_literals">Character literals </h3>
<p>A <i>character</i> is represented using backticks. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">let</span> char0 = <span class="SStr">`a`</span>
    <span class="SKwd">let</span> char1 = <span class="SStr">`我`</span>
}</span></div>
<p>By default, a character literal is a 32-bit integer that can be assigned to any integer type or to a <span class="code-inline">rune</span>, as long as the value fits within the target type. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    {
        <span class="SKwd">let</span> a: <span class="STpe">u8</span> = <span class="SStr">`a`</span>
        <span class="SKwd">let</span> b: <span class="STpe">u16</span> = <span class="SStr">`a`</span>
        <span class="SKwd">let</span> c: <span class="STpe">u32</span> = <span class="SStr">`我`</span>
        <span class="SKwd">let</span> d: <span class="STpe">u64</span> = <span class="SStr">`我`</span>
        <span class="SKwd">let</span> e: <span class="STpe">rune</span> = <span class="SStr">`我`</span>
    }

    {
        <span class="SKwd">let</span> a: <span class="STpe">s8</span> = <span class="SStr">`a`</span>
        <span class="SKwd">let</span> b: <span class="STpe">s16</span> = <span class="SStr">`a`</span>
        <span class="SKwd">let</span> c: <span class="STpe">s32</span> = <span class="SStr">`我`</span>
        <span class="SKwd">let</span> d: <span class="STpe">s64</span> = <span class="SStr">`我`</span>
    }
}</span></div>
<p>However, you can specify the underlying type of a character literal by using a type suffix. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">let</span> a = <span class="SStr">`0`</span>'<span class="STpe">u8</span>
    <span class="SItr">@assert</span>(a == <span class="SNum">48</span>)              <span class="SCmt">// ASCII value of '0' is 48.</span>
    <span class="SItr">@assert</span>(<span class="SItr">@typeof</span>(a) == <span class="STpe">u8</span>)

    <span class="SKwd">let</span> b = <span class="SStr">`1`</span>'<span class="STpe">u16</span>
    <span class="SItr">@assert</span>(b == <span class="SNum">49</span>)              <span class="SCmt">// ASCII value of '1' is 49.</span>
    <span class="SItr">@assert</span>(<span class="SItr">@typeof</span>(b) == <span class="STpe">u16</span>)

    <span class="SKwd">let</span> c = <span class="SStr">`2`</span>'<span class="STpe">u32</span>
    <span class="SItr">@assert</span>(c == <span class="SNum">50</span>)              <span class="SCmt">// ASCII value of '2' is 50.</span>
    <span class="SItr">@assert</span>(<span class="SItr">@typeof</span>(c) == <span class="STpe">u32</span>)

    <span class="SKwd">let</span> d = <span class="SStr">`3`</span>'<span class="STpe">u64</span>
    <span class="SItr">@assert</span>(d == <span class="SNum">51</span>)              <span class="SCmt">// ASCII value of '3' is 51.</span>
    <span class="SItr">@assert</span>(<span class="SItr">@typeof</span>(d) == <span class="STpe">u64</span>)

    <span class="SKwd">let</span> e = <span class="SStr">`4`</span>'<span class="STpe">rune</span>
    <span class="SItr">@assert</span>(e == <span class="SNum">52</span>)              <span class="SCmt">// ASCII value of '4' is 52.</span>
    <span class="SItr">@assert</span>(<span class="SItr">@typeof</span>(e) == <span class="STpe">rune</span>)
}</span></div>
<h3 id="_012_string_swg_Escape_sequences">Escape sequences </h3>
<p>Strings and character literals can contain <i>escape sequences</i> to represent special characters. </p>
<p>An escape sequence begins with a backslash <span class="code-inline">`.</code> </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">const</span> a = <span class="SStr">"this is ASCII code 0x00:   \0"</span>     <span class="SCmt">// null</span>
    <span class="SKwd">const</span> b = <span class="SStr">"this is ASCII code 0x07:   \a"</span>     <span class="SCmt">// bell</span>
    <span class="SKwd">const</span> c = <span class="SStr">"this is ASCII code 0x08:   \b"</span>     <span class="SCmt">// backspace</span>
    <span class="SKwd">const</span> d = <span class="SStr">"this is ASCII code 0x09:   \t"</span>     <span class="SCmt">// horizontal tab</span>
    <span class="SKwd">const</span> e = <span class="SStr">"this is ASCII code 0x0A:   \n"</span>     <span class="SCmt">// line feed</span>
    <span class="SKwd">const</span> f = <span class="SStr">"this is ASCII code 0x0B:   \v"</span>     <span class="SCmt">// vertical tab</span>
    <span class="SKwd">const</span> g = <span class="SStr">"this is ASCII code 0x0C:   \f"</span>     <span class="SCmt">// form feed</span>
    <span class="SKwd">const</span> h = <span class="SStr">"this is ASCII code 0x0D:   \r"</span>     <span class="SCmt">// carriage return</span>
    <span class="SKwd">const</span> i = <span class="SStr">"this is ASCII code 0x22:   \""</span>     <span class="SCmt">// double quote</span>
    <span class="SKwd">const</span> j = <span class="SStr">"this is ASCII code 0x27:   \'"</span>     <span class="SCmt">// single quote</span>
    <span class="SKwd">const</span> k = <span class="SStr">"this is ASCII code 0x60:   \`"</span>     <span class="SCmt">// backtick</span>
    <span class="SKwd">const</span> l = <span class="SStr">"this is ASCII code 0x5C:   \\"</span>     <span class="SCmt">// backslash</span>
}</span></div>
<p>Escape sequences can also specify ASCII or Unicode values. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    {
        <span class="SKwd">const</span> a = <span class="SStr">"\x26"</span>           <span class="SCmt">// 1 byte, hexadecimal, extended ASCII</span>
        <span class="SKwd">const</span> b = <span class="SStr">"\u2626"</span>         <span class="SCmt">// 2 bytes, hexadecimal, Unicode 16-bit</span>
        <span class="SKwd">const</span> c = <span class="SStr">"\U00101234"</span>     <span class="SCmt">// 4 bytes, hexadecimal, Unicode 32-bit</span>
    }

    {
        <span class="SKwd">const</span> d = <span class="SStr">"\u2F46\u2F46"</span>
        <span class="SCmp">#assert</span> d == <span class="SStr">"⽆⽆"</span>

        <span class="SKwd">const</span> e = <span class="SStr">`\u2F46`</span>
        <span class="SCmp">#assert</span> e == <span class="SStr">`⽆`</span>
    }
}</span></div>
<h3 id="_012_string_swg_Raw_strings">Raw strings </h3>
<p>A <i>raw string</i> does not process escape sequences or special characters within it. </p>
<p>A raw string starts and ends with the <span class="code-inline">#</span> character. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">const</span> a = <span class="SStr">#"\u2F46"#</span>
    <span class="SCmp">#assert</span> a != <span class="SStr">"⽆"</span>               <span class="SCmt">// The raw string does not interpret \u2F46.</span>
    <span class="SCmp">#assert</span> a == <span class="SStr">#"\u2F46"#</span>         <span class="SCmt">// The raw string matches the literal content.</span>
}</span></div>
<p>These two strings are equivalent: </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">const</span> a = <span class="SStr">"\\hello \\world"</span>     <span class="SCmt">// Using escape sequences</span>
    <span class="SKwd">const</span> b = <span class="SStr">#"\hello \world"#</span>     <span class="SCmt">// As a raw string, no escaping needed</span>
    <span class="SCmp">#assert</span> a == b                  <span class="SCmt">// Both strings are identical.</span>
}</span></div>
<p>A raw string can span multiple lines because the line feed is part of the string. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">const</span> a = <span class="SStr">#"this is
                a
                string
                "#</span>
}</span></div>
<p>In a multi-line raw string, all leading spaces before the closing <span class="code-inline">"#</span> are removed from each line. So, the above string is equivalent to: </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SCmt">// this is</span>
    <span class="SCmt">// a</span>
    <span class="SCmt">// string</span>
}</span></div>
<h3 id="_012_string_swg_Multiline_strings">Multiline strings </h3>
<p>A multiline string starts and ends with <span class="code-inline">"""</span>. Unlike raw strings, multiline strings still process escape sequences. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">const</span> a = <span class="SStr">""</span><span class="SStr">"this is
                 a
                 string
                 "</span><span class="SStr">""</span>

    <span class="SCmt">// Equivalent to:</span>

    <span class="SCmt">// this is</span>
    <span class="SCmt">// a</span>
    <span class="SCmt">// string</span>
}</span></div>
<p>In a multiline or raw string, if you end a line with <span class="code-inline">`, the following 'end of line' (EOL) will **not** be included in the string.</code> </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">const</span> a = <span class="SStr">""</span><span class="SStr">"\
              this is
              a
              string
              "</span><span class="SStr">""</span>
    <span class="SCmt">// Is equivalent to:</span>
    <span class="SCmt">// this is</span>
    <span class="SCmt">// a</span>
    <span class="SCmt">// string</span>
}</span></div>
<h3 id="_012_string_swg_@stringof_and_@nameof">@stringof and @nameof </h3>
<p>The <span class="code-inline">@stringof</span> intrinsic returns the string representation of a constant expression at compile time. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">const</span> <span class="SCst">X</span> = <span class="SNum">1</span>
    <span class="SCmp">#assert</span> <span class="SItr">@stringof</span>(<span class="SCst">X</span>) == <span class="SStr">"1"</span>
    <span class="SCmp">#assert</span> <span class="SItr">@stringof</span>(<span class="SCst">X</span> + <span class="SNum">10</span>) == <span class="SStr">"11"</span>
}</span></div>
<p>You can use the <span class="code-inline">@nameof</span> intrinsic to return the name of a variable, function, etc., as a string. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">const</span> <span class="SCst">X</span> = <span class="SNum">1</span>
    <span class="SCmp">#assert</span> <span class="SItr">@nameof</span>(<span class="SCst">X</span>) == <span class="SStr">"X"</span>
}</span></div>

<h2 id="_013_variables_swg">Variables</h2><p>To declare a variable in Swag, you use the <span class="code-inline">let</span> or <span class="code-inline">var</span> keyword, followed by a <span class="code-inline">:</span> and then the type. </p>
<p>- <span class="code-inline">let</span> is used for a variable that cannot be changed after its initial assignment. It's immutable. - <span class="code-inline">var</span> is used for a variable that can be changed after its initial assignment. It's mutable. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SCmt">// 'a' is a constant variable of type 'u32' and is initialized with the value 1. It cannot be modified later.</span>
    <span class="SKwd">let</span> a: <span class="STpe">u32</span> = <span class="SNum">1</span>
    <span class="SItr">@assert</span>(a == <span class="SNum">1</span>)

    <span class="SCmt">// 'b' is a constant variable of type 'string'.</span>
    <span class="SKwd">let</span> b: <span class="STpe">string</span> = <span class="SStr">"string"</span>
    <span class="SItr">@assert</span>(b == <span class="SStr">"string"</span>)

    <span class="SCmt">// 'c' is a mutable variable of type 's32', so its value can be changed.</span>
    <span class="SKwd">var</span> c: <span class="STpe">s32</span> = <span class="SNum">42</span>
    c += <span class="SNum">1</span>
    <span class="SItr">@assert</span>(c == <span class="SNum">43</span>)  <span class="SCmt">// Verifies that 'c' has been correctly incremented.</span>
}</span></div>
<p>We can also declare multiple variables on the same line. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">let</span> a, b: <span class="STpe">u32</span> = <span class="SNum">123</span>  <span class="SCmt">// Both 'a' and 'b' are of type 'u32' and are initialized with the value 123.</span>
    <span class="SItr">@assert</span>(a == <span class="SNum">123</span>)
    <span class="SItr">@assert</span>(b == <span class="SNum">123</span>)
}</span></div>
<p>Alternatively, you can declare variables of different types on the same line. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">let</span> a: <span class="STpe">u32</span> = <span class="SNum">12</span>, b: <span class="STpe">f32</span> = <span class="SNum">1.5</span>  <span class="SCmt">// 'a' is of type 'u32' and 'b' is of type 'f32'.</span>
    <span class="SItr">@assert</span>(a == <span class="SNum">12</span>)
    <span class="SItr">@assert</span>(b == <span class="SNum">1.5</span>)
}</span></div>
<p>If you don't assign an initial value, the variable will be automatically initialized with its default value. Therefore, a variable in Swag is <b>always</b> initialized. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> a: <span class="STpe">bool</span>
    <span class="SItr">@assert</span>(a == <span class="SKwd">false</span>)  <span class="SCmt">// The default value for a boolean is 'false'.</span>

    <span class="SKwd">var</span> b: <span class="STpe">string</span>
    <span class="SItr">@assert</span>(b == <span class="SKwd">null</span>)  <span class="SCmt">// The default value for a string is 'null'.</span>

    <span class="SKwd">var</span> c: <span class="STpe">f64</span>
    <span class="SItr">@assert</span>(c == <span class="SNum">0</span>)  <span class="SCmt">// The default value for a floating-point number is 0.</span>
}</span></div>
<p>However, if you want a variable to remain uninitialized (and avoid the cost of initialization), you can assign it <span class="code-inline">undefined</span>. This should be used with caution as it leaves the variable in an undefined state. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> a: <span class="STpe">bool</span> = <span class="SKwd">undefined</span>  <span class="SCmt">// 'a' is left uninitialized.</span>
    <span class="SKwd">var</span> b: <span class="STpe">string</span> = <span class="SKwd">undefined</span>  <span class="SCmt">// 'b' is left uninitialized.</span>
}</span></div>
<p>As we have seen, the type in a declaration is optional if it can be deduced from the assigned value. This feature is known as <b>type inference</b>. </p>
<p>Here are some examples of <b>type inference</b>. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">let</span> a = <span class="SNum">1.5</span>  <span class="SCmt">// The type of 'a' is inferred to be 'f32' because 1.5 is a floating-point literal.</span>
    <span class="SItr">@assert</span>(a == <span class="SNum">1.5</span>)
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(a) == <span class="STpe">f32</span>

    <span class="SKwd">let</span> b = <span class="SStr">"string"</span>  <span class="SCmt">// The type of 'b' is inferred to be 'string'.</span>
    <span class="SItr">@assert</span>(b == <span class="SStr">"string"</span>)
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(b) == <span class="STpe">string</span>

    <span class="SKwd">let</span> c = <span class="SNum">1.5</span>'<span class="STpe">f64</span>  <span class="SCmt">// The type of 'c' is explicitly set to 'f64' using a suffix.</span>
    <span class="SItr">@assert</span>(c == <span class="SNum">1.5</span>)
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(c) == <span class="STpe">f64</span>
}</span></div>
<p>Type inference also works when declaring multiple variables. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">let</span> a, b = <span class="SKwd">true</span>  <span class="SCmt">// Both 'a' and 'b' are inferred to be of type 'bool'.</span>
    <span class="SItr">@assert</span>(a == <span class="SKwd">true</span>)
    <span class="SItr">@assert</span>(b == <span class="SKwd">true</span>)
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(a) == <span class="SItr">@typeof</span>(<span class="SKwd">true</span>)
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(b) == <span class="SItr">@typeof</span>(a)

    <span class="SKwd">let</span> c = <span class="SNum">1.5</span>, d = <span class="SStr">"string"</span>  <span class="SCmt">// 'c' is inferred to be 'f32', and 'd' is inferred to be 'string'.</span>
    <span class="SItr">@assert</span>(c == <span class="SNum">1.5</span>)
    <span class="SItr">@assert</span>(d == <span class="SStr">"string"</span>)
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(c) == <span class="STpe">f32</span>
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(d) == <span class="STpe">string</span>
}</span></div>
<h3 id="_013_variables_swg_Special_variables">Special variables </h3>
<p>A global variable can be tagged with <span class="code-inline">#[Swag.Tls]</span> to store it in thread-local storage, meaning each thread will have its own copy of the variable. </p>
<div class="code-block"><span class="SCde"><span class="SAtr">#[Swag.Tls]</span>
<span class="SKwd">var</span> <span class="SCst">G</span> = <span class="SNum">0</span>  <span class="SCmt">// 'G' is a global variable with thread-local storage.</span></span></div>
<p>A local variable can be tagged with <span class="code-inline">#[Swag.Global]</span> to make it global, similar to the <span class="code-inline">static</span> keyword in C/C++. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">func</span> <span class="SFct">toto</span>()-&gt;<span class="STpe">s32</span>
    {
        <span class="SAtr">#[Swag.Global]</span>
        <span class="SKwd">var</span> <span class="SCst">G1</span> = <span class="SNum">0</span>  <span class="SCmt">// 'G1' is a static-like variable, retaining its value between function calls.</span>

        <span class="SCst">G1</span> += <span class="SNum">1</span>
        <span class="SLgc">return</span> <span class="SCst">G1</span>
    }

    <span class="SItr">@assert</span>(<span class="SFct">toto</span>() == <span class="SNum">1</span>)  <span class="SCmt">// First call increments G1 to 1.</span>
    <span class="SItr">@assert</span>(<span class="SFct">toto</span>() == <span class="SNum">2</span>)  <span class="SCmt">// Second call increments G1 to 2.</span>
    <span class="SItr">@assert</span>(<span class="SFct">toto</span>() == <span class="SNum">3</span>)  <span class="SCmt">// Third call increments G1 to 3.</span>
}</span></div>
<p>A global variable can also be marked with <span class="code-inline">#[Swag.Compiler]</span>. Such variables are only accessible during compile-time and are not included in the runtime code. </p>
<div class="code-block"><span class="SCde"><span class="SAtr">#[Swag.Compiler]</span>
<span class="SKwd">var</span> <span class="SCst">G2</span> = <span class="SNum">0</span>  <span class="SCmt">// 'G2' is a compile-time only variable.</span>

<span class="SFct">#run</span>
{
    <span class="SCst">G2</span> += <span class="SNum">5</span>  <span class="SCmt">// This increment happens at compile-time.</span>
}</span></div>

<h2 id="_014_const_swg">Const</h2><p>If you use <span class="code-inline">const</span> instead of <span class="code-inline">var</span> or <span class="code-inline">let</span>, the value must be <b>known by the compiler</b> at compile time. This means that the value of a <span class="code-inline">const</span> is evaluated during compilation, and there's no runtime memory footprint for simple types like integers or strings. The compiler simply replaces the use of these constants with their values wherever they are used in the code. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SCmt">// These are constants, not variables. Once declared, they cannot be modified.</span>
    <span class="SKwd">const</span> a = <span class="SNum">666</span>  <span class="SCmt">// 'a' is a constant integer known at compile time.</span>
    <span class="SCmp">#assert</span> a == <span class="SNum">666</span>

    <span class="SKwd">const</span> b: <span class="STpe">string</span> = <span class="SStr">"string"</span>  <span class="SCmt">// 'b' is a constant string known at compile time.</span>
    <span class="SCmp">#assert</span> b == <span class="SStr">"string"</span>
}</span></div>
<p>Constants in Swag can also have more complex types, such as arrays or structures. In such cases, there is a memory footprint because these constants are stored in the data segment of the program. This also means that you can take the address of these constants at runtime. </p>
<p>This is our first static array. It contains 3 elements, and the type of the elements is <span class="code-inline">s32</span> (signed 32-bit integer). </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">const</span> a: [<span class="SNum">3</span>] <span class="STpe">s32</span> = [<span class="SNum">0</span>, <span class="SNum">1</span>, <span class="SNum">2</span>]  <span class="SCmt">// 'a' is a constant array of 3 integers.</span>

    <span class="SKwd">let</span> ptr = &a[<span class="SNum">0</span>]       <span class="SCmt">// Take the address of the first element of the array.</span>
    <span class="SItr">@assert</span>(ptr[<span class="SNum">0</span>] == <span class="SNum">0</span>)  <span class="SCmt">// Access the first element through the pointer.</span>
    <span class="SItr">@assert</span>(ptr[<span class="SNum">2</span>] == <span class="SNum">2</span>)  <span class="SCmt">// Access the third element through the pointer.</span>

    <span class="SCmt">// Since 'a' is a constant, we can also perform compile-time checks.</span>
    <span class="SCmp">#assert</span> a[<span class="SNum">0</span>] == <span class="SNum">0</span>
    <span class="SCmp">#assert</span> a[<span class="SNum">1</span>] == <span class="SNum">1</span>
    <span class="SCmp">#assert</span> a[<span class="SNum">2</span>] == <span class="SNum">2</span>
}</span></div>
<p>Here is an example of a multidimensional array declared as a constant. We will cover arrays in more detail later. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">const</span> <span class="SCst">M4x4</span>: [<span class="SNum">4</span>, <span class="SNum">4</span>] <span class="STpe">f32</span> = [
    [<span class="SNum">1</span>, <span class="SNum">0</span>, <span class="SNum">0</span>, <span class="SNum">0</span>],  <span class="SCmt">// First row</span>
    [<span class="SNum">0</span>, <span class="SNum">1</span>, <span class="SNum">0</span>, <span class="SNum">0</span>],  <span class="SCmt">// Second row</span>
    [<span class="SNum">0</span>, <span class="SNum">0</span>, <span class="SNum">1</span>, <span class="SNum">0</span>],  <span class="SCmt">// Third row</span>
    [<span class="SNum">0</span>, <span class="SNum">0</span>, <span class="SNum">0</span>, <span class="SNum">1</span>]]  <span class="SCmt">// Fourth row</span>
}</span></div>
<p>The key difference between <span class="code-inline">let</span> and <span class="code-inline">const</span> is that the value of a <span class="code-inline">const</span> must be known at compile time, whereas the value of a <span class="code-inline">let</span> can be determined dynamically at runtime. However, both <span class="code-inline">let</span> and <span class="code-inline">const</span> require that the variable or constant is assigned exactly once and cannot be reassigned. </p>

<h2 id="_015_operators_swg">Operators</h2><p>These are all the Swag operators that can be used to manipulate variables and values. </p>
<h3 id="_015_operators_swg_Arithmetic_operators">Arithmetic operators </h3>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> x: <span class="STpe">s32</span> = <span class="SNum">10</span>

    <span class="SCmt">// Addition</span>
    x = x + <span class="SNum">1</span>

    <span class="SCmt">// Substraction</span>
    x = x - <span class="SNum">1</span>

    <span class="SCmt">// Multiplication</span>
    x = x * <span class="SNum">2</span>

    <span class="SCmt">// Division</span>
    x = x / <span class="SNum">2</span>

    <span class="SCmt">// Modulus</span>
    x = x % <span class="SNum">2</span>
}</span></div>
<h3 id="_015_operators_swg_Bitwise_operators">Bitwise operators </h3>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> x: <span class="STpe">s32</span> = <span class="SNum">10</span>

    <span class="SCmt">// Xor</span>
    x = x ^ <span class="SNum">2</span>

    <span class="SCmt">// Bitmask and</span>
    x = x & <span class="SNum">0b0000_0001</span>'<span class="STpe">s32</span>

    <span class="SCmt">// Bitmask or</span>
    x = x | <span class="SKwd">cast</span>(<span class="STpe">s32</span>) <span class="SNum">0b0000_0001</span>

    <span class="SCmt">// Shift bits left</span>
    x = x &lt;&lt; <span class="SNum">1</span>

    <span class="SCmt">// Shift bits right</span>
    x = x &gt;&gt; <span class="SNum">1</span>
}</span></div>
<h3 id="_015_operators_swg_Assignment_operators">Assignment operators </h3>
<p>All the arithmetic and bitwise operators have an <span class="code-inline">affect</span> version. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> x: <span class="STpe">s32</span> = <span class="SNum">10</span>

    x += <span class="SNum">1</span>
    x -= <span class="SNum">1</span>
    x *= <span class="SNum">2</span>
    x /= <span class="SNum">2</span>
    x %= <span class="SNum">2</span>
    x ^= <span class="SNum">2</span>
    x |= <span class="SNum">0b0000_0001</span>
    x &= <span class="SNum">0b0000_0001</span>
    x &lt;&lt;= <span class="SNum">1</span>
    x &gt;&gt;= <span class="SNum">1</span>
}</span></div>
<h3 id="_015_operators_swg_Unary_operators">Unary operators </h3>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> x = <span class="SKwd">true</span>
    <span class="SKwd">var</span> y = <span class="SNum">0b0000_0001</span>'<span class="STpe">u8</span>
    <span class="SKwd">var</span> z = <span class="SNum">1</span>

    <span class="SCmt">// Invert boolean</span>
    x = !x

    <span class="SCmt">// Invert bits</span>
    y = ~y

    <span class="SCmt">// Negative</span>
    z = -z

    <span class="SItr">@assert</span>(z == -<span class="SNum">1</span>)
    <span class="SItr">@assert</span>(x == <span class="SKwd">false</span>)
    <span class="SItr">@assert</span>(y == <span class="SNum">0b1111_1110</span>)
}</span></div>
<h3 id="_015_operators_swg_Comparison_operators">Comparison operators </h3>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    {
        <span class="SKwd">var</span> a = <span class="SKwd">false</span>

        <span class="SCmt">// Equal</span>
        a = <span class="SNum">1</span> == <span class="SNum">1</span> ? <span class="SKwd">true</span> : <span class="SKwd">false</span>

        <span class="SCmt">// Not equal</span>
        a = <span class="SNum">1</span> != <span class="SNum">1</span> ? <span class="SKwd">true</span> : <span class="SKwd">false</span>

        <span class="SCmt">// Lower equal</span>
        a = <span class="SNum">1</span> &lt;= <span class="SNum">1</span> ? <span class="SKwd">true</span> : <span class="SKwd">false</span>

        <span class="SCmt">// Greater equal</span>
        a = <span class="SNum">1</span> &gt;= <span class="SNum">1</span> ? <span class="SKwd">true</span> : <span class="SKwd">false</span>

        <span class="SCmt">// Lower</span>
        a = <span class="SNum">1</span> &lt; <span class="SNum">1</span> ? <span class="SKwd">true</span> : <span class="SKwd">false</span>

        <span class="SCmt">// Greater</span>
        a = <span class="SNum">1</span> &gt; <span class="SNum">1</span> ? <span class="SKwd">true</span> : <span class="SKwd">false</span>
    }

    {
        <span class="SKwd">let</span> x = <span class="SNum">5</span>
        <span class="SKwd">let</span> y = <span class="SNum">10</span>
        <span class="SItr">@assert</span>(x == <span class="SNum">5</span>)
        <span class="SItr">@assert</span>(x != <span class="SNum">10</span>)
        <span class="SItr">@assert</span>(x &lt;= <span class="SNum">5</span>)
        <span class="SItr">@assert</span>(x &lt; <span class="SNum">10</span>)
        <span class="SItr">@assert</span>(x &gt;= <span class="SNum">5</span>)
        <span class="SItr">@assert</span>(x &gt; <span class="SNum">0</span>)
    }
}</span></div>
<h3 id="_015_operators_swg_Logical_operators">Logical operators </h3>
<p>This is <b>not</b> <span class="code-inline">&&</span> and <span class="code-inline">||</span> like in C/C++, but <span class="code-inline">and</span> and <span class="code-inline">or</span>. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> a = <span class="SKwd">false</span>
    a = (<span class="SNum">1</span> &gt; <span class="SNum">10</span>) <span class="SLgc">and</span> (<span class="SNum">10</span> &lt; <span class="SNum">1</span>)
    a = (<span class="SNum">1</span> &gt; <span class="SNum">10</span>) <span class="SLgc">or</span> (<span class="SNum">10</span> &lt; <span class="SNum">1</span>)
}</span></div>
<h3 id="_015_operators_swg_Ternary_operator">Ternary operator </h3>
<p>The ternary operator will test an expression, and will return a value depending on the result of the test. <span class="code-inline">A = Expression ? B : C</span> will return <span class="code-inline">B</span> if the expression is true, and will return <span class="code-inline">C</span> if the expression is false. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SCmt">// Returns 1 because the expression 'true' is... true.</span>
    <span class="SKwd">let</span> x = <span class="SKwd">true</span> ? <span class="SNum">1</span> : <span class="SNum">666</span>
    <span class="SItr">@assert</span>(x == <span class="SNum">1</span>)

    <span class="SCmt">// Returns 666 because the expression 'x == 52' is false.</span>
    <span class="SKwd">let</span> y = (x == <span class="SNum">52</span>) ? <span class="SNum">1</span> : <span class="SNum">666</span>
    <span class="SItr">@assert</span>(y == <span class="SNum">666</span>)
}</span></div>
<h3 id="_015_operators_swg_Spaceshift_operator">Spaceshift operator </h3>
<p>Operator <span class="code-inline">&lt;=&gt;</span> will return -1, 0 or 1 if the expression on the left is <b>lower</b>, <b>equal</b> or <b>greater</b> than the expression on the right. The returned type is <span class="code-inline">s32</span>. </p>
<div class="code-block"><span class="SCde"><span class="SCst">A</span> &lt;=&gt; <span class="SCst">B</span> == -<span class="SNum">1</span> <span class="SLgc">if</span> <span class="SCst">A</span> &lt; <span class="SCst">B</span>
<span class="SCst">A</span> &lt;=&gt; <span class="SCst">B</span> == <span class="SNum">0</span>  <span class="SLgc">if</span> <span class="SCst">A</span> == <span class="SCst">B</span>
<span class="SCst">A</span> &lt;=&gt; <span class="SCst">B</span> == <span class="SNum">1</span>  <span class="SLgc">if</span> <span class="SCst">A</span> &gt; <span class="SCst">B</span></span></div>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    {
        <span class="SKwd">let</span> a = -<span class="SNum">1.5</span> &lt;=&gt; <span class="SNum">2.31</span>
        <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(a) == <span class="STpe">s32</span>
        <span class="SItr">@assert</span>(a == -<span class="SNum">1</span>)

        <span class="SItr">@assert</span>(-<span class="SNum">10</span> &lt;=&gt; <span class="SNum">10</span> == -<span class="SNum">1</span>)
        <span class="SItr">@assert</span>(<span class="SNum">10</span> &lt;=&gt; -<span class="SNum">10</span> == <span class="SNum">1</span>)
        <span class="SItr">@assert</span>(<span class="SNum">10</span> &lt;=&gt; <span class="SNum">10</span> == <span class="SNum">0</span>)
    }

    {
        <span class="SKwd">let</span> x1 = <span class="SNum">10</span> &lt;=&gt; <span class="SNum">20</span>
        <span class="SItr">@assert</span>(x1 == -<span class="SNum">1</span>)
        <span class="SKwd">let</span> x2 = <span class="SNum">20</span> &lt;=&gt; <span class="SNum">10</span>
        <span class="SItr">@assert</span>(x2 == <span class="SNum">1</span>)
        <span class="SKwd">let</span> x3 = <span class="SNum">20</span> &lt;=&gt; <span class="SNum">20</span>
        <span class="SItr">@assert</span>(x3 == <span class="SNum">0</span>)
    }
}</span></div>
<h3 id="_015_operators_swg_Null-coalescing_operator">Null-coalescing operator </h3>
<p>The operator <span class="code-inline">orelse</span> will return the left expression if it is not zero, otherwise it will return the right expression. </p>
<p>Works with strings, pointers and structures with the <span class="code-inline">opData</span> special function (we'll see that later). </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> a = <span class="SStr">"string1"</span>
    <span class="SKwd">let</span> b = <span class="SStr">"string2"</span>

    <span class="SCmt">// c = a if a is not null, else c = b.</span>
    <span class="SKwd">var</span> c = a <span class="SLgc">orelse</span> b
    <span class="SItr">@assert</span>(c == <span class="SStr">"string1"</span>)

    a = <span class="SKwd">null</span>
    c = a <span class="SLgc">orelse</span> b
    <span class="SItr">@assert</span>(c == <span class="SStr">"string2"</span>)
}</span></div>
<p>Works also for basic types like integers. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">let</span> a = <span class="SNum">0</span>
    <span class="SKwd">let</span> b = <span class="SNum">1</span>
    <span class="SKwd">let</span> c = a <span class="SLgc">orelse</span> b
    <span class="SItr">@assert</span>(c == b)
}</span></div>
<h3 id="_015_operators_swg_Type_promotion">Type promotion </h3>
<p>Unlike C, types are not promoted to 32 bits when dealing with 8 or 16 bits types. But types will be promoted if the two sides of an operation: not have the same type. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(<span class="SNum">0</span>'<span class="STpe">u8</span> + <span class="SNum">1</span>'<span class="STpe">u8</span>) == <span class="STpe">u8</span>
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(<span class="SNum">0</span>'<span class="STpe">u8</span> + <span class="SNum">1</span>'<span class="STpe">u16</span>) == <span class="STpe">u16</span> <span class="SCmt">// Priority to bigger type</span>
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(<span class="SNum">0</span>'<span class="STpe">u8</span> + <span class="SNum">1</span>'<span class="STpe">u32</span>) == <span class="STpe">u32</span>
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(<span class="SNum">0</span>'<span class="STpe">u8</span> + <span class="SNum">1</span>'<span class="STpe">u64</span>) == <span class="STpe">u64</span>
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(<span class="SNum">0</span>'<span class="STpe">u8</span> + <span class="SNum">1</span>'<span class="STpe">s8</span>) == <span class="STpe">s8</span> <span class="SCmt">// Priority to signed type</span>
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(<span class="SNum">0</span>'<span class="STpe">u8</span> + <span class="SNum">1</span>'<span class="STpe">s16</span>) == <span class="STpe">s16</span>
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(<span class="SNum">0</span>'<span class="STpe">u8</span> + <span class="SNum">1</span>'<span class="STpe">s32</span>) == <span class="STpe">s32</span>
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(<span class="SNum">0</span>'<span class="STpe">u8</span> + <span class="SNum">1</span>'<span class="STpe">s64</span>) == <span class="STpe">s64</span>
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(<span class="SNum">0</span>'<span class="STpe">u8</span> + <span class="SNum">1</span>'<span class="STpe">f32</span>) == <span class="STpe">f32</span>
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(<span class="SNum">0</span>'<span class="STpe">u8</span> + <span class="SNum">1</span>'<span class="STpe">f64</span>) == <span class="STpe">f64</span>

    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(<span class="SNum">0</span>'<span class="STpe">s8</span> + <span class="SNum">1</span>'<span class="STpe">u16</span>) == <span class="STpe">u16</span> <span class="SCmt">// Priority to bigger type also</span>
}</span></div>
<p>This means that a 8/16 bits operation (like an addition) can more easily <b>overflow</b> if you: not take care. In that case, you can use the <span class="code-inline"> #prom</span> <b>modifier</b> on the operation, which will promote the type to at least 32 bits. The operation will be done accordingly. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(<span class="SNum">255</span>'<span class="STpe">u8</span> + <span class="SKwd">#prom</span> <span class="SNum">1</span>'<span class="STpe">u8</span>) == <span class="STpe">u32</span>
    <span class="SCmp">#assert</span> <span class="SNum">255</span>'<span class="STpe">u8</span> + <span class="SKwd">#prom</span> <span class="SNum">1</span>'<span class="STpe">u8</span> == <span class="SNum">256</span> <span class="SCmt">// No overflow, because the operation is done in 32 bits.</span>
}</span></div>
<p>We'll see later how Swag deals with that kind of overflow, and more generally, with <b>safety</b>. </p>
<h3 id="_015_operators_swg_Operator_precedence">Operator precedence </h3>
<div class="code-block"><span class="SCde">~
* / %
+ -
&gt;&gt; &lt;&lt;
&
|
^
&lt;=&gt;
== !=
&lt; &lt;= &gt; &gt;=
<span class="SLgc">and</span>
<span class="SLgc">or</span></span></div>
<p>If two operators have the same precedence, the expression is evaluated from left to right. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SCmt">// Multiplication before addition</span>
    <span class="SItr">@assert</span>(<span class="SNum">10</span> + <span class="SNum">2</span> * <span class="SNum">3</span> == <span class="SNum">16</span>)
    <span class="SCmt">// Parentheses change precedence</span>
    <span class="SItr">@assert</span>((<span class="SNum">10</span> + <span class="SNum">2</span>) * <span class="SNum">3</span> == <span class="SNum">36</span>)
    <span class="SCmt">// Addition and subtraction before comparison</span>
    <span class="SItr">@assert</span>((<span class="SNum">5</span> + <span class="SNum">3</span> &lt; <span class="SNum">10</span> - <span class="SNum">2</span>) == <span class="SKwd">false</span>)
    <span class="SCmt">// 'and' before 'or'</span>
    <span class="SItr">@assert</span>((<span class="SKwd">false</span> <span class="SLgc">and</span> <span class="SKwd">false</span> <span class="SLgc">or</span> <span class="SKwd">true</span>) == <span class="SKwd">true</span>)
    <span class="SCmt">// '&lt;&lt;' before '&'</span>
    <span class="SItr">@assert</span>((<span class="SNum">10</span> & <span class="SNum">2</span> &lt;&lt; <span class="SNum">1</span>) == <span class="SNum">0</span>)
    <span class="SItr">@assert</span>(((<span class="SNum">10</span> & <span class="SNum">2</span>) &lt;&lt; <span class="SNum">1</span>) == <span class="SNum">4</span>)
}</span></div>

<h2 id="_016_cast_swg">Cast</h2><h3 id="_016_cast_swg_Explicit_Cast">Explicit Cast </h3>
<p>Sometimes, it's necessary to change the type of a value explicitly. This can be done using <span class="code-inline">cast(type)</span> to convert a value from one type to another. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SCmt">// 'x' is a floating-point number (f32 by default)</span>
    <span class="SKwd">let</span> x = <span class="SNum">1.0</span>
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(x) == <span class="STpe">f32</span>

    <span class="SCmt">// 'y' is explicitly cast to a 32-bit integer (s32)</span>
    <span class="SKwd">let</span> y = <span class="SKwd">cast</span>(<span class="STpe">s32</span>) x
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(y) == <span class="STpe">s32</span>
    <span class="SItr">@assert</span>(y == <span class="SNum">1</span>)  <span class="SCmt">// The floating-point value 1.0 is cast to the integer value 1</span>
}</span></div>
<h3 id="_016_cast_swg_acast_(Automatic_Cast)">acast (Automatic Cast) </h3>
<p><span class="code-inline">acast</span> stands for <span class="code-inline">automatic cast</span>. It lets the compiler automatically determine and perform the cast to match the type on the left-hand side of the assignment. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">let</span> x: <span class="STpe">f32</span> = <span class="SNum">1.0</span>
    <span class="SKwd">let</span> y: <span class="STpe">s32</span> = <span class="SKwd">acast</span> x  <span class="SCmt">// Automatically cast 'x' to 's32'</span>
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(y) == <span class="STpe">s32</span>
    <span class="SItr">@assert</span>(y == <span class="SNum">1</span>)
}</span></div>
<h3 id="_016_cast_swg_Bitcast">Bitcast </h3>
<p>The <span class="code-inline">#bit</span> modifier allows you to reinterpret the bits of a value as a different type without changing the underlying bit pattern. This operation is called a <b>bitcast</b> and is only valid when the source and destination types have the same size. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">let</span> x: <span class="STpe">f32</span> = <span class="SNum">1.0</span>
    <span class="SKwd">let</span> y: <span class="STpe">u32</span> = <span class="SKwd">cast</span>(<span class="STpe">u32</span>) <span class="SKwd">#bit</span> x  <span class="SCmt">// Reinterpret the bits of 'x' as a 'u32'</span>
    <span class="SItr">@assert</span>(y == <span class="SNum">0x3f800000</span>)  <span class="SCmt">// 1.0 in IEEE 754 floating-point format equals 0x3f800000 in hex</span>

    <span class="SCmt">// Casting back to the original type should yield the original value</span>
    <span class="SCmp">#assert</span> <span class="SKwd">cast</span>(<span class="STpe">u32</span>) <span class="SKwd">#bit</span> <span class="SNum">1.0</span> == <span class="SNum">0x3f800000</span>
    <span class="SCmp">#assert</span> <span class="SKwd">cast</span>(<span class="STpe">f32</span>) <span class="SKwd">#bit</span> <span class="SNum">0x3f800000</span> == <span class="SNum">1.0</span>  <span class="SCmt">// Reinterpreting the bits back to 'f32' gives 1.0</span>
}</span></div>
<p>This example demonstrates the reverse operation, where we take an integer representing a bit pattern and reinterpret it as a floating-point number. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">let</span> rawBits: <span class="STpe">u32</span> = <span class="SNum">0x40490FDB</span>              <span class="SCmt">// Hexadecimal representation of the float 3.1415927</span>
    <span class="SKwd">let</span> pi: <span class="STpe">f32</span>      = <span class="SKwd">cast</span>(<span class="STpe">f32</span>) <span class="SKwd">#bit</span> rawBits  <span class="SCmt">// Interpret the bit pattern as a floating-point number</span>
    <span class="SItr">@assert</span>(pi == <span class="SNum">3.1415927</span>)                   <span class="SCmt">// This should now represent the value of pi as a floating-point number</span>

    <span class="SCmt">// Verifying that casting back to the original bit pattern restores the initial value</span>
    <span class="SKwd">let</span> backToBits: <span class="STpe">u32</span> = <span class="SKwd">cast</span>(<span class="STpe">u32</span>) <span class="SKwd">#bit</span> pi
    <span class="SItr">@assert</span>(backToBits == <span class="SNum">0x40490FDB</span>)
}</span></div>
<h3 id="_016_cast_swg_Implicit_Casts">Implicit Casts </h3>
<p>Swag can automatically perform type conversions, known as <i>implicit casts</i>, when assigning a value of one type to a variable of another type. This happens without requiring an explicit <span class="code-inline">cast</span> statement.  </p>
<p>However, implicit casts are only allowed when there is no loss of precision or range. This ensures that the value remains unchanged and no data is lost during the conversion. </p>
<h4 id="_016_cast_swg_Implicit_Casts_Implicit_Cast_Rules">Implicit Cast Rules </h4>
<ol>
<li><b>Widening Conversions</b>: Implicit casts are allowed when converting a smaller type to a larger type, such as from <span class="code-inline">s8</span> to <span class="code-inline">s16</span>, or from <span class="code-inline">f32</span> to <span class="code-inline">f64</span>. These conversions are safe because the larger type can represent all possible values of the smaller type.</li>
<li><b>Sign Preservation</b>: When implicitly casting between signed and unsigned types, Swag ensures that no data loss occurs by checking that the value can be represented in both types. Implicit casts from unsigned to signed types (and vice versa) are only allowed when the value is positive and within the range of the target type.</li>
<li><b>No Implicit Narrowing</b>: Swag does not allow implicit casts that could potentially lose data or precision, such as from <span class="code-inline">s32</span> to <span class="code-inline">s8</span>. These narrowing conversions require an explicit cast to signal that the developer acknowledges the potential data loss.</li>
</ol>
<h4 id="_016_cast_swg_Implicit_Casts_Examples_of_Implicit_Casts">Examples of Implicit Casts </h4>
<p>Let's explore some examples to illustrate these rules. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SCmt">// Implicit cast from 8-bit signed integer (s8) to 16-bit signed integer (s16)</span>
    <span class="SKwd">let</span> x: <span class="STpe">s16</span> = <span class="SNum">1</span>'<span class="STpe">s8</span>  <span class="SCmt">// Safe conversion, no loss of precision</span>

    <span class="SCmt">// Implicit cast from 16-bit signed integer (s16) to 32-bit signed integer (s32)</span>
    <span class="SKwd">let</span> y: <span class="STpe">s32</span> = <span class="SNum">1</span>'<span class="STpe">s16</span>  <span class="SCmt">// Safe conversion, no loss of precision</span>

    <span class="SCmt">// Implicit cast from 32-bit signed integer (s32) to 64-bit signed integer (s64)</span>
    <span class="SKwd">let</span> z: <span class="STpe">s64</span> = <span class="SNum">1</span>'<span class="STpe">s32</span>  <span class="SCmt">// Safe conversion, no loss of precision</span>

    <span class="SCmt">// Implicit cast from 8-bit unsigned integer (u8) to 16-bit unsigned integer (u16)</span>
    <span class="SKwd">let</span> a: <span class="STpe">u16</span> = <span class="SNum">255</span>'<span class="STpe">u8</span>  <span class="SCmt">// Safe conversion, no loss of precision</span>

    <span class="SCmt">// Implicit cast from 16-bit unsigned integer (u16) to 32-bit unsigned integer (u32)</span>
    <span class="SKwd">let</span> b: <span class="STpe">u32</span> = <span class="SNum">65535</span>'<span class="STpe">u16</span>  <span class="SCmt">// Safe conversion, no loss of precision</span>

    <span class="SCmt">// Implicit cast from 32-bit unsigned integer (u32) to 64-bit unsigned integer (u64)</span>
    <span class="SKwd">let</span> c: <span class="STpe">u64</span> = <span class="SNum">4294967295</span>'<span class="STpe">u32</span>  <span class="SCmt">// Safe conversion, no loss of precision</span>

    <span class="SCmt">// Implicit cast from 32-bit floating-point (f32) to 64-bit floating-point (f64)</span>
    <span class="SKwd">let</span> d: <span class="STpe">f64</span> = <span class="SNum">1.23</span>'<span class="STpe">f32</span>  <span class="SCmt">// Safe conversion, f64 can represent all f32 values accurately</span>
}</span></div>
<h4 id="_016_cast_swg_Implicit_Casts_Examples_Where_Implicit_Casts_Are_Not_Allowed">Examples Where Implicit Casts Are Not Allowed </h4>
<p>There are cases where an implicit cast is not allowed due to the potential for data loss or precision issues. In these situations, Swag requires an explicit cast to ensure that the developer is aware of and accepts the risks.  </p>
<p>Note also that the cast modifier <span class="code-inline">#over</span> (stands for overflow) can be necessary to indicate the a value can lost some precision without raising an error. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SCmt">// Implicit cast from 16-bit signed integer (s16) to 8-bit signed integer (s8)</span>
    <span class="SCmt">// This would generate a compilation error because s8 cannot represent all s16 values.</span>
    <span class="SCmt">// Uncommenting the following lines would cause an error:</span>

    <span class="SCmt">// let z0: s16 = 100</span>
    <span class="SCmt">// let z1: s8 = z0  // Error: Implicit cast from 's16' to 's8' is not allowed</span>

    <span class="SCmt">// To perform this cast, an explicit cast is required:</span>
    <span class="SKwd">let</span> z0: <span class="STpe">s16</span> = <span class="SNum">256</span>
    <span class="SKwd">let</span> z1: <span class="STpe">s8</span> = <span class="SKwd">cast</span>(<span class="STpe">s8</span>) <span class="SKwd">#over</span> z0  <span class="SCmt">// Explicit cast needed to convert from s16 to s8</span>
    <span class="SItr">@assert</span>(z1 == <span class="SNum">0</span>)  <span class="SCmt">// This may cause loss of data as 256 cannot be represented in s8 (which maxes out at 127)</span>

    <span class="SCmt">// Implicit cast from unsigned to signed type where the value is out of range</span>
    <span class="SKwd">let</span> u_val: <span class="STpe">u16</span> = <span class="SNum">65535</span>
    <span class="SCmt">// let s_val: s16 = u_val  // Error: Implicit cast from 'u16' to 's16' is not allowed due to potential data loss</span>

    <span class="SCmt">// To perform this cast, an explicit cast is required, with the risk of data loss:</span>
    <span class="SKwd">let</span> s_val: <span class="STpe">s16</span> = <span class="SKwd">cast</span>(<span class="STpe">s16</span>) <span class="SKwd">#over</span> u_val  <span class="SCmt">// This could result in an unexpected negative value</span>
    <span class="SItr">@assert</span>(s_val == -<span class="SNum">1</span>)  <span class="SCmt">// 65535 in u16 becomes -1 in s16 due to overflow</span>

    <span class="SCmt">// Implicit cast from f64 to f32, which can lose precision</span>
    <span class="SKwd">let</span> large_float: <span class="STpe">f64</span> = <span class="SNum">1.23456789012345611111</span>
    <span class="SCmt">// let smaller_float: f32 = large_float  // Error: Implicit cast from 'f64' to 'f32' is not allowed due to precision loss</span>

    <span class="SCmt">// Explicit cast is required when converting from f64 to f32</span>
    <span class="SKwd">let</span> smaller_float: <span class="STpe">f32</span> = <span class="SKwd">cast</span>(<span class="STpe">f32</span>) large_float
}</span></div>

<h2 id="_020_array_swg">Array</h2><p>Remember that dynamic arrays are part of the <span class="code-inline">Std.Core</span> module. Here we are only discussing native static arrays, which are arrays with fixed size determined at compile time. </p>
<p>A static array is declared using the syntax <span class="code-inline">[N]</span> followed by the type, where <span class="code-inline">N</span> represents the number of elements in the array (the dimension). </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> array: [<span class="SNum">2</span>] <span class="STpe">s32</span>  <span class="SCmt">// Declare a static array of two 32-bit signed integers (s32)</span>
    array[<span class="SNum">0</span>] = <span class="SNum">1</span>  <span class="SCmt">// Assign the first element</span>
    array[<span class="SNum">1</span>] = <span class="SNum">2</span>  <span class="SCmt">// Assign the second element</span>
}</span></div>
<p>You can determine the number of elements in an array using the <span class="code-inline">@countof</span> intrinsic and the size of the array in bytes using <span class="code-inline">@sizeof</span>. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> array: [<span class="SNum">2</span>] <span class="STpe">s32</span>  <span class="SCmt">// Declare a static array of two s32 elements</span>
    <span class="SCmp">#assert</span> <span class="SItr">@countof</span>(array) == <span class="SNum">2</span>  <span class="SCmt">// Verify the array contains 2 elements</span>
    <span class="SCmp">#assert</span> <span class="SItr">@sizeof</span>(array) == <span class="SNum">2</span> * <span class="SItr">@sizeof</span>(<span class="STpe">s32</span>)  <span class="SCmt">// Verify the size in bytes (2 elements * size of s32)</span>
}</span></div>
<p>To obtain the address of an array, you can use the <span class="code-inline">@dataof</span> intrinsic. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> array: [<span class="SNum">2</span>] <span class="STpe">s32</span>  <span class="SCmt">// Declare a static array of two s32 elements</span>
    <span class="SKwd">var</span> ptr0   = <span class="SItr">@dataof</span>(array)  <span class="SCmt">// Get the address of the array</span>
    ptr0[<span class="SNum">0</span>] = <span class="SNum">1</span>  <span class="SCmt">// Access the first element through the pointer</span>

    <span class="SCmt">// You can also take the address of the first element directly</span>
    <span class="SKwd">var</span> ptr1 = &array[<span class="SNum">0</span>]
    ptr1[<span class="SNum">1</span>] = <span class="SNum">2</span>  <span class="SCmt">// Access the second element through the pointer</span>

    <span class="SItr">@assert</span>(array[<span class="SNum">0</span>] == <span class="SNum">1</span>)
    <span class="SItr">@assert</span>(array[<span class="SNum">1</span>] == <span class="SNum">2</span>)
}</span></div>
<p>An <b>array literal</b> is a list of elements enclosed in square brackets <span class="code-inline">[A, B, ...]</span>. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> arr = [<span class="SNum">1</span>, <span class="SNum">2</span>, <span class="SNum">3</span>, <span class="SNum">4</span>]  <span class="SCmt">// Declare and initialize an array of four s32 elements</span>
    <span class="SCmp">#assert</span> <span class="SItr">@countof</span>(arr) == <span class="SNum">4</span>  <span class="SCmt">// Verify the array contains 4 elements</span>
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(arr) == <span class="STpe">#type</span> [<span class="SNum">4</span>] <span class="STpe">s32</span>  <span class="SCmt">// Verify the array's type</span>
}</span></div>
<p>The size of the array can be deduced from the initialization expression, meaning you don't have to specify the dimension if it can be inferred. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SCmt">// The dimension is deduced from the initialization with 2 elements</span>
    <span class="SKwd">var</span> array: [] <span class="STpe">s32</span> = [<span class="SNum">1</span>, <span class="SNum">2</span>]
    <span class="SItr">@assert</span>(array[<span class="SNum">0</span>] == <span class="SNum">1</span>)
    <span class="SItr">@assert</span>(array[<span class="SNum">1</span>] == <span class="SNum">2</span>)
    <span class="SCmp">#assert</span> <span class="SItr">@countof</span>(array) == <span class="SNum">2</span>

    <span class="SCmt">// Both dimensions and types are deduced from the initialization expression</span>
    <span class="SKwd">var</span> array1 = [<span class="SStr">"10"</span>, <span class="SStr">"20"</span>, <span class="SStr">"30"</span>]
    <span class="SItr">@assert</span>(array1[<span class="SNum">0</span>] == <span class="SStr">"10"</span>)
    <span class="SItr">@assert</span>(array1[<span class="SNum">1</span>] == <span class="SStr">"20"</span>)
    <span class="SItr">@assert</span>(array1[<span class="SNum">2</span>] == <span class="SStr">"30"</span>)
    <span class="SCmp">#assert</span> <span class="SItr">@countof</span>(array1) == <span class="SNum">3</span>
}</span></div>
<p>Like other types in Swag, an array is initialized by default with zero values (0 for integers, null for strings, false for booleans, etc.). </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> array: [<span class="SNum">2</span>] <span class="STpe">s32</span>  <span class="SCmt">// Declare a static array of two s32 elements</span>
    <span class="SItr">@assert</span>(array[<span class="SNum">0</span>] == <span class="SNum">0</span>)  <span class="SCmt">// Default initialization to 0</span>
    <span class="SItr">@assert</span>(array[<span class="SNum">1</span>] == <span class="SNum">0</span>)
}</span></div>
<p>For performance reasons, you can prevent an array from being initialized by using <span class="code-inline">undefined</span>. This is useful when you know the array will be fully initialized manually and you want to avoid the cost of default initialization. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> array: [<span class="SNum">100</span>] <span class="STpe">s32</span> = <span class="SKwd">undefined</span>  <span class="SCmt">// Declare a large array without initializing it</span>
}</span></div>
<p>A static array with compile-time values can be stored as a constant, meaning it cannot be modified after its declaration. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">const</span> array = [<span class="SNum">1</span>, <span class="SNum">2</span>, <span class="SNum">3</span>, <span class="SNum">4</span>]  <span class="SCmt">// Declare a constant array</span>
    <span class="SCmp">#assert</span> array[<span class="SNum">0</span>] == <span class="SNum">1</span>  <span class="SCmt">// Dereference is done at compile time</span>
    <span class="SCmp">#assert</span> array[<span class="SNum">3</span>] == <span class="SNum">4</span>
}</span></div>
<p>If the type of the array is not explicitly specified, the type of the <b>first</b> literal value will be used for all other elements in the array. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> arr = [<span class="SNum">1</span>'<span class="STpe">f64</span>, <span class="SNum">2</span>, <span class="SNum">3</span>, <span class="SNum">4</span>]  <span class="SCmt">// All values are deduced to be 'f64'</span>
    <span class="SCmp">#assert</span> <span class="SItr">@countof</span>(arr) == <span class="SNum">4</span>
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(arr) == <span class="STpe">#type</span> [<span class="SNum">4</span>] <span class="STpe">f64</span>  <span class="SCmt">// Verify that all elements are of type f64</span>
    <span class="SItr">@assert</span>(arr[<span class="SNum">3</span>] == <span class="SNum">4.0</span>)
}</span></div>
<p>Swag supports multi-dimensional arrays, allowing you to declare arrays with multiple dimensions. </p>
<p>The syntax for multi-dimensional arrays is <span class="code-inline">[X, Y, Z...]</span>, where <span class="code-inline">X</span>, <span class="code-inline">Y</span>, and <span class="code-inline">Z</span> are the dimensions. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> array: [<span class="SNum">2</span>, <span class="SNum">2</span>] <span class="STpe">s32</span>  <span class="SCmt">// Declare a 2x2 array of s32</span>
    array[<span class="SNum">0</span>, <span class="SNum">0</span>] = <span class="SNum">1</span>  <span class="SCmt">// Access and assign the first element</span>
    array[<span class="SNum">0</span>, <span class="SNum">1</span>] = <span class="SNum">2</span>  <span class="SCmt">// Access and assign the second element</span>
    array[<span class="SNum">1</span>, <span class="SNum">0</span>] = <span class="SNum">3</span>  <span class="SCmt">// Access and assign the third element</span>
    array[<span class="SNum">1</span>, <span class="SNum">1</span>] = <span class="SNum">4</span>  <span class="SCmt">// Access and assign the fourth element</span>
}</span></div>
<p>Swag also accepts the C/C++ syntax for declaring arrays, where you declare an array of arrays. In essence, this approach is equivalent to declaring a multi-dimensional array in Swag. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> array: [<span class="SNum">2</span>] [<span class="SNum">2</span>] <span class="STpe">s32</span>  <span class="SCmt">// Declare a 2x2 array of s32 using C/C++ syntax</span>
    array[<span class="SNum">0</span>, <span class="SNum">0</span>] = <span class="SNum">1</span>
    array[<span class="SNum">0</span>, <span class="SNum">1</span>] = <span class="SNum">2</span>
    array[<span class="SNum">1</span>, <span class="SNum">0</span>] = <span class="SNum">3</span>
    array[<span class="SNum">1</span>, <span class="SNum">1</span>] = <span class="SNum">4</span>
}</span></div>
<p>The sizes of arrays, including multi-dimensional arrays, can be deduced from the initialization expression. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> array  = [<span class="SNum">1</span>, <span class="SNum">2</span>, <span class="SNum">3</span>, <span class="SNum">4</span>]  <span class="SCmt">// Size deduced to be 4</span>
    <span class="SKwd">var</span> array1 = [[<span class="SNum">1</span>, <span class="SNum">2</span>], [<span class="SNum">3</span>, <span class="SNum">4</span>]]  <span class="SCmt">// 2x2 array, size deduced from initialization</span>

    <span class="SCmp">#assert</span> <span class="SItr">@countof</span>(array) == <span class="SNum">4</span>  <span class="SCmt">// Verify size is 4</span>
    <span class="SCmp">#assert</span> <span class="SItr">@countof</span>(array1) == <span class="SNum">2</span>  <span class="SCmt">// Verify outer array has 2 elements</span>
}</span></div>
<p>Swag allows you to initialize an entire array with a single value. This is only available for variables, not constants, and only basic types (integers, floats, strings, bools, runes) are supported. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SCmt">// Initialize the entire 2x2 boolean array with 'true'</span>
    <span class="SKwd">var</span> arr: [<span class="SNum">2</span>, <span class="SNum">2</span>] <span class="STpe">bool</span> = <span class="SKwd">true</span>
    <span class="SItr">@assert</span>(arr[<span class="SNum">0</span>, <span class="SNum">0</span>] == <span class="SKwd">true</span>)
    <span class="SItr">@assert</span>(arr[<span class="SNum">1</span>, <span class="SNum">1</span>] == <span class="SKwd">true</span>)

    <span class="SCmt">// Initialize the entire 5x10 string array with "string"</span>
    <span class="SKwd">var</span> arr1: [<span class="SNum">5</span>, <span class="SNum">10</span>] <span class="STpe">string</span> = <span class="SStr">"string"</span>
    <span class="SItr">@assert</span>(arr1[<span class="SNum">0</span>, <span class="SNum">0</span>] == <span class="SStr">"string"</span>)
    <span class="SItr">@assert</span>(arr1[<span class="SNum">4</span>, <span class="SNum">9</span>] == <span class="SStr">"string"</span>)
}</span></div>

<h2 id="_021_slice_swg">Slice</h2><p>A slice in Swag is a flexible view over a contiguous block of memory. Unlike static arrays, slices can be resized or point to different segments of memory at runtime. A slice consists of a pointer to the data buffer and a <span class="code-inline">u64</span> value that stores the number of elements in the slice. </p>
<p>This makes slices a powerful tool for working with subsets of data without copying it, allowing efficient manipulation of large datasets. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> a: [..] <span class="STpe">bool</span> <span class="SCmt">// Declare a slice of boolean values</span>
    <span class="SCmp">#assert</span> <span class="SItr">@sizeof</span>(a) == <span class="SItr">@sizeof</span>(*<span class="STpe">void</span>) + <span class="SItr">@sizeof</span>(<span class="STpe">u64</span>)  <span class="SCmt">// Verify that the size of the slice includes the pointer and the element count</span>
}</span></div>
<p>Slices can be initialized similarly to arrays. You can assign an array literal to a slice, and the slice will point to the elements in the array. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> a: <span class="SKwd">const</span> [..] <span class="STpe">u32</span> = [<span class="SNum">10</span>, <span class="SNum">20</span>, <span class="SNum">30</span>, <span class="SNum">40</span>, <span class="SNum">50</span>]  <span class="SCmt">// Initialize a slice with an array literal</span>
    <span class="SItr">@assert</span>(<span class="SItr">@countof</span>(a) == <span class="SNum">5</span>)  <span class="SCmt">// The slice contains 5 elements</span>
    <span class="SItr">@assert</span>(a[<span class="SNum">0</span>] == <span class="SNum">10</span>)
    <span class="SItr">@assert</span>(a[<span class="SNum">4</span>] == <span class="SNum">50</span>)

    <span class="SCmt">// Slices are dynamic, so you can change the elements at runtime.</span>
    a = [<span class="SNum">1</span>, <span class="SNum">2</span>]  <span class="SCmt">// Reassign the slice to a different array</span>
    <span class="SItr">@assert</span>(<span class="SItr">@countof</span>(a) == <span class="SNum">2</span>)
    <span class="SItr">@assert</span>(a[<span class="SNum">0</span>] == <span class="SNum">1</span>)
    <span class="SItr">@assert</span>(a[<span class="SNum">1</span>] == <span class="SNum">2</span>)
}</span></div>
<p>At runtime, you can use <span class="code-inline">@dataof</span> to retrieve the address of the data buffer and <span class="code-inline">@countof</span> to get the number of elements in the slice. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> a: <span class="SKwd">const</span> [..] <span class="STpe">u32</span> = [<span class="SNum">10</span>, <span class="SNum">20</span>, <span class="SNum">30</span>, <span class="SNum">40</span>, <span class="SNum">50</span>]  <span class="SCmt">// Initialize a slice with an array literal</span>
    <span class="SKwd">let</span> count = <span class="SItr">@countof</span>(a)  <span class="SCmt">// Get the number of elements in the slice</span>
    <span class="SKwd">let</span> addr  = <span class="SItr">@dataof</span>(a)   <span class="SCmt">// Get the address of the slice's data buffer</span>
    <span class="SItr">@assert</span>(count == <span class="SNum">5</span>)
    <span class="SItr">@assert</span>(addr[<span class="SNum">0</span>] == <span class="SNum">10</span>)
    <span class="SItr">@assert</span>(addr[<span class="SNum">4</span>] == <span class="SNum">50</span>)

    a = [<span class="SNum">1</span>, <span class="SNum">2</span>]  <span class="SCmt">// Reassign the slice to a different array</span>
    <span class="SItr">@assert</span>(<span class="SItr">@countof</span>(a) == <span class="SNum">2</span>)
}</span></div>
<p>You can create a slice with your own pointer and count using <span class="code-inline">@mkslice</span>. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> array: [<span class="SNum">4</span>] <span class="STpe">u32</span> = [<span class="SNum">10</span>, <span class="SNum">20</span>, <span class="SNum">30</span>, <span class="SNum">40</span>]

    <span class="SCmt">// Creates a slice of 'array' (a view, or subpart) starting at index 2, with 2 elements.</span>
    <span class="SKwd">let</span> slice: [..] <span class="STpe">u32</span> = <span class="SItr">@mkslice</span>(&array[<span class="SNum">0</span>] + <span class="SNum">2</span>, <span class="SNum">2</span>)

    <span class="SItr">@assert</span>(<span class="SItr">@countof</span>(slice) == <span class="SNum">2</span>)
    <span class="SItr">@assert</span>(slice[<span class="SNum">0</span>] == <span class="SNum">30</span>)
    <span class="SItr">@assert</span>(slice[<span class="SNum">1</span>] == <span class="SNum">40</span>)

    <span class="SItr">@assert</span>(array[<span class="SNum">2</span>] == <span class="SNum">30</span>)
    slice[<span class="SNum">0</span>] = <span class="SNum">314</span>
    <span class="SItr">@assert</span>(array[<span class="SNum">2</span>] == <span class="SNum">314</span>)
}</span></div>
<p>For a string, the slice must be <span class="code-inline">const</span> because a string is immutable. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">let</span> str       = <span class="SStr">"string"</span>
    <span class="SKwd">let</span> strSlice: <span class="SKwd">const</span> [..] <span class="STpe">u8</span> = <span class="SItr">@mkslice</span>(<span class="SItr">@dataof</span>(str), <span class="SNum">2</span>)
    <span class="SItr">@assert</span>(strSlice[<span class="SNum">0</span>] == <span class="SStr">`s`</span>)
    <span class="SItr">@assert</span>(strSlice[<span class="SNum">1</span>] == <span class="SStr">`t`</span>)
}</span></div>
<p>Instead of <span class="code-inline">@mkslice</span>, you can slice something with the <span class="code-inline">..</span> operator. For example, you can slice a string. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">let</span> str = <span class="SStr">"string"</span>

    <span class="SCmt">// Creates a slice starting at byte 0 and ending at byte 3.</span>
    <span class="SKwd">let</span> slice = str[<span class="SNum">1.</span>.<span class="SNum">3</span>]

    <span class="SItr">@assert</span>(slice == <span class="SStr">"tri"</span>)
}</span></div>
<p>The upper limit is <b>included</b> by default. If you want to exclude it, use <span class="code-inline">..&lt;</span> instead of <span class="code-inline">..</span>. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">let</span> str   = <span class="SStr">"string"</span>
    <span class="SKwd">let</span> slice = str[<span class="SNum">1.</span>.&lt;<span class="SNum">3</span>]
    <span class="SItr">@assert</span>(slice == <span class="SStr">"tr"</span>)
}</span></div>
<p>You can omit the upper bound if you want to slice to the end. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">let</span> str   = <span class="SStr">"string"</span>
    <span class="SKwd">let</span> slice = str[<span class="SNum">2.</span>.]
    <span class="SItr">@assert</span>(slice == <span class="SStr">"ring"</span>)
}</span></div>
<p>You can omit the lower bound if you want to slice from the start (0). </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">let</span> str   = <span class="SStr">"string"</span>
    <span class="SKwd">let</span> slice = str[..<span class="SNum">2</span>]     <span class="SCmt">// Index 2 is included</span>
    <span class="SItr">@assert</span>(slice == <span class="SStr">"str"</span>)
    <span class="SKwd">let</span> slice1 = str[..&lt;<span class="SNum">2</span>] <span class="SCmt">// Index 2 is excluded</span>
    <span class="SItr">@assert</span>(slice1 == <span class="SStr">"st"</span>)
}</span></div>
<p>You can also slice an array. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">let</span> arr   = [<span class="SNum">10</span>, <span class="SNum">20</span>, <span class="SNum">30</span>, <span class="SNum">40</span>]
    <span class="SKwd">let</span> slice = arr[<span class="SNum">2.</span>.<span class="SNum">3</span>]
    <span class="SItr">@assert</span>(slice[<span class="SNum">0</span>] == <span class="SNum">30</span>)
    <span class="SItr">@assert</span>(slice[<span class="SNum">1</span>] == <span class="SNum">40</span>)
    <span class="SItr">@assert</span>(<span class="SItr">@countof</span>(slice) == <span class="SNum">2</span>)

    <span class="SCmt">// Creates a slice for the whole array</span>
    <span class="SKwd">let</span> slice1 = arr[..]
    <span class="SItr">@assert</span>(<span class="SItr">@countof</span>(slice1) == <span class="SItr">@countof</span>(arr))
}</span></div>
<p>You can slice another slice. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">let</span> arr    = [<span class="SNum">10</span>, <span class="SNum">20</span>, <span class="SNum">30</span>, <span class="SNum">40</span>]
    <span class="SKwd">let</span> slice1 = arr[<span class="SNum">1.</span>.<span class="SNum">3</span>]
    <span class="SItr">@assert</span>(slice1[<span class="SNum">0</span>] == <span class="SNum">20</span>)
    <span class="SItr">@assert</span>(slice1[<span class="SNum">1</span>] == <span class="SNum">30</span>)
    <span class="SItr">@assert</span>(slice1[<span class="SNum">2</span>] == <span class="SNum">40</span>)

    <span class="SKwd">let</span> slice2 = slice1[<span class="SNum">1.</span>.<span class="SNum">2</span>]
    <span class="SItr">@assert</span>(slice2[<span class="SNum">0</span>] == <span class="SNum">30</span>)
    <span class="SItr">@assert</span>(slice2[<span class="SNum">1</span>] == <span class="SNum">40</span>)
}</span></div>
<p>You can transform a pointer to a slice. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> arr   = [<span class="SNum">10</span>, <span class="SNum">20</span>, <span class="SNum">30</span>, <span class="SNum">40</span>]
    <span class="SKwd">let</span> ptr   = &arr[<span class="SNum">2</span>]
    <span class="SKwd">let</span> slice = ptr[<span class="SNum">0.</span>.<span class="SNum">1</span>]
    <span class="SItr">@assert</span>(slice[<span class="SNum">0</span>] == <span class="SNum">30</span>)
    <span class="SItr">@assert</span>(slice[<span class="SNum">1</span>] == <span class="SNum">40</span>)
    <span class="SItr">@assert</span>(<span class="SItr">@countof</span>(slice) == <span class="SNum">2</span>)
}</span></div>

<h2 id="_022_pointers_swg">Pointers</h2><h3 id="_022_pointers_swg_Single_value_pointers">Single value pointers </h3>
<p>A pointer to a <b>single element</b> is declared with <span class="code-inline">*</span>. This allows you to create a pointer that can hold the address of one specific instance of a data type. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> ptr1: *<span class="STpe">u8</span>      <span class="SCmt">// This is a pointer to a single 'u8' value</span>
    <span class="SKwd">var</span> ptr2: **<span class="STpe">u8</span>     <span class="SCmt">// This is a pointer to another pointer, which in turn points to a single 'u8' value</span>
}</span></div>
<p>A pointer can be <span class="code-inline">null</span>, meaning it does not point to any valid memory location. In Swag, like in many other languages, a null pointer is often used to indicate that the pointer is not yet initialized or intentionally points to "nothing". </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> ptr1: *<span class="STpe">u8</span>  <span class="SCmt">// Declaring a pointer to 'u8' without initialization</span>
    <span class="SItr">@assert</span>(ptr1 == <span class="SKwd">null</span>)  <span class="SCmt">// By default, the pointer is null, indicating it doesn't point to any valid memory location</span>
}</span></div>
<p>You can take the address of a variable using the <span class="code-inline">&</span> operator. This operator allows you to obtain the memory address of a variable, which can then be stored in a pointer. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> arr = <span class="SNum">1</span>
    <span class="SKwd">let</span> ptr = &arr  <span class="SCmt">// Take the address of the variable 'arr'</span>
    <span class="SItr">@assert</span>(<span class="SItr">@typeof</span>(ptr) == *<span class="STpe">s32</span>)  <span class="SCmt">// The type of 'ptr' is a pointer to 's32'</span>
}</span></div>
<p>You can get the value that a pointer is pointing to by using the <span class="code-inline">dref</span> intrinsic. Dereferencing a pointer means accessing the data stored at the memory location that the pointer holds. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> arr = <span class="SNum">42</span>
    <span class="SKwd">let</span> ptr = &arr  <span class="SCmt">// Take the address of 'arr'</span>
    <span class="SItr">@assert</span>(<span class="SKwd">dref</span> ptr == <span class="SNum">42</span>)  <span class="SCmt">// Dereference the pointer to access the value of 'arr'</span>
}</span></div>
<p>Pointers can be <span class="code-inline">const</span>, meaning that the pointer itself cannot change what it points to, though the data being pointed to may still be mutable. This is useful when you want to ensure that a pointer always refers to the same memory location. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">let</span> str  = <span class="SStr">"string"</span>
    <span class="SKwd">let</span> ptr: <span class="SKwd">const</span> *<span class="STpe">u8</span> = <span class="SItr">@dataof</span>(str)  <span class="SCmt">// A const pointer to a 'u8' value</span>
    <span class="SItr">@assert</span>(<span class="SKwd">dref</span> ptr == <span class="SStr">`s`</span>)  <span class="SCmt">// Dereferencing the pointer to get the first character of the string</span>
}</span></div>
<p>You can be more specific with pointers by combining <span class="code-inline">const</span> in different ways, but consider if it's necessary for your use case. For example, you can have pointers that are themselves constant, pointers to constant data, or both. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> ptr:  *<span class="SKwd">const</span> *<span class="STpe">u8</span>           <span class="SCmt">// A normal pointer to a const pointer to 'u8'</span>
    <span class="SKwd">var</span> ptr1: <span class="SKwd">const</span> *<span class="SKwd">const</span> *<span class="STpe">u8</span>     <span class="SCmt">// A const pointer to a const pointer to 'u8'</span>
    <span class="SKwd">var</span> ptr2: <span class="SKwd">const</span> **<span class="STpe">u8</span>           <span class="SCmt">// A const pointer to a normal pointer to 'u8'</span>
}</span></div>
<h3 id="_022_pointers_swg_Multiple_values_pointers">Multiple values pointers </h3>
<p>If you want to enable <b>pointer arithmetic</b> and make a pointer to <b>multiple values</b> (essentially treating the pointer as pointing to an array or a memory block), declare your pointer with <span class="code-inline">^</span> instead of <span class="code-inline">*</span>. This is useful when you need to iterate over a sequence of elements in memory. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> ptr: ^<span class="STpe">u8</span>  <span class="SCmt">// Declaring a pointer to a memory block of 'u8' values</span>

    <span class="SCmt">// Pointer arithmetic allows you to move the pointer to different elements in the memory block</span>
    ptr = ptr - <span class="SNum">1</span>  <span class="SCmt">// Move the pointer back by one 'u8'</span>
}</span></div>
<p>Taking the address of an array element allows for pointer arithmetic. When you take the address of an element in an array, the resulting pointer is treated as pointing to multiple elements, enabling arithmetic operations like incrementing or decrementing the pointer. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> x: [<span class="SNum">4</span>] <span class="STpe">s32</span>  <span class="SCmt">// Declare an array of 4 s32 elements</span>
    <span class="SKwd">var</span> ptr = &x[<span class="SNum">1</span>]  <span class="SCmt">// Take the address of the second element</span>

    <span class="SCmt">// Pointer arithmetic is now enabled because 'ptr' is treated as pointing to multiple values</span>
    ptr = ptr - <span class="SNum">1</span>  <span class="SCmt">// Move the pointer back to the first element</span>
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(ptr) == ^<span class="STpe">s32</span>  <span class="SCmt">// The type of ptr is now a pointer to multiple 's32' values</span>
}</span></div>
<p>As pointer arithmetic is enabled, you can dereference that kind of pointer by using an index. This allows you to access elements in an array-like manner, even though you're working with a pointer. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> arr = [<span class="SNum">1</span>, <span class="SNum">2</span>, <span class="SNum">3</span>, <span class="SNum">4</span>]  <span class="SCmt">// Declare and initialize an array of s32 elements</span>
    <span class="SKwd">let</span> ptr = &arr[<span class="SNum">0</span>]  <span class="SCmt">// Take the address of the first element</span>
    <span class="SItr">@assert</span>(<span class="SItr">@typeof</span>(ptr) == ^<span class="STpe">s32</span>)  <span class="SCmt">// The pointer type allows pointer arithmetic</span>

    <span class="SCmt">// Dereferencing by index to access the elements</span>
    <span class="SKwd">let</span> value1 = ptr[<span class="SNum">0</span>]  <span class="SCmt">// Access the first element</span>
    <span class="SItr">@assert</span>(value1 == <span class="SNum">1</span>)
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(value1) == <span class="STpe">s32</span>

    <span class="SKwd">let</span> value2 = ptr[<span class="SNum">1</span>]  <span class="SCmt">// Access the second element</span>
    <span class="SItr">@assert</span>(value2 == <span class="SNum">2</span>)
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(value2) == <span class="STpe">s32</span>

    <span class="SCmt">// You can still use 'dref' for the first element, which is equivalent to ptr[0]</span>
    <span class="SKwd">let</span> value = <span class="SKwd">dref</span> ptr  <span class="SCmt">// Dereference the pointer to access the first element</span>
    <span class="SItr">@assert</span>(value == <span class="SNum">1</span>)
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(value) == <span class="STpe">s32</span>
}</span></div>

<h2 id="_023_references_swg">References</h2><p>Swag also supports <b>references</b>, which are pointers that behave like values. References in Swag provide a convenient way to work with memory addresses while abstracting away the need for explicit pointer syntax, making them easier and safer to use in many cases. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> x = <span class="SNum">42</span>

    <span class="SCmt">// Use '&' to declare a reference.</span>
    <span class="SCmt">// Here we declare a reference to the variable 'x'.</span>
    <span class="SCmt">// Unlike C++, you must take the address of 'x' to convert it into a reference.</span>
    <span class="SKwd">let</span> myRef: <span class="SKwd">const</span> &<span class="STpe">s32</span> = &x

    <span class="SCmt">// References behave like values, so no explicit dereferencing is needed.</span>
    <span class="SCmt">// You can think of this as an alias for 'x'.</span>
    <span class="SItr">@assert</span>(myRef == <span class="SNum">42</span>)
}</span></div>
<p>When an assignment is made to a reference outside of its initialization, the operation changes the value of the variable being referenced, not the reference itself. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> x = <span class="SNum">42</span>
    <span class="SKwd">var</span> myRef: &<span class="STpe">s32</span> = &x  <span class="SCmt">// The reference is mutable because it's not declared 'const'</span>
    <span class="SItr">@assert</span>(myRef == <span class="SNum">42</span>)

    <span class="SCmt">// This changes the value of 'x' through the reference.</span>
    myRef = <span class="SNum">66</span>
    <span class="SItr">@assert</span>(myRef == <span class="SNum">66</span>)

    <span class="SCmt">// Since 'myRef' is an alias for 'x', 'x' is also updated.</span>
    <span class="SItr">@assert</span>(x == <span class="SNum">66</span>)
}</span></div>
<p>However, unlike in C++, you can change the reference itself (reassign it) rather than the value it points to. To reassign the reference, use the <span class="code-inline">ref</span> modifier in the assignment. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> x = <span class="SNum">1</span>
    <span class="SKwd">var</span> y = <span class="SNum">1000</span>

    <span class="SKwd">var</span> myRef: &<span class="STpe">s32</span> = &x
    <span class="SItr">@assert</span>(myRef == <span class="SNum">1</span>)

    <span class="SCmt">// Here, we reassign 'myRef' to point to 'y' instead of 'x'.</span>
    <span class="SCmt">// The value of 'x' remains unchanged.</span>
    myRef = <span class="SKwd">#ref</span> &y
    <span class="SItr">@assert</span>(myRef == <span class="SNum">1000</span>)
}</span></div>
<p>Most of the time, you need to take the address of a variable to create a reference to it. The only exception is when passing a reference to a function parameter, and the reference is declared as <span class="code-inline">const</span>. In such cases, taking the address explicitly is not necessary. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SCmt">// We can pass a literal directly because 'x' in the function 'toto' is a const reference.</span>
    <span class="SCmt">// No need to take the address manually.</span>
    <span class="SFct">toto</span>(<span class="SNum">4</span>)
}

<span class="SKwd">func</span> <span class="SFct">toto</span>(x: <span class="SKwd">const</span> &<span class="STpe">s32</span>)
{
    <span class="SItr">@assert</span>(x == <span class="SNum">4</span>)

    <span class="SCmt">// Internally, the reference is still an address to an 's32'.</span>
    <span class="SKwd">let</span> ptr = &x
    <span class="SItr">@assert</span>(<span class="SKwd">dref</span> ptr == <span class="SNum">4</span>)
}</span></div>
<p>This approach is particularly useful for structs, as it allows passing literals directly to functions. </p>
<div class="code-block"><span class="SCde"><span class="SCmt">// Our first simple struct!</span>
<span class="SKwd">struct</span> <span class="SCst">MyStruct</span> { x: <span class="STpe">s32</span>, y: <span class="STpe">s32</span> }

<span class="SFct">#test</span>
{
    <span class="SFct">titi0</span>({<span class="SNum">1</span>, <span class="SNum">2</span>})
    <span class="SFct">titi1</span>({<span class="SNum">3</span>, <span class="SNum">4</span>})
    <span class="SFct">titi2</span>({<span class="SNum">5</span>, <span class="SNum">6</span>})
}

<span class="SKwd">func</span> <span class="SFct">titi0</span>(param: <span class="SKwd">const</span> &{ x: <span class="STpe">s32</span>, y: <span class="STpe">s32</span> })
{
    <span class="SCmt">// We'll discuss tuples and field naming later, but for now, we can access tuple items by position.</span>
    <span class="SItr">@assert</span>(param.item0 == <span class="SNum">1</span>)
    <span class="SItr">@assert</span>(param.item1 == <span class="SNum">2</span>)
}</span></div>
<p>Note that declaring a tuple type or a struct type as a parameter is equivalent to passing a constant reference. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">titi1</span>(param: { x: <span class="STpe">s32</span>, y: <span class="STpe">s32</span> })
{
    <span class="SItr">@assert</span>(param.x == <span class="SNum">3</span>)
    <span class="SItr">@assert</span>(param.y == <span class="SNum">4</span>)
}

<span class="SKwd">func</span> <span class="SFct">titi2</span>(param: <span class="SCst">MyStruct</span>)
{
    <span class="SItr">@assert</span>(param.x == <span class="SNum">5</span>)
    <span class="SItr">@assert</span>(param.y == <span class="SNum">6</span>)
}</span></div>

<h2 id="_024_any_swg">Any</h2><p><span class="code-inline">any</span> is a specific type in Swag that can store values of any other type. </p>
<div class="blockquote blockquote-warning">
<div class="blockquote-title-block"><i class="fa fa-exclamation-triangle"></i>  <span class="blockquote-title">Warning</span></div><p> <span class="code-inline">any</span> is <b>not a variant</b>. It's a dynamically typed <b>reference</b> to an existing value. This means that <span class="code-inline">any</span> does not store a copy of the value, but rather a reference to the actual value, along with its type information. </p>
</div>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> a: <span class="STpe">any</span>    <span class="SCmt">// Declare a variable of type 'any'</span>

    <span class="SCmt">// Store an 's32' literal value in the 'any'</span>
    a = <span class="SNum">6</span>

    <span class="SCmt">// To access the value stored inside 'any', you need to cast it to the desired type.</span>
    <span class="SItr">@assert</span>(<span class="SKwd">cast</span>(<span class="STpe">s32</span>) a == <span class="SNum">6</span>)

    <span class="SCmt">// Now store a string instead of the 's32' value</span>
    a = <span class="SStr">"string"</span>
    <span class="SItr">@assert</span>(<span class="SKwd">cast</span>(<span class="STpe">string</span>) a == <span class="SStr">"string"</span>)

    <span class="SCmt">// Now store a bool instead</span>
    a = <span class="SKwd">true</span>
    <span class="SItr">@assert</span>(<span class="SKwd">cast</span>(<span class="STpe">bool</span>) a == <span class="SKwd">true</span>)
}</span></div>
<p><span class="code-inline">any</span> is effectively a pointer to the value it references, along with a <span class="code-inline">typeinfo</span> that describes the type of the value. You can use <span class="code-inline">@dataof</span> to retrieve the pointer to the actual value. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">let</span> a: <span class="STpe">any</span> = <span class="SNum">6</span>                         <span class="SCmt">// Store an s32 value in 'a'</span>
    <span class="SKwd">let</span> ptr = <span class="SKwd">cast</span>(<span class="SKwd">const</span> *<span class="STpe">s32</span>) <span class="SItr">@dataof</span>(a)  <span class="SCmt">// Retrieve the pointer to the stored value</span>
    <span class="SItr">@assert</span>(<span class="SKwd">dref</span> ptr == <span class="SNum">6</span>)                 <span class="SCmt">// Dereference the pointer to get the value</span>
}</span></div>
<p>When you use <span class="code-inline">@typeof</span> on an <span class="code-inline">any</span>, it will return the type <span class="code-inline">any</span> itself, as <span class="code-inline">any</span> is the type of the reference. However, <span class="code-inline">@kindof</span> gives you the actual underlying type of the value stored in <span class="code-inline">any</span>. Note that <span class="code-inline">@kindof</span> is evaluated at runtime. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> a: <span class="STpe">any</span> = <span class="SStr">"string"</span>              <span class="SCmt">// Store a string in 'a'</span>
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(a) == <span class="STpe">any</span>          <span class="SCmt">// The type of 'a' is 'any'</span>
    <span class="SItr">@assert</span>(<span class="SItr">@kindof</span>(a) == <span class="STpe">string</span>)      <span class="SCmt">// The underlying type of the value is 'string'</span>

    a = <span class="SKwd">true</span>                           <span class="SCmt">// Change the stored value to a bool</span>
    <span class="SItr">@assert</span>(<span class="SItr">@kindof</span>(a) == <span class="STpe">bool</span>)        <span class="SCmt">// Now the underlying type is 'bool'</span>
}</span></div>
<p>You can retrieve the value stored in an <span class="code-inline">any</span> either directly or as a constant reference. This flexibility allows you to work with the value in whatever way best suits your needs. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">let</span> a: <span class="STpe">any</span> = <span class="SNum">42</span>                    <span class="SCmt">// Store an s32 value in 'a'</span>
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(a) == <span class="STpe">any</span>          <span class="SCmt">// The type of 'a' is 'any'</span>
    <span class="SItr">@assert</span>(<span class="SItr">@kindof</span>(a) == <span class="STpe">s32</span>)         <span class="SCmt">// The underlying type is 's32'</span>

    <span class="SKwd">let</span> b = <span class="SKwd">cast</span>(<span class="STpe">s32</span>) a                <span class="SCmt">// Get the value itself</span>
    <span class="SItr">@assert</span>(b == <span class="SNum">42</span>)
    <span class="SKwd">let</span> c = <span class="SKwd">cast</span>(<span class="SKwd">const</span> &<span class="STpe">s32</span>) a         <span class="SCmt">// Get a constant reference to the value</span>
    <span class="SItr">@assert</span>(c == <span class="SNum">42</span>)
}</span></div>
<p>You can also create arrays that contain multiple types using <span class="code-inline">any</span>. This allows for a heterogeneous collection where each element can be of a different type. This is useful in cases where you need to store and manipulate data of various types in a single structure. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> array: [] <span class="STpe">any</span> = [<span class="SKwd">true</span>, <span class="SNum">2</span>, <span class="SNum">3.0</span>, <span class="SStr">"4"</span>]  <span class="SCmt">// An array containing different types</span>
    <span class="SItr">@assert</span>(<span class="SItr">@kindof</span>(array[<span class="SNum">0</span>]) == <span class="STpe">bool</span>)       <span class="SCmt">// The first element is a bool</span>
    <span class="SItr">@assert</span>(<span class="SItr">@kindof</span>(array[<span class="SNum">1</span>]) == <span class="STpe">s32</span>)        <span class="SCmt">// The second element is an s32</span>
    <span class="SItr">@assert</span>(<span class="SItr">@kindof</span>(array[<span class="SNum">2</span>]) == <span class="STpe">f32</span>)        <span class="SCmt">// The third element is an f32</span>
    <span class="SItr">@assert</span>(<span class="SItr">@kindof</span>(array[<span class="SNum">3</span>]) == <span class="STpe">string</span>)     <span class="SCmt">// The fourth element is a string</span>

    <span class="SItr">@assert</span>(<span class="SKwd">cast</span>(<span class="STpe">bool</span>) array[<span class="SNum">0</span>] == <span class="SKwd">true</span>)
    <span class="SItr">@assert</span>(<span class="SKwd">cast</span>(<span class="STpe">s32</span>) array[<span class="SNum">1</span>] == <span class="SNum">2</span>)
    <span class="SItr">@assert</span>(<span class="SKwd">cast</span>(<span class="STpe">f32</span>) array[<span class="SNum">2</span>] == <span class="SNum">3.0</span>)
    <span class="SItr">@assert</span>(<span class="SKwd">cast</span>(<span class="STpe">string</span>) array[<span class="SNum">3</span>] == <span class="SStr">"4"</span>)
}</span></div>
<p>An <span class="code-inline">any</span> value can be set to null and tested against null, just like pointers or other nullable types. This feature allows you to handle cases where a value may or may not be set, providing flexibility in managing optional values. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> x: <span class="STpe">any</span>                          <span class="SCmt">// Declare an 'any' type variable</span>
    <span class="SItr">@assert</span>(x == <span class="SKwd">null</span>)                  <span class="SCmt">// Initially, 'x' is null</span>

    x = <span class="SNum">6</span>                               <span class="SCmt">// Store an s32 value in 'x'</span>
    <span class="SItr">@assert</span>(x != <span class="SKwd">null</span>)                  <span class="SCmt">// Now 'x' holds a value</span>
    <span class="SItr">@assert</span>(<span class="SKwd">cast</span>(<span class="STpe">s32</span>) x == <span class="SNum">6</span>)

    x = <span class="SStr">"string"</span>                        <span class="SCmt">// Change the stored value to a string</span>
    <span class="SItr">@assert</span>(x != <span class="SKwd">null</span>)
    <span class="SItr">@assert</span>(<span class="SKwd">cast</span>(<span class="STpe">string</span>) x == <span class="SStr">"string"</span>)

    x = <span class="SKwd">null</span>                            <span class="SCmt">// Set 'x' back to null</span>
    <span class="SItr">@assert</span>(x == <span class="SKwd">null</span>)
}</span></div>
<p>An <span class="code-inline">any</span> value can be tested against a type using <span class="code-inline">==</span> and <span class="code-inline">!=</span>. This effectively calls <span class="code-inline">@kindof</span> to retrieve the underlying type for comparison. This capability is useful when you need to ensure that the value inside <span class="code-inline">any</span> is of a specific type before performing operations on it. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> x: <span class="STpe">any</span>                          <span class="SCmt">// Declare an 'any' type variable</span>
    <span class="SItr">@assert</span>(x == <span class="SKwd">null</span>)                  <span class="SCmt">// 'x' is null</span>
    <span class="SItr">@assert</span>(x != <span class="STpe">s32</span>)                   <span class="SCmt">// 'x' does not contain an s32</span>

    x = <span class="SNum">6</span>                               <span class="SCmt">// Store an s32 value in 'x'</span>
    <span class="SItr">@assert</span>(x == <span class="STpe">s32</span>)                   <span class="SCmt">// Now 'x' contains an s32</span>
    <span class="SItr">@assert</span>(x != <span class="STpe">bool</span>)                  <span class="SCmt">// But it does not contain a bool</span>

    x = <span class="SStr">"string"</span>                        <span class="SCmt">// Change the stored value to a string</span>
    <span class="SItr">@assert</span>(x != <span class="STpe">s32</span>)                   <span class="SCmt">// Now 'x' contains a string, not an s32</span>
    <span class="SItr">@assert</span>(x == <span class="STpe">string</span>)                <span class="SCmt">// Confirm it holds a string</span>

    <span class="SKwd">struct</span> <span class="SCst">A</span> {}                         <span class="SCmt">// Define a simple struct</span>

    x = <span class="SCst">A</span>{}                             <span class="SCmt">// Store an instance of A in 'x'</span>
    <span class="SItr">@assert</span>(x == <span class="SCst">A</span>)                     <span class="SCmt">// 'x' is of type A</span>
}</span></div>

<h2 id="_025_tuple_swg">Tuple</h2><p>A tuple in Swag is an anonymous structure, also known as a struct literal. The syntax for creating a tuple is simply <span class="code-inline">{}</span>, enclosing the elements you want to include. Tuples are useful for grouping different types of data together without the need to formally define a structure. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">let</span> tuple1 = {<span class="SNum">2</span>, <span class="SNum">2</span>}                    <span class="SCmt">// A tuple with two integer elements</span>
    <span class="SKwd">let</span> tuple2 = {<span class="SStr">"string"</span>, <span class="SNum">2</span>, <span class="SKwd">true</span>}       <span class="SCmt">// A tuple with a string, an integer, and a boolean</span>
}</span></div>
<p>By default, tuple values are accessed using automatically assigned field names in the form of <span class="code-inline">itemX</span>, where <span class="code-inline">X</span> is the index of the field starting from 0. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">let</span> tuple = {<span class="SStr">"string"</span>, <span class="SNum">2</span>, <span class="SKwd">true</span>}        <span class="SCmt">// A tuple with three elements</span>
    <span class="SItr">@assert</span>(tuple.item0 == <span class="SStr">"string"</span>)       <span class="SCmt">// Access the first element</span>
    <span class="SItr">@assert</span>(tuple.item1 == <span class="SNum">2</span>)              <span class="SCmt">// Access the second element</span>
    <span class="SItr">@assert</span>(tuple.item2 == <span class="SKwd">true</span>)           <span class="SCmt">// Access the third element</span>
}</span></div>
<p>You can also specify your own names for the tuple fields, which allows for more descriptive access to the tuple's elements. When you define field names, the default <span class="code-inline">itemX</span> names are still available. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">let</span> tuple = {x: <span class="SNum">1.0</span>, y: <span class="SNum">2.0</span>}           <span class="SCmt">// A tuple with named fields 'x' and 'y'</span>
    <span class="SItr">@assert</span>(tuple.x == <span class="SNum">1.0</span>)                <span class="SCmt">// Access using the custom field name 'x'</span>
    <span class="SItr">@assert</span>(tuple.item0 == <span class="SNum">1.0</span>)            <span class="SCmt">// Access using the default 'item0'</span>
    <span class="SItr">@assert</span>(tuple.y == <span class="SNum">2.0</span>)                <span class="SCmt">// Access using the custom field name 'y'</span>
    <span class="SItr">@assert</span>(tuple.item1 == <span class="SNum">2.0</span>)            <span class="SCmt">// Access using the default 'item1'</span>
}</span></div>
<p>When creating a tuple literal with variables, the tuple fields will automatically take the names of the variables, unless you specify different names explicitly. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">let</span> x = <span class="SNum">555</span>
    <span class="SKwd">let</span> y = <span class="SNum">666</span>
    <span class="SKwd">let</span> t = {x, y}                         <span class="SCmt">// Tuple fields automatically named 'x' and 'y'</span>
    <span class="SItr">@assert</span>(t.x == <span class="SNum">555</span>)                    <span class="SCmt">// Access using the variable name 'x'</span>
    <span class="SItr">@assert</span>(t.item0 == t.x)                <span class="SCmt">// 'item0' corresponds to 'x'</span>
    <span class="SItr">@assert</span>(t.y == <span class="SNum">666</span>)                    <span class="SCmt">// Access using the variable name 'y'</span>
    <span class="SItr">@assert</span>(t.item1 == t.y)                <span class="SCmt">// 'item1' corresponds to 'y'</span>
}</span></div>
<p>Even if two tuples do not have the same field names, they can still be assigned to each other as long as the field types are compatible. This flexibility allows for easy data transfer between different tuple structures. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> x: { a: <span class="STpe">s32</span>, b: <span class="STpe">s32</span> }              <span class="SCmt">// A tuple with fields 'a' and 'b'</span>
    <span class="SKwd">var</span> y: { c: <span class="STpe">s32</span>, d: <span class="STpe">s32</span> }              <span class="SCmt">// Another tuple with fields 'c' and 'd'</span>

    y = {<span class="SNum">1</span>, <span class="SNum">2</span>}                             <span class="SCmt">// Initialize 'y' with values</span>
    x = y                                  <span class="SCmt">// Assign 'y' to 'x', field names do not need to match</span>
    <span class="SItr">@assert</span>(x.a == <span class="SNum">1</span>)                      <span class="SCmt">// 'x.a' takes the value of 'y.c'</span>
    <span class="SItr">@assert</span>(x.b == <span class="SNum">2</span>)                      <span class="SCmt">// 'x.b' takes the value of 'y.d'</span>

    <span class="SCmt">// Even though 'x' and 'y' have different field names, their types are not considered identical.</span>
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(x) != <span class="SItr">@typeof</span>(y)
}</span></div>
<h3 id="_025_tuple_swg_Tuple_unpacking">Tuple unpacking </h3>
<p>Tuples can be unpacked, meaning their fields can be extracted into individual variables. This feature is particularly useful for splitting a tuple's data into separate variables for further processing. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> tuple1 = {x: <span class="SNum">1.0</span>, y: <span class="SNum">2.0</span>}          <span class="SCmt">// A tuple with fields 'x' and 'y'</span>

    <span class="SCmt">// Unpack the tuple: 'value0' gets the value of 'x', and 'value1' gets the value of 'y'</span>
    <span class="SKwd">let</span> (value0, value1) = tuple1
    <span class="SItr">@assert</span>(value0 == <span class="SNum">1.0</span>)
    <span class="SItr">@assert</span>(value1 == <span class="SNum">2.0</span>)

    <span class="SKwd">var</span> tuple2 = {<span class="SStr">"name"</span>, <span class="SKwd">true</span>}            <span class="SCmt">// A tuple with a string and a boolean</span>
    <span class="SKwd">let</span> (name, value) = tuple2             <span class="SCmt">// Unpack into 'name' and 'value'</span>
    <span class="SItr">@assert</span>(name == <span class="SStr">"name"</span>)
    <span class="SItr">@assert</span>(value == <span class="SKwd">true</span>)
}</span></div>
<p>You can ignore certain fields when unpacking a tuple by using the <span class="code-inline">?</span> placeholder. This is useful when you only need specific elements of a tuple and want to disregard the rest. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> tuple1 = {x: <span class="SNum">1.0</span>, y: <span class="SNum">2.0</span>}          <span class="SCmt">// A tuple with fields 'x' and 'y'</span>
    <span class="SKwd">let</span> (x, ?) = tuple1                    <span class="SCmt">// Unpack 'x' and ignore the second field</span>
    <span class="SItr">@assert</span>(x == <span class="SNum">1.0</span>)
    <span class="SKwd">let</span> (?, y) = tuple1                    <span class="SCmt">// Ignore the first field and unpack 'y'</span>
    <span class="SItr">@assert</span>(y == <span class="SNum">2.0</span>)
}</span></div>

<h2 id="_030_enum_swg">Enum</h2><p>Enums in Swag allow you to define a set of named values. Unlike C/C++, enum values in Swag can end with <span class="code-inline">;</span>, <span class="code-inline">,</span>, or an end of line (eol). </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">enum</span> <span class="SCst">Values0</span>
    {
        <span class="SCst">A</span>
        <span class="SCst">B</span>
    }

    <span class="SKwd">enum</span> <span class="SCst">Values1</span>
    {
        <span class="SCst">A</span>,
        <span class="SCst">B</span>,
    }

    <span class="SCmt">// The last comma is not necessary</span>
    <span class="SKwd">enum</span> <span class="SCst">Values2</span>
    {
        <span class="SCst">A</span>,
        <span class="SCst">B</span>
    }

    <span class="SKwd">enum</span> <span class="SCst">Values3</span> { <span class="SCst">A</span>, <span class="SCst">B</span> }
    <span class="SKwd">enum</span> <span class="SCst">Values4</span> { <span class="SCst">A</span>; <span class="SCst">B</span>; }
    <span class="SKwd">enum</span> <span class="SCst">Values5</span> { <span class="SCst">A</span>; <span class="SCst">B</span> }
}</span></div>
<p>By default, an enum in Swag is of type <span class="code-inline">s32</span>, meaning the underlying storage for each value is a 32-bit signed integer. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">enum</span> <span class="SCst">Values</span> { <span class="SCst">A</span>, <span class="SCst">B</span> }
    <span class="SKwd">let</span> type = <span class="SItr">@typeof</span>(<span class="SCst">Values</span>)

    <span class="SCmt">// 'type' here is a 'typeinfo' dedicated to the enum type.</span>
    <span class="SCmt">// In that case, 'type.rawType' represents the enum's underlying type.</span>
    <span class="SItr">@assert</span>(type.rawType == <span class="STpe">s32</span>)
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(<span class="SCst">Values</span>) == <span class="SCst">Values</span>
}</span></div>
<p><span class="code-inline">@kindof</span> can be used to return the underlying type of the enum at runtime. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">enum</span> <span class="SCst">RGB</span> { <span class="SCst">R</span>, <span class="SCst">G</span>, <span class="SCst">B</span> }
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(<span class="SCst">RGB</span>) == <span class="SCst">RGB</span>
    <span class="SCmp">#assert</span> <span class="SItr">@kindof</span>(<span class="SCst">RGB</span>) != <span class="SCst">RGB</span>
    <span class="SCmp">#assert</span> <span class="SItr">@kindof</span>(<span class="SCst">RGB</span>) == <span class="STpe">s32</span>
}</span></div>
<p>You can specify a custom underlying type for an enum by appending it after the enum name. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">enum</span> <span class="SCst">Values1</span>: <span class="STpe">s64</span> <span class="SCmt">// Force the underlying type to be 's64' instead of 's32'</span>
    {
        <span class="SCst">A</span>
        <span class="SCst">B</span>
        <span class="SCst">C</span>
    }

    <span class="SCmp">#assert</span> <span class="SItr">@kindof</span>(<span class="SCst">Values1</span>) == <span class="STpe">s64</span>
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(<span class="SCst">Values1</span>.<span class="SCst">A</span>) == <span class="SCst">Values1</span>
}</span></div>
<p>Enum values, if not explicitly specified, start at 0 and increase by 1 for each subsequent value. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">enum</span> <span class="SCst">Value</span>: <span class="STpe">s64</span>
    {
        <span class="SCst">A</span>
        <span class="SCst">B</span>
        <span class="SCst">C</span>
    }

    <span class="SCmp">#assert</span> <span class="SCst">Value</span>.<span class="SCst">A</span> == <span class="SNum">0</span>
    <span class="SCmp">#assert</span> <span class="SCst">Value</span>.<span class="SCst">B</span> == <span class="SNum">1</span>
    <span class="SCmp">#assert</span> <span class="SCst">Value</span>.<span class="SCst">C</span> == <span class="SNum">2</span>
}</span></div>
<p>You can specify custom values for each enum element. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">enum</span> <span class="SCst">Value</span>: <span class="STpe">s64</span>
    {
        <span class="SCst">A</span> = <span class="SNum">10</span>
        <span class="SCst">B</span> = <span class="SNum">20</span>
        <span class="SCst">C</span> = <span class="SNum">30</span>
    }

    <span class="SCmp">#assert</span> <span class="SCst">Value</span>.<span class="SCst">A</span> == <span class="SNum">10</span>
    <span class="SCmp">#assert</span> <span class="SCst">Value</span>.<span class="SCst">B</span> == <span class="SNum">20</span>
    <span class="SCmp">#assert</span> <span class="SCst">Value</span>.<span class="SCst">C</span> == <span class="SNum">30</span>
}</span></div>
<p>If you omit a value after assigning a specific one, it will be assigned the previous value plus 1. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">enum</span> <span class="SCst">Value</span>: <span class="STpe">u32</span>
    {
        <span class="SCst">A</span> = <span class="SNum">10</span>
        <span class="SCst">B</span> <span class="SCmt">// Will be 11</span>
        <span class="SCst">C</span> <span class="SCmt">// Will be 12</span>
    }

    <span class="SCmp">#assert</span> <span class="SCst">Value</span>.<span class="SCst">A</span> == <span class="SNum">10</span>
    <span class="SCmp">#assert</span> <span class="SCst">Value</span>.<span class="SCst">B</span> == <span class="SNum">11</span>
    <span class="SCmp">#assert</span> <span class="SCst">Value</span>.<span class="SCst">C</span> == <span class="SNum">12</span>
}</span></div>
<p>For non-integer types, you <b>must</b> specify the values explicitly, as they cannot be deduced automatically. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">enum</span> <span class="SCst">Value1</span>: <span class="STpe">string</span>
    {
        <span class="SCst">A</span> = <span class="SStr">"string 1"</span>
        <span class="SCst">B</span> = <span class="SStr">"string 2"</span>
        <span class="SCst">C</span> = <span class="SStr">"string 3"</span>
    }

    <span class="SCmp">#assert</span> <span class="SCst">Value1</span>.<span class="SCst">A</span> == <span class="SStr">"string 1"</span>
    <span class="SCmp">#assert</span> <span class="SCst">Value1</span>.<span class="SCst">B</span> == <span class="SStr">"string 2"</span>
    <span class="SCmp">#assert</span> <span class="SCst">Value1</span>.<span class="SCst">C</span> == <span class="SStr">"string 3"</span>

    <span class="SKwd">enum</span> <span class="SCst">Value2</span>: <span class="STpe">f32</span>
    {
        <span class="SCst">A</span> = <span class="SNum">1.0</span>
        <span class="SCst">B</span> = <span class="SNum">3.14</span>
        <span class="SCst">C</span> = <span class="SNum">6.0</span>
    }

    <span class="SCmp">#assert</span> <span class="SCst">Value2</span>.<span class="SCst">A</span> == <span class="SNum">1.0</span>
    <span class="SCmp">#assert</span> <span class="SCst">Value2</span>.<span class="SCst">B</span> == <span class="SNum">3.14</span>
    <span class="SCmp">#assert</span> <span class="SCst">Value2</span>.<span class="SCst">C</span> == <span class="SNum">6.0</span>
}</span></div>
<p><span class="code-inline">@countof</span> can be used to return the number of values defined in an enum. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">enum</span> <span class="SCst">Value</span>: <span class="STpe">string</span>
    {
        <span class="SCst">A</span> = <span class="SStr">"string 1"</span>
        <span class="SCst">B</span> = <span class="SStr">"string 2"</span>
        <span class="SCst">C</span> = <span class="SStr">"string 3"</span>
    }

    <span class="SItr">@assert</span>(<span class="SItr">@countof</span>(<span class="SCst">Value</span>) == <span class="SNum">3</span>)
    <span class="SCmp">#assert</span> <span class="SItr">@countof</span>(<span class="SCst">Value</span>) == <span class="SNum">3</span>
}</span></div>
<p>You can use the <span class="code-inline">using</span> keyword to make enum values accessible without needing to specify the enum name. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">enum</span> <span class="SCst">Value</span>
    {
        <span class="SCst">A</span>
        <span class="SCst">B</span>
        <span class="SCst">C</span>
    }

    <span class="SKwd">using</span> <span class="SCst">Value</span>

    <span class="SCmt">// No need to prefix with 'Value.'</span>
    <span class="SItr">@assert</span>(<span class="SCst">A</span> == <span class="SNum">0</span>)
    <span class="SItr">@assert</span>(<span class="SCst">B</span> == <span class="SNum">1</span>)
    <span class="SItr">@assert</span>(<span class="SCst">C</span> == <span class="SNum">2</span>)

    <span class="SCmt">// The normal form is still possible</span>
    <span class="SItr">@assert</span>(<span class="SCst">Value</span>.<span class="SCst">A</span> == <span class="SNum">0</span>)
    <span class="SItr">@assert</span>(<span class="SCst">Value</span>.<span class="SCst">B</span> == <span class="SNum">1</span>)
    <span class="SItr">@assert</span>(<span class="SCst">Value</span>.<span class="SCst">C</span> == <span class="SNum">2</span>)
}</span></div>
<h3 id="_030_enum_swg_Enum_as_flags">Enum as flags </h3>
<p>Enums can be used as flags if declared with the <span class="code-inline">#[Swag.EnumFlags]</span> attribute. For this, the enum's underlying type should be <span class="code-inline">u8</span>, <span class="code-inline">u16</span>, <span class="code-inline">u32</span>, or <span class="code-inline">u64</span>. By default, enums declared as flags start at 1 (not 0), and each value is typically a power of 2. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SAtr">#[Swag.EnumFlags]</span>
    <span class="SKwd">enum</span> <span class="SCst">MyFlags</span>: <span class="STpe">u8</span>
    {
        <span class="SCst">A</span>   <span class="SCmt">// First value is 1</span>
        <span class="SCst">B</span>   <span class="SCmt">// Value is 2</span>
        <span class="SCst">C</span>   <span class="SCmt">// Value is 4</span>
        <span class="SCst">D</span>   <span class="SCmt">// Value is 8</span>
    }

    <span class="SCmp">#assert</span> <span class="SCst">MyFlags</span>.<span class="SCst">A</span> == <span class="SNum">0b00000001</span>
    <span class="SCmp">#assert</span> <span class="SCst">MyFlags</span>.<span class="SCst">B</span> == <span class="SNum">0b00000010</span>
    <span class="SCmp">#assert</span> <span class="SCst">MyFlags</span>.<span class="SCst">C</span> == <span class="SNum">0b00000100</span>
    <span class="SCmp">#assert</span> <span class="SCst">MyFlags</span>.<span class="SCst">D</span> == <span class="SNum">0b00001000</span>

    <span class="SKwd">let</span> value = <span class="SCst">MyFlags</span>.<span class="SCst">B</span> | <span class="SCst">MyFlags</span>.<span class="SCst">C</span>
    <span class="SItr">@assert</span>(value == <span class="SNum">0b00000110</span>)
    <span class="SItr">@assert</span>(value & <span class="SCst">MyFlags</span>.<span class="SCst">B</span> == <span class="SCst">MyFlags</span>.<span class="SCst">B</span>)
    <span class="SItr">@assert</span>(value & <span class="SCst">MyFlags</span>.<span class="SCst">C</span> == <span class="SCst">MyFlags</span>.<span class="SCst">C</span>)
}</span></div>
<h3 id="_030_enum_swg_Enum_of_arrays">Enum of arrays </h3>
<p>You can define an enum where each value is associated with a const static array. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">enum</span> <span class="SCst">Value</span>: <span class="SKwd">const</span> [<span class="SNum">2</span>] <span class="STpe">s32</span>
    {
        <span class="SCst">A</span> = [<span class="SNum">1</span>, <span class="SNum">2</span>]
        <span class="SCst">B</span> = [<span class="SNum">10</span>, <span class="SNum">20</span>]
    }

    <span class="SCmp">#assert</span> <span class="SCst">Value</span>.<span class="SCst">A</span>[<span class="SNum">0</span>] == <span class="SNum">1</span>
    <span class="SCmp">#assert</span> <span class="SCst">Value</span>.<span class="SCst">A</span>[<span class="SNum">1</span>] == <span class="SNum">2</span>
    <span class="SCmp">#assert</span> <span class="SCst">Value</span>.<span class="SCst">B</span>[<span class="SNum">0</span>] == <span class="SNum">10</span>
    <span class="SCmp">#assert</span> <span class="SCst">Value</span>.<span class="SCst">B</span>[<span class="SNum">1</span>] == <span class="SNum">20</span>
}</span></div>
<h3 id="_030_enum_swg_Enum_of_slices">Enum of slices </h3>
<p>Similarly, you can define an enum where each value is associated with a const slice. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">enum</span> <span class="SCst">Value</span>: <span class="SKwd">const</span> [..] <span class="STpe">s32</span>
    {
        <span class="SCst">A</span> = [<span class="SNum">1</span>, <span class="SNum">2</span>]
        <span class="SCst">B</span> = [<span class="SNum">10</span>, <span class="SNum">20</span>, <span class="SNum">30</span>, <span class="SNum">40</span>]
    }

    <span class="SCmp">#assert</span> <span class="SItr">@countof</span>(<span class="SCst">Value</span>.<span class="SCst">A</span>) == <span class="SNum">2</span>
    <span class="SCmp">#assert</span> <span class="SItr">@countof</span>(<span class="SCst">Value</span>.<span class="SCst">B</span>) == <span class="SNum">4</span>

    <span class="SKwd">let</span> x = <span class="SCst">Value</span>.<span class="SCst">A</span>
    <span class="SItr">@assert</span>(x[<span class="SNum">0</span>] == <span class="SNum">1</span>)
    <span class="SItr">@assert</span>(x[<span class="SNum">1</span>] == <span class="SNum">2</span>)
    <span class="SKwd">let</span> y = <span class="SCst">Value</span>.<span class="SCst">B</span>
    <span class="SItr">@assert</span>(y[<span class="SNum">0</span>] == <span class="SNum">10</span>)
    <span class="SItr">@assert</span>(y[<span class="SNum">1</span>] == <span class="SNum">20</span>)
}</span></div>
<h3 id="_030_enum_swg_Nested_enums">Nested enums </h3>
<p>Enums can be nested within other enums using the <span class="code-inline">using</span> keyword. Both enums must share the same underlying type. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">enum</span> <span class="SCst">BasicErrors</span>
{
    <span class="SCst">FailedToLoad</span>
    <span class="SCst">FailedToSave</span>
}

<span class="SCmt">// The enum 'BasicErrors' is nested inside 'MyErrors'</span>
<span class="SKwd">enum</span> <span class="SCst">MyErrors</span>
{
    <span class="SKwd">using</span> <span class="SCst">BasicErrors</span>
    <span class="SCst">NotFound</span> = <span class="SNum">100</span>
}</span></div>
<p>To access a value inside a nested enum, use the enum name (a scope is created). </p>
<div class="code-block"><span class="SCde"><span class="SKwd">const</span> <span class="SCst">MyError0</span> = <span class="SCst">MyErrors</span>.<span class="SCst">BasicErrors</span>.<span class="SCst">FailedToSave</span></span></div>
<p>An <b>automatic cast</b> occurs when converting a nested enum to its parent enum. For example, a value of type <span class="code-inline">BasicErrors</span> can be passed to a parameter of type <span class="code-inline">MyErrors</span> because <span class="code-inline">MyErrors</span> includes <span class="code-inline">BasicErrors</span>. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">const</span> <span class="SCst">E0</span>: <span class="SCst">MyErrors</span>    = <span class="SCst">MyErrors</span>.<span class="SCst">BasicErrors</span>.<span class="SCst">FailedToLoad</span>
    <span class="SKwd">const</span> <span class="SCst">E1</span>: <span class="SCst">BasicErrors</span> = <span class="SCst">BasicErrors</span>.<span class="SCst">FailedToLoad</span>

    <span class="SKwd">func</span> <span class="SFct">toto</span>(err: <span class="SCst">MyErrors</span>)
    {
        <span class="SItr">@assert</span>(err == <span class="SCst">BasicErrors</span>.<span class="SCst">FailedToLoad</span>)
        <span class="SItr">@assert</span>(err == <span class="SCst">MyErrors</span>.<span class="SCst">BasicErrors</span>.<span class="SCst">FailedToLoad</span>)
    }

    <span class="SFct">toto</span>(<span class="SCst">E0</span>)
    <span class="SFct">toto</span>(<span class="SCst">E1</span>) <span class="SCmt">// Automatic cast from 'BasicErrors' to 'MyErrors'</span>
}</span></div>
<h3 id="_030_enum_swg_Specific_attributes">Specific attributes </h3>
<p>You can use <span class="code-inline">#[Swag.EnumIndex]</span> to enable an enum value to be used directly as an index without needing to cast it. The underlying enum type must be an integer type. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SAtr">#[Swag.EnumIndex]</span>
    <span class="SKwd">enum</span> <span class="SCst">MyIndex</span> { <span class="SCst">First</span>, <span class="SCst">Second</span>, <span class="SCst">Third</span> }

    <span class="SKwd">const</span> <span class="SCst">Array</span> = [<span class="SNum">0</span>, <span class="SNum">1</span>, <span class="SNum">2</span>]
    <span class="SKwd">const</span> <span class="SCst">Valu</span> = <span class="SCst">Array</span>[<span class="SCst">MyIndex</span>.<span class="SCst">First</span>]   <span class="SCmt">// No need to cast 'MyIndex.First'</span>
}</span></div>
<p>You can use <span class="code-inline">#[Swag.NoDuplicate]</span> to prevent duplicated values inside an enum. If the compiler detects a value defined more than once, it will raise an error. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SAtr">#[Swag.NoDuplicate]</span>
    <span class="SKwd">enum</span> <span class="SCst">MyEnum</span>
    {
        <span class="SCst">Val0</span> = <span class="SNum">0</span>
        <span class="SCmt">//Val1 = 0      // Will raise an error because '0' is already defined</span>
    }
}</span></div>
<h3 id="_030_enum_swg_Enum_type_inference">Enum type inference </h3>
<p>Enums in Swag allow type inference, so you can often omit the enum type when assigning a value. </p>
<p>The enum type is not necessary in the assignment expression when declaring a variable, as it can be deduced. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">enum</span> <span class="SCst">Values</span> { <span class="SCst">A</span>; <span class="SCst">B</span>; }

    <span class="SCmt">// The explicit form</span>
    <span class="SKwd">let</span> x: <span class="SCst">Values</span> = <span class="SCst">Values</span>.<span class="SCst">A</span>

    <span class="SCmt">// The inferred form</span>
    <span class="SKwd">let</span> y: <span class="SCst">Values</span> = <span class="SCst">A</span>

    <span class="SItr">@assert</span>(x == y)
}</span></div>
<p>The enum type is not necessary in a <span class="code-inline">case</span> expression of a <span class="code-inline">switch</span> block, as it is inferred from the switch expression. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">enum</span> <span class="SCst">Values</span> { <span class="SCst">A</span>; <span class="SCst">B</span>; }
    <span class="SKwd">let</span> x = <span class="SCst">Values</span>.<span class="SCst">A</span>

    <span class="SCmt">// The explicit form</span>
    <span class="SLgc">switch</span> x
    {
    <span class="SLgc">case</span> <span class="SCst">Values</span>.<span class="SCst">A</span>: <span class="SLgc">break</span>
    <span class="SLgc">case</span> <span class="SCst">Values</span>.<span class="SCst">B</span>: <span class="SLgc">break</span>
    }

    <span class="SCmt">// The simplified form</span>
    <span class="SLgc">switch</span> x
    {
    <span class="SLgc">case</span> <span class="SCst">A</span>: <span class="SLgc">break</span>
    <span class="SLgc">case</span> <span class="SCst">B</span>: <span class="SLgc">break</span>
    }
}</span></div>
<p>In expressions where the enum type can be deduced, you can omit the enum name and use the <span class="code-inline">.Value</span> syntax. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">enum</span> <span class="SCst">Values</span> { <span class="SCst">A</span>, <span class="SCst">B</span> }

    <span class="SCmt">// The explicit form</span>
    <span class="SKwd">let</span> x = <span class="SCst">Values</span>.<span class="SCst">A</span>

    <span class="SCmt">// The simplified form, since 'Values' can be inferred from 'x'</span>
    <span class="SItr">@assert</span>(x == .<span class="SCst">A</span>)
    <span class="SItr">@assert</span>(x != .<span class="SCst">B</span>)
}</span></div>
<p>This also works for enums used as flags. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SAtr">#[Swag.EnumFlags]</span>
    <span class="SKwd">enum</span> <span class="SCst">Values</span> { <span class="SCst">A</span>, <span class="SCst">B</span> }

    <span class="SKwd">let</span> x = <span class="SCst">Values</span>.<span class="SCst">A</span> | <span class="SCst">Values</span>.<span class="SCst">B</span>
    <span class="SItr">@assert</span>((x & .<span class="SCst">A</span>) <span class="SLgc">and</span> (x & .<span class="SCst">B</span>))
}</span></div>
<p>In most cases, the simplified <span class="code-inline">.Value</span> syntax also works when passing enum values to functions. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">enum</span> <span class="SCst">Values</span> { <span class="SCst">A</span>; <span class="SCst">B</span>; }
    <span class="SKwd">func</span> <span class="SFct">toto</span>(v1, v2: <span class="SCst">Values</span>) {}
    <span class="SFct">toto</span>(.<span class="SCst">A</span>, .<span class="SCst">B</span>)
}</span></div>
<h3 id="_030_enum_swg_Visiting_Enum_Values">Visiting Enum Values </h3>
<p>Using type reflection, Swag allows you to iterate over all the values of an enum. This feature is particularly useful when you need to perform operations across all enum values or when you want to dynamically generate behavior based on the values of an enum. </p>
<p>The two primary mechanisms for iterating over enum values are the <span class="code-inline">loop</span> construct and the <span class="code-inline">visit</span> statement. </p>
<h4 id="_030_enum_swg_Visiting_Enum_Values_Loop">Loop </h4>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">enum</span> <span class="SCst">RGB</span> { <span class="SCst">R</span>, <span class="SCst">G</span>, <span class="SCst">B</span> }                  <span class="SCmt">// Define a simple enum with three values: R, G, and B</span>

    <span class="SCmt">// Counting Enum Values</span>
    <span class="SKwd">var</span> cpt = <span class="SNum">0</span>
    <span class="SLgc">loop</span> idx <span class="SLgc">in</span> <span class="SCst">RGB</span>:                      <span class="SCmt">// Iterate over all enum values using 'loop'</span>
        cpt += <span class="SNum">1</span>                          <span class="SCmt">// Increment counter for each value</span>
    <span class="SItr">@assert</span>(cpt == <span class="SNum">3</span>)                     <span class="SCmt">// Assert that the enum has exactly 3 values</span>

    <span class="SCmt">// Note that '@countof(RGB)' will give you the exact same result</span>
    <span class="SItr">@assert</span>(cpt == <span class="SItr">@countof</span>(<span class="SCst">RGB</span>))
}</span></div>
<p>The <span class="code-inline">loop idx in RGB:</span> statement is a powerful construct that allows you to iterate over all values in the <span class="code-inline">RGB</span> enum. During each iteration, <span class="code-inline">idx</span> holds the current enum value, which can be used within the loop body. In the example above, the loop simply counts the number of values in the enum, asserting that the total is 3. </p>
<p>This method is useful when you need to apply the same operation to each enum value, such as populating a list, checking conditions, or performing calculations. </p>
<h4 id="_030_enum_swg_Visiting_Enum_Values_Visit">Visit </h4>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">enum</span> <span class="SCst">RGB</span> { <span class="SCst">R</span>, <span class="SCst">G</span>, <span class="SCst">B</span> }                  <span class="SCmt">// Define a simple enum with three values: R, G, and B</span>

    <span class="SCmt">// Performing Actions Based on Enum Values</span>
    <span class="SLgc">visit</span> val <span class="SLgc">in</span> <span class="SCst">RGB</span>                      <span class="SCmt">// Use 'visit' to iterate over each value in the enum</span>
    {
        <span class="SLgc">switch</span> val                        <span class="SCmt">// A switch statement to perform actions based on the enum value</span>
        {
        <span class="SLgc">case</span> <span class="SCst">R</span>:                           <span class="SCmt">// Case for the value 'R'</span>
            <span class="SCmt">// Perform action for 'R'</span>
            <span class="SLgc">break</span>
        <span class="SLgc">case</span> <span class="SCst">G</span>:                           <span class="SCmt">// Case for the value 'G'</span>
            <span class="SCmt">// Perform action for 'G'</span>
            <span class="SLgc">break</span>
        <span class="SLgc">case</span> <span class="SCst">B</span>:                           <span class="SCmt">// Case for the value 'B'</span>
            <span class="SCmt">// Perform action for 'B'</span>
            <span class="SLgc">break</span>
        <span class="SLgc">default</span>:                          
            <span class="SCmt">// This should never be reached as we are covering all enum cases</span>
            <span class="SItr">@assert</span>(<span class="SKwd">false</span>)
        }
    }
}</span></div>
<p>The <span class="code-inline">visit val in RGB</span> statement offers a more structured way to iterate over an enum. Each iteration provides the current enum value in the <span class="code-inline">val</span> variable, which can then be used in a <span class="code-inline">switch</span> statement (or other logic) to perform specific actions based on the value. </p>
<p>In the provided example, the <span class="code-inline">switch</span> statement checks the value of <span class="code-inline">val</span> and executes different blocks of code depending on whether <span class="code-inline">val</span> is <span class="code-inline">R</span>, <span class="code-inline">G</span>, or <span class="code-inline">B</span>. This is particularly useful when you need to perform different actions based on the specific value of the enum, rather than treating all values the same. </p>

<h2 id="_031_impl_swg">Impl</h2><p>The <span class="code-inline">impl</span> keyword in Swag allows you to define methods and functions within the scope of an enum. This feature enhances the functionality of enums by enabling you to associate behaviors directly with the enum values. The keyword <span class="code-inline">self</span> is used within these methods to refer to the current enum value. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">enum</span> <span class="SCst">RGB</span>
{
    <span class="SCst">R</span>
    <span class="SCst">G</span>
    <span class="SCst">B</span>
}</span></div>
<p>In the example below, we use the <span class="code-inline">impl enum</span> syntax to define methods for the <span class="code-inline">RGB</span> enum. The <span class="code-inline">impl</span> keyword is used to declare that the following block contains methods associated with the <span class="code-inline">RGB</span> enum. We will explore <span class="code-inline">impl</span> further when we look at structs, but the concept is similar. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">impl</span> <span class="SKwd">enum</span> <span class="SCst">RGB</span>
{
    <span class="SCmt">// This method checks if the enum value is `R`.</span>
    <span class="SKwd">func</span> <span class="SFct">isRed</span>(<span class="STpe">self</span>) =&gt; <span class="STpe">self</span> == <span class="SCst">R</span>

    <span class="SCmt">// This method checks if the enum value is either `R` or `B`.</span>
    <span class="SKwd">func</span> <span class="SFct">isRedOrBlue</span>(<span class="STpe">self</span>) =&gt; <span class="STpe">self</span> == <span class="SCst">R</span> <span class="SLgc">or</span> <span class="STpe">self</span> == <span class="SCst">B</span>
}

<span class="SFct">#test</span>
{
    <span class="SCmt">// Test the methods defined in the `impl` block.</span>

    <span class="SCmt">// Check if `RGB.R` is red</span>
    <span class="SItr">@assert</span>(<span class="SCst">RGB</span>.<span class="SFct">isRed</span>(<span class="SCst">RGB</span>.<span class="SCst">R</span>))

    <span class="SCmt">// Check if `RGB.B` is red or blue</span>
    <span class="SItr">@assert</span>(<span class="SCst">RGB</span>.<span class="SFct">isRedOrBlue</span>(<span class="SCst">RGB</span>.<span class="SCst">B</span>))

    <span class="SCmt">// The `using` keyword allows us to omit the enum type when calling methods</span>
    <span class="SKwd">using</span> <span class="SCst">RGB</span>
    <span class="SItr">@assert</span>(<span class="SFct">isRedOrBlue</span>(<span class="SCst">R</span>))
    <span class="SItr">@assert</span>(<span class="SFct">isRedOrBlue</span>(<span class="SCst">B</span>))

    <span class="SCmt">// A first look at Uniform Function Call Syntax (UFCS)</span>
    <span class="SCmt">// UFCS allows calling methods directly on enum values, improving readability</span>
    <span class="SItr">@assert</span>(<span class="SCst">R</span>.<span class="SFct">isRedOrBlue</span>())
    <span class="SItr">@assert</span>(!<span class="SCst">RGB</span>.<span class="SCst">G</span>.<span class="SFct">isRedOrBlue</span>())
}</span></div>

<h2 id="_035_namespace_swg">Namespace</h2><p>Namespaces in Swag allow you to create a global scope for symbols, helping organize your code and avoid naming conflicts. A namespace groups functions, variables, and other declarations under a specific name, making them accessible only through that name unless otherwise specified. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">namespace</span> <span class="SCst">A</span>
{
    <span class="SCmt">// This function is inside the namespace 'A'.</span>
    <span class="SKwd">func</span> <span class="SFct">a</span>() =&gt; <span class="SNum">1</span>
}</span></div>
<p>You can nest namespaces to create hierarchical structures. In this example, <span class="code-inline">C</span> is a namespace inside <span class="code-inline">B</span>, which is itself inside <span class="code-inline">A</span>. This allows for even more granular organization of symbols. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">namespace</span> <span class="SCst">A</span>.<span class="SCst">B</span>.<span class="SCst">C</span>
{
    <span class="SCmt">// This function is inside the nested namespace 'A.B.C'.</span>
    <span class="SKwd">func</span> <span class="SFct">a</span>() =&gt; <span class="SNum">2</span>
}

<span class="SFct">#test</span>
{
    <span class="SCmt">// Accessing functions using their full namespace paths.</span>
    <span class="SItr">@assert</span>(<span class="SCst">A</span>.<span class="SFct">a</span>() == <span class="SNum">1</span>)          <span class="SCmt">// Calls 'a' from namespace 'A'</span>
    <span class="SItr">@assert</span>(<span class="SCst">A</span>.<span class="SCst">B</span>.<span class="SCst">C</span>.<span class="SFct">a</span>() == <span class="SNum">2</span>)      <span class="SCmt">// Calls 'a' from nested namespace 'A.B.C'</span>
}</span></div>
<p>The <span class="code-inline">using</span> keyword can be applied to namespaces, allowing you to access symbols without needing to fully qualify them in the current file. This can make code more concise and easier to read by removing the need for repeated namespace references. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">using</span> <span class="SKwd">namespace</span> <span class="SCst">Private</span>
{
    <span class="SKwd">const</span> <span class="SCst">FileSymbol</span> = <span class="SNum">0</span>         <span class="SCmt">// A constant defined inside the 'Private' namespace</span>
}

<span class="SKwd">const</span> <span class="SCst">B</span> = <span class="SCst">Private</span>.<span class="SCst">FileSymbol</span>     <span class="SCmt">// Accessing 'FileSymbol' using the full namespace</span>
<span class="SKwd">const</span> <span class="SCst">C</span> = <span class="SCst">FileSymbol</span>             <span class="SCmt">// Direct access to 'FileSymbol' thanks to 'using'</span></span></div>
<p>You can also create a private scope using <span class="code-inline">private</span> instead of a named namespace. In this case, the compiler generates a unique name for the scope, effectively making all enclosed symbols private to the file. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">private</span>
{
    <span class="SKwd">const</span> <span class="SCst">OtherSymbol</span> = <span class="SNum">0</span>        <span class="SCmt">// A constant defined in an unnamed private scope</span>
}

<span class="SKwd">const</span> <span class="SCst">D</span> = <span class="SCst">OtherSymbol</span>            <span class="SCmt">// Direct access to 'OtherSymbol' since it is private to the file</span></span></div>
<p>By default, all symbols from a Swag source file are exported to other files within the same module. However, using <span class="code-inline">private</span> or namespaces helps protect against name conflicts, especially in larger projects where multiple files may define similar or identical symbols. </p>

<h2 id="_050_if_swg">If</h2><p>A basic test with an <span class="code-inline">if</span>. </p>
<p>In Swag, curly braces are optional for control structures like <span class="code-inline">if</span>, but if you omit them, you must use a colon <span class="code-inline">:</span>. This rule also applies to other control structures such as <span class="code-inline">while</span>, <span class="code-inline">for</span>, and <span class="code-inline">loop</span>. </p>
<p>Unlike C/C++, the condition in an <span class="code-inline">if</span> statement does not need to be enclosed in parentheses. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> a = <span class="SNum">0</span>

    <span class="SLgc">if</span> a == <span class="SNum">1</span>:                    <span class="SCmt">// No curly braces, so use ':'</span>
        <span class="SItr">@assert</span>(<span class="SKwd">false</span>)            <span class="SCmt">// This block will not execute because the condition is false</span>

    <span class="SLgc">if</span> (a == <span class="SNum">1</span>):                  <span class="SCmt">// Parentheses can still be used if desired</span>
        <span class="SItr">@assert</span>(<span class="SKwd">false</span>)

    <span class="SLgc">if</span> a == <span class="SNum">0</span>                     <span class="SCmt">// Curly braces can be used, no colon needed</span>
    {
        <span class="SItr">@assert</span>(<span class="SKwd">true</span>)             <span class="SCmt">// This block will execute because the condition is true</span>
    }

    <span class="SCmt">// 'else' can be used as in most languages.</span>
    <span class="SCmt">// If there are no curly braces, the colon ':' is mandatory.</span>
    <span class="SLgc">if</span> a == <span class="SNum">0</span>:
        a += <span class="SNum">1</span>                    <span class="SCmt">// This will execute, making 'a' equal to 1</span>
    <span class="SLgc">else</span>:
        a += <span class="SNum">2</span>
    <span class="SItr">@assert</span>(a == <span class="SNum">1</span>)

    <span class="SCmt">// 'elif' is used for else-if chains, similar to 'else if'.</span>
    <span class="SLgc">if</span> a == <span class="SNum">1</span>:
        a += <span class="SNum">1</span>                    <span class="SCmt">// This will execute, making 'a' equal to 2</span>
    <span class="SLgc">else</span>:
        <span class="SLgc">if</span> a == <span class="SNum">2</span>:
            <span class="SItr">@assert</span>(<span class="SKwd">false</span>)
        <span class="SLgc">elif</span> a == <span class="SNum">3</span>:
            <span class="SItr">@assert</span>(<span class="SKwd">false</span>)
        <span class="SLgc">elif</span> a == <span class="SNum">4</span>:
            <span class="SItr">@assert</span>(<span class="SKwd">false</span>)

    <span class="SCmt">// Logical expressions with 'and' and 'or' work as expected</span>
    <span class="SLgc">if</span> a == <span class="SNum">0</span> <span class="SLgc">and</span> a == <span class="SNum">1</span>:         <span class="SCmt">// This condition is false</span>
        <span class="SItr">@assert</span>(<span class="SKwd">false</span>)
    <span class="SLgc">if</span> a == <span class="SNum">0</span> <span class="SLgc">or</span> a == <span class="SNum">1</span>:          <span class="SCmt">// This condition is true (a is 1)</span>
        <span class="SItr">@assert</span>(<span class="SKwd">true</span>)
}</span></div>
<h3 id="_050_if_swg_Variable_declaration">Variable declaration </h3>
<p>In an <span class="code-inline">if</span> expression, you can simultaneously declare and test a variable. The use of <span class="code-inline">var</span>, <span class="code-inline">let</span>, or <span class="code-inline">const</span> is mandatory in this context. </p>
<p>The <span class="code-inline">if</span> statement will implicitly convert the declared variable to a boolean, evaluating it as <span class="code-inline">true</span> if it is non-zero. This means the <span class="code-inline">if</span> block will execute only if the declared variable is not zero. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SCmt">// Declare and test 'a' in the same expression.</span>
    <span class="SCmt">// Since 'a' is 0, the if block will not execute.</span>
    <span class="SCmt">// 'a' is only visible within this 'if' block.</span>
    <span class="SLgc">if</span> <span class="SKwd">let</span> a = <span class="SNum">0</span>
    {
        <span class="SItr">@assert</span>(<span class="SKwd">false</span>)
    }

    <span class="SCmt">// You can redeclare 'a' as a constant in another block.</span>
    <span class="SCmt">// Since 'a' is 1, the if block will execute.</span>
    <span class="SLgc">if</span> <span class="SKwd">const</span> a = <span class="SNum">1</span>:
        <span class="SItr">@assert</span>(a == <span class="SNum">1</span>)
    <span class="SLgc">else</span>:
        <span class="SItr">@assert</span>(<span class="SKwd">false</span>)

    <span class="SCmt">// Another example with 'let'</span>
    <span class="SLgc">if</span> <span class="SKwd">let</span> a = <span class="SNum">1</span>:
        <span class="SItr">@assert</span>(a == <span class="SNum">1</span>)
    <span class="SLgc">else</span>:
        <span class="SItr">@assert</span>(<span class="SKwd">false</span>)
}</span></div>
<p>You can also add an additional condition to the <span class="code-inline">if</span> statement with a <span class="code-inline">where</span> clause. This clause is evaluated only if the initial test passes. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">func</span> <span class="SFct">retSomething</span>()-&gt;<span class="STpe">string</span> =&gt; <span class="SStr">"string"</span>
    <span class="SKwd">func</span> <span class="SFct">retNothing</span>()-&gt;<span class="STpe">string</span>   =&gt; <span class="SKwd">null</span>

    <span class="SCmt">// The 'where' clause is checked only if 'str' is not 0.</span>
    <span class="SLgc">if</span> <span class="SKwd">let</span> str = <span class="SFct">retSomething</span>() <span class="SLgc">where</span> str[<span class="SNum">0</span>] == <span class="SStr">`s`</span>:
        <span class="SItr">@assert</span>(<span class="SKwd">true</span>)
    <span class="SLgc">else</span>:
        <span class="SItr">@assert</span>(<span class="SKwd">false</span>)

    <span class="SCmt">// In this case, the initial test fails because 'str' is null,</span>
    <span class="SCmt">// so the 'where' clause is not evaluated.</span>
    <span class="SLgc">if</span> <span class="SKwd">let</span> str = <span class="SFct">retNothing</span>() <span class="SLgc">where</span> str[<span class="SNum">0</span>] == <span class="SStr">`s`</span>:
        <span class="SItr">@assert</span>(<span class="SKwd">false</span>)
    <span class="SLgc">else</span>:
        <span class="SItr">@assert</span>(<span class="SKwd">true</span>)
}</span></div>

<h2 id="_051_loop_swg">Loop</h2><p><span class="code-inline">loop</span> is used to iterate a given number of times. It's a versatile construct in Swag, allowing for various iteration patterns. </p>
<p>The loop expression is evaluated <b>once</b> and must be a <b>positive value</b>. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> cpt = <span class="SNum">0</span>
    <span class="SLgc">loop</span> <span class="SNum">10</span>:                        <span class="SCmt">// Loops 10 times</span>
        cpt += <span class="SNum">1</span>
    <span class="SItr">@assert</span>(cpt == <span class="SNum">10</span>)
}</span></div>
<p>The compiler keyword <span class="code-inline">#index</span> returns the current index of the loop, starting at 0. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> cpt = <span class="SNum">0</span>'<span class="STpe">u64</span>
    <span class="SLgc">loop</span> <span class="SNum">5</span>
    {
        cpt += <span class="SItr">#index</span>
    }

    <span class="SItr">@assert</span>(cpt == <span class="SNum">0</span> + <span class="SNum">1</span> + <span class="SNum">2</span> + <span class="SNum">3</span> + <span class="SNum">4</span>)
}</span></div>
<p>You can name the loop index if you want. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> cpt  = <span class="SNum">0</span>
    <span class="SKwd">var</span> cpt1 = <span class="SNum">0</span>

    <span class="SLgc">loop</span> i <span class="SLgc">in</span> <span class="SNum">5</span>                     <span class="SCmt">// index is named 'i'</span>
    {
        cpt += i
        cpt1 += <span class="SItr">#index</span>              <span class="SCmt">// #index is still available, even when named</span>
    }

    <span class="SItr">@assert</span>(cpt == <span class="SNum">0</span> + <span class="SNum">1</span> + <span class="SNum">2</span> + <span class="SNum">3</span> + <span class="SNum">4</span>)
    <span class="SItr">@assert</span>(cpt1 == cpt)
}</span></div>
<p><span class="code-inline">loop</span> can be used on any type that supports the <span class="code-inline">@countof</span> intrinsic, such as slices, arrays, and strings. We'll explore using it with structs later. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> arr = [<span class="SNum">10</span>, <span class="SNum">20</span>, <span class="SNum">30</span>, <span class="SNum">40</span>]
    <span class="SCmp">#assert</span> <span class="SItr">@countof</span>(arr) == <span class="SNum">4</span>

    <span class="SKwd">var</span> cpt = <span class="SNum">0</span>
    <span class="SLgc">loop</span> arr:                       <span class="SCmt">// The array contains 4 elements, so the loop count is 4</span>
        cpt += arr[<span class="SItr">#index</span>]
    <span class="SItr">@assert</span>(cpt == <span class="SNum">10</span> + <span class="SNum">20</span> + <span class="SNum">30</span> + <span class="SNum">40</span>)
}</span></div>
<div class="blockquote blockquote-warning">
<div class="blockquote-title-block"><i class="fa fa-exclamation-triangle"></i>  <span class="blockquote-title">Warning</span></div><p> On a string, it will loop over each byte, <b>not</b> runes (if a rune is encoded in more than one byte). To iterate over runes, use the Std.Core module. </p>
</div>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> cpt = <span class="SNum">0</span>
    <span class="SLgc">loop</span> <span class="SStr">"⻘"</span>:
        cpt += <span class="SNum">1</span>

    <span class="SCmt">// cpt is equal to 3 because '⻘' is encoded with 3 bytes</span>
    <span class="SItr">@assert</span>(cpt == <span class="SNum">3</span>)
}</span></div>
<p>You can loop in reverse order by adding the <span class="code-inline">back</span> modifier after the <span class="code-inline">loop</span>. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> cpt = <span class="SNum">0</span>

    <span class="SCmt">// Index will be 2, 1, then 0.</span>
    <span class="SLgc">loop</span> <span class="SKwd">#back</span> <span class="SNum">3</span>
    {
        <span class="SLgc">if</span> cpt == <span class="SNum">0</span>:
            <span class="SItr">@assert</span>(<span class="SItr">#index</span> == <span class="SNum">2</span>)
        <span class="SLgc">elif</span> cpt == <span class="SNum">1</span>:
            <span class="SItr">@assert</span>(<span class="SItr">#index</span> == <span class="SNum">1</span>)
        <span class="SLgc">elif</span> cpt == <span class="SNum">2</span>:
            <span class="SItr">@assert</span>(<span class="SItr">#index</span> == <span class="SNum">0</span>)

        cpt += <span class="SNum">1</span>
    }
}</span></div>
<h3 id="_051_loop_swg_break,_continue">break, continue </h3>
<p><span class="code-inline">break</span> and <span class="code-inline">continue</span> can be used inside a loop. </p>
<p>Exit a loop early with <span class="code-inline">break</span>. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> cpt = <span class="SNum">0</span>
    <span class="SLgc">loop</span> x <span class="SLgc">in</span> <span class="SNum">10</span>
    {
        <span class="SLgc">if</span> x == <span class="SNum">5</span>:
            <span class="SLgc">break</span>
        cpt += <span class="SNum">1</span>
    }

    <span class="SItr">@assert</span>(cpt == <span class="SNum">5</span>)
}</span></div>
<p>Use <span class="code-inline">continue</span> to skip the remainder of the loop body and return to the loop evaluation. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> cpt = <span class="SNum">0</span>
    <span class="SLgc">loop</span> x <span class="SLgc">in</span> <span class="SNum">10</span>
    {
        <span class="SLgc">if</span> x == <span class="SNum">5</span>:
            <span class="SLgc">continue</span>              <span class="SCmt">// Skip iteration when x is 5</span>
        cpt += <span class="SNum">1</span>
    }

    <span class="SItr">@assert</span>(cpt == <span class="SNum">9</span>)
}</span></div>
<h3 id="_051_loop_swg_Ranges">Ranges </h3>
<p><span class="code-inline">loop</span> can also iterate over a <b>range</b> of signed values. </p>
<p>Use <span class="code-inline">to</span> to loop from a given value <i>to</i> another value. The first value must be lower than or equal to the upper bound. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> count = <span class="SNum">0</span>
    <span class="SKwd">var</span> sum   = <span class="SNum">0</span>
    <span class="SLgc">loop</span> i <span class="SLgc">in</span> -<span class="SNum">1</span> <span class="SLgc">to</span> <span class="SNum">1</span>              <span class="SCmt">// loop from -1 to 1, inclusive</span>
    {
        count += <span class="SNum">1</span>
        sum += i
    }

    <span class="SItr">@assert</span>(sum == <span class="SNum">0</span>)
    <span class="SItr">@assert</span>(count == <span class="SNum">3</span>)
}</span></div>
<p>Exclude the last value with <span class="code-inline">until</span> instead of <span class="code-inline">to</span>. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SCmt">// Loop from 1 to 2, excluding 3</span>
    <span class="SKwd">var</span> cpt = <span class="SNum">0</span>
    <span class="SLgc">loop</span> i <span class="SLgc">in</span> <span class="SNum">1</span> <span class="SLgc">until</span> <span class="SNum">3</span>
    {
        cpt += i
    }

    <span class="SItr">@assert</span>(cpt == <span class="SNum">1</span> + <span class="SNum">2</span>)
}</span></div>
<p>With a range, you can also loop in reverse order by adding the <span class="code-inline">back</span> modifier. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SCmt">// Loop from 5 to 0</span>
    <span class="SLgc">loop</span> <span class="SKwd">#back</span> <span class="SNum">0</span> <span class="SLgc">to</span> <span class="SNum">5</span>
    {
    }

    <span class="SCmt">// Loop from 1 to -1</span>
    <span class="SLgc">loop</span> <span class="SKwd">#back</span> -<span class="SNum">1</span> <span class="SLgc">to</span> <span class="SNum">1</span>
    {
    }

    <span class="SCmt">// Loop from 1 to -2, excluding the upper limit.</span>
    <span class="SLgc">loop</span> <span class="SKwd">#back</span> -<span class="SNum">2</span> <span class="SLgc">until</span> <span class="SNum">2</span>
    {
    }
}</span></div>
<h3 id="_051_loop_swg_Infinite_loop">Infinite loop </h3>
<p>A loop without an expression but with a block is infinite, equivalent to <span class="code-inline">while true {}</span>. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SLgc">loop</span>
    {
        <span class="SLgc">if</span> <span class="SItr">#index</span> == <span class="SNum">4</span>:            <span class="SCmt">// #index is still valid, but cannot be renamed</span>
            <span class="SLgc">break</span>
    }
}</span></div>
<h3 id="_051_loop_swg_where">where </h3>
<p>The <span class="code-inline">where</span> clause allows you to filter loop iterations based on a condition. When using <span class="code-inline">where</span>, the loop will only execute for indexes or values that satisfy the condition specified in the <span class="code-inline">where</span> clause. This provides a convenient way to add additional logic to your loops, ensuring that only certain iterations are executed. </p>
<p>The <span class="code-inline">where</span> clause can be added directly after the <span class="code-inline">loop</span> statement. This clause applies a condition to the loop's index or value, and only those that meet the condition will be processed. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> result = <span class="SNum">0</span>

    <span class="SCmt">// The loop will only visit 'even' indexes from 0 to 9.</span>
    <span class="SLgc">loop</span> i <span class="SLgc">in</span> <span class="SNum">10</span> <span class="SLgc">where</span> i % <span class="SNum">2</span> == <span class="SNum">0</span>
    {
        result += i
    }

    <span class="SItr">@assert</span>(result == <span class="SNum">0</span> + <span class="SNum">2</span> + <span class="SNum">4</span> + <span class="SNum">6</span> + <span class="SNum">8</span>)  <span class="SCmt">// Result is the sum of even numbers within the range</span>
}</span></div>
<p>When looping over arrays, the <span class="code-inline">where</span> clause can filter elements based on their value or index. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> arr = [<span class="SNum">10</span>, <span class="SNum">21</span>, <span class="SNum">30</span>, <span class="SNum">41</span>, <span class="SNum">50</span>]
    <span class="SKwd">var</span> sumOfEvens = <span class="SNum">0</span>

    <span class="SCmt">// This loop sums up only the even numbers in the array.</span>
    <span class="SLgc">loop</span> i <span class="SLgc">in</span> arr <span class="SLgc">where</span> arr[i] % <span class="SNum">2</span> == <span class="SNum">0</span>
    {
        sumOfEvens += arr[i]
    }

    <span class="SItr">@assert</span>(sumOfEvens == <span class="SNum">10</span> + <span class="SNum">30</span> + <span class="SNum">50</span>)  <span class="SCmt">// Only the even numbers (10, 30, 50) are summed</span>
}</span></div>
<p>You can use the <span class="code-inline">where</span> clause to create complex filtering conditions. For example, you can combine multiple logical expressions to refine which elements the loop should process. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> arr = [<span class="SNum">10</span>, <span class="SNum">15</span>, <span class="SNum">20</span>, <span class="SNum">25</span>, <span class="SNum">30</span>, <span class="SNum">35</span>]
    <span class="SKwd">var</span> filteredSum = <span class="SNum">0</span>

    <span class="SCmt">// Sum only even numbers greater than 15.</span>
    <span class="SLgc">loop</span> i <span class="SLgc">in</span> arr <span class="SLgc">where</span> arr[i] % <span class="SNum">2</span> == <span class="SNum">0</span> <span class="SLgc">and</span> arr[i] &gt; <span class="SNum">15</span>
    {
        filteredSum += arr[i]
    }

    <span class="SItr">@assert</span>(filteredSum == <span class="SNum">20</span> + <span class="SNum">30</span>)  <span class="SCmt">// Only 20 and 30 meet the condition</span>
}</span></div>
<p>The <span class="code-inline">where</span> clause can also be applied to loops over ranges, allowing for precise control over which range values are included in the loop. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> sumOfPositiveEvens = <span class="SNum">0</span>

    <span class="SCmt">// Loop over the range from -5 to 5, but only include positive even numbers.</span>
    <span class="SLgc">loop</span> i <span class="SLgc">in</span> -<span class="SNum">5</span> <span class="SLgc">to</span> <span class="SNum">5</span> <span class="SLgc">where</span> i &gt; <span class="SNum">0</span> <span class="SLgc">and</span> i % <span class="SNum">2</span> == <span class="SNum">0</span>
    {
        sumOfPositiveEvens += i
    }

    <span class="SItr">@assert</span>(sumOfPositiveEvens == <span class="SNum">2</span> + <span class="SNum">4</span>)  <span class="SCmt">// Only positive even numbers (2, 4) are summed</span>
}</span></div>
<p>You can combine the <span class="code-inline">back</span> modifier with the <span class="code-inline">where</span> clause to filter values while iterating in reverse. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> arr = [<span class="SNum">10</span>, <span class="SNum">20</span>, <span class="SNum">30</span>, <span class="SNum">40</span>, <span class="SNum">50</span>]
    <span class="SKwd">var</span> reversedSum = <span class="SNum">0</span>

    <span class="SCmt">// Loop through the array in reverse, summing only the even values.</span>
    <span class="SLgc">loop</span> <span class="SKwd">#back</span> i <span class="SLgc">in</span> arr <span class="SLgc">where</span> arr[i] % <span class="SNum">2</span> == <span class="SNum">0</span>
    {
        reversedSum += arr[i]
    }

    <span class="SItr">@assert</span>(reversedSum == <span class="SNum">50</span> + <span class="SNum">40</span> + <span class="SNum">30</span> + <span class="SNum">20</span> + <span class="SNum">10</span>)  <span class="SCmt">// Sums all even values in reverse order</span>
}</span></div>
<p>The <span class="code-inline">where</span> clause supports complex logical expressions, enabling intricate filtering criteria directly within the loop. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> arr = [<span class="SNum">10</span>, <span class="SNum">25</span>, <span class="SNum">30</span>, <span class="SNum">45</span>, <span class="SNum">50</span>, <span class="SNum">65</span>]
    <span class="SKwd">var</span> complexSum = <span class="SNum">0</span>

    <span class="SCmt">// Sum elements that are either even or greater than 40.</span>
    <span class="SLgc">loop</span> i <span class="SLgc">in</span> arr <span class="SLgc">where</span> arr[i] % <span class="SNum">2</span> == <span class="SNum">0</span> <span class="SLgc">or</span> arr[i] &gt; <span class="SNum">40</span>
    {
        complexSum += arr[i]
    }

    <span class="SItr">@assert</span>(complexSum == <span class="SNum">10</span> + <span class="SNum">30</span> + <span class="SNum">45</span> + <span class="SNum">50</span> + <span class="SNum">65</span>)  <span class="SCmt">// Values matching the complex condition are summed</span>
}</span></div>
<p>The <span class="code-inline">where</span> clause is a powerful feature in Swag that enhances the flexibility of loops by allowing you to add conditional logic directly within the loop construct. This allows for cleaner, more readable code by avoiding the need for additional <span class="code-inline">if</span> statements inside the loop body. </p>

<h2 id="_052_visit_swg">Visit</h2><p><span class="code-inline">visit</span> is used to iterate over all elements of a collection. It provides a simple and efficient way to process each item in the collection, whether it's an array, slice, or string. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SCmt">// Here we visit every byte of the string.</span>
    <span class="SCmt">// At each iteration, the byte will be stored in the variable 'value'.</span>
    <span class="SLgc">visit</span> value <span class="SLgc">in</span> <span class="SStr">"ABC"</span>
    {
        <span class="SCmt">// '#index' is also available. It stores the loop index.</span>
        <span class="SKwd">let</span> a = <span class="SItr">#index</span>
        <span class="SLgc">switch</span> a
        {
        <span class="SLgc">case</span> <span class="SNum">0</span>:
            <span class="SItr">@assert</span>(value == <span class="SStr">`A`</span>)
        <span class="SLgc">case</span> <span class="SNum">1</span>:
            <span class="SItr">@assert</span>(value == <span class="SStr">`B`</span>)
        <span class="SLgc">case</span> <span class="SNum">2</span>:
            <span class="SItr">@assert</span>(value == <span class="SStr">`C`</span>)
        }
    }
}</span></div>
<p>You can name both the <b>value</b> and the loop <b>index</b>, in that order. This allows for more readable code, especially when working with nested loops or complex data structures. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SLgc">visit</span> value, index <span class="SLgc">in</span> <span class="SStr">"ABC"</span>
    {
        <span class="SKwd">let</span> a = index
        <span class="SLgc">switch</span> a
        {
        <span class="SLgc">case</span> <span class="SNum">0</span>:
            <span class="SItr">@assert</span>(value == <span class="SStr">`A`</span>)
        <span class="SLgc">case</span> <span class="SNum">1</span>:
            <span class="SItr">@assert</span>(value == <span class="SStr">`B`</span>)
        <span class="SLgc">case</span> <span class="SNum">2</span>:
            <span class="SItr">@assert</span>(value == <span class="SStr">`C`</span>)
        }
    }
}</span></div>
<p>Both names are optional. If you don't specify names for the value and index, you can use the default aliases <span class="code-inline">#alias0</span> for the value and <span class="code-inline">#alias1</span> for the index. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SLgc">visit</span> <span class="SStr">"ABC"</span>
    {
        <span class="SKwd">let</span> a = <span class="SItr">#alias1</span> <span class="SCmt">// This is the index.</span>
        <span class="SItr">@assert</span>(a == <span class="SItr">#index</span>)
        <span class="SLgc">switch</span> a
        {
        <span class="SLgc">case</span> <span class="SNum">0</span>:
            <span class="SItr">@assert</span>(<span class="SItr">#alias0</span> == <span class="SStr">`A`</span>)
        <span class="SLgc">case</span> <span class="SNum">1</span>:
            <span class="SItr">@assert</span>(<span class="SItr">#alias0</span> == <span class="SStr">`B`</span>)
        <span class="SLgc">case</span> <span class="SNum">2</span>:
            <span class="SItr">@assert</span>(<span class="SItr">#alias0</span> == <span class="SStr">`C`</span>)
        }
    }
}</span></div>
<p>You can visit elements in reverse order by adding the <span class="code-inline">back</span> modifier. This is useful when you need to process a collection from end to start. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SCmt">// Here we visit every byte of the string in reverse order.</span>
    <span class="SKwd">var</span> cpt = <span class="SNum">0</span>
    <span class="SLgc">visit</span> <span class="SKwd">#back</span> value <span class="SLgc">in</span> <span class="SStr">"ABC"</span>
    {
        <span class="SCmt">// '#index' is also available. It stores the loop index.</span>
        <span class="SLgc">switch</span> cpt
        {
        <span class="SLgc">case</span> <span class="SNum">0</span>:
            <span class="SItr">@assert</span>(value == <span class="SStr">`C`</span>)
            <span class="SItr">@assert</span>(<span class="SItr">#index</span> == <span class="SNum">2</span>)
        <span class="SLgc">case</span> <span class="SNum">1</span>:
            <span class="SItr">@assert</span>(value == <span class="SStr">`B`</span>)
            <span class="SItr">@assert</span>(<span class="SItr">#index</span> == <span class="SNum">1</span>)
        <span class="SLgc">case</span> <span class="SNum">2</span>:
            <span class="SItr">@assert</span>(value == <span class="SStr">`A`</span>)
            <span class="SItr">@assert</span>(<span class="SItr">#index</span> == <span class="SNum">0</span>)
        }

        cpt += <span class="SNum">1</span>
    }
}</span></div>
<p>You can visit arrays or slices, which allows you to easily process each element. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> array = [<span class="SNum">10</span>, <span class="SNum">20</span>, <span class="SNum">30</span>]

    <span class="SKwd">var</span> result = <span class="SNum">0</span>
    <span class="SLgc">visit</span> it <span class="SLgc">in</span> array:
        result += it

    <span class="SItr">@assert</span>(result == <span class="SNum">10</span> + <span class="SNum">20</span> + <span class="SNum">30</span>)
}</span></div>
<p><span class="code-inline">visit</span> also works for multi-dimensional arrays, making it easy to process complex data structures. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> array: [<span class="SNum">2</span>, <span class="SNum">2</span>] <span class="STpe">s32</span> = [[<span class="SNum">10</span>, <span class="SNum">20</span>], [<span class="SNum">30</span>, <span class="SNum">40</span>]]

    <span class="SKwd">var</span> result = <span class="SNum">0</span>
    <span class="SLgc">visit</span> it <span class="SLgc">in</span> array:
        result += it

    <span class="SItr">@assert</span>(result == <span class="SNum">10</span> + <span class="SNum">20</span> + <span class="SNum">30</span> + <span class="SNum">40</span>)
}</span></div>
<p>You can visit elements using a pointer to the value by adding <span class="code-inline">&</span> before the value name. This allows you to modify the elements in place. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> array: [<span class="SNum">2</span>, <span class="SNum">2</span>] <span class="STpe">s32</span> = [[<span class="SNum">1</span>, <span class="SNum">2</span>], [<span class="SNum">3</span>, <span class="SNum">4</span>]]

    <span class="SKwd">var</span> result = <span class="SNum">0</span>
    <span class="SLgc">visit</span> &it <span class="SLgc">in</span> array
    {
        result += <span class="SKwd">dref</span> it
        <span class="SKwd">dref</span> it = <span class="SNum">555</span>
    }

    <span class="SItr">@assert</span>(result == <span class="SNum">1</span> + <span class="SNum">2</span> + <span class="SNum">3</span> + <span class="SNum">4</span>)

    <span class="SItr">@assert</span>(array[<span class="SNum">0</span>, <span class="SNum">0</span>] == <span class="SNum">555</span>)
    <span class="SItr">@assert</span>(array[<span class="SNum">0</span>, <span class="SNum">1</span>] == <span class="SNum">555</span>)
    <span class="SItr">@assert</span>(array[<span class="SNum">1</span>, <span class="SNum">0</span>] == <span class="SNum">555</span>)
    <span class="SItr">@assert</span>(array[<span class="SNum">1</span>, <span class="SNum">1</span>] == <span class="SNum">555</span>)
}</span></div>
<h3 id="_052_visit_swg_where">where </h3>
<p>You can add a <span class="code-inline">where</span> clause after the <span class="code-inline">visit</span> to filter the elements you want to process. This is useful for applying conditions directly within the loop. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> array: [] <span class="STpe">s32</span> = [<span class="SNum">1</span>, <span class="SNum">2</span>, <span class="SNum">3</span>, <span class="SNum">4</span>]
    <span class="SKwd">var</span> result = <span class="SNum">0</span>

    <span class="SCmt">// Just process even values.</span>
    <span class="SLgc">visit</span> value <span class="SLgc">in</span> array <span class="SLgc">where</span> value & <span class="SNum">1</span> == <span class="SNum">0</span>:
        result += value

    <span class="SItr">@assert</span>(result == <span class="SNum">6</span>)

    <span class="SCmt">// Equivalent using an if statement inside the visit loop:</span>
    result = <span class="SNum">0</span>
    <span class="SLgc">visit</span> value <span class="SLgc">in</span> array:
        <span class="SLgc">if</span> value & <span class="SNum">1</span> == <span class="SNum">0</span>:
            result += value  
    <span class="SItr">@assert</span>(result == <span class="SNum">6</span>) 

    <span class="SCmt">// Equivalent using continue to skip odd values:</span>
    result = <span class="SNum">0</span>
    <span class="SLgc">visit</span> value <span class="SLgc">in</span> array
    {
        <span class="SLgc">if</span> (value & <span class="SNum">1</span>) != <span class="SNum">0</span>:
            <span class="SLgc">continue</span>
        result += value        
    }
    <span class="SItr">@assert</span>(result == <span class="SNum">6</span>)
}</span></div>

<h2 id="_053_for_swg">For</h2><h3 id="_053_for_swg_Introduction_to_`for`_Loops">Introduction to <span class="code-inline">for</span> Loops </h3>
<p><span class="code-inline">for</span> loops in Swag follow the traditional structure seen in C/C++: a <i>start statement</i>, a <i>test expression</i>, and an <i>ending statement</i>. This structure provides a versatile way to iterate over a range of values with fine control over the loop's behavior. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> cpt = <span class="SNum">0</span>
    <span class="SCmt">// Standard 'for' loop with initialization, condition, and increment.</span>
    <span class="SLgc">for</span> <span class="SKwd">var</span> i = <span class="SNum">0</span>; i &lt; <span class="SNum">10</span>; i += <span class="SNum">1</span>;:
        cpt += <span class="SNum">1</span>
    <span class="SItr">@assert</span>(cpt == <span class="SNum">10</span>)

    <span class="SCmt">// Semicolons ';' can be replaced by newlines for cleaner syntax.</span>
    <span class="SLgc">for</span> <span class="SKwd">var</span> i = <span class="SNum">0</span>
        i &lt; <span class="SNum">10</span>
        i += <span class="SNum">1</span>
    {
        cpt += <span class="SNum">1</span>
    }
    <span class="SItr">@assert</span>(cpt == <span class="SNum">20</span>)

    <span class="SCmt">// Instead of a single expression, you can use a statement block for the initialization or increment.</span>
    <span class="SLgc">for</span> { <span class="SKwd">var</span> i = <span class="SNum">0</span>; cpt = <span class="SNum">0</span>; }
        i &lt; <span class="SNum">10</span>
        i += <span class="SNum">1</span>
    {
        cpt += <span class="SNum">1</span>
    }
    <span class="SItr">@assert</span>(cpt == <span class="SNum">10</span>)

    <span class="SLgc">for</span> { <span class="SKwd">var</span> i = <span class="SNum">0</span>; cpt = <span class="SNum">0</span>; }
        i &lt; <span class="SNum">10</span>
        { i += <span class="SNum">2</span>; i -= <span class="SNum">1</span>; }
    {
        cpt += <span class="SNum">1</span>
    }
    <span class="SItr">@assert</span>(cpt == <span class="SNum">10</span>)
}</span></div>
<h3 id="_053_for_swg_Accessing_Loop_Index_with_`#index`">Accessing Loop Index with <span class="code-inline">#index</span> </h3>
<p>Like <span class="code-inline">loop</span>, <span class="code-inline">visit</span>, and <span class="code-inline">while</span>, the <span class="code-inline">for</span> loop gives you access to <span class="code-inline">#index</span>, which represents the <b>current loop index</b>. This can be useful when you need to keep track of the iteration count separately from the loop variable. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> cpt = <span class="SNum">0</span>'<span class="STpe">u64</span>
    <span class="SCmt">// Use of `#index` to accumulate the loop indices.</span>
    <span class="SLgc">for</span> <span class="SKwd">var</span> i: <span class="STpe">u32</span> = <span class="SNum">10</span>; i &lt; <span class="SNum">15</span>; i += <span class="SNum">1</span>:
        cpt += <span class="SItr">#index</span>
    <span class="SItr">@assert</span>(cpt == <span class="SNum">0</span>+<span class="SNum">1</span>+<span class="SNum">2</span>+<span class="SNum">3</span>+<span class="SNum">4</span>)

    <span class="SKwd">var</span> cpt1 = <span class="SNum">0</span>'<span class="STpe">u64</span>
    <span class="SLgc">for</span> <span class="SKwd">var</span> i = <span class="SNum">10</span>; i &lt; <span class="SNum">15</span>; i += <span class="SNum">1</span>:
        cpt1 += <span class="SItr">#index</span>
    <span class="SItr">@assert</span>(cpt1 == <span class="SNum">0</span>+<span class="SNum">1</span>+<span class="SNum">2</span>+<span class="SNum">3</span>+<span class="SNum">4</span>)
}</span></div>
<h3 id="_053_for_swg_Using_`break`_and_`continue`_in_`for`_Loops">Using <span class="code-inline">break</span> and <span class="code-inline">continue</span> in <span class="code-inline">for</span> Loops </h3>
<p>As with other loops, you can use <span class="code-inline">break</span> to exit a <span class="code-inline">for</span> loop prematurely and <span class="code-inline">continue</span> to skip the rest of the current iteration and move on to the next iteration. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> sum = <span class="SNum">0</span>
    <span class="SLgc">for</span> <span class="SKwd">var</span> i = <span class="SNum">0</span>; i &lt; <span class="SNum">10</span>; i += <span class="SNum">1</span>
    {
        <span class="SLgc">if</span> i == <span class="SNum">5</span>:
            <span class="SLgc">break</span>  <span class="SCmt">// Exit the loop when 'i' equals 5</span>
        sum += i
    }
    <span class="SItr">@assert</span>(sum == <span class="SNum">0</span>+<span class="SNum">1</span>+<span class="SNum">2</span>+<span class="SNum">3</span>+<span class="SNum">4</span>)  <span class="SCmt">// Sum is 10</span>

    sum = <span class="SNum">0</span>
    <span class="SLgc">for</span> <span class="SKwd">var</span> i = <span class="SNum">0</span>; i &lt; <span class="SNum">10</span>; i += <span class="SNum">1</span>
    {
        <span class="SLgc">if</span> i % <span class="SNum">2</span> == <span class="SNum">0</span>:
            <span class="SLgc">continue</span>  <span class="SCmt">// Skip even numbers</span>
        sum += i
    }
    <span class="SItr">@assert</span>(sum == <span class="SNum">1</span>+<span class="SNum">3</span>+<span class="SNum">5</span>+<span class="SNum">7</span>+<span class="SNum">9</span>)  <span class="SCmt">// Sum is 25</span>
}</span></div>
<h3 id="_053_for_swg_Nested_`for`_Loops">Nested <span class="code-inline">for</span> Loops </h3>
<p><span class="code-inline">for</span> loops can be nested, allowing for more complex iteration patterns. The <span class="code-inline">#index</span> within a nested loop corresponds to the innermost loop. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> result = <span class="SNum">0</span>'<span class="STpe">u64</span>
    
    <span class="SCmt">// Outer loop</span>
    <span class="SLgc">for</span> <span class="SKwd">var</span> i = <span class="SNum">0</span>; i &lt; <span class="SNum">5</span>; i += <span class="SNum">1</span>
    {
        <span class="SCmt">// Inner loop</span>
        <span class="SLgc">for</span> <span class="SKwd">var</span> j = <span class="SNum">0</span>; j &lt; <span class="SNum">5</span>; j += <span class="SNum">1</span>
        {
            result += <span class="SItr">#index</span>  <span class="SCmt">// Adds the index of the inner loop</span>
        }
    }
    <span class="SItr">@assert</span>(result == <span class="SNum">10</span> * <span class="SNum">5</span>)  <span class="SCmt">// Each inner loop runs 5 times, so the sum of indices (0+1+2+3+4) * 5 = 10*5</span>
}</span></div>
<h3 id="_053_for_swg_Iterating_Over_Arrays_with_`for`">Iterating Over Arrays with <span class="code-inline">for</span> </h3>
<p>The <span class="code-inline">for</span> loop can also be used to iterate over elements of an array or other iterable collections. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> array = [<span class="SNum">1</span>, <span class="SNum">2</span>, <span class="SNum">3</span>, <span class="SNum">4</span>, <span class="SNum">5</span>]
    <span class="SKwd">var</span> sum = <span class="SNum">0</span>

    <span class="SLgc">for</span> <span class="SKwd">var</span> i = <span class="SNum">0</span>; i &lt; <span class="SItr">@countof</span>(array); i += <span class="SNum">1</span>
    {
        sum += array[i]
    }
    <span class="SItr">@assert</span>(sum == <span class="SNum">1</span>+<span class="SNum">2</span>+<span class="SNum">3</span>+<span class="SNum">4</span>+<span class="SNum">5</span>)  <span class="SCmt">// Sum is 15</span>
}</span></div>

<h2 id="_054_while_swg">While</h2><h3 id="_054_while_swg_Introduction_to_`while`_Loops">Introduction to <span class="code-inline">while</span> Loops </h3>
<p><span class="code-inline">while</span> is a loop that continues to execute as long as the specified expression evaluates to <span class="code-inline">true</span>. The loop stops running when the expression becomes <span class="code-inline">false</span>. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> i = <span class="SNum">0</span>
    <span class="SLgc">while</span> i &lt; <span class="SNum">10</span>:        <span class="SCmt">// Loop runs until 'i' is no longer less than 10</span>
        i += <span class="SNum">1</span>
    <span class="SItr">@assert</span>(i == <span class="SNum">10</span>)     <span class="SCmt">// Asserts that 'i' is 10 after the loop exits</span>
}</span></div>
<h3 id="_054_while_swg_Breaking_Out_of_a_`while`_Loop">Breaking Out of a <span class="code-inline">while</span> Loop </h3>
<p>You can use <span class="code-inline">break</span> to exit a <span class="code-inline">while</span> loop prematurely. This is useful when you want to stop the loop based on a certain condition before the loop's natural termination. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> i = <span class="SNum">0</span>
    <span class="SLgc">while</span> i &lt; <span class="SNum">10</span>
    {
        <span class="SLgc">if</span> i == <span class="SNum">5</span>:
            <span class="SLgc">break</span>        <span class="SCmt">// Exit the loop when 'i' equals 5</span>
        i += <span class="SNum">1</span>
    }

    <span class="SItr">@assert</span>(i == <span class="SNum">5</span>)       <span class="SCmt">// Asserts that 'i' equals 5 after breaking out of the loop</span>
}</span></div>
<h3 id="_054_while_swg_Skipping_Iterations_with_`continue`">Skipping Iterations with <span class="code-inline">continue</span> </h3>
<p><span class="code-inline">continue</span> can be used to skip the rest of the current iteration and immediately proceed to the next iteration of the loop. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> sum = <span class="SNum">0</span>
    <span class="SKwd">var</span> i = <span class="SNum">0</span>
    <span class="SLgc">while</span> i &lt; <span class="SNum">10</span>
    {
        i += <span class="SNum">1</span>
        <span class="SLgc">if</span> i % <span class="SNum">2</span> == <span class="SNum">0</span>:
            <span class="SLgc">continue</span>      <span class="SCmt">// Skip the even numbers</span>
        sum += i          <span class="SCmt">// Add only odd numbers to 'sum'</span>
    }

    <span class="SItr">@assert</span>(sum == <span class="SNum">25</span>)    <span class="SCmt">// Asserts that the sum of odd numbers from 1 to 9 is 25</span>
}</span></div>
<h3 id="_054_while_swg_Nested_`while`_Loops">Nested <span class="code-inline">while</span> Loops </h3>
<p><span class="code-inline">while</span> loops can be nested within each other. The <span class="code-inline">break</span> and <span class="code-inline">continue</span> statements only affect the innermost loop in which they are placed. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> i = <span class="SNum">0</span>
    <span class="SKwd">var</span> j = <span class="SNum">0</span>
    <span class="SKwd">var</span> count = <span class="SNum">0</span>

    <span class="SLgc">while</span> i &lt; <span class="SNum">3</span>
    {
        j = <span class="SNum">0</span>
        <span class="SLgc">while</span> j &lt; <span class="SNum">3</span>
        {
            <span class="SLgc">if</span> j == <span class="SNum">2</span>:
                <span class="SLgc">break</span>      <span class="SCmt">// Breaks out of the inner loop when 'j' equals 2</span>
            count += <span class="SNum">1</span>
            j += <span class="SNum">1</span>
        }
        i += <span class="SNum">1</span>
    }

    <span class="SItr">@assert</span>(count == <span class="SNum">6</span>)    <span class="SCmt">// Asserts that the inner loop ran 6 times (2 times for each of the 3 iterations of the outer loop)</span>
}</span></div>
<h3 id="_054_while_swg_Using_`while`_with_Complex_Conditions">Using <span class="code-inline">while</span> with Complex Conditions </h3>
<p>The <span class="code-inline">while</span> loop condition can involve complex logical expressions, allowing for intricate control flow. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> a = <span class="SNum">0</span>
    <span class="SKwd">var</span> b = <span class="SNum">1</span>
    <span class="SKwd">var</span> iterations = <span class="SNum">0</span>

    <span class="SLgc">while</span> a &lt; <span class="SNum">100</span> <span class="SLgc">and</span> b &lt; <span class="SNum">200</span>
    {
        a += <span class="SNum">10</span>
        b += <span class="SNum">20</span>
        iterations += <span class="SNum">1</span>
    }

    <span class="SItr">@assert</span>(a == <span class="SNum">100</span>)         <span class="SCmt">// Asserts that 'a' reached 100</span>
    <span class="SItr">@assert</span>(b == <span class="SNum">201</span>)         <span class="SCmt">// Asserts that 'b' reached 201</span>
    <span class="SItr">@assert</span>(iterations == <span class="SNum">10</span>) <span class="SCmt">// Asserts that the loop ran 5 times</span>
}</span></div>

<h2 id="_055_switch_swg">Switch</h2><div class="code-block"><span class="SCde"><span class="SCmp">#global</span> skipfmt</span></div>
<p>The <span class="code-inline">switch</span> statement in this language is similar to the one in C/C++, with a notable difference: there's no need for an explicit <span class="code-inline">break</span> statement at the end of each <span class="code-inline">case</span> block unless the <span class="code-inline">case</span> is empty. This prevents unintentional <span class="code-inline">fallthrough</span> behavior, where execution would continue from one <span class="code-inline">case</span> to the next without stopping. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">let</span> value = <span class="SNum">6</span>

    <span class="SCmt">// The switch statement here checks the value of 'value'.</span>
    <span class="SCmt">// Depending on the value, it executes the corresponding case.</span>
    <span class="SLgc">switch</span> value
    {
    <span class="SLgc">case</span> <span class="SNum">0</span>:  
        <span class="SItr">@assert</span>(<span class="SKwd">false</span>)  <span class="SCmt">// If value is 0, this assertion fails (test fails).</span>
    <span class="SLgc">case</span> <span class="SNum">5</span>:  
        <span class="SItr">@assert</span>(<span class="SKwd">false</span>)  <span class="SCmt">// If value is 5, this assertion fails (test fails).</span>
    <span class="SLgc">case</span> <span class="SNum">6</span>:  
        <span class="SItr">@assert</span>(<span class="SKwd">true</span>)   <span class="SCmt">// If value is 6, this assertion passes (test succeeds).</span>
    <span class="SLgc">default</span>: 
        <span class="SItr">@assert</span>(<span class="SKwd">false</span>)  <span class="SCmt">// If value doesn't match any case, this fails (test fails).</span>
    }

    <span class="SKwd">let</span> ch = <span class="SStr">`A`</span>'<span class="STpe">rune</span>

    <span class="SCmt">// This switch checks the value of 'ch'.</span>
    <span class="SLgc">switch</span> ch
    {
    <span class="SLgc">case</span> <span class="SStr">`B`</span>:
        <span class="SItr">@assert</span>(<span class="SKwd">false</span>)  <span class="SCmt">// If ch is 'B', this fails.</span>
    <span class="SLgc">case</span> <span class="SStr">`A`</span>: 
        <span class="SLgc">break</span>           <span class="SCmt">// Exits the switch when 'ch' is 'A'.</span>
    }
}</span></div>
<p>You can assign multiple values to a single <span class="code-inline">case</span> statement, making it convenient to handle several potential matches with the same block of code. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">let</span> value = <span class="SNum">6</span>

    <span class="SCmt">// Here, case 2, 4, and 6 all trigger the same action.</span>
    <span class="SLgc">switch</span> value
    {
    <span class="SLgc">case</span> <span class="SNum">2</span>, <span class="SNum">4</span>, <span class="SNum">6</span>:   
        <span class="SItr">@assert</span>(<span class="SKwd">true</span>)   <span class="SCmt">// If value is 2, 4, or 6, this assertion passes.</span>
    <span class="SLgc">default</span>:        
        <span class="SItr">@assert</span>(<span class="SKwd">false</span>)  <span class="SCmt">// If value is anything else, this fails.</span>
    }

    <span class="SCmt">// The same concept, but with each value listed on its own line for clarity.</span>
    <span class="SLgc">switch</span> value
    {
    <span class="SLgc">case</span> <span class="SNum">2</span>,
         <span class="SNum">4</span>,
         <span class="SNum">6</span>:
        <span class="SItr">@assert</span>(<span class="SKwd">true</span>)   <span class="SCmt">// Matches any of these values.</span>
    <span class="SLgc">default</span>:
        <span class="SItr">@assert</span>(<span class="SKwd">false</span>)  <span class="SCmt">// Fails if no case matches.</span>
    }
}</span></div>
<p>The <span class="code-inline">switch</span> statement can be used with any type that supports the <span class="code-inline">==</span> operator. This includes not only numeric types but also strings, making it versatile for various comparisons. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">let</span> value = <span class="SStr">"myString"</span>

    <span class="SCmt">// Switch works with strings, too.</span>
    <span class="SLgc">switch</span> value
    {
    <span class="SLgc">case</span> <span class="SStr">"myString"</span>:    
        <span class="SItr">@assert</span>(<span class="SKwd">true</span>)   <span class="SCmt">// If value is "myString", this passes.</span>
    <span class="SLgc">case</span> <span class="SStr">"otherString"</span>: 
        <span class="SItr">@assert</span>(<span class="SKwd">false</span>)  <span class="SCmt">// If value is "otherString", this fails.</span>
    <span class="SLgc">default</span>:            
        <span class="SItr">@assert</span>(<span class="SKwd">false</span>)  <span class="SCmt">// If value doesn't match any case, this fails.</span>
    }
}</span></div>
<p>If you need behavior similar to C/C++ where one <span class="code-inline">case</span> can intentionally "fall through" to the next, you can use the <span class="code-inline">fallthrough</span> keyword. This explicitly tells the compiler to continue execution in the next <span class="code-inline">case</span> block. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">let</span> value = <span class="SNum">6</span>

    <span class="SCmt">// Demonstrates fallthrough: execution will continue from case 6 to case 7.</span>
    <span class="SLgc">switch</span> value
    {
    <span class="SLgc">case</span> <span class="SNum">6</span>:
        <span class="SLgc">fallthrough</span>     <span class="SCmt">// Fall through to the next case, even though value is still 6.</span>
    <span class="SLgc">case</span> <span class="SNum">7</span>:
        <span class="SItr">@assert</span>(value == <span class="SNum">6</span>) <span class="SCmt">// This assertion checks that value is 6, even in case 7.</span>
    <span class="SLgc">default</span>:
        <span class="SItr">@assert</span>(<span class="SKwd">true</span>)   <span class="SCmt">// Default case runs if no other cases match; true assertion keeps the test passing.</span>
    }
}</span></div>
<p>The <span class="code-inline">break</span> statement can be used to exit a <span class="code-inline">case</span> block early. This is useful if you only want to execute part of a <span class="code-inline">case</span> and then skip the rest of it. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">let</span> value = <span class="SNum">6</span>

    <span class="SCmt">// This switch demonstrates the use of break to exit early.</span>
    <span class="SLgc">switch</span> value
    {
    <span class="SLgc">case</span> <span class="SNum">6</span>:
        <span class="SLgc">if</span> value == <span class="SNum">6</span>:
            <span class="SLgc">break</span>       <span class="SCmt">// Exits the switch here if the condition is met.</span>
        <span class="SItr">@assert</span>(<span class="SKwd">false</span>)  <span class="SCmt">// This will never be reached if value is 6.</span>
    <span class="SLgc">default</span>:
        <span class="SItr">@assert</span>(<span class="SKwd">false</span>)  <span class="SCmt">// This fails if value doesn't match any case.</span>
    }
}</span></div>
<p>A <span class="code-inline">case</span> cannot be left empty. If you need a <span class="code-inline">case</span> to do nothing, use the <span class="code-inline">break</span> statement explicitly. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">let</span> value = <span class="SNum">6</span>

    <span class="SCmt">// This switch demonstrates handling cases where no action is needed.</span>
    <span class="SLgc">switch</span> value
    {
    <span class="SLgc">case</span> <span class="SNum">5</span>:     
        <span class="SItr">@assert</span>(<span class="SKwd">false</span>)  <span class="SCmt">// This fails if value is 5.</span>
    <span class="SLgc">case</span> <span class="SNum">6</span>:     
        <span class="SLgc">break</span>           <span class="SCmt">// No action taken; exits the switch cleanly.</span>
    <span class="SLgc">default</span>:    
        <span class="SItr">@assert</span>(<span class="SKwd">false</span>)  <span class="SCmt">// Fails if value doesn't match any case.</span>
    }
}</span></div>
<p>A <span class="code-inline">case</span> expression doesn't need to be a constant. You can use variables or even expressions that evaluate at runtime. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">let</span> test = <span class="SNum">2</span>
    <span class="SKwd">let</span> a = <span class="SNum">0</span>
    <span class="SKwd">let</span> b = <span class="SNum">1</span>

    <span class="SCmt">// This switch demonstrates the flexibility of using variables and expressions.</span>
    <span class="SLgc">switch</span> test
    {
    <span class="SLgc">case</span> a:     <span class="SCmt">// Here, 'a' is a variable, not a constant.</span>
        <span class="SItr">@assert</span>(<span class="SKwd">false</span>)  <span class="SCmt">// Fails if test is equal to a (which is 0).</span>
    <span class="SLgc">case</span> b:     <span class="SCmt">// 'b' is another variable.</span>
        <span class="SItr">@assert</span>(<span class="SKwd">false</span>)  <span class="SCmt">// Fails if test is equal to b (which is 1).</span>
    <span class="SLgc">case</span> b + <span class="SNum">1</span>: <span class="SCmt">// This case uses an expression.</span>
        <span class="SItr">@assert</span>(<span class="SKwd">true</span>)   <span class="SCmt">// Passes if test equals 2 (b + 1).</span>
    }
}</span></div>
<h3 id="_055_switch_swg_The_'Swag_Complete'_Attribute">The <span class="code-inline">Swag.Complete</span> Attribute </h3>
<p>The <span class="code-inline">Swag.Complete</span> attribute ensures that all possible cases in an enum are handled in the <span class="code-inline">switch</span> statement. If any enum value is not covered by a <span class="code-inline">case</span>, the compiler will raise an error, ensuring exhaustive handling of all enum variants. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">enum</span> <span class="SCst">Color</span> { <span class="SCst">Red</span>, <span class="SCst">Green</span>, <span class="SCst">Blue</span> }
    <span class="SKwd">let</span> color = <span class="SCst">Color</span>.<span class="SCst">Red</span>

    <span class="SAtr">#[Swag.Complete]</span>
    <span class="SLgc">switch</span> color
    {
    <span class="SLgc">case</span> <span class="SCst">Color</span>.<span class="SCst">Red</span>:     
        <span class="SLgc">break</span>           <span class="SCmt">// Handle the Red case.</span>
    <span class="SLgc">case</span> <span class="SCst">Color</span>.<span class="SCst">Green</span>:   
        <span class="SItr">@assert</span>(<span class="SKwd">false</span>)  <span class="SCmt">// Fails if color is Green.</span>
    <span class="SLgc">case</span> <span class="SCst">Color</span>.<span class="SCst">Blue</span>:    
        <span class="SItr">@assert</span>(<span class="SKwd">false</span>)  <span class="SCmt">// Fails if color is Blue.</span>
    }
}</span></div>
<h3 id="_055_switch_swg_Specific_Types">Specific Types </h3>
<p>When switching on a variable of type <span class="code-inline">any</span> or <span class="code-inline">interface</span>, the <span class="code-inline">switch</span> statement matches based on the underlying type of the variable. This behavior is similar to calling the <span class="code-inline">@kindof</span> intrinsic, which retrieves the type of the variable. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">let</span> x: <span class="STpe">any</span> = <span class="SStr">"value"</span>

    <span class="SCmt">// This switch is based on the type of 'x'.</span>
    <span class="SLgc">switch</span> x    <span class="SCmt">// Implicitly performs @kindof(x) to determine the type of 'x'.</span>
    {
    <span class="SLgc">case</span> <span class="STpe">string</span>: 
        <span class="SLgc">break</span>           <span class="SCmt">// Matches if 'x' is a string.</span>
    <span class="SLgc">default</span>:     
        <span class="SItr">@assert</span>(<span class="SKwd">false</span>)  <span class="SCmt">// Fails if 'x' is not a string.</span>
    }
}</span></div>
<h3 id="_055_switch_swg_Using_Ranges">Using Ranges </h3>
<p>A <span class="code-inline">switch</span> statement can match against a <b>range of values</b>. This is particularly useful for handling multiple values that fall within a specific range without having to list each one individually. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> success = <span class="SKwd">false</span>
    <span class="SKwd">let</span> x = <span class="SNum">6</span>

    <span class="SCmt">// This switch uses ranges to match a group of values.</span>
    <span class="SLgc">switch</span> x
    {
    <span class="SLgc">case</span> <span class="SNum">0</span> <span class="SLgc">to</span> <span class="SNum">5</span>:  
        <span class="SItr">@assert</span>(<span class="SKwd">false</span>)  <span class="SCmt">// Fails if x is between 0 and 5, inclusive.</span>
    <span class="SLgc">case</span> <span class="SNum">6</span> <span class="SLgc">to</span> <span class="SNum">15</span>: 
        success = <span class="SKwd">true</span>  <span class="SCmt">// Succeeds if x is between 6 and 15, inclusive.</span>
    }

    <span class="SItr">@assert</span>(success)   <span class="SCmt">// Ensures that the correct case was matched.</span>
}</span></div>
<p>If ranges overlap, the first valid range that matches will be used, and subsequent ranges are ignored. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> success = <span class="SKwd">false</span>
    <span class="SKwd">let</span> x = <span class="SNum">6</span>

    <span class="SCmt">// Demonstrates overlapping ranges and their precedence.</span>
    <span class="SLgc">switch</span> x
    {
    <span class="SLgc">case</span> <span class="SNum">0</span> <span class="SLgc">to</span> <span class="SNum">10</span>:    
        success = <span class="SKwd">true</span>  <span class="SCmt">// This case is matched first, and the subsequent case is ignored.</span>
    <span class="SLgc">case</span> <span class="SNum">5</span> <span class="SLgc">until</span> <span class="SNum">15</span>: 
        <span class="SItr">@assert</span>(<span class="SKwd">false</span>)  <span class="SCmt">// This is not reached because the first case matches.</span>
    }

    <span class="SItr">@assert</span>(success)   <span class="SCmt">// Confirms that the first range was correctly applied.</span>
}</span></div>
<h3 id="_055_switch_swg_The_'where'_clause">The <span class="code-inline">where</span> clause </h3>
<p>You can add a <span class="code-inline">where</span> clause to a <span class="code-inline">case</span> in a <span class="code-inline">switch</span> statement. The <span class="code-inline">where</span> clause allows you to add an additional condition that must be satisfied for that <span class="code-inline">case</span> to execute. This provides a finer level of control within your <span class="code-inline">switch</span> statements, enabling more complex decision-making based on multiple variables. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">let</span> x = <span class="SNum">6</span>
    <span class="SKwd">let</span> y = <span class="SNum">10</span>

    <span class="SCmt">// In this switch, each case not only checks the value of 'x',</span>
    <span class="SCmt">// but also applies an additional condition using 'where'.</span>
    <span class="SLgc">switch</span> x
    {
    <span class="SLgc">case</span> <span class="SNum">6</span> <span class="SLgc">where</span> y == <span class="SNum">9</span>:    
        <span class="SCmt">// This case checks if 'x' is 6 and 'y' is 9.</span>
        <span class="SCmt">// 'y' is 10 here, so this case is skipped.</span>
        <span class="SItr">@assert</span>(<span class="SKwd">false</span>)   <span class="SCmt">// This will not be executed.</span>
    <span class="SLgc">case</span> <span class="SNum">6</span> <span class="SLgc">where</span> y == <span class="SNum">10</span>:   
        <span class="SCmt">// This case checks if 'x' is 6 and 'y' is 10.</span>
        <span class="SCmt">// Both conditions are true, so this case is executed.</span>
        <span class="SItr">@assert</span>(<span class="SKwd">true</span>)    <span class="SCmt">// This assertion will pass.</span>
    <span class="SLgc">default</span>:
        <span class="SCmt">// This case is a fallback if no other cases match.</span>
        <span class="SItr">@assert</span>(<span class="SKwd">false</span>)   <span class="SCmt">// This will not be executed since the second case matches.</span>
    }
}</span></div>
<p>The <span class="code-inline">where</span> clause can also be applied to a <span class="code-inline">default</span> case, providing additional filtering even when no specific <span class="code-inline">case</span> matches. This is particularly useful when you want the <span class="code-inline">default</span> behavior to occur only under certain conditions. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">let</span> x = <span class="SNum">7</span>
    <span class="SKwd">let</span> y = <span class="SNum">10</span>

    <span class="SCmt">// This switch demonstrates how different 'where' clauses can lead to different outcomes.</span>
    <span class="SLgc">switch</span> x
    {
    <span class="SLgc">case</span> <span class="SNum">6</span> <span class="SLgc">where</span> y == <span class="SNum">10</span>: 
        <span class="SCmt">// Checks if 'x' is 6 and 'y' is 10.</span>
        <span class="SCmt">// 'x' is 7 here, so this case is skipped.</span>
        <span class="SItr">@assert</span>(<span class="SKwd">false</span>)   <span class="SCmt">// This will not be executed.</span>
    <span class="SLgc">case</span> <span class="SNum">7</span> <span class="SLgc">where</span> y == <span class="SNum">9</span>:  
        <span class="SCmt">// Checks if 'x' is 7 and 'y' is 9.</span>
        <span class="SCmt">// 'y' is 10 here, so this case is also skipped.</span>
        <span class="SItr">@assert</span>(<span class="SKwd">false</span>)   <span class="SCmt">// This will not be executed.</span>
    <span class="SLgc">default</span> <span class="SLgc">where</span> y == <span class="SNum">10</span>: 
        <span class="SCmt">// This default case includes a 'where' clause.</span>
        <span class="SCmt">// If 'x' didn't match any previous case and 'y' is 10, this case is executed.</span>
        <span class="SLgc">break</span>            <span class="SCmt">// This will execute since 'y' is 10.</span>
    <span class="SLgc">default</span>:
        <span class="SCmt">// A general fallback default case.</span>
        <span class="SItr">@assert</span>(<span class="SKwd">false</span>)   <span class="SCmt">// This will not be executed since the previous default matched.</span>
    }
}</span></div>
<h3 id="_055_switch_swg_Switch_Without_an_Expression">Switch Without an Expression </h3>
<p>If no expression is provided in the <span class="code-inline">switch</span> statement, it behaves like a series of <span class="code-inline">if/else</span> statements. Each <span class="code-inline">case</span> is evaluated in order, and the first one that evaluates to <span class="code-inline">true</span> is executed. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">let</span> value = <span class="SNum">6</span>
    <span class="SKwd">let</span> value1 = <span class="SStr">"true"</span>

    <span class="SCmt">// Demonstrates a switch statement without an expression.</span>
    <span class="SCmt">// This works like an if-else chain.</span>
    <span class="SLgc">switch</span>
    {
    <span class="SLgc">case</span> value == <span class="SNum">6</span> <span class="SLgc">or</span> value == <span class="SNum">7</span>:
        <span class="SItr">@assert</span>(<span class="SKwd">true</span>)   <span class="SCmt">// Passes if value is 6 or 7.</span>
        <span class="SLgc">fallthrough</span>     <span class="SCmt">// Continues to the next case regardless.</span>
    <span class="SLgc">case</span> value1 == <span class="SStr">"true"</span>:
        <span class="SItr">@assert</span>(<span class="SKwd">true</span>)   <span class="SCmt">// Passes if value1 is "true".</span>
    <span class="SLgc">default</span>:
        <span class="SItr">@assert</span>(<span class="SKwd">false</span>)  <span class="SCmt">// Fails if no previous case was matched.</span>
    }
}</span></div>

<h2 id="_056_break_swg">Break</h2><p>We have already seen that <span class="code-inline">break</span> is used to exit a <span class="code-inline">loop</span>, <span class="code-inline">visit</span>, <span class="code-inline">while</span>, <span class="code-inline">for</span>, and <span class="code-inline">switch</span>. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SLgc">loop</span> <span class="SNum">10</span>:
        <span class="SLgc">break</span>                 <span class="SCmt">// Exits the loop immediately</span>
    <span class="SLgc">for</span> <span class="SKwd">var</span> i = <span class="SNum">0</span>; i &lt; <span class="SNum">10</span>; i += <span class="SNum">1</span>:
        <span class="SLgc">break</span>                 <span class="SCmt">// Exits the loop immediately</span>
    <span class="SLgc">while</span> <span class="SKwd">false</span>:
        <span class="SLgc">break</span>                 <span class="SCmt">// Although this line will never be reached, it demonstrates usage</span>
}</span></div>
<p>By default, <span class="code-inline">break</span> will exit only the innermost loop or control structure. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> cpt = <span class="SNum">0</span>
    <span class="SLgc">loop</span> <span class="SNum">10</span>
    {
        <span class="SLgc">loop</span> <span class="SNum">10</span>
        {
            <span class="SLgc">break</span>             <span class="SCmt">// Exits the inner loop...</span>
        }

        <span class="SCmt">// ...and continues execution here in the outer loop</span>
        cpt += <span class="SNum">1</span>
    }

    <span class="SItr">@assert</span>(cpt == <span class="SNum">10</span>)        <span class="SCmt">// The outer loop runs 10 times</span>
}</span></div>
<p>However, you can <b>name a scope</b> using the <span class="code-inline">#scope</span> compiler keyword and then use <span class="code-inline">break</span> to exit directly to the end of that named scope. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> cpt = <span class="SNum">0</span>

    <span class="SCmt">// Define a scope named 'BigScope'</span>
    <span class="SCmp">#scope</span> <span class="SCst">BigScope</span>
    {
        <span class="SLgc">loop</span> <span class="SNum">10</span>
        {
            cpt += <span class="SNum">1</span>
            <span class="SLgc">break</span> <span class="SCst">BigScope</span>    <span class="SCmt">// Breaks out of the entire 'BigScope'</span>
        }

        <span class="SItr">@assert</span>(<span class="SKwd">false</span>)        <span class="SCmt">// This assertion is never reached</span>
    }

    <span class="SCmt">// Execution resumes here after breaking out of 'BigScope'</span>
    <span class="SItr">@assert</span>(cpt == <span class="SNum">1</span>)         <span class="SCmt">// Confirms that the loop inside 'BigScope' ran only once</span>
}</span></div>
<p>When used with a named scope, <span class="code-inline">continue</span> allows you to return to the beginning of the scope, effectively restarting the scope's execution. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> cpt = <span class="SNum">0</span>
    <span class="SCmp">#scope</span> <span class="SCst">Loop</span>
    {
        cpt += <span class="SNum">1</span>
        <span class="SLgc">if</span> cpt == <span class="SNum">5</span>:
            <span class="SLgc">break</span>            <span class="SCmt">// Exits the 'Loop' scope when `cpt` reaches 5</span>
        <span class="SLgc">continue</span>             <span class="SCmt">// Returns to the start of 'Loop'</span>
    }

    <span class="SItr">@assert</span>(cpt == <span class="SNum">5</span>)        <span class="SCmt">// Ensures the loop runs exactly 5 times</span>
}</span></div>
<p>It's not mandatory to name the scope, so this technique can be used as an alternative to a series of if/else statements, providing a clean and structured way to manage multiple conditions. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">let</span> cpt = <span class="SNum">0</span>
    <span class="SCmp">#scope</span>
    {
        <span class="SLgc">if</span> cpt == <span class="SNum">1</span>
        {
            <span class="SItr">@assert</span>(cpt == <span class="SNum">1</span>)
            <span class="SLgc">break</span>            <span class="SCmt">// Exits the unnamed scope</span>
        }

        <span class="SLgc">if</span> cpt == <span class="SNum">2</span>
        {
            <span class="SItr">@assert</span>(cpt == <span class="SNum">2</span>)
            <span class="SLgc">break</span>            <span class="SCmt">// Exits the unnamed scope</span>
        }

        <span class="SLgc">if</span> cpt == <span class="SNum">3</span>
        {
            <span class="SItr">@assert</span>(cpt == <span class="SNum">3</span>)
            <span class="SLgc">break</span>            <span class="SCmt">// Exits the unnamed scope</span>
        }
    }
}</span></div>
<p>Note that a scope can be followed by a simple statement rather than a block, allowing for flexible control over flow. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SCmp">#scope</span> <span class="SCst">Up</span>
    <span class="SLgc">loop</span> <span class="SNum">10</span>
    {
        <span class="SLgc">loop</span> <span class="SNum">10</span>
        {
            <span class="SLgc">if</span> <span class="SItr">#index</span> == <span class="SNum">5</span>:
                <span class="SLgc">break</span> <span class="SCst">Up</span>      <span class="SCmt">// Exits to the 'Up' scope</span>
        }

        <span class="SItr">@assert</span>(<span class="SKwd">false</span>)        <span class="SCmt">// This assertion is never reached</span>
    }
}</span></div>

<h2 id="_060_struct_swg">Struct</h2>
<h3 id="_060_struct_swg__061_001_declaration_swg">Declaration</h3><p>This is a basic <span class="code-inline">struct</span> declaration. Note that <span class="code-inline">var</span> is not required for declaring fields within the struct. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">struct</span> <span class="SCst">MyStruct</span>
    {
        name: <span class="STpe">string</span>  <span class="SCmt">// Field 'name' of type 'string'</span>
    }

    <span class="SKwd">struct</span> <span class="SCst">MyStruct1</span>
    {
        x:    <span class="STpe">s32</span>     <span class="SCmt">// Field 'x' of type 's32'</span>
        y, z: <span class="STpe">s32</span>     <span class="SCmt">// Fields 'y' and 'z' of type 's32', declared together</span>
        val:  <span class="STpe">bool</span>    <span class="SCmt">// Field 'val' of type 'bool'</span>
        myS:  <span class="SCst">MyStruct</span> <span class="SCmt">// Field 'myS' of type 'MyStruct', demonstrating a struct within a struct</span>
    }
}</span></div>
<p>Fields within a struct can be separated by either <span class="code-inline">;</span> or <span class="code-inline">,</span>. The trailing separator is optional. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">struct</span> <span class="SCst">MyStruct</span> { name: <span class="STpe">string</span>, val1: <span class="STpe">bool</span> }  <span class="SCmt">// Fields separated by commas</span>
    <span class="SKwd">struct</span> <span class="SCst">MyStruct1</span> { x: <span class="STpe">s32</span>; y, z: <span class="STpe">s32</span>; val: <span class="STpe">bool</span>; myS: <span class="SCst">MyStruct</span> }  <span class="SCmt">// Fields separated by semicolons</span>
}</span></div>
<p>A struct can be anonymous when declared as a variable type. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> tuple: <span class="SKwd">struct</span>
    {
        x: <span class="STpe">f32</span>    <span class="SCmt">// Field 'x' of type 'f32'</span>
        y: <span class="STpe">f32</span>    <span class="SCmt">// Field 'y' of type 'f32'</span>
    }

    <span class="SKwd">var</span> tuple1: <span class="SKwd">struct</span>{ x, y: <span class="STpe">f32</span> }  <span class="SCmt">// Anonymous struct with fields 'x' and 'y' of type 'f32'</span>

    tuple.x = <span class="SNum">1.0</span>  <span class="SCmt">// Assigning value to 'x'</span>
    tuple.y = <span class="SNum">2.0</span>  <span class="SCmt">// Assigning value to 'y'</span>
    <span class="SItr">@assert</span>(tuple.x == <span class="SNum">1.0</span>)  <span class="SCmt">// Asserting 'x' is 1.0</span>
    <span class="SItr">@assert</span>(tuple.y == <span class="SNum">2.0</span>)  <span class="SCmt">// Asserting 'y' is 2.0</span>
}

<span class="SFct">#test</span>
{
    <span class="SKwd">struct</span> <span class="SCst">MyStruct</span>
    {
        rgb: <span class="SKwd">struct</span>{ x, y, z: <span class="STpe">f32</span> }  <span class="SCmt">// Nested anonymous struct for RGB values</span>
        hsl: <span class="SKwd">struct</span>{ h, s, l: <span class="STpe">f32</span> }  <span class="SCmt">// Nested anonymous struct for HSL values</span>
    }
}</span></div>
<p>Struct fields can be initialized with default values. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">struct</span> <span class="SCst">MyStruct</span>
    {
        x: <span class="STpe">s32</span>    = <span class="SNum">666</span>     <span class="SCmt">// Field 'x' with default value 666</span>
        y: <span class="STpe">string</span> = <span class="SStr">"454"</span>   <span class="SCmt">// Field 'y' with default value "454"</span>
    }

    <span class="SKwd">var</span> v = <span class="SCst">MyStruct</span>{}  <span class="SCmt">// Initializing struct with default values</span>
    <span class="SItr">@assert</span>(v.x == <span class="SNum">666</span>)  <span class="SCmt">// Asserting 'x' has the default value 666</span>
    <span class="SItr">@assert</span>(v.y == <span class="SStr">"454"</span>)  <span class="SCmt">// Asserting 'y' has the default value "454"</span>
}</span></div>
<p>Struct variables can be initialized in different ways. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SCmt">// Initializing fields within the struct declaration</span>
    <span class="SKwd">struct</span> <span class="SCst">MyStruct</span>
    {
        x, y: <span class="STpe">s32</span> = <span class="SNum">1</span>  <span class="SCmt">// Both 'x' and 'y' are initialized to 1</span>
    }

    <span class="SCmt">// Without parameters, all fields take their default values as defined in the struct</span>
    <span class="SKwd">var</span> v0: <span class="SCst">MyStruct</span>  <span class="SCmt">// Struct with default initialization</span>
    <span class="SItr">@assert</span>(v0.x == <span class="SNum">1</span>)  <span class="SCmt">// Asserting 'x' is 1</span>
    <span class="SItr">@assert</span>(v0.y == <span class="SNum">1</span>)  <span class="SCmt">// Asserting 'y' is 1</span>

    <span class="SCmt">// Initializing with parameters between {..}, in the order of field declarations</span>
    <span class="SKwd">var</span> v1: <span class="SCst">MyStruct</span>{<span class="SNum">10</span>, <span class="SNum">20</span>}  <span class="SCmt">// Initializing 'x' with 10 and 'y' with 20</span>
    <span class="SItr">@assert</span>(v1.x == <span class="SNum">10</span>)  <span class="SCmt">// Asserting 'x' is 10</span>
    <span class="SItr">@assert</span>(v1.y == <span class="SNum">20</span>)  <span class="SCmt">// Asserting 'y' is 20</span>

    <span class="SCmt">// Named initialization of fields, allowing omission of some fields</span>
    <span class="SKwd">var</span> v2 = <span class="SCst">MyStruct</span>{y: <span class="SNum">20</span>}  <span class="SCmt">// 'x' takes the default value, 'y' is initialized to 20</span>
    <span class="SItr">@assert</span>(v2.x == <span class="SNum">1</span>)  <span class="SCmt">// Asserting 'x' is the default value 1</span>
    <span class="SItr">@assert</span>(v2.y == <span class="SNum">20</span>)  <span class="SCmt">// Asserting 'y' is 20</span>

    <span class="SCmt">// Initializing using a tuple, as long as types match</span>
    <span class="SKwd">var</span> v3: <span class="SCst">MyStruct</span> = {<span class="SNum">10</span>, <span class="SNum">20</span>}  <span class="SCmt">// Initializing using a tuple</span>
    <span class="SItr">@assert</span>(v3.x == <span class="SNum">10</span>)  <span class="SCmt">// Asserting 'x' is 10</span>
    <span class="SItr">@assert</span>(v3.y == <span class="SNum">20</span>)  <span class="SCmt">// Asserting 'y' is 20</span>
}</span></div>
<p>A struct can be assigned to a constant, as long as it can be evaluated at compile-time. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">struct</span> <span class="SCst">MyStruct</span>
    {
        x: <span class="STpe">s32</span>    = <span class="SNum">666</span>     <span class="SCmt">// Field 'x' with default value 666</span>
        y: <span class="STpe">string</span> = <span class="SStr">"454"</span>   <span class="SCmt">// Field 'y' with default value "454"</span>
    }

    <span class="SKwd">const</span> <span class="SCst">X</span>: <span class="SCst">MyStruct</span>{<span class="SNum">50</span>, <span class="SStr">"value"</span>}  <span class="SCmt">// Constant struct with specific values</span>
    <span class="SCmp">#assert</span> <span class="SCst">X</span>.x == <span class="SNum">50</span>  <span class="SCmt">// Compile-time assertion that 'x' is 50</span>
    <span class="SCmp">#assert</span> <span class="SCst">X</span>.y == <span class="SStr">"value"</span>  <span class="SCmt">// Compile-time assertion that 'y' is "value"</span>
}</span></div>
<p>A function can take a struct as an argument. No copy is made; this is equivalent to a const reference in C++. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">struct</span> <span class="SCst">Struct3</span>
{
    x, y, z: <span class="STpe">s32</span> = <span class="SNum">666</span>  <span class="SCmt">// Struct with default values for 'x', 'y', and 'z'</span>
}

<span class="SKwd">func</span> <span class="SFct">toto</span>(v: <span class="SCst">Struct3</span>)
{
    <span class="SItr">@assert</span>(v.x == <span class="SNum">5</span>)  <span class="SCmt">// Asserting 'x' is 5</span>
    <span class="SItr">@assert</span>(v.y == <span class="SNum">5</span>)  <span class="SCmt">// Asserting 'y' is 5</span>
    <span class="SItr">@assert</span>(v.z == <span class="SNum">666</span>)  <span class="SCmt">// Asserting 'z' is the default value 666</span>
}

<span class="SKwd">func</span> <span class="SFct">titi</span>(v: <span class="SCst">Struct3</span>)
{
    <span class="SItr">@assert</span>(v.x == <span class="SNum">5</span>)  <span class="SCmt">// Asserting 'x' is 5</span>
    <span class="SItr">@assert</span>(v.y == <span class="SNum">666</span>)  <span class="SCmt">// Asserting 'y' is the default value 666</span>
}

<span class="SFct">#test</span>
{
    <span class="SCmt">// Calling with a struct literal</span>
    <span class="SFct">toto</span>(<span class="SCst">Struct3</span>{<span class="SNum">5</span>, <span class="SNum">5</span>, <span class="SNum">666</span>})  <span class="SCmt">// Passing explicit values for 'x', 'y', and 'z'</span>

    <span class="SCmt">// Type can be inferred from the argument</span>
    <span class="SFct">toto</span>({<span class="SNum">5</span>, <span class="SNum">5</span>, <span class="SNum">666</span>})  <span class="SCmt">// Inferring the struct type from the provided values</span>

    <span class="SCmt">// Initializing only some parts of the struct, in the order of field declarations</span>
    <span class="SFct">titi</span>({<span class="SNum">5</span>})  <span class="SCmt">// Initializes 'x' only, 'y' and 'z' take default values</span>
    <span class="SFct">titi</span>({<span class="SNum">5</span>, <span class="SNum">666</span>})  <span class="SCmt">// Initializes 'x' and 'y', 'z' takes the default value</span>

    <span class="SCmt">// Initializing using named fields, with the ability to omit some fields</span>
    <span class="SFct">titi</span>({x: <span class="SNum">5</span>, z: <span class="SNum">5</span>})  <span class="SCmt">// 'y' remains at its default value of 666, 'x' and 'z' are set</span>
}</span></div>

<h3 id="_060_struct_swg__062_002_impl_swg">Impl</h3><p>Like for an enum, <span class="code-inline">impl</span> is used to declare functions and constants within the scope of a struct. </p>
<div class="code-block"><span class="SCde"><span class="SAtr">#[Swag.ExportType("methods")]</span> <span class="SCmt">// This attribute exports the methods for type reflection</span>
<span class="SKwd">struct</span> <span class="SCst">MyStruct</span>
{
    x:     <span class="STpe">s32</span> = <span class="SNum">5</span>   <span class="SCmt">// Field 'x' with default value 5</span>
    y:     <span class="STpe">s32</span> = <span class="SNum">10</span>  <span class="SCmt">// Field 'y' with default value 10</span>
    z:     <span class="STpe">s32</span> = <span class="SNum">20</span>  <span class="SCmt">// Field 'z' with default value 20</span>
}

<span class="SKwd">impl</span> <span class="SCst">MyStruct</span>
{
    <span class="SCmt">// Declare a constant within the struct's scope</span>
    <span class="SKwd">const</span> <span class="SCst">MyConst</span> = <span class="SKwd">true</span>

    <span class="SCmt">// Declare a function within the struct's scope</span>
    <span class="SKwd">func</span> <span class="SFct">returnTrue</span>() =&gt; <span class="SKwd">true</span>
}</span></div>
<p>To access the constant and the function, use the <span class="code-inline">MyStruct</span> namespace. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SItr">@assert</span>(<span class="SCst">MyStruct</span>.<span class="SCst">MyConst</span>)           <span class="SCmt">// Accessing the constant within the struct's scope</span>
    <span class="SItr">@assert</span>(<span class="SCst">MyStruct</span>.<span class="SFct">returnTrue</span>())      <span class="SCmt">// Calling the function within the struct's scope</span>
}</span></div>
<p>You can have multiple <span class="code-inline">impl</span> blocks for a struct. The difference between an <span class="code-inline">impl</span> block and a namespace is that <span class="code-inline">self</span> and <span class="code-inline">Self</span> are defined within an <span class="code-inline">impl</span> block. They refer to the corresponding type. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">impl</span> <span class="SCst">MyStruct</span>
{
    <span class="SCmt">// 'self' is an alias for 'var self: Self', where 'Self' refers to 'MyStruct'</span>
    <span class="SKwd">func</span> <span class="SFct">returnX</span>(<span class="SKwd">using</span> <span class="STpe">self</span>) =&gt; x       <span class="SCmt">// Accessing 'x' through 'self'</span>
    <span class="SKwd">func</span> <span class="SFct">returnY</span>(<span class="STpe">self</span>)       =&gt; <span class="STpe">self</span>.y  <span class="SCmt">// Accessing 'y' using 'self' explicitly</span>

    <span class="SCmt">// 'Self' refers to the type 'MyStruct'</span>
    <span class="SKwd">func</span> <span class="SFct">returnZ</span>(me: <span class="STpe">Self</span>) =&gt; me.z      <span class="SCmt">// Accessing 'z' using 'Self' as the type alias</span>
}</span></div>
<p>If you declare your function with <span class="code-inline">mtd</span> (method) instead of <span class="code-inline">func</span>, the first parameter is implicitly <span class="code-inline">using self</span>. If you declare your function with <span class="code-inline">mtd const</span> (method const) instead of <span class="code-inline">func</span>, the first parameter is implicitly <span class="code-inline">const using self</span>. This is <b>syntactic sugar</b> to avoid explicitly writing <span class="code-inline">using self</span>. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">impl</span> <span class="SCst">MyStruct</span>
{
    <span class="SKwd">mtd</span> <span class="SFct">methodReturnX</span>()          =&gt; x  <span class="SCmt">// Equivalent to 'func methodReturnX(using self) =&gt; x'</span>
    <span class="SKwd">func</span> <span class="SFct">funcReturnX</span>(<span class="SKwd">using</span> <span class="STpe">self</span>) =&gt; x  <span class="SCmt">// Explicitly using 'self'</span>
}

<span class="SFct">#test</span>
{
    <span class="SKwd">var</span> c: <span class="SCst">MyStruct</span>
    <span class="SItr">@assert</span>(c.<span class="SFct">returnX</span>() == <span class="SNum">5</span>)          <span class="SCmt">// Calling the method to return 'x'</span>
    <span class="SItr">@assert</span>(c.<span class="SFct">methodReturnX</span>() == <span class="SNum">5</span>)    <span class="SCmt">// Using the 'mtd' syntax to return 'x'</span>
    <span class="SItr">@assert</span>(c.<span class="SFct">funcReturnX</span>() == <span class="SNum">5</span>)      <span class="SCmt">// Explicitly calling with 'func' syntax</span>
    <span class="SItr">@assert</span>(c.<span class="SFct">returnY</span>() == <span class="SNum">10</span>)         <span class="SCmt">// Calling the method to return 'y'</span>
    <span class="SItr">@assert</span>(c.<span class="SFct">returnZ</span>() == <span class="SNum">20</span>)         <span class="SCmt">// Calling the method to return 'z'</span>
}</span></div>
<p>All functions in an <span class="code-inline">impl</span> block can be accessed via reflection, provided the struct is declared with <span class="code-inline">#[Swag.ExportType("methods")]</span> (by default, methods are not exported). </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SCmt">// Create a type alias named 'Lambda' for a function pointer</span>
    <span class="SKwd">typealias</span> <span class="SCst">Lambda</span> = <span class="SKwd">func</span>(<span class="SCst">MyStruct</span>)-&gt;<span class="STpe">s32</span>

    <span class="SKwd">var</span> fnX: <span class="SCst">Lambda</span>  <span class="SCmt">// Function pointer for 'returnX'</span>
    <span class="SKwd">var</span> fnY: <span class="SCst">Lambda</span>  <span class="SCmt">// Function pointer for 'returnY'</span>
    <span class="SKwd">var</span> fnZ: <span class="SCst">Lambda</span>  <span class="SCmt">// Function pointer for 'returnZ'</span>

    <span class="SCmt">// The 'typeinfo' of a struct contains a 'methods' field, which is a slice of methods</span>
    <span class="SKwd">let</span> t = <span class="SCst">MyStruct</span>
    <span class="SLgc">visit</span> p <span class="SLgc">in</span> t.methods
    {
        <span class="SCmt">// When visiting 'methods', the 'value' field contains the function pointer,</span>
        <span class="SCmt">// which can be cast to the correct type</span>
        <span class="SLgc">switch</span> p.name
        {
        <span class="SLgc">case</span> <span class="SStr">"returnX"</span>:
            fnX = <span class="SKwd">cast</span>(<span class="SCst">Lambda</span>) p.value  <span class="SCmt">// Casting the function pointer to 'Lambda'</span>
        <span class="SLgc">case</span> <span class="SStr">"returnY"</span>:
            fnY = <span class="SKwd">cast</span>(<span class="SCst">Lambda</span>) p.value  <span class="SCmt">// Casting the function pointer to 'Lambda'</span>
        <span class="SLgc">case</span> <span class="SStr">"returnZ"</span>:
            fnZ = <span class="SKwd">cast</span>(<span class="SCst">Lambda</span>) p.value  <span class="SCmt">// Casting the function pointer to 'Lambda'</span>
        }
    }

    <span class="SCmt">// These are now valid functions that can be called</span>
    <span class="SKwd">var</span> v: <span class="SCst">MyStruct</span>
    <span class="SItr">@assert</span>(<span class="SFct">fnX</span>(v) == <span class="SNum">5</span>)   <span class="SCmt">// Calling the function pointer 'fnX'</span>
    <span class="SItr">@assert</span>(<span class="SFct">fnY</span>(v) == <span class="SNum">10</span>)  <span class="SCmt">// Calling the function pointer 'fnY'</span>
    <span class="SItr">@assert</span>(<span class="SFct">fnZ</span>(v) == <span class="SNum">20</span>)  <span class="SCmt">// Calling the function pointer 'fnZ'</span>
}</span></div>

<h3 id="_060_struct_swg__063_003_special_functions_swg">Special functions</h3><p>A struct can have special operations in the <span class="code-inline">impl</span> block. This operations are predefined, and known by the compiler. </p>
<p>This is the way to go to <b>overload operators</b> for example. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">struct</span> <span class="SCst">Struct</span>
{
    x, y: <span class="STpe">s32</span>
}

<span class="SKwd">typealias</span> <span class="SCst">OneType</span>      = <span class="STpe">bool</span>
<span class="SKwd">typealias</span> <span class="SCst">AnotherType</span>  = <span class="STpe">s32</span>
<span class="SKwd">typealias</span> <span class="SCst">WhateverType</span> = <span class="STpe">bool</span>

<span class="SKwd">impl</span> <span class="SCst">Struct</span>
{
    <span class="SCmt">// Called each time a variable needs to be dropped</span>
    <span class="SCmt">// This is the destructor in C++</span>
    <span class="SKwd">func</span> <span class="SFct">opDrop</span>(<span class="SKwd">using</span> <span class="STpe">self</span>) {}

    <span class="SCmt">// Called after a raw copy operation from one value to another</span>
    <span class="SKwd">func</span> <span class="SFct">opPostCopy</span>(<span class="SKwd">using</span> <span class="STpe">self</span>) {}

    <span class="SCmt">// Called after a move semantic operation from one value to another. We'll see that later.</span>
    <span class="SKwd">func</span> <span class="SFct">opPostMove</span>(<span class="SKwd">using</span> <span class="STpe">self</span>) {}

    <span class="SCmt">// Get value by slice [low..up]</span>
    <span class="SCmt">// Must return a string or a slice</span>
    <span class="SKwd">func</span> <span class="SFct">opSlice</span>(<span class="SKwd">using</span> <span class="STpe">self</span>, low, up: <span class="STpe">u64</span>)-&gt;<span class="STpe">string</span> { <span class="SLgc">return</span> <span class="SStr">"true"</span>; }

    <span class="SCmt">// Get value by index</span>
    <span class="SKwd">func</span> <span class="SFct">opIndex</span>(<span class="SKwd">using</span> <span class="STpe">self</span>, index: <span class="SCst">OneType</span>)-&gt;<span class="SCst">WhateverType</span> { <span class="SLgc">return</span> <span class="SKwd">true</span>; }

    <span class="SCmt">// Called by @countof</span>
    <span class="SCmt">// Use in a 'loop' block for example</span>
    <span class="SKwd">func</span> <span class="SFct">opCount</span>(<span class="SKwd">using</span> <span class="STpe">self</span>)-&gt;<span class="STpe">u64</span> { <span class="SLgc">return</span> <span class="SNum">0</span>; }
    <span class="SCmt">// Called by @dataof</span>
    <span class="SKwd">func</span> <span class="SFct">opData</span>(<span class="SKwd">using</span> <span class="STpe">self</span>)-&gt;*<span class="SCst">WhateverType</span> { <span class="SLgc">return</span> <span class="SKwd">null</span>; }

    <span class="SCmt">// Called for explicit/implicit casting between struct value and return type</span>
    <span class="SCmt">// Can be overloaded by a different return type</span>
    <span class="SCmt">// Example: var x = cast(OneType) v</span>
    <span class="SAtr">#[Swag.Overload]</span>
    <span class="SKwd">func</span> <span class="SFct">opCast</span>(<span class="SKwd">using</span> <span class="STpe">self</span>)-&gt;<span class="SCst">OneType</span> { <span class="SLgc">return</span> <span class="SKwd">true</span>; }
    <span class="SAtr">#[Swag.Overload]</span>
    <span class="SKwd">func</span> <span class="SFct">opCast</span>(<span class="SKwd">using</span> <span class="STpe">self</span>)-&gt;<span class="SCst">AnotherType</span> { <span class="SLgc">return</span> <span class="SNum">0</span>; }

    <span class="SCmt">// Called to compare the struct value with something else</span>
    <span class="SCmt">// Can be overloaded</span>
    <span class="SCmt">// Returns true if they are equal, otherwise returns false</span>
    <span class="SCmt">// Called by '==', '!='</span>
    <span class="SAtr">#[Swag.Overload]</span>
    <span class="SKwd">func</span> <span class="SFct">opEquals</span>(<span class="SKwd">using</span> <span class="STpe">self</span>, other: <span class="SCst">OneType</span>)-&gt;<span class="STpe">bool</span> { <span class="SLgc">return</span> <span class="SKwd">false</span>; }
    <span class="SAtr">#[Swag.Overload]</span>
    <span class="SKwd">func</span> <span class="SFct">opEquals</span>(<span class="SKwd">using</span> <span class="STpe">self</span>, other: <span class="SCst">AnotherType</span>)-&gt;<span class="STpe">bool</span> { <span class="SLgc">return</span> <span class="SKwd">false</span>; }

    <span class="SCmt">// Called to compare the struct value with something else</span>
    <span class="SCmt">// Can be overloaded</span>
    <span class="SCmt">// Returns -1, 0 or 1</span>
    <span class="SCmt">// Called by '&lt;', '&gt;', '&lt;=', '&gt;=', '&lt;=&gt;'</span>
    <span class="SAtr">#[Swag.Overload]</span>
    <span class="SKwd">func</span> <span class="SFct">opCmp</span>(<span class="SKwd">using</span> <span class="STpe">self</span>, other: <span class="SCst">OneType</span>)-&gt;<span class="STpe">s32</span> { <span class="SLgc">return</span> <span class="SNum">0</span>; }
    <span class="SAtr">#[Swag.Overload]</span>
    <span class="SKwd">func</span> <span class="SFct">opCmp</span>(<span class="SKwd">using</span> <span class="STpe">self</span>, other: <span class="SCst">AnotherType</span>)-&gt;<span class="STpe">s32</span> { <span class="SLgc">return</span> <span class="SNum">0</span>; }

    <span class="SCmt">// Affect struct with another value</span>
    <span class="SCmt">// Can be overloaded</span>
    <span class="SCmt">// Called by '='</span>
    <span class="SAtr">#[Swag.Overload]</span>
    <span class="SKwd">func</span> <span class="SFct">opAffect</span>(<span class="SKwd">using</span> <span class="STpe">self</span>, other: <span class="SCst">OneType</span>) {}
    <span class="SAtr">#[Swag.Overload]</span>
    <span class="SKwd">func</span> <span class="SFct">opAffect</span>(<span class="SKwd">using</span> <span class="STpe">self</span>, other: <span class="SCst">AnotherType</span>) {}

    <span class="SCmt">// Affect struct with a literal value with a specified suffix</span>
    <span class="SCmt">// Generic function, can be overloaded</span>
    <span class="SCmt">// Called by '='</span>
    <span class="SAtr">#[Swag.Overload]</span>
    <span class="SKwd">func</span>(suffix: <span class="STpe">string</span>) <span class="SFct">opAffectLiteral</span>(<span class="SKwd">using</span> <span class="STpe">self</span>, value: <span class="SCst">OneType</span>) {}
    <span class="SAtr">#[Swag.Overload]</span>
    <span class="SKwd">func</span>(suffix: <span class="STpe">string</span>) <span class="SFct">opAffectLiteral</span>(<span class="SKwd">using</span> <span class="STpe">self</span>, value: <span class="SCst">AnotherType</span>) {}

    <span class="SCmt">// Affect struct with another value at a given index</span>
    <span class="SCmt">// Can be overloaded</span>
    <span class="SCmt">// Called by '[] ='</span>
    <span class="SAtr">#[Swag.Overload]</span>
    <span class="SKwd">func</span> <span class="SFct">opIndexAffect</span>(<span class="SKwd">using</span> <span class="STpe">self</span>, index: <span class="SCst">OneType</span>, value: <span class="SCst">OneType</span>) {}
    <span class="SAtr">#[Swag.Overload]</span>
    <span class="SKwd">func</span> <span class="SFct">opIndexAffect</span>(<span class="SKwd">using</span> <span class="STpe">self</span>, index: <span class="SCst">OneType</span>, value: <span class="SCst">AnotherType</span>) {}

    <span class="SCmt">// Binary operator. 'op' generic argument contains the operator string</span>
    <span class="SCmt">// Generic function, can be overloaded</span>
    <span class="SCmt">// Called by '+', '-', '*', '/', '%', '|', '&', '^', '&lt;&lt;', '&gt;&gt;'</span>
    <span class="SAtr">#[Swag.Overload]</span>
    <span class="SKwd">func</span>(op: <span class="STpe">string</span>) <span class="SFct">opBinary</span>(<span class="SKwd">using</span> <span class="STpe">self</span>, other: <span class="SCst">OneType</span>)-&gt;<span class="STpe">Self</span> { <span class="SLgc">return</span> {<span class="SNum">1</span>, <span class="SNum">2</span>}; }
    <span class="SAtr">#[Swag.Overload]</span>
    <span class="SKwd">func</span>(op: <span class="STpe">string</span>) <span class="SFct">opBinary</span>(<span class="SKwd">using</span> <span class="STpe">self</span>, other: <span class="SCst">AnotherType</span>)-&gt;<span class="STpe">Self</span> { <span class="SLgc">return</span> {<span class="SNum">1</span>, <span class="SNum">2</span>}; }

    <span class="SCmt">// Unary operator. 'op' generic argument contains the operator string (see below)</span>
    <span class="SCmt">// Generic function</span>
    <span class="SCmt">// Called by '!', '-', '~'</span>
    <span class="SKwd">func</span>(op: <span class="STpe">string</span>) <span class="SFct">opUnary</span>(<span class="SKwd">using</span> <span class="STpe">self</span>)-&gt;<span class="STpe">Self</span> { <span class="SLgc">return</span> {<span class="SNum">1</span>, <span class="SNum">2</span>}; }

    <span class="SCmt">// Affect operator. 'op' generic argument contains the operator string (see below)</span>
    <span class="SCmt">// Generic function, can be overloaded</span>
    <span class="SCmt">// Called by '+=', '-=', '*=', '/=', '%=', '|=', '&=', '^=', '&lt;&lt;=', '&gt;&gt;='</span>
    <span class="SAtr">#[Swag.Overload]</span>
    <span class="SKwd">func</span>(op: <span class="STpe">string</span>) <span class="SFct">opAssign</span>(<span class="SKwd">using</span> <span class="STpe">self</span>, other: <span class="SCst">OneType</span>) {}
    <span class="SAtr">#[Swag.Overload]</span>
    <span class="SKwd">func</span>(op: <span class="STpe">string</span>) <span class="SFct">opAssign</span>(<span class="SKwd">using</span> <span class="STpe">self</span>, other: <span class="SCst">AnotherType</span>) {}

    <span class="SCmt">// Affect operator. 'op' generic argument contains the operator string (see below)</span>
    <span class="SCmt">// Generic function, can be overloaded</span>
    <span class="SCmt">// Called by '+=', '-=', '*=', '/=', '%=', '|=', '&=', '^=', '&lt;&lt;=', '&gt;&gt;='</span>
    <span class="SAtr">#[Swag.Overload]</span>
    <span class="SKwd">func</span>(op: <span class="STpe">string</span>) <span class="SFct">opIndexAssign</span>(<span class="SKwd">using</span> <span class="STpe">self</span>, index: <span class="SCst">OneType</span>, value: <span class="SCst">OneType</span>) {}
    <span class="SAtr">#[Swag.Overload]</span>
    <span class="SKwd">func</span>(op: <span class="STpe">string</span>) <span class="SFct">opIndexAssign</span>(<span class="SKwd">using</span> <span class="STpe">self</span>, index: <span class="SCst">OneType</span>, value: <span class="SCst">AnotherType</span>) {}

    <span class="SCmt">// Called by a 'visit' block</span>
    <span class="SCmt">// Can have multiple versions, by adding a name after 'opVisit'</span>
    <span class="SCmt">// This is the way to go for iterators</span>
    <span class="SAtr">#[Swag.Macro]</span>
    {
        <span class="SKwd">func</span>(ptr: <span class="STpe">bool</span>, back: <span class="STpe">bool</span>) <span class="SFct">opVisit</span>(<span class="SKwd">using</span> <span class="STpe">self</span>, stmt: <span class="STpe">code</span>) {}
        <span class="SKwd">func</span>(ptr: <span class="STpe">bool</span>, back: <span class="STpe">bool</span>) <span class="SFct">opVisitWhatever</span>(<span class="SKwd">using</span> <span class="STpe">self</span>, stmt: <span class="STpe">code</span>) {}
        <span class="SKwd">func</span>(ptr: <span class="STpe">bool</span>, back: <span class="STpe">bool</span>) <span class="SFct">opVisitAnother</span>(<span class="SKwd">using</span> <span class="STpe">self</span>, stmt: <span class="STpe">code</span>) {}
    }
}</span></div>

<h3 id="_060_struct_swg__064_004_affectation_swg">Affectation</h3><p><span class="code-inline">opAffect</span> allows you to define custom assignment behavior for your struct using the <span class="code-inline">=</span> operator. You can overload <span class="code-inline">opAffect</span> to handle different types, providing specific behavior depending on the type of value assigned. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">struct</span> <span class="SCst">Struct</span>
{
    x, y, z: <span class="STpe">s32</span> = <span class="SNum">666</span>  <span class="SCmt">// Fields with default values</span>
}

<span class="SKwd">impl</span> <span class="SCst">Struct</span>
{
    <span class="SCmt">// Overloaded `opAffect` to handle `s32` assignments</span>
    <span class="SAtr">#[Swag.Overload]</span>
    <span class="SKwd">mtd</span> <span class="SFct">opAffect</span>(value: <span class="STpe">s32</span>) { 
        x, y = value;  <span class="SCmt">// Assign value to fields x and y</span>
    }

    <span class="SCmt">// Overloaded `opAffect` to handle `bool` assignments</span>
    <span class="SAtr">#[Swag.Overload]</span>
    <span class="SKwd">mtd</span> <span class="SFct">opAffect</span>(value: <span class="STpe">bool</span>) { 
        x, y = value ? <span class="SNum">1</span> : <span class="SNum">0</span>;  <span class="SCmt">// Assign 1 if true, 0 if false, to fields x and y</span>
    }
}

<span class="SFct">#test</span>
{
    <span class="SCmt">// Initialize 'v' and call opAffect(s32) with the value '4'</span>
    <span class="SKwd">var</span> v: <span class="SCst">Struct</span> = <span class="SNum">4</span>'<span class="STpe">s32</span>
    <span class="SItr">@assert</span>(v.x == <span class="SNum">4</span>)    <span class="SCmt">// 'x' is set to 4</span>
    <span class="SItr">@assert</span>(v.y == <span class="SNum">4</span>)    <span class="SCmt">// 'y' is set to 4</span>
    <span class="SItr">@assert</span>(v.z == <span class="SNum">666</span>)  <span class="SCmt">// 'z' retains its default value as 'opAffect' does not modify it</span>

    <span class="SCmt">// Initialize 'v1' and call opAffect(bool) with 'true'</span>
    <span class="SKwd">var</span> v1: <span class="SCst">Struct</span> = <span class="SKwd">true</span>
    <span class="SItr">@assert</span>(v1.x == <span class="SNum">1</span>)   <span class="SCmt">// 'x' is set to 1 (true)</span>
    <span class="SItr">@assert</span>(v1.y == <span class="SNum">1</span>)   <span class="SCmt">// 'y' is set to 1 (true)</span>

    <span class="SCmt">// Assign 'false' to 'v1', invoking opAffect(bool)</span>
    v1 = <span class="SKwd">false</span>
    <span class="SItr">@assert</span>(v1.x == <span class="SNum">0</span>)   <span class="SCmt">// 'x' is set to 0 (false)</span>
    <span class="SItr">@assert</span>(v1.y == <span class="SNum">0</span>)   <span class="SCmt">// 'y' is set to 0 (false)</span>
}</span></div>
<p>If <span class="code-inline">opAffect</span> fully initializes the struct, you can use the <span class="code-inline">#[Swag.Complete]</span> attribute to optimize initialization. This prevents the struct from being initialized with default values before being reassigned, which can save performance. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">impl</span> <span class="SCst">Struct</span>
{
    <span class="SCmt">// Marking `opAffect` with `Swag.Complete` ensures only one initialization step</span>
    <span class="SAtr">#[Swag.Complete, Swag.Overload]</span>
    <span class="SKwd">mtd</span> <span class="SFct">opAffect</span>(value: <span class="STpe">u64</span>) { 
        x, y, z = <span class="SKwd">cast</span>(<span class="STpe">s32</span>) value;  <span class="SCmt">// Assign value to x, y, z, converting from u64 to s32</span>
    }

    <span class="SCmt">// Implicit conversion example for u16</span>
    <span class="SAtr">#[Swag.Implicit, Swag.Overload]</span>
    <span class="SKwd">mtd</span> <span class="SFct">opAffect</span>(value: <span class="STpe">u16</span>) { 
        x, y = <span class="SKwd">cast</span>(<span class="STpe">s32</span>) value;  <span class="SCmt">// Assign value to x, y, converting from u16 to s32</span>
    }
}</span></div>
<p>Here, the variable <span class="code-inline">v</span> is initialized directly by <span class="code-inline">opAffect(u64)</span>, skipping the default initialization. This approach is more optimal as it reduces unnecessary assignments. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> v: <span class="SCst">Struct</span> = <span class="SNum">2</span>'<span class="STpe">u64</span>
    <span class="SItr">@assert</span>(v.x == <span class="SNum">2</span>)    <span class="SCmt">// 'x' is directly set to 2</span>
    <span class="SItr">@assert</span>(v.y == <span class="SNum">2</span>)    <span class="SCmt">// 'y' is directly set to 2</span>
    <span class="SItr">@assert</span>(v.z == <span class="SNum">2</span>)    <span class="SCmt">// 'z' is directly set to 2</span>
}</span></div>
<p>By default, function arguments do not undergo automatic conversion via <span class="code-inline">opAffect</span>, so explicit casting is necessary. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">func</span> <span class="SFct">toto</span>(v: <span class="SCst">Struct</span>)
    {
        <span class="SItr">@assert</span>(v.x == <span class="SNum">5</span>)
        <span class="SItr">@assert</span>(v.y == <span class="SNum">5</span>)
        <span class="SItr">@assert</span>(v.z == <span class="SNum">666</span>)
    }

    <span class="SKwd">func</span> <span class="SFct">titi</span>(v: <span class="SCst">Struct</span>)
    {
        <span class="SItr">@assert</span>(v.y == <span class="SNum">666</span>)
    }

    <span class="SCmt">// Explicit cast required to invoke opAffect(s32)</span>
    <span class="SFct">toto</span>(<span class="SKwd">cast</span>(<span class="SCst">Struct</span>) <span class="SNum">5</span>'<span class="STpe">s32</span>)

    <span class="SCmt">// With #[Swag.Implicit], casting is not necessary; automatic conversion occurs</span>
    <span class="SFct">toto</span>(<span class="SNum">5</span>'<span class="STpe">u16</span>)  <span class="SCmt">// Implicitly calls opAffect(u16)</span>
}</span></div>
<p>If you want a struct to be usable as a constant and initialized via <span class="code-inline">opAffect</span>, you can mark the method with <span class="code-inline">#[Swag.ConstExpr]</span>. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">struct</span> <span class="SCst">Vector2</span>
{
    x, y: <span class="STpe">f32</span>  <span class="SCmt">// Fields of type f32</span>
}

<span class="SKwd">impl</span> <span class="SCst">Vector2</span>
{
    <span class="SAtr">#[Swag.ConstExpr]</span>
    <span class="SKwd">mtd</span> <span class="SFct">opAffect</span>(one: <span class="STpe">f32</span>)
    {
        x, y = one  <span class="SCmt">// Assign the same value to both x and y</span>
    }
}

<span class="SCmt">// Using opAffect(f32) to initialize a constant at compile time</span>
<span class="SKwd">const</span> <span class="SCst">One</span>: <span class="SCst">Vector2</span> = <span class="SNum">1.0</span>
<span class="SCmp">#assert</span> <span class="SCst">One</span>.x == <span class="SNum">1.0</span>
<span class="SCmp">#assert</span> <span class="SCst">One</span>.y == <span class="SNum">1.0</span></span></div>

<h3 id="_060_struct_swg__064_005_count_swg">Count</h3><div class="code-block"><span class="SCde"><span class="SKwd">struct</span> <span class="SCst">MyStruct</span> {}

<span class="SKwd">impl</span> <span class="SCst">MyStruct</span>
{
    <span class="SCmt">// Define 'opCount' to return the number of iterations for loops involving this struct.</span>
    <span class="SKwd">mtd</span> <span class="SFct">opCount</span>() =&gt; <span class="SNum">4</span>'<span class="STpe">u64</span>  <span class="SCmt">// Returns 4 as the count value</span>
}</span></div>
<p>With <span class="code-inline">opCount</span> defined, you can loop over an instance of <span class="code-inline">MyStruct</span> just like you would with an array or other iterable type. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> v = <span class="SCst">MyStruct</span>{}

    <span class="SCmt">// '@countof' utilizes 'opCount' to determine the number of elements</span>
    <span class="SItr">@assert</span>(<span class="SItr">@countof</span>(v) == <span class="SNum">4</span>)

    <span class="SCmt">// Loop through the struct, with the loop running as many times as 'opCount' returns</span>
    <span class="SKwd">var</span> cpt = <span class="SNum">0</span>
    <span class="SLgc">loop</span> v:
        cpt += <span class="SNum">1</span>  <span class="SCmt">// Increment the counter each iteration</span>

    <span class="SItr">@assert</span>(cpt == <span class="SNum">4</span>)  <span class="SCmt">// Ensure the loop ran 4 times, as specified by 'opCount'</span>
}</span></div>

<h3 id="_060_struct_swg__064_006_post_copy_and_post_move_swg">Post copy and post move</h3><p>Swag supports both copy and move semantics for structures. In this example, we'll demonstrate these concepts using a <span class="code-inline">Vector3</span> struct. Although a <span class="code-inline">Vector3</span> struct typically wouldn't require move semantics (since it doesn't involve heap allocation), this example serves to illustrate how these features work in Swag. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">struct</span> <span class="SCst">Vector3</span>
{
    x, y, z: <span class="STpe">s32</span> = <span class="SNum">666</span>
}

<span class="SKwd">impl</span> <span class="SCst">Vector3</span>
{
    <span class="SCmt">// This method is invoked after a copy operation.</span>
    <span class="SCmt">// It represents the "copy semantics" and is useful for custom behavior after copying.</span>
    <span class="SKwd">mtd</span> <span class="SFct">opPostCopy</span>()
    {
        x, y, z += <span class="SNum">1</span>  <span class="SCmt">// Increment all fields by 1 to indicate a copy has occurred.</span>
    }

    <span class="SCmt">// This method is invoked after a move operation.</span>
    <span class="SCmt">// It represents the "move semantics" and is useful for custom behavior after moving.</span>
    <span class="SKwd">mtd</span> <span class="SFct">opPostMove</span>()
    {
        x, y, z += <span class="SNum">2</span>  <span class="SCmt">// Increment all fields by 2 to indicate a move has occurred.</span>
    }

    <span class="SCmt">// This method is invoked when an object is about to be destroyed.</span>
    <span class="SCmt">// Even though `Vector3` doesn't manage resources, this is where you would clean up resources like heap-allocated memory.</span>
    <span class="SKwd">mtd</span> <span class="SFct">opDrop</span>() {}
}

<span class="SFct">#test</span>
{
    <span class="SKwd">var</span> a = <span class="SCst">Vector3</span>{}           <span class="SCmt">// Default initialization</span>
    <span class="SKwd">var</span> b = <span class="SCst">Vector3</span>{<span class="SNum">100</span>, <span class="SNum">200</span>, <span class="SNum">300</span>}  <span class="SCmt">// Custom initialization</span>

    <span class="SCmt">// Copy Semantics (default behavior)</span>
    <span class="SCmt">// 1. Calls 'opDrop' on 'a' (if it exists), because 'a' might already hold a value.</span>
    <span class="SCmt">// 2. Copies 'b' to 'a'.</span>
    <span class="SCmt">// 3. Calls 'opPostCopy' on 'a' (if it exists).</span>
    a = b
    <span class="SItr">@assert</span>(a.x == <span class="SNum">101</span>)  <span class="SCmt">// +1 because of 'opPostCopy'</span>
    <span class="SItr">@assert</span>(a.y == <span class="SNum">201</span>)
    <span class="SItr">@assert</span>(a.z == <span class="SNum">301</span>)

    <span class="SCmt">// Move Semantics</span>
    <span class="SCmt">// The `#move` modifier triggers move semantics:</span>
    <span class="SCmt">// 1. Calls 'opDrop' on 'a' (if it exists).</span>
    <span class="SCmt">// 2. Moves 'b' to 'a'.</span>
    <span class="SCmt">// 3. Calls 'opPostMove' on 'a' (if it exists).</span>
    <span class="SCmt">// 4. Reinitializes 'b' to default values (if 'opDrop' exists).</span>
    a = <span class="SKwd">#move</span> b
    <span class="SItr">@assert</span>(a.x == <span class="SNum">102</span>)  <span class="SCmt">// +2 because of 'opPostMove'</span>
    <span class="SItr">@assert</span>(a.y == <span class="SNum">202</span>)
    <span class="SItr">@assert</span>(a.z == <span class="SNum">302</span>)

    <span class="SCmt">// After the move, 'b' is reinitialized to default values (666) because 'opDrop' exists.</span>
    <span class="SItr">@assert</span>(b.x == <span class="SNum">666</span>)

    <span class="SCmt">// You can bypass the first 'opDrop' call with the `#nodrop` modifier.</span>
    <span class="SCmt">// Use this when you know 'a' is in an undefined state and doesn't require cleanup.</span>
    a = <span class="SKwd">#nodrop</span> b         <span class="SCmt">// Copy 'b' to 'a' without dropping 'a' first.</span>
    a = <span class="SKwd">#nodrop</span> <span class="SKwd">#move</span> b   <span class="SCmt">// Move 'b' to 'a' without dropping 'a' first.</span>

    <span class="SCmt">// For move semantics, you can avoid the reinitialization of 'b' using `#moveraw`.</span>
    <span class="SCmt">// This is risky and should only be used if you're certain 'b' won't be dropped or if you plan to reinitialize it manually.</span>
    a = <span class="SKwd">#moveraw</span> b        <span class="SCmt">// Move 'b' to 'a' without resetting 'b'.</span>
    a = <span class="SKwd">#nodrop</span> <span class="SKwd">#moveraw</span> b <span class="SCmt">// Move 'b' to 'a' without dropping 'a' first and without resetting 'b'.</span>
}</span></div>
<h4 id="_060_struct_swg__064_006_post_copy_and_post_move_swg_Move_Semantics_in_Functions">Move Semantics in Functions </h4>
<p>You can indicate a move semantic intention in function parameters by using <span class="code-inline">&&</span> instead of <span class="code-inline">&</span>. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SCmt">// This version of 'assign' takes ownership of 'from' by moving its contents into 'assignTo'.</span>
    <span class="SCmt">// The 'moveref' tells the compiler that this version takes ownership of 'from'.</span>
    <span class="SAtr">#[Swag.Overload]</span>
    <span class="SKwd">func</span> <span class="SFct">assign</span>(assignTo: &<span class="SCst">Vector3</span>, from: &&<span class="SCst">Vector3</span>)
    {
        assignTo = <span class="SKwd">#move</span> from  <span class="SCmt">// Move 'from' to 'assignTo'.</span>
    }

    <span class="SCmt">// This version of 'assign' performs a copy instead of a move.</span>
    <span class="SCmt">// 'from' remains unchanged, so it's passed by value.</span>
    <span class="SAtr">#[Swag.Overload]</span>
    <span class="SKwd">func</span> <span class="SFct">assign</span>(assignTo: &<span class="SCst">Vector3</span>, from: <span class="SCst">Vector3</span>)
    {
        assignTo = from  <span class="SCmt">// Copy 'from' to 'assignTo'.</span>
    }

    <span class="SKwd">var</span> a = <span class="SCst">Vector3</span>{<span class="SNum">1</span>, <span class="SNum">2</span>, <span class="SNum">3</span>}  <span class="SCmt">// Initialize 'a'.</span>
    <span class="SKwd">var</span> b: <span class="SCst">Vector3</span>            <span class="SCmt">// Declare 'b'.</span>

    <span class="SCmt">// Call the copy version of 'assign'.</span>
    <span class="SFct">assign</span>(&b, a)
    <span class="SItr">@assert</span>(b.x == <span class="SNum">2</span> <span class="SLgc">and</span> b.y == <span class="SNum">3</span> <span class="SLgc">and</span> b.z == <span class="SNum">4</span>)  <span class="SCmt">// +1 on each field due to 'opPostCopy'.</span>
    <span class="SItr">@assert</span>(a.x == <span class="SNum">1</span> <span class="SLgc">and</span> a.y == <span class="SNum">2</span> <span class="SLgc">and</span> a.z == <span class="SNum">3</span>)  <span class="SCmt">// 'a' remains unchanged.</span>

    <span class="SCmt">// Call the move version of 'assign' using 'moveref'.</span>
    <span class="SFct">assign</span>(&b, <span class="SKwd">moveref</span> &a)
    <span class="SItr">@assert</span>(b.x == <span class="SNum">3</span> <span class="SLgc">and</span> b.y == <span class="SNum">4</span> <span class="SLgc">and</span> b.z == <span class="SNum">5</span>)  <span class="SCmt">// +2 on each field due to 'opPostMove'.</span>
    <span class="SItr">@assert</span>(a.x == <span class="SNum">666</span> <span class="SLgc">and</span> a.y == <span class="SNum">666</span> <span class="SLgc">and</span> a.z == <span class="SNum">666</span>)  <span class="SCmt">// 'a' is reset to default values after the move.</span>
}</span></div>

<h3 id="_060_struct_swg__064_007_visit_swg">Visit</h3><div class="code-block"><span class="SCde"><span class="SKwd">struct</span> <span class="SCst">MyStruct</span>
{
    x:     <span class="STpe">s32</span> = <span class="SNum">10</span>
    y:     <span class="STpe">s32</span> = <span class="SNum">20</span>
    z:     <span class="STpe">s32</span> = <span class="SNum">30</span>
}</span></div>
<p><span class="code-inline">opVisit</span> is a versatile macro used for iteration over elements in a struct, but it is not limited to fields. It can be used to visit any kind of data the struct holds, such as elements of a dynamic array, internal buffers, or complex object graphs. </p>
<p>The <span class="code-inline">#[Swag.Macro]</span> attribute is mandatory for <span class="code-inline">opVisit</span>. It is also a generic function with two compile-time boolean parameters: </p>
<ul>
<li><span class="code-inline">ptr</span>: If <span class="code-inline">true</span>, visit elements by pointer (address).</li>
<li><span class="code-inline">back</span>: If <span class="code-inline">true</span>, visit elements in reverse order (last to first).</li>
</ul>
<div class="code-block"><span class="SCde"><span class="SKwd">impl</span> <span class="SCst">MyStruct</span>
{
    <span class="SAtr">#[Swag.Macro]</span>
    <span class="SKwd">func</span>(ptr: <span class="STpe">bool</span>, back: <span class="STpe">bool</span>) <span class="SFct">opVisit</span>(<span class="STpe">self</span>, stmt: <span class="STpe">code</span>)
    {
        <span class="SCmt">// `ptr` and `back` provide flexibility for visiting by reference or in reverse order.</span>
        <span class="SCmp">#if</span> ptr:
            <span class="SCmp">#error</span> <span class="SStr">"visiting by pointer is not supported in this example"</span>

        <span class="SCmp">#if</span> back:
            <span class="SCmp">#error</span> <span class="SStr">"reverse visiting is not supported in this example"</span>

        <span class="SCmt">// Example of visiting the fields of `MyStruct`. This is just one way to use `opVisit`.</span>
        <span class="SLgc">loop</span> idx <span class="SLgc">in</span> <span class="SNum">3</span>
        {
            <span class="SCmt">// `#macro` ensures the code is injected into the caller's scope.</span>
            <span class="SCmp">#macro</span>
            {
                <span class="SCmt">// `#alias0` is used for the current value being visited.</span>
                <span class="SKwd">var</span> <span class="SItr">#alias0</span>: <span class="STpe">s32</span> = <span class="SKwd">undefined</span>

                <span class="SCmt">// Access the correct field based on the current `idx`.</span>
                <span class="SLgc">switch</span> <span class="SCmp">#up</span> idx
                {
                <span class="SLgc">case</span> <span class="SNum">0</span>:
                    <span class="SItr">#alias0</span> = <span class="SCmp">#up</span> <span class="STpe">self</span>.x
                <span class="SLgc">case</span> <span class="SNum">1</span>:
                    <span class="SItr">#alias0</span> = <span class="SCmp">#up</span> <span class="STpe">self</span>.y
                <span class="SLgc">case</span> <span class="SNum">2</span>:
                    <span class="SItr">#alias0</span> = <span class="SCmp">#up</span> <span class="STpe">self</span>.z
                }

                <span class="SCmt">// `#alias1` holds the current index of the iteration.</span>
                <span class="SKwd">var</span> <span class="SItr">#alias1</span> = <span class="SItr">#index</span>

                <span class="SCmt">// Insert the user-defined logic from the calling scope.</span>
                <span class="SCmp">#mixin</span> <span class="SCmp">#up</span> stmt
            }
        }
    }
}</span></div>
<p>This is just one way to use <span class="code-inline">opVisit</span>, visiting the struct's fields. You can apply the same concept to visit more complex data, like elements in a dynamic array or other internal structures. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> myStruct = <span class="SCst">MyStruct</span>{}
    <span class="SKwd">var</span> cpt = <span class="SNum">0</span>

    <span class="SCmt">// Visit each field of `MyStruct` in declaration order.</span>
    <span class="SCmt">// `v` corresponds to the value (i.e., #alias0).</span>
    <span class="SCmt">// `i` corresponds to the index (i.e., #alias1).</span>
    <span class="SLgc">visit</span> v, i <span class="SLgc">in</span> myStruct
    {
        <span class="SLgc">switch</span> i
        {
        <span class="SLgc">case</span> <span class="SNum">0</span>:
            <span class="SItr">@assert</span>(v == <span class="SNum">10</span>)
        <span class="SLgc">case</span> <span class="SNum">1</span>:
            <span class="SItr">@assert</span>(v == <span class="SNum">20</span>)
        <span class="SLgc">case</span> <span class="SNum">2</span>:
            <span class="SItr">@assert</span>(v == <span class="SNum">30</span>)
        }

        cpt += <span class="SNum">1</span>
    }

    <span class="SItr">@assert</span>(cpt == <span class="SNum">3</span>)
}</span></div>
<p>You can also implement different versions of <span class="code-inline">opVisit</span> to handle other data. For example, if your struct contains dynamic arrays or buffers, you can iterate over those elements using the same macro structure. </p>
<p>Here, we'll demonstrate a variant that visits the fields in reverse order. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">impl</span> <span class="SCst">MyStruct</span>
{
    <span class="SAtr">#[Swag.Macro]</span>
    <span class="SKwd">mtd</span>(ptr: <span class="STpe">bool</span>, back: <span class="STpe">bool</span>) <span class="SFct">opVisitReverse</span>(stmt: <span class="STpe">code</span>)
    {
        <span class="SCmt">// In this version, we visit the fields in reverse order.</span>
        <span class="SLgc">loop</span> idx <span class="SLgc">in</span> <span class="SNum">3</span>
        {
            <span class="SCmp">#macro</span>
            {
                <span class="SKwd">var</span> <span class="SItr">#alias0</span>: <span class="STpe">s32</span> = <span class="SKwd">undefined</span>
                <span class="SLgc">switch</span> <span class="SCmp">#up</span> idx
                {
                <span class="SLgc">case</span> <span class="SNum">0</span>:
                    <span class="SItr">#alias0</span> = <span class="SCmp">#up</span> <span class="STpe">self</span>.z
                <span class="SLgc">case</span> <span class="SNum">1</span>:
                    <span class="SItr">#alias0</span> = <span class="SCmp">#up</span> <span class="STpe">self</span>.y
                <span class="SLgc">case</span> <span class="SNum">2</span>:
                    <span class="SItr">#alias0</span> = <span class="SCmp">#up</span> <span class="STpe">self</span>.x
                }

                <span class="SKwd">var</span> <span class="SItr">#alias1</span> = <span class="SItr">#index</span>
                <span class="SCmp">#mixin</span> <span class="SCmp">#up</span> stmt
            }
        }
    }
}</span></div>
<p>The <span class="code-inline">opVisitReverse</span> variant allows us to visit the struct in reverse order. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> myStruct = <span class="SCst">MyStruct</span>{}
    <span class="SKwd">var</span> cpt = <span class="SNum">0</span>

    <span class="SCmt">// Call the variant `opVisitReverse` to iterate over the fields in reverse order.</span>
    <span class="SLgc">visit</span>(<span class="SCst">Reverse</span>) v, i <span class="SLgc">in</span> myStruct
    {
        <span class="SLgc">switch</span> i
        {
        <span class="SLgc">case</span> <span class="SNum">0</span>:
            <span class="SItr">@assert</span>(v == <span class="SNum">30</span>)
        <span class="SLgc">case</span> <span class="SNum">1</span>:
            <span class="SItr">@assert</span>(v == <span class="SNum">20</span>)
        <span class="SLgc">case</span> <span class="SNum">2</span>:
            <span class="SItr">@assert</span>(v == <span class="SNum">10</span>)
        }

        cpt += <span class="SNum">1</span>
    }

    <span class="SItr">@assert</span>(cpt == <span class="SNum">3</span>)
}</span></div>
<p>Beyond struct fields, <span class="code-inline">opVisit</span> can be designed to visit elements in dynamic arrays, buffers, or other types of data. The flexibility of <span class="code-inline">opVisit</span> means it can adapt to whatever data structure the struct holds. </p>
<p>For instance, if you had a struct with a slice: </p>
<div class="code-block"><span class="SCde"><span class="SKwd">struct</span> <span class="SCst">SliceStruct</span>
{
    buffer: [] <span class="STpe">s32</span> = [<span class="SNum">1</span>, <span class="SNum">2</span>, <span class="SNum">3</span>, <span class="SNum">4</span>, <span class="SNum">5</span>]
}</span></div>
<p>You could define an <span class="code-inline">opVisit</span> that iterates over the elements of the <span class="code-inline">buffer</span> rather than the struct's fields. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">impl</span> <span class="SCst">SliceStruct</span>
{
    <span class="SAtr">#[Swag.Macro]</span>
    <span class="SKwd">func</span>(ptr: <span class="STpe">bool</span>, back: <span class="STpe">bool</span>) <span class="SFct">opVisit</span>(<span class="STpe">self</span>, stmt: <span class="STpe">code</span>)
    {
        <span class="SLgc">loop</span> idx <span class="SLgc">in</span> <span class="SItr">@countof</span>(<span class="STpe">self</span>.buffer)
        {
            <span class="SCmp">#macro</span>
            {
                <span class="SCmt">// #alias0 represents the value of the current buffer element.</span>
                <span class="SKwd">var</span> <span class="SItr">#alias0</span> = <span class="SCmp">#up</span> <span class="STpe">self</span>.buffer[<span class="SCmp">#up</span> idx]

                <span class="SCmt">// #alias1 represents the current index.</span>
                <span class="SKwd">var</span> <span class="SItr">#alias1</span> = <span class="SItr">#index</span>

                <span class="SCmt">// Insert the user-provided logic from the caller.</span>
                <span class="SCmp">#mixin</span> <span class="SCmp">#up</span> stmt
            }
        }
    }
}

<span class="SFct">#test</span>
{
    <span class="SKwd">var</span> arrStruct = <span class="SCst">SliceStruct</span>{}
    <span class="SKwd">var</span> sum = <span class="SNum">0</span>

    <span class="SCmt">// Visit each element in the dynamic array buffer.</span>
    <span class="SLgc">visit</span> v, i <span class="SLgc">in</span> arrStruct
    {
        sum += v
    }

    <span class="SItr">@assert</span>(sum == <span class="SNum">1</span> + <span class="SNum">2</span> + <span class="SNum">3</span> + <span class="SNum">4</span> + <span class="SNum">5</span>)
}</span></div>

<h3 id="_060_struct_swg__067_008_offset_swg">Offset</h3><p>You can force the layout of a field within a struct using the <span class="code-inline">Swag.Offset</span> attribute. This allows you to manually specify the memory offset of a field, potentially causing fields to overlap or share memory space. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">struct</span> <span class="SCst">MyStruct</span>
    {
        x: <span class="STpe">s32</span>

        <span class="SCmt">// 'y' is located at the same offset as 'x', meaning they share the same memory space.</span>
        <span class="SCmt">// This effectively creates an overlay, where changes to one field affect the other.</span>
        <span class="SAtr">#[Swag.Offset("x")]</span>
        y: <span class="STpe">s32</span>
    }

    <span class="SCmt">// Although there are two fields defined, they occupy the same space, so the struct only uses 4 bytes.</span>
    <span class="SCmp">#assert</span> <span class="SItr">@sizeof</span>(<span class="SCst">MyStruct</span>) == <span class="SNum">4</span>

    <span class="SKwd">var</span> v = <span class="SCst">MyStruct</span>{}
    v.x = <span class="SNum">666</span>

    <span class="SCmt">// Since 'x' and 'y' share the same memory space, modifying 'x' also modifies 'y'.</span>
    <span class="SItr">@assert</span>(v.y == <span class="SNum">666</span>)
}</span></div>
<p>Another example of using <span class="code-inline">Swag.Offset</span>, this time to reference a group of fields by index. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">struct</span> <span class="SCst">MyStruct</span>
    {
        x, y, z: <span class="STpe">f32</span>

        <span class="SCmt">// Here, 'idx' is an array that references the same memory locations as 'x', 'y', and 'z'.</span>
        <span class="SCmt">// This allows you to access 'x', 'y', and 'z' via indexed access through 'idx'.</span>
        <span class="SAtr">#[Swag.Offset("x")]</span>
        idx: [<span class="SNum">3</span>] <span class="STpe">f32</span>
    }

    <span class="SKwd">var</span> v: <span class="SCst">MyStruct</span>
    v.x = <span class="SNum">10</span>; v.y = <span class="SNum">20</span>; v.z = <span class="SNum">30</span>

    <span class="SCmt">// Each index in 'idx' directly references 'x', 'y', and 'z'.</span>
    <span class="SItr">@assert</span>(v.idx[<span class="SNum">0</span>] == v.x)
    <span class="SItr">@assert</span>(v.idx[<span class="SNum">1</span>] == v.y)
    <span class="SItr">@assert</span>(v.idx[<span class="SNum">2</span>] == v.z)
}</span></div>

<h3 id="_060_struct_swg__068_009_packing_swg">Packing</h3><p>You can control the layout of struct fields with the attributes <span class="code-inline">#[Swag.Pack]</span> and <span class="code-inline">#[Swag.Align]</span>. These attributes allow you to specify how tightly packed or aligned your data should be in memory. </p>
<p>The default struct packing in Swag is similar to C, where each field is aligned based on the size of its type. This is equivalent to <span class="code-inline">#[Swag.Pack(0)]</span>, meaning no additional packing is applied. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">struct</span> <span class="SCst">MyStruct</span>
    {
        x:     <span class="STpe">bool</span>     <span class="SCmt">// offset 0: aligned to 1 byte (no padding needed)</span>
        y:     <span class="STpe">s32</span>      <span class="SCmt">// offset 4: aligned to 4 bytes (3 bytes of padding before y)</span>
        z:     <span class="STpe">s64</span>      <span class="SCmt">// offset 8: aligned to 8 bytes (no padding needed)</span>
    }

    <span class="SCmp">#assert</span> <span class="SItr">@offsetof</span>(<span class="SCst">MyStruct</span>.x) == <span class="SNum">0</span>
    <span class="SCmp">#assert</span> <span class="SItr">@offsetof</span>(<span class="SCst">MyStruct</span>.y) == <span class="SNum">4</span>
    <span class="SCmp">#assert</span> <span class="SItr">@offsetof</span>(<span class="SCst">MyStruct</span>.z) == <span class="SNum">8</span>
    <span class="SCmp">#assert</span> <span class="SItr">@sizeof</span>(<span class="SCst">MyStruct</span>) == <span class="SNum">16</span>
}</span></div>
<p>You can reduce the packing of fields using <span class="code-inline">#[Swag.Pack]</span>, which determines the alignment of each field to the specified value. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SAtr">#[Swag.Pack(1)]</span>
    <span class="SKwd">struct</span> <span class="SCst">MyStruct1</span>
    {
        x:     <span class="STpe">bool</span>     <span class="SCmt">// offset 0: 1 byte (no padding)</span>
        y:     <span class="STpe">s32</span>      <span class="SCmt">// offset 1: 4 bytes (no padding)</span>
    }
    <span class="SCmp">#assert</span> <span class="SItr">@offsetof</span>(<span class="SCst">MyStruct1</span>.x) == <span class="SNum">0</span>
    <span class="SCmp">#assert</span> <span class="SItr">@offsetof</span>(<span class="SCst">MyStruct1</span>.y) == <span class="SNum">1</span>
    <span class="SCmp">#assert</span> <span class="SItr">@sizeof</span>(<span class="SCst">MyStruct1</span>) == <span class="SNum">5</span>

    <span class="SAtr">#[Swag.Pack(2)]</span>
    <span class="SKwd">struct</span> <span class="SCst">MyStruct2</span>
    {
        x:     <span class="STpe">bool</span>     <span class="SCmt">// offset 0: 1 byte</span>
        y:     <span class="STpe">s32</span>      <span class="SCmt">// offset 2: 4 bytes (1 byte of padding before y)</span>
    }
    <span class="SCmp">#assert</span> <span class="SItr">@offsetof</span>(<span class="SCst">MyStruct2</span>.x) == <span class="SNum">0</span>
    <span class="SCmp">#assert</span> <span class="SItr">@offsetof</span>(<span class="SCst">MyStruct2</span>.y) == <span class="SNum">2</span>
    <span class="SCmp">#assert</span> <span class="SItr">@sizeof</span>(<span class="SCst">MyStruct2</span>) == <span class="SNum">6</span>

    <span class="SAtr">#[Swag.Pack(4)]</span>
    <span class="SKwd">struct</span> <span class="SCst">MyStruct3</span>
    {
        x:     <span class="STpe">bool</span>     <span class="SCmt">// offset 0: 1 byte</span>
        y:     <span class="STpe">s64</span>      <span class="SCmt">// offset 4: 8 bytes (3 bytes of padding before y)</span>
    }
    <span class="SCmp">#assert</span> <span class="SItr">@offsetof</span>(<span class="SCst">MyStruct3</span>.x) == <span class="SNum">0</span>
    <span class="SCmp">#assert</span> <span class="SItr">@offsetof</span>(<span class="SCst">MyStruct3</span>.y) == <span class="SNum">4</span>
    <span class="SCmp">#assert</span> <span class="SItr">@sizeof</span>(<span class="SCst">MyStruct3</span>) == <span class="SNum">12</span>
}</span></div>
<p>The total struct size is always a multiple of the largest alignment of its fields. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">struct</span> <span class="SCst">MyStruct1</span>
    {
        x:     <span class="STpe">s32</span>      <span class="SCmt">// 4 bytes</span>
        y:     <span class="STpe">bool</span>     <span class="SCmt">// 1 byte</span>
        <span class="SCmt">// 3 bytes of padding to align with s32 size</span>
    }
    <span class="SCmp">#assert</span> <span class="SItr">@sizeof</span>(<span class="SCst">MyStruct1</span>) == <span class="SNum">8</span>
}</span></div>
<p>You can enforce struct alignment with <span class="code-inline">#[Swag.Align]</span>. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">struct</span> <span class="SCst">MyStruct1</span>
    {
        x:     <span class="STpe">bool</span>     <span class="SCmt">// 1 byte</span>
        y:     <span class="STpe">bool</span>     <span class="SCmt">// 1 byte</span>
    }
    <span class="SCmp">#assert</span> <span class="SItr">@offsetof</span>(<span class="SCst">MyStruct1</span>.x) == <span class="SNum">0</span>
    <span class="SCmp">#assert</span> <span class="SItr">@offsetof</span>(<span class="SCst">MyStruct1</span>.y) == <span class="SNum">1</span>
    <span class="SCmp">#assert</span> <span class="SItr">@sizeof</span>(<span class="SCst">MyStruct1</span>) == <span class="SNum">2</span>

    <span class="SAtr">#[Swag.Align(8)]</span>
    <span class="SKwd">struct</span> <span class="SCst">MyStruct2</span>
    {
        x:     <span class="STpe">bool</span>     <span class="SCmt">// 1 byte</span>
        y:     <span class="STpe">bool</span>     <span class="SCmt">// 1 byte</span>
        <span class="SCmt">// 6 bytes of padding to align struct size to 8</span>
    }
    <span class="SCmp">#assert</span> <span class="SItr">@offsetof</span>(<span class="SCst">MyStruct2</span>.x) == <span class="SNum">0</span>
    <span class="SCmp">#assert</span> <span class="SItr">@offsetof</span>(<span class="SCst">MyStruct2</span>.y) == <span class="SNum">1</span>
    <span class="SCmp">#assert</span> <span class="SItr">@sizeof</span>(<span class="SCst">MyStruct2</span>) == <span class="SNum">8</span>
}</span></div>
<p>You can also set alignment for individual fields. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">struct</span> <span class="SCst">MyStruct1</span>
    {
        x: <span class="STpe">bool</span> <span class="SCmt">// offset 0: 1 byte</span>
        <span class="SAtr">#[Swag.Align(8)]</span>
        y: <span class="STpe">bool</span> <span class="SCmt">// offset 8: aligned to 8 bytes (7 bytes of padding before y)</span>
    }
    <span class="SCmp">#assert</span> <span class="SItr">@sizeof</span>(<span class="SCst">MyStruct1</span>) == <span class="SNum">9</span>

    <span class="SAtr">#[Swag.Align(8)]</span>
    <span class="SKwd">struct</span> <span class="SCst">MyStruct2</span>
    {
        x: <span class="STpe">bool</span> <span class="SCmt">// offset 0: 1 byte</span>
        <span class="SAtr">#[Swag.Align(4)]</span>
        y: <span class="STpe">bool</span> <span class="SCmt">// offset 4: aligned to 4 bytes (3 bytes of padding before y)</span>
        <span class="SCmt">// 3 bytes of padding to align struct size to 8</span>
    }
    <span class="SCmp">#assert</span> <span class="SItr">@sizeof</span>(<span class="SCst">MyStruct2</span>) == <span class="SNum">8</span>
}</span></div>

<h2 id="_070_union_swg">Union</h2><p>An <span class="code-inline">union</span> is a special type of struct where all fields share the same memory location, meaning they all start at offset 0. This allows multiple fields to occupy the same space in memory. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SCmt">// Define a union with three fields: x, y, and z, all of type f32.</span>
    <span class="SKwd">union</span> <span class="SCst">MyUnion</span>
    {
        x, y, z: <span class="STpe">f32</span>
    }

    <span class="SCmt">// Initialize the union, setting the value of x to 666.</span>
    <span class="SKwd">var</span> v = <span class="SCst">MyUnion</span>{x: <span class="SNum">666</span>}

    <span class="SCmt">// Since all fields share the same memory, setting x also sets y and z.</span>
    <span class="SItr">@assert</span>(v.y == <span class="SNum">666</span>)
    <span class="SItr">@assert</span>(v.z == <span class="SNum">666</span>)
}</span></div>
<p>In this example, the union type allows you to define multiple fields (x, y, z), but since they all occupy the same memory location, modifying one field will affect the others. This is useful when you need to store different types of data in the same memory space but at different times. Here, <span class="code-inline">x</span>, <span class="code-inline">y</span>, and <span class="code-inline">z</span> are all of type <span class="code-inline">f32</span>, so changing one affects the others since they overlap in memory. </p>

<h2 id="_075_interface_swg">Interface</h2><p>Interfaces in Swag are <b>virtual tables</b> (a list of function pointers) that can be associated with a struct. </p>
<p>Unlike C++, the virtual table is not embedded within the struct. It is a <b>separate</b> object. You can then <i>implement</i> an interface for a given struct without changing the struct definition. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">struct</span> <span class="SCst">Point2</span>
{
    x, y: <span class="STpe">f32</span>
}

<span class="SKwd">struct</span> <span class="SCst">Point3</span>
{
    x, y, z: <span class="STpe">f32</span>
}</span></div>
<p>Here we declare an interface <span class="code-inline">IReset</span>, with two functions <span class="code-inline">set</span> and <span class="code-inline">reset</span>. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">interface</span> <span class="SCst">IReset</span>
{
    <span class="SCmt">// The first parameter must be 'self'</span>
    <span class="SKwd">func</span> <span class="SFct">set</span>(<span class="STpe">self</span>, val: <span class="STpe">f32</span>);

    <span class="SCmt">// You can also use the 'mtd' declaration to avoid specifying the 'self' yourself</span>
    <span class="SKwd">mtd</span> <span class="SFct">reset</span>();
}</span></div>
<p>You can implement an interface for any given struct with <span class="code-inline">impl</span> and <span class="code-inline">for</span>. For example here, we implement interface <span class="code-inline">IReset</span> for struct <span class="code-inline">Point2</span>. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">impl</span> <span class="SCst">IReset</span> <span class="SLgc">for</span> <span class="SCst">Point2</span>
{
    <span class="SCmt">// You must add 'impl' to indicate that you want to implement a function of the interface.</span>
    <span class="SKwd">mtd</span> <span class="SKwd">impl</span> <span class="SFct">set</span>(val: <span class="STpe">f32</span>)
    {
        x = val
        y = val + <span class="SNum">1</span>
    }

    <span class="SCmt">// Don't forget that 'mtd' is just syntaxic sugar. 'func' still works.</span>
    <span class="SKwd">func</span> <span class="SKwd">impl</span> <span class="SFct">reset</span>(<span class="STpe">self</span>)
    {
        <span class="STpe">self</span>.x, <span class="STpe">self</span>.y = <span class="SNum">0</span>
    }

    <span class="SCmt">// Note that you can also declare 'normal' functions or methods in an 'impl block'.</span>
    <span class="SKwd">mtd</span> <span class="SFct">myOtherMethod</span>() {}
}</span></div>
<p>And we implement interface <span class="code-inline">IReset</span> also for struct <span class="code-inline">Point3</span>. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">impl</span> <span class="SCst">IReset</span> <span class="SLgc">for</span> <span class="SCst">Point3</span>
{
    <span class="SKwd">mtd</span> <span class="SKwd">impl</span> <span class="SFct">set</span>(val: <span class="STpe">f32</span>)
    {
        x = val
        y = val + <span class="SNum">1</span>
        z = val + <span class="SNum">2</span>
    }

    <span class="SKwd">mtd</span> <span class="SKwd">impl</span> <span class="SFct">reset</span>()
    {
        x, y, z = <span class="SNum">0</span>
    }
}</span></div>
<p>We can then use these interfaces on either <span class="code-inline">Vector2</span> or <span class="code-inline">Vector3</span>. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> pt2: <span class="SCst">Point2</span>
    <span class="SKwd">var</span> pt3: <span class="SCst">Point3</span>

    <span class="SCmt">// To get the interface associated to a given struct, use the 'cast' operator.</span>
    <span class="SCmt">// If the compiler does not find the corresponding implementation, it will raise an error.</span>
    <span class="SKwd">var</span> itf = <span class="SKwd">cast</span>(<span class="SCst">IReset</span>) pt2
    itf.<span class="SFct">set</span>(<span class="SNum">10</span>)
    <span class="SItr">@assert</span>(pt2.x == <span class="SNum">10</span>)
    <span class="SItr">@assert</span>(pt2.y == <span class="SNum">10</span> + <span class="SNum">1</span>)

    itf = <span class="SKwd">cast</span>(<span class="SCst">IReset</span>) pt3
    itf.<span class="SFct">set</span>(<span class="SNum">10</span>)
    <span class="SItr">@assert</span>(pt3.x == <span class="SNum">10</span>)
    <span class="SItr">@assert</span>(pt3.y == <span class="SNum">10</span> + <span class="SNum">1</span>)
    <span class="SItr">@assert</span>(pt3.z == <span class="SNum">10</span> + <span class="SNum">2</span>)
    itf.<span class="SFct">reset</span>()
    <span class="SItr">@assert</span>(pt3.x == <span class="SNum">0</span> <span class="SLgc">and</span> pt3.y == <span class="SNum">0</span> <span class="SLgc">and</span> pt3.z == <span class="SNum">0</span>)
}</span></div>
<p>You can also access, with a normal call, all functions declared in an interface implementation block for a given struct. They are located in a dedicated scope. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> pt2: <span class="SCst">Point2</span>
    <span class="SKwd">var</span> pt3: <span class="SCst">Point3</span>

    <span class="SCmt">// The scope where all functions are located has the same name as the interface.</span>
    pt2.<span class="SCst">IReset</span>.<span class="SFct">set</span>(<span class="SNum">10</span>)
    pt2.<span class="SCst">IReset</span>.<span class="SFct">reset</span>()
    pt3.<span class="SCst">IReset</span>.<span class="SFct">set</span>(<span class="SNum">10</span>)
    pt3.<span class="SCst">IReset</span>.<span class="SFct">reset</span>()
}</span></div>
<p>An interface is a real type, with a size equivalent to 2 pointers. A pointer to the <i>object</i> and a pointer to the <i>virtual table</i>. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> pt2: <span class="SCst">Point2</span>
    <span class="SKwd">var</span> pt3: <span class="SCst">Point3</span>
    <span class="SKwd">var</span> itf  = <span class="SKwd">cast</span>(<span class="SCst">IReset</span>) pt2

    <span class="SCmp">#assert</span> <span class="SItr">@sizeof</span>(itf) == <span class="SNum">2</span> * <span class="SItr">@sizeof</span>(*<span class="STpe">void</span>)

    <span class="SCmt">// You can retrieve the concrete type associated with an interface instance with '@kindof'.</span>
    itf = <span class="SKwd">cast</span>(<span class="SCst">IReset</span>) pt2
    <span class="SItr">@assert</span>(<span class="SItr">@kindof</span>(itf) == <span class="SCst">Point2</span>)
    itf = <span class="SKwd">cast</span>(<span class="SCst">IReset</span>) pt3
    <span class="SItr">@assert</span>(<span class="SItr">@kindof</span>(itf) == <span class="SCst">Point3</span>)

    <span class="SCmt">// You can retrieve the concrete data associated with an interface instance with '@dataof'</span>
    itf = <span class="SKwd">cast</span>(<span class="SCst">IReset</span>) pt2
    <span class="SItr">@assert</span>(<span class="SItr">@dataof</span>(itf) == &pt2)
    itf = <span class="SKwd">cast</span>(<span class="SCst">IReset</span>) pt3
    <span class="SItr">@assert</span>(<span class="SItr">@dataof</span>(itf) == &pt3)
}</span></div>
<h3 id="_075_interface_swg_Default_implementation">Default implementation </h3>
<p>When you declare an interface, you can define a default implementation for each function. In that case, if a struct does not redefine the function, then the default implementation will be called instead. </p>
<p>Just declare a body in the interface function to have a default implementation. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">interface</span> <span class="SCst">ITest</span>
{
    <span class="SKwd">mtd</span> <span class="SFct">isImplemented</span>()-&gt;<span class="STpe">bool</span> { <span class="SLgc">return</span> <span class="SKwd">false</span>; }
}</span></div>
<p>Here we define a specific version of <span class="code-inline">isImplemented</span> for <span class="code-inline">Point2</span>, and no specific implementation for <span class="code-inline">Point3</span>. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">impl</span> <span class="SCst">ITest</span> <span class="SLgc">for</span> <span class="SCst">Point2</span>
{
    <span class="SKwd">mtd</span> <span class="SKwd">impl</span> <span class="SFct">isImplemented</span>()-&gt;<span class="STpe">bool</span> { <span class="SLgc">return</span> <span class="SKwd">true</span>; }
}

<span class="SKwd">impl</span> <span class="SCst">ITest</span> <span class="SLgc">for</span> <span class="SCst">Point3</span>
{
}</span></div>
<p>So for <span class="code-inline">Point3</span>, <span class="code-inline">isImplemented()</span> will return <span class="code-inline">false</span> because this is the default implementation. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> v2: <span class="SCst">Point2</span>
    <span class="SKwd">var</span> v3: <span class="SCst">Point3</span>

    <span class="SCmt">// 'isImplemented' has been redefined, and will return 'true' for Point2</span>
    <span class="SKwd">let</span> i2 = <span class="SKwd">cast</span>(<span class="SCst">ITest</span>) v2
    <span class="SItr">@assert</span>(i2.<span class="SFct">isImplemented</span>())

    <span class="SCmt">// 'isImplemented' is not redefined, it will return false for Point3</span>
    <span class="SKwd">let</span> i3 = <span class="SKwd">cast</span>(<span class="SCst">ITest</span>) v3
    <span class="SItr">@assert</span>(!i3.<span class="SFct">isImplemented</span>())
}</span></div>

<h2 id="_100_function_swg">Function</h2>
<h3 id="_100_function_swg__101_001_declaration_swg">Declaration</h3><h4 id="_100_function_swg__101_001_declaration_swg_Introduction_to_Function_Declarations">Introduction to Function Declarations </h4>
<p>A function declaration typically begins with the <span class="code-inline">func</span> keyword followed by the function name. </p>
<div class="code-block"><span class="SCde"><span class="SAtr">#[Swag.Overload]</span>
<span class="SKwd">func</span> <span class="SFct">toto</span>() {}</span></div>
<h4 id="_100_function_swg__101_001_declaration_swg_Returning_Values_from_Functions">Returning Values from Functions </h4>
<p>If the function needs to return a value, use <span class="code-inline">-&gt;</span> followed by the return type. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">toto1</span>() -&gt; <span class="STpe">s32</span>
{
    <span class="SLgc">return</span> <span class="SNum">0</span>
}</span></div>
<h4 id="_100_function_swg__101_001_declaration_swg_Inferring_Return_Types">Inferring Return Types </h4>
<p>For simple expressions, the return type can be inferred by using <span class="code-inline">=&gt;</span> instead of <span class="code-inline">-&gt;</span>. In this example, the return type is inferred as <span class="code-inline">s32</span>. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">sum</span>(x, y: <span class="STpe">s32</span>) =&gt; x + y</span></div>
<h4 id="_100_function_swg__101_001_declaration_swg_Shorter_Syntax_for_Functions_Without_Return_Values">Shorter Syntax for Functions Without Return Values </h4>
<p>For functions that do not return a value, a shorter syntax can be used. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">print</span>(val: <span class="STpe">string</span>) = <span class="SItr">@print</span>(val)</span></div>
<h4 id="_100_function_swg__101_001_declaration_swg_Defining_Parameters_in_Functions">Defining Parameters in Functions </h4>
<p>Parameters are defined after the function name, within parentheses. In this example, we declare two parameters, <span class="code-inline">x</span> and <span class="code-inline">y</span> of type <span class="code-inline">s32</span>, and an additional parameter of type <span class="code-inline">f32</span>. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">sum1</span>(x, y: <span class="STpe">s32</span>, unused: <span class="STpe">f32</span>) -&gt; <span class="STpe">s32</span>
{
    <span class="SLgc">return</span> x + y
}</span></div>
<h4 id="_100_function_swg__101_001_declaration_swg_Using_Default_Parameter_Values">Using Default Parameter Values </h4>
<p>Parameters can have default values. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">sum2</span>(x, y: <span class="STpe">s32</span>, unused: <span class="STpe">f32</span> = <span class="SNum">666</span>) -&gt; <span class="STpe">s32</span>
{
    <span class="SLgc">return</span> x + y
}</span></div>
<h4 id="_100_function_swg__101_001_declaration_swg_Inferred_Parameter_Types">Inferred Parameter Types </h4>
<p>If a parameter has a default value, its type can be inferred. Here, <span class="code-inline">x</span> and <span class="code-inline">y</span> are inferred to be of type <span class="code-inline">f32</span> because <span class="code-inline">0.0</span> is a 32-bit floating-point literal. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">sum3</span>(x, y = <span class="SNum">0.0</span>)
{
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(x) == <span class="STpe">f32</span>
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(y) == <span class="STpe">f32</span>
}</span></div>
<h4 id="_100_function_swg__101_001_declaration_swg_Overloading_Functions">Overloading Functions </h4>
<p>Functions can be overloaded to handle different parameter types or counts. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">enum</span> <span class="SCst">Values</span> { <span class="SCst">A</span>, <span class="SCst">B</span> }

<span class="SAtr">#[Swag.Overload]</span>
<span class="SKwd">func</span> <span class="SFct">toto</span>(x: <span class="STpe">s32</span>, y = <span class="SCst">Values</span>.<span class="SCst">A</span>)
{
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(y) == <span class="SCst">Values</span>
}</span></div>
<h4 id="_100_function_swg__101_001_declaration_swg_Nested_Functions">Nested Functions </h4>
<p>Functions can be nested inside other functions, providing a way to organize code within a specific scope. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SCmt">// These are not closures but functions within a sub-scope.</span>
    <span class="SKwd">func</span> <span class="SFct">sub</span>(x, y: <span class="STpe">s32</span>) =&gt; x - y

    <span class="SCmt">// Simple function call.</span>
    <span class="SKwd">let</span> x = <span class="SFct">sub</span>(<span class="SNum">5</span>, <span class="SNum">2</span>)
    <span class="SItr">@assert</span>(x == <span class="SNum">3</span>)
}</span></div>
<h4 id="_100_function_swg__101_001_declaration_swg_Named_Parameters_and_Parameter_Order">Named Parameters and Parameter Order </h4>
<p>You can name parameters and call functions without respecting the parameter order. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">func</span> <span class="SFct">sub</span>(x, y: <span class="STpe">s32</span>) =&gt; x - y

    {
        <span class="SKwd">let</span> x1 = <span class="SFct">sub</span>(x: <span class="SNum">5</span>, y: <span class="SNum">2</span>)
        <span class="SItr">@assert</span>(x1 == <span class="SNum">3</span>)
        <span class="SKwd">let</span> x2 = <span class="SFct">sub</span>(y: <span class="SNum">5</span>, x: <span class="SNum">2</span>)
        <span class="SItr">@assert</span>(x2 == -<span class="SNum">3</span>)
    }

    {
        <span class="SKwd">func</span> <span class="SFct">returnMe</span>(x, y: <span class="STpe">s32</span> = <span class="SNum">0</span>) =&gt; x + y * <span class="SNum">2</span>
        <span class="SItr">@assert</span>(<span class="SFct">returnMe</span>(x: <span class="SNum">10</span>) == <span class="SNum">10</span>)
        <span class="SItr">@assert</span>(<span class="SFct">returnMe</span>(y: <span class="SNum">10</span>) == <span class="SNum">20</span>)
    }
}</span></div>
<h4 id="_100_function_swg__101_001_declaration_swg_Multiple_Return_Values">Multiple Return Values </h4>
<p>Functions in Swag can return multiple values, often using an anonymous struct or tuple. </p>
<h5 id="_100_function_swg__101_001_declaration_swg_Multiple_Return_Values_Returning_Multiple_Values_with_Anonymous_Structs">Returning Multiple Values with Anonymous Structs </h5>
<p>An <b>anonymous struct</b> can be used to return multiple values from a function. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">func</span> <span class="SFct">myFunction</span>() -&gt; { x, y: <span class="STpe">f32</span> }
    {
        <span class="SLgc">return</span> {<span class="SNum">1.0</span>, <span class="SNum">2.0</span>}
    }

    <span class="SKwd">var</span> result = <span class="SFct">myFunction</span>()
    <span class="SItr">@assert</span>(result.item0 == <span class="SNum">1.0</span>)
    <span class="SItr">@assert</span>(result.item1 == <span class="SNum">2.0</span>)

    <span class="SKwd">let</span> (x, y) = <span class="SFct">myFunction</span>()
    <span class="SItr">@assert</span>(x == <span class="SNum">1.0</span>)
    <span class="SItr">@assert</span>(y == <span class="SNum">2.0</span>)

    <span class="SKwd">let</span> (z, w) = <span class="SFct">myFunction</span>()
    <span class="SItr">@assert</span>(z == <span class="SNum">1.0</span>)
    <span class="SItr">@assert</span>(w == <span class="SNum">2.0</span>)
}</span></div>
<h5 id="_100_function_swg__101_001_declaration_swg_Multiple_Return_Values_Using_`retval`_to_Structure_Return_Statements">Using <span class="code-inline">retval</span> to Structure Return Statements </h5>
<p>You can also use the <span class="code-inline">retval</span> type, which is an alias for the function's return type, to structure your return statements. This allows for more flexibility in complex return scenarios. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">func</span> <span class="SFct">returns2</span>() -&gt; { x: <span class="STpe">s32</span>, y: <span class="STpe">s32</span> }
    {
        <span class="SCmt">// You can return a tuple literal as long as the types match.</span>
        <span class="SLgc">if</span> <span class="SKwd">false</span>:
            <span class="SLgc">return</span> {<span class="SNum">1</span>, <span class="SNum">2</span>}

        <span class="SCmt">// Alternatively, use the special type 'retval' as a type alias to the function's return type.</span>
        <span class="SKwd">using</span> <span class="SKwd">var</span> result: <span class="SKwd">retval</span>
        x = <span class="SNum">1</span>
        y = <span class="SNum">2</span>
        <span class="SLgc">return</span> result
    }

    <span class="SCmt">// You can access the struct fields by name or using 'item?'.</span>
    <span class="SKwd">var</span> result = <span class="SFct">returns2</span>()
    <span class="SItr">@assert</span>(result.x == <span class="SNum">1</span>)
    <span class="SItr">@assert</span>(result.item0 == <span class="SNum">1</span>)
    <span class="SItr">@assert</span>(result.y == <span class="SNum">2</span>)
    <span class="SItr">@assert</span>(result.item1 == <span class="SNum">2</span>)

    <span class="SCmt">// You can also deconstruct the returned struct.</span>
    <span class="SKwd">let</span> (x, y) = <span class="SFct">returns2</span>()
    <span class="SItr">@assert</span>(x == <span class="SNum">1</span>)
    <span class="SItr">@assert</span>(y == <span class="SNum">2</span>)
}</span></div>

<h3 id="_100_function_swg__102_002_lambda_swg">Lambda</h3><h4 id="_100_function_swg__102_002_lambda_swg_Introduction_to_Lambdas_in_Swag">Introduction to Lambdas in Swag </h4>
<p>A lambda in Swag is essentially a <b>pointer to a function</b>. This allows functions to be stored in variables, passed as arguments, or returned from other functions. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">func</span> <span class="SFct">myFunction0</span>() {}
    <span class="SKwd">func</span> <span class="SFct">myFunction1</span>(x: <span class="STpe">s32</span>) =&gt; x * x

    <span class="SCmt">// 'ptr0' is a pointer to a function that takes no parameters and returns nothing.</span>
    <span class="SKwd">let</span> ptr0: <span class="SKwd">func</span>() = &myFunction0
    <span class="SFct">ptr0</span>()  <span class="SCmt">// Call the function through the pointer</span>

    <span class="SCmt">// The type of 'ptr1' is inferred from 'myFunction1'.</span>
    <span class="SKwd">let</span> ptr1 = &myFunction1
    <span class="SItr">@assert</span>(<span class="SFct">myFunction1</span>(<span class="SNum">2</span>) == <span class="SNum">4</span>)
    <span class="SItr">@assert</span>(<span class="SFct">ptr1</span>(<span class="SNum">2</span>) == <span class="SNum">4</span>)  <span class="SCmt">// Call the function using the pointer</span>
}</span></div>
<h4 id="_100_function_swg__102_002_lambda_swg_Null_Lambdas">Null Lambdas </h4>
<p>A lambda can also be null, which means it does not point to any function. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> lambda: <span class="SKwd">func</span>() -&gt; <span class="STpe">bool</span>
    <span class="SItr">@assert</span>(lambda == <span class="SKwd">null</span>)  <span class="SCmt">// Confirm that the lambda is initially null</span>
}</span></div>
<h4 id="_100_function_swg__102_002_lambda_swg_Using_Lambdas_as_Function_Parameters">Using Lambdas as Function Parameters </h4>
<p>Lambdas can be used as function parameter types, allowing functions to take other functions as arguments. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">typealias</span> <span class="SCst">Callback</span> = <span class="SKwd">func</span>(<span class="STpe">s32</span>) -&gt; <span class="STpe">s32</span>
    <span class="SKwd">func</span> <span class="SFct">toDo</span>(value: <span class="STpe">s32</span>, ptr: <span class="SCst">Callback</span>) -&gt; <span class="STpe">s32</span> =&gt; <span class="SFct">ptr</span>(value)  <span class="SCmt">// Execute the lambda with the given value</span>

    <span class="SKwd">func</span> <span class="SFct">square</span>(x: <span class="STpe">s32</span>) =&gt; x * x
    <span class="SItr">@assert</span>(<span class="SFct">toDo</span>(<span class="SNum">4</span>, &square) == <span class="SNum">16</span>)  <span class="SCmt">// Pass the square function as a callback</span>
}</span></div>
<h4 id="_100_function_swg__102_002_lambda_swg_Anonymous_Functions">Anonymous Functions </h4>
<p>You can also create <i>anonymous functions</i> (also known as function literals) directly in your code. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> cb = <span class="SKwd">func</span>(x: <span class="STpe">s32</span>) -&gt; <span class="STpe">s32</span> =&gt; x * x  <span class="SCmt">// Define an anonymous function that squares a number</span>
    <span class="SItr">@assert</span>(<span class="SFct">cb</span>(<span class="SNum">4</span>) == <span class="SNum">16</span>)
    
    <span class="SCmt">// Reassign the variable to a new anonymous function that cubes a number</span>
    cb = <span class="SKwd">func</span>(x: <span class="STpe">s32</span>) -&gt; <span class="STpe">s32</span> =&gt; x * x * x
    <span class="SItr">@assert</span>(<span class="SFct">cb</span>(<span class="SNum">4</span>) == <span class="SNum">64</span>)
}</span></div>
<h4 id="_100_function_swg__102_002_lambda_swg_Passing_Anonymous_Functions_as_Parameters">Passing Anonymous Functions as Parameters </h4>
<p>Anonymous functions can be passed directly as parameters to other functions without needing to be assigned to a variable first. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">typealias</span> <span class="SCst">Callback</span> = <span class="SKwd">func</span>(<span class="STpe">s32</span>) -&gt; <span class="STpe">s32</span>
    <span class="SKwd">func</span> <span class="SFct">toDo</span>(value: <span class="STpe">s32</span>, ptr: <span class="SCst">Callback</span>) -&gt; <span class="STpe">s32</span> =&gt; <span class="SFct">ptr</span>(value)

    <span class="SCmt">// Passing anonymous functions as arguments</span>
    <span class="SItr">@assert</span>(<span class="SFct">toDo</span>(<span class="SNum">4</span>, <span class="SKwd">func</span>(x: <span class="STpe">s32</span>) =&gt; x * x) == <span class="SNum">16</span>)
    <span class="SItr">@assert</span>(<span class="SFct">toDo</span>(<span class="SNum">4</span>, <span class="SKwd">func</span>(x: <span class="STpe">s32</span>) =&gt; x + x) == <span class="SNum">8</span>)
    <span class="SItr">@assert</span>(<span class="SFct">toDo</span>(<span class="SNum">4</span>, <span class="SKwd">func</span>(x: <span class="STpe">s32</span>) -&gt; <span class="STpe">s32</span> { <span class="SLgc">return</span> x - x; }) == <span class="SNum">0</span>)
}</span></div>
<h4 id="_100_function_swg__102_002_lambda_swg_Inferred_Parameter_Types_in_Anonymous_Functions">Inferred Parameter Types in Anonymous Functions </h4>
<p>Parameter types in anonymous functions can be inferred, allowing for cleaner and more concise code. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">typealias</span> <span class="SCst">Callback</span> = <span class="SKwd">func</span>(<span class="STpe">s32</span>) -&gt; <span class="STpe">s32</span>
    <span class="SKwd">func</span> <span class="SFct">toDo</span>(value: <span class="STpe">s32</span>, ptr: <span class="SCst">Callback</span>) -&gt; <span class="STpe">s32</span> =&gt; <span class="SFct">ptr</span>(value)

    <span class="SCmt">// The types of 'x' are inferred from the context</span>
    <span class="SItr">@assert</span>(<span class="SFct">toDo</span>(<span class="SNum">4</span>, <span class="SKwd">func</span>(x) =&gt; x * x) == <span class="SNum">16</span>)
    <span class="SItr">@assert</span>(<span class="SFct">toDo</span>(<span class="SNum">4</span>, <span class="SKwd">func</span>(x) =&gt; x + x) == <span class="SNum">8</span>)
    <span class="SItr">@assert</span>(<span class="SFct">toDo</span>(<span class="SNum">4</span>, <span class="SKwd">func</span>(x) { <span class="SLgc">return</span> x - x; }) == <span class="SNum">0</span>)
}</span></div>
<h4 id="_100_function_swg__102_002_lambda_swg_Omitting_Types_When_Assigning_Lambdas">Omitting Types When Assigning Lambdas </h4>
<p>When assigning a lambda to a variable, the types of parameters and the return type can also be omitted, as they will be inferred from the variable's type. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> fct: <span class="SKwd">func</span>(<span class="STpe">s32</span>, <span class="STpe">s32</span>) -&gt; <span class="STpe">bool</span>

    <span class="SCmt">// Assign a lambda with inferred parameter types</span>
    fct = <span class="SKwd">func</span>(x, y) =&gt; x == y
    <span class="SItr">@assert</span>(<span class="SFct">fct</span>(<span class="SNum">10</span>, <span class="SNum">10</span>))

    <span class="SCmt">// Assign a lambda with a block body</span>
    fct = <span class="SKwd">func</span>(x, y) { <span class="SLgc">return</span> x != y; }
    <span class="SItr">@assert</span>(<span class="SFct">fct</span>(<span class="SNum">20</span>, <span class="SNum">120</span>))
}</span></div>
<h4 id="_100_function_swg__102_002_lambda_swg_Lambdas_with_Default_Parameter_Values">Lambdas with Default Parameter Values </h4>
<p>Lambdas can have default parameter values, making them flexible and easy to use in various contexts. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    {
        <span class="SCmt">// Lambda with a default parameter value</span>
        <span class="SKwd">let</span> x = <span class="SKwd">func</span>(val = <span class="SKwd">true</span>)
        {
            <span class="SItr">@assert</span>(val == <span class="SKwd">true</span>)
        }

        <span class="SFct">x</span>()  <span class="SCmt">// Call the lambda without arguments, using the default value</span>
    }

    {
        <span class="SKwd">var</span> x: <span class="SKwd">func</span>(val: <span class="STpe">bool</span> = <span class="SKwd">true</span>)

        <span class="SCmt">// Lambda where the parameter type is inferred, and the default value is used</span>
        x = <span class="SKwd">func</span>(val)
        {
            <span class="SItr">@assert</span>(val == <span class="SKwd">true</span>)
        }

        <span class="SFct">x</span>()       <span class="SCmt">// Call with the default value</span>
        <span class="SFct">x</span>(<span class="SKwd">true</span>)   <span class="SCmt">// Explicitly pass the value</span>
    }
}</span></div>

<h3 id="_100_function_swg__103_003_closure_swg">Closure</h3><h4 id="_100_function_swg__103_003_closure_swg_Introduction_to_Closures_in_Swag">Introduction to Closures in Swag </h4>
<p>Swag supports a limited implementation of the <span class="code-inline">closure</span> concept. A closure allows you to capture variables from its surrounding scope. Currently, Swag allows capturing up to 48 bytes, ensuring no hidden allocations. However, only simple variables (without custom behaviors like <span class="code-inline">opDrop</span>, <span class="code-inline">opPostCopy</span>, or <span class="code-inline">opPostMove</span>) can be captured. </p>
<h4 id="_100_function_swg__103_003_closure_swg_Declaring_a_Closure">Declaring a Closure </h4>
<p>A closure is declared similarly to a lambda, with the captured variables specified between <span class="code-inline">|...|</span> before the function parameters. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">let</span> a = <span class="SNum">125</span>
    <span class="SKwd">let</span> b = <span class="SNum">521</span>

    <span class="SCmt">// Capture 'a' and 'b' by value, meaning copies of these variables are captured.</span>
    <span class="SKwd">let</span> fct: <span class="SKwd">closure</span>() = <span class="SKwd">closure</span>|a, b|()
    {
        <span class="SCmt">// Inside the closure, the captured values 'a' and 'b' are accessible.</span>
        <span class="SItr">@assert</span>(a == <span class="SNum">125</span>)
        <span class="SItr">@assert</span>(b == <span class="SNum">521</span>)
    }

    <span class="SCmt">// Invoke the closure.</span>
    <span class="SFct">fct</span>()
}</span></div>
<h4 id="_100_function_swg__103_003_closure_swg_Capturing_Variables_by_Reference">Capturing Variables by Reference </h4>
<p>Variables can also be captured by reference using <span class="code-inline">&</span>. By default, variables are captured by value. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> a = <span class="SNum">125</span>

    <span class="SCmt">// Capture 'a' by reference, meaning changes to 'a' inside the closure will affect 'a' outside the closure as well.</span>
    <span class="SKwd">let</span> fct: <span class="SKwd">closure</span>() = <span class="SKwd">closure</span>|&a|()
    {
        <span class="SCmt">// Modify the captured variable 'a' by incrementing it.</span>
        a += <span class="SNum">1</span>
    }

    <span class="SCmt">// Invoke the closure, which increments 'a' by 1.</span>
    <span class="SFct">fct</span>()
    <span class="SItr">@assert</span>(a == <span class="SNum">126</span>)  <span class="SCmt">// Check that 'a' has been incremented to 126.</span>

    <span class="SCmt">// Invoke the closure again, further incrementing 'a'.</span>
    <span class="SFct">fct</span>()
    <span class="SItr">@assert</span>(a == <span class="SNum">127</span>)  <span class="SCmt">// Check that 'a' has been incremented to 127.</span>
}</span></div>
<h4 id="_100_function_swg__103_003_closure_swg_Assigning_Lambdas_to_Closure_Variables">Assigning Lambdas to Closure Variables </h4>
<p>A closure variable can also hold a standard lambda (without capture). This provides flexibility in assigning different types of functions to the same variable. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> fct: <span class="SKwd">closure</span>(<span class="STpe">s32</span>, <span class="STpe">s32</span>) -&gt; <span class="STpe">s32</span>

    <span class="SCmt">// Assign a simple lambda that adds two integers to the closure variable 'fct'.</span>
    fct = <span class="SKwd">func</span>(x, y) =&gt; x + y

    <span class="SCmt">// Test the lambda by passing values 1 and 2, expecting the result to be 3.</span>
    <span class="SItr">@assert</span>(<span class="SFct">fct</span>(<span class="SNum">1</span>, <span class="SNum">2</span>) == <span class="SNum">3</span>)
}</span></div>
<h4 id="_100_function_swg__103_003_closure_swg_Capturing_Complex_Types">Capturing Complex Types </h4>
<p>You can capture arrays, structs, slices, etc., as long as they fit within the maximum capture size and the struct is a Plain Old Data (POD) type. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> x = [<span class="SNum">1</span>, <span class="SNum">2</span>, <span class="SNum">3</span>]  <span class="SCmt">// An array of integers.</span>

    <span class="SKwd">var</span> fct: <span class="SKwd">closure</span>(<span class="STpe">s32</span>) -&gt; <span class="STpe">s32</span>

    <span class="SCmt">// Capture the array 'x' by value (a copy of the array is made).</span>
    fct = <span class="SKwd">closure</span>|x|(toAdd)
    {
        <span class="SKwd">var</span> res = <span class="SNum">0</span>
        <span class="SCmt">// Iterate over the captured array 'x' and sum its elements.</span>
        <span class="SLgc">visit</span> v <span class="SLgc">in</span> x:
            res += v
        <span class="SCmt">// Add the 'toAdd' parameter to the sum.</span>
        res += toAdd
        <span class="SLgc">return</span> res
    }

    <span class="SCmt">// Invoke the closure with the value 4, expecting the result to be the sum of 1+2+3+4.</span>
    <span class="SKwd">let</span> result = <span class="SFct">fct</span>(<span class="SNum">4</span>)
    <span class="SItr">@assert</span>(result == <span class="SNum">1</span> + <span class="SNum">2</span> + <span class="SNum">3</span> + <span class="SNum">4</span>)
}</span></div>
<h4 id="_100_function_swg__103_003_closure_swg_Modifying_Captured_Variables">Modifying Captured Variables </h4>
<p>Captured variables are mutable and part of the closure, allowing you to modify them. This enables the creation of stateful functions. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SCmt">// A function that returns a closure, which increments a captured variable 'x' each time it's called.</span>
    <span class="SKwd">func</span> <span class="SFct">getInc</span>() -&gt; <span class="SKwd">closure</span>() -&gt; <span class="STpe">s32</span>
    {
        <span class="SKwd">let</span> x = <span class="SNum">10</span>  <span class="SCmt">// Initialize 'x' with 10.</span>

        <span class="SCmt">// Return a closure that captures 'x' by value.</span>
        <span class="SLgc">return</span> <span class="SKwd">closure</span>|x|() -&gt; <span class="STpe">s32</span>
        {
            <span class="SCmt">// Increment the captured 'x' and return its new value.</span>
            x += <span class="SNum">1</span>
            <span class="SLgc">return</span> x
        }
    }

    <span class="SCmt">// Obtain the closure returned by 'getInc'.</span>
    <span class="SKwd">let</span> fct = <span class="SFct">getInc</span>()

    <span class="SCmt">// Invoke the closure multiple times, verifying that 'x' increments correctly.</span>
    <span class="SItr">@assert</span>(<span class="SFct">fct</span>() == <span class="SNum">11</span>)  <span class="SCmt">// First call, 'x' becomes 11.</span>
    <span class="SItr">@assert</span>(<span class="SFct">fct</span>() == <span class="SNum">12</span>)  <span class="SCmt">// Second call, 'x' becomes 12.</span>
    <span class="SItr">@assert</span>(<span class="SFct">fct</span>() == <span class="SNum">13</span>)  <span class="SCmt">// Third call, 'x' becomes 13.</span>
}</span></div>

<h3 id="_100_function_swg__104_004_mixin_swg">Mixin</h3><h4 id="_100_function_swg__104_004_mixin_swg_Introduction_to_Swag_Mixins">Introduction to Swag Mixins </h4>
<p>A mixin in Swag is declared similarly to a function but with the attribute <span class="code-inline">#[Swag.Mixin]</span>.  Mixins allow injecting code into the caller's scope, manipulating variables or executing code as if it were part of that scope. This documentation provides an overview of Swag Mixins with various examples. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SAtr">#[Swag.Mixin]</span>
    <span class="SKwd">func</span> <span class="SFct">myMixin</span>() {}
}</span></div>
<h4 id="_100_function_swg__104_004_mixin_swg_Basic_Example_of_a_Mixin">Basic Example of a Mixin </h4>
<p>A mixin function can directly modify variables in the caller's scope. Below is an example where a mixin increments a variable <span class="code-inline">a</span> by 1 each time it is called. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SAtr">#[Swag.Mixin]</span>
    <span class="SKwd">func</span> <span class="SFct">myMixin</span>()
    {
        a += <span class="SNum">1</span>
    }

    <span class="SKwd">var</span> a = <span class="SNum">0</span>
    <span class="SFct">myMixin</span>() <span class="SCmt">// Equivalent to writing 'a += 1' directly in the scope</span>
    <span class="SFct">myMixin</span>() <span class="SCmt">// Again, equivalent to 'a += 1'</span>
    <span class="SItr">@assert</span>(a == <span class="SNum">2</span>)  <span class="SCmt">// Verifies that 'a' has been incremented twice</span>
}</span></div>
<h4 id="_100_function_swg__104_004_mixin_swg_Mixins_with_Parameters">Mixins with Parameters </h4>
<p>Mixins behave like functions, so you can define parameters, assign default values, and even return values. This example shows a mixin with an <span class="code-inline">increment</span> parameter that defaults to 1. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SAtr">#[Swag.Mixin]</span>
    <span class="SKwd">func</span> <span class="SFct">myMixin</span>(increment: <span class="STpe">s32</span> = <span class="SNum">1</span>)
    {
        a += increment
    }

    <span class="SKwd">var</span> a = <span class="SNum">0</span>
    <span class="SFct">myMixin</span>()    <span class="SCmt">// Equivalent to 'a += 1', using the default value</span>
    <span class="SFct">myMixin</span>(<span class="SNum">2</span>)   <span class="SCmt">// Equivalent to 'a += 2', using the passed parameter</span>
    <span class="SItr">@assert</span>(a == <span class="SNum">3</span>)  <span class="SCmt">// Verifies that 'a' has been incremented by 1 and 2</span>
}</span></div>
<h4 id="_100_function_swg__104_004_mixin_swg_Mixins_with_Code_Blocks">Mixins with Code Blocks </h4>
<p>A mixin can accept a special parameter of type <span class="code-inline">code</span>, which represents a Swag code block defined at the call site.  The mixin can execute this code block multiple times using the <span class="code-inline">#mixin</span> keyword. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SAtr">#[Swag.Mixin]</span>
    <span class="SKwd">func</span> <span class="SFct">doItTwice</span>(what: <span class="STpe">code</span>)
    {
        <span class="SCmt">// The mixin can insert the passed code block multiple times using '#mixin'.</span>
        <span class="SCmp">#mixin</span> what
        <span class="SCmp">#mixin</span> what
    }

    <span class="SKwd">var</span> a = <span class="SNum">0</span>

    <span class="SCmt">// Use '#code' to define a code block that will be executed twice by the mixin.</span>
    <span class="SFct">doItTwice</span>(<span class="SCmp">#code</span> { a += <span class="SNum">1</span>; })
    <span class="SItr">@assert</span>(a == <span class="SNum">2</span>)  <span class="SCmt">// Verifies that 'a' was incremented twice</span>
}</span></div>
<h4 id="_100_function_swg__104_004_mixin_swg_Mixing_Code_Blocks_in_Separate_Statements">Mixing Code Blocks in Separate Statements </h4>
<p>When the last parameter of a mixin is of type <span class="code-inline">code</span>, the code can be declared in a separate statement after the mixin call. This provides a more natural syntax for passing code blocks. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SAtr">#[Swag.Mixin]</span>
    <span class="SKwd">func</span> <span class="SFct">doItTwice</span>(value: <span class="STpe">s32</span>, what: <span class="STpe">code</span>)
    {
        <span class="SCmp">#mixin</span> what
        <span class="SCmp">#mixin</span> what
    }

    <span class="SKwd">var</span> a = <span class="SNum">0</span>

    <span class="SCmt">// Pass a code block using the `#code` syntax.</span>
    <span class="SFct">doItTwice</span>(<span class="SNum">4</span>, <span class="SCmp">#code</span> { a += value; })

    <span class="SCmt">// Alternatively, you can pass the code block directly in a more natural syntax.</span>
    <span class="SFct">doItTwice</span>(<span class="SNum">2</span>)
    {
        a += value
    }

    <span class="SItr">@assert</span>(a == <span class="SNum">12</span>)  <span class="SCmt">// Verifies that 'a' was incremented as expected</span>
}</span></div>
<h4 id="_100_function_swg__104_004_mixin_swg_Creating_Aliases_with_Mixins">Creating Aliases with Mixins </h4>
<p>You can use the special name <span class="code-inline">#alias</span> to create a named alias for an identifier.  This allows flexible manipulation of variables through mixins. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SAtr">#[Swag.Mixin]</span>
    <span class="SKwd">func</span> <span class="SFct">inc10</span>()
    {
        <span class="SItr">#alias0</span> += <span class="SNum">10</span>  <span class="SCmt">// Increment the aliased variable by 10</span>
    }

    <span class="SKwd">var</span> a, b = <span class="SNum">0</span>
    <span class="SFct">inc10</span>(|a|)  <span class="SCmt">// Use 'a' as the alias</span>
    <span class="SFct">inc10</span>(|b|)  <span class="SCmt">// Use 'b' as the alias</span>
    <span class="SItr">@assert</span>(a == b <span class="SLgc">and</span> b == <span class="SNum">10</span>)  <span class="SCmt">// Verifies that both 'a' and 'b' were incremented by 10</span>
}

<span class="SFct">#test</span>
{
    <span class="SAtr">#[Swag.Mixin]</span>
    <span class="SKwd">func</span> <span class="SFct">setVar</span>(value: <span class="STpe">s32</span>)
    {
        <span class="SKwd">let</span> <span class="SItr">#alias0</span> = value  <span class="SCmt">// Set the aliased variable to 'value'</span>
    }

    <span class="SFct">setVar</span>(|a| <span class="SNum">10</span>)  <span class="SCmt">// Set 'a' to 10</span>
    <span class="SFct">setVar</span>(|b| <span class="SNum">20</span>)  <span class="SCmt">// Set 'b' to 20</span>
    <span class="SItr">@assert</span>(a == <span class="SNum">10</span>)  <span class="SCmt">// Verifies that 'a' was set to 10</span>
    <span class="SItr">@assert</span>(b == <span class="SNum">20</span>)  <span class="SCmt">// Verifies that 'b' was set to 20</span>
    <span class="SFct">setVar</span>(<span class="SNum">30</span>)  <span class="SCmt">// No alias provided, so default alias '#alias0' is used</span>
    <span class="SItr">@assert</span>(<span class="SItr">#alias0</span> == <span class="SNum">30</span>)  <span class="SCmt">// Verifies that '#alias0' was set to 30</span>
}</span></div>
<h4 id="_100_function_swg__104_004_mixin_swg_Unique_Variable_Names_with_`#mix?`">Unique Variable Names with <span class="code-inline">#mix?</span> </h4>
<p>Mixins can declare special variables named <span class="code-inline">#mix?</span>.  These variables receive a unique name each time the mixin is invoked, preventing naming conflicts and allowing multiple mixin invocations in the same scope. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> total: <span class="STpe">s32</span>

    <span class="SAtr">#[Swag.Mixin]</span>
    <span class="SKwd">func</span> <span class="SFct">toScope</span>()
    {
        <span class="SKwd">var</span> <span class="SItr">#mix0</span>: <span class="STpe">s32</span> = <span class="SNum">1</span>  <span class="SCmt">// Declare a unique variable named '#mix0'</span>
        total += <span class="SItr">#mix0</span>       <span class="SCmt">// Add the value of '#mix0' to 'total'</span>
    }

    <span class="SFct">toScope</span>()  <span class="SCmt">// First invocation of the mixin</span>
    <span class="SFct">toScope</span>()  <span class="SCmt">// Second invocation</span>
    <span class="SFct">toScope</span>()  <span class="SCmt">// Third invocation</span>

    <span class="SItr">@assert</span>(total == <span class="SNum">3</span>)  <span class="SCmt">// Verifies that 'total' is the sum of all mixin invocations</span>
}</span></div>

<h3 id="_100_function_swg__105_005_macro_swg">Macro</h3><h4 id="_100_function_swg__105_005_macro_swg_Introduction_to_Swag_Macros">Introduction to Swag Macros </h4>
<p>A macro in Swag is declared similarly to a function, but with the attribute <span class="code-inline">#[Swag.Macro]</span>. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SAtr">#[Swag.Macro]</span>
    <span class="SKwd">func</span> <span class="SFct">myMacro</span>() {}
}</span></div>
<h4 id="_100_function_swg__105_005_macro_swg_Macro_Scope">Macro Scope </h4>
<p>Unlike a mixin, a macro has its own scope. This means that variables declared within the macro do not conflict with variables in the function where the macro is used. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SAtr">#[Swag.Macro]</span>
    <span class="SKwd">func</span> <span class="SFct">myMacro</span>()
    {
        <span class="SKwd">var</span> a = <span class="SNum">666</span> <span class="SCmt">// 'a' is local to the macro's scope</span>
    }

    <span class="SKwd">let</span> a = <span class="SNum">0</span>
    <span class="SFct">myMacro</span>() <span class="SCmt">// No conflict with the 'a' declared in the outer scope</span>
    <span class="SItr">@assert</span>(a == <span class="SNum">0</span>) <span class="SCmt">// 'a' remains unchanged</span>
}</span></div>
<h4 id="_100_function_swg__105_005_macro_swg_Resolving_Identifiers_Outside_the_Macro_Scope">Resolving Identifiers Outside the Macro Scope </h4>
<p>You can force an identifier to be resolved <b>outside</b> the macro's scope using the <span class="code-inline">#up</span> keyword. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SAtr">#[Swag.Macro]</span>
    <span class="SKwd">func</span> <span class="SFct">myMacro</span>()
    {
        <span class="SCmp">#up</span> a += <span class="SNum">1</span> <span class="SCmt">// References the 'a' from the outer scope</span>
    }

    <span class="SKwd">var</span> a = <span class="SNum">0</span>
    <span class="SFct">myMacro</span>() <span class="SCmt">// Increment the outer 'a' by 1</span>
    <span class="SFct">myMacro</span>() <span class="SCmt">// Increment the outer 'a' by 1 again</span>
    <span class="SItr">@assert</span>(a == <span class="SNum">2</span>) <span class="SCmt">// Verify that 'a' has been incremented twice</span>
}</span></div>
<h4 id="_100_function_swg__105_005_macro_swg_Macros_with_Code_Parameters">Macros with Code Parameters </h4>
<p>Like mixins, macros can accept <span class="code-inline">code</span> parameters, allowing the insertion of code blocks within the macro. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SAtr">#[Swag.Macro]</span>
    <span class="SKwd">func</span> <span class="SFct">myMacro</span>(what: <span class="STpe">code</span>)
    {
        <span class="SCmp">#mixin</span> what <span class="SCmt">// Insert the provided code block</span>
    }

    <span class="SKwd">var</span> a = <span class="SNum">0</span>

    <span class="SFct">myMacro</span>(<span class="SCmp">#code</span>
    {
        <span class="SCmp">#up</span> a += <span class="SNum">1</span> <span class="SCmt">// Increment 'a' within the code block</span>
    })

    <span class="SFct">myMacro</span>()
    {
        <span class="SCmp">#up</span> a += <span class="SNum">1</span> <span class="SCmt">// Another way to pass the code block directly</span>
    }

    <span class="SItr">@assert</span>(a == <span class="SNum">2</span>) <span class="SCmt">// Verify that 'a' has been incremented twice</span>
}</span></div>
<h4 id="_100_function_swg__105_005_macro_swg_Forcing_Code_into_the_Caller’s_Scope_with_`#macro`">Forcing Code into the Caller’s Scope with <span class="code-inline">#macro</span> </h4>
<p>You can use <span class="code-inline">#macro</span> within a macro to force the following code to be inserted in the caller's scope. This means no <span class="code-inline">#up</span> is necessary to reference variables from the caller. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SAtr">#[Swag.Macro]</span>
    <span class="SKwd">func</span> <span class="SFct">myMacro</span>(what: <span class="STpe">code</span>)
    {
        <span class="SCmt">// Declare 'a' in the macro's own scope</span>
        <span class="SKwd">var</span> a = <span class="SNum">666</span>

        <span class="SCmt">// Use '#macro' to ensure the following code operates in the caller's scope</span>
        <span class="SCmp">#macro</span>
        {
            <span class="SCmt">// The code block here references the caller's 'a'</span>
            <span class="SCmp">#mixin</span> <span class="SCmp">#up</span> what
        }
    }

    <span class="SKwd">var</span> a = <span class="SNum">1</span>
    <span class="SFct">myMacro</span>()
    {
        a += <span class="SNum">2</span> <span class="SCmt">// This 'a' references the caller's 'a' due to '#macro'</span>
    }

    <span class="SItr">@assert</span>(a == <span class="SNum">3</span>) <span class="SCmt">// Verify that the caller's 'a' was incremented</span>
}</span></div>
<h4 id="_100_function_swg__105_005_macro_swg_Performance_Considerations_with_Macros">Performance Considerations with Macros </h4>
<p>Macros allow you to extend the language without using function pointers, avoiding the cost of lambda calls. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SAtr">#[Swag.Macro]</span>
    <span class="SKwd">func</span> <span class="SFct">repeat</span>(count: <span class="STpe">s32</span>, what: <span class="STpe">code</span>)
    {
        <span class="SKwd">var</span> a = <span class="SNum">0</span>
        <span class="SLgc">while</span> a &lt; count
        {
            <span class="SCmp">#macro</span>
            {
                <span class="SCmt">// 'index' references 'a' from the caller's scope</span>
                <span class="SKwd">var</span> index = <span class="SCmp">#up</span> a
                <span class="SCmp">#mixin</span> <span class="SCmp">#up</span> what
            }

            a += <span class="SNum">1</span>
        }
    }

    <span class="SKwd">var</span> a = <span class="SNum">0</span>
    <span class="SFct">repeat</span>(<span class="SNum">5</span>)
    {
        a += index <span class="SCmt">// Sum 'index' values, where 'index' is from the macro's loop</span>
    }
    <span class="SItr">@assert</span>(a == <span class="SNum">0</span> + <span class="SNum">1</span> + <span class="SNum">2</span> + <span class="SNum">3</span> + <span class="SNum">4</span>) <span class="SCmt">// Verify the sum after the first repeat</span>

    <span class="SFct">repeat</span>(<span class="SNum">3</span>)
    {
        a += index <span class="SCmt">// Continue summing with a new loop</span>
    }
    <span class="SItr">@assert</span>(a == <span class="SNum">10</span> + <span class="SNum">3</span>) <span class="SCmt">// Verify the final sum after both repeats</span>
}</span></div>
<h4 id="_100_function_swg__105_005_macro_swg_Handling_Breaks_in_User_Code_with_Macros">Handling Breaks in User Code with Macros </h4>
<p>You can use <span class="code-inline">break</span> within user code to exit from a nested loop. This can be handled by macros using scoped loop control. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SAtr">#[Swag.Macro]</span>
    <span class="SKwd">func</span> <span class="SFct">repeatSquare</span>(count: <span class="STpe">u32</span>, what: <span class="STpe">code</span>)
    {
        <span class="SCmp">#scope</span> <span class="SCst">Up</span> <span class="SCmt">// Define a scope for breaking out of nested loops</span>
        <span class="SLgc">loop</span> count
        {
            <span class="SLgc">loop</span> count
            {
                <span class="SCmp">#macro</span>
                {
                    <span class="SCmt">// Handle 'break' in user code to break out of the outer loop</span>
                    <span class="SCmp">#mixin</span> <span class="SCmp">#up</span> what { <span class="SLgc">break</span> = <span class="SLgc">break</span> <span class="SCst">Up</span>; }
                }
            }
        }
    }

    <span class="SKwd">var</span> a = <span class="SNum">0</span>
    <span class="SFct">repeatSquare</span>(<span class="SNum">5</span>)
    {
        a += <span class="SNum">1</span>
        <span class="SLgc">if</span> a == <span class="SNum">10</span>:
            <span class="SLgc">break</span> <span class="SCmt">// This 'break' exits the outermost loop due to the macro handling</span>
    }

    <span class="SItr">@assert</span>(a == <span class="SNum">10</span>) <span class="SCmt">// Verify that the loop was exited correctly after 'a' reached 10</span>
}</span></div>
<h4 id="_100_function_swg__105_005_macro_swg_Using_Aliases_in_Macros">Using Aliases in Macros </h4>
<p>In a macro, you can use special variables named <span class="code-inline">#alias&lt;num&gt;</span>, similar to mixins. These allow you to define and reference variables with specific names. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SAtr">#[Swag.Macro]</span>
    <span class="SKwd">func</span> <span class="SFct">call</span>(v: <span class="STpe">s32</span>, stmt: <span class="STpe">code</span>)
    {
        <span class="SKwd">let</span> <span class="SItr">#alias0</span> = v <span class="SCmt">// Assign 'v' to '#alias0'</span>
        <span class="SKwd">let</span> <span class="SItr">#alias1</span> = v * <span class="SNum">2</span> <span class="SCmt">// Assign 'v * 2' to '#alias1'</span>
        <span class="SCmp">#mixin</span> stmt <span class="SCmt">// Insert the provided code block</span>
    }

    <span class="SFct">call</span>(<span class="SNum">20</span>)
    {
        <span class="SItr">@assert</span>(<span class="SItr">#alias0</span> == <span class="SNum">20</span>) <span class="SCmt">// Verify '#alias0' equals 20</span>
        <span class="SItr">@assert</span>(<span class="SItr">#alias1</span> == <span class="SNum">40</span>) <span class="SCmt">// Verify '#alias1' equals 40</span>
    }

    <span class="SCmt">// The caller can rename these special variables by using the alias syntax</span>
    <span class="SFct">call</span>(|x| <span class="SNum">20</span>)
    {
        <span class="SItr">@assert</span>(x == <span class="SNum">20</span>) <span class="SCmt">// 'x' is used as an alias for '#alias0'</span>
        <span class="SItr">@assert</span>(<span class="SItr">#alias1</span> == <span class="SNum">40</span>) <span class="SCmt">// '#alias1' is still referenced as is</span>
    }

    <span class="SFct">call</span>(|x, y| <span class="SNum">20</span>)
    {
        <span class="SItr">@assert</span>(x == <span class="SNum">20</span>) <span class="SCmt">// 'x' replaces '#alias0'</span>
        <span class="SItr">@assert</span>(y == <span class="SNum">40</span>) <span class="SCmt">// 'y' replaces '#alias1'</span>
    }
}</span></div>

<h3 id="_100_function_swg__105_006_variadic_parameters_swg">Variadic parameters</h3><h4 id="_100_function_swg__105_006_variadic_parameters_swg_Introduction_to_Variadic_Functions">Introduction to Variadic Functions </h4>
<p>A function can take a variable number of arguments using <span class="code-inline">...</span>. This allows you to create functions that accept a flexible number of arguments. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">func</span> <span class="SFct">myFunction</span>(value: <span class="STpe">bool</span>, parameters: ...)
    {
        <span class="SCmt">// This function can now take any number of additional arguments after 'value'</span>
    }

    <span class="SFct">myFunction</span>(<span class="SKwd">true</span>, <span class="SNum">4</span>, <span class="SStr">"true"</span>, <span class="SNum">5.6</span>) <span class="SCmt">// Passes 4, "true", and 5.6 as additional parameters</span>
}</span></div>
<h4 id="_100_function_swg__105_006_variadic_parameters_swg_Working_with_Variadic_Parameters_as_Slices">Working with Variadic Parameters as Slices </h4>
<p>When a function takes a variadic parameter, the <span class="code-inline">parameters</span> variable is treated as a slice of type <span class="code-inline">any</span>. This allows the function to handle different types of arguments flexibly at runtime. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">func</span> <span class="SFct">myFunction</span>(parameters: ...)
    {
        <span class="SCmt">// Determine the number of parameters passed</span>
        <span class="SItr">@assert</span>(<span class="SItr">@countof</span>(parameters) == <span class="SNum">3</span>)

        <span class="SCmt">// Each parameter is initially treated as 'any' type</span>
        <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(parameters[<span class="SNum">0</span>]) == <span class="STpe">any</span>
        <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(parameters[<span class="SNum">1</span>]) == <span class="STpe">any</span>
        <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(parameters[<span class="SNum">2</span>]) == <span class="STpe">any</span>

        <span class="SCmt">// Use '@kindof' to determine the actual type of each parameter at runtime</span>
        <span class="SItr">@assert</span>(<span class="SItr">@kindof</span>(parameters[<span class="SNum">0</span>]) == <span class="STpe">s32</span>)
        <span class="SItr">@assert</span>(<span class="SItr">@kindof</span>(parameters[<span class="SNum">1</span>]) == <span class="STpe">string</span>)
        <span class="SItr">@assert</span>(<span class="SItr">@kindof</span>(parameters[<span class="SNum">2</span>]) == <span class="STpe">f32</span>)
    }

    <span class="SFct">myFunction</span>(<span class="SNum">4</span>, <span class="SStr">"true"</span>, <span class="SNum">5.6</span>) <span class="SCmt">// Passes an integer, string, and float</span>
}</span></div>
<h4 id="_100_function_swg__105_006_variadic_parameters_swg_Forcing_Variadic_Parameters_to_a_Specific_Type">Forcing Variadic Parameters to a Specific Type </h4>
<p>If all variadic parameters are of the same type, you can enforce that type using type annotations. This makes the parameters' type explicit, and they will not be treated as <span class="code-inline">any</span>. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">func</span> <span class="SFct">myFunction</span>(value: <span class="STpe">bool</span>, parameters: <span class="STpe">s32</span>...)
    {
        <span class="SCmt">// All parameters in 'parameters' must be of type 's32'</span>
        <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(parameters[<span class="SNum">0</span>]).name == <span class="SStr">"s32"</span>
        <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(parameters[<span class="SNum">1</span>]).name == <span class="SStr">"s32"</span>
        <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(parameters[<span class="SNum">2</span>]) == <span class="STpe">s32</span>
        <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(parameters[<span class="SNum">3</span>]) == <span class="STpe">s32</span>

        <span class="SCmt">// Verify that the parameters have been passed correctly</span>
        <span class="SItr">@assert</span>(parameters[<span class="SNum">0</span>] == <span class="SNum">10</span>)
        <span class="SItr">@assert</span>(parameters[<span class="SNum">1</span>] == <span class="SNum">20</span>)
        <span class="SItr">@assert</span>(parameters[<span class="SNum">2</span>] == <span class="SNum">30</span>)
        <span class="SItr">@assert</span>(parameters[<span class="SNum">3</span>] == <span class="SNum">40</span>)
    }

    <span class="SFct">myFunction</span>(<span class="SKwd">true</span>, <span class="SNum">10</span>, <span class="SNum">20</span>, <span class="SNum">30</span>, <span class="SNum">40</span>) <span class="SCmt">// Passes four integers</span>
}</span></div>
<h4 id="_100_function_swg__105_006_variadic_parameters_swg_Passing_Variadic_Parameters_Between_Functions">Passing Variadic Parameters Between Functions </h4>
<p>You can pass variadic parameters from one function to another, maintaining their types and values. This is useful when you need to delegate tasks to other functions without losing the variadic nature of the arguments. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">func</span> <span class="SCst">A</span>(params: ...)
    {
        <span class="SCmt">// Expecting a string and a boolean</span>
        <span class="SItr">@assert</span>(<span class="SItr">@countof</span>(params) == <span class="SNum">2</span>)
        <span class="SItr">@assert</span>(<span class="SItr">@kindof</span>(params[<span class="SNum">0</span>]) == <span class="STpe">string</span>)
        <span class="SItr">@assert</span>(<span class="SItr">@kindof</span>(params[<span class="SNum">1</span>]) == <span class="STpe">bool</span>)
        <span class="SItr">@assert</span>(<span class="SKwd">cast</span>(<span class="STpe">string</span>) params[<span class="SNum">0</span>] == <span class="SStr">"value"</span>)
        <span class="SItr">@assert</span>(<span class="SKwd">cast</span>(<span class="STpe">bool</span>) params[<span class="SNum">1</span>] == <span class="SKwd">true</span>)
    }

    <span class="SKwd">func</span> <span class="SCst">B</span>(params: ...)
    {
        <span class="SCmt">// Pass the variadic parameters from B to A</span>
        <span class="SCst">A</span>(params)
    }

    <span class="SCst">B</span>(<span class="SStr">"value"</span>, <span class="SKwd">true</span>) <span class="SCmt">// Passes the parameters to function A through B</span>
}</span></div>
<h4 id="_100_function_swg__105_006_variadic_parameters_swg_Spreading_Arrays_or_Slices_to_Variadic_Parameters">Spreading Arrays or Slices to Variadic Parameters </h4>
<p>You can spread the contents of an array or slice into variadic parameters using <span class="code-inline">@spread</span>. This feature is handy when you have a collection of values that you want to pass as individual arguments. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">func</span> <span class="SFct">sum</span>(params: <span class="STpe">s32</span>...)-&gt;<span class="STpe">s32</span>
    {
        <span class="SCmt">// Variadic parameters can be iterated, as they are slices</span>
        <span class="SKwd">var</span> total = <span class="SNum">0</span>
        <span class="SLgc">visit</span> v <span class="SLgc">in</span> params:
            total += v
        <span class="SLgc">return</span> total
    }

    <span class="SKwd">var</span> arr = [<span class="SNum">1</span>, <span class="SNum">2</span>, <span class="SNum">3</span>, <span class="SNum">4</span>]
    <span class="SKwd">let</span> res = <span class="SFct">sum</span>(<span class="SItr">@spread</span>(arr))     <span class="SCmt">// is equivalent to sum(1, 2, 3, 4)</span>
    <span class="SItr">@assert</span>(res == <span class="SNum">1</span> + <span class="SNum">2</span> + <span class="SNum">3</span> + <span class="SNum">4</span>)

    <span class="SKwd">let</span> res1 = <span class="SFct">sum</span>(<span class="SItr">@spread</span>(arr[<span class="SNum">1.</span>.<span class="SNum">3</span>])) <span class="SCmt">// is equivalent to sum(2, 3, 4)</span>
    <span class="SItr">@assert</span>(res1 == <span class="SNum">2</span> + <span class="SNum">3</span> + <span class="SNum">4</span>)
}</span></div>
<h4 id="_100_function_swg__105_006_variadic_parameters_swg_Advanced_Example:_Combining_Variadic_and_Non-Variadic_Parameters">Advanced Example: Combining Variadic and Non-Variadic Parameters </h4>
<p>In this example, we demonstrate how to combine fixed parameters with variadic parameters and use them together in a function. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">private</span> <span class="SKwd">func</span> <span class="SFct">print</span>()
{
    <span class="SKwd">func</span> <span class="SFct">logMessage</span>(prefix: <span class="STpe">string</span>, messages: ...)
    {
        <span class="SCmt">// Print each message with the given prefix</span>
        <span class="SLgc">visit</span> msg <span class="SLgc">in</span> messages
        {
            <span class="SItr">@print</span>(prefix, <span class="SStr">" =&gt; "</span>, <span class="SKwd">cast</span>(<span class="STpe">string</span>) msg)
        }
    }

    <span class="SFct">logMessage</span>(<span class="SStr">"Error: "</span>, <span class="SStr">"File not found"</span>, <span class="SStr">"Access denied"</span>, <span class="SStr">"Disk full"</span>)
}</span></div>
<h4 id="_100_function_swg__105_006_variadic_parameters_swg_Example:_Handling_Different_Types_in_Variadic_Parameters">Example: Handling Different Types in Variadic Parameters </h4>
<p>This example shows how to handle different types within a variadic function, such as summing integers and concatenating strings. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">func</span> <span class="SFct">processParameters</span>(params: ...)-&gt;<span class="STpe">s32</span>
    {
        <span class="SKwd">var</span> sum = <span class="SNum">0</span>
        
        <span class="SLgc">visit</span> p <span class="SLgc">in</span> params
        {
            <span class="SLgc">switch</span> <span class="SItr">@kindof</span>(p)
            {
            <span class="SLgc">case</span> <span class="STpe">s32</span>:
                sum += <span class="SNum">1</span>
            <span class="SLgc">case</span> <span class="STpe">string</span>:
                sum += <span class="SNum">10</span>
            }   
        }

        <span class="SLgc">return</span> sum    
    }

    <span class="SKwd">let</span> result = <span class="SFct">processParameters</span>(<span class="SNum">1</span>, <span class="SNum">2</span>, <span class="SStr">"Hello, "</span>, <span class="SNum">3</span>, <span class="SStr">"World!"</span>)
    <span class="SItr">@assert</span>(result == <span class="SNum">1</span> + <span class="SNum">1</span> + <span class="SNum">10</span> + <span class="SNum">1</span> + <span class="SNum">10</span>)
}</span></div>

<h3 id="_100_function_swg__106_007_ufcs_swg">Ufcs</h3><h4 id="_100_function_swg__106_007_ufcs_swg_Introduction_to_Uniform_Function_Call_Syntax_(UFCS)">Introduction to Uniform Function Call Syntax (UFCS) </h4>
<p><i>UFCS</i> stands for <i>uniform function call syntax</i>. It allows any function to be called using the <span class="code-inline">param.func()</span> form when the first parameter of <span class="code-inline">func()</span> matches the type of <span class="code-inline">param</span>. This syntax provides a way to call static functions as if they were methods on an instance. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">func</span> <span class="SFct">myFunc</span>(param: <span class="STpe">bool</span>) =&gt; param

    <span class="SKwd">let</span> b = <span class="SKwd">false</span>
    <span class="SItr">@assert</span>(<span class="SFct">myFunc</span>(b) == b.<span class="SFct">myFunc</span>()) <span class="SCmt">// Using UFCS to call 'myFunc' as if it were a method on 'b'</span>
}</span></div>
<h4 id="_100_function_swg__106_007_ufcs_swg_Static_Functions_as_Methods">Static Functions as Methods </h4>
<p>In Swag, all functions are <i>static</i>, meaning they are not bound to instances of structs or classes. However, UFCS allows these functions to be called in a method-like style. This feature is particularly useful for struct manipulation, making the code more readable and intuitive. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">struct</span> <span class="SCst">Point</span> { x, y: <span class="STpe">s32</span> }

    <span class="SKwd">func</span> <span class="SFct">set</span>(<span class="SKwd">using</span> pt: *<span class="SCst">Point</span>, value: <span class="STpe">s32</span>)
    {
        x, y = value
    }

    <span class="SKwd">var</span> pt: <span class="SCst">Point</span>

    <span class="SCmt">// Using UFCS to call 'set' as if it were a method of 'pt'</span>
    pt.<span class="SFct">set</span>(<span class="SNum">10</span>)
    <span class="SItr">@assert</span>(pt.x == <span class="SNum">10</span> <span class="SLgc">and</span> pt.y == <span class="SNum">10</span>)

    <span class="SCmt">// Normal static function call</span>
    <span class="SFct">set</span>(&pt, <span class="SNum">20</span>)
    <span class="SItr">@assert</span>(pt.x == <span class="SNum">20</span> <span class="SLgc">and</span> pt.y == <span class="SNum">20</span>)
}</span></div>
<h4 id="_100_function_swg__106_007_ufcs_swg_UFCS_with_Multiple_Parameters">UFCS with Multiple Parameters </h4>
<p>UFCS works seamlessly with functions that take multiple parameters, provided the first parameter matches the type of the instance. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">struct</span> <span class="SCst">Vector</span> { x, y: <span class="STpe">f32</span> }

    <span class="SKwd">func</span> <span class="SFct">add</span>(<span class="SKwd">using</span> vec: *<span class="SCst">Vector</span>, dx: <span class="STpe">f32</span>, dy: <span class="STpe">f32</span>)
    {
        x += dx
        y += dy
    }

    <span class="SKwd">var</span> v: <span class="SCst">Vector</span>

    <span class="SCmt">// Using UFCS to call 'add' as if it were a method of 'v'</span>
    v.<span class="SFct">add</span>(<span class="SNum">1.0</span>, <span class="SNum">2.0</span>)
    <span class="SItr">@assert</span>(v.x == <span class="SNum">1.0</span> <span class="SLgc">and</span> v.y == <span class="SNum">2.0</span>)

    <span class="SCmt">// Normal static function call</span>
    <span class="SFct">add</span>(&v, <span class="SNum">3.0</span>, <span class="SNum">4.0</span>)
    <span class="SItr">@assert</span>(v.x == <span class="SNum">4.0</span> <span class="SLgc">and</span> v.y == <span class="SNum">6.0</span>)
}</span></div>
<h4 id="_100_function_swg__106_007_ufcs_swg_UFCS_and_Function_Overloading">UFCS and Function Overloading </h4>
<p>UFCS supports function overloading, where the appropriate function is chosen based on the types of the parameters provided. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">struct</span> <span class="SCst">Complex</span> { real, imag: <span class="STpe">f32</span> }

    <span class="SAtr">#[Swag.Overload]</span>
    <span class="SKwd">func</span> <span class="SFct">multiply</span>(c: *<span class="SCst">Complex</span>, scalar: <span class="STpe">f32</span>)
    {
        c.real *= scalar
        c.imag *= scalar
    }

    <span class="SAtr">#[Swag.Overload]</span>
    <span class="SKwd">func</span> <span class="SFct">multiply</span>(<span class="SKwd">using</span> c: *<span class="SCst">Complex</span>, other: *<span class="SCst">Complex</span>)
    {
        real = (real * other.real) - (imag * other.imag)
        imag = (real * other.imag) + (imag * other.real)
    }

    <span class="SKwd">var</span> c1 = <span class="SCst">Complex</span>{<span class="SNum">2.0</span>, <span class="SNum">3.0</span>}
    <span class="SKwd">var</span> c2 = <span class="SCst">Complex</span>{<span class="SNum">4.0</span>, <span class="SNum">5.0</span>}

    <span class="SCmt">// Using UFCS to multiply by a scalar</span>
    c1.<span class="SFct">multiply</span>(<span class="SNum">2.0</span>)
    <span class="SItr">@assert</span>(c1.real == <span class="SNum">4.0</span> <span class="SLgc">and</span> c1.imag == <span class="SNum">6.0</span>)

    <span class="SCmt">// Using UFCS to multiply by another Complex number</span>
    c1.<span class="SFct">multiply</span>(&c2)
    <span class="SItr">@assert</span>(c1.real == -<span class="SNum">14.0</span> <span class="SLgc">and</span> c1.imag == -<span class="SNum">46.0</span>)
}</span></div>

<h3 id="_100_function_swg__107_008_constexpr_swg">Constexpr</h3><p>A function marked with <span class="code-inline">Swag.ConstExpr</span> can be executed by the compiler if it can. </p>
<div class="code-block"><span class="SCde"><span class="SAtr">#[Swag.ConstExpr]</span>
<span class="SKwd">func</span> <span class="SFct">sum</span>(x, y: <span class="STpe">f32</span>) =&gt; x + y</span></div>
<p>Here <span class="code-inline">G</span> will be baked to 3 by the compiler. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">const</span> <span class="SCst">G</span> = <span class="SFct">sum</span>(<span class="SNum">1</span>, <span class="SNum">2</span>)
<span class="SCmp">#assert</span> <span class="SCst">G</span> == <span class="SNum">3</span></span></div>
<p>If a function is not <span class="code-inline">ConstExpr</span>, you can force the compile time call with <span class="code-inline">#run</span>. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">mul</span>(x, y: <span class="STpe">f32</span>) =&gt; x * y
<span class="SKwd">const</span> <span class="SCst">G1</span> = <span class="SFct">#run</span> <span class="SFct">mul</span>(<span class="SNum">3</span>, <span class="SNum">6</span>)
<span class="SCmp">#assert</span> <span class="SCst">G1</span> == <span class="SNum">18</span></span></div>

<h3 id="_100_function_swg__108_009_function_overloading_swg">Function overloading</h3><p>Functions can have the same names as long as their parameters are different. Such function must be marked with <span class="code-inline">#[Swag.Overload]</span>. </p>
<div class="code-block"><span class="SCde"><span class="SAtr">#[Swag.ConstExpr, Swag.Overload]</span>
{
    <span class="SKwd">func</span> <span class="SFct">sum</span>(x, y: <span class="STpe">s32</span>)    =&gt; x + y
    <span class="SKwd">func</span> <span class="SFct">sum</span>(x, y, z: <span class="STpe">s32</span>) =&gt; x + y + z
}

<span class="SCmp">#assert</span> <span class="SFct">sum</span>(<span class="SNum">1</span>, <span class="SNum">2</span>) == <span class="SNum">3</span>
<span class="SCmp">#assert</span> <span class="SFct">sum</span>(<span class="SNum">1</span>, <span class="SNum">2</span>, <span class="SNum">3</span>) == <span class="SNum">6</span></span></div>

<h3 id="_100_function_swg__109_010_discard_swg">Discard</h3><p>By default, you must always use the returned value of a function, otherwise the compiler will generate an error. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">func</span> <span class="SFct">sum</span>(x, y: <span class="STpe">s32</span>) =&gt; x + y

    <span class="SCmt">// This would generated an error, because the return value of 'sum' is not used</span>
    <span class="SCmt">// sum(2, 3)</span>

    <span class="SCmt">// To force the return value to be ignored, you can use 'discard' at the call site</span>
    <span class="SKwd">discard</span> <span class="SFct">sum</span>(<span class="SNum">2</span>, <span class="SNum">3</span>)
}</span></div>
<p>If a function authorizes the caller to not use its return value, because it's not that important, it can be marked with <span class="code-inline">Swag.Discardable</span>. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SAtr">#[Swag.Discardable]</span>
    <span class="SKwd">func</span> <span class="SFct">mul</span>(x, y: <span class="STpe">s32</span>)-&gt;<span class="STpe">s32</span> =&gt; x * y

    <span class="SCmt">// This is fine to ignore the return value of 'mul' (even if strange)</span>
    <span class="SFct">mul</span>(<span class="SNum">2</span>, <span class="SNum">4</span>)
}</span></div>

<h3 id="_100_function_swg__110_011_retval_swg">Retval</h3><p>Inside a function, you can use the <span class="code-inline">retval</span> type which is an alias to the function return type. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">func</span> <span class="SFct">toto</span>()-&gt;<span class="STpe">s32</span>
    {
        <span class="SKwd">var</span> result: <span class="SKwd">retval</span>
        result = <span class="SNum">10</span>
        <span class="SLgc">return</span> result
    }

    <span class="SItr">@assert</span>(<span class="SFct">toto</span>() == <span class="SNum">10</span>)
}</span></div>
<p>But <span class="code-inline">retval</span> will also make a direct reference to the caller storage, to avoid an unnecessary copy (if possible). So this is mostly a hint for the compiler, and usefull when the function returns a complexe type like a struct, a tuple or an array. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">struct</span> <span class="SCst">RGB</span> { x, y, z: <span class="STpe">f64</span> }

    <span class="SKwd">func</span> <span class="SFct">getWhite</span>()-&gt;<span class="SCst">RGB</span>
    {
        <span class="SCmt">// To avoid the clear of the returned struct, we use = undefined</span>
        <span class="SKwd">var</span> result: <span class="SKwd">retval</span> = <span class="SKwd">undefined</span>
        result.x = <span class="SNum">0.5</span>
        result.y = <span class="SNum">0.1</span>
        result.z = <span class="SNum">1.0</span>
        <span class="SLgc">return</span> result
    }

    <span class="SCmt">// Here the 'getWhite' function can directly modify r, g and b without storing</span>
    <span class="SCmt">// a temporary value on the stack.</span>
    <span class="SKwd">let</span> (r, g, b) = <span class="SFct">getWhite</span>()
    <span class="SItr">@assert</span>(r == <span class="SNum">0.5</span>)
    <span class="SItr">@assert</span>(g == <span class="SNum">0.1</span>)
    <span class="SItr">@assert</span>(b == <span class="SNum">1.0</span>)
}</span></div>
<p>This is the preferred way (because optimal) to return a struct or an array. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">func</span> <span class="SFct">toto</span>()-&gt;[<span class="SNum">255</span>] <span class="STpe">s32</span>
    {
        <span class="SCmt">// To avoid the clear of the array, we use = undefined</span>
        <span class="SKwd">var</span> result: <span class="SKwd">retval</span> = <span class="SKwd">undefined</span>
        <span class="SLgc">loop</span> i <span class="SLgc">in</span> <span class="SNum">255</span>:
            result[i] = i
        <span class="SLgc">return</span> result
    }

    <span class="SKwd">var</span> arr = <span class="SFct">toto</span>()
    <span class="SItr">@assert</span>(arr[<span class="SNum">0</span>] == <span class="SNum">0</span>)
    <span class="SItr">@assert</span>(arr[<span class="SNum">100</span>] == <span class="SNum">100</span>)
    <span class="SItr">@assert</span>(arr[<span class="SNum">254</span>] == <span class="SNum">254</span>)
}</span></div>

<h3 id="_100_function_swg__111_012_foreign_swg">Foreign</h3><p>Swag can interop with external "modules" (dlls under windows), which contain exported C functions. </p>
<p>Put a special attribute <span class="code-inline">Swag.Foreign</span> before the function prototype, and specify the module name where the function is located. </p>
<p>The module name can be a Swag compiled module, or an external system module (where the location depends on the OS). </p>
<p>In the case below, the function is located in <span class="code-inline">kernel32.dll</span> (under windows) </p>
<div class="code-block"><span class="SCde"><span class="SAtr">#[Swag.Foreign("kernel32")]</span>
<span class="SKwd">func</span> <span class="SCst">ExitProcess</span>(uExitCode: <span class="STpe">u32</span>);

<span class="SCmt">// Like for other attributes, you can use a block.</span>
<span class="SAtr">#[Swag.Foreign("kernel32")]</span>
{
    <span class="SKwd">func</span> <span class="SCst">Sleep</span>(dwMilliseconds: <span class="STpe">u32</span>);
}</span></div>
<p>Note that in the case of an external module, you will have to declare somewhere the imported library too. </p>
<p><span class="code-inline">#foreignlib</span> is here to force a link to the given library (when generating executables). </p>
<div class="code-block"><span class="SCde"><span class="SCmp">#foreignlib</span> <span class="SStr">"kernel32"</span></span></div>

<h2 id="_120_intrinsics_swg">Intrinsics</h2><p>This is the list of all intrinsics. All intrinsics start with <span class="code-inline">@</span>, which is reserved for them. </p>
<div class="code-block"><span class="SCde"><span class="SCmp">#global</span> skip</span></div>
<h3 id="_120_intrinsics_swg_Base">Base </h3>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SItr">@assert</span>(value: <span class="STpe">bool</span>);
<span class="SKwd">func</span> <span class="SItr">@breakpoint</span>();
<span class="SKwd">func</span> <span class="SItr">@getcontext</span>()-&gt;*<span class="SCst">Swag</span>.<span class="SCst">Context</span>;
<span class="SKwd">func</span> <span class="SItr">@setcontext</span>(context: <span class="SKwd">const</span> *<span class="SCst">Swag</span>.<span class="SCst">Context</span>);
<span class="SKwd">func</span> <span class="SItr">@isbytecode</span>()-&gt;<span class="STpe">bool</span>;
<span class="SKwd">func</span> <span class="SItr">@compiler</span>()-&gt;<span class="SCst">Swag</span>.<span class="SCst">ICompiler</span>;

<span class="SKwd">func</span> <span class="SItr">@args</span>()-&gt;<span class="SKwd">const</span> [..] <span class="STpe">string</span>;
<span class="SItr">@panic</span>()
<span class="SItr">@compilererror</span>()
<span class="SItr">@compilerwarning</span>()</span></div>
<h3 id="_120_intrinsics_swg_Buildin">Buildin </h3>
<div class="code-block"><span class="SCde"><span class="SItr">@spread</span>()
<span class="SItr">@init</span>()
<span class="SItr">@drop</span>()
<span class="SItr">@postmove</span>()
<span class="SItr">@postcopy</span>()
<span class="SItr">@sizeof</span>()
<span class="SItr">@alignof</span>()
<span class="SItr">@offsetof</span>()
<span class="SItr">@typeof</span>()
<span class="SItr">@kindof</span>()
<span class="SItr">@countof</span>()
<span class="SItr">@stringof</span>()
<span class="SItr">@dataof</span>()
<span class="SItr">@mkslice</span>()
<span class="SItr">@mkstring</span>()
<span class="SItr">@mkany</span>()
<span class="SItr">@mkinterface</span>()
<span class="SItr">@mkcallback</span>()
<span class="SItr">@pinfos</span>()
<span class="SItr">@isconstexpr</span>()
<span class="SItr">@itftableof</span>()

<span class="SItr">#index</span></span></div>
<h3 id="_120_intrinsics_swg_Memory_related">Memory related </h3>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SItr">@alloc</span>(size: <span class="STpe">u64</span>)-&gt;*<span class="STpe">void</span>;
<span class="SKwd">func</span> <span class="SItr">@realloc</span>(ptr: *<span class="STpe">void</span>, size: <span class="STpe">u64</span>)-&gt;*<span class="STpe">void</span>;
<span class="SKwd">func</span> <span class="SItr">@free</span>(ptr: *<span class="STpe">void</span>);
<span class="SKwd">func</span> <span class="SItr">@memset</span>(dst: *<span class="STpe">void</span>, value: <span class="STpe">u8</span>, size: <span class="STpe">u64</span>);
<span class="SKwd">func</span> <span class="SItr">@memcpy</span>(dst: *<span class="STpe">void</span>, src: <span class="SKwd">const</span> *<span class="STpe">void</span>, size: <span class="STpe">u64</span>);
<span class="SKwd">func</span> <span class="SItr">@memmove</span>(dst: *<span class="STpe">void</span>, src: <span class="SKwd">const</span> *<span class="STpe">void</span>, size: <span class="STpe">u64</span>);
<span class="SKwd">func</span> <span class="SItr">@memcmp</span>(dst, src: <span class="SKwd">const</span> *<span class="STpe">void</span>, size: <span class="STpe">u64</span>)-&gt;<span class="STpe">s32</span>;
<span class="SKwd">func</span> <span class="SItr">@strlen</span>(value: <span class="SKwd">const</span> *<span class="STpe">u8</span>)-&gt;<span class="STpe">u64</span>;</span></div>
<h3 id="_120_intrinsics_swg_Atomic_operations">Atomic operations </h3>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SItr">@atomadd</span>(addr: *<span class="STpe">s8</span>, value: <span class="STpe">s8</span>)-&gt;<span class="STpe">s8</span>;
<span class="SKwd">func</span> <span class="SItr">@atomadd</span>(addr: *<span class="STpe">s16</span>, value: <span class="STpe">s16</span>)-&gt;<span class="STpe">s16</span>;
<span class="SKwd">func</span> <span class="SItr">@atomadd</span>(addr: *<span class="STpe">s32</span>, value: <span class="STpe">s32</span>)-&gt;<span class="STpe">s32</span>;
<span class="SKwd">func</span> <span class="SItr">@atomadd</span>(addr: *<span class="STpe">s64</span>, value: <span class="STpe">s64</span>)-&gt;<span class="STpe">s64</span>;
<span class="SKwd">func</span> <span class="SItr">@atomadd</span>(addr: *<span class="STpe">u8</span>, value: <span class="STpe">u8</span>)-&gt;<span class="STpe">u8</span>;
<span class="SKwd">func</span> <span class="SItr">@atomadd</span>(addr: *<span class="STpe">u16</span>, value: <span class="STpe">u16</span>)-&gt;<span class="STpe">u16</span>;
<span class="SKwd">func</span> <span class="SItr">@atomadd</span>(addr: *<span class="STpe">u32</span>, value: <span class="STpe">u32</span>)-&gt;<span class="STpe">u32</span>;
<span class="SKwd">func</span> <span class="SItr">@atomadd</span>(addr: *<span class="STpe">u64</span>, value: <span class="STpe">u64</span>)-&gt;<span class="STpe">u64</span>;

<span class="SKwd">func</span> <span class="SItr">@atomand</span>(addr: *<span class="STpe">s8</span>, value: <span class="STpe">s8</span>)-&gt;<span class="STpe">s8</span>;
<span class="SKwd">func</span> <span class="SItr">@atomand</span>(addr: *<span class="STpe">s16</span>, value: <span class="STpe">s16</span>)-&gt;<span class="STpe">s16</span>;
<span class="SKwd">func</span> <span class="SItr">@atomand</span>(addr: *<span class="STpe">s32</span>, value: <span class="STpe">s32</span>)-&gt;<span class="STpe">s32</span>;
<span class="SKwd">func</span> <span class="SItr">@atomand</span>(addr: *<span class="STpe">s64</span>, value: <span class="STpe">s64</span>)-&gt;<span class="STpe">s64</span>;
<span class="SKwd">func</span> <span class="SItr">@atomand</span>(addr: *<span class="STpe">u8</span>, value: <span class="STpe">u8</span>)-&gt;<span class="STpe">u8</span>;
<span class="SKwd">func</span> <span class="SItr">@atomand</span>(addr: *<span class="STpe">u16</span>, value: <span class="STpe">u16</span>)-&gt;<span class="STpe">u16</span>;
<span class="SKwd">func</span> <span class="SItr">@atomand</span>(addr: *<span class="STpe">u32</span>, value: <span class="STpe">u32</span>)-&gt;<span class="STpe">u32</span>;
<span class="SKwd">func</span> <span class="SItr">@atomand</span>(addr: *<span class="STpe">u64</span>, value: <span class="STpe">u64</span>)-&gt;<span class="STpe">u64</span>;

<span class="SKwd">func</span> <span class="SItr">@atomor</span>(addr: *<span class="STpe">s8</span>, value: <span class="STpe">s8</span>)-&gt;<span class="STpe">s8</span>;
<span class="SKwd">func</span> <span class="SItr">@atomor</span>(addr: *<span class="STpe">s16</span>, value: <span class="STpe">s16</span>)-&gt;<span class="STpe">s16</span>;
<span class="SKwd">func</span> <span class="SItr">@atomor</span>(addr: *<span class="STpe">s32</span>, value: <span class="STpe">s32</span>)-&gt;<span class="STpe">s32</span>;
<span class="SKwd">func</span> <span class="SItr">@atomor</span>(addr: *<span class="STpe">s64</span>, value: <span class="STpe">s64</span>)-&gt;<span class="STpe">s64</span>;
<span class="SKwd">func</span> <span class="SItr">@atomor</span>(addr: *<span class="STpe">u8</span>, value: <span class="STpe">u8</span>)-&gt;<span class="STpe">u8</span>;
<span class="SKwd">func</span> <span class="SItr">@atomor</span>(addr: *<span class="STpe">u16</span>, value: <span class="STpe">u16</span>)-&gt;<span class="STpe">u16</span>;
<span class="SKwd">func</span> <span class="SItr">@atomor</span>(addr: *<span class="STpe">u32</span>, value: <span class="STpe">u32</span>)-&gt;<span class="STpe">u32</span>;
<span class="SKwd">func</span> <span class="SItr">@atomor</span>(addr: *<span class="STpe">u64</span>, value: <span class="STpe">u64</span>)-&gt;<span class="STpe">u64</span>;

<span class="SKwd">func</span> <span class="SItr">@atomxor</span>(addr: *<span class="STpe">s8</span>, value: <span class="STpe">s8</span>)-&gt;<span class="STpe">s8</span>;
<span class="SKwd">func</span> <span class="SItr">@atomxor</span>(addr: *<span class="STpe">s16</span>, value: <span class="STpe">s16</span>)-&gt;<span class="STpe">s16</span>;
<span class="SKwd">func</span> <span class="SItr">@atomxor</span>(addr: *<span class="STpe">s32</span>, value: <span class="STpe">s32</span>)-&gt;<span class="STpe">s32</span>;
<span class="SKwd">func</span> <span class="SItr">@atomxor</span>(addr: *<span class="STpe">s64</span>, value: <span class="STpe">s64</span>)-&gt;<span class="STpe">s64</span>;
<span class="SKwd">func</span> <span class="SItr">@atomxor</span>(addr: *<span class="STpe">u8</span>, value: <span class="STpe">u8</span>)-&gt;<span class="STpe">u8</span>;
<span class="SKwd">func</span> <span class="SItr">@atomxor</span>(addr: *<span class="STpe">u16</span>, value: <span class="STpe">u16</span>)-&gt;<span class="STpe">u16</span>;
<span class="SKwd">func</span> <span class="SItr">@atomxor</span>(addr: *<span class="STpe">u32</span>, value: <span class="STpe">u32</span>)-&gt;<span class="STpe">u32</span>;
<span class="SKwd">func</span> <span class="SItr">@atomxor</span>(addr: *<span class="STpe">u64</span>, value: <span class="STpe">u64</span>)-&gt;<span class="STpe">u64</span>;

<span class="SKwd">func</span> <span class="SItr">@atomxchg</span>(addr: *<span class="STpe">s8</span>,  exchangeWith: <span class="STpe">s8</span>)-&gt;<span class="STpe">s8</span>;
<span class="SKwd">func</span> <span class="SItr">@atomxchg</span>(addr: *<span class="STpe">s16</span>, exchangeWith: <span class="STpe">s16</span>)-&gt;<span class="STpe">s16</span>;
<span class="SKwd">func</span> <span class="SItr">@atomxchg</span>(addr: *<span class="STpe">s32</span>, exchangeWith: <span class="STpe">s32</span>)-&gt;<span class="STpe">s32</span>;
<span class="SKwd">func</span> <span class="SItr">@atomxchg</span>(addr: *<span class="STpe">s64</span>, exchangeWith: <span class="STpe">s64</span>)-&gt;<span class="STpe">s64</span>;
<span class="SKwd">func</span> <span class="SItr">@atomxchg</span>(addr: *<span class="STpe">u8</span>,  exchangeWith: <span class="STpe">u8</span>)-&gt;<span class="STpe">u8</span>;
<span class="SKwd">func</span> <span class="SItr">@atomxchg</span>(addr: *<span class="STpe">u16</span>, exchangeWith: <span class="STpe">u16</span>)-&gt;<span class="STpe">u16</span>;
<span class="SKwd">func</span> <span class="SItr">@atomxchg</span>(addr: *<span class="STpe">u32</span>, exchangeWith: <span class="STpe">u32</span>)-&gt;<span class="STpe">u32</span>;
<span class="SKwd">func</span> <span class="SItr">@atomxchg</span>(addr: *<span class="STpe">u64</span>, exchangeWith: <span class="STpe">u64</span>)-&gt;<span class="STpe">u64</span>;

<span class="SKwd">func</span> <span class="SItr">@atomcmpxchg</span>(addr: *<span class="STpe">s8</span>,  compareTo, exchangeWith: <span class="STpe">s8</span>)-&gt;<span class="STpe">s8</span>;
<span class="SKwd">func</span> <span class="SItr">@atomcmpxchg</span>(addr: *<span class="STpe">s16</span>, compareTo, exchangeWith: <span class="STpe">s16</span>)-&gt;<span class="STpe">s16</span>;
<span class="SKwd">func</span> <span class="SItr">@atomcmpxchg</span>(addr: *<span class="STpe">s32</span>, compareTo, exchangeWith: <span class="STpe">s32</span>)-&gt;<span class="STpe">s32</span>;
<span class="SKwd">func</span> <span class="SItr">@atomcmpxchg</span>(addr: *<span class="STpe">s64</span>, compareTo, exchangeWith: <span class="STpe">s64</span>)-&gt;<span class="STpe">s64</span>;
<span class="SKwd">func</span> <span class="SItr">@atomcmpxchg</span>(addr: *<span class="STpe">u8</span>,  compareTo, exchangeWith: <span class="STpe">u8</span>)-&gt;<span class="STpe">u8</span>;
<span class="SKwd">func</span> <span class="SItr">@atomcmpxchg</span>(addr: *<span class="STpe">u16</span>, compareTo, exchangeWith: <span class="STpe">u16</span>)-&gt;<span class="STpe">u16</span>;
<span class="SKwd">func</span> <span class="SItr">@atomcmpxchg</span>(addr: *<span class="STpe">u32</span>, compareTo, exchangeWith: <span class="STpe">u32</span>)-&gt;<span class="STpe">u32</span>;
<span class="SKwd">func</span> <span class="SItr">@atomcmpxchg</span>(addr: *<span class="STpe">u64</span>, compareTo, exchangeWith: <span class="STpe">u64</span>)-&gt;<span class="STpe">u64</span>;</span></div>
<h3 id="_120_intrinsics_swg_Math">Math </h3>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SItr">@sqrt</span>(value: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span>;
<span class="SKwd">func</span> <span class="SItr">@sqrt</span>(value: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span>;

<span class="SKwd">func</span> <span class="SItr">@sin</span>(value: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span>;
<span class="SKwd">func</span> <span class="SItr">@sin</span>(value: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span>;
<span class="SKwd">func</span> <span class="SItr">@cos</span>(value: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span>;
<span class="SKwd">func</span> <span class="SItr">@cos</span>(value: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span>;
<span class="SKwd">func</span> <span class="SItr">@tan</span>(value: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span>;
<span class="SKwd">func</span> <span class="SItr">@tan</span>(value: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span>;

<span class="SKwd">func</span> <span class="SItr">@sinh</span>(value: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span>;
<span class="SKwd">func</span> <span class="SItr">@sinh</span>(value: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span>;
<span class="SKwd">func</span> <span class="SItr">@cosh</span>(value: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span>;
<span class="SKwd">func</span> <span class="SItr">@cosh</span>(value: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span>;
<span class="SKwd">func</span> <span class="SItr">@tanh</span>(value: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span>;
<span class="SKwd">func</span> <span class="SItr">@tanh</span>(value: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span>;

<span class="SKwd">func</span> <span class="SItr">@asin</span>(value: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span>;
<span class="SKwd">func</span> <span class="SItr">@asin</span>(value: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span>;
<span class="SKwd">func</span> <span class="SItr">@acos</span>(value: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span>;
<span class="SKwd">func</span> <span class="SItr">@acos</span>(value: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span>;
<span class="SKwd">func</span> <span class="SItr">@atan</span>(value: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span>;
<span class="SKwd">func</span> <span class="SItr">@atan</span>(value: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span>;

<span class="SKwd">func</span> <span class="SItr">@log</span>(value: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span>;
<span class="SKwd">func</span> <span class="SItr">@log</span>(value: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span>;
<span class="SKwd">func</span> <span class="SItr">@log2</span>(value: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span>;
<span class="SKwd">func</span> <span class="SItr">@log2</span>(value: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span>;
<span class="SKwd">func</span> <span class="SItr">@log10</span>(value: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span>;
<span class="SKwd">func</span> <span class="SItr">@log10</span>(value: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span>;

<span class="SKwd">func</span> <span class="SItr">@floor</span>(value: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span>;
<span class="SKwd">func</span> <span class="SItr">@floor</span>(value: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span>;
<span class="SKwd">func</span> <span class="SItr">@ceil</span>(value: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span>;
<span class="SKwd">func</span> <span class="SItr">@ceil</span>(value: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span>;
<span class="SKwd">func</span> <span class="SItr">@trunc</span>(value: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span>;
<span class="SKwd">func</span> <span class="SItr">@trunc</span>(value: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span>;
<span class="SKwd">func</span> <span class="SItr">@round</span>(value: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span>;
<span class="SKwd">func</span> <span class="SItr">@round</span>(value: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span>;

<span class="SKwd">func</span> <span class="SItr">@abs</span>(value: <span class="STpe">s8</span>)-&gt;<span class="STpe">s8</span>;
<span class="SKwd">func</span> <span class="SItr">@abs</span>(value: <span class="STpe">s16</span>)-&gt;<span class="STpe">s16</span>;
<span class="SKwd">func</span> <span class="SItr">@abs</span>(value: <span class="STpe">s32</span>)-&gt;<span class="STpe">s32</span>;
<span class="SKwd">func</span> <span class="SItr">@abs</span>(value: <span class="STpe">s64</span>)-&gt;<span class="STpe">s64</span>;
<span class="SKwd">func</span> <span class="SItr">@abs</span>(value: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span>;
<span class="SKwd">func</span> <span class="SItr">@abs</span>(value: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span>;

<span class="SKwd">func</span> <span class="SItr">@exp</span>(value: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span>;
<span class="SKwd">func</span> <span class="SItr">@exp</span>(value: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span>;
<span class="SKwd">func</span> <span class="SItr">@exp2</span>(value: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span>;
<span class="SKwd">func</span> <span class="SItr">@exp2</span>(value: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span>;
<span class="SKwd">func</span> <span class="SItr">@pow</span>(value1, value2: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span>;
<span class="SKwd">func</span> <span class="SItr">@pow</span>(value1, value2: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span>;

<span class="SKwd">func</span> <span class="SItr">@min</span>(value1, value2: <span class="STpe">s8</span>)-&gt;<span class="STpe">s8</span>;
<span class="SKwd">func</span> <span class="SItr">@min</span>(value1, value2: <span class="STpe">s16</span>)-&gt;<span class="STpe">s16</span>;
<span class="SKwd">func</span> <span class="SItr">@min</span>(value1, value2: <span class="STpe">s32</span>)-&gt;<span class="STpe">s32</span>;
<span class="SKwd">func</span> <span class="SItr">@min</span>(value1, value2: <span class="STpe">s64</span>)-&gt;<span class="STpe">s64</span>;
<span class="SKwd">func</span> <span class="SItr">@min</span>(value1, value2: <span class="STpe">u8</span>)-&gt;<span class="STpe">u8</span>;
<span class="SKwd">func</span> <span class="SItr">@min</span>(value1, value2: <span class="STpe">u16</span>)-&gt;<span class="STpe">u16</span>;
<span class="SKwd">func</span> <span class="SItr">@min</span>(value1, value2: <span class="STpe">u32</span>)-&gt;<span class="STpe">u32</span>;
<span class="SKwd">func</span> <span class="SItr">@min</span>(value1, value2: <span class="STpe">u64</span>)-&gt;<span class="STpe">u64</span>;
<span class="SKwd">func</span> <span class="SItr">@min</span>(value1, value2: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span>;
<span class="SKwd">func</span> <span class="SItr">@min</span>(value1, value2: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span>;

<span class="SKwd">func</span> <span class="SItr">@max</span>(value1, value2: <span class="STpe">s8</span>)-&gt;<span class="STpe">s8</span>;
<span class="SKwd">func</span> <span class="SItr">@max</span>(value1, value2: <span class="STpe">s16</span>)-&gt;<span class="STpe">s16</span>;
<span class="SKwd">func</span> <span class="SItr">@max</span>(value1, value2: <span class="STpe">s32</span>)-&gt;<span class="STpe">s32</span>;
<span class="SKwd">func</span> <span class="SItr">@max</span>(value1, value2: <span class="STpe">s64</span>)-&gt;<span class="STpe">s64</span>;
<span class="SKwd">func</span> <span class="SItr">@max</span>(value1, value2: <span class="STpe">u8</span>)-&gt;<span class="STpe">u8</span>;
<span class="SKwd">func</span> <span class="SItr">@max</span>(value1, value2: <span class="STpe">u16</span>)-&gt;<span class="STpe">u16</span>;
<span class="SKwd">func</span> <span class="SItr">@max</span>(value1, value2: <span class="STpe">u32</span>)-&gt;<span class="STpe">u32</span>;
<span class="SKwd">func</span> <span class="SItr">@max</span>(value1, value2: <span class="STpe">u64</span>)-&gt;<span class="STpe">u64</span>;
<span class="SKwd">func</span> <span class="SItr">@max</span>(value1, value2: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span>;
<span class="SKwd">func</span> <span class="SItr">@max</span>(value1, value2: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span>;

<span class="SKwd">func</span> <span class="SItr">@bitcountnz</span>(value: <span class="STpe">u8</span>)-&gt;<span class="STpe">u8</span>;
<span class="SKwd">func</span> <span class="SItr">@bitcountnz</span>(value: <span class="STpe">u16</span>)-&gt;<span class="STpe">u16</span>;
<span class="SKwd">func</span> <span class="SItr">@bitcountnz</span>(value: <span class="STpe">u32</span>)-&gt;<span class="STpe">u32</span>;
<span class="SKwd">func</span> <span class="SItr">@bitcountnz</span>(value: <span class="STpe">u64</span>)-&gt;<span class="STpe">u64</span>;
<span class="SKwd">func</span> <span class="SItr">@bitcounttz</span>(value: <span class="STpe">u8</span>)-&gt;<span class="STpe">u8</span>;
<span class="SKwd">func</span> <span class="SItr">@bitcounttz</span>(value: <span class="STpe">u16</span>)-&gt;<span class="STpe">u16</span>;
<span class="SKwd">func</span> <span class="SItr">@bitcounttz</span>(value: <span class="STpe">u32</span>)-&gt;<span class="STpe">u32</span>;
<span class="SKwd">func</span> <span class="SItr">@bitcounttz</span>(value: <span class="STpe">u64</span>)-&gt;<span class="STpe">u64</span>;
<span class="SKwd">func</span> <span class="SItr">@bitcountlz</span>(value: <span class="STpe">u8</span>)-&gt;<span class="STpe">u8</span>;
<span class="SKwd">func</span> <span class="SItr">@bitcountlz</span>(value: <span class="STpe">u16</span>)-&gt;<span class="STpe">u16</span>;
<span class="SKwd">func</span> <span class="SItr">@bitcountlz</span>(value: <span class="STpe">u32</span>)-&gt;<span class="STpe">u32</span>;
<span class="SKwd">func</span> <span class="SItr">@bitcountlz</span>(value: <span class="STpe">u64</span>)-&gt;<span class="STpe">u64</span>;

<span class="SKwd">func</span> <span class="SItr">@byteswap</span>(value: <span class="STpe">u16</span>)-&gt;<span class="STpe">u16</span>;
<span class="SKwd">func</span> <span class="SItr">@byteswap</span>(value: <span class="STpe">u32</span>)-&gt;<span class="STpe">u32</span>;
<span class="SKwd">func</span> <span class="SItr">@byteswap</span>(value: <span class="STpe">u64</span>)-&gt;<span class="STpe">u64</span>;

<span class="SKwd">func</span> <span class="SItr">@rol</span>(value, num: <span class="STpe">u8</span>)-&gt;<span class="STpe">u8</span>;
<span class="SKwd">func</span> <span class="SItr">@rol</span>(value, num: <span class="STpe">u16</span>)-&gt;<span class="STpe">u16</span>;
<span class="SKwd">func</span> <span class="SItr">@rol</span>(value, num: <span class="STpe">u32</span>)-&gt;<span class="STpe">u32</span>;
<span class="SKwd">func</span> <span class="SItr">@rol</span>(value, num: <span class="STpe">u64</span>)-&gt;<span class="STpe">u64</span>;

<span class="SKwd">func</span> <span class="SItr">@ror</span>(value, num: <span class="STpe">u8</span>)-&gt;<span class="STpe">u8</span>;
<span class="SKwd">func</span> <span class="SItr">@ror</span>(value, num: <span class="STpe">u16</span>)-&gt;<span class="STpe">u16</span>;
<span class="SKwd">func</span> <span class="SItr">@ror</span>(value, num: <span class="STpe">u32</span>)-&gt;<span class="STpe">u32</span>;
<span class="SKwd">func</span> <span class="SItr">@ror</span>(value, num: <span class="STpe">u64</span>)-&gt;<span class="STpe">u64</span>;

<span class="SKwd">func</span> <span class="SItr">@muladd</span>(val1, val2, val3: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span>;
<span class="SKwd">func</span> <span class="SItr">@muladd</span>(val1, val2, val3: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span>;</span></div>

<h2 id="_121_init_swg">Init</h2><h3 id="_121_init_swg_@init">@init </h3>
<p><span class="code-inline">@init</span> can be used to reinitialize a variable or memory block to the default value. </p>
<p>You can just specify the variable to reinitialize as a unique argument. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> x = <span class="SNum">666</span>
    <span class="SItr">@init</span>(x)
    <span class="SItr">@assert</span>(x == <span class="SNum">0</span>) <span class="SCmt">// For a simple variable, the default value is 0</span>
}</span></div>
<p>But you can also specify a pointer and a count to reinitialize a variable amount of values. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> x = [<span class="SNum">1</span>, <span class="SNum">2</span>]
    <span class="SItr">@init</span>(&x, <span class="SNum">2</span>) <span class="SCmt">// Initialize 2 elements</span>
    <span class="SItr">@assert</span>(x[<span class="SNum">0</span>] == <span class="SNum">0</span>)
    <span class="SItr">@assert</span>(x[<span class="SNum">1</span>] == <span class="SNum">0</span>)

    x[<span class="SNum">0</span>] = <span class="SNum">1</span>
    x[<span class="SNum">1</span>] = <span class="SNum">2</span>
    <span class="SItr">@init</span>(x) <span class="SCmt">// Or you can just specify the variable name</span>
    <span class="SItr">@assert</span>(x[<span class="SNum">0</span>] == <span class="SNum">0</span>)
    <span class="SItr">@assert</span>(x[<span class="SNum">1</span>] == <span class="SNum">0</span>)
}</span></div>
<p>You can also specify a <i>value</i> to initialize, instead of the default one. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> x = <span class="SNum">666</span>'<span class="STpe">f32</span>
    <span class="SItr">@init</span>(x)(<span class="SNum">3.14</span>) <span class="SCmt">// Initialize to 3.14 instead of zero</span>
    <span class="SItr">@assert</span>(x == <span class="SNum">3.14</span>)
}</span></div>
<p>Same for an array. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> x = [<span class="SNum">1</span>, <span class="SNum">2</span>]
    <span class="SItr">@init</span>(&x, <span class="SNum">2</span>)(<span class="SNum">555</span>)
    <span class="SItr">@assert</span>(x[<span class="SNum">0</span>] == <span class="SNum">555</span>)
    <span class="SItr">@assert</span>(x[<span class="SNum">1</span>] == <span class="SNum">555</span>)
}</span></div>
<p>When called on a struct, the struct will be restored to the values defined in it. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">struct</span> <span class="SCst">RGB</span> { r = <span class="SNum">1</span>, g = <span class="SNum">2</span>, b = <span class="SNum">3</span> }

    <span class="SKwd">var</span> rgb: <span class="SCst">RGB</span>{<span class="SNum">10</span>, <span class="SNum">20</span>, <span class="SNum">30</span>}

    <span class="SItr">@assert</span>(rgb.r == <span class="SNum">10</span>)
    <span class="SItr">@assert</span>(rgb.g == <span class="SNum">20</span>)
    <span class="SItr">@assert</span>(rgb.b == <span class="SNum">30</span>)

    <span class="SItr">@init</span>(rgb)

    <span class="SItr">@assert</span>(rgb.r == <span class="SNum">1</span>)
    <span class="SItr">@assert</span>(rgb.g == <span class="SNum">2</span>)
    <span class="SItr">@assert</span>(rgb.b == <span class="SNum">3</span>)
}</span></div>
<p>But you can also specified the values. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">struct</span> <span class="SCst">RGB</span> { r = <span class="SNum">1</span>, g = <span class="SNum">2</span>, b = <span class="SNum">3</span> }

    <span class="SKwd">var</span> rgb: <span class="SCst">RGB</span>{<span class="SNum">10</span>, <span class="SNum">20</span>, <span class="SNum">30</span>}

    <span class="SItr">@assert</span>(rgb.r == <span class="SNum">10</span>)
    <span class="SItr">@assert</span>(rgb.g == <span class="SNum">20</span>)
    <span class="SItr">@assert</span>(rgb.b == <span class="SNum">30</span>)

    <span class="SItr">@init</span>(rgb)(<span class="SNum">5</span>, <span class="SNum">6</span>, <span class="SNum">7</span>)

    <span class="SItr">@assert</span>(rgb.r == <span class="SNum">5</span>)
    <span class="SItr">@assert</span>(rgb.g == <span class="SNum">6</span>)
    <span class="SItr">@assert</span>(rgb.b == <span class="SNum">7</span>)
}</span></div>
<p>And this works also for array. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">struct</span> <span class="SCst">RGB</span> { r = <span class="SNum">1</span>, g = <span class="SNum">2</span>, b = <span class="SNum">3</span> }

    <span class="SKwd">var</span> rgb: [<span class="SNum">4</span>] <span class="SCst">RGB</span>
    <span class="SItr">@init</span>(&rgb, <span class="SNum">4</span>)(<span class="SNum">5</span>, <span class="SNum">6</span>, <span class="SNum">7</span>)
    <span class="SItr">@assert</span>(rgb[<span class="SNum">3</span>].r == <span class="SNum">5</span>)
    <span class="SItr">@assert</span>(rgb[<span class="SNum">3</span>].g == <span class="SNum">6</span>)
    <span class="SItr">@assert</span>(rgb[<span class="SNum">3</span>].b == <span class="SNum">7</span>)

    <span class="SItr">@init</span>(rgb)(<span class="SNum">50</span>, <span class="SNum">60</span>, <span class="SNum">70</span>)
    <span class="SItr">@assert</span>(rgb[<span class="SNum">3</span>].r == <span class="SNum">50</span>)
    <span class="SItr">@assert</span>(rgb[<span class="SNum">3</span>].g == <span class="SNum">60</span>)
    <span class="SItr">@assert</span>(rgb[<span class="SNum">3</span>].b == <span class="SNum">70</span>)
}</span></div>
<h3 id="_121_init_swg_@drop">@drop </h3>
<p>For a struct, <span class="code-inline">@init</span> will <b>not</b> call <span class="code-inline">opDrop</span>, so this is mostly useful to initialize a plain old data. </p>
<p>But there is also a <span class="code-inline">@drop</span> intrinsic, which works the same, except that it will <span class="code-inline">drop</span> all the content by calling <span class="code-inline">opDrop</span> if it is defined. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">struct</span> <span class="SCst">RGB</span> { r = <span class="SNum">1</span>, g = <span class="SNum">2</span>, b = <span class="SNum">3</span> }

    <span class="SKwd">var</span> rgb: [<span class="SNum">4</span>] <span class="SCst">RGB</span>

    <span class="SCmt">// In fact this is a no op, as struct RGB is plain old data, without a defined 'opDrop'</span>
    <span class="SItr">@drop</span>(&rgb, <span class="SNum">4</span>)

    <span class="SItr">@init</span>(&rgb, <span class="SNum">4</span>)(<span class="SNum">5</span>, <span class="SNum">6</span>, <span class="SNum">7</span>)
    <span class="SItr">@assert</span>(rgb[<span class="SNum">3</span>].r == <span class="SNum">5</span>)
    <span class="SItr">@assert</span>(rgb[<span class="SNum">3</span>].g == <span class="SNum">6</span>)
    <span class="SItr">@assert</span>(rgb[<span class="SNum">3</span>].b == <span class="SNum">7</span>)
}</span></div>

<h2 id="_130_generic_swg">Generic</h2>
<h3 id="_130_generic_swg__131_001_declaration_swg">Declaration</h3><p>A function can be generic by specifying some parameters after <span class="code-inline">func</span>. At the call site, you specify the generic parameters with <span class="code-inline">funcCall'(type1, type2, ...)(parameters)</span>. Note that parenthesis can be omitted if there's only one generic parameter. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    {
        <span class="SCmt">// Here 'T' is a generic type.</span>
        <span class="SKwd">func</span>(<span class="SKwd">var</span> <span class="SCst">T</span>) <span class="SFct">myFunc</span>(val: <span class="SCst">T</span>) =&gt; <span class="SNum">2</span> * val

        <span class="SItr">@assert</span>(<span class="SFct">myFunc</span>'<span class="STpe">s32</span>(<span class="SNum">2</span>) == <span class="SNum">4</span>)
        <span class="SItr">@assert</span>(<span class="SFct">myFunc</span>'<span class="STpe">f32</span>(<span class="SNum">2.0</span>) == <span class="SNum">4.0</span>)
    }

    {
        <span class="SCmt">// You can omit 'var' to declare the generic type, because a single identifier</span>
        <span class="SCmt">// is considered to be a type.</span>
        <span class="SKwd">func</span>(<span class="SCst">T</span>) <span class="SFct">myFunc</span>(val: <span class="SCst">T</span>) =&gt; <span class="SNum">2</span> * val

        <span class="SItr">@assert</span>(<span class="SFct">myFunc</span>'<span class="STpe">s32</span>(<span class="SNum">2</span>) == <span class="SNum">4</span>)
        <span class="SItr">@assert</span>(<span class="SFct">myFunc</span>'<span class="STpe">f32</span>(<span class="SNum">2.0</span>) == <span class="SNum">4.0</span>)
    }

    {
        <span class="SCmt">// You can set a default value to the type.</span>
        <span class="SKwd">func</span>(<span class="SCst">T</span> = <span class="STpe">s32</span>) <span class="SFct">myFunc</span>(val: <span class="SCst">T</span>) =&gt; <span class="SNum">2</span> * val

        <span class="SItr">@assert</span>(<span class="SFct">myFunc</span>(<span class="SNum">2</span>'<span class="STpe">s32</span>) == <span class="SNum">4</span>)
        <span class="SItr">@assert</span>(<span class="SFct">myFunc</span>'<span class="STpe">f32</span>(<span class="SNum">2.0</span>) == <span class="SNum">4.0</span>)
    }

    {
        <span class="SCmt">// Of course you can specify more than one generic parameter</span>
        <span class="SKwd">func</span>(<span class="SCst">K</span>, <span class="SCst">V</span>) <span class="SFct">myFunc</span>(key: <span class="SCst">K</span>, value: <span class="SCst">V</span>) =&gt; value

        <span class="SItr">@assert</span>(<span class="SFct">myFunc</span>(<span class="SNum">2</span>'<span class="STpe">s32</span>, <span class="SStr">"value"</span>) == <span class="SStr">"value"</span>) <span class="SCmt">// K and V are deduced</span>
        <span class="SItr">@assert</span>(<span class="SFct">myFunc</span>'(<span class="STpe">s32</span>, <span class="STpe">string</span>)(<span class="SNum">2</span>, <span class="SStr">"value"</span>) == <span class="SStr">"value"</span>) <span class="SCmt">// K and V are explicit</span>

        <span class="SItr">@assert</span>(<span class="SFct">myFunc</span>(<span class="SNum">2</span>'<span class="STpe">s32</span>, <span class="SKwd">true</span>) == <span class="SKwd">true</span>)
        <span class="SItr">@assert</span>(<span class="SFct">myFunc</span>'(<span class="STpe">s32</span>, <span class="STpe">bool</span>)(<span class="SNum">2</span>, <span class="SKwd">true</span>) == <span class="SKwd">true</span>)
    }
}</span></div>
<p>Generic types can be deduced from parameters, so <span class="code-inline">func'type()</span> is not always necessary. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">func</span>(<span class="SCst">T</span>) <span class="SFct">myFunc</span>(val: <span class="SCst">T</span>) =&gt; <span class="SNum">2</span> * val

    <span class="SItr">@assert</span>(<span class="SFct">myFunc</span>(<span class="SNum">2</span>'<span class="STpe">s32</span>) == <span class="SNum">4</span>) <span class="SCmt">// T is deduced to be s32</span>
    <span class="SItr">@assert</span>(<span class="SFct">myFunc</span>(<span class="SNum">2.0</span>'<span class="STpe">f32</span>) == <span class="SNum">4.0</span>) <span class="SCmt">// T is deduced to be f32</span>
}</span></div>
<p>You can also specify constants as generic parameters. </p>
<p><span class="code-inline">N</span> is a constant a type <span class="code-inline">s32</span>. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">func</span>(<span class="SKwd">const</span> <span class="SCst">N</span>: <span class="STpe">s32</span>) <span class="SFct">myFunc</span>() = <span class="SItr">@assert</span>(<span class="SCst">N</span> == <span class="SNum">10</span>)
    <span class="SFct">myFunc</span>'<span class="SNum">10</span>()
}</span></div>
<p><span class="code-inline">const</span> can also be omitted, as an identifier followed by a type definition is considered to be a constant and not a type. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">func</span>(<span class="SCst">N</span>: <span class="STpe">s32</span>) <span class="SFct">myFunc</span>() = <span class="SItr">@assert</span>(<span class="SCst">N</span> == <span class="SNum">10</span>)
    <span class="SFct">myFunc</span>'<span class="SNum">10</span>()
}</span></div>
<p>You can also assign a default value to the constant. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">func</span>(<span class="SCst">N</span>: <span class="STpe">s32</span> = <span class="SNum">10</span>) <span class="SFct">myFunc</span>() = <span class="SItr">@assert</span>(<span class="SCst">N</span> == <span class="SNum">10</span>)
    <span class="SFct">myFunc</span>()
}</span></div>
<p>You can ommit the type if you declare the constant with <span class="code-inline">const</span>. It will be deduced from the assignment expression. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">func</span>(<span class="SKwd">const</span> <span class="SCst">N</span> = <span class="SNum">10</span>) <span class="SFct">myFunc</span>() = <span class="SItr">@assert</span>(<span class="SCst">N</span> == <span class="SNum">10</span>)
    <span class="SFct">myFunc</span>()
}</span></div>
<p>You can mix types and constants. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    {
        <span class="SCmt">// `T` is a type, `N` is a constant of type `s32`, because remember that an identifier</span>
        <span class="SCmt">// alone is considered to be a generic type.</span>
        <span class="SKwd">func</span>(<span class="SCst">T</span>, <span class="SCst">N</span>: <span class="STpe">s32</span>) <span class="SFct">myFunc</span>(x: <span class="SCst">T</span>) =&gt; x * <span class="SCst">N</span>

        <span class="SKwd">namealias</span> call = <span class="SFct">myFunc</span>'(<span class="STpe">s32</span>, <span class="SNum">10</span>)
        <span class="SItr">@assert</span>(<span class="SFct">call</span>(<span class="SNum">2</span>) == <span class="SNum">20</span>)
        <span class="SItr">@assert</span>(<span class="SFct">call</span>(<span class="SNum">100</span>) == <span class="SNum">1000</span>)
    }

    {
        <span class="SCmt">// So if you want to declare multiple constants, specify the type (or 'const') for each.</span>
        <span class="SKwd">func</span>(<span class="SCst">T</span>: <span class="STpe">s32</span>, <span class="SCst">N</span>: <span class="STpe">s32</span>) <span class="SFct">myFunc</span>() =&gt; <span class="SCst">T</span> * <span class="SCst">N</span>

        <span class="SItr">@assert</span>(<span class="SFct">myFunc</span>'(<span class="SNum">5</span>, <span class="SNum">10</span>)() == <span class="SNum">50</span>)
    }

    {
        <span class="SCmt">// And if you want to declare multiple types with default values, specify the value for each.</span>
        <span class="SKwd">func</span>(<span class="SCst">T</span> = <span class="STpe">s32</span>, <span class="SCst">V</span> = <span class="STpe">s32</span>) <span class="SFct">myFunc</span>(x: <span class="SCst">T</span>, y: <span class="SCst">V</span>) =&gt; x * y

        <span class="SItr">@assert</span>(<span class="SFct">myFunc</span>(<span class="SNum">1</span>'<span class="STpe">s32</span>, <span class="SNum">2</span>'<span class="STpe">f32</span>) == <span class="SNum">2.0</span>)
        <span class="SItr">@assert</span>(<span class="SFct">myFunc</span>(<span class="SNum">1</span>'<span class="STpe">s32</span>, <span class="SNum">2</span>'<span class="STpe">s32</span>) == <span class="SNum">2</span>)
    }
}</span></div>
<p>Like functions, a struct can also be generic. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    {
        <span class="SKwd">struct</span>(<span class="SCst">T</span>) <span class="SCst">Struct</span>
        {
            val: <span class="SCst">T</span>
        }

        <span class="SKwd">var</span> x: <span class="SCst">Struct</span>'<span class="STpe">s32</span>
        <span class="SItr">@assert</span>(<span class="SItr">@typeof</span>(x.val) == <span class="STpe">s32</span>)
        <span class="SKwd">var</span> x1: <span class="SCst">Struct</span>'<span class="STpe">f32</span>
        <span class="SItr">@assert</span>(<span class="SItr">@typeof</span>(x1.val) == <span class="STpe">f32</span>)
    }

    {
        <span class="SKwd">struct</span>(<span class="SCst">T</span>, <span class="SCst">N</span>: <span class="STpe">s32</span>) <span class="SCst">Struct</span>
        {
            val: [<span class="SCst">N</span>] <span class="SCst">T</span>
        }

        <span class="SKwd">var</span> x: <span class="SCst">Struct</span>'(<span class="STpe">bool</span>, <span class="SNum">10</span>)
        <span class="SItr">@assert</span>(<span class="SItr">@typeof</span>(x.val) == <span class="STpe">#type</span> [<span class="SNum">10</span>] <span class="STpe">bool</span>)
    }
}</span></div>

<h3 id="_130_generic_swg__132_002_where_swg">Where</h3><h4 id="_130_generic_swg__132_002_where_swg_Single_evaluation">Single evaluation </h4>
<p>On a function, you can use <span class="code-inline">where</span> to check if the usage of the function is correct. </p>
<p>If the <span class="code-inline">where</span> expression returns false, then the function will not be considered for the call. If there's no other overload to match, then the compiler will raise an error. </p>
<p>The <span class="code-inline">where</span> expression is evaluated <b>only once</b>, whatever the call, so it is typically used to apply a <b>constraint</b> on generic parameters. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SCmt">// Here we validate the function only if the generic type is `s32` or `s64`.</span>
    <span class="SKwd">func</span>(<span class="SCst">T</span>) <span class="SFct">sum</span>(x: <span class="SCst">T</span>...)-&gt;<span class="SCst">T</span>
        <span class="SLgc">where</span> <span class="SCst">T</span> == <span class="STpe">s32</span> <span class="SLgc">or</span> <span class="SCst">T</span> == <span class="STpe">s64</span>
    {
        <span class="SKwd">var</span> total = <span class="SNum">0</span>'<span class="SCst">T</span>
        <span class="SLgc">visit</span> it <span class="SLgc">in</span> x:
            total += it
        <span class="SLgc">return</span> total
    }

    <span class="SCmt">// This is ok.</span>
    <span class="SKwd">let</span> res1 = <span class="SFct">sum</span>'<span class="STpe">s32</span>(<span class="SNum">1</span>, <span class="SNum">2</span>)
    <span class="SItr">@assert</span>(res1 == <span class="SNum">3</span>)
    <span class="SKwd">let</span> res2 = <span class="SFct">sum</span>'<span class="STpe">s64</span>(<span class="SNum">10</span>, <span class="SNum">20</span>)
    <span class="SItr">@assert</span>(res2 == <span class="SNum">30</span>)

    <span class="SCmt">// But the following would generate an error because the type is `f32`.</span>
    <span class="SCmt">// So there's no match possible for that type.</span>

    <span class="SCmt">//var res1 = sum'f32(1, 2)</span>
}</span></div>
<p>You can use <span class="code-inline">where</span> to make a kind of a generic specialisation. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SCmt">// s32 version</span>
    <span class="SAtr">#[Swag.Overload]</span>
    <span class="SKwd">func</span>(<span class="SCst">T</span>) <span class="SFct">isNull</span>(x: <span class="SCst">T</span>)-&gt;<span class="STpe">bool</span>
        <span class="SLgc">where</span> <span class="SCst">T</span> == <span class="STpe">s32</span>
    {
        <span class="SLgc">return</span> x == <span class="SNum">0</span>
    }

    <span class="SCmt">// f32/f64 version</span>
    <span class="SAtr">#[Swag.Overload]</span>
    <span class="SKwd">func</span>(<span class="SCst">T</span>) <span class="SFct">isNull</span>(x: <span class="SCst">T</span>)-&gt;<span class="STpe">bool</span>
        <span class="SLgc">where</span> <span class="SCst">T</span> == <span class="STpe">f32</span> <span class="SLgc">or</span> <span class="SCst">T</span> == <span class="STpe">f64</span>
    {
        <span class="SLgc">return</span> <span class="SItr">@abs</span>(x) &lt; <span class="SNum">0.01</span>
    }

    <span class="SItr">@assert</span>(<span class="SFct">isNull</span>(<span class="SNum">0</span>'<span class="STpe">s32</span>))
    <span class="SItr">@assert</span>(<span class="SFct">isNull</span>(<span class="SNum">0.001</span>'<span class="STpe">f32</span>))
}</span></div>
<p>Instead of a single expression, <span class="code-inline">where</span> can be followed by a block that needs to return a <span class="code-inline">bool</span> value. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">func</span>(<span class="SCst">T</span>) <span class="SFct">sum</span>(x: <span class="SCst">T</span>...)-&gt;<span class="SCst">T</span>
        <span class="SLgc">where</span>
        {
            <span class="SLgc">if</span> <span class="SItr">@typeof</span>(<span class="SCst">T</span>) == <span class="STpe">s32</span> <span class="SLgc">or</span> <span class="SItr">@typeof</span>(<span class="SCst">T</span>) == <span class="STpe">s64</span>:
                <span class="SLgc">return</span> <span class="SKwd">true</span>
            <span class="SLgc">return</span> <span class="SKwd">false</span>
        }
    {
        <span class="SKwd">var</span> total = <span class="SNum">0</span>'<span class="SCst">T</span>
        <span class="SLgc">visit</span> it <span class="SLgc">in</span> x:
            total += it
        <span class="SLgc">return</span> total
    }
}</span></div>
<p>By using <span class="code-inline">@compilererror</span>, you can then trigger your own errors at compile time if the type is incorrect. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">func</span>(<span class="SCst">T</span>) <span class="SFct">sum</span>(x, y: <span class="SCst">T</span>)-&gt;<span class="SCst">T</span>
        <span class="SLgc">where</span>
        {
            <span class="SLgc">if</span> <span class="SCst">T</span> == <span class="STpe">s32</span> <span class="SLgc">or</span> <span class="SCst">T</span> == <span class="STpe">s64</span>:
                <span class="SLgc">return</span> <span class="SKwd">true</span>
            <span class="SItr">@compilererror</span>(<span class="SStr">"invalid type "</span> ++ <span class="SItr">@stringof</span>(<span class="SCst">T</span>), <span class="SItr">@location</span>(<span class="SCst">T</span>))
            <span class="SLgc">return</span> <span class="SKwd">false</span>
        }
    {
        <span class="SLgc">return</span> x + y
    }

    <span class="SCmt">// This will trigger an error</span>

    <span class="SCmt">// var x = sum'f32(1, 2)</span>
}</span></div>
<p><span class="code-inline">where</span> can also be used on a generic struct. Unlike functions, if the expression failed, then you will have an error right away because there's no overload in the case of structures. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">struct</span>(<span class="SCst">T</span>) <span class="SCst">Point</span>
        <span class="SLgc">where</span> <span class="SCst">T</span> == <span class="STpe">f32</span> <span class="SLgc">or</span> <span class="SCst">T</span> == <span class="STpe">f64</span>
    {
        x, y: <span class="SCst">T</span>
    }

    <span class="SCmt">// Fine.</span>
    <span class="SKwd">var</span> v: <span class="SCst">Point</span>'<span class="STpe">f32</span>

    <span class="SCmt">// Error.</span>
    <span class="SCmt">// var v: Point's32</span>
}</span></div>
<h4 id="_130_generic_swg__132_002_where_swg_Multiple_evaluations">Multiple evaluations </h4>
<p>You can add the specific mode <span class="code-inline">call</span> just after <span class="code-inline">where</span>.  The expression will then not only be called once per instance, but will be evaluated for <b>each</b> call. So it can be used to check call parameters, as long as they can be <b>evaluated at compile time</b>. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    {
        <span class="SKwd">func</span> <span class="SFct">div</span>(x, y: <span class="STpe">s32</span>)-&gt;<span class="STpe">s32</span>
            <span class="SLgc">where</span>(call)
            {
                <span class="SCmt">// Here we use '@isconstexpr'.</span>
                <span class="SCmt">// If 'y' cannot be evaluated at compile time, then we can: nothing about it.</span>
                <span class="SLgc">if</span> !<span class="SItr">@isconstexpr</span>(y):
                    <span class="SLgc">return</span> <span class="SKwd">true</span>
                <span class="SLgc">if</span> y == <span class="SNum">0</span>:
                    <span class="SItr">@compilererror</span>(<span class="SStr">"division by zero"</span>, <span class="SItr">@location</span>(y))
                <span class="SLgc">return</span> <span class="SKwd">true</span>
            }
        {
            <span class="SLgc">return</span> x / y
        }

        <span class="SCmt">// Fine</span>
        <span class="SKwd">var</span> x1 = <span class="SFct">div</span>(<span class="SNum">1</span>, <span class="SNum">1</span>)

        <span class="SCmt">// Error at compile time, division by zero.</span>

        <span class="SCmt">// var x2 = div(1, 0)</span>
    }

    {
        <span class="SCmt">// A version of 'first' where 'x' is known at compile time.</span>
        <span class="SAtr">#[Swag.Overload]</span>
        <span class="SKwd">func</span> <span class="SFct">first</span>(x: <span class="STpe">s32</span>)-&gt;<span class="STpe">s32</span>
            <span class="SLgc">where</span>(call) <span class="SItr">@isconstexpr</span>(x)
        {
            <span class="SLgc">return</span> <span class="SNum">555</span>
        }

        <span class="SCmt">// A version of 'first' where 'x' is **not** known at compile time.</span>
        <span class="SAtr">#[Swag.Overload]</span>
        <span class="SKwd">func</span> <span class="SFct">first</span>(x: <span class="STpe">s32</span>)-&gt;<span class="STpe">s32</span>
            <span class="SLgc">where</span>(call) !<span class="SItr">@isconstexpr</span>(x)
        {
            <span class="SLgc">return</span> <span class="SNum">666</span>
        }

        <span class="SCmt">// Will call version 1 because parameter is a literal.</span>
        <span class="SItr">@assert</span>(<span class="SFct">first</span>(<span class="SNum">0</span>) == <span class="SNum">555</span>)

        <span class="SCmt">// Will call version 2 because parameter is a variable.</span>
        <span class="SKwd">var</span> a: <span class="STpe">s32</span>
        <span class="SItr">@assert</span>(<span class="SFct">first</span>(a) == <span class="SNum">666</span>)
    }
}</span></div>

<h2 id="_140_attributes_swg">Attributes</h2><p>Attributes are tags associated with functions, structures etc... </p>

<h3 id="_140_attributes_swg__141_001_user_attributes_swg">User attributes</h3><p>User attributes are declared like functions, but with the <span class="code-inline">attr</span> keyword before instead of <span class="code-inline">func</span>. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">using</span> <span class="SCst">Swag</span>
<span class="SKwd">attr</span> <span class="SCst">AttributeA</span>()</span></div>
<p>Like functions, attributes can have parameters. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">attr</span> <span class="SCst">AttributeB</span>(x, y: <span class="STpe">s32</span>, z: <span class="STpe">string</span>)</span></div>
<p>So attributes can also have default values. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">attr</span> <span class="SCst">AttributeBA</span>(x: <span class="STpe">s32</span>, y: <span class="STpe">string</span> = <span class="SStr">"string"</span>)</span></div>
<p>You can define a usage before the attribute definition to restrict its usage. </p>
<div class="code-block"><span class="SCde"><span class="SAtr">#[AttrUsage(AttributeUsage.Function)]</span>
<span class="SKwd">attr</span> <span class="SCst">AttributeC</span>()</span></div>
<p>To use an attribute, the syntax is <span class="code-inline">#[attribute, attribute...]</span>. It should be placed <b>before</b> the thing you want to tag. </p>
<div class="code-block"><span class="SCde"><span class="SAtr">#[AttributeA, AttributeB(0, 0, "string")]</span>
<span class="SKwd">func</span> <span class="SFct">function1</span>() {}</span></div>
<p>You can declare multiple usages. </p>
<div class="code-block"><span class="SCde"><span class="SAtr">#[AttrUsage(AttributeUsage.Function | AttributeUsage.Struct)]</span>
<span class="SKwd">attr</span> <span class="SCst">AttributeD</span>(x: <span class="STpe">s32</span>)

<span class="SAtr">#[AttributeD(6)]</span>
<span class="SKwd">func</span> <span class="SFct">function2</span>() {}

<span class="SAtr">#[AttributeD(150)]</span>
<span class="SKwd">struct</span> struct1 {}</span></div>
<p>Finally, attributes can be retrieved at runtime thanks to <b>type reflection</b>. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">let</span> type = <span class="SItr">@typeof</span>(function2) <span class="SCmt">// Get the type of the function</span>
    <span class="SItr">@assert</span>(<span class="SItr">@countof</span>(type.attributes) == <span class="SNum">1</span>) <span class="SCmt">// Check that the function has one attribute associated with it</span>
}</span></div>

<h3 id="_140_attributes_swg__142_002_predefined_attributes_swg">Predefined attributes</h3><p>This is the list of predefined attributes. All are located in the reserved <span class="code-inline">Swag</span> namespace. </p>
<div class="code-block"><span class="SCde"><span class="SCmp">#global</span> skip

<span class="SCmt">// Can be executed at compile time</span>
<span class="SAtr">#[AttrUsage(AttributeUsage.Function | AttributeUsage.Struct)]</span>
<span class="SKwd">attr</span> <span class="SCst">ConstExpr</span>()

<span class="SCmt">// On a function or a struct, this will print the associated generated bytecode (right after generation, without bytecode optimizations)</span>
<span class="SAtr">#[AttrUsage(AttributeUsage.Function | AttributeUsage.Struct | AttributeUsage.File)]</span>
<span class="SKwd">attr</span> <span class="SCst">PrintGenBc</span>()

<span class="SCmt">// On a function or a struct, this will print the associated generated bytecode (after bytecode optimizations)</span>
<span class="SAtr">#[AttrUsage(AttributeUsage.Function | AttributeUsage.Struct | AttributeUsage.File)]</span>
<span class="SKwd">attr</span> <span class="SCst">PrintBc</span>()

<span class="SCmt">// The following function or variable is only defined at compile time</span>
<span class="SAtr">#[AttrUsage(AttributeUsage.Function | AttributeUsage.GlobalVariable | AttributeUsage.Constant)]</span>
<span class="SKwd">attr</span> <span class="SCst">Compiler</span>()

<span class="SCmt">// Force a function to be inlined</span>
<span class="SAtr">#[AttrUsage(AttributeUsage.Function)]</span>
<span class="SKwd">attr</span> <span class="SCst">Inline</span>()

<span class="SCmt">// Never inline the following function.</span>
<span class="SCmt">// This is a hint for the 'llvm' backend.</span>
<span class="SAtr">#[AttrUsage(AttributeUsage.Function)]</span>
<span class="SKwd">attr</span> <span class="SCst">NoInline</span>()

<span class="SCmt">// The following function is a 'macro'</span>
<span class="SAtr">#[AttrUsage(AttributeUsage.Function)]</span>
<span class="SKwd">attr</span> <span class="SCst">Macro</span>()

<span class="SCmt">// The following function is a 'mixin'</span>
<span class="SAtr">#[AttrUsage(AttributeUsage.Function)]</span>
<span class="SKwd">attr</span> <span class="SCst">Mixin</span>()

<span class="SCmt">// Can force an 'opCast' special function to work as implicit</span>
<span class="SAtr">#[AttrUsage(AttributeUsage.Function)]</span>
<span class="SKwd">attr</span> <span class="SCst">Implicit</span>()

<span class="SCmt">// The following switch must be complete</span>
<span class="SAtr">#[AttrUsage(AttributeUsage.Function)]</span>
<span class="SKwd">attr</span> <span class="SCst">Complete</span>()

<span class="SCmt">// The following function can be overloaded</span>
<span class="SAtr">#[AttrUsage(AttributeUsage.Function)]</span>
<span class="SKwd">attr</span> <span class="SCst">Overload</span>()

<span class="SCmt">// A 'return' in the following inlined function must be done in the callee context</span>
<span class="SAtr">#[AttrUsage(AttributeUsage.Function)]</span>
<span class="SKwd">attr</span> <span class="SCst">CalleeReturn</span>()

<span class="SCmt">// The following function is foreign (imported)</span>
<span class="SAtr">#[AttrUsage(AttributeUsage.Function)]</span>
<span class="SKwd">attr</span> <span class="SCst">Foreign</span>(module: <span class="STpe">string</span>, function: <span class="STpe">string</span> = <span class="SStr">""</span>)

<span class="SCmt">// The following function accepts that the called does not use its return value</span>
<span class="SAtr">#[AttrUsage(AttributeUsage.Function | AttributeUsage.Variable)]</span>
<span class="SKwd">attr</span> <span class="SCst">Discardable</span>()

<span class="SCmt">// The following definition is deprecated and should not be used</span>
<span class="SAtr">#[AttrUsage(AttributeUsage.Function | AttributeUsage.Struct | AttributeUsage.Enum | AttributeUsage.EnumValue)]</span>
<span class="SKwd">attr</span> <span class="SCst">Deprecated</span>(msg: <span class="STpe">string</span> = <span class="SKwd">null</span>)

<span class="SCmt">// The following function is forced to not be generic, even if defined inside a generic 'struct'.</span>
<span class="SAtr">#[AttrUsage(AttributeUsage.Function)]</span>
<span class="SKwd">attr</span> <span class="SCst">NotGeneric</span>()

<span class="SCmt">// Put the following global variable in the 'tls' segment.</span>
<span class="SCmt">// A copy of the variable will be available for each thread.</span>
<span class="SAtr">#[AttrUsage(AttributeUsage.GlobalVariable)]</span>
<span class="SKwd">attr</span> <span class="SCst">Tls</span>()

<span class="SCmt">// 'struct' packing information</span>
<span class="SAtr">#[AttrUsage(AttributeUsage.Struct)]</span>
<span class="SKwd">attr</span> <span class="SCst">Pack</span>(value: <span class="STpe">u8</span>)

<span class="SCmt">// The following struct should never be copied</span>
<span class="SAtr">#[AttrUsage(AttributeUsage.Struct)]</span>
<span class="SKwd">attr</span> <span class="SCst">NoCopy</span>()

<span class="SCmt">// When exporting the following struct,: not export its content</span>
<span class="SAtr">#[AttrUsage(AttributeUsage.Struct)]</span>
<span class="SKwd">attr</span> <span class="SCst">Opaque</span>()

<span class="SCmt">// Struct field member relocation.</span>
<span class="SCmt">// The field offset in the struct should be the same as the variable 'name'</span>
<span class="SAtr">#[AttrUsage(AttributeUsage.StructVariable)]</span>
<span class="SKwd">attr</span> <span class="SCst">Offset</span>(name: <span class="STpe">string</span>)

<span class="SCmt">// The following enum is a set of flags</span>
<span class="SAtr">#[AttrUsage(AttributeUsage.Enum)]</span>
<span class="SKwd">attr</span> <span class="SCst">EnumFlags</span>()

<span class="SCmt">// The following enum can be used to index arrays without casting</span>
<span class="SAtr">#[AttrUsage(AttributeUsage.Enum)]</span>
<span class="SKwd">attr</span> <span class="SCst">EnumIndex</span>()

<span class="SCmt">// The following enum can't have duplicated values</span>
<span class="SAtr">#[AttrUsage(AttributeUsage.Enum)]</span>
<span class="SKwd">attr</span> <span class="SCst">NoDuplicate</span>()

<span class="SCmt">// The following switch is incomplete</span>
<span class="SAtr">#[AttrUsage(AttributeUsage.Enum)]</span>
<span class="SKwd">attr</span> <span class="SCst">Incomplete</span>()

<span class="SAtr">#[AttrUsage(AttributeUsage.Struct)]</span>
<span class="SKwd">attr</span> <span class="SCst">ExportType</span>(what: <span class="STpe">string</span>)

<span class="SCmt">// Do not generate documentation.</span>
<span class="SAtr">#[AttrUsage(AttributeUsage.All | AttributeUsage.File)]</span>
<span class="SKwd">attr</span> <span class="SCst">NoDoc</span>()

<span class="SCmt">// Enable/Disable one or more safety checks.</span>
<span class="SCmt">// For example:</span>
<span class="SCmt">// ```swag</span>
<span class="SCmt">// #[Swag.Safety("", false)]                    // Disable all</span>
<span class="SCmt">// #[Swag.Safety("boundcheck|nan", false)]      // Disable 'boundcheck' and 'nan' checks</span>
<span class="SCmt">// ```</span>
<span class="SCmt">// Safety checks are:</span>
<span class="SCmt">// | 'boundcheck'   | Check out of bound access</span>
<span class="SCmt">// | 'overflow'     | Check type conversion lost of bits or precision</span>
<span class="SCmt">// | 'math'         | Various math checks (like a negative '@sqrt')</span>
<span class="SCmt">// | 'switch'       | Check an invalid case in a '#[Swag.Complete]' switch</span>
<span class="SCmt">// | 'unreachable'  | Panic if an '@unreachable' instruction is executed</span>
<span class="SCmt">// | 'any'          | Panic if a cast from a 'any' variable does not match the real underlying type</span>
<span class="SCmt">// | 'bool'         | Panic if a 'bool' does not have a valid value ('true' or 'false')</span>
<span class="SCmt">// | 'nan'          | Panic if a 'nan' is used in a float arithmetic operation</span>
<span class="SCmt">// | 'sanity'       | Do a 'sanity' check (per function)</span>
<span class="SCmt">// | 'null'         | Panic on dereferencing some null pointers</span>
<span class="SCmt">// If 'what' is null or empty, every options are will be affected.</span>
<span class="SAtr">#[AttrUsage(AttributeUsage.All | AttributeUsage.File), AttrMulti]</span>
<span class="SKwd">attr</span> <span class="SCst">Safety</span>(what: <span class="STpe">string</span>, value: <span class="STpe">bool</span>)

<span class="SCmt">// Enable/Disable a given function optimization.</span>
<span class="SCmt">// Options are:</span>
<span class="SCmt">// | 'bytecode'   | Enable/Disable bytecode optimization for the function</span>
<span class="SCmt">// | 'backend'    | Enable/Disable backend machine code optimization for the function (llvm only)</span>
<span class="SCmt">// If 'what' is null or empty, every options will be affected.</span>
<span class="SAtr">#[AttrUsage(AttributeUsage.Function | AttributeUsage.File), AttrMulti]</span>
<span class="SKwd">attr</span> <span class="SCst">Optim</span>(what: <span class="STpe">string</span>, value: <span class="STpe">bool</span>)

<span class="SAtr">#[AttrUsage(AttributeUsage.All | AttributeUsage.File)]</span>
<span class="SKwd">attr</span> <span class="SCst">Overflow</span>(value: <span class="STpe">bool</span>)

<span class="SCmt">// Warning behavior for [[Warning]] attribute</span>
<span class="SKwd">enum</span> <span class="SCst">WarningLevel</span>: <span class="STpe">u8</span>
{
    <span class="SCst">Enable</span>      <span class="SCmt">// Enable the given warning</span>
    <span class="SCst">Disable</span>     <span class="SCmt">// Disable the given warning</span>
    <span class="SCst">Error</span>       <span class="SCmt">// Force the given warning to be raised as an error</span>
}

<span class="SCmt">// Change the behavior of a given warning or list of warnings.</span>
<span class="SCmt">// For example:</span>
<span class="SCmt">// ```swag</span>
<span class="SCmt">// #[Swag.Warning("Wrn0006", Swag.WarningLevel.Error)</span>
<span class="SCmt">// #[Swag.Warning("Wrn0002|Wrn0006", Swag.WarningLevel.Disable)</span>
<span class="SCmt">// #global #[Swag.Warning("Wrn0005", Swag.WarningLevel.Enable)]</span>
<span class="SCmt">// ```</span>
<span class="SCmt">// You can also change the warning behaviors for the whole module in your [[BuildCfg]]</span>
<span class="SAtr">#[AttrUsage(AttributeUsage.All | AttributeUsage.File), AttrMulti]</span>
<span class="SKwd">attr</span> <span class="SCst">Warning</span>(what: <span class="STpe">string</span>, level: <span class="SCst">WarningLevel</span>)

<span class="SAtr">#[AttrUsage(AttributeUsage.All)]</span>
<span class="SKwd">attr</span> <span class="SCst">Match</span>(what: <span class="STpe">string</span>, value: <span class="STpe">bool</span>)

<span class="SKwd">attr</span> <span class="SCst">Strict</span>()
<span class="SKwd">attr</span> <span class="SCst">Global</span>()
<span class="SKwd">attr</span> <span class="SCst">Align</span>(value: <span class="STpe">u8</span>)</span></div>

<h2 id="_160_scoping_swg">Scoping</h2>
<h3 id="_160_scoping_swg__161_001_defer_swg">Defer</h3><p><span class="code-inline">defer</span> is used to call an expression when the current scope is left. It's purely compile time, so it does not evaluate until the block is left. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> v = <span class="SNum">0</span>
    <span class="SLgc">defer</span> <span class="SItr">@assert</span>(v == <span class="SNum">1</span>)
    v += <span class="SNum">1</span>
    <span class="SCmt">// defer expression will be executed here</span>
}</span></div>
<p><span class="code-inline">defer</span> can also be used within a block. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> v = <span class="SNum">0</span>
    <span class="SLgc">defer</span>
    {
        v += <span class="SNum">10</span>
        <span class="SItr">@assert</span>(v == <span class="SNum">15</span>)
    }

    v += <span class="SNum">5</span>
    <span class="SCmt">// defer block will be executed here</span>
}</span></div>
<p><span class="code-inline">defer</span> expressions are called when leaving the corresponding scope, even with <span class="code-inline">return</span>, <span class="code-inline">break</span>, <span class="code-inline">continue</span> etc., and even inside a <span class="code-inline">loop/while/for</span> etc. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> <span class="SCst">G</span> = <span class="SNum">0</span>
    <span class="SLgc">loop</span> <span class="SNum">10</span>
    {
        <span class="SLgc">defer</span> <span class="SCst">G</span> += <span class="SNum">1</span>
        <span class="SLgc">if</span> <span class="SCst">G</span> == <span class="SNum">2</span>:
            <span class="SLgc">break</span> <span class="SCmt">// will be called here, before breaking the loop</span>
        <span class="SCmt">// will be called here also</span>
    }

    <span class="SItr">@assert</span>(<span class="SCst">G</span> == <span class="SNum">3</span>)
}</span></div>
<p><span class="code-inline">defer</span> are executed in reverse order of their declaration. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> x = <span class="SNum">1</span>
    <span class="SLgc">defer</span> <span class="SItr">@assert</span>(x == <span class="SNum">2</span>) <span class="SCmt">// Will be executed second</span>
    <span class="SLgc">defer</span> x *= <span class="SNum">2</span> <span class="SCmt">// Will be executed first</span>
}</span></div>
<p>It's typically used to unregister/destroy a resource, by putting the release code just after the creation one. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">func</span> <span class="SFct">createResource</span>()                 =&gt; <span class="SKwd">true</span>
    <span class="SKwd">func</span> <span class="SFct">releaseResource</span>(resource: *<span class="STpe">bool</span>) = <span class="SKwd">dref</span> resource = <span class="SKwd">false</span>
    <span class="SKwd">func</span> <span class="SFct">isResourceCreated</span>(b: <span class="STpe">bool</span>)       =&gt; b

    <span class="SKwd">var</span> resource = <span class="SKwd">false</span>
    <span class="SLgc">loop</span> <span class="SNum">10</span>
    {
        resource = <span class="SFct">createResource</span>()
        <span class="SLgc">defer</span>
        {
            <span class="SItr">@assert</span>(resource.<span class="SFct">isResourceCreated</span>())
            <span class="SFct">releaseResource</span>(&resource)
        }

        <span class="SLgc">if</span> <span class="SItr">#index</span> == <span class="SNum">2</span>:
            <span class="SLgc">break</span>
    }

    <span class="SItr">@assert</span>(!resource.<span class="SFct">isResourceCreated</span>())
}</span></div>

<h3 id="_160_scoping_swg__162_002_using_swg">Using</h3><p><span class="code-inline">using</span> brings the scope of a namespace, a struct or an enum in the current one. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">enum</span> <span class="SCst">RGB</span> { <span class="SCst">R</span>, <span class="SCst">G</span>, <span class="SCst">B</span> }
    <span class="SItr">@assert</span>(<span class="SCst">RGB</span>.<span class="SCst">R</span> == <span class="SNum">0</span>)

    <span class="SKwd">using</span> <span class="SCst">RGB</span>
    <span class="SItr">@assert</span>(<span class="SCst">G</span> == <span class="SNum">1</span>)
}</span></div>
<p><span class="code-inline">using</span> can also be used with a variable </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">struct</span> <span class="SCst">Point</span> { x: <span class="STpe">s32</span>, y: <span class="STpe">s32</span> }

    <span class="SKwd">var</span> pt: <span class="SCst">Point</span>

    <span class="SKwd">using</span> pt
    x = <span class="SNum">1</span> <span class="SCmt">// no need to specify 'pt'</span>
    y = <span class="SNum">2</span> <span class="SCmt">// no need to specify 'pt'</span>

    <span class="SItr">@assert</span>(pt.x == <span class="SNum">1</span>)
    <span class="SItr">@assert</span>(pt.y == <span class="SNum">2</span>)
}</span></div>
<p>You can declare a variable with <span class="code-inline">using</span> just before. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">struct</span> <span class="SCst">Point</span> { x: <span class="STpe">s32</span>, y: <span class="STpe">s32</span> }
    <span class="SKwd">using</span> <span class="SKwd">var</span> pt: <span class="SCst">Point</span>
    x = <span class="SNum">1</span> <span class="SCmt">// no need to specify 'pt'</span>
    y = <span class="SNum">2</span> <span class="SCmt">// no need to specify 'pt'</span>

    <span class="SItr">@assert</span>(pt.x == <span class="SNum">1</span>)
    <span class="SItr">@assert</span>(pt.y == <span class="SNum">2</span>)
}</span></div>
<h4 id="_160_scoping_swg__162_002_using_swg_For_a_function_parameter">For a function parameter </h4>
<p><span class="code-inline">using</span> applied to a function parameter can be seen as the equivalent of the hidden <span class="code-inline">this</span> in C++. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">struct</span> <span class="SCst">Point</span> { x: <span class="STpe">s32</span>, y: <span class="STpe">s32</span> }

    <span class="SKwd">func</span> <span class="SFct">setOne</span>(<span class="SKwd">using</span> point: *<span class="SCst">Point</span>)
    {
        <span class="SCmt">// Here there's no need to specify 'point'</span>
        x, y = <span class="SNum">1</span>
    }

    <span class="SKwd">var</span> pt: <span class="SCst">Point</span>
    <span class="SFct">setOne</span>(&pt)
    <span class="SItr">@assert</span>(pt.x == <span class="SNum">1</span>)
    <span class="SItr">@assert</span>(pt.y == <span class="SNum">1</span>)

    <span class="SCmt">// ufcs</span>
    pt.<span class="SFct">setOne</span>()
    <span class="SItr">@assert</span>(pt.x == <span class="SNum">1</span>)
    <span class="SItr">@assert</span>(pt.y == <span class="SNum">1</span>)
}</span></div>
<h4 id="_160_scoping_swg__162_002_using_swg_For_a_field">For a field </h4>
<p><span class="code-inline">using</span> can also be used with a field inside a struct. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">struct</span> <span class="SCst">Point2</span>
    {
        x, y: <span class="STpe">s32</span>
    }

    <span class="SKwd">struct</span> <span class="SCst">Point3</span>
    {
        <span class="SKwd">using</span> base:     <span class="SCst">Point2</span>
        z:              <span class="STpe">s32</span>
    }

    <span class="SCmt">// That way the content of the field can be referenced directly</span>
    <span class="SKwd">var</span> value: <span class="SCst">Point3</span>
    value.x = <span class="SNum">0</span> <span class="SCmt">// Equivalent to value.base.x = 0</span>
    value.y = <span class="SNum">0</span> <span class="SCmt">// Equivalent to value.base.y = 0</span>
    value.z = <span class="SNum">0</span>
    <span class="SItr">@assert</span>(&value.x == &value.base.x)
    <span class="SItr">@assert</span>(&value.y == &value.base.y)

    <span class="SCmt">// The compiler can then cast automatically 'Point3' to 'Point2'</span>
    <span class="SKwd">func</span> <span class="SFct">set1</span>(<span class="SKwd">using</span> ptr: *<span class="SCst">Point2</span>)
    {
        x, y = <span class="SNum">1</span>
    }

    <span class="SFct">set1</span>(&value) <span class="SCmt">// Here the cast is automatic thanks to the using</span>
    <span class="SItr">@assert</span>(value.x == <span class="SNum">1</span>)
    <span class="SItr">@assert</span>(value.y == <span class="SNum">1</span>)
    <span class="SItr">@assert</span>(value.base.x == <span class="SNum">1</span>)
    <span class="SItr">@assert</span>(value.base.y == <span class="SNum">1</span>)
}</span></div>

<h3 id="_160_scoping_swg__163_003_with_swg">With</h3><p>You can use <span class="code-inline">with</span> to avoid repeating the same variable again and again. You can then access fields with a simple <span class="code-inline">.</span>. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">struct</span> <span class="SCst">Point</span> { x, y: <span class="STpe">s32</span> }

<span class="SKwd">impl</span> <span class="SCst">Point</span>
{
    <span class="SKwd">mtd</span> <span class="SFct">setOne</span>()
    {
        x, y = <span class="SNum">1</span>
    }
}</span></div>
<p><span class="code-inline">with</span> on a variable. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> pt: <span class="SCst">Point</span>
    <span class="SKwd">with</span> pt
    {
        .x = <span class="SNum">1</span> <span class="SCmt">// equivalent to pt.x</span>
        .y = <span class="SNum">2</span> <span class="SCmt">// equivalent to pt.y</span>
    }

    <span class="SItr">@assert</span>(pt.x == <span class="SNum">1</span>)
    <span class="SItr">@assert</span>(pt.y == <span class="SNum">2</span>)
}</span></div>
<p>Works for function calls to. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> pt: <span class="SCst">Point</span>
    <span class="SKwd">with</span> pt
    {
        .<span class="SFct">setOne</span>() <span class="SCmt">// equivalent to pt.setOne() or setOne(pt)</span>
        .y = <span class="SNum">2</span> <span class="SCmt">// equivalent to pt.y</span>
        <span class="SItr">@assert</span>(.x == <span class="SNum">1</span>) <span class="SCmt">// equivalent to pt.x</span>
        <span class="SItr">@assert</span>(.y == <span class="SNum">2</span>) <span class="SCmt">// equivalent to pt.y</span>
        <span class="SItr">@assert</span>(pt.x == <span class="SNum">1</span>)
        <span class="SItr">@assert</span>(pt.y == <span class="SNum">2</span>)
    }
}</span></div>
<p>Works also with a namespace. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">with</span> <span class="SCst">NameSpace</span>
    {
        .<span class="SFct">inside0</span>()
        .<span class="SFct">inside1</span>()
    }
}</span></div>
<p>Instead of an identifier name, <span class="code-inline">with</span> also accepts a variable declaration. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">with</span> <span class="SKwd">var</span> pt = <span class="SCst">Point</span>{<span class="SNum">1</span>, <span class="SNum">2</span>}
    {
        .x = <span class="SNum">10</span>
        .y = <span class="SNum">20</span>
    }

    <span class="SItr">@assert</span>(pt.x == <span class="SNum">10</span> <span class="SLgc">and</span> pt.y == <span class="SNum">20</span>)
}

<span class="SFct">#test</span>
{
    <span class="SKwd">with</span> <span class="SKwd">var</span> pt: <span class="SCst">Point</span>
    {
        .x = <span class="SNum">10</span>
        .y = <span class="SNum">20</span>
    }

    <span class="SItr">@assert</span>(pt.x == <span class="SNum">10</span> <span class="SLgc">and</span> pt.y == <span class="SNum">20</span>)
}</span></div>
<p>Or an affectation statement. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> pt: <span class="SCst">Point</span>
    <span class="SKwd">with</span> pt = <span class="SCst">Point</span>{<span class="SNum">1</span>, <span class="SNum">2</span>}
    {
        .x = <span class="SNum">10</span>
        .y = <span class="SNum">20</span>
    }

    <span class="SItr">@assert</span>(pt.x == <span class="SNum">10</span> <span class="SLgc">and</span> pt.y == <span class="SNum">20</span>)
}

<span class="SKwd">namespace</span> <span class="SCst">NameSpace</span>
{
    <span class="SKwd">func</span> <span class="SFct">inside0</span>() {}
    <span class="SKwd">func</span> <span class="SFct">inside1</span>() {}
}</span></div>

<h2 id="_164_alias_swg">Alias</h2><h3 id="_164_alias_swg_Type_alias">Type alias </h3>
<p><span class="code-inline">typealias</span> is used to make a shortcut to another type. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">enum</span> <span class="SCst">RGB</span> { <span class="SCst">R</span>, <span class="SCst">G</span>, <span class="SCst">B</span> }
    <span class="SItr">@assert</span>(<span class="SCst">RGB</span>.<span class="SCst">R</span> == <span class="SNum">0</span>)

    <span class="SKwd">typealias</span> <span class="SCst">Color</span> = <span class="SCst">RGB</span> <span class="SCmt">// 'Color' is now equivalent to 'RGB'</span>
    <span class="SItr">@assert</span>(<span class="SCst">Color</span>.<span class="SCst">G</span> == <span class="SNum">1</span>)
}</span></div>
<p>You can then use the new name in place of the original type. This does not create a new type. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">typealias</span> <span class="SCst">Float32</span> = <span class="STpe">f32</span>
    <span class="SKwd">typealias</span> <span class="SCst">Float64</span> = <span class="STpe">f64</span>

    <span class="SKwd">var</span> x: <span class="SCst">Float32</span> = <span class="SNum">1.0</span>     <span class="SCmt">// Same as 'f32'</span>
    <span class="SKwd">var</span> y: <span class="SCst">Float64</span> = <span class="SNum">1.0</span>     <span class="SCmt">// Same as 'f64'</span>
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(<span class="SCst">Float32</span>) == <span class="STpe">f32</span>
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(<span class="SCst">Float64</span>) == <span class="STpe">f64</span>
}</span></div>
<p>But to create a new type, a <span class="code-inline">typealias</span> can also be marked with the <span class="code-inline">Swag.Strict</span> attribute. In that case, all implicit casts won't be done. Explicit cast are still possible. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SAtr">#[Swag.Strict]</span>
    <span class="SKwd">typealias</span> <span class="SCst">MyType</span> = <span class="STpe">s32</span>
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(<span class="SCst">MyType</span>) != <span class="STpe">s32</span>
    <span class="SKwd">var</span> x: <span class="SCst">MyType</span> = <span class="SKwd">cast</span>(<span class="SCst">MyType</span>) <span class="SNum">0</span>
}</span></div>
<h3 id="_164_alias_swg_Name_alias">Name alias </h3>
<p>You can alias a function name with <span class="code-inline">namealias</span> </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">func</span> <span class="SFct">thisIsABigFunctionName</span>(x: <span class="STpe">s32</span>) =&gt; x * x
    <span class="SKwd">namealias</span> myFunc = thisIsABigFunctionName
    <span class="SItr">@assert</span>(<span class="SFct">myFunc</span>(<span class="SNum">4</span>) == <span class="SNum">16</span>)
}</span></div>
<p>You can also alias variables and namespaces </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> myLongVariableName: <span class="STpe">s32</span> = <span class="SNum">0</span>
    <span class="SKwd">namealias</span> short = myLongVariableName
    short += <span class="SNum">2</span>
    <span class="SItr">@assert</span>(myLongVariableName == <span class="SNum">2</span>)
}</span></div>

<h2 id="_170_error_management_swg">Error management</h2><p>In a few words, a function marked with <span class="code-inline">throw</span> can return an error by calling <span class="code-inline">throw</span> followed by the error value. An error value is a struct. If an error has been raised, a caller can either stop its execution and return that same error with <span class="code-inline">try</span>, or it can <span class="code-inline">catch</span> the error and deal with it with a dedicated intrinsic <span class="code-inline">@err()</span>. </p>
<p>So <span class="code-inline">throw Error{}</span> is equivalent to a return, and every rules when leaving a function are the same (call of <span class="code-inline">defer</span>, variables drop and so on). </p>
<div class="blockquote blockquote-default">
<p> These are <b>not</b> exceptions ! You should consider <span class="code-inline">throw</span> as a special <span class="code-inline">return</span>, with a specific value. </p>
</div>
<h3 id="_170_error_management_swg_throw">throw </h3>
<p>A function capable of returning an error must be annotated with <span class="code-inline">throw</span>. This allows the function to raise an error with the same <span class="code-inline">throw</span> keyword, passing an error value in the form of a struct. </p>
<div class="code-block"><span class="SCde"><span class="SCmt">// Defines one error.</span>
<span class="SKwd">struct</span> <span class="SCst">MyError</span>
{
    <span class="SCmt">// The default runtime base error contains some default fields, like a 'message'</span>
    <span class="SKwd">using</span> base: <span class="SCst">Swag</span>.<span class="SCst">BaseError</span>
}</span></div>
<p>Note that when a function returns because of an error, the real return value will always be equal to the <b>default value</b> depending on the type. </p>
<div class="code-block"><span class="SCde"><span class="SCmt">// The function 'count()' can raise an error, so we annotate it with 'throw', at the end of its signature.</span>
<span class="SKwd">func</span> <span class="SFct">count</span>(name: <span class="STpe">string</span>)-&gt;<span class="STpe">u64</span> <span class="SKwd">throw</span>
{
    <span class="SLgc">if</span> name == <span class="SKwd">null</span>
    {
        <span class="SCmt">// Throw a 'MyError' error, and initialize it with a compile-time string.</span>
        <span class="SCmt">// 'count()' will also return 0, because this is the default value for 'u64'.</span>
        <span class="SKwd">throw</span> <span class="SCst">MyError</span>{<span class="SStr">"null pointer"</span>}
    }

    <span class="SLgc">return</span> <span class="SItr">@countof</span>(name)
}</span></div>
<h3 id="_170_error_management_swg_catch">catch </h3>
<p>The caller will then have to deal with the error in some way. </p>
<p>It can <span class="code-inline">catch</span> it, and test (or not) its value with the <span class="code-inline">@err()</span> intrinsic. In that case, the error is dismissed, and the execution will continue at the call site. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">myFunc</span>()
{
    <span class="SCmt">// Dismiss the eventual error with 'catch' and continue execution</span>
    <span class="SKwd">let</span> cpt = <span class="SKwd">catch</span> <span class="SFct">count</span>(<span class="SStr">"fileName"</span>)

    <span class="SCmt">// We can test it with '@err()', which returns the 'throw' corresponding value as long as</span>
    <span class="SCmt">// another error is not raised. Consider this is a good idea to test it right</span>
    <span class="SCmt">// after the 'catch'.</span>
    <span class="SLgc">if</span> <span class="SItr">@err</span>() != <span class="SKwd">null</span>
    {
        <span class="SCmt">// '@err()' returns an 'any', so you can test the type of the error directly with its name</span>
        <span class="SItr">@assert</span>(<span class="SItr">@err</span>() == <span class="SCst">MyError</span>)

        <span class="SCmt">// The function 'count()' should have return the default value, so 0</span>
        <span class="SItr">@assert</span>(cpt == <span class="SNum">0</span>)

        <span class="SItr">@print</span>(<span class="SStr">"an error was raised"</span>)
        <span class="SLgc">return</span>
    }
}</span></div>
<h3 id="_170_error_management_swg_trycatch">trycatch </h3>
<p>Instead of <span class="code-inline">catch</span>, you can use <span class="code-inline">trycatch</span>, which will dismiss the error and exit the current function, returning the default value if necessary. For the caller, no error has been raised. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">myOtherFunc</span>()
{
    <span class="SCmt">// If count() throws an error, we just exit the function without further notice.</span>
    <span class="SKwd">var</span> cpt1 = <span class="SKwd">trycatch</span> <span class="SFct">count</span>(<span class="SStr">"fileName"</span>)

    <span class="SCmt">// This is equivalent to:</span>
    <span class="SKwd">var</span> cpt2 = <span class="SKwd">catch</span> <span class="SFct">count</span>(<span class="SStr">"filename"</span>)
    <span class="SLgc">if</span> <span class="SItr">@err</span>() != <span class="SKwd">null</span>:
        <span class="SLgc">return</span>
}</span></div>
<h3 id="_170_error_management_swg_try">try </h3>
<p>The caller can also <b>stop the execution</b> with <span class="code-inline">try</span>, and return to its own caller with the same error raised. The function must then also be marked with <span class="code-inline">throw</span>. </p>
<p>Here, the caller of <span class="code-inline">myFunc1</span> will also have to deal with the error. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">myFunc1</span>() <span class="SKwd">throw</span>
{
    <span class="SCmt">// If 'count()' raises an error, 'myFunc1' will return with the same error</span>
    <span class="SKwd">var</span> cpt = <span class="SKwd">try</span> <span class="SFct">count</span>(<span class="SStr">"filename"</span>)
}</span></div>
<p>This is equivalent to: </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">myFunc2</span>() <span class="SKwd">throw</span>
{
    <span class="SCmt">// If 'count()' raises an error, 'myFunc2' will return with the same error</span>
    <span class="SKwd">var</span> cpt = <span class="SKwd">catch</span> <span class="SFct">count</span>(<span class="SStr">"filename"</span>)
    <span class="SLgc">if</span> <span class="SItr">@err</span>() != <span class="SKwd">null</span>:
        <span class="SKwd">throw</span> <span class="SItr">@err</span>()
}</span></div>
<h3 id="_170_error_management_swg_assume">assume </h3>
<p>The caller can also panic if an error is raised, with <span class="code-inline">assume</span>. </p>
<div class="blockquote blockquote-default">
<p> This can be disabled in release builds (in that case the behaviour is undefined). </p>
</div>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">myFunc3</span>()
{
    <span class="SCmt">// Here the program will stop with a panic message if 'count()' throws an error</span>
    <span class="SKwd">var</span> cpt = <span class="SKwd">assume</span> <span class="SFct">count</span>(<span class="SStr">"filename"</span>)
}</span></div>
<div class="blockquote blockquote-default">
<p> If an error is never catched, then Swag will panic at runtime, as the top level caller always have an <span class="code-inline">assume</span>. </p>
</div>
<h4 id="_170_error_management_swg_assume_Implicit_assume">Implicit assume </h4>
<p>You can annotate the whole function with <span class="code-inline">assume</span> (instead of <span class="code-inline">throw</span>). This is equivalent of one big block around the function body. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">myFunc3A</span>() <span class="SKwd">assume</span>
{
    <span class="SCmt">// Here the program will stop with a panic message if 'count()' throws an error, but no</span>
    <span class="SCmt">// need to specify 'assume'.</span>
    <span class="SKwd">var</span> cpt = <span class="SFct">count</span>(<span class="SStr">"filename"</span>)

    <span class="SCmt">// Same here. Implicit 'assume'.</span>
    <span class="SKwd">var</span> cpt1 = <span class="SFct">count</span>(<span class="SStr">"filename"</span>)
}</span></div>
<h3 id="_170_error_management_swg_Blocks">Blocks </h3>
<p>You can use a block instead of one single statement (this does not create a scope). </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">myFunc4</span>() <span class="SKwd">throw</span>
{
    <span class="SCmt">// This is not really necessary, see below, but this is just to show 'try' with a block</span>
    <span class="SCmt">// instead of one single call</span>
    <span class="SKwd">try</span>
    {
        <span class="SKwd">var</span> cpt0 = <span class="SFct">count</span>(<span class="SStr">"filename"</span>)
        <span class="SKwd">var</span> cpt1 = <span class="SFct">count</span>(<span class="SStr">"other filename"</span>)
    }

    <span class="SKwd">assume</span>
    {
        <span class="SKwd">var</span> cpt2 = <span class="SFct">count</span>(<span class="SStr">"filename"</span>)
        <span class="SKwd">var</span> cpt3 = <span class="SFct">count</span>(<span class="SStr">"other filename"</span>)
    }

    <span class="SCmt">// Works also for 'catch' if you: not want to deal with the error message.</span>
    <span class="SCmt">// calling '@err()' in that case is not really relevant.</span>
    <span class="SKwd">catch</span>
    {
        <span class="SKwd">var</span> cpt4 = <span class="SFct">count</span>(<span class="SStr">"filename"</span>)
        <span class="SKwd">var</span> cpt5 = <span class="SFct">count</span>(<span class="SStr">"other filename"</span>)
    }

    <span class="SCmt">// Works also for 'trycatch' if you: not want to deal with the error message</span>
    <span class="SCmt">// and you want to return as soon as an error is raised.</span>
    <span class="SKwd">trycatch</span>
    {
        <span class="SKwd">var</span> cpt6 = <span class="SFct">count</span>(<span class="SStr">"filename"</span>)
        <span class="SKwd">var</span> cpt7 = <span class="SFct">count</span>(<span class="SStr">"other filename"</span>)
    }
}</span></div>
<h3 id="_170_error_management_swg_Implicit_try">Implicit try </h3>
<p>When a function is marked with <span class="code-inline">throw</span>, the <span class="code-inline">try</span> for a function call is implicit <b>if not specified</b>. That means that most of the time it's not necessary to specify it if you: not want to (if you: not want to be explicit about it). </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">func</span> <span class="SFct">mySubFunc2</span>() <span class="SKwd">throw</span>
    {
        <span class="SKwd">throw</span> <span class="SCst">MyError</span>{<span class="SStr">"error from mySubFunc2"</span>}
    }

    <span class="SKwd">func</span> <span class="SFct">mySubFunc3</span>() <span class="SKwd">throw</span>
    {
        <span class="SKwd">throw</span> <span class="SCst">MyError</span>{<span class="SStr">"error from mySubFunc3"</span>}
    }

    <span class="SKwd">func</span> <span class="SFct">mySubFunc1</span>() <span class="SKwd">throw</span>
    {
        <span class="SCmt">// There's no need to add a 'try' before the call because 'mySubFunc1' is marked with 'throw'.</span>
        <span class="SCmt">// This is less verbose when you: not want to: something special in case</span>
        <span class="SCmt">// of errors (with 'assume', 'catch' or 'trycatch')</span>
        <span class="SFct">mySubFunc2</span>() <span class="SCmt">// implicit</span>
        <span class="SKwd">try</span> <span class="SFct">mySubFunc3</span>() <span class="SCmt">// explicit</span>
    }

    <span class="SKwd">catch</span> <span class="SFct">mySubFunc1</span>()
    <span class="SItr">@assert</span>(<span class="SItr">@err</span>() == <span class="SCst">MyError</span>)
}</span></div>
<h3 id="_170_error_management_swg_The_error_struct">The error struct </h3>
<p>We have seen that the error value is a struct. This means that you can add some specific error parameters, like the line and column numbers in the case of a syntax error for example. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">struct</span> <span class="SCst">SyntaxError</span>
{
    <span class="SKwd">using</span> base:     <span class="SCst">Swag</span>.<span class="SCst">BaseError</span>
    line, col:      <span class="STpe">u32</span>
}</span></div>
<p>But be aware that a reference to an external value (like a <span class="code-inline">string</span>, an <span class="code-inline">any</span> etc.) must remain valid all the time. The runtime will drop complexe types when needed, so you could store complex things in the heap, or in a dedicated allocator in the current context. </p>
<h3 id="_170_error_management_swg_defer">defer </h3>
<p>Throwing an error is equivalent to returning from the function. So a <span class="code-inline">defer</span> expression works also in that case. </p>
<p>But <span class="code-inline">defer</span> can have a specific mode (<span class="code-inline">err</span> or <span class="code-inline">noerr</span>) to control if it should be executed depending on the error status. </p>
<table class="table-markdown">
<tr><td> <span class="code-inline">defer(err)</span>   </td><td> will be called for each <span class="code-inline">throw</span>, so only when an error is raised</td></tr>
<tr><td> <span class="code-inline">defer(noerr)</span> </td><td> will be called only when the function returns in a normal way</td></tr>
<tr><td> <span class="code-inline">defer</span>       </td><td> will always be called whatever the reason is (normal way or error)</td></tr>
</table>
<div class="code-block"><span class="SCde"><span class="SKwd">var</span> g_Defer = <span class="SNum">0</span>

<span class="SKwd">func</span> <span class="SFct">raiseError</span>() <span class="SKwd">throw</span>
{
    <span class="SKwd">throw</span> <span class="SCst">MyError</span>{<span class="SStr">"error"</span>}
}

<span class="SKwd">func</span> <span class="SFct">testDefer</span>(err: <span class="STpe">bool</span>) <span class="SKwd">throw</span>
{
    <span class="SLgc">defer</span>(err) g_Defer += <span class="SNum">1</span> <span class="SCmt">// This will be called in case an error is raised, before exiting</span>
    <span class="SLgc">defer</span>(noerr) g_Defer += <span class="SNum">2</span> <span class="SCmt">// This will only be called in case an error is not raised</span>
    <span class="SLgc">defer</span> g_Defer += <span class="SNum">3</span> <span class="SCmt">// This will be called in both cases</span>
    <span class="SLgc">if</span> err:
        <span class="SFct">raiseError</span>()
}

<span class="SFct">#test</span>
{
    g_Defer = <span class="SNum">0</span>
    <span class="SKwd">catch</span> <span class="SFct">testDefer</span>(<span class="SKwd">true</span>)
    <span class="SItr">@assert</span>(g_Defer == <span class="SNum">4</span>) <span class="SCmt">// Will call only defer(err) and the normal defer</span>

    g_Defer = <span class="SNum">0</span>
    <span class="SKwd">catch</span> <span class="SFct">testDefer</span>(<span class="SKwd">false</span>)
    <span class="SItr">@assert</span>(g_Defer == <span class="SNum">5</span>) <span class="SCmt">// Will call only defer(noerr) and the normal defer</span>
}</span></div>

<h2 id="_175_safety_swg">Safety</h2><p>Swag comes with a bunch of safety checks which can be activated by module, function or even instruction with the <span class="code-inline">#[Swag.Safety]</span> attribute. </p>
<p>Safety checks can also be changed for a specific build configuration (<span class="code-inline">--cfg:&lt;config&gt;</span>) with <span class="code-inline">buildCfg.safetyGuards</span>. </p>
<div class="blockquote blockquote-default">
<p> Swag comes with four predefined configurations : <span class="code-inline">debug</span>, <span class="code-inline">fast-debug</span>, <span class="code-inline">fast-compile</span> and <span class="code-inline">release</span>. Safety checks are disabled in <span class="code-inline">fast-compile</span> and <span class="code-inline">release</span>. </p>
</div>
<h3 id="_175_safety_swg_overflow">overflow </h3>
<div class="code-block"><span class="SAtr">#[Swag.Safety("overflow", true)]</span></div>
<p>Swag will panic if some operators overflow and if we lose some bits during an integer conversion. </p>
<p>Operators that can overflow are : <span class="code-inline">+ - * &lt;&lt; &gt;&gt;</span> and their equivalent <span class="code-inline">+= -= *= &lt;&lt;= &gt;&gt;=</span>. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> x = <span class="SNum">255</span>'<span class="STpe">u8</span>
    <span class="SCmt">// x += 1      // This would overflow, and panic, because we lose informations</span>
}</span></div>
<p>But if you know what your are doing, you can use a special version of those operators, which will not panic. Add the <span class="code-inline"> #over</span> modifier after the operation. This will disable safety checks. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> x = <span class="SNum">255</span>'<span class="STpe">u8</span>
    x += <span class="SKwd">#over</span> <span class="SNum">1</span> <span class="SCmt">// Overflow is expected, so this will wrap around</span>
    <span class="SItr">@assert</span>(x == <span class="SNum">0</span>)
}</span></div>
<p>You can also use <span class="code-inline">#[Swag.Overflow(true)]</span> to authorize overflow on a more global scale. </p>
<div class="code-block"><span class="SCde"><span class="SAtr">#[Swag.Overflow(true)]</span>
<span class="SFct">#test</span>
{
    <span class="SKwd">var</span> x = <span class="SNum">255</span>'<span class="STpe">u8</span>
    x += <span class="SNum">1</span> <span class="SCmt">// No need for operator modifier ' #over'</span>
    <span class="SItr">@assert</span>(x == <span class="SNum">0</span>)
}</span></div>
<p>For 8 or 16 bits, you can promote an operation to 32 bits by using ' #prom'. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">let</span> x = <span class="SNum">255</span>'<span class="STpe">u8</span> + <span class="SKwd">#prom</span> <span class="SNum">1</span>
    <span class="SItr">@assert</span>(x == <span class="SNum">256</span>)
    <span class="SItr">@assert</span>(<span class="SItr">@typeof</span>(x) == <span class="STpe">u32</span>)
}</span></div>
<p>Swag will also check that a cast does not lose information. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">let</span> x1 = <span class="SNum">255</span>'<span class="STpe">u8</span>

    <span class="SCmt">//var y0 = cast(s8) x1     // This would lose information and panic, as 255 cannot be encoded in 's8'</span>
    <span class="SCmt">//@print(y0)</span>

    <span class="SKwd">let</span> y1 = <span class="SKwd">cast</span>(<span class="STpe">s8</span>) <span class="SKwd">#over</span> x1 <span class="SCmt">// But ' #over' can be used on the cast operation too</span>
    <span class="SItr">@assert</span>(y1 == -<span class="SNum">1</span>)

    <span class="SKwd">let</span> x2 = -<span class="SNum">1</span>'<span class="STpe">s8</span>
    <span class="SCmt">//var y2 = cast(u8) x2     // This cast also is not possible, because 'x2' is negative and 'y' is 'u8'</span>
    <span class="SCmt">//@print(y)</span>
    <span class="SKwd">let</span> y2 = <span class="SKwd">cast</span>(<span class="STpe">u8</span>) <span class="SKwd">#over</span> x2
    <span class="SItr">@assert</span>(y2 == <span class="SNum">255</span>)
}</span></div>
<p>Rember that you can disable these safety checks with the corresponding attribute. </p>
<div class="code-block"><span class="SCde"><span class="SAtr">#[Swag.Overflow(true)]</span>
<span class="SFct">#test</span>
{
    <span class="SKwd">var</span> x = <span class="SNum">255</span>'<span class="STpe">u8</span>
    x += <span class="SNum">255</span> <span class="SCmt">// 254</span>
    x += <span class="SNum">1</span> <span class="SCmt">// 255</span>
    x &gt;&gt;= <span class="SNum">1</span> <span class="SCmt">// 127</span>
    <span class="SItr">@assert</span>(x == <span class="SNum">127</span>)
}</span></div>
<h3 id="_175_safety_swg_any">any </h3>
<div class="code-block"><span class="SAtr">#[Swag.Safety("any", true)]</span></div>
<p>Swag will panic if a bad cast from <span class="code-inline">any</span> is performed. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">let</span> x: <span class="STpe">any</span> = <span class="SStr">"1"</span>
    <span class="SKwd">let</span> y  = <span class="SKwd">cast</span>(<span class="STpe">string</span>) x     <span class="SCmt">// This is valid, because this is the correct underlying type</span>
    <span class="SCmt">//var z = cast(s32) x      // This is not valid, and will panic</span>
    <span class="SCmt">//@assert(z == 0)</span>
}</span></div>
<h3 id="_175_safety_swg_boundcheck">boundcheck </h3>
<div class="code-block"><span class="SAtr">#[Swag.Safety("boundcheck", true)]</span></div>
<p>Swag will panic if an index is out of range when dereferencing a sized value like an array, a slice, a string... </p>
<p>Safety for fixed size arrays. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> x   = [<span class="SNum">0</span>, <span class="SNum">1</span>, <span class="SNum">2</span>]
    <span class="SKwd">var</span> idx = <span class="SNum">10</span>
    <span class="SCmt">//@assert(x[idx] == 1)     // '10' is out of range, will panic</span>
}</span></div>
<p>Safety when indexing a slice. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">let</span> x:  <span class="SKwd">const</span> [..] <span class="STpe">s32</span> = [<span class="SNum">0</span>, <span class="SNum">1</span>, <span class="SNum">2</span>]
    <span class="SKwd">var</span> idx = <span class="SNum">1</span>
    <span class="SItr">@assert</span>(x[idx] == <span class="SNum">1</span>) <span class="SCmt">// '1' is in range, ok</span>
    idx += <span class="SNum">9</span>
    <span class="SCmt">//@assert(x[idx] == 1)      // '10' is out of range, will panic</span>
}</span></div>
<p>Safety when slicing a sized value. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> x: <span class="SKwd">const</span> [..] <span class="STpe">s32</span> = [<span class="SNum">0</span>, <span class="SNum">1</span>, <span class="SNum">2</span>]
    <span class="SCmt">//var slice = x[1..4]              // '4' is out of range, will panic</span>
    <span class="SCmt">//@assert(slice[0] == 1)</span>
}

<span class="SFct">#test</span>
{
    <span class="SKwd">var</span> x   = <span class="SStr">"string"</span>
    <span class="SKwd">var</span> idx = <span class="SNum">10</span>
    <span class="SCmt">//var slice = x[0..idx]            // '10' is out of range, will panic</span>
    <span class="SCmt">//@assert(slice[0] == `s`)</span>
}</span></div>
<h3 id="_175_safety_swg_math">math </h3>
<div class="code-block"><span class="SAtr">#[Swag.Safety("math", true)]</span></div>
<p>Swag will panic if some math operations are invalid. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> x = <span class="SNum">1</span>'<span class="STpe">f32</span>
    <span class="SKwd">var</span> y = <span class="SNum">0</span>'<span class="STpe">f32</span>
    <span class="SCmt">//var z = x / y        // Division by zero, panic</span>
    <span class="SCmt">//@print(z)</span>
}</span></div>
<p>Swag will also check for invalid arguments on some math intrinsics. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SCmt">// All of this will panic if the arguments are unsupported.</span>

    <span class="SCmt">//@abs(-128)</span>
    <span class="SCmt">//@log(-2'f32)</span>
    <span class="SCmt">//@log2(-2'f32)</span>
    <span class="SCmt">//@log10(2'f64)</span>
    <span class="SCmt">//@sqrt(-2'f32)</span>
    <span class="SCmt">//@asin(-2'f32)</span>
    <span class="SCmt">//@acos(2'f32)</span>
}</span></div>
<h3 id="_175_safety_swg_switch">switch </h3>
<div class="code-block"><span class="SAtr">#[Swag.Safety("switch", true)]</span></div>
<p>Swag will panic if a switch is marked with <span class="code-inline">#[Swag.Complete]</span>, but the value is not covered by a <span class="code-inline">case</span>. </p>
<h3 id="_175_safety_swg_bool">bool </h3>
<div class="code-block"><span class="SAtr">#[Swag.Safety("bool", true)]</span></div>
<p>Swag will panic if a boolean value is not <span class="code-inline">true</span> (1) or <span class="code-inline">false</span> (0). </p>
<h3 id="_175_safety_swg_nan">nan </h3>
<div class="code-block"><span class="SAtr">#[Swag.Safety("nan", true)]</span></div>
<p>Swag will panic if a floating point <span class="code-inline">NaN</span> is used in an operation. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
}</span></div>

<h2 id="_180_type_reflection_swg">Type reflection</h2><p>In Swag, <b>types are also values</b> that can be inspected at compile time or at runtime. The two main intrinsics for this are <span class="code-inline">@typeof</span> and <span class="code-inline">@kindof</span>. </p>
<p>You can get the type of an expression with <span class="code-inline">@typeof</span>, or just with the type itself. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">let</span> ptr1 = <span class="SItr">@typeof</span>(<span class="STpe">s8</span>)
    <span class="SItr">@assert</span>(ptr1.name == <span class="SStr">"s8"</span>)
    <span class="SItr">@assert</span>(ptr1 == <span class="STpe">s8</span>)

    <span class="SKwd">let</span> ptr2 = <span class="SItr">@typeof</span>(<span class="STpe">s16</span>)
    <span class="SItr">@assert</span>(ptr2.name == <span class="SStr">"s16"</span>)
    <span class="SItr">@assert</span>(ptr2 == <span class="STpe">s16</span>)

    <span class="SCmt">// See that '@typeof' is not really necessary if the expression on the right is a type.</span>
    <span class="SKwd">let</span> ptr3 = <span class="STpe">s32</span>
    <span class="SItr">@assert</span>(ptr3.name == <span class="SStr">"s32"</span>)
    <span class="SItr">@assert</span>(ptr3 == <span class="SItr">@typeof</span>(<span class="STpe">s32</span>))

    <span class="SKwd">let</span> ptr4 = <span class="STpe">s64</span>
    <span class="SItr">@assert</span>(ptr4.name == <span class="SStr">"s64"</span>)
    <span class="SItr">@assert</span>(ptr4 == <span class="STpe">s64</span>)
}</span></div>
<p>The return result of <span class="code-inline">@typeof</span> is a const pointer to a <span class="code-inline">Swag.TypeInfo</span> kind of structure. This is a typealias for the <span class="code-inline">typeinfo</span> type. All types have a corresponding struct that can be found in the <span class="code-inline">Swag</span> namespace, which is part of the compiler runtime. </p>
<div class="blockquote blockquote-note">
<div class="blockquote-title-block"><i class="fa fa-info-circle"></i>  <span class="blockquote-title">Note</span></div><p> You can find all the type descriptors in the runtime <a href="https://www.swag-lang.org/swag.runtime.php">documentation</a> </p>
</div>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">let</span> ptr = <span class="STpe">bool</span>
    <span class="SItr">@assert</span>(<span class="SItr">@typeof</span>(ptr) == <span class="SItr">@typeof</span>(<span class="SKwd">const</span> *<span class="SCst">Swag</span>.<span class="SCst">TypeInfoNative</span>))

    <span class="SCmt">// '#type' can be used when the right expression is ambiguous. In the case of arrays, it could be</span>
    <span class="SCmt">// a type or the start of an array literal, so '#type' tells the compiler this is a type.</span>
    <span class="SKwd">let</span> ptr1 = <span class="STpe">#type</span> [<span class="SNum">2</span>] <span class="STpe">s32</span>
    <span class="SItr">@assert</span>(<span class="SItr">@typeof</span>(ptr1) == <span class="SItr">@typeof</span>(<span class="SKwd">const</span> *<span class="SCst">Swag</span>.<span class="SCst">TypeInfoArray</span>))
    <span class="SItr">@assert</span>(ptr1.name == <span class="SStr">"[2] s32"</span>)

    <span class="SKwd">let</span> ptr2 = <span class="SItr">@typeof</span>([<span class="SNum">1</span>, <span class="SNum">2</span>, <span class="SNum">3</span>])
    <span class="SItr">@assert</span>(<span class="SItr">@typeof</span>(ptr2) == <span class="SItr">@typeof</span>(<span class="SKwd">const</span> *<span class="SCst">Swag</span>.<span class="SCst">TypeInfoArray</span>))
    <span class="SItr">@assert</span>(ptr2.name == <span class="SStr">"const [3] s32"</span>)
}</span></div>
<p>The <span class="code-inline">TypeInfo</span> structure contains a different enum value for each type. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">let</span> typeOf = <span class="STpe">f64</span>
    <span class="SItr">@assert</span>(typeOf.kind == <span class="SCst">Swag</span>.<span class="SCst">TypeInfoKind</span>.<span class="SCst">Native</span>)

    <span class="SCmt">// This can be evaluated compile-time</span>
    <span class="SKwd">using</span> <span class="SCst">Swag</span>
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(*<span class="STpe">u8</span>).kind == <span class="SCst">TypeInfoKind</span>.<span class="SCst">Pointer</span>
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>([<span class="SNum">1</span>, <span class="SNum">2</span>, <span class="SNum">3</span>]).kind == <span class="SCst">TypeInfoKind</span>.<span class="SCst">Array</span>
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>({<span class="SNum">1</span>, <span class="SNum">2</span>, <span class="SNum">3</span>}).kind == <span class="SCst">TypeInfoKind</span>.<span class="SCst">Struct</span>
}</span></div>
<h3 id="_180_type_reflection_swg_@decltype">@decltype </h3>
<p><span class="code-inline">@decltype</span> can be used to transform a <span class="code-inline">typeinfo</span> to a real compiler type. This is the opposite of <span class="code-inline">@typeof</span> or <span class="code-inline">@kindof</span>. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> x: <span class="SItr">@decltype</span>(<span class="SItr">@typeof</span>(<span class="STpe">s32</span>))
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(x) == <span class="STpe">s32</span>
}</span></div>
<p><span class="code-inline">@decltype</span> can evaluate a <i>constexpr</i> expression that returns a <span class="code-inline">typeinfo</span> to determine the real type. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SAtr">#[Swag.ConstExpr]</span>
    <span class="SKwd">func</span> <span class="SFct">getType</span>(needAString: <span class="STpe">bool</span>)-&gt;<span class="STpe">typeinfo</span>
    {
        <span class="SLgc">if</span> needAString:
            <span class="SLgc">return</span> <span class="STpe">string</span>
        <span class="SLgc">else</span>:
            <span class="SLgc">return</span> <span class="STpe">s32</span>
    }

    <span class="SKwd">var</span> x: <span class="SItr">@decltype</span>(<span class="SFct">getType</span>(needAString: <span class="SKwd">false</span>))
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(x) == <span class="STpe">s32</span>
    x = <span class="SNum">0</span>

    <span class="SKwd">var</span> x1: <span class="SItr">@decltype</span>(<span class="SFct">getType</span>(needAString: <span class="SKwd">true</span>))
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(x1) == <span class="STpe">string</span>
    x1 = <span class="SStr">"0"</span>
}</span></div>

<h2 id="_190_compile-time_evaluation_swg">Compile-time evaluation</h2><p>One thing which is very powerfull with Swag is that <b>everything</b> can be executed compile-time. This is the reason why you can also use it as a scripting language, where the compiler acts as an interpreter. </p>

<h3 id="_190_compile-time_evaluation_swg__191_001_constexpr_swg">Constexpr</h3><p>The attribute <span class="code-inline">#[Swag.ConstExpr]</span> can be used to mark functions. It tells the compiler that this specific function can be called compile-time if possible. </p>
<div class="code-block"><span class="SCde"><span class="SCmt">// 'isThisDebug' is marked with 'Swag.ConstExpr', so it can be automatically evaluated</span>
<span class="SCmt">// at compile time</span>
<span class="SAtr">#[Swag.ConstExpr]</span>
<span class="SKwd">func</span> <span class="SFct">isThisDebug</span>() =&gt; <span class="SKwd">true</span>

<span class="SCmt">// This call is valid, and will be done by the compiler. As 'isThisDebug' returns</span>
<span class="SCmt">// true, then the '#error' inside the '#if' will never be compiled.</span>
<span class="SCmp">#if</span> <span class="SFct">isThisDebug</span>() == <span class="SKwd">false</span>
{
    <span class="SCmp">#error</span> <span class="SStr">"this should not be called !"</span>
}</span></div>
<p>A more complicated compile-time <span class="code-inline">#assert</span>. </p>
<div class="code-block"><span class="SCde"><span class="SAtr">#[Swag.ConstExpr]</span>
<span class="SKwd">func</span> <span class="SFct">factorial</span>(x: <span class="STpe">s32</span>)-&gt;<span class="STpe">s32</span>
{
    <span class="SLgc">if</span> x == <span class="SNum">1</span>:
        <span class="SLgc">return</span> <span class="SNum">1</span>
    <span class="SLgc">return</span> x * <span class="SFct">factorial</span>(x - <span class="SNum">1</span>)
}

<span class="SCmp">#assert</span> <span class="SFct">factorial</span>(<span class="SNum">4</span>) == <span class="SNum">24</span> <span class="SCmt">// Evaluated at compile time</span></span></div>

<h3 id="_190_compile-time_evaluation_swg__192_002_run_swg">Run</h3><h4 id="_190_compile-time_evaluation_swg__192_002_run_swg_Force_compile-time_call">Force compile-time call </h4>
<p><span class="code-inline">#run</span> can be used to call a function that is not marked with <span class="code-inline">#[Swag.ConstExpr]</span>. </p>
<div class="code-block"><span class="SCde"><span class="SCmt">// This time 'isThisRelease' is not marked with 'Swag.ConstExpr'</span>
<span class="SKwd">func</span> <span class="SFct">isThisRelease</span>() =&gt; <span class="SKwd">true</span>

<span class="SCmt">// But this call is still valid because we force the compile time execution with '#run'</span>
<span class="SCmp">#if</span> <span class="SFct">#run</span> <span class="SFct">isThisRelease</span>() == <span class="SKwd">false</span>
{
    <span class="SCmp">#error</span> <span class="SStr">"this should not be called !"</span>
}</span></div>
<p>So that means that you can call everything compile-time, even a function from an external module, a system function etc. </p>
<div class="code-block"><span class="SCde"><span class="SCmt">// This function was not 'prepared' for compile-time evaluation, because there's no</span>
<span class="SCmt">// specific attribute</span>
<span class="SKwd">func</span> <span class="SFct">sum</span>(values: <span class="STpe">s32</span>...)-&gt;<span class="STpe">s32</span>
{
    <span class="SKwd">var</span> result = <span class="SNum">0</span>'<span class="STpe">s32</span>
    <span class="SLgc">visit</span> v <span class="SLgc">in</span> values:
        result += v
    <span class="SLgc">return</span> result
}

<span class="SCmt">// But you can call it compile-time with '#run'</span>
<span class="SKwd">const</span> <span class="SCst">SumValue</span> = <span class="SFct">#run</span> <span class="SFct">sum</span>(<span class="SNum">1</span>, <span class="SNum">2</span>, <span class="SNum">3</span>, <span class="SNum">4</span>) + <span class="SNum">10</span>
<span class="SCmp">#assert</span> <span class="SCst">SumValue</span> == <span class="SNum">20</span></span></div>
<h4 id="_190_compile-time_evaluation_swg__192_002_run_swg_#run_block">#run block </h4>
<p><span class="code-inline">#run</span> is also a special function that will be called by the compiler. You can have as many <span class="code-inline">#run</span> block as you want, but be aware that the execution order in that case is undefined. </p>
<p>It can be used to precompute some global values for example. </p>
<div class="code-block"><span class="SCde"><span class="SCmt">// A global variable we would like to initialize in a 'complexe' way.</span>
<span class="SKwd">var</span> <span class="SCst">G</span>: [<span class="SNum">5</span>] <span class="STpe">f32</span> = <span class="SKwd">undefined</span></span></div>
<p>Initialize <span class="code-inline">G</span> with <span class="code-inline">[1,2,4,8,16]</span> at compile time. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#run</span>
{
    <span class="SKwd">var</span> value = <span class="SNum">1</span>'<span class="STpe">f32</span>
    <span class="SLgc">loop</span> i <span class="SLgc">in</span> <span class="SItr">@countof</span>(<span class="SCst">G</span>)
    {
        <span class="SCst">G</span>[i] = value
        value *= <span class="SNum">2</span>
    }
}</span></div>
<p><span class="code-inline">#test</span> blocks are executed after <span class="code-inline">#run</span>, even at compile time (during testing). So we can test the values of <span class="code-inline">G</span> here. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SItr">@assert</span>(<span class="SCst">G</span>[<span class="SNum">0</span>] == <span class="SNum">1</span>)
    <span class="SItr">@assert</span>(<span class="SCst">G</span>[<span class="SNum">1</span>] == <span class="SNum">2</span>)
    <span class="SItr">@assert</span>(<span class="SCst">G</span>[<span class="SNum">2</span>] == <span class="SNum">4</span>)
    <span class="SItr">@assert</span>(<span class="SCst">G</span>[<span class="SNum">3</span>] == <span class="SNum">8</span>)
    <span class="SItr">@assert</span>(<span class="SCst">G</span>[<span class="SNum">4</span>] == <span class="SNum">16</span>)
}</span></div>
<p>This is where we can see that Swag can be used as a scripting language, because if you have a project with just some <span class="code-inline">#run</span> blocks, you have in fact a... script. </p>
<h4 id="_190_compile-time_evaluation_swg__192_002_run_swg_#run_expression">#run expression </h4>
<p><span class="code-inline">#run</span> can also be used as an expression block. The return type is deduced from the <span class="code-inline">return</span> statement. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">const</span> <span class="SCst">Value</span> = <span class="SFct">#run</span>
    {
        <span class="SKwd">var</span> result: <span class="STpe">f32</span>
        <span class="SLgc">loop</span> <span class="SNum">10</span>:
            result += <span class="SNum">1</span>
        <span class="SLgc">return</span> result <span class="SCmt">// 'Value' will be of type 'f32'</span>
    }
    <span class="SCmp">#assert</span> <span class="SCst">Value</span> == <span class="SNum">10.0</span>
}</span></div>
<p>Can also be used to initialize a static array. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">const</span> <span class="SCst">N</span>           = <span class="SNum">4</span>
    <span class="SKwd">const</span> <span class="SCst">PowerOfTwo</span>: [<span class="SCst">N</span>] <span class="STpe">s32</span> = <span class="SFct">#run</span>
    {
        <span class="SKwd">var</span> arr: [<span class="SCst">N</span>] <span class="STpe">s32</span>
        <span class="SLgc">loop</span> i <span class="SLgc">in</span> arr:
            arr[i] = <span class="SNum">1</span> &lt;&lt; <span class="SKwd">cast</span>(<span class="STpe">u32</span>) i
        <span class="SLgc">return</span> arr
    }

    <span class="SCmp">#assert</span> <span class="SCst">PowerOfTwo</span>[<span class="SNum">0</span>] == <span class="SNum">1</span>
    <span class="SCmp">#assert</span> <span class="SCst">PowerOfTwo</span>[<span class="SNum">1</span>] == <span class="SNum">2</span>
    <span class="SCmp">#assert</span> <span class="SCst">PowerOfTwo</span>[<span class="SNum">2</span>] == <span class="SNum">4</span>
    <span class="SCmp">#assert</span> <span class="SCst">PowerOfTwo</span>[<span class="SNum">3</span>] == <span class="SNum">8</span>
}</span></div>
<p>Can also be used to initialize a string. </p>
<p>This is legit to return a string that is constructed on the stack, because the <span class="code-inline">#run</span> block will make a copy. Remember that a string is a pointer to the bytes (in UTF8 format) and a length. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">const</span> <span class="SCst">MyString</span>: <span class="STpe">string</span> = <span class="SFct">#run</span>
    {
        <span class="SKwd">var</span> str: [<span class="SNum">3</span>] <span class="STpe">u8</span>
        str[<span class="SNum">0</span>] = <span class="SStr">`a`</span>
        str[<span class="SNum">1</span>] = <span class="SStr">`b`</span>
        str[<span class="SNum">2</span>] = str[<span class="SNum">1</span>] + <span class="SNum">1</span>
        <span class="SLgc">return</span> <span class="SKwd">cast</span>(<span class="STpe">string</span>) str
    }
    <span class="SCmp">#assert</span> <span class="SCst">MyString</span> == <span class="SStr">"abc"</span>
}</span></div>
<p>Can also be used to initialize a plain old data struct. Note that you can also force the struct to be considered as a POD by tagging it with <span class="code-inline">#[Swag.ConstExpr]</span>. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">struct</span> <span class="SCst">RGB</span> { r, g, b: <span class="STpe">u8</span> }
    <span class="SKwd">const</span> <span class="SCst">White</span>: <span class="SCst">RGB</span> = <span class="SFct">#run</span>
    {
        <span class="SKwd">var</span> rgb: <span class="SCst">RGB</span> = <span class="SKwd">undefined</span>
        rgb.r = <span class="SNum">255</span>
        rgb.g = rgb.r
        rgb.b = rgb.r
        <span class="SLgc">return</span> rgb
    }
    <span class="SCmp">#assert</span> <span class="SCst">White</span>.r == <span class="SNum">255</span> <span class="SLgc">and</span> <span class="SCst">White</span>.g == <span class="SNum">255</span> <span class="SLgc">and</span> <span class="SCst">White</span>.b == <span class="SNum">255</span>
}</span></div>
<div class="blockquote blockquote-note">
<div class="blockquote-title-block"><i class="fa fa-info-circle"></i>  <span class="blockquote-title">Note</span></div><p> You can also convert a complex struct (which uses the heap for example), as long the struct implements <span class="code-inline">opCount</span> and <span class="code-inline">opSlice</span>. In that case, the resulting type will be a static array. The compiler will call <span class="code-inline">opCount</span> to get the size of the array, and <span class="code-inline">opSlice</span> to initialize its content. If the struct implements <span class="code-inline">opDrop</span>, then it will be called after the conversion to an array has been done. </p>
</div>

<h3 id="_190_compile-time_evaluation_swg__193_003_special_functions_swg">Special functions</h3><div class="code-block"><span class="SCde"><span class="SCmp">#global</span> skip</span></div>
<h4 id="_190_compile-time_evaluation_swg__193_003_special_functions_swg_#main">#main </h4>
<p><span class="code-inline">#main</span> is the program entry point. It can only be defined <b>once</b> per module, and has meaning only for an executable. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#main</span>
{
}</span></div>
<p>Unlike the C function <span class="code-inline">main()</span>, there's no argument, but you can use the intrinsic <span class="code-inline">@args</span> to get a slice of all the specified program arguments. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#main</span>
{
    <span class="SKwd">var</span> myArgs = <span class="SItr">@args</span>()
    <span class="SKwd">var</span> count = <span class="SItr">@countof</span>(myArgs)
    <span class="SLgc">if</span> myArgs[<span class="SNum">0</span>] == <span class="SStr">"fullscreen"</span>
    {
        ...
    }
}</span></div>
<h4 id="_190_compile-time_evaluation_swg__193_003_special_functions_swg_#premain">#premain </h4>
<p><span class="code-inline">#premain</span> will be called after all the modules have done their <span class="code-inline">#init</span> code, but before the <span class="code-inline">#main</span> function is called. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#premain</span>
{
}</span></div>
<h4 id="_190_compile-time_evaluation_swg__193_003_special_functions_swg_#init">#init </h4>
<p><span class="code-inline">#init</span> will be called at runtime, during the module initialization. You can have as many <span class="code-inline">#init</span> as you want, but the execution order in the same module is undefined. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#init</span>
{
}</span></div>
<h4 id="_190_compile-time_evaluation_swg__193_003_special_functions_swg_#drop">#drop </h4>
<p><span class="code-inline">#drop</span> will be called at runtime, when module is unloaded. You can have as many <span class="code-inline">#drop</span> as you want. The execution order in the same module is undefined, but is always the inverse order of <span class="code-inline">#init</span>. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#drop</span>
{
}</span></div>
<h4 id="_190_compile-time_evaluation_swg__193_003_special_functions_swg_#test">#test </h4>
<p><span class="code-inline">#test</span> is a special function than can be used in the <span class="code-inline">tests/</span> folder of the workspace. All <span class="code-inline">#test</span> will be executed only if swag is running in test mode. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
}</span></div>

<h3 id="_190_compile-time_evaluation_swg__194_004_compiler_instructions_swg">Compiler instructions</h3><h4 id="_190_compile-time_evaluation_swg__194_004_compiler_instructions_swg_#assert">#assert </h4>
<p><span class="code-inline">#assert</span> is a static assert (at compile time). </p>
<div class="code-block"><span class="SCde"><span class="SCmp">#assert</span> <span class="SKwd">true</span></span></div>
<p><span class="code-inline">@defined(SYMBOL)</span> returns true, at compile time, if the given symbol exists in the current context. </p>
<div class="code-block"><span class="SCde"><span class="SCmp">#assert</span> !<span class="SItr">@defined</span>(<span class="SCst">DOES_NOT_EXISTS</span>)
<span class="SCmp">#assert</span> <span class="SItr">@defined</span>(<span class="SCst">Global</span>)
<span class="SKwd">var</span> <span class="SCst">Global</span> = <span class="SNum">0</span></span></div>
<h4 id="_190_compile-time_evaluation_swg__194_004_compiler_instructions_swg_#if/#else">#if/#else </h4>
<p>A static <span class="code-inline">#if/#elif/#else</span>, with an expression that can be evaluated at compile time. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">const</span> <span class="SCst">DEBUG</span>   = <span class="SNum">1</span>
<span class="SKwd">const</span> <span class="SCst">RELEASE</span> = <span class="SNum">0</span>
<span class="SCmp">#if</span> <span class="SCst">DEBUG</span>
{
}
<span class="SCmp">#elif</span> <span class="SCst">RELEASE</span>
{
}
<span class="SCmp">#else</span>
{
}</span></div>
<h4 id="_190_compile-time_evaluation_swg__194_004_compiler_instructions_swg_#error/#warning">#error/#warning </h4>
<p><span class="code-inline">#error</span> to raise a compile-time error, and <span class="code-inline">#warning</span> to raise a compile-time warning. </p>
<div class="code-block"><span class="SCde"><span class="SCmp">#if</span> <span class="SKwd">false</span>
{
    <span class="SCmp">#error</span> <span class="SStr">"this is an error"</span>
    <span class="SCmp">#warning</span> <span class="SStr">"this is a warning"</span>
}</span></div>
<h4 id="_190_compile-time_evaluation_swg__194_004_compiler_instructions_swg_#global">#global </h4>
<p>A bunch of <span class="code-inline">#global</span> can be put <b>at the top</b> of a source file. </p>
<div class="code-block"><span class="SCde"><span class="SCmt">// Skip the content of the file (but must be a valid swag file)</span>
<span class="SCmp">#global</span> skip

<span class="SCmt">// All symbols in the file will be public/internal</span>
<span class="SCmp">#global</span> <span class="SKwd">public</span>
<span class="SCmp">#global</span> <span class="SKwd">internal</span>

<span class="SCmt">// All symbols in the file will go in the namespace 'Toto'</span>
<span class="SCmp">#global</span> <span class="SKwd">namespace</span> <span class="SCst">Toto</span>

<span class="SCmt">// A #if for the whole file</span>
<span class="SCmp">#global</span> <span class="SCmp">#if</span> <span class="SCst">DEBUG</span> == <span class="SKwd">true</span>

<span class="SCmt">// Some attributes can be assigned to the full file</span>
<span class="SCmp">#global</span> <span class="SAtr">#[Swag.Safety("", true)]</span>

<span class="SCmt">// The file will be exported for external usage</span>
<span class="SCmt">// It's like putting everything in public, except that the file will</span>
<span class="SCmt">// be copied in its totality in the public folder</span>
<span class="SCmp">#global</span> export</span></div>
<h4 id="_190_compile-time_evaluation_swg__194_004_compiler_instructions_swg_#foreignlib">#foreignlib </h4>
<p>Link with a given external library. </p>
<div class="code-block"><span class="SCde"><span class="SCmp">#foreignlib</span> <span class="SStr">"windows.lib"</span></span></div>

<h2 id="_210_code_inspection_swg">Code inspection</h2><p><span class="code-inline">#message</span> is a special function that will be called by the compiler when something specific occurs during the build. The parameter of <span class="code-inline">#message</span> is a mask that tells the compiler when to call the function. </p>
<p>With the <span class="code-inline">Swag.CompilerMsgMask.SemFunctions</span> flag, for example, <span class="code-inline">#message</span> will be called each time a function of the module <b>has been typed</b>. You can then use <span class="code-inline">getMessage()</span> in the <span class="code-inline">@compiler()</span> interface to retrieve some informations about the reason of the call. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#message</span>(<span class="SCst">Swag</span>.<span class="SCst">CompilerMsgMask</span>.<span class="SCst">SemFunctions</span>)
{
    <span class="SCmt">// Get the interface to communicate with the compiler</span>
    <span class="SKwd">let</span> itf = <span class="SItr">@compiler</span>()

    <span class="SCmt">// Get the current message</span>
    <span class="SKwd">let</span> msg = itf.<span class="SFct">getMessage</span>()

    <span class="SCmt">// We know that the type in the message is a function because of the '#message' mask.</span>
    <span class="SCmt">// So we can safely cast.</span>
    <span class="SKwd">let</span> typeFunc = <span class="SKwd">cast</span>(<span class="SKwd">const</span> *<span class="SCst">Swag</span>.<span class="SCst">TypeInfoFunc</span>) msg.type

    <span class="SCmt">// The message name, for `Swag.CompilerMsgMask.SemFunctions`, is the name of the</span>
    <span class="SCmt">// function being compiled.</span>
    <span class="SKwd">let</span> nameFunc = msg.name

    <span class="SCmt">// As an example, we count that name if it starts with "XX".</span>
    <span class="SCmt">// But we could use custom function attributes instead...</span>
    <span class="SLgc">if</span> <span class="SItr">@countof</span>(nameFunc) &gt; <span class="SNum">2</span> <span class="SLgc">and</span> nameFunc[<span class="SNum">0</span>] == <span class="SStr">`X`</span> <span class="SLgc">and</span> nameFunc[<span class="SNum">1</span>] == <span class="SStr">`X`</span>:
        <span class="SCst">G</span> += <span class="SNum">1</span>
}

<span class="SKwd">var</span> <span class="SCst">G</span> = <span class="SNum">0</span>

<span class="SKwd">func</span> <span class="SCst">XXTestFunc1</span>() {}
<span class="SKwd">func</span> <span class="SCst">XXTestFunc2</span>() {}
<span class="SKwd">func</span> <span class="SCst">XXTestFunc3</span>() {}</span></div>
<p>The compiler will call the following function after the semantic pass. So after <b>all the functions</b> of the module have been parsed. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#message</span>(<span class="SCst">Swag</span>.<span class="SCst">CompilerMsgMask</span>.<span class="SCst">PassAfterSemantic</span>)
{
    <span class="SCmt">// We should have found 3 functions starting with "XX"</span>
    <span class="SItr">@assert</span>(<span class="SCst">G</span> == <span class="SNum">3</span>)
}</span></div>
<p>This will be called for every global variables of the module. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#message</span>(<span class="SCst">Swag</span>.<span class="SCst">CompilerMsgMask</span>.<span class="SCst">SemGlobals</span>)
{
    <span class="SKwd">let</span> itf = <span class="SItr">@compiler</span>()
    <span class="SKwd">var</span> msg = itf.<span class="SFct">getMessage</span>()
}</span></div>
<p>This will be called for every global types of the module (structs, enums, interfaces...). </p>
<div class="code-block"><span class="SCde"><span class="SFct">#message</span>(<span class="SCst">Swag</span>.<span class="SCst">CompilerMsgMask</span>.<span class="SCst">SemTypes</span>)
{
    <span class="SKwd">let</span> itf = <span class="SItr">@compiler</span>()
    <span class="SKwd">var</span> msg = itf.<span class="SFct">getMessage</span>()
}</span></div>

<h2 id="_220_meta_programming_swg">Meta programming</h2><p>In Swag you can construct some source code at compile time, which will then be compiled. The source code you provide in the form of a <b>string</b> must be a valid Swag program. </p>

<h3 id="_220_meta_programming_swg__221_001_ast_swg">Ast</h3><p>The most simple way to produce a string which contains the Swag code to compile is with an <span class="code-inline">#ast</span> block. An <span class="code-inline">#ast</span> block is executed at compile time and the string it returns will be compiled <b>inplace</b>. </p>
<p>The <span class="code-inline">#ast</span> can be a simple expression with the string to compile. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SFct">#ast</span> <span class="SStr">"var x = 666"</span>
    <span class="SItr">@assert</span>(x == <span class="SNum">666</span>)
}</span></div>
<p>Or it can be a block, with an explicit <span class="code-inline">return</span> </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">var</span> cpt = <span class="SNum">2</span>
    <span class="SFct">#ast</span>
    {
        <span class="SKwd">const</span> <span class="SCst">INC</span> = <span class="SNum">5</span>
        <span class="SLgc">return</span> <span class="SStr">"cpt += "</span> ++ <span class="SCst">INC</span> <span class="SCmt">// Equivalent to 'cpt += 5'</span>
    }

    <span class="SItr">@assert</span>(cpt == <span class="SNum">7</span>)
}</span></div>
<h4 id="_220_meta_programming_swg__221_001_ast_swg_Struct_and_enums">Struct and enums </h4>
<p><span class="code-inline">#ast</span> can for example be used to generate the content of a <span class="code-inline">struct</span> or <span class="code-inline">enum</span>. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">struct</span> <span class="SCst">MyStruct</span>
    {
        <span class="SFct">#ast</span>
        {
            <span class="SLgc">return</span> <span class="SStr">"x, y: s32 = 666"</span>
        }
    }

    <span class="SKwd">var</span> v: <span class="SCst">MyStruct</span>
    <span class="SItr">@assert</span>(v.x == <span class="SNum">666</span>)
    <span class="SItr">@assert</span>(v.y == <span class="SNum">666</span>)
}</span></div>
<p>It works with generics too, and can be mixed with static declarations. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">struct</span>(<span class="SCst">T</span>) <span class="SCst">MyStruct</span>
    {
        <span class="SFct">#ast</span>
        {
            <span class="SLgc">return</span> <span class="SStr">"x, y: "</span> ++ <span class="SItr">@typeof</span>(<span class="SCst">T</span>).name
        }

        z: <span class="STpe">string</span>
    }

    <span class="SKwd">var</span> v: <span class="SCst">MyStruct</span>'<span class="STpe">bool</span>
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(v.x) == <span class="STpe">bool</span>
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(v.y) == <span class="STpe">bool</span>
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(v.z) == <span class="STpe">string</span>

    <span class="SKwd">var</span> v1: <span class="SCst">MyStruct</span>'<span class="STpe">f64</span>
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(v1.x) == <span class="STpe">f64</span>
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(v1.y) == <span class="STpe">f64</span>
    <span class="SCmp">#assert</span> <span class="SItr">@typeof</span>(v1.z) == <span class="STpe">string</span>
}</span></div>
<p><span class="code-inline">#ast</span> needs to return a <i>string like</i> value, which can of course be dynamically constructed. In real life we should use the Core.String struct here. But instead we: it by hand, for the purpose of the example. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SAtr">#[Swag.Compiler]</span>
    <span class="SKwd">func</span> <span class="SFct">append</span>(buf: ^<span class="STpe">u8</span>, val: <span class="STpe">string</span>)
    {
        <span class="SKwd">var</span> len = <span class="SNum">0</span>
        <span class="SLgc">while</span> buf[len]:
            len += <span class="SNum">1</span>
        <span class="SItr">@memcpy</span>(buf + len, <span class="SItr">@dataof</span>(val), <span class="SKwd">cast</span>(<span class="STpe">u64</span>) <span class="SItr">@countof</span>(val) + <span class="SNum">1</span>)
    }

    <span class="SKwd">struct</span> <span class="SCst">Vector3</span>
    {
        <span class="SFct">#ast</span>
        {
            <span class="SCmt">// We construct the code to compile in this local array</span>
            <span class="SKwd">var</span> buf: [<span class="SNum">256</span>] <span class="STpe">u8</span>
            <span class="SFct">append</span>(buf, <span class="SStr">"x: f32 = 1\n"</span>)
            <span class="SFct">append</span>(buf, <span class="SStr">"y: f32 = 2\n"</span>)
            <span class="SFct">append</span>(buf, <span class="SStr">"z: f32 = 3\n"</span>)

            <span class="SCmt">// And returns to the compiler the corresponding *code*</span>
            <span class="SLgc">return</span> <span class="SKwd">cast</span>(<span class="STpe">string</span>) buf
        }
    }

    <span class="SKwd">var</span> v: <span class="SCst">Vector3</span>
    <span class="SItr">@assert</span>(v.x == <span class="SNum">1</span>)
    <span class="SItr">@assert</span>(v.y == <span class="SNum">2</span>)
    <span class="SItr">@assert</span>(v.z == <span class="SNum">3</span>)
}</span></div>
<h4 id="_220_meta_programming_swg__221_001_ast_swg_For_example">For example </h4>
<p>This is a real life example of an <span class="code-inline">#ast</span> usage from the <span class="code-inline">Std.Core</span> module. Here we generate a structure which contains all the fields of an original other structure, but where the types are forced to be <span class="code-inline">bool</span>. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">struct</span>(<span class="SCst">T</span>) <span class="SCst">IsSet</span>
{
    <span class="SFct">#ast</span>
    {
        <span class="SCmt">// A `StringBuilder` is used to manipulate dynamic strings.</span>
        <span class="SKwd">var</span> str = <span class="SCst">StrConv</span>.<span class="SCst">StringBuilder</span>{}

        <span class="SCmt">// We get the type of the generic parameter 'T'</span>
        <span class="SKwd">let</span> typeof = <span class="SItr">@typeof</span>(<span class="SCst">T</span>)

        <span class="SCmt">// Then we visit all the fields, assuming the type is a struct (or this will not compile).</span>
        <span class="SCmt">// For each original field, we create one with the same name, but with a `bool` type.</span>
        <span class="SLgc">visit</span> f <span class="SLgc">in</span> typeof.fields
            str.<span class="SFct">appendFormat</span>(<span class="SStr">"%: bool\n"</span>, f.name)

        <span class="SCmt">// Then we return the constructed source code.</span>
        <span class="SCmt">// It will be used by the compiler to generate the content of the `IsSet` struct.</span>
        <span class="SLgc">return</span> str.<span class="SFct">toString</span>()
    }
}</span></div>
<h4 id="_220_meta_programming_swg__221_001_ast_swg_At_global_scope">At global scope </h4>
<p><span class="code-inline">#ast</span> can also be called at the global scope. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#ast</span>
{
    <span class="SKwd">const</span> value = <span class="SNum">666</span>
    <span class="SLgc">return</span> <span class="SStr">"const myGeneratedConst = "</span> ++ value
}</span></div>
<p>But be aware that you must use <span class="code-inline">#placeholder</span> in case you are generating global symbols that can be used by something else in the code. This will tell Swag that <i>this symbol</i> will exist at some point, so please wait for it to <i>exist</i> before complaining. </p>
<div class="code-block"><span class="SCde"><span class="SCmp">#placeholder</span> myGeneratedConst <span class="SCmt">// Symbol `myGeneratedConst` will be generated</span></span></div>
<p>Here for example, thanks to the <span class="code-inline">#placeholder</span>, the <span class="code-inline">#assert</span> will wait for the symbol <span class="code-inline">myGeneratedConst</span> to be replaced with its real content. </p>
<div class="code-block"><span class="SCde"><span class="SCmp">#assert</span> myGeneratedConst == <span class="SNum">666</span></span></div>

<h3 id="_220_meta_programming_swg__222_002_compiler_interface_swg">Compiler interface</h3><p>The other method to compile generated code is to use the function <span class="code-inline">compileString()</span> in the <span class="code-inline">@compiler()</span> interface. Of course this should be called at compile time, and mostly during a <span class="code-inline">#message</span> call. </p>
<p>Here is a real life example from the <span class="code-inline">Std.Ogl</span> module (opengl wrapper), which uses <span class="code-inline">#message</span> to track functions marked with a specific <b>user attribute</b> <span class="code-inline">Ogl.Extension</span>, and generates some code for each function that has been found. </p>
<p>First we declare a new specific attribute, which can then be associated with a function. </p>
<div class="code-block"><span class="SCde"><span class="SAtr">#[AttrUsage(AttributeUsage.Function)]</span>
<span class="SKwd">attr</span> <span class="SCst">Extension</span>()

<span class="SCmt">// Here is an example of usage of that attribute.</span>
<span class="SAtr">#[Extension]</span>
{
    <span class="SKwd">func</span> <span class="SFct">glUniformMatrix2x3fv</span>(location: <span class="SCst">GLint</span>, count: <span class="SCst">GLsizei</span>, transpose: <span class="SCst">GLboolean</span>, value: <span class="SKwd">const</span> *<span class="SCst">GLfloat</span>);
    <span class="SKwd">func</span> <span class="SFct">glUniformMatrix2x4fv</span>(location: <span class="SCst">GLint</span>, count: <span class="SCst">GLsizei</span>, transpose: <span class="SCst">GLboolean</span>, value: <span class="SKwd">const</span> *<span class="SCst">GLfloat</span>);
    <span class="SKwd">func</span> <span class="SFct">glUniformMatrix3x2fv</span>(location: <span class="SCst">GLint</span>, count: <span class="SCst">GLsizei</span>, transpose: <span class="SCst">GLboolean</span>, value: <span class="SKwd">const</span> *<span class="SCst">GLfloat</span>);
    <span class="SKwd">func</span> <span class="SFct">glUniformMatrix3x4fv</span>(location: <span class="SCst">GLint</span>, count: <span class="SCst">GLsizei</span>, transpose: <span class="SCst">GLboolean</span>, value: <span class="SKwd">const</span> *<span class="SCst">GLfloat</span>);
    <span class="SKwd">func</span> <span class="SFct">glUniformMatrix4x2fv</span>(location: <span class="SCst">GLint</span>, count: <span class="SCst">GLsizei</span>, transpose: <span class="SCst">GLboolean</span>, value: <span class="SKwd">const</span> *<span class="SCst">GLfloat</span>);
    <span class="SKwd">func</span> <span class="SFct">glUniformMatrix4x3fv</span>(location: <span class="SCst">GLint</span>, count: <span class="SCst">GLsizei</span>, transpose: <span class="SCst">GLboolean</span>, value: <span class="SKwd">const</span> *<span class="SCst">GLfloat</span>);
}</span></div>
<p>The following will be used to track the functions with that specific attribute. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">struct</span> <span class="SCst">OneFunc</span>
{
    type: <span class="STpe">typeinfo</span>
    name: <span class="STpe">string</span>
}

<span class="SAtr">#[Compiler]</span>
<span class="SKwd">var</span> g_Functions: <span class="SCst">Array</span>'<span class="SCst">OneFunc</span></span></div>
<p>This <span class="code-inline">#message</span> will be called for each function of the <span class="code-inline">Ogl</span> module. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#message</span>(<span class="SCst">CompilerMsgMask</span>.<span class="SCst">SemFunctions</span>)
{
    <span class="SKwd">let</span> itf = <span class="SItr">@compiler</span>()
    <span class="SKwd">var</span> msg = itf.<span class="SFct">getMessage</span>()

    <span class="SCmt">// If the function does not have our attribute, forget it</span>
    <span class="SLgc">if</span> !<span class="SCst">Reflection</span>.<span class="SFct">hasAttribute</span>(msg.type, <span class="SCst">Extension</span>)
        <span class="SLgc">return</span>

    <span class="SCmt">// We just track all the functions with the given attribute</span>
    g_Functions.<span class="SFct">add</span>({msg.type, msg.name})
}</span></div>
<p>We will generate a <span class="code-inline">glInitExtensions</span> global function, so we register it as a placeholder. </p>
<div class="code-block"><span class="SCde"><span class="SCmp">#placeholder</span> glInitExtensions</span></div>
<p>This is called once all functions of the module have been typed, and this is the main code generation. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#message</span>(<span class="SCst">CompilerMsgMask</span>.<span class="SCst">PassAfterSemantic</span>)
{
    <span class="SKwd">var</span> builderVars: <span class="SCst">StringBuilder</span>
    <span class="SKwd">var</span> builderInit: <span class="SCst">StringBuilder</span>

    <span class="SCmt">// Generate the `glInitExtensions` function</span>
    builderInit.<span class="SFct">appendString</span>(<span class="SStr">"public func glInitExtensions()\n{\n"</span>);

    <span class="SCmt">// Visit all functions we have registered, i.e. all functions with the `Ogl.Extension` attribute.</span>
    <span class="SLgc">visit</span> e <span class="SLgc">in</span> g_Functions
    {
        <span class="SKwd">let</span> typeFunc = <span class="SKwd">cast</span>(<span class="SKwd">const</span> *<span class="SCst">TypeInfoFunc</span>) e.type

        <span class="SCmt">// Declare a lambda variable for that extension</span>
        builderVars.<span class="SFct">appendFormat</span>(<span class="SStr">"var ext_%: %\n"</span>, e.name, typeFunc.name)

        <span class="SCmt">// Make a wrapper function</span>
        builderVars.<span class="SFct">appendFormat</span>(<span class="SStr">"public func %("</span>, e.name)
        <span class="SLgc">visit</span> p, i <span class="SLgc">in</span> typeFunc.parameters
        {
            <span class="SLgc">if</span> i != <span class="SNum">0</span> builderVars.<span class="SFct">appendString</span>(<span class="SStr">", "</span>)
            builderVars.<span class="SFct">appendFormat</span>(<span class="SStr">"p%: %"</span>, i, p.pointedType.name)
        }

        <span class="SLgc">if</span> typeFunc.returnType == <span class="STpe">void</span>
            builderVars.<span class="SFct">appendFormat</span>(<span class="SStr">")\n{\n"</span>)
        <span class="SLgc">else</span>
            builderVars.<span class="SFct">appendFormat</span>(<span class="SStr">")-&gt;%\n{\n"</span>, typeFunc.returnType.name)
        builderVars.<span class="SFct">appendFormat</span>(<span class="SStr">"\treturn ext_%("</span>, e.name)
        <span class="SLgc">visit</span> p, i <span class="SLgc">in</span> typeFunc.parameters
        {
            <span class="SLgc">if</span> i != <span class="SNum">0</span> builderVars.<span class="SFct">appendString</span>(<span class="SStr">", "</span>)
            builderVars.<span class="SFct">appendFormat</span>(<span class="SStr">"p%"</span>, i)
        }

        builderVars.<span class="SFct">appendString</span>(<span class="SStr">");\n}\n\n"</span>)

        <span class="SCmt">// Initialize the variable with the getExtensionAddress</span>
        builderInit.<span class="SFct">appendFormat</span>(<span class="SStr">"\text_% = cast(%) getExtensionAddress(@dataof(\"%\"))\n"</span>, e.name, typeFunc.name, e.name);
    }

    <span class="SCmt">// Compile !!</span>
    <span class="SKwd">let</span> itf = <span class="SItr">@compiler</span>()
    <span class="SKwd">var</span> str = builderVars.<span class="SFct">toString</span>()
    itf.<span class="SFct">compileString</span>(str.<span class="SFct">toString</span>())

    builderInit.<span class="SFct">appendString</span>(<span class="SStr">"}\n"</span>);
    str = builderInit.<span class="SFct">toString</span>()
    itf.<span class="SFct">compileString</span>(str.<span class="SFct">toString</span>())
}</span></div>

<h2 id="_230_documentation_md">Documentation</h2><p>The Swag compiler can generate documentation for all the modules of a given workspace. </p>
<div class="code-block"><span class="SCde">swag doc -w:myWorkspaceFolder</span></div>
<p>Swag can generate documentations in various modes. That mode should be specified in the <span class="code-inline">module.swg</span> file, in the <a href="swag.runtime.php#Swag_BuildCfg">Swag.BuildCfg</a> struct. </p>
<div class="code-block"><span class="SCde"><span class="SFct">#dependencies</span>
{
    <span class="SCmp">#import</span> <span class="SStr">"pixel"</span>

    <span class="SFct">#run</span>
    {
        <span class="SKwd">let</span> itf = <span class="SItr">@compiler</span>()
        <span class="SKwd">let</span> cfg = itf.<span class="SFct">getBuildCfg</span>()
        cfg.genDoc.kind = .<span class="SCst">Api</span> <span class="SCmt">// Specify the documentation generation mode</span>
    }
}</span></div>
<table class="table-markdown">
<tr><th style="text-align: left;">Kind</th><th style="text-align: left;">Purpose</th></tr><tr><td style="text-align: left;"> Swag.DocKind.Api      </td><td style="text-align: left;"> Generates an api documentation (all public symbols)</td></tr>
<tr><td style="text-align: left;"> Swag.DocKind.Examples </td><td style="text-align: left;"> Generates a documentation like this one</td></tr>
<tr><td style="text-align: left;"> Swag.DocKind.Pages    </td><td style="text-align: left;"> Generates different pages, where each file is a page (a variation of  <span class="code-inline">Examples</span>)</td></tr>
</table>
<h3 id="_230_documentation_md_Markdown_files">Markdown files </h3>
<p>If the module contains <b>markdown</b> files with the <span class="code-inline">.md</span> extension, they will be processed as if they were Swag comments. </p>
<h3 id="_230_documentation_md_Format_of_comments">Format of comments </h3>
<h4 id="_230_documentation_md_Format_of_comments_Paragraphs">Paragraphs </h4>
<div class="code-block"><span class="SCde"><span class="SCmt">// Everything between empty lines is considered to be a simple paragraph. Which</span>
<span class="SCmt">// means that if you put several comments on several lines like this, they all</span>
<span class="SCmt">// will be part of the same paragraph.</span>
<span class="SCmt">//</span>
<span class="SCmt">// This is another paragraph because there's an empty line before.</span>
<span class="SCmt">//</span>
<span class="SCmt">// This is yet another paragraph.</span></span></div>
<div class="blockquote blockquote-example">
<div class="blockquote-title-block"><i class="fa fa-magnifying-glass"></i>  <span class="blockquote-title">Result</span></div><p> Everything between empty lines is considered to be a simple paragraph. Which  means that if you put several comments on several lines like this, they all  will be part of the same paragraph. </p>
<p> This is another paragraph because there's an empty line before. </p>
<p> This is yet another paragraph. </p>
</div>
<p>Inside a paragraph, you can end of line with <span class="code-inline">\</span> to force a break without creating a new paragraph. </p>
<div class="code-block"><span class="SCde"><span class="SCmt">// First line.</span>
<span class="SCmt">// Second line is on first line.\</span>
<span class="SCmt">// But third line has a break before.</span></span></div>
<div class="blockquote blockquote-example">
<div class="blockquote-title-block"><i class="fa fa-magnifying-glass"></i>  <span class="blockquote-title">Result</span></div><p> First line.  Second line is on first line.<br/>  But third line has a break before. </p>
</div>
<p>A paragraph that starts with <span class="code-inline">---</span> is a <b>verbatim</b> paragraph where every blanks and end of lines are respected. The paragraph will be generated <b>as is</b> without any markdown change. </p>
<div class="code-block"><span class="SCde"><span class="SCmt">// ---</span>
<span class="SCmt">// Even...</span>
<span class="SCmt">//</span>
<span class="SCmt">// ...empty lines are preserved.</span>
<span class="SCmt">//</span>
<span class="SCmt">// You end that kind of paragraph with another '---' alone on its line.</span>
<span class="SCmt">// Note that **everything** is not bold, put printed 'as it is'.</span>
<span class="SCmt">// ---</span></span></div>
<div class="blockquote blockquote-example">
<div class="blockquote-title-block"><i class="fa fa-magnifying-glass"></i>  <span class="blockquote-title">Result</span></div><p style="white-space: break-spaces"> Even...

 ...empty lines are preserved.

 You end that kind of paragraph with another '---' alone on its line.
 Note that **everything** is not bold, put printed 'as it is'.</p>
</div>
<h4 id="_230_documentation_md_Format_of_comments_Lists">Lists </h4>
<p>You can create a <b>list</b> of bullet points with <span class="code-inline">*</span>. </p>
<div class="code-block"><span class="SCde"><span class="SCmt">// * This is a bullet point</span>
<span class="SCmt">// * This is a bullet point</span>
<span class="SCmt">// * This is a bullet point</span></span></div>
<div class="blockquote blockquote-example">
<div class="blockquote-title-block"><i class="fa fa-magnifying-glass"></i>  <span class="blockquote-title">Result</span></div><ul>
<li>This is a bullet point</li>
<li>This is a bullet point</li>
<li>This is a bullet point</li>
</ul>
</div>
<p>You can create an <b>ordered</b> list by starting the line with a digit followed by a <span class="code-inline">.</span>. </p>
<div class="code-block"><span class="SCde"><span class="SCmt">// 1. This is an ordered list</span>
<span class="SCmt">// 1. The digit itself does not matter, real numbers will be computed</span>
<span class="SCmt">// 0. This is another one</span></span></div>
<div class="blockquote blockquote-example">
<div class="blockquote-title-block"><i class="fa fa-magnifying-glass"></i>  <span class="blockquote-title">Result</span></div><ol>
<li>This is an ordered list</li>
<li>The digit itself does not matter, real numbers will be computed</li>
<li>This is another one</li>
</ol>
</div>
<div class="blockquote blockquote-warning">
<div class="blockquote-title-block"><i class="fa fa-exclamation-triangle"></i>  <span class="blockquote-title">Warning</span></div><p> Swag only supports single line list items. You cannot have complex paragraphs (or sub lists). </p>
</div>
<h4 id="_230_documentation_md_Format_of_comments_Definition_Lists">Definition Lists </h4>
<p>You can add a definition title with the <span class="code-inline">+</span> character followed by a blank, and then the title. The description paragraph should come just after the title, with at least 4 blanks or one tabulation. </p>
<div class="code-block"><span class="SCde"><span class="SCmt">// + Title</span>
<span class="SCmt">//     This is the description.</span>
<span class="SCmt">// + Other title</span>
<span class="SCmt">//     This is the other description.</span></span></div>
<div class="blockquote blockquote-example">
<div class="blockquote-title-block"><i class="fa fa-magnifying-glass"></i>  <span class="blockquote-title">Result</span></div><div class="description-list-title"><p>Title</p></div>
<div class="description-list-block">
<p> This is the description. </p>
</div>
<div class="description-list-title"><p>Other title</p></div>
<div class="description-list-block">
<p> This is the other description. </p>
</div>
</div>
<p>A description can contain complex paragraphs. </p>
<div class="code-block"><span class="SCde"><span class="SCmt">// + Title</span>
<span class="SCmt">//     This is an embedded list.</span>
<span class="SCmt">//     * Item1</span>
<span class="SCmt">//     * Item2</span></span></div>
<div class="blockquote blockquote-example">
<div class="blockquote-title-block"><i class="fa fa-magnifying-glass"></i>  <span class="blockquote-title">Result</span></div><div class="description-list-title"><p>Title</p></div>
<div class="description-list-block">
<p> This is an embedded list. </p>
<ul>
<li>Item1</li>
<li>Item2</li>
</ul>
</div>
</div>
<p>The description paragraph can contain some empty lines. </p>
<div class="code-block"><span class="SCde"><span class="SCmt">// + Other title</span>
<span class="SCmt">//</span>
<span class="SCmt">//     This is the other description</span>
<span class="SCmt">//     on more than one line.</span></span></div>
<div class="blockquote blockquote-example">
<div class="blockquote-title-block"><i class="fa fa-magnifying-glass"></i>  <span class="blockquote-title">Result</span></div><div class="description-list-title"><p>Other title</p></div>
<div class="description-list-block">
<p> This is the other description  on more than one line. </p>
</div>
</div>
<h4 id="_230_documentation_md_Format_of_comments_Quotes">Quotes </h4>
<p>You can create a <b>quote</b> with <span class="code-inline">&gt;</span> </p>
<div class="code-block"><span class="SCde"><span class="SCmt">// &gt; This is a block quote on multiple</span>
<span class="SCmt">// &gt; lines.</span>
<span class="SCmt">// &gt;</span>
<span class="SCmt">// &gt; End of the quote.</span></span></div>
<div class="blockquote blockquote-default">
<p> This is a block quote on multiple  lines. </p>
<p> End of the quote. </p>
</div>
<p>You can create a special quote by adding a title on the first line. There must be exactly one blank between <span class="code-inline">&gt;</span> and the title, and the title case should be respected. </p>
<ul>
<li><span class="code-inline">NOTE:</span></li>
<li><span class="code-inline">TIP:</span></li>
<li><span class="code-inline">WARNING:</span></li>
<li><span class="code-inline">ATTENTION:</span></li>
<li><span class="code-inline">EXAMPLE:</span></li>
</ul>
<div class="code-block"><span class="SCde"><span class="SCmt">// &gt; NOTE:</span>
<span class="SCmt">// &gt; This is the note content</span></span></div>
<div class="blockquote blockquote-note">
<div class="blockquote-title-block"><i class="fa fa-info-circle"></i>  <span class="blockquote-title">Note</span></div><p> This is the note content </p>
</div>
<div class="code-block"><span class="SCde"><span class="SCmt">// &gt; TIP:</span>
<span class="SCmt">// &gt; This is a tip.</span></span></div>
<div class="blockquote blockquote-tip">
<div class="blockquote-title-block"><i class="fa fa-lightbulb-o"></i>  <span class="blockquote-title">Tip</span></div><p> This is a tip. </p>
</div>
<div class="code-block"><span class="SCde"><span class="SCmt">// &gt; WARNING:</span>
<span class="SCmt">// &gt; This is the warning content</span>
<span class="SCmt">// &gt;</span>
<span class="SCmt">// &gt; Another paragraph</span></span></div>
<div class="blockquote blockquote-warning">
<div class="blockquote-title-block"><i class="fa fa-exclamation-triangle"></i>  <span class="blockquote-title">Warning</span></div><p> This is the warning content </p>
<p> Another paragraph </p>
</div>
<div class="code-block"><span class="SCmt">// &gt; ATTENTION: The content of the quote can be written on the same line as the title</span></div>
<div class="blockquote blockquote-attention">
<div class="blockquote-title-block"><i class="fa fa-ban"></i>  <span class="blockquote-title">Attention</span></div><p> The content of the quote can be written on the same line as the title </p>
</div>
<div class="code-block"><span class="SCde"><span class="SCmt">// &gt; EXAMPLE:</span>
<span class="SCmt">// &gt; In the 'module.swg' file, we have changed the 'example' title to be `"Result"` instead of `"Example"`.</span></span></div>
<div class="blockquote blockquote-example">
<div class="blockquote-title-block"><i class="fa fa-magnifying-glass"></i>  <span class="blockquote-title">Result</span></div><p> In the <span class="code-inline">module.swg</span> file, we have changed the <span class="code-inline">example</span> title to be <span class="code-inline">"Result"</span> instead of <span class="code-inline">"Example"</span>. </p>
</div>
<h4 id="_230_documentation_md_Format_of_comments_Tables">Tables </h4>
<p>You can create a <b>table</b> by starting a line with <span class="code-inline">|</span>. Each column must then be separated with <span class="code-inline">|</span>. The last column can end with <span class="code-inline">|</span>, but this is not mandatory. </p>
<div class="code-block"><span class="SCde"><span class="SCmt">// A table with 4 lines of 2 columns:</span>
<span class="SCmt">// | boundcheck   | Check out of bound access</span>
<span class="SCmt">// | overflow     | Check type conversion lost of bits or precision</span>
<span class="SCmt">// | math         | Various math checks (like a negative '@sqrt')        |</span>
<span class="SCmt">// | switch       | Check an invalid case in a '#[Swag.Complete]' switch |</span></span></div>
<div class="blockquote blockquote-example">
<div class="blockquote-title-block"><i class="fa fa-magnifying-glass"></i>  <span class="blockquote-title">Result</span></div><table class="table-markdown">
<tr><td> boundcheck   </td><td> Check out of bound access</td></tr>
<tr><td> overflow     </td><td> Check type conversion lost of bits or precision</td></tr>
<tr><td> math         </td><td> Various math checks (like a negative <span class="code-inline">@sqrt</span>)        </td></tr>
<tr><td> switch       </td><td> Check an invalid case in a <span class="code-inline">#[Swag.Complete]</span> switch </td></tr>
</table>
</div>
<p>You can define a header to the table if you separate the first line from the second one with a separator like <span class="code-inline">---</span>. A valid separator must have a length of at least 3 characters, and must only contain <span class="code-inline">-</span> or <span class="code-inline">:</span>. </p>
<div class="code-block"><span class="SCde"><span class="SCmt">// | Title1 | Title2</span>
<span class="SCmt">// | ------ | ------</span>
<span class="SCmt">// | Item1  | Item2</span>
<span class="SCmt">// | Item1  | Item2</span></span></div>
<div class="blockquote blockquote-example">
<div class="blockquote-title-block"><i class="fa fa-magnifying-glass"></i>  <span class="blockquote-title">Result</span></div><table class="table-markdown">
<tr><th style="text-align: left;">Title1</th><th style="text-align: left;">Title2</th></tr><tr><td style="text-align: left;"> Item1  </td><td style="text-align: left;"> Item2</td></tr>
<tr><td style="text-align: left;"> Item1  </td><td style="text-align: left;"> Item2</td></tr>
</table>
</div>
<p>You can define the <b>column alignment</b> by adding <span class="code-inline">:</span> at the start and/or at the end of a separator. </p>
<div class="code-block"><span class="SCde"><span class="SCmt">// | Title1     | Title2       | Title3</span>
<span class="SCmt">// | :-----     | :----:       | -----:</span>
<span class="SCmt">// | Align left | Align center | Align right</span></span></div>
<div class="blockquote blockquote-example">
<div class="blockquote-title-block"><i class="fa fa-magnifying-glass"></i>  <span class="blockquote-title">Result</span></div><table class="table-markdown">
<tr><th style="text-align: left;">Title1</th><th style="text-align: center;">Title2</th><th style="text-align: right;">Title3</th></tr><tr><td style="text-align: left;"> Align left </td><td style="text-align: center;"> Align center </td><td style="text-align: right;"> Align right</td></tr>
</table>
</div>
<h4 id="_230_documentation_md_Format_of_comments_Code">Code </h4>
<p>You can create a simple <b>code paragraph</b> with three backticks before and after the code. </p>
<div class="code-block"><span class="SCde"><span class="SCmt">// ```</span>
<span class="SCmt">// if a == true</span>
<span class="SCmt">//   @print("true")</span>
<span class="SCmt">// ```</span></span></div>
<div class="blockquote blockquote-example">
<div class="blockquote-title-block"><i class="fa fa-magnifying-glass"></i>  <span class="blockquote-title">Result</span></div><div class="code-block"><span class="SCde">if a == true
   @print("true")</span></div>
</div>
<p>You can also create that kind of paragraph by simply indenting the code with four blanks or one tabulation. </p>
<div class="code-block"><span class="SCde"><span class="SCmt">//    if a == false</span>
<span class="SCmt">//        @print("false")</span></span></div>
<p>And if you want <b>syntax coloration</b>, add <span class="code-inline">swag</span> after the three backticks. Only Swag syntax is supported right now. </p>
<div class="code-block"><span class="SCde"><span class="SCmt">// ```swag</span>
<span class="SCmt">// if a == true</span>
<span class="SCmt">//   @print("true")</span>
<span class="SCmt">// ```</span></span></div>
<div class="blockquote blockquote-example">
<div class="blockquote-title-block"><i class="fa fa-magnifying-glass"></i>  <span class="blockquote-title">Result</span></div><div class="code-block"><span class="SCde"><span class="SLgc">if</span> a == <span class="SKwd">true</span>
   <span class="SItr">@print</span>(<span class="SStr">"true"</span>)</span></div>
</div>
<h4 id="_230_documentation_md_Format_of_comments_Titles">Titles </h4>
<p>You can define <b>titles</b> with <span class="code-inline">#</span>, <span class="code-inline">##</span> ... followed by a blank, and then the text. The real level of the title will depend on the context and the generated documentation kind. </p>
<div class="code-block"><span class="SCde"><span class="SCmt">// # Title 1</span>
<span class="SCmt">// ## Title 2</span>
<span class="SCmt">// ### Title 3</span>
<span class="SCmt">// #### Title 4</span>
<span class="SCmt">// ##### Title 5</span>
<span class="SCmt">// ###### Title 6</span></span></div>
<h4 id="_230_documentation_md_Format_of_comments_References">References </h4>
<p>You can create an external <b>reference</b> with <span class="code-inline">[name](link)</span>. </p>
<div class="code-block"><span class="SCmt">// This is a [reference](https://github.com/swag-lang/swag) to the Swag repository on GitHub.</span></div>
<div class="blockquote blockquote-example">
<div class="blockquote-title-block"><i class="fa fa-magnifying-glass"></i>  <span class="blockquote-title">Result</span></div><p> This is a <a href="https://github.com/swag-lang/swag">reference</a> to the Swag repository on GitHub. </p>
</div>
<h4 id="_230_documentation_md_Format_of_comments_Images">Images </h4>
<p>You can insert an external <b>image</b> with <span class="code-inline">![name](link)</span>. </p>
<div class="code-block"><span class="SCmt">// This is an image ![image](https://swag-lang/imgs/swag_icon.png).</span></div>
<h4 id="_230_documentation_md_Format_of_comments_Markdown">Markdown </h4>
<p>Some other markers are also supported inside texts. </p>
<div class="code-block"><span class="SCde"><span class="SCmt">// This is `inline code` with back ticks.\</span>
<span class="SCmt">// This is inline 'code' with normal ticks, but just for a single word (no blanks).\</span>
<span class="SCmt">// This is **bold**.\</span>
<span class="SCmt">// This is *italic*.\</span>
<span class="SCmt">// This is ***bold and italic***.\</span>
<span class="SCmt">// This is ~~strikethrough~~.\</span>
<span class="SCmt">// This character \n is escaped, and 'n' will be output as is.\</span></span></div>
<div class="blockquote blockquote-example">
<div class="blockquote-title-block"><i class="fa fa-magnifying-glass"></i>  <span class="blockquote-title">Result</span></div><p> This is <span class="code-inline">inline code</span> with back ticks.<br/>  This is inline <span class="code-inline">code</span> with normal ticks, but just for a single word (no blanks).<br/>  This is <b>bold</b>.<br/>  This is <i>italic</i>.<br/>  This is <b><i>bold and italic</i></b>.<br/>  This is <span class="strikethrough-text">strikethrough</span>.<br/>  This character n is escaped, and <span class="code-inline">n</span> will be output as is.<br/> </p>
</div>

<h3 id="_230_documentation_md__231_001_Api_swg">Api</h3><p>In <span class="code-inline">Swag.DocKind.Api</span> mode, swag will collect all <b>public definitions</b> to generate the documentation. <a href="std.core.php">Std.Core</a> is an example of documentation generated in that mode. </p>
<p>The main module documentation should be placed at the top of the corresponding <span class="code-inline">module.swg</span> file. </p>
<div class="code-block"><span class="SCde"><span class="SCmt">// This is the main module documentation.</span>
<span class="SFct">#dependencies</span>
{
}</span></div>
<p>Other comments need to be placed just before a function, struct or enum. </p>
<p>The first paragraph is considered to be the <b>short description</b> which can appear on specific parts of the documentation. So make it... short. </p>
<p>If the first line ends with a dot <span class="code-inline">.</span>, then this marks the end of the paragraph, i.e. the end of the short description. </p>
<div class="code-block"><span class="SCde"><span class="SCmt">// This first paragraph is the short description of function 'test1'</span>
<span class="SCmt">//</span>
<span class="SCmt">// This second paragraph should be the long description.</span>
<span class="SKwd">func</span> <span class="SFct">test1</span>() {}

<span class="SCmt">// This is the short description of 'test'.</span>
<span class="SCmt">// As the previous first line ends with '.', this is another paragraph, so this should be</span>
<span class="SCmt">// the long description. No need for an empty line before.</span>
<span class="SKwd">func</span> <span class="SFct">test</span>() {}</span></div>
<p>For constants or enum values, the document comment is the one declared at the end of the line. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">const</span> <span class="SCst">A</span> = <span class="SNum">0</span> <span class="SCmt">// This is the documentation comment of constant 'A'</span>

<span class="SKwd">enum</span> <span class="SCst">Color</span>
{
    <span class="SCst">Red</span>      <span class="SCmt">// This is the documentation comment of enum value 'Red'</span>
    <span class="SCst">Blue</span>     <span class="SCmt">// This is the documentation comment of enum value 'Blue'</span>
}</span></div>
<h4 id="_230_documentation_md__231_001_Api_swg_References">References </h4>
<p>You can create a <b>reference</b> to something in the current module with <span class="code-inline">[[name]]</span> or <span class="code-inline">[[name1.name2 etc.]]</span> </p>
<div class="code-block"><span class="SCde"><span class="SCmt">// This is a function with a 'value' parameter.</span>
<span class="SKwd">func</span> <span class="SFct">one</span>(value: <span class="STpe">s32</span>) {}

<span class="SCmt">// This is a reference to [[one]]</span>
<span class="SKwd">func</span> <span class="SFct">two</span>() {}</span></div>
<h4 id="_230_documentation_md__231_001_Api_swg_NoDoc">NoDoc </h4>
<p>You can use the <span class="code-inline">#[Swag.NoDoc]</span> attribute to prevent a certain element from showing up in the documentation. </p>
<div class="code-block"><span class="SCde"><span class="SCmt">// The function 'three' will be ignored when generating the documentation.</span>
<span class="SAtr">#[Swag.NoDoc]</span>
<span class="SKwd">func</span> <span class="SFct">three</span>() {}</span></div>

<h3 id="_230_documentation_md__231_002_Examples_md">Examples</h3><p>In <span class="code-inline">Swag.DocKind.Examples</span> mode, swag will generate a documentation like this one. Each file will be a chapter or a sub chapter. </p>
<p>File names must start with a number of 3 digits, and can be followed by another number for a sub part. </p>
<div class="code-block"><span class="SCde">100_my_title.swg            =&gt; will generate a '&lt;h1&gt;My title&lt;&lt;h1&gt;' heading
101_001_my_sub_title.swg    =&gt; will generate a '&lt;h2&gt;My sub title&lt;&lt;h2&gt;' heading
102_002_my_sub_title.swg    =&gt; will generate a '&lt;h2&gt;My sub title&lt;&lt;h2&gt;' heading
110_my_other_title.swg      =&gt; will generate a '&lt;h1&gt;My other title&lt;&lt;h1&gt;' heading
111_my_other_title.md       =&gt; you can mix with '.md' files</span></div>
<p>In that mode, the comments in the code you want to be interpreted as documentation (and not swag comments) must start with <span class="code-inline">/**</span>. </p>
<div class="code-block"><span class="SCde">/**
This is a valid documentation comment.
The comment must start with /** and end with */, which should be alone on their respective line.
*/</span></div>
<p>Note that the documentation you are reading right now has been generated in that mode, from the <a href="https://github.com/swag-lang/swag/tree/master/bin/reference/tests/language">std/reference/language</a> module. </p>

<h3 id="_230_documentation_md__231_003_Pages_md">Pages</h3><p>In <span class="code-inline">Swag.DocKind.Pages</span> mode, each file will generate its own page, with the same name. Other than that, it's the same behavior as the <span class="code-inline">Swag.DocKind.Examples</span> mode. </p>
<p>Can be usefull to generate web pages for <a href="https://github.com/swag-lang/swag/tree/master/bin/reference/tests/web">example</a>. </p>
<div class="swag-watermark">
Generated on 22-08-2024 with <a href="https://swag-lang.org/index.php">swag</a> 0.38.0</div>
</div>
</div>
</div>

    <script> 
		function getOffsetTop(element, parent) {
			let offsetTop = 0;
			while (element && element != parent) {
				offsetTop += element.offsetTop;
				element = element.offsetParent;
			}
			return offsetTop;
		}	
		document.addEventListener("DOMContentLoaded", function() {
			let hash = window.location.hash;
			if (hash)
			{
				let parentScrollable = document.querySelector('.right');
				if (parentScrollable)
				{
					let targetElement = parentScrollable.querySelector(hash);
					if (targetElement)
					{
						parentScrollable.scrollTop = getOffsetTop(targetElement, parentScrollable);
					}
				}
			}
        });
    </script>
</body>
</html>
