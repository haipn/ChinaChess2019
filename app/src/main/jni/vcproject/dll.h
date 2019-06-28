#ifdef DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

class DLL_API Cdll {
public:
	Cdll(void);
};

extern DLL_API int ndll;

DLL_API int fndll(void);
