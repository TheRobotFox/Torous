#include "Conscreen/Conscreen.h"
#include "Conscreen/Conscreen_ANSI.h"
#include "Conscreen/Conscreen_console.h"
#include "Conscreen/Conscreen_screen.h"
#include "Matrix.hpp"
#include "Vec.hpp"
#include <iostream>
#include <string>

#define PI 3.14159265358979
using namespace anli;


class Torous
{

    public:
    Vec<3> m_pos;
    double m_rotX, m_rotY,
        m_holeRadius, m_tubeRadius,
        angle=0.1;

    static std::vector<Vec<3>> getCircle(Vec<3> center, double radius, Dimension a, double angle)
    {
        std::vector<Vec<3>> circle;
        for(double time=0; time<2.0*PI; time+=angle){

            Matrix rot= Matrix<3,3>::getRotation(a, time);
            Vec<3> v= {0,0,0};
            v.get(a.next())=radius;
            circle.push_back(center+rot*v);
        }
        return circle;
    }
        Torous(Vec<3> pos, double holeRadius, double tubeRadius, double rotX, double rotY)
            : m_pos(pos), m_rotX(rotX), m_rotY(rotY), m_holeRadius(holeRadius), m_tubeRadius(tubeRadius)
        {}

        std::vector<Matrix<2,3>> getPoints()
        {
            Vec<3> first_pos = m_pos;
            first_pos.get(X)+=m_holeRadius;
            std::vector<Vec<3>> circle = getCircle(first_pos, m_tubeRadius, Y, angle);
            std::vector<Matrix<2,3>> torous;
            for(double time=0; time < 2.0*PI; time+=angle){
                std::ranges::copy(circle
                                  | std::ranges::views::transform([time,this](const Vec<3> &p){
                                      Vec<3> pos = Matrix<3,3>::getRotation(X,this->m_rotX)
                                          *Matrix<3,3>::getRotation(Y,this->m_rotY)
                                          *Matrix<3,3>::getRotation(Z,time)*p;
                                    return Matrix<2,3>{pos, (pos-p).norm()};
                                  }),
                                std::back_inserter(torous));
            }
            return torous;
        }
};

class Camera{
    // double project(double dist, double height){
    //     return m_focalLength*(height/dist);
    // }
    Vec<2> project(Vec<3> point){
        point = point-m_position;
        Matrix<3,2> proj = {
            {0},
            {0,1},
            {2,0}
        };
        return proj*point*m_focalLength/point.x;
    }
    public:
    Vec<3> m_position;
    double m_focalLength;
        Camera(Vec<3> pos, double f)
            : m_position(pos), m_focalLength(f)
        {}
        void renderPoint(Torous t, double ambient)
        {
            auto points = t.getPoints();
            Vec<3> lightVec = Vec<3>{2,-5,5}.norm();
            for(Matrix<2,3> &tp : points){
                Vec<3> point=tp.get(0),
                       norm =tp.get(1);

                double angle = std::max(-lightVec*norm,0.0);
                double dist = std::abs(point.x-m_position.x);
                unsigned int light = std::max(angle*255+dist*ambient,0.0);
                light = light > 255? 255 : light;
                Conscreen_pixel p = {.character='#', .style=CONSCREEN_ANSI_DEFAULT((uint8_t)light,(uint8_t)light,(uint8_t)light)};
                Conscreen_point s = Conscreen_console_size_get();

                Vec<2> imgCoords = project(point);
                Conscreen_screen_set(imgCoords.x+s.x/2,imgCoords.y+s.y/2, p);

                // std::string sa = std::to_string(angle);
                // for(int i=0; i< sa.length(); i++){
                //     Conscreen_pixel f = {.character=sa.at(i), .style=CONSCREEN_ANSI_NORMAL};
                //     Conscreen_screen_set(i,0,f);
                // }
            }
    }
};

int main()
{
    Conscreen_init();
    Torous t({0,0,0}, 5, 2, 0, 0);
    auto points = t.getPoints();
    for(auto p : points) std::cout << p;
    Camera c({-20,0,0}, 20);
    Conscreen_point p = Conscreen_console_size_get();


    bool flag=true;
    double time =0, light=3;

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
                c.m_position.x+=d;
                break;
            case '[':
                light+=0.3;
                break;
            case ']':
                light-=0.3;
                break;
            case '+':
                c.m_focalLength+=d;
                break;
            case '-':
                c.m_focalLength-=d;
                break;
            case 'j':
                c.m_position.y+=d;
                break;
            case 'u':
                c.m_position.y-=d;
                break;
            case 'd':
                c.m_position.z+=d;
                break;
            case 'a':
                c.m_position.z-=d;
                break;
            case 's':
                c.m_position.x-=d;
                break;
            case 'g':
                t.m_rotX+=0.05;
                break;
            case 'h':
                t.m_rotY+=0.05;
                break;
            default:
                flag=false;
        }
    }

    Conscreen_deinit();
}
