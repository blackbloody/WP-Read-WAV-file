#include <SFML/OpenGL.hpp>
#include <random>

class MovingCircle
{
public:

    // a reference to our SFMLWidget
    SFMLWidget& widget;
    
    std::vector<float> ampp;
    
    MovingCircle(SFMLWidget& widget, std::vector<float> amp) : widget(widget)
    {
        ampp = amp;
        
        widget.signal_draw().connect(sigc::bind_return(
                                     sigc::hide(
                                         sigc::mem_fun(this, &MovingCircle::draw)),
                                     true));
                                     
        widget.signal_size_allocate().connect(sigc::hide(
                                                    sigc::mem_fun(this, &MovingCircle::resize_view)));
    }
    
    void animate()
    {
        widget.invalidate();
    }
    
    void draw()
    {
        glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
//        float x = 0;
//        float y = 0;
//        for (int i = 0; i < 5; i++) {
//            glPointSize(1.9f);
//            glBegin(GL_POINTS);
//            glColor3f(1.0f, 0.0f, 0.0f);
//            y += 0.06;
//            x += 0.05f; 
//            glVertex2f(x, y);
//            glEnd();      
//        }
        
        /*
        drawPoint(0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
        drawPoint(1.0f, 0.0f, 1.0f, 1.0f, 0.0f);
        
        glBegin( GL_LINES);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(1.0f, 0.0f);
        glEnd();
        //////////////////////////////
        drawPoint(0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
        drawPoint(-1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
        
        glBegin( GL_LINES);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(-1.0f, 0.0f);
        glEnd();
        */
        
        drawPoint(
        randd(), randd(), 
        0.0f, 0.0f, 1.0f);
        
        drawPointAndLine(
        0.0f, 0.0f, 
        2.0f, 0.0f, 
        randd(), 0.0f, 0.0f);
        
        drawPointAndLine(
        -1.0f, 0.0f, 
        0.0f, 0.0f, 
        0.0f, 1.0f, 0.0f);
        
        glFlush();
        
        widget.display();
    }
    
    float randd() {
        return (float)rand() / ((float)RAND_MAX + 1);
    }

    
    void drawPoint(float x, float y, float r, float g, float b) {
        glPointSize(5.1f);
        glBegin(GL_POINTS);
        glColor3f(r, g, b);
        glVertex2f(x, y);
        glEnd();
    }
    
    void drawPointAndLine(float x1, float y1, float x2, float y2, float r, float g, float b) {
        
        glPointSize(0.1f);
        glBegin(GL_POINTS);
        glColor3f(r, g, b);
        glVertex2f(x1, y1);
        glEnd();
        
        glPointSize(0.1f);
        glBegin(GL_POINTS);
        glColor3f(r, g, b);
        glVertex2f(x2, y2);
        glEnd();
        
        glBegin( GL_LINES);
        glVertex2f(x1, y1);
        glVertex2f(x2, y2);
        glEnd();
        
    }
    
    void resize_view()
    {
        // Let the View fit the pixels of the window.
        sf::Vector2f lower_right(widget.renderWindow.getSize().x,
                                 widget.renderWindow.getSize().y);
    
        sf::View view(lower_right * 0.5f,
                      lower_right);
        widget.renderWindow.setView(view);
    }
};