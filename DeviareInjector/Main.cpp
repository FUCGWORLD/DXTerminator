#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "NktHookLib.h"

 //-----------------------------------------------------------

static LPSTR ToAnsi(__in_z LPCWSTR sW);

//-----------------------------------------------------------


int __CRTDECL wmain(__in int argc, __in wchar_t* argv[]) {
        DWORD dwOsErr;

        //目标程序路径
        LPWSTR szExeNameW  = (LPWSTR)L"C:\\Program Files\\Star Rail\\Game\\StarRail.exe";
        //目标Dll路径
        LPWSTR szDllToInjectNameW = (LPWSTR)L"C:\\Users\\Administrator\\Desktop\\HSR_DEV\\d3d11.dll";
        LPSTR szInitFunctionA = (LPSTR)"DllMain";

        STARTUPINFOW sSiW;
        PROCESS_INFORMATION sPi;

        memset(&sSiW, 0, sizeof(sSiW));
        sSiW.cb = (DWORD)sizeof(sSiW);
        memset(&sPi, 0, sizeof(sPi));
        dwOsErr = NktHookLibHelpers::CreateProcessWithDllW(szExeNameW, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &sSiW, &sPi,
            szDllToInjectNameW, NULL, NULL);
        if (dwOsErr == ERROR_SUCCESS)
        {
            wprintf_s(L"Process #%lu successfully launched with dll injected!\n", sPi.dwProcessId);
            ::CloseHandle(sPi.hThread);
            ::CloseHandle(sPi.hProcess);
        }
        else if (dwOsErr == ERROR_PATH_NOT_FOUND) {
            wprintf_s(L"Error %lu: Cannot find target program path.\n", dwOsErr);
        }
        else
        {
            wprintf_s(L"Error %lu: Cannot launch process and inject dll.\n", dwOsErr);
        }
}


//int __CRTDECL wmain(__in int argc, __in wchar_t* argv[], __in wchar_t* envp[])
//{
//    DWORD dwOsErr, dwPid;
//    LPWSTR szExeNameW, szDllToInjectNameW;
//    LPSTR szInitFunctionA;
//
//    //check arguments
//    if (argc < 3)
//    {
//        wprintf_s(L"Use: InjectDLL path-to-exe|process-id path-to-dll [initialize-function-name]\n");
//        return 1;
//    }
//    //if first argument is numeric, assume a process ID
//    if (argv[1][0] >= L'0' && argv[1][0] <= L'9')
//    {
//        LPWSTR szStopW;
//
//        dwPid = (DWORD)wcstoul(argv[1], &szStopW, 10);
//        if (dwPid == 0 || *szStopW != 0)
//        {
//            wprintf_s(L"Error: Invalid process ID specified.\n");
//            return 1;
//        }
//        if (dwPid == ::GetCurrentProcessId())
//        {
//            wprintf_s(L"Error: Cannot inject a dll into myself.\n");
//            return 1;
//        }
//        szExeNameW = NULL;
//    }
//    else
//    {
//        //assume a process path to execute
//        dwPid = 0;
//        szExeNameW = argv[1];
//    }
//
//    //take dll name
//    if (argv[2][0] == 0)
//    {
//        wprintf_s(L"Error: Invalid dll name specified.\n");
//        return 1;
//    }
//    szDllToInjectNameW = argv[2];
//
//    //is initialize function specified?
//    szInitFunctionA = NULL;
//    if (argc >= 4 && argv[3][0] != 0)
//    {
//        szInitFunctionA = ToAnsi(argv[3]);
//        if (!szInitFunctionA)
//        {
//            wprintf_s(L"Error: Not enough memory.\n");
//            return 1;
//        }
//    }
//
//    //execute action
//    if (dwPid != 0)
//    {
//        //if a process ID was specified, inject dll into that process
//        DWORD dwExitCode;
//        HANDLE hInjectorThread;
//
//        dwOsErr = NktHookLibHelpers::InjectDllByPidW(dwPid, szDllToInjectNameW, szInitFunctionA, 5000, &hInjectorThread);
//        if (dwOsErr == ERROR_SUCCESS)
//        {
//            wprintf_s(L"Dll successfully injected!\n");
//            ::WaitForSingleObject(hInjectorThread, INFINITE);
//            ::GetExitCodeThread(hInjectorThread, &dwExitCode);
//            ::CloseHandle(hInjectorThread);
//            wprintf_s(L"Initialize function return value was: %lu\n", dwExitCode);
//        }
//        else
//        {
//            wprintf_s(L"Error %lu: Cannot inject Dll in target process \n", dwOsErr);
//        }
//    }
//    else
//    {
//        STARTUPINFOW sSiW;
//        PROCESS_INFORMATION sPi;
//
//        memset(&sSiW, 0, sizeof(sSiW));
//        sSiW.cb = (DWORD)sizeof(sSiW);
//        memset(&sPi, 0, sizeof(sPi));
//        dwOsErr = NktHookLibHelpers::CreateProcessWithDllW(szExeNameW, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &sSiW, &sPi,
//            szDllToInjectNameW, NULL, szInitFunctionA);
//        if (dwOsErr == ERROR_SUCCESS)
//        {
//            wprintf_s(L"Process #%lu successfully launched with dll injected!\n", sPi.dwProcessId);
//            ::CloseHandle(sPi.hThread);
//            ::CloseHandle(sPi.hProcess);
//        }
//        else
//        {
//            wprintf_s(L"Error %lu: Cannot launch process and inject dll.\n", dwOsErr);
//        }
//    }
//    free(szInitFunctionA);
//    return (dwOsErr == ERROR_SUCCESS) ? 0 : 2;
//}

//-----------------------------------------------------------

static LPSTR ToAnsi(__in_z LPCWSTR sW)
{
    int srcLen, destLen;
    LPSTR sA;

    srcLen = (int)wcslen(sW);
    if (!srcLen)
    {
        sA = (LPSTR)malloc(sizeof(CHAR));
        if (sA)
            *sA = 0;
        return sA;
    }
    destLen = ::WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK | WC_NO_BEST_FIT_CHARS, sW, srcLen, NULL, 0, NULL, NULL);
    sA = (LPSTR)malloc((destLen + 1) * sizeof(CHAR));
    if (!sA)
        return NULL;
    destLen = ::WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK | WC_NO_BEST_FIT_CHARS, sW, srcLen, sA, destLen, NULL, NULL);
    sA[destLen] = 0;
    return sA;
}

