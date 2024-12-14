#include <iostream>
#include <cmath>
#include <vector>
#include <iomanip>

const double pi{4*atan(1)};

enum class shape_type{Unassigned, Square, Rectangle, Ellipse, Circle, Cube, Cuboid, Ellipsoid, Sphere, Prism};
std::vector<std::string> shape_types_output{"Unassigned", "Square", "Rectangle", "Ellipse", "Circle", "Cube", "Cuboid", "Ellipsoid", "Sphere", "Prism"};

std::ostream& operator<<(std::ostream& output, const shape_type& type)
{
    /* As with the Galaxies assignment, I've used an enum to contain shape data and the << operator has been
    overridden to allow for a native output to the terminal. */
    std::string shape_type_string{shape_types_output[int(type)]};
    output << shape_type_string;
    return output;
}

class shape
{   
    /* Base class for any shape*/
    protected:
        //Provides storage of a shape's type, having a default "Unassigned" type seems useful
        shape_type type{shape_type::Unassigned};

    public:
        shape() = default;
        virtual ~shape() = default;

        //Pure virtual functions for area/volume, designed to be overridden for each unique shape
        virtual double area() = 0;
        virtual double volume() = 0;
        
        shape_type get_type()
        {
            return type;
        }

        virtual void properties()
        {
            /* Returns the properties of a shape as needed */
            std::cout << "Shape: " << this->type << std::endl;
            std::cout << "Area: " << this->area() << " units^2" << std::endl;
            std::cout << "Volume: " << this->volume() << " units^3" << std::endl;
        }

};

class polygon: public shape
{
    /* Base class for 2D shapes, derived from the 'shape' base class */
    public:
        polygon() = default;
        virtual ~polygon() = default;

        //A perimeter method is defined for a polygon, as it is useful for prisms
        virtual double perimeter() = 0;
        virtual double area() = 0;

        double volume()
        {
            /* Polygons have no volume, hence the override is done here */
            return 0;
        }
};

class polyhedron: public shape
{
    /* Base class for 3D shapes, derived from the 'shape' base class */
    public:
        polyhedron() = default;
        virtual ~polyhedron() = default;

        //Although areas are not required for 3D shapes, I've decided to use it for the surface area
        virtual double area() = 0;
        virtual double volume() = 0;
};  

class square: public polygon
{
    private:
        double length{1};

    public:
        square(double l)
        {
            length = l;
            type = shape_type::Square;
        }

        ~square() = default;

        double perimeter()
        {
            return 4*length;
        }

        double area()
        {
            return std::pow(length, 2);
        }
};

class rectangle: public polygon
{
    private:
        double length{1};
        double height{1};

    public:
        rectangle(double l, double h)
        {
            length = l;
            height = h;
            type = shape_type::Rectangle;
        }

        ~rectangle() = default;

        double perimeter()
        {
            return 2*(length + height);
        }

        double area()
        {
            return length * height;
        }
};

class ellipse: public polygon
{
    private:
        double semi_axis_a{1};
        double semi_axis_b{1};

    public:
        ellipse(double a, double b)
        {
            semi_axis_a = a;
            semi_axis_b = b;
            type = shape_type::Ellipse;
        }

        ~ellipse() = default;

        //Ramanujan Approximation
        double perimeter()
        {
            return pi*(3*(semi_axis_a + semi_axis_b) - std::pow(((3*semi_axis_a + semi_axis_b)*(semi_axis_a + 3*semi_axis_b)), 0.5));
        }

        double area()
        {
            return pi * semi_axis_a * semi_axis_b; 
        }
};

class circle: public polygon
{   
    private:
        double radius{1};

    public:
        circle(double r)
        {
            radius = r;
            type = shape_type::Circle;
        }

        ~circle() = default;
        
        double perimeter()
        {
            return 2*pi*radius;
        }

        double area()
        {
            return pi * std::pow(radius, 2);
        }
};

class cube: public polyhedron
{
    private:
        double width{1};

    public:
        cube(double w)
        {
            width = w;
            type = shape_type::Cube;
        }

        ~cube() = default;

        double area()
        {
            return 6 * std::pow(width, 2);
        }

        double volume()
        {
            return std::pow(width, 3);
        }
};

class cuboid: public polyhedron
{
    private:
        double width{1};
        double depth{1};
        double height{1};

    public:
        cuboid(double x, double y, double z)
        {
            width = x;
            depth = y;
            height = z;
            type = shape_type::Cuboid;
        }

        ~cuboid() = default;

        double area()
        {
            return 2*(width*depth + width*height + depth*height);
        }

        double volume()
        {
            return width*depth*height;
        }
};

class ellipsoid: public polyhedron
{
    private:
        double semi_axis_a{1};
        double semi_axis_b{1};
        double semi_axis_c{1};

    public:
        ellipsoid(double a, double b, double c)
        {
            semi_axis_a = a;
            semi_axis_b = b;
            semi_axis_c = c;
            type = shape_type::Ellipsoid;
        }

        ~ellipsoid() = default;

        double area()
        {
            if(semi_axis_a == semi_axis_b){ 
                double eccentricity;
                if(semi_axis_a > semi_axis_c){
                    //Oblate case
                    eccentricity = std::pow((1 - std::pow((semi_axis_c/semi_axis_a), 2)), 0.5);
                    return 2*pi*std::pow(semi_axis_a, 2)*(1 + (std::pow((semi_axis_c/semi_axis_a), 2)/eccentricity)*atanh(eccentricity));
                } else{
                    //Prolate case
                    eccentricity = std::pow((1 - std::pow((semi_axis_a/semi_axis_c), 2)), 0.5);
                    return 2*pi*std::pow(semi_axis_a, 2)*(1 + semi_axis_c/(semi_axis_a*eccentricity)*asinh(eccentricity));
                }
            } else{
                //Triaxial approximation, given that exact solutions do exist, but rely on computational integrals which would be out of scope here
                double power{1.6075};
                double numerator{std::pow(semi_axis_a, power)*std::pow(semi_axis_b, power) + std::pow(semi_axis_a, power)*std::pow(semi_axis_c, power) +
                std::pow(semi_axis_b, power)*std::pow(semi_axis_c, power)};
                return 4*pi*std::pow((numerator/3), (1/power));
            }
        }

        double volume()
        {
            return (4*semi_axis_a*semi_axis_b*semi_axis_c)/3;
        }
};

class sphere: public polyhedron
{
    private:
        double radius{1};

    public:
        sphere(double r)
        {
            radius = r;
            type = shape_type::Sphere;
        }

        ~sphere() = default;

        double area()
        {
            return 4*pi*std::pow(radius, 2);
        }

        double volume()
        {
            return (4*pi*std::pow(radius, 3))/3;
        }
};

class prism: public polyhedron
{
    private:
        polygon* face{nullptr};
        double height{1};
        shape_type face_type{shape_type::Unassigned};

    public:
        prism(polygon* base_face, double length)
        {
            face = base_face;
            height = length;
            type = shape_type::Prism;
            face_type = face->get_type();
        }

        ~prism()
        {
            delete face;
        }

        double area()
        {
            double base_area{face->area()};
            double base_perimeter{face->perimeter()};
            return 2*base_area + base_perimeter*height; 
        }

        double volume()
        {
            double base_area{face->area()};
            return base_area * height;
        }

        void properties()
        {
            std::cout << "Shape: " << this->face_type << " " << this->type;
            //Provides additional names for specific prisms
            if(this->face_type == shape_type::Circle){
               std::cout << " (Cylinder) " << std::endl;
            } else if(this->face_type == shape_type::Square){
                std::cout << " (Square-Base Cuboid) " << std::endl;
            } else if(this->face_type == shape_type::Rectangle){
                std::cout << " (Rectangle-Base Cuboid) " << std::endl;
            } else{
                std::cout << std::endl;
            }
            std::cout << "Area: " << this->area() << " units^2" << std::endl;
            std::cout << "Volume: " << this->volume() << " units^3" << std::endl;
        }
};

int main()
{   
    shape** shapes;
    shapes = new shape*[12];
    shapes[0] = new square(2);
    shapes[1] = new rectangle(2,3);
    shapes[2] = new ellipse(2,3);
    shapes[3] = new circle(2);
    shapes[4] = new cube(2);
    shapes[5] = new cuboid(2,3,4);
    shapes[6] = new ellipsoid(2,2,3);
    shapes[7] = new sphere(2);
    shapes[8] = new prism(new circle(2), 4);
    shapes[9] = new prism(new square(2), 4);
    shapes[10] = new prism(new rectangle(2, 3), 4);
    shapes[11] = new prism(new ellipse(2, 3), 4);
    
    for(int i{0}; i < 12; i++){

        shapes[i]->properties();
        std::cout << std::endl;
        delete shapes[i];
    }

    delete[] shapes;
    return 0;
}