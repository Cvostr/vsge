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

void GTK_BindFileExtensionsToDialog(FileDialogDesc* desc, GtkWidget *dialog){
#ifdef __linux__
    for(auto& extension : desc->extensions){
        GtkFileFilter *filter = gtk_file_filter_new ();
        gtk_file_filter_set_name (filter, extension.description.c_str());
        gtk_file_filter_add_pattern (filter, extension.extension.c_str());
        gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (dialog), filter);
    }
#endif
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
}

void MessageDialog(MessageDialogDesc* desc, int& action){
    action = 0;

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

    GtkWidget* dialog = gtk_message_dialog_new (nullptr,
                                  GTK_DIALOG_DESTROY_WITH_PARENT,
                                  GTK_MESSAGE_ERROR,
                                  btype,
                                  desc->message.c_str()
                                  );

    gint result = gtk_dialog_run (GTK_DIALOG (dialog));

    if (result == -8)
    {
        action = ACCEPT_BUTTON;
    }else
    {
        action = CANCEL_BUTTON;
    }
   
    gtk_widget_destroy (dialog);
    GTK_Wait();
#endif
}