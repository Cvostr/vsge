#define _CRT_SECURE_NO_WARNINGS
#include "DialogWindows.hpp"

#ifdef __linux__
#include <gtk/gtk.h>
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

void OpenFileDialog(FileDialogDesc* desc, std::string& result){
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
}

void SaveFileDialog(FileDialogDesc* desc, std::string& result){

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
}

void OpenFolderDialog(std::string& result) {
#ifdef _WIN32

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
                                "%s",
                                desc->dialog_title.c_str()
                                );
    GtkWidget* text_edit = nullptr;

    if(desc->has_text_field){
        GtkContainer* vbox = GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(dialog)));
        text_edit = gtk_entry_new();
        gtk_entry_set_text((GtkEntry*)text_edit, desc->text_to_edit.c_str());
        gtk_container_add (vbox, text_edit);
    }

    gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG (dialog), "%s", desc->message.c_str());

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
}