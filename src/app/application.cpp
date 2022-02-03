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

void Application::onFileStructure(WAV file) {
    std::cout << file.num_sample_per_second() << "\n";
}

void Application::onFileAllSampleToPlay(uint8_t* all_sample_sound, std::vector<int> offsetItem) {
    
    unsigned int pcm, tmp, rate;
    int dir;
	int channels, seconds;
	snd_pcm_t *handler;
	snd_pcm_hw_params_t *params;
	snd_pcm_uframes_t frames;
	int buff_size;
    size_t loops;

	rate = 11025;
	channels = 1;
	seconds = 3;
    
    /* Open PCM device for playback */
	if ((pcm = snd_pcm_open(&handler, PCM_DEVICE, SND_PCM_STREAM_PLAYBACK, 0))< 0)
		std::cout << "OPEN PLAYBACK ERROR: " << snd_strerror(pcm) << "\n";
	
	snd_pcm_hw_params_alloca(&params);
	snd_pcm_hw_params_any(handler, params);

	if (pcm = snd_pcm_hw_params_set_access(handler, params, SND_PCM_ACCESS_RW_INTERLEAVED) << 0)
		std::cout << "cannot set interleaved mode: " << snd_strerror(pcm) << "\n";
        
	if (pcm = snd_pcm_hw_params_set_format(handler, params, SND_PCM_FORMAT_S16_LE) < 0)
		std::cout << "cannot set format. " << snd_strerror(pcm) << "\n";
        
	if (pcm = snd_pcm_hw_params_set_channels(handler, params, channels) < 0) 
		std::cout << "cannot set channels number. " << snd_strerror(pcm) << "\n";
        
	if (pcm = snd_pcm_hw_params_set_rate_near(handler, params, &rate, &dir) < 0) 
		std::cout << "cannot set rate. " << snd_strerror(pcm) << "\n";
        
	if (pcm = snd_pcm_hw_params(handler, params) < 0)
		std::cout << "cannot set harware parameters. " << snd_strerror(pcm) << "\n";
    
    
    for (int i = 0; i < SND_PCM_STREAM_LAST; i++)
        std::cout << "Stream Types " << i <<": " << snd_pcm_stream_name(snd_pcm_stream_t(i)) << "\n";
    
	std::cout << "PCM name: " << snd_pcm_name(handler) << "\n";
	std::cout << "PCM state: " << snd_pcm_state_name(snd_pcm_state(handler)) << "\n";
	
	snd_pcm_hw_params_get_channels(params, &tmp);
	std::cout << "channels: " << tmp << "\n";
	
	if (tmp == 1)
		std::cout << "mono\n";
	else if (tmp == 2)
		std::cout << "streo\n";
        
    snd_pcm_hw_params_get_period_time(params, &pcm, &dir);
    std::cout << "Period time: " << pcm << "\n";
    loops = 3000000 / pcm;
    snd_pcm_hw_params_get_period_size(params, &frames, 0);
    //while (loops > 0) {loops--;}
    
    //uint8_t* sample_play;
    //memcpy(sample_play, all_sample_sound, 88200);
    
    while (loops > 0) 
    {
        loops--;
        
        pcm = snd_pcm_writei(handler, all_sample_sound, frames);
        if (pcm == -EPIPE) {
            // EPIPE means under-run
            snd_pcm_prepare(handler);
        }
        else if (pcm < 0) {
            std::cout << "cannot write to PCM device: " << snd_strerror(pcm) << "\n";
        }
        else if (pcm != (int)frames) {
            std::cout << "short write: " << pcm << "\n";
        }
        
        //delete sample_play;
    }
        
    snd_pcm_drain(handler);
    snd_pcm_close(handler);
    
    //delete [] sample_play;
    //delete all_sample_sound;

//	for (loops = (seconds * 1000000) / tmp; loops > 0; loops) {
//		if (pcm = snd_pcm_writei(handler, all_sample_sound, frames) == -EPIPE)
//			snd_pcm_prepare(handler);
//		else if (pcm < 0)
//			std::cout << "cannot write to PCM device: " << snd_strerror(pcm) << "\n";
//	}
    
    
    std::cout << "Played" << "\n";
    
}

void Application::onSampleAmpl(int* valueAmpl) {
    
}
