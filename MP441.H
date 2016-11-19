
//open MP441
extern "C" HANDLE __declspec(dllexport)  __stdcall MP441_OpenDevice(__int32 dev_num);
//close Device
extern "C" __int32 __declspec(dllexport)  __stdcall MP441_CloseDevice(HANDLE hDevice1);
//********************************************
//get board info
//model or type in *bStr
extern "C" __int32 __declspec(dllexport)  __stdcall MP441_Info(HANDLE hDevice,char *modle);
//read DI
extern "C" __int32 __declspec(dllexport)  __stdcall MP441_DI(HANDLE hDevice,__int32 ionum);
//write do
extern "C" __int32 __declspec(dllexport)  __stdcall MP441_DO(HANDLE hDevice,__int32 ionum,__int32 iodata);
