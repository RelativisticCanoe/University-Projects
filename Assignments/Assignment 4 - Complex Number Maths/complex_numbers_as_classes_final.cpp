/**
 * Assignment 4: Complex Number Class and Operator Overloading
 * Written by: James Michael Patrick Brady
 * 
 * Program Overview:
 * This file defines a complex number class with two souble variables: one for the real
 * component of the number and one for the imaginary component. The main arithmetic, assignment
 * and IO operators have been overloaded to natively work with the class, such that this file
 * could be saved as a header, imported into .cpp file and be used without having to modify
 * that source file to allow for complex numbers. This of course won't be necessary, as
 * the <complex> header already exists for this purpose.
 * 
 * There are also several member functions that return the properties of the complex numbers for
 * use in calculations without allowing for the modfication of the two components of the number.
 * The real part, imaginary part, modulus and argument are returnable as doubles and the 
 * conjugate is returned as another complex number object. Several utilities are also included
 * to set the complex number, mostly useful for inputs (as one is used in the definition of >>)
 * and for initializations where the components are not determined yet. They are unlikely to be
 * useful most of the time, but I've included them because there are scenarios where setter functions
 * could be useful despite >>, +=, -=, *= and /= having been overloaded.
 * 
 * A few protections are also provided to return NaNs when it is not possible to return
 * a number - e.g. division by 0 + 0i and attempting to find the angle at the origin. In addition,
 * it is possible to return the argument between [0, 2*pi] or [-pi, pi], as different bases can be
 * useful in different scenarios.
 * 
 * The main() function acts to demonstrate all class interactions and features implemented, including
 * user input and subsequent extraction of the input into a complex object.
 **/

//Headers
#include <iostream>
#include <iomanip>
#include <cmath>
#include <string>

//Functions Used Within Classes
void clear_iostream();

//Classes and Operators
class complex
{
	private:
		/* The two components of a complex number. Made private to restrict access and prevent
		accidents or unwanted overrides, alongside uses of <type> function() const in member functions
		and const complex& <name> when passing by reference. */
		double real;
		double imaginary;
	
	public:
		complex()
		{
			//Default values if not specified at construction
			real = 0;
			imaginary = 0;
		}

		complex(double x, double y)
		{
			real = x;
			imaginary = y;
		}

		//Note to self: "delete this;" does not work well (recursive loop)! Only use on dynamic objects!
		~complex()
		{
			std::cout << "complex object storing value " << *this << " at address " << this << " removed from memory" << std::endl;
		}

		/*The arithmetic and I/O operators defined for this class.
		The _= operators have also been defined because sometimes you want to reassign objects, like
		with normal numbers .These operators hence drop the const for the left object, as we want
		to change it in the operation. */
		friend complex operator+(const complex& a, const complex& b);
		friend complex operator+=(complex& a, const complex& b);
		friend complex operator-(const complex& a, const complex& b);
		friend complex operator-=(complex& a, const complex& b);
		friend complex operator*(const complex& a, const complex& b);
		friend complex operator*=(complex& a, const complex& b);
		friend complex operator/(const complex& a, const complex& b);
		friend complex operator/=(complex& a, const complex& b);
		friend std::ostream& operator<<(std::ostream& output, const complex& comp);
		friend std::istream& operator>>(std::istream& input, complex& comp);

		//Sets the complex number as a whole if it needs to change
		void set_complex()
		{
			std::cout << "Please input a complex number of the form (a +/- ib): ";
			std::cin >> *this;
			if(std::cin.fail())
			{
				clear_iostream();
				std::cout << "Invalid, lease input a valid complex number of the form (a +/- ib): ";
				std::cin >> *this;
			}
			std::cout << std::endl;
		}

		void set_complex(const double& x, const double& y)
		{
			this->real = x;
			this->imaginary = y;
		}

		//Allows the below function to directly access the real/imaginary values of an input
		friend void set_complex(const complex& z);

		//Convenient if you need to copy a complex without using = for whatever reason?
		void set_complex(const complex& z)
		{
			/* I am only using this */
			this->real = z.real;
			this->imaginary = z.imaginary;
		}

		/* Functions to return the components of the complex number which can be used in operations
		without allowing for modification of the attributes themselves. These functions are also const
		as they should only depend on the attributes of the object and hence should not be modifiable. */

		const double re() const
		{
			return real; 
		}

		const double im() const
		{
			return imaginary;
		}

		const double modulus() const
		{
			return std::sqrt(real*real + imaginary*imaginary);
		}

		const complex conjugate() const
		{
			return complex(real, -imaginary);
		}

		const double argument(bool positive = false) const
		{
			if(imaginary == 0){
				/* Default cases for when the standard formula breaks down (Im{z}=0).
				These are first as they are less expensive computationally, hence
				the class is more efficient overall. */
				if(real > 0){
					return 0;
				} else if(real < 0){
					return M_PI;
				} else {
					/* How do you even define an angle if you're at the origin?
					You don't. */
					return std::nan("");
				}
			} else if((imaginary >= 0) || (positive == false)){
				//Included a condition that allows for negative angles by default
				return 2 * std::atan((modulus()-real)/imaginary);
			} else {
				//Corrects to give a positive angle if specified.
				return 2 * std::atan((modulus()-real)/imaginary) + (2 * M_PI);
			}
		}
};

//Operators for the complex class
complex operator+(const complex& a, const complex& b)
{
	double new_real = a.real + b.real;
	double new_imaginary = a.imaginary + b.imaginary;
	return complex(new_real, new_imaginary);
}

complex operator+=(complex& a, const complex& b)
{
	a.real = a.real + b.real;
	a.imaginary = a.imaginary + b.imaginary;
	return a;
}

complex operator-(const complex& a, const complex& b)
{
	double new_real = a.real - b.real;
	double new_imaginary = a.imaginary - b.imaginary;
	return complex(new_real, new_imaginary);
}

complex operator-=(complex& a, const complex& b)
{
	a.real = a.real - b.real;
	a.imaginary = a.imaginary - b.imaginary;
	return a;
}

complex operator*(const complex& a, const complex& b)
{
	double new_real = ((a.real * b.real) - (a.imaginary * b.imaginary));
	double new_imaginary = (a.imaginary * b.real) + (a.real * b.imaginary);
	return complex(new_real, new_imaginary);
}

complex operator*=(complex& a, const complex& b)
{
	double temp_real;
	double temp_imaginary;
	temp_real = ((a.real * b.real) - (a.imaginary * b.imaginary));
	temp_imaginary = (a.imaginary * b.real) + (a.real * b.imaginary);
	a.real = temp_real;
	a.imaginary = temp_imaginary;
	return a;
}

complex operator/(const complex& a, const complex& b)
{
	/* Could also do via conjugates -> less room for error? */
	if((b.real == 0 )&& (b.imaginary == 0)){
		//Building in protection against division by zero issues
		return complex(std::nan(""), std::nan(""));
	} else {
		double denominator = (b.real * b.real) + (b.imaginary * b.imaginary);
		double new_real = ((a.real * b.real) + (a.imaginary * b.imaginary))/denominator;
		double new_imaginary = ((a.imaginary * b.real) - (a.real * b.imaginary))/denominator;
		return complex(new_real, new_imaginary);
	}
}

complex operator/=(complex& a, const complex& b)
{
	if((b.real == 0 ) && (b.imaginary == 0)){
		//As with operator/, just assigning to the values of a instead
		a.real = std::nan("");
		a.imaginary = std::nan("");
		return a;
	} else {
		//As the imaginary and real values of a are dependent on each other, they must be assigned after calculation
		double temp_real;
		double temp_imaginary;
		double denominator = (b.real * b.real) + (b.imaginary * b.imaginary);
		temp_real = ((a.real * b.real) + (a.imaginary * b.imaginary))/denominator;
		temp_imaginary = ((a.imaginary * b.real) - (a.real * b.imaginary))/denominator;
		a.real = temp_real;
		a.imaginary = temp_imaginary;
		return a;
	}
}

std::istream& operator>>(std::istream& input, complex& comp)
{
	/* This is the only non= operator that doesn't use a const complex& object as the idea
	is that we WANT to change the underlying object, not use it for a calculation
	(where it can be useful to leave the object unchanged) or output it. */
	double real_num;
	std::string operation;
	std::string imaginary_str;
	double imaginary_num;

	input >> real_num >> operation >> imaginary_str;

	if(!(operation == "+" || operation == "-") || imaginary_str[0] != 'i'){
		//Critically allows for a fail case, which allows for validation in all contexts
		std::cin.setstate(std::ios_base::failbit);
	} else{
		imaginary_str.erase(imaginary_str.begin());
		imaginary_num = std::stod(imaginary_str);
		if(operation == "-"){
			imaginary_num *= -1;
		}
	}

	comp.real = real_num;
	comp.imaginary = imaginary_num;
	return input;
}

std::ostream& operator<<(std::ostream& output, const complex& comp)
{
	if(comp.imaginary >= 0){
		std::cout << comp.real << "+" << comp.imaginary << "i";	
	} else {
		//The float already handles the - part, just not the +
		std::cout << comp.real << comp.imaginary << "i";
	}
	return output;
}

//Functions Used In int main() Only 
void get_components(const complex& z);

//int main() and Function Definitions
int main()
{
	complex complex_1(3, 4);
	complex complex_2(1, -2);
	complex input_complex;
	input_complex.set_complex();
	get_components(complex_1);
	std::cout << std::endl;
	get_components(complex_2);
	std::cout << std::endl;
	get_components(input_complex);
	std::cout << std::endl;

	std::cout << "Sum of 3+4i and 1-2i: " << complex_1 + complex_2 << std::endl;
	std::cout << "Difference of 3+4i and 1-2i: " << complex_1 - complex_2 << std::endl;
	std::cout << "Product of 3+4i and 1-2i: " << complex_1 * complex_2 << std::endl;
	std::cout << "Quotient of 3+4i by 1-2i: " << complex_1 / complex_2 << std::endl;

	//Complex numbers corresponding to different quadrants of an Argand diagram
	complex tl(-3, 4);
	complex bl(-3, -4);
	complex br(3, -4);
	tl += bl;
	//Should be 6 - 0i
	std::cout << tl << std::endl;
	//Should be 0 + 0i
	std::cout << bl + complex_1 << std::endl;
	return 0;
}

void get_components(const complex& z)
{
	/* Outputs the components of a complex object, as a sanity check
	Defaults to an argument in the range [-pi, pi], though the 'positive' bool
	for complex::modulus() allows this to be changed to [0, 2*pi] in other situations. */
	std::cout << "Complex Number: " << z << std::endl;
	std::cout << "Real Component: " << z.re() << std::endl;
	std::cout << "Imaginary Component: " << z.im() << "i" << std::endl;
	std::cout << "Modulus: " << z.modulus() << std::endl;
	std::cout << "Argument: " << z.argument() << " radians" << std::endl;
	std::cout << "Conjugate: " << z.conjugate() << std::endl;
}

void clear_iostream()
{
  /* Clears the IOStream objects after inputs, as a part of validation and preventing an overflowing stream.
  It's more convenient than writing these two lines every time I need to check an input and I really like
  working with modular code. */
  std::cin.clear();
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}