/*
    Utilidades de color ANSI para la consola (códigos de escape).
*/
#pragma once
namespace Ansi {
inline constexpr const char* reset  = "\x1b[0m";
inline constexpr const char* dim    = "\x1b[2m";
inline constexpr const char* red    = "\x1b[31m";
inline constexpr const char* green  = "\x1b[32m";
inline constexpr const char* yellow = "\x1b[33m";
inline constexpr const char* cyan   = "\x1b[36m";


}

