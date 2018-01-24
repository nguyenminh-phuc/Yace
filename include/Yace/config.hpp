#ifndef YACE_CONFIG_HPP
#define YACE_CONFIG_HPP

#include <climits>

#ifdef _MSC_VER
#  define YACE_DLLEXPORT __declspec(dllexport)
#  define YACE_DLLIMPORT __declspec(dllimport)
#elif defined(__GNUC__)
#  define YACE_DLLEXPORT __attribute__((visibility("default")))
#  define YACE_DLLIMPORT
#else
#   error("Unknown compiler")
#endif

#ifdef _YACE_BUILD_DLL
#  define YACE_API      YACE_DLLEXPORT
#elif defined(YACE_DLL)
#  define YACE_API      YACE_DLLIMPORT
#else
#  define YACE_API
#endif

#ifdef _MSC_VER
#  pragma warning(disable: 4251) // class 'X' needs to have dll-interface to be used by clients of class 'Y'
#  pragma warning(disable: 4201) // nonstandard extension used: nameless struct/union
#endif

#ifdef NDEBUG
#   define YACE_LOG(...)
#else
#   define YACE_LOG(...) fprintf(stderr, ##__VA_ARGS__)
#endif

#define YACE_APPLICATION   ye::application::get_instance()
#define YACE_WINDOW        YACE_APPLICATION.get_window()
#define YACE_OPENGL_MAJOR  3
#define YACE_OPENGL_MINOR  3
#define YACE_FRAMERATE     60 // Chip-8 is designed to execute 60 opcodes per second
#define YACE_SCREEN_WIDTH  64
#define YACE_SCREEN_HEIGHT 32

static_assert(CHAR_BIT == 8, "CHAR_BIT != 8");

#endif
