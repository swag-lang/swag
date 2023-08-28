<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta http-equiv="X-UA-Compatible" content="IE=edge">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<?php include('common/start-head.php'); ?><title>Module libc</title>
<link rel="stylesheet" type="text/css" href="css/style.css">
<style>

        .container {
            display:        flex;
            flex-wrap:      nowrap;
            flex-direction: row;
            margin:         0px auto;
            padding:        0px;
        }
        .left {
            display:    block;
            overflow-y: scroll;
            width:      500px;
        }
        .leftpage {
            margin:     10px;
        }
        .right {
            display:    block;
            width:      100%;
        }
        .rightpage {
            max-width:  1024px;
            margin:     10px auto;
        }
        @media(min-width: 640px) {
            .container {
                max-width: 640px;
            }
        }
        @media(min-width: 768px) {
            .container {
                max-width: 768px;
            }
        }
        @media(min-width: 1024px) {
            .container {
                max-width: 1024px;
            }
        }
        @media(min-width: 1280px) {
            .container {
                max-width: 1280px;
            }
        }
        @media(min-width: 1536px) {
            .container {
                max-width: 1536px;
            }
        }
        @media screen and (max-width: 600px) {
            .left {
                display: none;
            }
            .rightpage {
                margin:  10px;
            }
        }

            .container {
                height:     100vh;
            }
            .right {
                overflow-y: scroll;
            }

        body {
            margin:         0px;
            line-height:    1.3em;
            font-family:    Segoe UI;
        }
        .container blockquote {
            background-color:   LightYellow;
            border-left:        6px solid Orange;
            margin:             20px;
            margin-left:        50px;
            margin-right:       50px;
            padding:            10px;
        }
        .container a {
            color:              DoggerBlue;
        }
        .precode a {
            color:              inherit;
        }
        .codetype a {
            color:              inherit;
        }
        .left a {
            text-decoration:    none;
        }
        .container a:hover {
            text-decoration:    underline;
        }
        table.item {
            border-collapse:    separate;
            background-color:   Black;
            color:              White;
            width:              100%;
            margin-top:         70px;
            margin-right:       0px;
            font-size:          110%;
        }
        .item td:first-child {
            width:              33%;
            white-space:        nowrap;
        }
        table.enumeration {
            border:             1px solid LightGrey;
            border-collapse:    collapse;
            width:              100%;
            font-size:          90%;
        }
        .enumeration td {
            padding:            6px;
            border:             1px solid LightGrey;
            border-collapse:    collapse;
            min-width:          100px;
        }
        .enumeration td:first-child {
            background-color:   #f8f8f8;
            white-space:        nowrap;
        }
        .codetype {
            background-color:   #eeeeee;
        }
        .container td:last-child {
            width:              100%;
        }
        .left ul {
            list-style-type:    none;
            margin-left:        -20px;
        }
        .titletype {
            font-weight:        normal;
            font-size:          80%;
        }
        .titlelight {
            font-weight:        normal;
        }
        .titlestrong {
            font-weight:        bold;
            font-size:          100%;
        }
        .left h3 {
            background-color:   Black;
            color:              White;
            padding:            6px;
        }
        .right h1 {
            margin-top:         50px;
            margin-bottom:      50px;
        }
        .right h2 {
            margin-top:         35px;
        }
        .srcref {
            text-align:         right;
        }
        .incode {
            background-color:   #eeeeee;
            padding:            2px;
            border: 1px dotted  #cccccc;
        }
        .tdname .incode {
            background-color:   revert;
            padding:            2px;
            border:             revert;
        }
        .addinfos {
            font-size:          90%;
            white-space:        break-spaces;
            overflow-wrap:      break-word;
        }
        .precode {
            background-color:   #eeeeee;
            border:             1px solid LightGrey;
            padding:            10px;
            margin:             20px;
            white-space:        pre;
            overflow-x:         auto;
        }
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
    .SInv { color: #ff0000; }
</style>
<?php include('common/end-head.php'); ?></head>
<body>
<?php include('common/start-body.php'); ?><div class="container">
<div class="left">
<div class="leftpage">
<h2>Table of Contents</h2>
<h3>Structs</h3>
<h4></h4>
<ul>
<li><a href="#Libc_FILE">FILE</a></li>
<li><a href="#Libc_div_t">div_t</a></li>
<li><a href="#Libc_ldiv_t">ldiv_t</a></li>
<li><a href="#Libc_lldiv_t">lldiv_t</a></li>
<li><a href="#Libc_tm">tm</a></li>
</ul>
<h3>Constants</h3>
<h4></h4>
<ul>
<li><a href="#Libc_BUFSIZ">BUFSIZ</a></li>
<li><a href="#Libc_CLOCKS_PER_SEC">CLOCKS_PER_SEC</a></li>
<li><a href="#Libc_EOF">EOF</a></li>
<li><a href="#Libc_EXIT_FAILURE">EXIT_FAILURE</a></li>
<li><a href="#Libc_EXIT_SUCCESS">EXIT_SUCCESS</a></li>
<li><a href="#Libc_FILENAME_MAX">FILENAME_MAX</a></li>
<li><a href="#Libc_FOPEN_MAX">FOPEN_MAX</a></li>
<li><a href="#Libc_FP_ILOGB0">FP_ILOGB0</a></li>
<li><a href="#Libc_FP_ILOGBNAN">FP_ILOGBNAN</a></li>
<li><a href="#Libc_FP_INFINITE">FP_INFINITE</a></li>
<li><a href="#Libc_FP_NAN">FP_NAN</a></li>
<li><a href="#Libc_FP_NORMAL">FP_NORMAL</a></li>
<li><a href="#Libc_FP_SUBNORMAL">FP_SUBNORMAL</a></li>
<li><a href="#Libc_FP_ZERO">FP_ZERO</a></li>
<li><a href="#Libc_ForeignLib">ForeignLib</a></li>
<li><a href="#Libc_HUGE_VAL">HUGE_VAL</a></li>
<li><a href="#Libc_HUGE_VALF">HUGE_VALF</a></li>
<li><a href="#Libc_INFINITY">INFINITY</a></li>
<li><a href="#Libc_INT16_MAX">INT16_MAX</a></li>
<li><a href="#Libc_INT16_MIN">INT16_MIN</a></li>
<li><a href="#Libc_INT32_MAX">INT32_MAX</a></li>
<li><a href="#Libc_INT32_MIN">INT32_MIN</a></li>
<li><a href="#Libc_INT64_MAX">INT64_MAX</a></li>
<li><a href="#Libc_INT64_MIN">INT64_MIN</a></li>
<li><a href="#Libc_INT8_MAX">INT8_MAX</a></li>
<li><a href="#Libc_INT8_MIN">INT8_MIN</a></li>
<li><a href="#Libc_L_tmpnam">L_tmpnam</a></li>
<li><a href="#Libc_MATH_ERREXCEPT">MATH_ERREXCEPT</a></li>
<li><a href="#Libc_MATH_ERRNO">MATH_ERRNO</a></li>
<li><a href="#Libc_NAN">NAN</a></li>
<li><a href="#Libc_PTRDIFF_MAX">PTRDIFF_MAX</a></li>
<li><a href="#Libc_PTRDIFF_MIN">PTRDIFF_MIN</a></li>
<li><a href="#Libc_SEEK_CUR">SEEK_CUR</a></li>
<li><a href="#Libc_SEEK_END">SEEK_END</a></li>
<li><a href="#Libc_SEEK_SET">SEEK_SET</a></li>
<li><a href="#Libc_SIZE_MAX">SIZE_MAX</a></li>
<li><a href="#Libc_TIME_UTC">TIME_UTC</a></li>
<li><a href="#Libc_TMP_MAX">TMP_MAX</a></li>
<li><a href="#Libc_UINT16_MAX">UINT16_MAX</a></li>
<li><a href="#Libc_UINT32_MAX">UINT32_MAX</a></li>
<li><a href="#Libc_UINT64_MAX">UINT64_MAX</a></li>
<li><a href="#Libc_UINT8_MAX">UINT8_MAX</a></li>
<li><a href="#Libc__IOFBF">_IOFBF</a></li>
<li><a href="#Libc__IOLBF">_IOLBF</a></li>
<li><a href="#Libc__IONBF">_IONBF</a></li>
</ul>
<h3>Type Aliases</h3>
<h4></h4>
<ul>
<li><a href="#Libc_clock_t">clock_t</a></li>
<li><a href="#Libc_double_t">double_t</a></li>
<li><a href="#Libc_float_t">float_t</a></li>
<li><a href="#Libc_fpos_t">fpos_t</a></li>
<li><a href="#Libc_int16_t">int16_t</a></li>
<li><a href="#Libc_int32_t">int32_t</a></li>
<li><a href="#Libc_int64_t">int64_t</a></li>
<li><a href="#Libc_int8_t">int8_t</a></li>
<li><a href="#Libc_int_fast16_t">int_fast16_t</a></li>
<li><a href="#Libc_int_fast32_t">int_fast32_t</a></li>
<li><a href="#Libc_int_fast64_t">int_fast64_t</a></li>
<li><a href="#Libc_int_fast8_t">int_fast8_t</a></li>
<li><a href="#Libc_int_least16_t">int_least16_t</a></li>
<li><a href="#Libc_int_least32_t">int_least32_t</a></li>
<li><a href="#Libc_int_least64_t">int_least64_t</a></li>
<li><a href="#Libc_int_least8_t">int_least8_t</a></li>
<li><a href="#Libc_intmax_t">intmax_t</a></li>
<li><a href="#Libc_intptr_t">intptr_t</a></li>
<li><a href="#Libc_ptrdiff_t">ptrdiff_t</a></li>
<li><a href="#Libc_size_t">size_t</a></li>
<li><a href="#Libc_time_t">time_t</a></li>
<li><a href="#Libc_uint16_t">uint16_t</a></li>
<li><a href="#Libc_uint32_t">uint32_t</a></li>
<li><a href="#Libc_uint64_t">uint64_t</a></li>
<li><a href="#Libc_uint8_t">uint8_t</a></li>
<li><a href="#Libc_uint_fast16_t">uint_fast16_t</a></li>
<li><a href="#Libc_uint_fast32_t">uint_fast32_t</a></li>
<li><a href="#Libc_uint_fast64_t">uint_fast64_t</a></li>
<li><a href="#Libc_uint_fast8_t">uint_fast8_t</a></li>
<li><a href="#Libc_uint_least16_t">uint_least16_t</a></li>
<li><a href="#Libc_uint_least32_t">uint_least32_t</a></li>
<li><a href="#Libc_uint_least64_t">uint_least64_t</a></li>
<li><a href="#Libc_uint_least8_t">uint_least8_t</a></li>
<li><a href="#Libc_uintmax_t">uintmax_t</a></li>
<li><a href="#Libc_uintptr_t">uintptr_t</a></li>
<li><a href="#Libc_va_list">va_list</a></li>
<li><a href="#Libc_wchar_t">wchar_t</a></li>
</ul>
<h3>Functions</h3>
<h4></h4>
<ul>
<li><a href="#Libc__Exit">Libc._Exit</a></li>
<li><a href="#Libc_abort">Libc.abort</a></li>
<li><a href="#Libc_abs">Libc.abs</a></li>
<li><a href="#Libc_acos">Libc.acos</a></li>
<li><a href="#Libc_acosf">Libc.acosf</a></li>
<li><a href="#Libc_acosh">Libc.acosh</a></li>
<li><a href="#Libc_acoshf">Libc.acoshf</a></li>
<li><a href="#Libc_acrt_iob_func">Libc.acrt_iob_func</a></li>
<li><a href="#Libc_aligned_alloc">Libc.aligned_alloc</a></li>
<li><a href="#Libc_asctime">Libc.asctime</a></li>
<li><a href="#Libc_asin">Libc.asin</a></li>
<li><a href="#Libc_asinf">Libc.asinf</a></li>
<li><a href="#Libc_asinh">Libc.asinh</a></li>
<li><a href="#Libc_asinhf">Libc.asinhf</a></li>
<li><a href="#Libc_atan">Libc.atan</a></li>
<li><a href="#Libc_atan2">Libc.atan2</a></li>
<li><a href="#Libc_atan2f">Libc.atan2f</a></li>
<li><a href="#Libc_atanf">Libc.atanf</a></li>
<li><a href="#Libc_atanh">Libc.atanh</a></li>
<li><a href="#Libc_atanhf">Libc.atanhf</a></li>
<li><a href="#Libc_atof">Libc.atof</a></li>
<li><a href="#Libc_atoi">Libc.atoi</a></li>
<li><a href="#Libc_atol">Libc.atol</a></li>
<li><a href="#Libc_atoll">Libc.atoll</a></li>
<li><a href="#Libc_bsearch">Libc.bsearch</a></li>
<li><a href="#Libc_calloc">Libc.calloc</a></li>
<li><a href="#Libc_cbrt">Libc.cbrt</a></li>
<li><a href="#Libc_cbrtf">Libc.cbrtf</a></li>
<li><a href="#Libc_ceil">Libc.ceil</a></li>
<li><a href="#Libc_ceilf">Libc.ceilf</a></li>
<li><a href="#Libc_clearerr">Libc.clearerr</a></li>
<li><a href="#Libc_clock">Libc.clock</a></li>
<li><a href="#Libc_copysign">Libc.copysign</a></li>
<li><a href="#Libc_copysignf">Libc.copysignf</a></li>
<li><a href="#Libc_cos">Libc.cos</a></li>
<li><a href="#Libc_cosf">Libc.cosf</a></li>
<li><a href="#Libc_cosh">Libc.cosh</a></li>
<li><a href="#Libc_coshf">Libc.coshf</a></li>
<li><a href="#Libc_ctime">Libc.ctime</a></li>
<li><a href="#Libc_difftime">Libc.difftime</a></li>
<li><a href="#Libc_div">Libc.div</a></li>
<li><a href="#Libc_erf">Libc.erf</a></li>
<li><a href="#Libc_erfc">Libc.erfc</a></li>
<li><a href="#Libc_erfcf">Libc.erfcf</a></li>
<li><a href="#Libc_erff">Libc.erff</a></li>
<li><a href="#Libc_exit">Libc.exit</a></li>
<li><a href="#Libc_exp">Libc.exp</a></li>
<li><a href="#Libc_exp2">Libc.exp2</a></li>
<li><a href="#Libc_exp2f">Libc.exp2f</a></li>
<li><a href="#Libc_expf">Libc.expf</a></li>
<li><a href="#Libc_expm1">Libc.expm1</a></li>
<li><a href="#Libc_expm1f">Libc.expm1f</a></li>
<li><a href="#Libc_fabs">Libc.fabs</a></li>
<li><a href="#Libc_fabsf">Libc.fabsf</a></li>
<li><a href="#Libc_fclose">Libc.fclose</a></li>
<li><a href="#Libc_fdim">Libc.fdim</a></li>
<li><a href="#Libc_fdimf">Libc.fdimf</a></li>
<li><a href="#Libc_feof">Libc.feof</a></li>
<li><a href="#Libc_ferror">Libc.ferror</a></li>
<li><a href="#Libc_fflush">Libc.fflush</a></li>
<li><a href="#Libc_fgetc">Libc.fgetc</a></li>
<li><a href="#Libc_fgetpos">Libc.fgetpos</a></li>
<li><a href="#Libc_fgets">Libc.fgets</a></li>
<li><a href="#Libc_floor">Libc.floor</a></li>
<li><a href="#Libc_floorf">Libc.floorf</a></li>
<li><a href="#Libc_fma">Libc.fma</a></li>
<li><a href="#Libc_fmaf">Libc.fmaf</a></li>
<li><a href="#Libc_fmax">Libc.fmax</a></li>
<li><a href="#Libc_fmaxf">Libc.fmaxf</a></li>
<li><a href="#Libc_fmin">Libc.fmin</a></li>
<li><a href="#Libc_fminf">Libc.fminf</a></li>
<li><a href="#Libc_fmod">Libc.fmod</a></li>
<li><a href="#Libc_fmodf">Libc.fmodf</a></li>
<li><a href="#Libc_fopen">Libc.fopen</a></li>
<li><a href="#Libc_fpclassify">Libc.fpclassify</a></li>
<li><a href="#Libc_fpclassifyf">Libc.fpclassifyf</a></li>
<li><a href="#Libc_fprintf">Libc.fprintf</a></li>
<li><a href="#Libc_fputc">Libc.fputc</a></li>
<li><a href="#Libc_fread">Libc.fread</a></li>
<li><a href="#Libc_free">Libc.free</a></li>
<li><a href="#Libc_freopen">Libc.freopen</a></li>
<li><a href="#Libc_frexp">Libc.frexp</a></li>
<li><a href="#Libc_frexpf">Libc.frexpf</a></li>
<li><a href="#Libc_fscanf">Libc.fscanf</a></li>
<li><a href="#Libc_fseek">Libc.fseek</a></li>
<li><a href="#Libc_fsetpos">Libc.fsetpos</a></li>
<li><a href="#Libc_ftell">Libc.ftell</a></li>
<li><a href="#Libc_fwrite">Libc.fwrite</a></li>
<li><a href="#Libc_getc">Libc.getc</a></li>
<li><a href="#Libc_getchar">Libc.getchar</a></li>
<li><a href="#Libc_getenv">Libc.getenv</a></li>
<li><a href="#Libc_gmtime">Libc.gmtime</a></li>
<li><a href="#Libc_hypot">Libc.hypot</a></li>
<li><a href="#Libc_hypotf">Libc.hypotf</a></li>
<li><a href="#Libc_ilogb">Libc.ilogb</a></li>
<li><a href="#Libc_ilogbf">Libc.ilogbf</a></li>
<li><a href="#Libc_isalnum">Libc.isalnum</a></li>
<li><a href="#Libc_isalpha">Libc.isalpha</a></li>
<li><a href="#Libc_isblank">Libc.isblank</a></li>
<li><a href="#Libc_iscntrl">Libc.iscntrl</a></li>
<li><a href="#Libc_isdigit">Libc.isdigit</a></li>
<li><a href="#Libc_isfinite">Libc.isfinite</a></li>
<li><a href="#Libc_isgraph">Libc.isgraph</a></li>
<li><a href="#Libc_isgreater">Libc.isgreater</a></li>
<li><a href="#Libc_isgreaterequal">Libc.isgreaterequal</a></li>
<li><a href="#Libc_isinf">Libc.isinf</a></li>
<li><a href="#Libc_isless">Libc.isless</a></li>
<li><a href="#Libc_islessequal">Libc.islessequal</a></li>
<li><a href="#Libc_islessgreater">Libc.islessgreater</a></li>
<li><a href="#Libc_islower">Libc.islower</a></li>
<li><a href="#Libc_isnan">Libc.isnan</a></li>
<li><a href="#Libc_isnormal">Libc.isnormal</a></li>
<li><a href="#Libc_isprint">Libc.isprint</a></li>
<li><a href="#Libc_ispunct">Libc.ispunct</a></li>
<li><a href="#Libc_isspace">Libc.isspace</a></li>
<li><a href="#Libc_isunordered">Libc.isunordered</a></li>
<li><a href="#Libc_isupper">Libc.isupper</a></li>
<li><a href="#Libc_isxdigi">Libc.isxdigi</a></li>
<li><a href="#Libc_labs">Libc.labs</a></li>
<li><a href="#Libc_ldexp">Libc.ldexp</a></li>
<li><a href="#Libc_ldexpf">Libc.ldexpf</a></li>
<li><a href="#Libc_ldiv">Libc.ldiv</a></li>
<li><a href="#Libc_lgamma">Libc.lgamma</a></li>
<li><a href="#Libc_lgammaf">Libc.lgammaf</a></li>
<li><a href="#Libc_llabs">Libc.llabs</a></li>
<li><a href="#Libc_lldiv">Libc.lldiv</a></li>
<li><a href="#Libc_llrint">Libc.llrint</a></li>
<li><a href="#Libc_llrintf">Libc.llrintf</a></li>
<li><a href="#Libc_llround">Libc.llround</a></li>
<li><a href="#Libc_llroundf">Libc.llroundf</a></li>
<li><a href="#Libc_localtime">Libc.localtime</a></li>
<li><a href="#Libc_log">Libc.log</a></li>
<li><a href="#Libc_log10">Libc.log10</a></li>
<li><a href="#Libc_log10f">Libc.log10f</a></li>
<li><a href="#Libc_log1p">Libc.log1p</a></li>
<li><a href="#Libc_log1pf">Libc.log1pf</a></li>
<li><a href="#Libc_log2">Libc.log2</a></li>
<li><a href="#Libc_log2f">Libc.log2f</a></li>
<li><a href="#Libc_logb">Libc.logb</a></li>
<li><a href="#Libc_logbf">Libc.logbf</a></li>
<li><a href="#Libc_logf">Libc.logf</a></li>
<li><a href="#Libc_lrint">Libc.lrint</a></li>
<li><a href="#Libc_lrintf">Libc.lrintf</a></li>
<li><a href="#Libc_lround">Libc.lround</a></li>
<li><a href="#Libc_lroundf">Libc.lroundf</a></li>
<li><a href="#Libc_malloc">Libc.malloc</a></li>
<li><a href="#Libc_mblen">Libc.mblen</a></li>
<li><a href="#Libc_mbstowcs">Libc.mbstowcs</a></li>
<li><a href="#Libc_mbtowc">Libc.mbtowc</a></li>
<li><a href="#Libc_memchr">Libc.memchr</a></li>
<li><a href="#Libc_memcmp">Libc.memcmp</a></li>
<li><a href="#Libc_memcpy">Libc.memcpy</a></li>
<li><a href="#Libc_memmove">Libc.memmove</a></li>
<li><a href="#Libc_memset">Libc.memset</a></li>
<li><a href="#Libc_mktime">Libc.mktime</a></li>
<li><a href="#Libc_modf">Libc.modf</a></li>
<li><a href="#Libc_modff">Libc.modff</a></li>
<li><a href="#Libc_nan">Libc.nan</a></li>
<li><a href="#Libc_nanf">Libc.nanf</a></li>
<li><a href="#Libc_nearbyint">Libc.nearbyint</a></li>
<li><a href="#Libc_nearbyintf">Libc.nearbyintf</a></li>
<li><a href="#Libc_nextafter">Libc.nextafter</a></li>
<li><a href="#Libc_nextafterf">Libc.nextafterf</a></li>
<li><a href="#Libc_perror">Libc.perror</a></li>
<li><a href="#Libc_pow">Libc.pow</a></li>
<li><a href="#Libc_powf">Libc.powf</a></li>
<li><a href="#Libc_printf">Libc.printf</a></li>
<li><a href="#Libc_putc">Libc.putc</a></li>
<li><a href="#Libc_putchar">Libc.putchar</a></li>
<li><a href="#Libc_puts">Libc.puts</a></li>
<li><a href="#Libc_qsort">Libc.qsort</a></li>
<li><a href="#Libc_quick_exit">Libc.quick_exit</a></li>
<li><a href="#Libc_rand">Libc.rand</a></li>
<li><a href="#Libc_realloc">Libc.realloc</a></li>
<li><a href="#Libc_remainder">Libc.remainder</a></li>
<li><a href="#Libc_remainderf">Libc.remainderf</a></li>
<li><a href="#Libc_remove">Libc.remove</a></li>
<li><a href="#Libc_remquo">Libc.remquo</a></li>
<li><a href="#Libc_remquof">Libc.remquof</a></li>
<li><a href="#Libc_rename">Libc.rename</a></li>
<li><a href="#Libc_rewind">Libc.rewind</a></li>
<li><a href="#Libc_rint">Libc.rint</a></li>
<li><a href="#Libc_rintf">Libc.rintf</a></li>
<li><a href="#Libc_round">Libc.round</a></li>
<li><a href="#Libc_roundf">Libc.roundf</a></li>
<li><a href="#Libc_scalbln">Libc.scalbln</a></li>
<li><a href="#Libc_scalblnf">Libc.scalblnf</a></li>
<li><a href="#Libc_scalbn">Libc.scalbn</a></li>
<li><a href="#Libc_scalbnf">Libc.scalbnf</a></li>
<li><a href="#Libc_scanf">Libc.scanf</a></li>
<li><a href="#Libc_setbuf">Libc.setbuf</a></li>
<li><a href="#Libc_setvbuf">Libc.setvbuf</a></li>
<li><a href="#Libc_signbit">Libc.signbit</a></li>
<li><a href="#Libc_sin">Libc.sin</a></li>
<li><a href="#Libc_sinf">Libc.sinf</a></li>
<li><a href="#Libc_sinh">Libc.sinh</a></li>
<li><a href="#Libc_sinhf">Libc.sinhf</a></li>
<li><a href="#Libc_snprintf">Libc.snprintf</a></li>
<li><a href="#Libc_sprintf">Libc.sprintf</a></li>
<li><a href="#Libc_sqrt">Libc.sqrt</a></li>
<li><a href="#Libc_sqrtf">Libc.sqrtf</a></li>
<li><a href="#Libc_srand">Libc.srand</a></li>
<li><a href="#Libc_sscanf">Libc.sscanf</a></li>
<li><a href="#Libc_stdio_common_vfprintf">Libc.stdio_common_vfprintf</a></li>
<li><a href="#Libc_stdio_common_vfscanf">Libc.stdio_common_vfscanf</a></li>
<li><a href="#Libc_stdio_common_vsprintf">Libc.stdio_common_vsprintf</a></li>
<li><a href="#Libc_stdio_common_vsscanf">Libc.stdio_common_vsscanf</a></li>
<li><a href="#Libc_strcat">Libc.strcat</a></li>
<li><a href="#Libc_strchr">Libc.strchr</a></li>
<li><a href="#Libc_strcmp">Libc.strcmp</a></li>
<li><a href="#Libc_strcoll">Libc.strcoll</a></li>
<li><a href="#Libc_strcpy">Libc.strcpy</a></li>
<li><a href="#Libc_strcspn">Libc.strcspn</a></li>
<li><a href="#Libc_strerror">Libc.strerror</a></li>
<li><a href="#Libc_strftime">Libc.strftime</a></li>
<li><a href="#Libc_strlen">Libc.strlen</a></li>
<li><a href="#Libc_strncat">Libc.strncat</a></li>
<li><a href="#Libc_strncmp">Libc.strncmp</a></li>
<li><a href="#Libc_strncpy">Libc.strncpy</a></li>
<li><a href="#Libc_strpbrk">Libc.strpbrk</a></li>
<li><a href="#Libc_strrchr">Libc.strrchr</a></li>
<li><a href="#Libc_strspn">Libc.strspn</a></li>
<li><a href="#Libc_strstr">Libc.strstr</a></li>
<li><a href="#Libc_strtod">Libc.strtod</a></li>
<li><a href="#Libc_strtof">Libc.strtof</a></li>
<li><a href="#Libc_strtok">Libc.strtok</a></li>
<li><a href="#Libc_strtol">Libc.strtol</a></li>
<li><a href="#Libc_strtoll">Libc.strtoll</a></li>
<li><a href="#Libc_strtoul">Libc.strtoul</a></li>
<li><a href="#Libc_strtoull">Libc.strtoull</a></li>
<li><a href="#Libc_strxfrm">Libc.strxfrm</a></li>
<li><a href="#Libc_system">Libc.system</a></li>
<li><a href="#Libc_tan">Libc.tan</a></li>
<li><a href="#Libc_tanf">Libc.tanf</a></li>
<li><a href="#Libc_tanh">Libc.tanh</a></li>
<li><a href="#Libc_tanhf">Libc.tanhf</a></li>
<li><a href="#Libc_tgamma">Libc.tgamma</a></li>
<li><a href="#Libc_tgammaf">Libc.tgammaf</a></li>
<li><a href="#Libc_time">Libc.time</a></li>
<li><a href="#Libc_tmpfile">Libc.tmpfile</a></li>
<li><a href="#Libc_tmpnam">Libc.tmpnam</a></li>
<li><a href="#Libc_tolower">Libc.tolower</a></li>
<li><a href="#Libc_toupper">Libc.toupper</a></li>
<li><a href="#Libc_trunc">Libc.trunc</a></li>
<li><a href="#Libc_truncf">Libc.truncf</a></li>
<li><a href="#Libc_ungetc">Libc.ungetc</a></li>
<li><a href="#Libc_va_end">Libc.va_end</a></li>
<li><a href="#Libc_va_start">Libc.va_start</a></li>
<li><a href="#Libc_vfprintf">Libc.vfprintf</a></li>
<li><a href="#Libc_vfscanf">Libc.vfscanf</a></li>
<li><a href="#Libc_vprintf">Libc.vprintf</a></li>
<li><a href="#Libc_vscanf">Libc.vscanf</a></li>
<li><a href="#Libc_vsnprintf">Libc.vsnprintf</a></li>
<li><a href="#Libc_vsprintf">Libc.vsprintf</a></li>
<li><a href="#Libc_vsscanf">Libc.vsscanf</a></li>
<li><a href="#Libc_wcstombs">Libc.wcstombs</a></li>
<li><a href="#Libc_wctomb">Libc.wctomb</a></li>
</ul>
</div>
</div>
<div class="right">
<div class="rightpage">
<blockquote>
<b>Work in progress</b>. Generated documentation (swag doc 0.24.0)</blockquote>
<h1>Module libc</h1>
<p> Wrapper for LibC library. </p>
<h1>Content</h1>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_BUFSIZ"><span class="titletype">const</span> <span class="titlelight">Libc.</span><span class="titlestrong">Constants</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdio.swg#L11" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td id="Libc_BUFSIZ" class="codetype"><span class="SCst">BUFSIZ</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_CLOCKS_PER_SEC" class="codetype"><span class="SCst">CLOCKS_PER_SEC</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_EOF" class="codetype"><span class="SCst">EOF</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_EXIT_FAILURE" class="codetype"><span class="SCst">EXIT_FAILURE</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_EXIT_SUCCESS" class="codetype"><span class="SCst">EXIT_SUCCESS</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_FILENAME_MAX" class="codetype"><span class="SCst">FILENAME_MAX</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_FOPEN_MAX" class="codetype"><span class="SCst">FOPEN_MAX</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_FP_ILOGB0" class="codetype"><span class="SCst">FP_ILOGB0</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_FP_ILOGBNAN" class="codetype"><span class="SCst">FP_ILOGBNAN</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_FP_INFINITE" class="codetype"><span class="SCst">FP_INFINITE</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_FP_NAN" class="codetype"><span class="SCst">FP_NAN</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_FP_NORMAL" class="codetype"><span class="SCst">FP_NORMAL</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_FP_SUBNORMAL" class="codetype"><span class="SCst">FP_SUBNORMAL</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_FP_ZERO" class="codetype"><span class="SCst">FP_ZERO</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_ForeignLib" class="codetype"><span class="SCst">ForeignLib</span></td>
<td class="codetype"><span class="STpe">string</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_HUGE_VAL" class="codetype"><span class="SCst">HUGE_VAL</span></td>
<td class="codetype"><span class="STpe">f64</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_HUGE_VALF" class="codetype"><span class="SCst">HUGE_VALF</span></td>
<td class="codetype"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_INFINITY" class="codetype"><span class="SCst">INFINITY</span></td>
<td class="codetype"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_INT16_MAX" class="codetype"><span class="SCst">INT16_MAX</span></td>
<td class="codetype"><span class="STpe">s16</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_INT16_MIN" class="codetype"><span class="SCst">INT16_MIN</span></td>
<td class="codetype"><span class="STpe">s16</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_INT32_MAX" class="codetype"><span class="SCst">INT32_MAX</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_INT32_MIN" class="codetype"><span class="SCst">INT32_MIN</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_INT64_MAX" class="codetype"><span class="SCst">INT64_MAX</span></td>
<td class="codetype"><span class="STpe">s64</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_INT64_MIN" class="codetype"><span class="SCst">INT64_MIN</span></td>
<td class="codetype"><span class="STpe">s64</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_INT8_MAX" class="codetype"><span class="SCst">INT8_MAX</span></td>
<td class="codetype"><span class="STpe">s8</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_INT8_MIN" class="codetype"><span class="SCst">INT8_MIN</span></td>
<td class="codetype"><span class="STpe">s8</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_L_tmpnam" class="codetype"><span class="SCst">L_tmpnam</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_MATH_ERREXCEPT" class="codetype"><span class="SCst">MATH_ERREXCEPT</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_MATH_ERRNO" class="codetype"><span class="SCst">MATH_ERRNO</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_NAN" class="codetype"><span class="SCst">NAN</span></td>
<td class="codetype"><span class="STpe">f64</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_PTRDIFF_MAX" class="codetype"><span class="SCst">PTRDIFF_MAX</span></td>
<td class="codetype"><span class="STpe">s64</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_PTRDIFF_MIN" class="codetype"><span class="SCst">PTRDIFF_MIN</span></td>
<td class="codetype"><span class="STpe">s64</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_SEEK_CUR" class="codetype"><span class="SCst">SEEK_CUR</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_SEEK_END" class="codetype"><span class="SCst">SEEK_END</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_SEEK_SET" class="codetype"><span class="SCst">SEEK_SET</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_SIZE_MAX" class="codetype"><span class="SCst">SIZE_MAX</span></td>
<td class="codetype"><span class="STpe">u64</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_TIME_UTC" class="codetype"><span class="SCst">TIME_UTC</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_TMP_MAX" class="codetype"><span class="SCst">TMP_MAX</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_UINT16_MAX" class="codetype"><span class="SCst">UINT16_MAX</span></td>
<td class="codetype"><span class="STpe">u16</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_UINT32_MAX" class="codetype"><span class="SCst">UINT32_MAX</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_UINT64_MAX" class="codetype"><span class="SCst">UINT64_MAX</span></td>
<td class="codetype"><span class="STpe">u64</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_UINT8_MAX" class="codetype"><span class="SCst">UINT8_MAX</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td id="Libc__IOFBF" class="codetype"><span class="SCde">_IOFBF</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Libc__IOLBF" class="codetype"><span class="SCde">_IOLBF</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Libc__IONBF" class="codetype"><span class="SCde">_IONBF</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_clock_t"><span class="titletype">type alias</span> <span class="titlelight">Libc.</span><span class="titlestrong">Type Aliases</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\time.swg#L24" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td id="Libc_clock_t" class="codetype"><span class="SCde">clock_t</span></td>
<td class="codetype"><span class="STpe">s64</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_double_t" class="codetype"><span class="SCde">double_t</span></td>
<td class="codetype"><span class="STpe">f64</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_float_t" class="codetype"><span class="SCde">float_t</span></td>
<td class="codetype"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_fpos_t" class="codetype"><span class="SCde">fpos_t</span></td>
<td class="codetype"><span class="STpe">s64</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_int16_t" class="codetype"><span class="SCde">int16_t</span></td>
<td class="codetype"><span class="STpe">s16</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_int32_t" class="codetype"><span class="SCde">int32_t</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_int64_t" class="codetype"><span class="SCde">int64_t</span></td>
<td class="codetype"><span class="STpe">s64</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_int8_t" class="codetype"><span class="SCde">int8_t</span></td>
<td class="codetype"><span class="STpe">s8</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_int_fast16_t" class="codetype"><span class="SCde">int_fast16_t</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_int_fast32_t" class="codetype"><span class="SCde">int_fast32_t</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_int_fast64_t" class="codetype"><span class="SCde">int_fast64_t</span></td>
<td class="codetype"><span class="STpe">s64</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_int_fast8_t" class="codetype"><span class="SCde">int_fast8_t</span></td>
<td class="codetype"><span class="STpe">s8</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_int_least16_t" class="codetype"><span class="SCde">int_least16_t</span></td>
<td class="codetype"><span class="STpe">s16</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_int_least32_t" class="codetype"><span class="SCde">int_least32_t</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_int_least64_t" class="codetype"><span class="SCde">int_least64_t</span></td>
<td class="codetype"><span class="STpe">s64</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_int_least8_t" class="codetype"><span class="SCde">int_least8_t</span></td>
<td class="codetype"><span class="STpe">s8</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_intmax_t" class="codetype"><span class="SCde">intmax_t</span></td>
<td class="codetype"><span class="STpe">s64</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_intptr_t" class="codetype"><span class="SCde">intptr_t</span></td>
<td class="codetype"><span class="STpe">s64</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_ptrdiff_t" class="codetype"><span class="SCde">ptrdiff_t</span></td>
<td class="codetype"><span class="STpe">s64</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_size_t" class="codetype"><span class="SCde">size_t</span></td>
<td class="codetype"><span class="STpe">u64</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_time_t" class="codetype"><span class="SCde">time_t</span></td>
<td class="codetype"><span class="STpe">s64</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_uint16_t" class="codetype"><span class="SCde">uint16_t</span></td>
<td class="codetype"><span class="STpe">u16</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_uint32_t" class="codetype"><span class="SCde">uint32_t</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_uint64_t" class="codetype"><span class="SCde">uint64_t</span></td>
<td class="codetype"><span class="STpe">u64</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_uint8_t" class="codetype"><span class="SCde">uint8_t</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_uint_fast16_t" class="codetype"><span class="SCde">uint_fast16_t</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_uint_fast32_t" class="codetype"><span class="SCde">uint_fast32_t</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_uint_fast64_t" class="codetype"><span class="SCde">uint_fast64_t</span></td>
<td class="codetype"><span class="STpe">u64</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_uint_fast8_t" class="codetype"><span class="SCde">uint_fast8_t</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_uint_least16_t" class="codetype"><span class="SCde">uint_least16_t</span></td>
<td class="codetype"><span class="STpe">u16</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_uint_least32_t" class="codetype"><span class="SCde">uint_least32_t</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_uint_least64_t" class="codetype"><span class="SCde">uint_least64_t</span></td>
<td class="codetype"><span class="STpe">u64</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_uint_least8_t" class="codetype"><span class="SCde">uint_least8_t</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_uintmax_t" class="codetype"><span class="SCde">uintmax_t</span></td>
<td class="codetype"><span class="STpe">u64</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_uintptr_t" class="codetype"><span class="SCde">uintptr_t</span></td>
<td class="codetype"><span class="STpe">u64</span></td>
<td></td>
</tr>
<tr>
<td id="Libc_va_list" class="codetype"><span class="SCde">va_list</span></td>
<td class="codetype"><span class="SCde"><span class="SCst">Swag</span>.<span class="SCst">CVaList</span></span></td>
<td></td>
</tr>
<tr>
<td id="Libc_wchar_t" class="codetype"><span class="SCde">wchar_t</span></td>
<td class="codetype"><span class="STpe">u16</span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_FILE"><span class="titletype">struct</span> <span class="titlelight">Libc.</span><span class="titlestrong">FILE</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdio.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc__Exit"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">_Exit</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdlib.swg#L37" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> _Exit(status: <span class="STpe">s32</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_abort"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">abort</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdlib.swg#L35" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">abort</span>()</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_abs"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">abs</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdlib.swg#L45" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">abs</span>(j: <span class="STpe">s32</span>)-&gt;<span class="STpe">s32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_acos"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">acos</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L20" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">acos</span>(x: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_acosf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">acosf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L21" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">acosf</span>(x: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_acosh"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">acosh</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L35" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">acosh</span>(x: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_acoshf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">acoshf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L36" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">acoshf</span>(x: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_acrt_iob_func"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">acrt_iob_func</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdio.swg#L75" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">acrt_iob_func</span>(index: <span class="STpe">u32</span>)-&gt;*<span class="SCst">Libc</span>.<span class="SCst"><a href="#Libc_FILE">FILE</a></span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_aligned_alloc"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">aligned_alloc</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdlib.swg#L29" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">aligned_alloc</span>(aligment: <span class="SCst">Libc</span>.<a href="#Libc_size_t">size_t</a>, size: <span class="SCst">Libc</span>.<a href="#Libc_size_t">size_t</a>)-&gt;*<span class="STpe">void</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_asctime"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">asctime</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\time.swg#L8" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">asctime</span>(timeptr: *<span class="SCst">Libc</span>.<a href="#Libc_tm">tm</a>)-&gt;*<span class="STpe">u8</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_asin"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">asin</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L22" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">asin</span>(x: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_asinf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">asinf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L23" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">asinf</span>(x: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_asinh"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">asinh</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L37" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">asinh</span>(x: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_asinhf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">asinhf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L38" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">asinhf</span>(x: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_atan"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">atan</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L24" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">atan</span>(x: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_atan2"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">atan2</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L26" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">atan2</span>(y: <span class="STpe">f64</span>, x: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_atan2f"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">atan2f</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L27" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">atan2f</span>(y: <span class="STpe">f32</span>, x: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_atanf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">atanf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L25" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">atanf</span>(x: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_atanh"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">atanh</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L39" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">atanh</span>(x: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_atanhf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">atanhf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L40" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">atanhf</span>(x: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_atof"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">atof</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdlib.swg#L15" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">atof</span>(nptr: <span class="STpe">cstring</span>)-&gt;<span class="STpe">f64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_atoi"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">atoi</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdlib.swg#L16" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">atoi</span>(nptr: <span class="STpe">cstring</span>)-&gt;<span class="STpe">s32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_atol"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">atol</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdlib.swg#L17" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">atol</span>(nptr: <span class="STpe">cstring</span>)-&gt;<span class="STpe">s32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_atoll"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">atoll</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdlib.swg#L18" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">atoll</span>(nptr: <span class="STpe">cstring</span>)-&gt;<span class="STpe">s64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_bsearch"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">bsearch</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdlib.swg#L42" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">bsearch</span>(key: *<span class="STpe">void</span>, base: *<span class="STpe">void</span>, nmemb: <span class="SCst">Libc</span>.<a href="#Libc_size_t">size_t</a>, size: <span class="SCst">Libc</span>.<a href="#Libc_size_t">size_t</a>, compar: *<span class="STpe">void</span>)-&gt;*<span class="STpe">void</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_calloc"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">calloc</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdlib.swg#L30" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">calloc</span>(nmemb: <span class="SCst">Libc</span>.<a href="#Libc_size_t">size_t</a>, size: <span class="SCst">Libc</span>.<a href="#Libc_size_t">size_t</a>)-&gt;*<span class="STpe">void</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_cbrt"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">cbrt</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L77" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">cbrt</span>(x: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_cbrtf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">cbrtf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L78" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">cbrtf</span>(x: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_ceil"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">ceil</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L97" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">ceil</span>(x: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_ceilf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">ceilf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L98" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">ceilf</span>(x: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_clearerr"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">clearerr</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdio.swg#L53" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">clearerr</span>(stream: *<span class="SCst">Libc</span>.<span class="SCst"><a href="#Libc_FILE">FILE</a></span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_clock"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">clock</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\time.swg#L6" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">clock</span>()-&gt;<span class="SCst">Libc</span>.<a href="#Libc_clock_t">clock_t</a></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_copysign"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">copysign</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L125" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">copysign</span>(x: <span class="STpe">f64</span>, y: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_copysignf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">copysignf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L126" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">copysignf</span>(x: <span class="STpe">f32</span>, y: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_cos"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">cos</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L28" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">cos</span>(x: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_cosf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">cosf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L29" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">cosf</span>(x: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_cosh"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">cosh</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L41" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">cosh</span>(x: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_coshf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">coshf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L42" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">coshf</span>(x: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_ctime"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">ctime</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\time.swg#L18" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">ctime</span>(timer: <span class="SKwd">const</span> *<span class="SCst">Libc</span>.<a href="#Libc_time_t">time_t</a>)-&gt;*<span class="STpe">u8</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_difftime"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">difftime</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\time.swg#L14" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">difftime</span>(time1: <span class="SCst">Libc</span>.<a href="#Libc_time_t">time_t</a>, time2: <span class="SCst">Libc</span>.<a href="#Libc_time_t">time_t</a>)-&gt;<span class="STpe">f64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_div"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">div</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdlib.swg#L48" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">div</span>(numer: <span class="STpe">s32</span>, denom: <span class="STpe">s32</span>)-&gt;<span class="SCst">Libc</span>.<a href="#Libc_div_t">div_t</a></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_div_t"><span class="titletype">struct</span> <span class="titlelight">Libc.</span><span class="titlestrong">div_t</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdlib.swg#L9" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SCde">quot</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">rem</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_erf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">erf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L88" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">erf</span>(x: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_erfc"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">erfc</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L90" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">erfc</span>(x: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_erfcf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">erfcf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L91" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">erfcf</span>(x: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_erff"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">erff</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L89" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">erff</span>(x: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_exit"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">exit</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdlib.swg#L36" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">exit</span>(status: <span class="STpe">s32</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_exp"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">exp</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L48" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">exp</span>(x: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_exp2"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">exp2</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L50" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">exp2</span>(x: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_exp2f"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">exp2f</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L51" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">exp2f</span>(x: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_expf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">expf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L49" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">expf</span>(x: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_expm1"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">expm1</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L52" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">expm1</span>(x: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_expm1f"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">expm1f</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L53" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">expm1f</span>(x: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_fabs"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">fabs</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L79" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">fabs</span>(x: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_fabsf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">fabsf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L80" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">fabsf</span>(x: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_fclose"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">fclose</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdio.swg#L28" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">fclose</span>(stream: *<span class="SCst">Libc</span>.<span class="SCst"><a href="#Libc_FILE">FILE</a></span>)-&gt;<span class="STpe">s64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_fdim"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">fdim</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L132" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">fdim</span>(x: <span class="STpe">f64</span>, y: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_fdimf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">fdimf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L133" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">fdimf</span>(x: <span class="STpe">f32</span>, y: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_feof"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">feof</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdio.swg#L54" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">feof</span>(stream: *<span class="SCst">Libc</span>.<span class="SCst"><a href="#Libc_FILE">FILE</a></span>)-&gt;<span class="STpe">s64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_ferror"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">ferror</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdio.swg#L55" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">ferror</span>(stream: *<span class="SCst">Libc</span>.<span class="SCst"><a href="#Libc_FILE">FILE</a></span>)-&gt;<span class="STpe">s64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_fflush"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">fflush</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdio.swg#L29" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">fflush</span>(stream: *<span class="SCst">Libc</span>.<span class="SCst"><a href="#Libc_FILE">FILE</a></span>)-&gt;<span class="STpe">s64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_fgetc"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">fgetc</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdio.swg#L35" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">fgetc</span>(stream: *<span class="SCst">Libc</span>.<span class="SCst"><a href="#Libc_FILE">FILE</a></span>)-&gt;<span class="STpe">s64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_fgetpos"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">fgetpos</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdio.swg#L47" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">fgetpos</span>(stream: *<span class="SCst">Libc</span>.<span class="SCst"><a href="#Libc_FILE">FILE</a></span>, pos: *<span class="SCst">Libc</span>.<a href="#Libc_fpos_t">fpos_t</a>)-&gt;<span class="STpe">s64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_fgets"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">fgets</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdio.swg#L36" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">fgets</span>(s: *<span class="STpe">u8</span>, n: <span class="STpe">s64</span>, stream: *<span class="SCst">Libc</span>.<span class="SCst"><a href="#Libc_FILE">FILE</a></span>)-&gt;*<span class="STpe">u8</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_floor"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">floor</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L99" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">floor</span>(x: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_floorf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">floorf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L100" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">floorf</span>(x: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_fma"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">fma</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L138" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">fma</span>(x: <span class="STpe">f64</span>, y: <span class="STpe">f64</span>, z: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_fmaf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">fmaf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L139" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">fmaf</span>(x: <span class="STpe">f32</span>, y: <span class="STpe">f32</span>, z: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_fmax"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">fmax</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L134" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">fmax</span>(x: <span class="STpe">f64</span>, y: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_fmaxf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">fmaxf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L135" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">fmaxf</span>(x: <span class="STpe">f32</span>, y: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_fmin"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">fmin</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L136" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">fmin</span>(x: <span class="STpe">f64</span>, y: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_fminf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">fminf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L137" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">fminf</span>(x: <span class="STpe">f32</span>, y: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_fmod"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">fmod</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L118" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">fmod</span>(x: <span class="STpe">f64</span>, y: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_fmodf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">fmodf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L119" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">fmodf</span>(x: <span class="STpe">f32</span>, y: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_fopen"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">fopen</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdio.swg#L30" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">fopen</span>(filename: <span class="STpe">cstring</span>, mode: <span class="STpe">cstring</span>)-&gt;*<span class="SCst">Libc</span>.<span class="SCst"><a href="#Libc_FILE">FILE</a></span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_fpclassify"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">fpclassify</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L149" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">fpclassify</span>(x: <span class="STpe">f64</span>)-&gt;<span class="STpe">s32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_fpclassifyf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">fpclassifyf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L165" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">fpclassifyf</span>(x: <span class="STpe">f32</span>)-&gt;<span class="STpe">s32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_fprintf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">fprintf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdio.swg#L85" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">fprintf</span>(file: *<span class="SCst">Libc</span>.<span class="SCst"><a href="#Libc_FILE">FILE</a></span>, format: <span class="STpe">cstring</span>, args: <span class="STpe">cvarargs</span>)-&gt;<span class="STpe">s64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_fputc"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">fputc</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdio.swg#L37" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">fputc</span>(s: <span class="STpe">cstring</span>, stream: *<span class="SCst">Libc</span>.<span class="SCst"><a href="#Libc_FILE">FILE</a></span>)-&gt;<span class="STpe">s64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_fread"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">fread</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdio.swg#L44" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">fread</span>(ptr: *<span class="STpe">void</span>, size: <span class="SCst">Libc</span>.<a href="#Libc_size_t">size_t</a>, stream: *<span class="SCst">Libc</span>.<span class="SCst"><a href="#Libc_FILE">FILE</a></span>)-&gt;<span class="SCst">Libc</span>.<a href="#Libc_size_t">size_t</a></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_free"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">free</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdlib.swg#L31" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">free</span>(ptr: *<span class="STpe">void</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_freopen"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">freopen</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdio.swg#L31" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">freopen</span>(filename: <span class="STpe">cstring</span>, mode: <span class="STpe">cstring</span>, stream: *<span class="SCst">Libc</span>.<span class="SCst"><a href="#Libc_FILE">FILE</a></span>)-&gt;*<span class="SCst">Libc</span>.<span class="SCst"><a href="#Libc_FILE">FILE</a></span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_frexp"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">frexp</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L54" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">frexp</span>(value: <span class="STpe">f64</span>, exp: *<span class="STpe">s64</span>)-&gt;<span class="STpe">f64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_frexpf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">frexpf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L55" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">frexpf</span>(value: <span class="STpe">f32</span>, exp: *<span class="STpe">s64</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_fscanf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">fscanf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdio.swg#L159" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">fscanf</span>(file: *<span class="SCst">Libc</span>.<span class="SCst"><a href="#Libc_FILE">FILE</a></span>, format: <span class="STpe">cstring</span>, args: <span class="STpe">cvarargs</span>)-&gt;<span class="STpe">s64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_fseek"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">fseek</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdio.swg#L48" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">fseek</span>(stream: *<span class="SCst">Libc</span>.<span class="SCst"><a href="#Libc_FILE">FILE</a></span>, offset: <span class="STpe">s32</span>, whence: <span class="STpe">s64</span>)-&gt;<span class="STpe">s64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_fsetpos"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">fsetpos</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdio.swg#L49" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">fsetpos</span>(stream: *<span class="SCst">Libc</span>.<span class="SCst"><a href="#Libc_FILE">FILE</a></span>, pos: *<span class="SCst">Libc</span>.<a href="#Libc_fpos_t">fpos_t</a>)-&gt;<span class="STpe">s64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_ftell"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">ftell</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdio.swg#L50" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">ftell</span>(stream: *<span class="SCst">Libc</span>.<span class="SCst"><a href="#Libc_FILE">FILE</a></span>)-&gt;<span class="STpe">s32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_fwrite"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">fwrite</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdio.swg#L45" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">fwrite</span>(ptr: *<span class="STpe">void</span>, size: <span class="SCst">Libc</span>.<a href="#Libc_size_t">size_t</a>, nmemb: <span class="SCst">Libc</span>.<a href="#Libc_size_t">size_t</a>, stream: *<span class="SCst">Libc</span>.<span class="SCst"><a href="#Libc_FILE">FILE</a></span>)-&gt;<span class="SCst">Libc</span>.<a href="#Libc_size_t">size_t</a></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_getc"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">getc</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdio.swg#L38" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getc</span>(stream: *<span class="SCst">Libc</span>.<span class="SCst"><a href="#Libc_FILE">FILE</a></span>)-&gt;<span class="STpe">s64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_getchar"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">getchar</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdio.swg#L39" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getchar</span>()-&gt;<span class="STpe">s64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_getenv"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">getenv</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdlib.swg#L38" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getenv</span>(name: <span class="STpe">cstring</span>)-&gt;*<span class="STpe">u8</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_gmtime"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">gmtime</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\time.swg#L16" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">gmtime</span>(timer: *<span class="SCst">Libc</span>.<a href="#Libc_time_t">time_t</a>)-&gt;*<span class="SCst">Libc</span>.<a href="#Libc_tm">tm</a></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_hypot"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">hypot</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L81" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">hypot</span>(x: <span class="STpe">f64</span>, y: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_hypotf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">hypotf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L82" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">hypotf</span>(x: <span class="STpe">f32</span>, y: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_ilogb"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">ilogb</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L56" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">ilogb</span>(x: <span class="STpe">f64</span>)-&gt;<span class="STpe">s64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_ilogbf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">ilogbf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L57" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">ilogbf</span>(x: <span class="STpe">f32</span>)-&gt;<span class="STpe">s64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_isalnum"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">isalnum</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\ctype.swg#L6" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">isalnum</span>(c: <span class="STpe">s32</span>)-&gt;<span class="STpe">s32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_isalpha"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">isalpha</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\ctype.swg#L7" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">isalpha</span>(c: <span class="STpe">s32</span>)-&gt;<span class="STpe">s32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_isblank"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">isblank</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\ctype.swg#L8" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">isblank</span>(c: <span class="STpe">s32</span>)-&gt;<span class="STpe">s32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_iscntrl"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">iscntrl</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\ctype.swg#L9" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">iscntrl</span>(c: <span class="STpe">s32</span>)-&gt;<span class="STpe">s32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_isdigit"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">isdigit</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\ctype.swg#L10" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">isdigit</span>(c: <span class="STpe">s32</span>)-&gt;<span class="STpe">s32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_isfinite"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">isfinite</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L188" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">isfinite</span>(x: <span class="STpe">f32</span>)-&gt;<span class="STpe">bool</span>
<span class="SKwd">func</span> <span class="SFct">isfinite</span>(x: <span class="STpe">f64</span>)-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_isgraph"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">isgraph</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\ctype.swg#L11" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">isgraph</span>(c: <span class="STpe">s32</span>)-&gt;<span class="STpe">s32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_isgreater"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">isgreater</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L197" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">isgreater</span>(x: <span class="STpe">f32</span>, y: <span class="STpe">f32</span>)-&gt;<span class="STpe">bool</span>
<span class="SKwd">func</span> <span class="SFct">isgreater</span>(x: <span class="STpe">f64</span>, y: <span class="STpe">f64</span>)-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_isgreaterequal"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">isgreaterequal</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L199" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">isgreaterequal</span>(x: <span class="STpe">f32</span>, y: <span class="STpe">f32</span>)-&gt;<span class="STpe">bool</span>
<span class="SKwd">func</span> <span class="SFct">isgreaterequal</span>(x: <span class="STpe">f64</span>, y: <span class="STpe">f64</span>)-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_isinf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">isinf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L190" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">isinf</span>(x: <span class="STpe">f32</span>)-&gt;<span class="STpe">bool</span>
<span class="SKwd">func</span> <span class="SFct">isinf</span>(x: <span class="STpe">f64</span>)-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_isless"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">isless</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L201" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">isless</span>(x: <span class="STpe">f32</span>, y: <span class="STpe">f32</span>)-&gt;<span class="STpe">bool</span>
<span class="SKwd">func</span> <span class="SFct">isless</span>(x: <span class="STpe">f64</span>, y: <span class="STpe">f64</span>)-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_islessequal"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">islessequal</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L203" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">islessequal</span>(x: <span class="STpe">f32</span>, y: <span class="STpe">f32</span>)-&gt;<span class="STpe">bool</span>
<span class="SKwd">func</span> <span class="SFct">islessequal</span>(x: <span class="STpe">f64</span>, y: <span class="STpe">f64</span>)-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_islessgreater"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">islessgreater</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L205" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">islessgreater</span>(x: <span class="STpe">f32</span>, y: <span class="STpe">f32</span>)-&gt;<span class="STpe">bool</span>
<span class="SKwd">func</span> <span class="SFct">islessgreater</span>(x: <span class="STpe">f64</span>, y: <span class="STpe">f64</span>)-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_islower"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">islower</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\ctype.swg#L12" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">islower</span>(c: <span class="STpe">s32</span>)-&gt;<span class="STpe">s32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_isnan"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">isnan</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L192" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">isnan</span>(x: <span class="STpe">f32</span>)-&gt;<span class="STpe">bool</span>
<span class="SKwd">func</span> <span class="SFct">isnan</span>(x: <span class="STpe">f64</span>)-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_isnormal"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">isnormal</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L194" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">isnormal</span>(x: <span class="STpe">f32</span>)-&gt;<span class="STpe">bool</span>
<span class="SKwd">func</span> <span class="SFct">isnormal</span>(x: <span class="STpe">f64</span>)-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_isprint"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">isprint</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\ctype.swg#L13" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">isprint</span>(c: <span class="STpe">s32</span>)-&gt;<span class="STpe">s32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_ispunct"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">ispunct</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\ctype.swg#L14" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">ispunct</span>(c: <span class="STpe">s32</span>)-&gt;<span class="STpe">s32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_isspace"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">isspace</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\ctype.swg#L15" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">isspace</span>(c: <span class="STpe">s32</span>)-&gt;<span class="STpe">s32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_isunordered"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">isunordered</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L185" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">isunordered</span>(x: <span class="STpe">f32</span>, y: <span class="STpe">f32</span>)-&gt;<span class="STpe">bool</span>
<span class="SKwd">func</span> <span class="SFct">isunordered</span>(x: <span class="STpe">f64</span>, y: <span class="STpe">f64</span>)-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_isupper"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">isupper</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\ctype.swg#L16" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">isupper</span>(c: <span class="STpe">s32</span>)-&gt;<span class="STpe">s32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_isxdigi"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">isxdigi</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\ctype.swg#L17" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">isxdigi</span>(c: <span class="STpe">s32</span>)-&gt;<span class="STpe">s32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_labs"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">labs</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdlib.swg#L46" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">labs</span>(j: <span class="STpe">s32</span>)-&gt;<span class="STpe">s32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_ldexp"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">ldexp</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L58" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">ldexp</span>(x: <span class="STpe">f64</span>, exp: <span class="STpe">s64</span>)-&gt;<span class="STpe">f64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_ldexpf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">ldexpf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L59" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">ldexpf</span>(x: <span class="STpe">f32</span>, exp: <span class="STpe">s64</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_ldiv"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">ldiv</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdlib.swg#L49" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">ldiv</span>(numer: <span class="STpe">s32</span>, denom: <span class="STpe">s32</span>)-&gt;<span class="SCst">Libc</span>.<a href="#Libc_ldiv_t">ldiv_t</a></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_ldiv_t"><span class="titletype">struct</span> <span class="titlelight">Libc.</span><span class="titlestrong">ldiv_t</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdlib.swg#L10" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SCde">quot</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">rem</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_lgamma"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">lgamma</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L92" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">lgamma</span>(x: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_lgammaf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">lgammaf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L93" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">lgammaf</span>(x: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_llabs"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">llabs</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdlib.swg#L47" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">llabs</span>(j: <span class="STpe">s64</span>)-&gt;<span class="STpe">s64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_lldiv"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">lldiv</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdlib.swg#L50" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">lldiv</span>(numer: <span class="STpe">s64</span>, denom: <span class="STpe">s64</span>)-&gt;<span class="SCst">Libc</span>.<a href="#Libc_lldiv_t">lldiv_t</a></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_lldiv_t"><span class="titletype">struct</span> <span class="titlelight">Libc.</span><span class="titlestrong">lldiv_t</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdlib.swg#L11" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SCde">quot</span></td>
<td class="codetype"><span class="STpe">s64</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">rem</span></td>
<td class="codetype"><span class="STpe">s64</span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_llrint"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">llrint</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L107" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">llrint</span>(x: <span class="STpe">f64</span>)-&gt;<span class="STpe">s64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_llrintf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">llrintf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L108" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">llrintf</span>(x: <span class="STpe">f32</span>)-&gt;<span class="STpe">s64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_llround"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">llround</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L113" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">llround</span>(x: <span class="STpe">f64</span>)-&gt;<span class="STpe">s64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_llroundf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">llroundf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L114" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">llroundf</span>(x: <span class="STpe">f32</span>)-&gt;<span class="STpe">s64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_localtime"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">localtime</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\time.swg#L17" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">localtime</span>(timer: *<span class="SCst">Libc</span>.<a href="#Libc_time_t">time_t</a>)-&gt;*<span class="SCst">Libc</span>.<a href="#Libc_tm">tm</a></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_log"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">log</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L60" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">log</span>(x: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_log10"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">log10</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L62" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">log10</span>(x: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_log10f"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">log10f</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L63" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">log10f</span>(x: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_log1p"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">log1p</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L64" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">log1p</span>(x: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_log1pf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">log1pf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L65" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">log1pf</span>(x: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_log2"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">log2</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L66" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">log2</span>(x: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_log2f"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">log2f</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L67" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">log2f</span>(x: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_logb"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">logb</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L68" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">logb</span>(x: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_logbf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">logbf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L69" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">logbf</span>(x: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_logf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">logf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L61" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">logf</span>(x: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_lrint"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">lrint</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L105" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">lrint</span>(x: <span class="STpe">f64</span>)-&gt;<span class="STpe">s32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_lrintf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">lrintf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L106" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">lrintf</span>(x: <span class="STpe">f32</span>)-&gt;<span class="STpe">s32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_lround"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">lround</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L111" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">lround</span>(x: <span class="STpe">f64</span>)-&gt;<span class="STpe">s32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_lroundf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">lroundf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L112" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">lroundf</span>(x: <span class="STpe">f32</span>)-&gt;<span class="STpe">s32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_malloc"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">malloc</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdlib.swg#L32" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">malloc</span>(size: <span class="SCst">Libc</span>.<a href="#Libc_size_t">size_t</a>)-&gt;*<span class="STpe">void</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_mblen"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">mblen</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdlib.swg#L52" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">mblen</span>(s: <span class="STpe">cstring</span>, n: <span class="SCst">Libc</span>.<a href="#Libc_size_t">size_t</a>)-&gt;<span class="STpe">s32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_mbstowcs"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">mbstowcs</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdlib.swg#L56" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">mbstowcs</span>(pwcs: *<span class="SCst">Libc</span>.<a href="#Libc_wchar_t">wchar_t</a>, s: <span class="STpe">cstring</span>, n: <span class="SCst">Libc</span>.<a href="#Libc_size_t">size_t</a>)-&gt;<span class="SCst">Libc</span>.<a href="#Libc_size_t">size_t</a></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_mbtowc"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">mbtowc</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdlib.swg#L53" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">mbtowc</span>(pwc: *<span class="SCst">Libc</span>.<a href="#Libc_wchar_t">wchar_t</a>, s: <span class="STpe">cstring</span>, n: <span class="SCst">Libc</span>.<a href="#Libc_size_t">size_t</a>)-&gt;<span class="STpe">s32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_memchr"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">memchr</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\string.swg#L19" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">memchr</span>(ptr: <span class="SKwd">const</span> *<span class="STpe">void</span>, value: <span class="STpe">s32</span>, num: <span class="SCst">Libc</span>.<a href="#Libc_size_t">size_t</a>)-&gt;*<span class="STpe">void</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_memcmp"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">memcmp</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\string.swg#L18" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">memcmp</span>(ptr1: <span class="SKwd">const</span> *<span class="STpe">void</span>, ptr2: <span class="SKwd">const</span> *<span class="STpe">void</span>, num: <span class="SCst">Libc</span>.<a href="#Libc_size_t">size_t</a>)-&gt;<span class="STpe">s32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_memcpy"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">memcpy</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\string.swg#L6" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">memcpy</span>(dst: *<span class="STpe">void</span>, src: <span class="SKwd">const</span> *<span class="STpe">void</span>, num: <span class="SCst">Libc</span>.<a href="#Libc_size_t">size_t</a>)-&gt;*<span class="STpe">void</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_memmove"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">memmove</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\string.swg#L7" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">memmove</span>(dst: *<span class="STpe">void</span>, src: <span class="SKwd">const</span> *<span class="STpe">void</span>, num: <span class="SCst">Libc</span>.<a href="#Libc_size_t">size_t</a>)-&gt;*<span class="STpe">void</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_memset"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">memset</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\string.swg#L8" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">memset</span>(ptr: *<span class="STpe">void</span>, value: <span class="STpe">s32</span>, num: <span class="SCst">Libc</span>.<a href="#Libc_size_t">size_t</a>)-&gt;*<span class="STpe">void</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_mktime"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">mktime</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\time.swg#L7" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">mktime</span>(timeptr: *<span class="SCst">Libc</span>.<a href="#Libc_tm">tm</a>)-&gt;<span class="SCst">Libc</span>.<a href="#Libc_time_t">time_t</a></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_modf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">modf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L70" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">modf</span>(value: <span class="STpe">f64</span>, iptr: *<span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_modff"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">modff</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L71" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">modff</span>(value: <span class="STpe">f32</span>, iptr: *<span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_nan"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">nan</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L127" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">nan</span>(tagp: <span class="STpe">cstring</span>)-&gt;<span class="STpe">f64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_nanf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">nanf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L128" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">nanf</span>(tagp: <span class="STpe">cstring</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_nearbyint"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">nearbyint</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L101" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">nearbyint</span>(x: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_nearbyintf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">nearbyintf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L102" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">nearbyintf</span>(x: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_nextafter"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">nextafter</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L129" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">nextafter</span>(x: <span class="STpe">f64</span>, y: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_nextafterf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">nextafterf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L130" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">nextafterf</span>(x: <span class="STpe">f32</span>, y: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_perror"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">perror</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdio.swg#L56" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">perror</span>(s: <span class="STpe">cstring</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_pow"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">pow</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L83" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">pow</span>(x: <span class="STpe">f64</span>, y: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_powf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">powf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L84" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">powf</span>(x: <span class="STpe">f32</span>, y: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_printf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">printf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdio.swg#L95" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">printf</span>(format: <span class="STpe">cstring</span>, args: <span class="STpe">cvarargs</span>)-&gt;<span class="STpe">s64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_putc"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">putc</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdio.swg#L40" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">putc</span>(c: <span class="STpe">s64</span>, stream: *<span class="SCst">Libc</span>.<span class="SCst"><a href="#Libc_FILE">FILE</a></span>)-&gt;<span class="STpe">s64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_putchar"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">putchar</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdio.swg#L41" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">putchar</span>()-&gt;<span class="STpe">s64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_puts"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">puts</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdio.swg#L42" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">puts</span>(s: <span class="STpe">cstring</span>)-&gt;<span class="STpe">s64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_qsort"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">qsort</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdlib.swg#L43" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">qsort</span>(base: *<span class="STpe">void</span>, num: <span class="SCst">Libc</span>.<a href="#Libc_size_t">size_t</a>, size: <span class="SCst">Libc</span>.<a href="#Libc_size_t">size_t</a>, compar: *<span class="STpe">void</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_quick_exit"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">quick_exit</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdlib.swg#L39" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">quick_exit</span>(status: <span class="STpe">s32</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_rand"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">rand</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdlib.swg#L26" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">rand</span>()-&gt;<span class="STpe">s32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_realloc"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">realloc</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdlib.swg#L33" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">realloc</span>(ptr: *<span class="STpe">void</span>, size: <span class="SCst">Libc</span>.<a href="#Libc_size_t">size_t</a>)-&gt;*<span class="STpe">void</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_remainder"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">remainder</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L120" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">remainder</span>(x: <span class="STpe">f64</span>, y: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_remainderf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">remainderf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L121" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">remainderf</span>(x: <span class="STpe">f32</span>, y: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_remove"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">remove</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdio.swg#L23" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">remove</span>(filename: <span class="STpe">cstring</span>)-&gt;<span class="STpe">s64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_remquo"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">remquo</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L122" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">remquo</span>(x: <span class="STpe">f64</span>, y: <span class="STpe">f64</span>, quo: *<span class="STpe">s64</span>)-&gt;<span class="STpe">f64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_remquof"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">remquof</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L123" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">remquof</span>(x: <span class="STpe">f32</span>, y: <span class="STpe">f32</span>, quo: *<span class="STpe">s64</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_rename"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">rename</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdio.swg#L24" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">rename</span>(old: <span class="STpe">cstring</span>, new: <span class="STpe">cstring</span>)-&gt;<span class="STpe">s64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_rewind"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">rewind</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdio.swg#L51" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">rewind</span>(stream: *<span class="SCst">Libc</span>.<span class="SCst"><a href="#Libc_FILE">FILE</a></span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_rint"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">rint</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L103" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">rint</span>(x: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_rintf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">rintf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L104" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">rintf</span>(x: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_round"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">round</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L109" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">round</span>(x: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_roundf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">roundf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L110" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">roundf</span>(x: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_scalbln"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">scalbln</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L74" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">scalbln</span>(x: <span class="STpe">f64</span>, n: <span class="STpe">s32</span>)-&gt;<span class="STpe">f64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_scalblnf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">scalblnf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L75" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">scalblnf</span>(x: <span class="STpe">f32</span>, n: <span class="STpe">s32</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_scalbn"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">scalbn</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L72" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">scalbn</span>(x: <span class="STpe">f64</span>, n: <span class="STpe">s64</span>)-&gt;<span class="STpe">f64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_scalbnf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">scalbnf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L73" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">scalbnf</span>(x: <span class="STpe">f32</span>, n: <span class="STpe">s64</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_scanf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">scanf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdio.swg#L149" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">scanf</span>(format: <span class="STpe">cstring</span>, args: <span class="STpe">cvarargs</span>)-&gt;<span class="STpe">s64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_setbuf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">setbuf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdio.swg#L32" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setbuf</span>(stream: *<span class="SCst">Libc</span>.<span class="SCst"><a href="#Libc_FILE">FILE</a></span>, buf: *<span class="STpe">u8</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_setvbuf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">setvbuf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdio.swg#L33" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setvbuf</span>(stream: *<span class="SCst">Libc</span>.<span class="SCst"><a href="#Libc_FILE">FILE</a></span>, buf: *<span class="STpe">u8</span>, mode: <span class="STpe">s64</span>, size: <span class="SCst">Libc</span>.<a href="#Libc_size_t">size_t</a>)-&gt;<span class="STpe">s64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_signbit"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">signbit</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L182" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">signbit</span>(x: <span class="STpe">f32</span>)-&gt;<span class="STpe">s32</span>
<span class="SKwd">func</span> <span class="SFct">signbit</span>(x: <span class="STpe">f64</span>)-&gt;<span class="STpe">s32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_sin"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">sin</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L30" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">sin</span>(x: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_sinf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">sinf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L31" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">sinf</span>(x: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_sinh"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">sinh</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L43" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">sinh</span>(x: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_sinhf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">sinhf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L44" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">sinhf</span>(x: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_snprintf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">snprintf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdio.swg#L115" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">snprintf</span>(s: *<span class="STpe">u8</span>, count: <span class="SCst">Libc</span>.<a href="#Libc_size_t">size_t</a>, format: <span class="STpe">cstring</span>, args: <span class="STpe">cvarargs</span>)-&gt;<span class="STpe">s64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_sprintf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">sprintf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdio.swg#L105" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">sprintf</span>(s: *<span class="STpe">u8</span>, format: <span class="STpe">cstring</span>, args: <span class="STpe">cvarargs</span>)-&gt;<span class="STpe">s64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_sqrt"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">sqrt</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L85" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">sqrt</span>(x: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_sqrtf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">sqrtf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L86" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">sqrtf</span>(x: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_srand"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">srand</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdlib.swg#L27" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">srand</span>(seed: <span class="STpe">u32</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_sscanf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">sscanf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdio.swg#L181" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">sscanf</span>(s: <span class="STpe">cstring</span>, format: <span class="STpe">cstring</span>, args: <span class="STpe">cvarargs</span>)-&gt;<span class="STpe">s64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_stdio_common_vfprintf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">stdio_common_vfprintf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdio.swg#L66" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">stdio_common_vfprintf</span>(_Options: <span class="STpe">u64</span>, _Stream: *<span class="SCst">Libc</span>.<span class="SCst"><a href="#Libc_FILE">FILE</a></span>, _Format: <span class="STpe">cstring</span>, _Locale: *<span class="STpe">void</span>, _ArgList: *<span class="STpe">void</span>)-&gt;<span class="STpe">s32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_stdio_common_vfscanf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">stdio_common_vfscanf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdio.swg#L70" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">stdio_common_vfscanf</span>(_Options: <span class="STpe">u64</span>, _Stream: *<span class="SCst">Libc</span>.<span class="SCst"><a href="#Libc_FILE">FILE</a></span>, _Format: <span class="STpe">cstring</span>, _Locale: *<span class="STpe">void</span>, _ArgList: *<span class="STpe">void</span>)-&gt;<span class="STpe">s32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_stdio_common_vsprintf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">stdio_common_vsprintf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdio.swg#L68" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">stdio_common_vsprintf</span>(_Options: <span class="STpe">u64</span>, _Buffer: *<span class="STpe">u8</span>, _BufferCount: <span class="SCst">Libc</span>.<a href="#Libc_size_t">size_t</a>, _Format: <span class="STpe">cstring</span>, _Locale: *<span class="STpe">void</span>, _ArgList: *<span class="STpe">void</span>)-&gt;<span class="STpe">s32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_stdio_common_vsscanf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">stdio_common_vsscanf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdio.swg#L72" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">stdio_common_vsscanf</span>(_Options: <span class="STpe">u64</span>, _Buffer: <span class="STpe">cstring</span>, _BufferCount: <span class="SCst">Libc</span>.<a href="#Libc_size_t">size_t</a>, _Format: <span class="STpe">cstring</span>, _Locale: *<span class="STpe">void</span>, _ArgList: *<span class="STpe">void</span>)-&gt;<span class="STpe">s32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_strcat"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">strcat</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\string.swg#L12" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">strcat</span>(dst: *<span class="STpe">u8</span>, src: <span class="STpe">cstring</span>)-&gt;*<span class="STpe">u8</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_strchr"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">strchr</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\string.swg#L27" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">strchr</span>(str: <span class="STpe">cstring</span>, character: <span class="STpe">s32</span>)-&gt;*<span class="STpe">u8</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_strcmp"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">strcmp</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\string.swg#L28" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">strcmp</span>(str1: <span class="STpe">cstring</span>, str2: <span class="STpe">cstring</span>)-&gt;<span class="STpe">s32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_strcoll"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">strcoll</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\string.swg#L29" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">strcoll</span>(str1: <span class="STpe">cstring</span>, str2: <span class="STpe">cstring</span>)-&gt;<span class="STpe">s32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_strcpy"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">strcpy</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\string.swg#L10" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">strcpy</span>(dst: *<span class="STpe">u8</span>, src: <span class="STpe">cstring</span>)-&gt;*<span class="STpe">u8</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_strcspn"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">strcspn</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\string.swg#L21" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">strcspn</span>(str1: <span class="STpe">cstring</span>, str2: <span class="STpe">cstring</span>)-&gt;<span class="SCst">Libc</span>.<a href="#Libc_size_t">size_t</a></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_strerror"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">strerror</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\string.swg#L32" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">strerror</span>(errnum: <span class="STpe">s32</span>)-&gt;<span class="STpe">cstring</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_strftime"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">strftime</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\time.swg#L9" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">strftime</span>(s: *<span class="STpe">u8</span>, maxsize: <span class="SCst">Libc</span>.<a href="#Libc_size_t">size_t</a>, format: <span class="SKwd">const</span> *<span class="STpe">u8</span>, timeptr: *<span class="SCst">Libc</span>.<a href="#Libc_tm">tm</a>)-&gt;<span class="SCst">Libc</span>.<a href="#Libc_size_t">size_t</a></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_strlen"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">strlen</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\string.swg#L33" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">strlen</span>(s: <span class="STpe">cstring</span>)-&gt;<span class="SCst">Libc</span>.<a href="#Libc_size_t">size_t</a></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_strncat"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">strncat</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\string.swg#L13" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">strncat</span>(dst: *<span class="STpe">u8</span>, src: <span class="STpe">cstring</span>, num: <span class="SCst">Libc</span>.<a href="#Libc_size_t">size_t</a>)-&gt;*<span class="STpe">u8</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_strncmp"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">strncmp</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\string.swg#L30" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">strncmp</span>(str1: <span class="STpe">cstring</span>, str2: <span class="STpe">cstring</span>, num: <span class="STpe">u64</span>)-&gt;<span class="STpe">s32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_strncpy"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">strncpy</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\string.swg#L11" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">strncpy</span>(dst: *<span class="STpe">u8</span>, src: <span class="STpe">cstring</span>, num: <span class="SCst">Libc</span>.<a href="#Libc_size_t">size_t</a>)-&gt;*<span class="STpe">u8</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_strpbrk"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">strpbrk</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\string.swg#L22" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">strpbrk</span>(str1: *<span class="STpe">u8</span>, str2: <span class="STpe">cstring</span>)-&gt;*<span class="STpe">u8</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_strrchr"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">strrchr</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\string.swg#L23" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">strrchr</span>(str: *<span class="STpe">u8</span>, character: <span class="STpe">s32</span>)-&gt;*<span class="STpe">u8</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_strspn"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">strspn</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\string.swg#L24" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">strspn</span>(str1: <span class="STpe">cstring</span>, str2: <span class="STpe">cstring</span>)-&gt;<span class="SCst">Libc</span>.<a href="#Libc_size_t">size_t</a></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_strstr"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">strstr</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\string.swg#L25" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">strstr</span>(str1: *<span class="STpe">u8</span>, str2: <span class="STpe">cstring</span>)-&gt;*<span class="STpe">u8</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_strtod"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">strtod</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdlib.swg#L19" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">strtod</span>(nptr: <span class="STpe">cstring</span>, endptr: **<span class="STpe">u8</span>)-&gt;<span class="STpe">f64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_strtof"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">strtof</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdlib.swg#L20" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">strtof</span>(nptr: <span class="STpe">cstring</span>, endptr: **<span class="STpe">u8</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_strtok"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">strtok</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\string.swg#L26" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">strtok</span>(str: *<span class="STpe">u8</span>, delimiters: <span class="STpe">cstring</span>)-&gt;*<span class="STpe">u8</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_strtol"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">strtol</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdlib.swg#L21" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">strtol</span>(nptr: <span class="STpe">cstring</span>, endptr: **<span class="STpe">u8</span>, base: <span class="STpe">s32</span>)-&gt;<span class="STpe">s32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_strtoll"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">strtoll</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdlib.swg#L22" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">strtoll</span>(nptr: <span class="STpe">cstring</span>, endptr: **<span class="STpe">u8</span>, base: <span class="STpe">s32</span>)-&gt;<span class="STpe">s64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_strtoul"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">strtoul</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdlib.swg#L23" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">strtoul</span>(nptr: <span class="STpe">cstring</span>, endptr: **<span class="STpe">u8</span>, base: <span class="STpe">s32</span>)-&gt;<span class="STpe">u32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_strtoull"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">strtoull</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdlib.swg#L24" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">strtoull</span>(nptr: <span class="STpe">cstring</span>, endptr: **<span class="STpe">u8</span>, base: <span class="STpe">s32</span>)-&gt;<span class="STpe">u64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_strxfrm"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">strxfrm</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\string.swg#L31" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">strxfrm</span>(dst: *<span class="STpe">u8</span>, src: <span class="STpe">cstring</span>, num: <span class="SCst">Libc</span>.<a href="#Libc_size_t">size_t</a>)-&gt;<span class="SCst">Libc</span>.<a href="#Libc_size_t">size_t</a></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_system"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">system</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdlib.swg#L40" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">system</span>(cmd: <span class="STpe">cstring</span>)-&gt;<span class="STpe">s32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_tan"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">tan</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L32" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">tan</span>(x: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_tanf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">tanf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L33" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">tanf</span>(x: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_tanh"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">tanh</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L45" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">tanh</span>(x: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_tanhf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">tanhf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L46" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">tanhf</span>(x: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_tgamma"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">tgamma</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L94" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">tgamma</span>(x: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_tgammaf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">tgammaf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L95" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">tgammaf</span>(x: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_time"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">time</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\time.swg#L15" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">time</span>(timer: *<span class="SCst">Libc</span>.<a href="#Libc_time_t">time_t</a>)-&gt;<span class="SCst">Libc</span>.<a href="#Libc_time_t">time_t</a></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_tm"><span class="titletype">struct</span> <span class="titlelight">Libc.</span><span class="titlestrong">tm</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\time.swg#L27" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SCde">tm_sec</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tm_min</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tm_hour</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tm_mday</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tm_mon</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tm_year</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tm_wday</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tm_yday</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tm_isdst</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_tmpfile"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">tmpfile</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdio.swg#L25" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">tmpfile</span>()-&gt;*<span class="SCst">Libc</span>.<span class="SCst"><a href="#Libc_FILE">FILE</a></span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_tmpnam"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">tmpnam</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdio.swg#L26" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">tmpnam</span>(s: *<span class="STpe">u8</span>)-&gt;*<span class="STpe">u8</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_tolower"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">tolower</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\ctype.swg#L18" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">tolower</span>(c: <span class="STpe">s32</span>)-&gt;<span class="STpe">s32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_toupper"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">toupper</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\ctype.swg#L19" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">toupper</span>(c: <span class="STpe">s32</span>)-&gt;<span class="STpe">s32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_trunc"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">trunc</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L115" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">trunc</span>(x: <span class="STpe">f64</span>)-&gt;<span class="STpe">f64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_truncf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">truncf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\math.swg#L116" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">truncf</span>(x: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_ungetc"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">ungetc</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdio.swg#L43" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">ungetc</span>(c: <span class="STpe">s64</span>, stream: *<span class="SCst">Libc</span>.<span class="SCst"><a href="#Libc_FILE">FILE</a></span>)-&gt;<span class="STpe">s64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_va_end"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">va_end</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdarg.swg#L8" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SAtr">#[<a href="swag.runtime.php#Swag_Macro">Swag.Macro</a>]</span>
<span class="SKwd">func</span> <span class="SFct">va_end</span>(vl: *<span class="SCst">Swag</span>.<span class="SCst">CVaList</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_va_start"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">va_start</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdarg.swg#L7" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SAtr">#[<a href="swag.runtime.php#Swag_Macro">Swag.Macro</a>]</span>
<span class="SKwd">func</span> <span class="SFct">va_start</span>(vl: *<span class="SCst">Swag</span>.<span class="SCst">CVaList</span>, dummy: <span class="STpe">any</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_vfprintf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">vfprintf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdio.swg#L131" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">vfprintf</span>(file: *<span class="SCst">Libc</span>.<span class="SCst"><a href="#Libc_FILE">FILE</a></span>, format: <span class="STpe">cstring</span>, args: *<span class="SCst">Swag</span>.<span class="SCst">CVaList</span>)-&gt;<span class="STpe">s64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_vfscanf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">vfscanf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdio.swg#L169" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">vfscanf</span>(file: *<span class="SCst">Libc</span>.<span class="SCst"><a href="#Libc_FILE">FILE</a></span>, format: <span class="STpe">cstring</span>, args: *<span class="SCst">Swag</span>.<span class="SCst">CVaList</span>)-&gt;<span class="STpe">s64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_vprintf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">vprintf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdio.swg#L125" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">vprintf</span>(format: <span class="STpe">cstring</span>, args: *<span class="SCst">Swag</span>.<span class="SCst">CVaList</span>)-&gt;<span class="STpe">s64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_vscanf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">vscanf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdio.swg#L175" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">vscanf</span>(format: <span class="STpe">cstring</span>, args: *<span class="SCst">Swag</span>.<span class="SCst">CVaList</span>)-&gt;<span class="STpe">s64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_vsnprintf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">vsnprintf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdio.swg#L143" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">vsnprintf</span>(s: *<span class="STpe">u8</span>, count: <span class="SCst">Libc</span>.<a href="#Libc_size_t">size_t</a>, format: <span class="STpe">cstring</span>, args: *<span class="SCst">Swag</span>.<span class="SCst">CVaList</span>)-&gt;<span class="STpe">s64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_vsprintf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">vsprintf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdio.swg#L137" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">vsprintf</span>(s: *<span class="STpe">u8</span>, format: <span class="STpe">cstring</span>, args: *<span class="SCst">Swag</span>.<span class="SCst">CVaList</span>)-&gt;<span class="STpe">s64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_vsscanf"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">vsscanf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdio.swg#L191" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">vsscanf</span>(s: <span class="STpe">cstring</span>, format: <span class="STpe">cstring</span>, args: *<span class="SCst">Swag</span>.<span class="SCst">CVaList</span>)-&gt;<span class="STpe">s64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_wcstombs"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">wcstombs</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdlib.swg#L57" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">wcstombs</span>(s: *<span class="STpe">u8</span>, pwcs: *<span class="SCst">Libc</span>.<a href="#Libc_wchar_t">wchar_t</a>, n: <span class="SCst">Libc</span>.<a href="#Libc_size_t">size_t</a>)-&gt;<span class="SCst">Libc</span>.<a href="#Libc_size_t">size_t</a></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Libc_wctomb"><span class="titletype">func</span> <span class="titlelight">Libc.</span><span class="titlestrong">wctomb</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/libc\src\stdlib.swg#L54" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">wctomb</span>(s: *<span class="STpe">u8</span>, wc: <span class="SCst">Libc</span>.<a href="#Libc_wchar_t">wchar_t</a>)-&gt;<span class="STpe">s32</span></span></code>
</div>
</div>
</div>
</div>
</body>
</html>
