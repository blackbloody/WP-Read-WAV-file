#ifndef SIGNAL_READER_WINDOW_H
#define SIGNAL_READER_WINDOW_H

#include <iostream>
#include <gtkmm.h>

#include "sound_wave_sfml_opengl.h"
#include "basic_draw.h"

class MyListRow : public Gtk::ListBoxRow
{
public:
    
    SFMLWidget *ourRenderWindow;
    MovingCircle *moving_circle;
    
    //MyListRow(SFMLWidget vv) {
    MyListRow(std::vector<float> amp) {
        
        set_size_request (-1, 80);
        
        ourRenderWindow = new SFMLWidget(sf::VideoMode(1280, 720));
        moving_circle = new MovingCircle(*ourRenderWindow, amp);
        
        container.pack_start(*ourRenderWindow);
        add(container);
        show_all_children();
    }
protected:
    Gtk::VBox container;
    Gtk::Label label;
    Gtk::Button ourButton;
};

class SignalReaderWindow : public Gtk::Box {
public:
    SignalReaderWindow(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& refGlade) :
        Gtk::Box(obj),
        builder(refGlade) {
            builder->get_widget("box_list_signal", listViewSignal);
            
            std::vector<float> l1 = {-0.5f, 0.9f, 1.0f, 1.5f, 2.f};
            
            auto row = Gtk::manage(new MyListRow(l1));
            listViewSignal->append(*row);
            row->show();
    }

protected:
    Glib::RefPtr<Gtk::Builder> builder;
    
    Gtk::ListBox* listViewSignal;
};

#endif
