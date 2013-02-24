#pragma once
#ifndef DEFINES_H
#define DEFINES_H

#include <Windows.h>
#ifndef ERR
#define ERR(X) MessageBox(NULL, (X), L"Error", MB_OK)
#endif
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x) if ((x) != 0) { (x)->Release(); (x) = 0; }
#endif
#ifndef SAFE_SHUTDOWN
#define SAFE_SHUTDOWN(x) if ((x) != 0) { (x)->Shutdown(); delete x; (x) = 0; }
#endif
#endif