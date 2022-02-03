#pragma once

#include <gtkmm.h>
#include <iostream>
#include <string.h>
#include <cstdlib>

#include <chrono>
#include <thread>

#include "reader_wav.h"

#include </usr/include/alsa/asoundlib.h>

#define PCM_DEVICE "default"

class Application : public Gtk::ApplicationWindow, public IReaderWav {
public:
    Application(BaseObjectType* obj, Glib::RefPtr<Gtk::Builder> const& builder);
    virtual ~Application() = default;
    
    // virtual function wav
    void onFileStructure(WAV file);
    void onFileAllSampleToPlay(uint8_t* all_sample_sound, std::vector<int> offsetItem);
    void onSampleAmpl(int* valueAmpl);
    
    
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
