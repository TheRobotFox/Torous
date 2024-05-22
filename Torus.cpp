#include "Conscreen/Conscreen.h"
#include "Conscreen/Conscreen_ANSI.h"
#include "Conscreen/Conscreen_console.h"
#include "Conscreen/Conscreen_screen.h"
#include "Matrix.hpp"
#include "Vec.hpp"
#include <iostream>

#define PI 3.14159265358979
using namespace anli;


class Torous
{

    public:
    Vec3 m_pos;
    double m_rotX, m_rotY,
        m_holeRadius, m_tubeRadius,
        angle=0.01;

    static std::vector<Vec<3>> getCircle(Vec<3> center, double radius, std::size_t a, double angle)
    {
        std::vector<Vec<3>> circle;
        for(double time=0; time<2.0*PI; time+=angle){

            Matrix rot= Torous::getRotTransform(a, time);
            Vec<3> v= {0,0,0};
            v.get(a+1)=radius;
            circle.push_back(center+v.transform(rot));
        }
        return circle;
    }
        Torous(Vec<3> pos, double holeRadius, double tubeRadius, double rotX, double rotY)
            : m_pos(pos), m_rotX(rotX), m_rotY(rotY), m_holeRadius(holeRadius), m_tubeRadius(tubeRadius)
        {}

        std::vector<Vec<3>> getPoints()
        {
            Vec<3> first_pos = m_pos;
            first_pos.get(X)+=m_holeRadius;
            std::vector<Vec<3>> circle = getCircle(first_pos, m_tubeRadius, Y, angle);
            std::vector<Vec<3>> torous;
            for(double time=0; time < 2.0*PI; time+=angle){
                std::ranges::copy(circle
                                | std::ranges::views::transform([time](Vec<3> p){return p.transform(getRotTransform(Z,time));}),
                                std::back_inserter(torous));
            }
            return torous;
        }
};

class Camera{
    Vec<2> project(Vec<3> point){
        Matrix<3,2> proj = {
            {0},
            {1,0},
            {0,1}
        };
        return proj*point*m_focalLength/get<0>(point);
    }
    public:
    Vec<3> m_position;
    double m_focalLength;
        Camera(Vec<3> pos, double f)
            : m_position(pos), m_focalLength(f)
        {}
        void renderPoint(Torous t, double light)
        {
            std::vector<Vec<3>> points = t.getPoints();
            Vec<3> lightVec = Vec<3>{-5,-5,5}.norm();
            for(Vec<3> point : points){
                Vec<2> pos = project(point);

                double angle = (point-t.m_pos).norm()*lightVec;
                double dist = point.x-m_position.x;
                unsigned int light = angle*light*255 - dist*2;
                light = light>255? 255 : light;
                Conscreen_pixel p = {.character='#', .style=CONSCREEN_ANSI_DEFAULT((uint8_t)light,(uint8_t)light,(uint8_t)light)};
                Conscreen_point s = Conscreen_console_size_get();
                Conscreen_pixel current = Conscreen_screen_get(x+s.x/2,y+s.y/2);
                if(current.style.forground.r<dist || current.character!='#')
                    Conscreen_screen_set(x+s.x/2,y+s.y/2, p);
            }
    }
};

int main()
{
    Conscreen_init();
    Torous t({0,0,0}, 5, 1, 0, 0);
    Camera c({-20,0,0}, 20);
    Conscreen_point p = Conscreen_console_size_get();

    auto points = t.getPoints();
    for(auto p : points) std::cout << p;

    bool flag=true;
    double time =0, light=7;

    while(true){

        if(Conscreen_screen_begin()==CS_REDRAW
           || flag){
            Conscreen_screen_clear();
                c.renderPoint(t, light);
            Conscreen_screen_flush();
        }
        flag=true;
        double d=0.5;
        switch (Conscreen_console_get_key()) {
            case 'w':
                c.m_position.get(X)+=d;
                break;
            case '[':
                light+=0.1;
                break;
            case ']':
                light-=0.1;
                break;
            case '+':
                c.m_focalLength+=d;
                break;
            case '-':
                c.m_focalLength-=d;
                break;
            case 'j':
                c.m_position.get(Y)+=d;
                break;
            case 'u':
                c.m_position.get(Y)-=d;
                break;
            case 'd':
                c.m_position.get(Z)+=d;
                break;
            case 'a':
                c.m_position.get(Z)-=d;
                break;
            case 's':
                c.m_position.get(X)-=d;
                break;
            case ' ':
                time+=0.05;
                break;
            default:
                flag=false;
        }
    }

    Conscreen_deinit();
}
