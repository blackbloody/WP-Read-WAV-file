#pragma once

#include "signal_reader.h"

#include <gtkmm.h>
#include <iostream>
#include <string.h>
#include <cstdlib>

#include <chrono>
#include <thread>

#include "reader_wav.h"

#include </usr/include/alsa/asoundlib.h>

#include <iomanip>
#include <sstream>
#include <fstream> 
// for memory mapping
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>

#define PCM_DEVICE "default"

class Application : public Gtk::ApplicationWindow, public IReaderWav {
public:
    Application(BaseObjectType* obj, Glib::RefPtr<Gtk::Builder> const& builder);
    virtual ~Application() = default;
    
    // virtual function wav
    void onFileStructure(WAV file);
    void onFileAllSampleToPlay(uint8_t* all_sample_sound, std::vector<int> offsetItem);
    void onSampleAmpl(int* valueAmpl);
    
    SignalReaderWindow* signalReaderWindow = nullptr;
    
private:
    Glib::Dispatcher mDispatcherTreeView;
    
    //Gtk::Paned* panedTreeView;
    Gtk::TreeView* fileStruct;
    
    Gtk::ImageMenuItem* newMenu;
    Gtk::ImageMenuItem* quitMenu;
    Gtk::TextView* tvLog;
    
    Gtk::Notebook* mainNotebook;
    Gtk::Button* btnDemo1; 
    Gtk::Button* btnRandomSample;
    
    std::thread* mainThread;
    ReaderWav readerWav;
    
protected:
    Glib::RefPtr<Gtk::Builder> builder;
    void getNewFile();
    void demosound();
    void sampleSound();
    
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
