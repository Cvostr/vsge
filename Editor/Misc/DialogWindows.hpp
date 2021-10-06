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
    std::string base_file_name;

    std::string accept_btn_text;
    std::string cancel_btn_text;
};

enum DialogUserAction{
    DIALOG_USER_ACTION_ACCEPT,
    DIALOG_USER_ACTION_CANCEL
};

enum MessageDialogBtns{
    MESSAGE_DIALOG_BTN_OK,
    MESSAGE_DIALOG_BTN_OK_CANCEL,
    MESSAGE_DIALOG_BTN_YES_NO
};


enum MessageDialogType{
    MESSAGE_DIALOG_TYPE_INFO,
    MESSAGE_DIALOG_TYPE_WARNING,
    MESSAGE_DIALOG_TYPE_QUESTION,
    MESSAGE_DIALOG_TYPE_ERROR
};

struct MessageDialogDesc {
    std::string dialog_title;
    std::string message;

    int buttons;
    MessageDialogType dialog_type;

    bool has_text_field;
    std::string text_to_edit;
};

void OpenFileDialog(FileDialogDesc* desc, std::string& result);
void SaveFileDialog(FileDialogDesc* desc, std::string& result);
void MessageDialog(MessageDialogDesc* desc, DialogUserAction& action);