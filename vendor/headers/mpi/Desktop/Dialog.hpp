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

    class Dialog {
    public:
        static DialogUserAction MessageDialog(const MessageDialogDesc& dialogDesc);

    };
}