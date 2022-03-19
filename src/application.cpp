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
    
    builder->get_widget("btn_demo1", btnDemo1);
    builder->get_widget("btn_random_sample", btnRandomSample);
    
    newMenu->signal_activate().connect(sigc::mem_fun(*this, &Application::getNewFile));
    btnDemo1->signal_clicked().connect(sigc::mem_fun(*this, &Application::demosound));
    btnRandomSample->signal_clicked().connect(sigc::mem_fun(*this, &Application::sampleSound));
    
    refTreeModel = Gtk::TreeStore::create(mCol);
    fileStruct->set_model(refTreeModel);
    fileStruct->append_column("Name", mCol.colName);
    
    auto refBuilderGL = Gtk::Builder::create();
    refBuilderGL->add_from_file("../src/resources/signal_area.glade");
    
    signalReaderWindow = nullptr;
    refBuilderGL->get_widget_derived("box_signal_area", signalReaderWindow);
    
    Gtk::Image* ivTest = new Gtk::Image(Gtk::Stock::CLOSE, Gtk::ICON_SIZE_MENU);
    Gtk::Button* btnTest = new Gtk::Button();
    Gtk::Label* lblTest = new Gtk::Label();
    
    btnTest->set_focus_on_click(false);
    btnTest->set_image(*ivTest);
    btnTest->set_tooltip_text("Close");
    btnTest->set_relief(Gtk::ReliefStyle::RELIEF_NONE );
    btnTest->signal_clicked().connect([=]()
    {
        mainNotebook->remove_page(*signalReaderWindow);
        signalReaderWindow = nullptr;
    });
    
    lblTest->set_text("Signal Area");
    lblTest->show();
    btnTest->show();
    
    Gtk::HBox* hbox = new Gtk::HBox(false, 0);
    hbox->add(*lblTest);
    hbox->add(*btnTest);
    hbox->show();
    
    mainNotebook->append_page(*signalReaderWindow, *hbox);
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

void Application::demosound() {
    
    mainThread = new std::thread([this] {
        readerWav.onDemo(this);
    });
    
//    mainThread = new std::thread([this] 
//    {
//        struct stat fileInfo;
//        uint8_t* byteBuff;
//        //std::lock_guard<std::mutex> lock(mMutex);
//        std::string file_name = "/home/venom/Downloads/Doremifasolasido.wav";
//        const char* fileName = file_name.c_str();
//        int fd = open(fileName, O_RDONLY);
//        posix_fadvise(fd, 0, 0, 1);
//        fstat(fd, &fileInfo);
//        byteBuff = static_cast<uint8_t*>(mmap(NULL, fileInfo.st_size, PROT_READ, MAP_SHARED, fd, 0));
//        
//        unsigned int pcm, tmp, rate;
//        int channels, seconds;
//        snd_pcm_t *handler;
//        snd_pcm_hw_params_t *params;
//        snd_pcm_uframes_t frames;
//        int buff_size, loops, dir;
//        
//        //rate = 44100;
//        rate = 11025;
//        channels = 1;
//        seconds = 3;
//        
//        if ((pcm = snd_pcm_open(&handler, PCM_DEVICE, SND_PCM_STREAM_PLAYBACK, 0))< 0) {
//            std::cout << "PLAYBACK OPEN ERROR: " << snd_strerror(pcm) << "\n";
//            return;
//        }
//        
//        if ((pcm = snd_pcm_hw_params_malloc(&params)) < 0) 
//            std::cout << "cannot allocate hardware parameter structure: " << snd_strerror(pcm) << "\n";
//        if ((pcm = snd_pcm_hw_params_any(handler, params)) < 0)
//            std::cout << "cannot initialize hardware parameter structure: " << snd_strerror(pcm) << "\n";
//        
//        if (pcm = snd_pcm_hw_params_set_access(handler, params, SND_PCM_ACCESS_RW_INTERLEAVED) < 0)
//            std::cout << "cannot set interleaved mode: " << snd_strerror(pcm) << "\n";
//        if (pcm = snd_pcm_hw_params_set_format(handler, params, SND_PCM_FORMAT_S16_LE) < 0)
//            std::cout << "Can't set format. " << snd_strerror(pcm) << "\n";
//        if (pcm = snd_pcm_hw_params_set_channels(handler, params, channels) < 0) 
//            std::cout << "Can't set channels number. " << snd_strerror(pcm) << "\n";
//        if (pcm = snd_pcm_hw_params_set_rate_near(handler, params, &rate, 0) < 0) 
//            std::cout << "Can't set rate. " << snd_strerror(pcm) << "\n";
//            
//        if (pcm = snd_pcm_hw_params(handler, params) < 0)
//            std::cout << "Can't set harware parameters. " << snd_strerror(pcm) << "\n";
//            
//        // free device from all parameter
//        snd_pcm_hw_params_free(params);
//        //std::cout << "PCM name: " << snd_pcm_name(handler) << "\n";
//        //std::cout << "PCM state: " << snd_pcm_state_name(snd_pcm_state(handler)) << "\n";
//        
//        snd_pcm_hw_params_get_channels(params, &tmp);
//        //std::cout << "channels: " << tmp << "\n";
//        
//        if (tmp == 1)
//            std::cout << "mono\n";
//        else if (tmp == 2)
//            std::cout << "streo\n";
//
//        snd_pcm_hw_params_get_period_time(params, &pcm, 0);
//        std::cout << "Period time: " << pcm << "\n";
//        long microSec = seconds * 1000000;
//        loops = microSec / pcm;
//        std::cout << "LOOPS: " << loops << "\n";
//        
//        
//        int index_offset[4] = { 92, 22142, 44192, 66242 };
//        
//        int index = 0;
//        snd_pcm_hw_params_get_period_size(params, &frames, 0);
//        int secPass = seconds + 1;
//        while (secPass > 0) {
//            //loops--;
//            secPass--;
//            
//            if ((pcm = snd_pcm_wait(handler, 1000)) < 0) {
//                std::cout << "poll failed" << "\n";
//                break;
//            }
//            
//            uint8_t* sample = new uint8_t[88200];
//            memcpy(sample, byteBuff + index_offset[index++], 88200);
//            
//            if (pcm = snd_pcm_writei(handler, sample, frames) == -EPIPE)
//                snd_pcm_prepare(handler);
//            else if (pcm < 0)
//                std::cout << "cannot write to PCM device: " << snd_strerror(pcm) << "\n";
//        }
//            
//        snd_pcm_drain(handler);
//        snd_pcm_close(handler);
//        
//        if (munmap(byteBuff, fileInfo.st_size) == -1)
//        {
//            //close(fd);
//        }
//        //close(fd);
//    });
}

void Application::sampleSound() {
    mainThread = new std::thread([this] {
        readerWav.onSample();
    });
}

void Application::onFileStructure(WAV file) {
    std::cout << "ASDCF: " << file.num_sample() << "\n";
}

void Application::onFileAllSampleToPlay(uint8_t* all_sample_sound, std::vector<int> offsetItem) {
    
}

void Application::onSampleAmpl(int* valueAmpl) {
    
}
