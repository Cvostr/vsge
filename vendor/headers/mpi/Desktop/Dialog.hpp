#pragma once

#include <string>
#include <vector>

namespace Mpi {
    enum DialogUserAction {
        DIALOG_USER_ACTION_ACCEPT,
        DIALOG_USER_ACTION_CANCEL
    };

    enum MessageDialogBtns {
        MESSAGE_DIALOG_BTN_OK,
        MESSAGE_DIALOG_BTN_OK_CANCEL,
        MESSAGE_DIALOG_BTN_YES_NO
    };

    enum MessageDialogType {
        MESSAGE_DIALOG_TYPE_INFO,
        MESSAGE_DIALOG_TYPE_WARNING,
        MESSAGE_DIALOG_TYPE_QUESTION,
        MESSAGE_DIALOG_TYPE_ERROR
    };

    struct MessageDialogDesc {
        std::string dialogTitle;
        std::string dialogMessage;

        MessageDialogBtns buttons;
        MessageDialogType dialogType;

        MessageDialogDesc() {
            buttons = MESSAGE_DIALOG_BTN_OK;
            dialogType = MESSAGE_DIALOG_TYPE_INFO;
        }
    };

    struct FileExtensionDesc {
        std::string extension;
        std::string description;

        FileExtensionDesc() :
            extension("*.*"),
            description("All files")
        {}
        FileExtensionDesc(const std::string& description, const std::string& extension) :
            extension(extension),
            description(description)
        {}
    };


    struct FileDialogDesc {
        std::string dialogTitle;
        std::vector<FileExtensionDesc> extensions;
        std::string baseFileName;
        std::string initialDir;
    };

    class Dialog {
    public:
        static DialogUserAction MessageDialog(const MessageDialogDesc& dialogDesc);

        static std::string OpenFileDialog(const FileDialogDesc& desc);
        static std::string SaveFileDialog(const FileDialogDesc& desc);
    };
}