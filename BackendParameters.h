#pragma once
struct Target;

enum class BackendType
{
    Exe,
    Dll,
    Lib,
};

struct BackendParameters
{
    BackendType    type   = BackendType::Exe;
    Target*        target = nullptr;
    string         postFix;
    vector<string> defines;
};
