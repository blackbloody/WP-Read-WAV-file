#include "application.h"

Application::Application(BaseObjectType* obj, Glib::RefPtr<Gtk::Builder> const& builder) : 
Gtk::ApplicationWindow(obj), builder{builder}, readerWav()
{
    set_position(Gtk::WIN_POS_CENTER);
    
    builder->get_widget("file_struct", fileStruct);
    builder->get_widget("new_menu", newMenu);
    builder->get_widget("quit_menu", quitMenu);
    builder->get_widget("tv_log", tvLog);
    builder->get_widget("tab_view", mainNotebook);
    
    newMenu->signal_activate().connect(sigc::mem_fun(*this, &Application::getNewFile));
    
    refTreeModel = Gtk::TreeStore::create(mCol);
	fileStruct->set_model(refTreeModel);
	fileStruct->append_column("Name", mCol.colName);
}

void Application::getNewFile() {
    
    // Create the dialog box FileChooser
    Gtk::FileChooserDialog dialog("Please choose a file",Gtk::FILE_CHOOSER_ACTION_OPEN);
    dialog.set_transient_for(*this);
    dialog.set_select_multiple(false);
    
    //Add response buttons the the dialog:
    dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
    dialog.add_button("_Open", Gtk::RESPONSE_OK);
    
    Glib::RefPtr<Gtk::FileFilter> filter_text = Gtk::FileFilter::create();
    filter_text->set_name("Text files");
    filter_text->add_mime_type("text/plain");
    
    Glib::RefPtr<Gtk::FileFilter> filter_vid = Gtk::FileFilter::create();
    filter_vid->set_name("Video");
    filter_vid->add_mime_type("video/*");
    
    Glib::RefPtr<Gtk::FileFilter> filter_img = Gtk::FileFilter::create();
    filter_img->set_name("Image");
	filter_img->add_mime_type("image/*");
    
    Glib::RefPtr<Gtk::FileFilter> filter_audio = Gtk::FileFilter::create();
    filter_audio->set_name("Audio");
	filter_audio->add_mime_type("audio/wav");
    
    //dialog.add_filter(filter_text);
    //dialog.add_filter(filter_vid);
    //dialog.add_filter(filter_img);
    dialog.add_filter(filter_audio);
    
    //Show the dialog and wait for a user response:
    int result = dialog.run();
    
    switch(result)
    {
        case(Gtk::RESPONSE_OK):
        {
            refTreeModel->clear();
            
            Glib::RefPtr<Gio::File> ffile = dialog.get_file();
            std::string file_path = ffile->get_path();
            
            mainThread = new std::thread([this, file_path] 
            {
                readerWav.onReadFileStructure(this, file_path);
            });
            
            break;
            }
        case(Gtk::RESPONSE_CANCEL):
        default:
        {
            std::cout << "Cancel File" << std::endl;
            break;
            }
    }
    
}

void Application::onFileStructure(std::string item_name) {
    std::cout << item_name << "\n";
}
