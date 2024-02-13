#pragma once
struct Path;
struct BuildParameters;
enum class BuildCfgOutputKind;

struct BackendLinker
{
    static void getArgumentsCoff(const BuildParameters& buildParameters, Vector<Utf8>& arguments, BuildCfgOutputKind outputKind);
    static void getArguments(const BuildParameters& buildParameters, Vector<Utf8>& arguments, BuildCfgOutputKind outputKind);
    static bool link(const BuildParameters& buildParameters, const Vector<Utf8>& linkArguments);

    static bool link(const BuildParameters& buildParameters);
};
