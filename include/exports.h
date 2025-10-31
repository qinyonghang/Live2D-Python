#ifdef _WIN32
#ifdef LIVE2D_EXPORTS
#define LIVE2D_API __declspec(dllexport)
#else
#define LIVE2D_API __declspec(dllimport)
#endif
#else
#define LIVE2D_API
#endif