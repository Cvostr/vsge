#pragma once

#include <string>
#include <vector>

struct FileExtensionDesc{
    std::string extension;
    std::string description;

    FileExtensionDesc(const std::string& description, const std::string& extension) :
        extension(extension),
        description(description)
    {}
};

struct FileDialogDesc{
    std::string dialog_title;
    std::vector<FileExtensionDesc> extensions;

    std::string accept_btn_text;
    std::string cancel_btn_text;
};

#define ACCEPT_BUTTON 0x1
#define CANCEL_BUTTON 0x2

enum MessageDialogBtns{
    MESSAGE_DIALOG_BTN_OK,
    MESSAGE_DIALOG_BTN_OK_CANCEL,
    MESSAGE_DIALOG_BTN_YES_NO
};

struct MessageDialogDesc {
    std::string dialog_title;
    std::string message;

    int buttons;
};

void OpenFileDialog(FileDialogDesc* desc, std::string& result);
void SaveFileDialog(FileDialogDesc* desc, std::string& result);
void MessageDialog(MessageDialogDesc* desc, int& action);