#pragma once

#include <gtkmm.h>
#include <iostream>

#include <chrono>
#include <thread>

#include "reader_wav.h"

class Application : public Gtk::ApplicationWindow, public IReaderWav {
public:
    Application(BaseObjectType* obj, Glib::RefPtr<Gtk::Builder> const& builder);
    virtual ~Application() = default;
    
    // virtual function wav
    void onFileStructure(std::string ite_name);
    
    
private:
    Glib::Dispatcher mDispatcherTreeView;
    
    //Gtk::Paned* panedTreeView;
    Gtk::TreeView* fileStruct;
    
    Gtk::ImageMenuItem* newMenu;
    Gtk::ImageMenuItem* quitMenu;
    Gtk::TextView* tvLog;
    
    Gtk::Notebook* mainNotebook;
    
    std::thread* mainThread;
    ReaderWav readerWav;
    
protected:
    Glib::RefPtr<Gtk::Builder> builder;
    void getNewFile();
    
    class ModelFileColumns : public Gtk::TreeModel::ColumnRecord
    {
        public:
            ModelFileColumns() 
        {
            add(colID); 
            add(colName);
            //add(colFileStruct);
        }
        
        Gtk::TreeModelColumn<int> colID;
        Gtk::TreeModelColumn<Glib::ustring> colName;
        //Gtk::TreeModelColumn<MdlFile> colFileStruct;
    };
    ModelFileColumns mCol;
    Gtk::TreeModel::iterator mainIterateTree;
    Glib::RefPtr<Gtk::TreeStore> refTreeModel;
};
//Application myApplication;
