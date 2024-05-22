#include "Conscreen/Conscreen.h"
#include "Conscreen/Conscreen_ANSI.h"
#include "Conscreen/Conscreen_console.h"
#include "Conscreen/Conscreen_screen.h"
#include <cmath>
#include <cstddef>
#include <iterator>
#include <ranges>
#include <vector>
#include <iostream>

#define PI 3.14159265358979

enum Axies: int {
X=0,Y=1,Z=2
};

Axies next(Axies a, int n=1)
{
    return Axies{(a+n)%(Axies::Z+1)};
}

struct Matrix;
struct Vec3
{
    double x,y,z;
    Vec3 transform(Matrix m);

    constexpr void operator*=(double scalar){
        *this=Vec3{x*scalar,y*scalar,z*scalar};
    }
    constexpr Vec3 operator*(double scalar){
        return Vec3{x*scalar,y*scalar,z*scalar};
    }
    double& get(Axies a)
    {
        switch(a)
        {
            case Axies::X: return x;
            case Axies::Y: return y;
            case Axies::Z: return z;
        }
    }
    Vec3 operator-(const Vec3& other) const
    {
        return {this->x-other.x,
        this->y-other.y,
        this->z-other.z
        };
    }
    Vec3 operator+(const Vec3& other) const
    {
        return {this->x+other.x,
        this->y+other.y,
        this->z+other.z
        };
    }
    double scalarprodukt(const Vec3 &other)
    {
        return this->x*other.x+
          this->y*other.y+
          this->z*other.z;
    }
    Vec3 norm()
    {
        return {x/length(),y/length(),z/length()};
    }
    double length() const
    {
        return sqrt(x*x+y*y+z*z);
    }
};

struct Matrix
{
    friend std::ostream& operator<<(std::ostream& out, const Matrix& m){
        for(int i=0; i <= Axies::Z; i++){
            for(int d=0; d <= Axies::Z; d++)
                out << m.get(Axies{d}).get(Axies{i}) << '\t';
            out << std::endl;
        }
        return out;
    }

    Vec3 tx={1,0,0},
         ty={0,1,0},
         tz={0,0,1};

    Vec3 get(Axies a) const
    {
        switch(a)
        {
            case Axies::X: return tx;
            case Axies::Y: return ty;
            case Axies::Z: return tz;
        }
    }
    Vec3& get(Axies a)
    {
        switch(a)
        {
            case Axies::X: return tx;
            case Axies::Y: return ty;
            case Axies::Z: return tz;
        }
    }
};

Vec3 Vec3::transform(Matrix m)
{
    return Vec3{
    x*m.tx.x+y*m.ty.x+z*m.tz.x,
    x*m.tx.y+y*m.ty.y+z*m.tz.y,
    x*m.tx.z+y*m.ty.z+z*m.tz.z
    };
}

class Torous
{

    public:
    Vec3 m_pos;
    double m_rotX, m_rotY,
        m_holeRadius, m_tubeRadius,
        angle=0.01;

    static std::vector<Vec3> getCircle(Vec3 center, double radius, Axies a, double angle)
    {
        std::vector<Vec3> circle;
        for(double time=0; time<2.0*PI; time+=angle){

            Matrix rot= Torous::getRotTransform(a, time);
            Vec3 v={0,0,0};
            v.get(next(a))=radius;
            circle.push_back(center+v.transform(rot));
        }
        return circle;
    }
        Torous(Vec3 pos, double holeRadius, double tubeRadius, double rotX, double rotY)
            : m_pos(pos), m_rotX(rotX), m_rotY(rotY), m_holeRadius(holeRadius), m_tubeRadius(tubeRadius)
        {}

        std::vector<Vec3> getPoints()
        {
            Vec3 first_pos = m_pos;
            first_pos.get(X)+=m_holeRadius;
            std::vector<Vec3> circle = getCircle(first_pos, m_tubeRadius, Y, angle);
            std::vector<Vec3> torous;
            for(double time=0; time < 2.0*PI; time+=angle){
                std::ranges::copy(circle
                                | std::ranges::views::transform([time](Vec3 p){return p.transform(getRotTransform(Z,time));}),
                                std::back_inserter(torous));
            }
            return torous;
        }
    static Matrix getRotTransform(Axies a, double time)
    {
        Matrix m;
        m.get(next(a)).get(next(a))=cos(time);
        m.get(next(a)).get(next(a,2))=sin(time);
        m.get(next(a, 2)).get(next(a))=-sin(time);
        m.get(next(a, 2)).get(next(a,2))=cos(time);

        // std::cout << m << std::endl;
        return m;
    }
};

class Camera{
    double project(double dist, double height){
        return m_focalLength*(height/dist);
    }
    public:
    Vec3 m_position;
    double m_focalLength;
        Camera(Vec3 pos, double f)
            : m_position(pos), m_focalLength(f)
        {}
        void renderPoint(Torous t, double light)
        {
            std::vector<Vec3> points = t.getPoints();
            Vec3 lightVec = Vec3{-5,-5,5}.norm();
            for(Vec3 point : points){
                double x=project(point.x-m_position.x, point.z-m_position.z)*2,
                    y=project(point.x-m_position.x, point.y-m_position.y);

                double angle = lightVec.scalarprodukt((point-t.m_pos).norm());
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
    // std::cout << p.x << " : " << p.y << std::endl;

    auto points = t.getPoints();
    for(auto p : points) printf("%lf %fl %fl\n", p.x, p.y, p.z);

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
