#include "DialogWindows.hpp"

#ifdef __linux__
#include <gtk/gtk.h>
#endif

#ifdef _WIN32
#include <windows.h>
#undef CreateWindow
#include <Engine/Window.hpp>
#endif

void GTK_Init(){
    #ifdef __linux__
        if ( !gtk_init_check( NULL, NULL ) )
    {
        return;
    }
    #endif
}

void GTK_Wait(){
    #ifdef __linux__
        while (gtk_events_pending())
        gtk_main_iteration();
    #endif
}

#ifdef __linux__
void GTK_BindFileExtensionsToDialog(FileDialogDesc* desc, GtkWidget *dialog){

    for(auto& extension : desc->extensions){
        GtkFileFilter *filter = gtk_file_filter_new ();
        gtk_file_filter_set_name (filter, extension.description.c_str());
        gtk_file_filter_add_pattern (filter, extension.extension.c_str());
        gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (dialog), filter);
    }
}
#endif

void WIN32_CreateFileExtensions(FileDialogDesc* desc, std::string& out) {
    out = "";
    for (auto& extension : desc->extensions) {
        out += extension.description;
        out.push_back('\0');
        out += extension.extension;
        out.push_back('\0');
    }
}

void OpenFileDialog(FileDialogDesc* desc, std::string& result){

    std::string dialog_title = (!desc->dialog_title.empty()) ? desc->dialog_title : "Open File";
    std::string accept_btn_text = (!desc->accept_btn_text.empty()) ? desc->accept_btn_text : "Select";
    std::string cancel_btn_text = (!desc->cancel_btn_text.empty()) ? desc->cancel_btn_text : "Cancel";

#ifdef __linux__

    GTK_Init();

    GtkWidget *dialog = gtk_file_chooser_dialog_new (accept_btn_text.c_str(),
                                                    nullptr,
                                                    GTK_FILE_CHOOSER_ACTION_OPEN,
                                                    cancel_btn_text.c_str(), GTK_RESPONSE_CANCEL,
                                                    accept_btn_text.c_str(), GTK_RESPONSE_ACCEPT,
                                                    NULL);

    GTK_BindFileExtensionsToDialog(desc, dialog);                                          

    if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
    {
        char *filename;

        filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
        result = std::string(filename);
        g_free (filename);
    }

    gtk_widget_destroy (dialog);

    GTK_Wait();
#endif

#ifdef _WIN32
    VSGE::Window* win = VSGE::Window::Get();
    OPENFILENAMEA ofn;
    CHAR szFile[260] = { 0 };
    ZeroMemory(&ofn, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = (HWND)win->GetHWND();
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);

    std::string filter;
    WIN32_CreateFileExtensions(desc, filter);

    ofn.lpstrFilter = filter.c_str();
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    if (GetOpenFileNameA(&ofn) == TRUE)
        result = ofn.lpstrFile;
#endif
}

void SaveFileDialog(FileDialogDesc* desc, std::string& result){

    std::string dialog_title = (!desc->dialog_title.empty()) ? desc->dialog_title : "Save File";
    std::string accept_btn_text = (!desc->accept_btn_text.empty()) ? desc->accept_btn_text : "Save";
    std::string cancel_btn_text = (!desc->cancel_btn_text.empty()) ? desc->cancel_btn_text : "Cancel";
#ifdef __linux__

    GTK_Init();

    GtkWidget *dialog = gtk_file_chooser_dialog_new (accept_btn_text.c_str(),
                                                    nullptr,
                                                    GTK_FILE_CHOOSER_ACTION_SAVE,
                                                    cancel_btn_text.c_str(), GTK_RESPONSE_CANCEL,
                                                    accept_btn_text.c_str(), GTK_RESPONSE_ACCEPT,
                                                    NULL);

    GTK_BindFileExtensionsToDialog(desc, dialog);                                                

    if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
    {
        char *filename;

        filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
        result = std::string(filename);
        g_free (filename);
    }

    gtk_widget_destroy (dialog);

    GTK_Wait();
#endif

#ifdef _WIN32
    VSGE::Window* win = VSGE::Window::Get();
    OPENFILENAMEA ofn;
    CHAR szFile[260] = { 0 };
    ZeroMemory(&ofn, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = (HWND)win->GetHWND();
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);

    std::string filter;
    WIN32_CreateFileExtensions(desc, filter);

    ofn.lpstrFilter = filter.c_str();
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    ofn.lpstrDefExt = strchr(filter.c_str(), '\0') + 1;

    if (GetSaveFileNameA(&ofn) == TRUE)
        result = ofn.lpstrFile;
#endif
}

void MessageDialog(MessageDialogDesc* desc, DialogUserAction& action){
    action = DIALOG_USER_ACTION_CANCEL;

    #ifdef __linux__

    GTK_Init();

    GtkButtonsType btype = GTK_BUTTONS_OK;

    switch(desc->buttons){
        case MESSAGE_DIALOG_BTN_OK:
            btype = GTK_BUTTONS_OK;
            break;
        case MESSAGE_DIALOG_BTN_OK_CANCEL:
            btype = GTK_BUTTONS_OK_CANCEL;
            break;
        case MESSAGE_DIALOG_BTN_YES_NO:
            btype = GTK_BUTTONS_YES_NO;
            break;
    }

    GtkMessageType mtype = GTK_MESSAGE_INFO;

    switch(desc->dialog_type){
        case MESSAGE_DIALOG_TYPE_INFO:
            mtype = GTK_MESSAGE_INFO;
            break;
        case MESSAGE_DIALOG_TYPE_WARNING:
            mtype = GTK_MESSAGE_WARNING;
            break;
        case MESSAGE_DIALOG_TYPE_QUESTION:
            mtype = GTK_MESSAGE_QUESTION;
            break;
        case MESSAGE_DIALOG_TYPE_ERROR:
            mtype = GTK_MESSAGE_ERROR;
            break;
    }

    GtkWidget* dialog = gtk_message_dialog_new (nullptr,
                                  GTK_DIALOG_MODAL,
                                  mtype,
                                  btype,
                                  desc->dialog_title.c_str()
                                  );
    GtkWidget* text_edit = nullptr;

    if(desc->has_text_field){
        GtkContainer* vbox = GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(dialog)));
        text_edit = gtk_entry_new();
        gtk_entry_set_text((GtkEntry*)text_edit, desc->text_to_edit.c_str());
        gtk_container_add (vbox, text_edit);
    }

    gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG (dialog), desc->message.c_str());

    gtk_widget_show_all((dialog));

    gint result = gtk_dialog_run (GTK_DIALOG (dialog));

    if (result == GTK_RESPONSE_YES || result == GTK_RESPONSE_OK)
    {
        action = DIALOG_USER_ACTION_ACCEPT;
    }else if(result == GTK_RESPONSE_NO || result == GTK_RESPONSE_CANCEL)
    {
        action = DIALOG_USER_ACTION_CANCEL;
    }
   
    if(desc->has_text_field){
        desc->text_to_edit = std::string( gtk_entry_get_text((GtkEntry*)text_edit));
    }

    gtk_widget_destroy (dialog);
    GTK_Wait();
#endif

#ifdef _WIN32
    VSGE::Window* win = VSGE::Window::Get();
    uint32 btype = MB_OK;

    switch (desc->buttons) {
    case MESSAGE_DIALOG_BTN_OK:
        btype = MB_OK;
        break;
    case MESSAGE_DIALOG_BTN_OK_CANCEL:
        btype = MB_OKCANCEL;
        break;
    case MESSAGE_DIALOG_BTN_YES_NO:
        btype = MB_YESNO;
        break;
    }

    LPWSTR title = new WCHAR[desc->dialog_title.size() + 1];
    title[desc->dialog_title.size()] = '\0';
    mbstowcs((wchar_t*)title, desc->dialog_title.c_str(), desc->dialog_title.size());

    LPWSTR message = new WCHAR[desc->message.size() + 1];
    message[desc->message.size()] = '\0';
    mbstowcs((wchar_t*)message, desc->message.c_str(), desc->message.size());

    int result = MessageBoxW((HWND)win->GetHWND(), message, title, btype);

    if (result == IDOK || result == IDYES)
        action = ACCEPT_BUTTON;
    if (result == IDNO || result == IDCANCEL)
        action = CANCEL_BUTTON;
#endif
}