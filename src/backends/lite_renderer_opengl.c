#include <stdio.h>

#include "lite_renderer.h"

// Prefer low-battery GPU, so disable optimus!
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
__declspec(dllexport) DWORD  NvOptimusEnablement                    = 0; // https://developer.download.nvidia.com/devzone/devcenter/gamegraphics/files/OptimusRenderingPolicies.pdf
__declspec(dllexport) int    AmdPowerXpressRequestHighPerformance   = 0; // https://developer.amd.com/community/blog/2015/10/02/amd-enduro-system-for-developers/
#endif

//! EOF

