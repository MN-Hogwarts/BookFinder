#pragma once
#include <iostream>
#include <shobjidl.h> 
#include <ShlObj.h>
#include <shtypes.h>
#include <fstream>
#include <sstream>
#include <limits>
#include <stdexcept>
#include <Windows.h>
//#include "opencv2/core.hpp"
//#include "opencv2/imgproc.hpp"

using namespace std;
//using namespace cv;

PWSTR openFileDialog() {
	PWSTR filename = L"null1";
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
		COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr))
	{
		IFileOpenDialog *pFileOpen;

		// Create the FileOpenDialog object.
		hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
			IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

		if (SUCCEEDED(hr))
		{
			hr = pFileOpen->SetOptions(FOS_FORCEFILESYSTEM | FOS_PATHMUSTEXIST | FOS_FILEMUSTEXIST);
			if (SUCCEEDED(hr))
			{
				// Set the file types to display only. 
				// Notice that this is a 1-based array.
				COMDLG_FILTERSPEC rgSpec[] =
				{
					//{ L"Text Files", L"*.txt" },
					{ L"All Files", L"*.*" },
				};
				hr = pFileOpen->SetFileTypes(ARRAYSIZE(rgSpec), rgSpec);
				if (SUCCEEDED(hr))
				{

					// Set the default extension to be ".txt" file.
					pFileOpen->SetTitle(L"Open file...");
					hr = pFileOpen->SetDefaultExtension(L"*.*");
					if (SUCCEEDED(hr))
					{
						// Show the Open dialog box.
						hr = pFileOpen->Show(NULL);

						// Get the file name from the dialog box.
						if (SUCCEEDED(hr))
						{
							IShellItem *pItem;
							hr = pFileOpen->GetResult(&pItem);
							if (SUCCEEDED(hr))
							{
								LPWSTR pszFilePath;
								hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

								// Display the file name to the user.
								if (SUCCEEDED(hr))
								{
									filename = pszFilePath;
									CoTaskMemFree(pszFilePath);
								}
								pItem->Release();
							}
						}
						else if (hr == HRESULT_FROM_WIN32(ERROR_CANCELLED)) {
							filename = L"null";
						}
						pFileOpen->Release();
					}
				}
			}
		}
		CoUninitialize();
	}
	if (filename == L"null1")
		filename = L"null";
	return filename;
}

string choosefile() {
	PWSTR oFilename = openFileDialog();
	if (oFilename == L"null")
		return "null";
	else {
		wstring filename = oFilename;
		string parsedFilename(filename.begin(), filename.end());
		return parsedFilename;
	}
}

bool check1(string filename, string ext) {
	if (filename.find(ext, filename.length() - ext.length()) != string::npos) {
		return true;
	}
	return false;

}


string check() {
	string name = choosefile();
	if (check1(name, "jpg") || check1(name, "jpeg") || check1(name, "jpe") || check1(name, "bmp") || check1(name, "dib") || check1(name, "jp2") || check1(name, "png") || check1(name, "webp") || check1(name, "pbm") || check1(name, "pgm") || check1(name, "ppm") || check1(name, "sr") || check1(name, "ras") || check1(name, "tiff") || check1(name, "tif")) {
		return name;
	}

	else {
		return "ERROR";
	}
}



string input() {
	cv::Mat instructions;
	instructions = cv::imread("Instructions.jpg");
	cv::imshow("Instructions", instructions);
	cv::waitKey(0);
	cv::destroyWindow("Instructions");
	string name = check();
	do {
		if (name != "ERROR")
			return name;
		else {
			name = check();
		}
	} while (name == "ERROR");
		
}