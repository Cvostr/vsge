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

    class DialogFileFilter {
    private:
        std::string pattern;
        std::string description;
    public:

        const std::string& getDescription() const;

        const std::string& getPattern() const;

        DialogFileFilter();

        DialogFileFilter(const std::string& description, const std::string& pattern);
    };


    class FileDialogDesc {
    private:
        std::vector<DialogFileFilter> filters;
    public:
        std::string dialogTitle;
        std::string baseFileName;
        std::string initialDir;

        const std::vector<DialogFileFilter>& getFileFilters() const;

        void addFileFilter(const DialogFileFilter& filter);
        void addFileFilters(const std::vector<DialogFileFilter>& filters);
    };

    class Dialog {
    public:
        static DialogUserAction MessageDialog(const MessageDialogDesc& dialogDesc);

        static std::string OpenFileDialog(const FileDialogDesc& desc);
        static std::string SaveFileDialog(const FileDialogDesc& desc);
    };
}