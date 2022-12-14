#pragma once
#include "pch.h"
#include "PortBenderManager.h"
// Il blocco ifdef seguente viene in genere usato per creare macro che semplificano
// l'esportazione da una DLL. Tutti i file all'interno della DLL sono compilati con il simbolo SLIVERPORTBENDER_EXPORTS
// definito nella riga di comando. Questo simbolo non deve essere definito in alcun progetto
// che usa questa DLL. In questo modo qualsiasi altro progetto i cui file di origine includono questo file vedranno le funzioni
// le funzioni di SLIVERPORTBENDER_API come se fossero importate da una DLL, mentre questa DLL considera i simboli
// definiti con questa macro come esportati.
//#ifdef SLIVERPORTBENDER_EXPORTS
//#define SLIVERPORTBENDER_API __declspec(dllexport)
//#else
//#define SLIVERPORTBENDER_API __declspec(dllimport)
//#endif
//
//// Questa classe viene esportata dalla DLL
//class SLIVERPORTBENDER_API CSliverPortBender {
//public:
//	CSliverPortBender(void);
//	// TODO: aggiungere qui i metodi.
//};
//
//extern SLIVERPORTBENDER_API int nSliverPortBender;
//
//SLIVERPORTBENDER_API int fnSliverPortBender(void);



typedef int (*goCallback)(const char*, int);

extern "C" {
	__declspec(dllexport) int __cdecl entrypoint(char* argsBuffer, uint32_t bufferSize, goCallback callback);
	__declspec(dllexport) int __cdecl fakeEntryPoint();

}

