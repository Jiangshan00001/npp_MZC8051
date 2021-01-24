//this file is part of notepad++
//Copyright (C)2003 Don HO <donho@altern.org>
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#include "PluginDefinition.h"
#include "menuCmdID.h"

//
// put the headers you need here
//
#include <stdlib.h>
#include <time.h>
#include <shlwapi.h>

#include <ShellAPI.h>
#include "DockingFeature/GoToLineDlg.h"
#include "myexecute.h"
//#include "osdialog.h"

const TCHAR sectionName[] = TEXT("general");
const TCHAR keyName[] = TEXT("check_update");
const TCHAR configFileName[] = TEXT("MZC8051.ini");
wchar_t g_MODULE_FILE_PATH[1024] = { 0,0,0,0 };


#if 1

DemoDlg _goToLine;
#endif


#ifdef UNICODE 
	#define generic_itoa _itow
#else
	#define generic_itoa itoa
#endif

FuncItem funcItem[nbFunc];

//
// The data of Notepad++ that you can use in your plugin commands
//
NppData nppData;


TCHAR iniFilePath[MAX_PATH];
bool doCloseTag = false;

#define DOCKABLE_DEMO_INDEX 15

//
// Initialize your plugin data here
// It will be called while plugin loading   
void pluginInit(HANDLE hModule)
{
	// Initialize dockable demo dialog
	GetModuleFileName((HINSTANCE)hModule, g_MODULE_FILE_PATH, sizeof(g_MODULE_FILE_PATH) / sizeof(g_MODULE_FILE_PATH[0]) - 2);

	_goToLine.init((HINSTANCE)hModule, NULL);
}

//
// Here you can do the clean up, save the parameters (if any) for the next session
//
void pluginCleanUp()
{
	//::WritePrivateProfileString(sectionName, keyName, doCloseTag?TEXT("1"):TEXT("0"), iniFilePath);
}

//
// Initialization of your plugin commands
// You should fill your plugins commands here
void commandMenuInit()
{
	//
	// Firstly we get the parameters from your plugin config file (if any)
	//

	// get path of plugin configuration
	::SendMessage(nppData._nppHandle, NPPM_GETPLUGINSCONFIGDIR, MAX_PATH, (LPARAM)iniFilePath);

	// if config path doesn't exist, we create it
	if (PathFileExists(iniFilePath) == FALSE)
	{
		::CreateDirectory(iniFilePath, NULL);
	}

	// make your plugin config file full file path name
	PathAppend(iniFilePath, configFileName);

	// get the parameter value from plugin config
	doCloseTag = (::GetPrivateProfileInt(sectionName, keyName, 0, iniFilePath) != 0);


    //--------------------------------------------//
    //-- STEP 3. CUSTOMIZE YOUR PLUGIN COMMANDS --//
    //--------------------------------------------//
    // with function :
    // setCommand(int index,                      // zero based number to indicate the order of command
    //            TCHAR *commandName,             // the command name that you want to see in plugin menu
    //            PFUNCPLUGINCMD functionPointer, // the symbol of function (function pointer) associated with this command. The body should be defined below. See Step 4.
    //            ShortcutKey *shortcut,          // optional. Define a shortcut to trigger this command
    //            bool check0nInit                // optional. Make this menu item be checked visually
    //            );
    setCommand(0, TEXT("新建项目"), new_project, NULL, false);
    setCommand(1, TEXT("打开项目"), open_new_project, NULL, false);
	//setCommand(3, TEXT("添加文件"), add_file, NULL, false);

	setCommand(2, TEXT("---"), NULL, NULL, false);
	setCommand(3, TEXT("编译项目"), make_project, NULL, false);
	setCommand(4, TEXT("清理项目"), make_clean_project, NULL, false);
	setCommand(5, TEXT("---"), NULL, NULL, false);

	setCommand(6, TEXT("---"), NULL, NULL, false);
	setCommand(7, TEXT("打开当前项目Makefile"), open_curr_project, NULL, false);
	setCommand(8, TEXT("打开当前项目main.c"), open_curr_main_c, NULL, false);
	setCommand(9, TEXT("查看编译记录"), open_log_information, NULL, false);
	setCommand(10, TEXT("---"), NULL, NULL, false);
	



	setCommand(11, TEXT("关于"), set_about, NULL, false);
	setCommand(11, TEXT("使用说明"), open_how_to_use, NULL, false);
	

	//setCommand(12, TEXT("Dockable Dialog Demo"), DockableDlgDemo, NULL, false);

}


//
// Here you can do the clean up (especially for the shortcut)
//
void commandMenuCleanUp()
{
	// Don't forget to deallocate your shortcut here
}

//----------------------------------------------//
//-- STEP 4. DEFINE YOUR ASSOCIATED FUNCTIONS --//
//----------------------------------------------//
#include <string>
#include "replace.h"
#include "text_encode.h"
#include "osdialog.h"
#include "dir_file.h"

std::string get_exe_file_full_name_utf8()
{
	std::wstring exe_file_name;
	exe_file_name = g_MODULE_FILE_PATH;
	std::string exe_utf8 = UnicodeToUtf8(exe_file_name);
	exe_utf8 = replace(exe_utf8, ".dll", ".exe");
	return exe_utf8;
}

std::string get_exe_file_dir_utf8()
{
	std::wstring exe_file_name;
	exe_file_name = g_MODULE_FILE_PATH;
	std::string exe_utf8 = UnicodeToUtf8(exe_file_name);
	exe_utf8 = replace(exe_utf8, "MZC8051.dll", "");
	exe_utf8 = replace(exe_utf8, "mzc8051.dll", "");
	return exe_utf8;
}

#include <iostream>
#include <fstream>
#include<cstdio>
#include <io.h>//access
std::string g_curr_project_path_utf8;



void new_project()
{
	char * file_name = osdialog_file(OSDIALOG_SAVE, "./", "proj1", NULL);
	std::string path1 = get_path(file_name);
	std::string proj1 = get_file_name(file_name);
	std::string exe_file = get_exe_file_full_name_utf8();
	std::wstring exe_file_uni = Utf8ToUnicode(exe_file);

	if (file_name != NULL)
	{


		std::string exe_cmd_utf8 = "\"" + exe_file + "\" " + " -x lmake -d -m create " + "   -i \"" + path1 + "\"" + " -o \"" + proj1 + "\" \n";
		std::wstring exe_cmd_uni = Utf8ToUnicode(exe_cmd_utf8);
		//ShellExecute(NULL, TEXT("open"), exe_file_uni.c_str(), exe_cmd_uni.c_str(), NULL, SW_MINIMIZE);
		//Sleep(1500);
		myexecute(exe_file, exe_cmd_utf8);

		g_curr_project_path_utf8 = file_name;
#if 1
		std::string make_file = path_add(file_name, "Makefile");
		std::string make_main = path_add(file_name, "main.c");
		::SendMessage(nppData._nppHandle, NPPM_DOOPEN, 0, (LPARAM)(utf8_to_wchar(make_file.c_str())));
		::SendMessage(nppData._nppHandle, NPPM_DOOPEN, 0, (LPARAM)(utf8_to_wchar(make_main.c_str())));
#endif
	}
}
void open_new_project()
{
	char * file_name = osdialog_file(OSDIALOG_OPEN, "./", "Makefile", NULL);
	std::string path1 = get_path(file_name);
	std::string proj1 = get_file_name(file_name);
	std::string exe_file = get_exe_file_full_name_utf8();
	std::wstring exe_file_uni = Utf8ToUnicode(exe_file);

	if (file_name != NULL)
	{
		g_curr_project_path_utf8 = path1;
#if 1
		std::string make_file = file_name;
		::SendMessage(nppData._nppHandle, NPPM_DOOPEN, 0, (LPARAM)(utf8_to_wchar(make_file.c_str())));
#endif
	}
}

void open_curr_project()
{
	std::string make_file = path_add(g_curr_project_path_utf8, "Makefile");

	::SendMessage(nppData._nppHandle, NPPM_DOOPEN, 0, (LPARAM)(utf8_to_wchar(make_file.c_str())));
}


void add_file()
{
	char * file_name = osdialog_file(OSDIALOG_OPEN, "./", ".c", NULL);
	std::string path1 = get_path(file_name);
	std::string proj1 = get_file_name(file_name);
	std::string exe_file = get_exe_file_full_name_utf8();
	std::wstring exe_file_uni = Utf8ToUnicode(exe_file);

	if (file_name != NULL)
	{
		g_curr_project_path_utf8 = path1;
#if 1
		std::string make_file = file_name;
		::SendMessage(nppData._nppHandle, NPPM_DOOPEN, 0, (LPARAM)(utf8_to_wchar(make_file.c_str())));
#endif
	}
}
void make_project()
{
	std::string exe_file = get_exe_file_full_name_utf8();
	std::wstring exe_file_uni = Utf8ToUnicode(exe_file);

	std::string exe_cmd_utf8 = "\"" + exe_file + "\" " + " -x lmake -d -m make " + "   -i \"" + g_curr_project_path_utf8 + "\"" ;
	std::wstring exe_cmd_uni = Utf8ToUnicode(exe_cmd_utf8);
	//ShellExecute(NULL, TEXT("open"), exe_file_uni.c_str(), exe_cmd_uni.c_str(), NULL, SW_NORMAL);
	//Sleep(1500);
	int exit_code = myexecute(exe_file, exe_cmd_utf8);

	if (exit_code == 0)
	{
		::MessageBoxA(NULL, "OK", "compile OK", MB_OK | MB_ICONINFORMATION);
	}
	else
	{
		::MessageBoxA(NULL,  "COMPILE ERROR. see more infomation in log", "ERROR", MB_OK | MB_ICONERROR);
	}


}


void make_clean_project()
{
	std::string exe_file = get_exe_file_full_name_utf8();
	std::wstring exe_file_uni = Utf8ToUnicode(exe_file);

	std::string exe_cmd_utf8 = "\"" + exe_file + "\" " + " -x lmake -d -m clean " + "   -i \"" + g_curr_project_path_utf8 + "\"";
	std::wstring exe_cmd_uni = Utf8ToUnicode(exe_cmd_utf8);
	myexecute(exe_file, exe_cmd_utf8);

	//ShellExecute(NULL, TEXT("open"), exe_file_uni.c_str(), exe_cmd_uni.c_str(), NULL, SW_NORMAL);
	//Sleep(1500);
}

void set_about()
{
	MessageBox(NULL, TEXT("MZC8051 做中国专业的8051单片机C语言编译器. V0.01"), TEXT("miliuz"), MB_ICONINFORMATION);
}
void open_log_information()
{
	//打开编译日志

	std::string make_file_log = path_add(g_curr_project_path_utf8, "lmake_make.tmp");

	::SendMessage(nppData._nppHandle, NPPM_DOOPEN, 0, (LPARAM)(utf8_to_wchar(make_file_log.c_str())));
}
void open_curr_main_c()
{
	std::string make_file_log = path_add(g_curr_project_path_utf8, "main.c");

	::SendMessage(nppData._nppHandle, NPPM_DOOPEN, 0, (LPARAM)(utf8_to_wchar(make_file_log.c_str())));
}
void open_how_to_use()
{
	std::string file_dir =  get_exe_file_dir_utf8();
	std::string make_file_log = path_add(file_dir, "how_to_use.txt");

	::SendMessage(nppData._nppHandle, NPPM_DOOPEN, 0, (LPARAM)(utf8_to_wchar(make_file_log.c_str())));
}

//
// This function help you to initialize your plugin commands
//
bool setCommand(size_t index, TCHAR *cmdName, PFUNCPLUGINCMD pFunc, ShortcutKey *sk, bool check0nInit) 
{
    if (index >= nbFunc)
        return false;

    if (!pFunc)
        return false;

    lstrcpy(funcItem[index]._itemName, cmdName);
    funcItem[index]._pFunc = pFunc;
    funcItem[index]._init2Check = check0nInit;
    funcItem[index]._pShKey = sk;

    return true;
}




// Dockable Dialog Demo
// 
// This demonstration shows you how to do a dockable dialog.
// You can create your own non dockable dialog - in this case you don't nedd this demonstration.
// You have to create your dialog by inherented DockingDlgInterface class in order to make your dialog dockable
// - please see DemoDlg.h and DemoDlg.cpp to have more informations.
void DockableDlgDemo()
{
	_goToLine.setParent(nppData._nppHandle);
	tTbData	data = { 0 };

	if (!_goToLine.isCreated())
	{
		_goToLine.create(&data);

		// define the default docking behaviour
		data.uMask = DWS_DF_CONT_BOTTOM;

		data.pszModuleName = _goToLine.getPluginFileName();

		// the dlgDlg should be the index of funcItem where the current function pointer is
		// in this case is DOCKABLE_DEMO_INDEX
		data.dlgID = DOCKABLE_DEMO_INDEX;
		::SendMessage(nppData._nppHandle, NPPM_DMMREGASDCKDLG, 0, (LPARAM)&data);
	}
	_goToLine.display();
}

