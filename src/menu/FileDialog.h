#include "../globals.h"
#include <FileDialog/tinyfiledialogs.h>

#include <thread>
#include <iostream>

namespace FileDialog
{
    // int aNumOfFilterPatterns, const char *const *aFilterPatterns, const char *aSingleFilterDescription, int aAllowMultipleSelects
    void openFileDialog(const char *Title, const char *DefaultPath, int NumOfFilterPatterns, const char *FilerPatterns, const char *FileDesc, int numOfSelectableFiles)
    {
        globals.OpenedFilePath = tinyfd_openFileDialog(Title, DefaultPath, NumOfFilterPatterns, &FilerPatterns, FileDesc, numOfSelectableFiles);
    }
}

// globals.OpenedFileName = tinyfd_openFileDialog(
//     "Select Your Secure Connect Zip File",
//     "../",
//     2,
//     globals.FileDialogFilterPatterns,
//     "text files",
//     1);

// if (!globals.OpenedFileName)
// {
//     tinyfd_messageBox(
//         "Error",
//         "Open file name is NULL",
//         "ok",
//         "error",
//         0);
// }