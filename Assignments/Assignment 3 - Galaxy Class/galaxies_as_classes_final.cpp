/**
 * Assignment 3: Galaxy Class
 * Written by: James Michael Patrick Brady
 * 
 * Program Overview:
 * This file defines a galaxy class with all the members required by the rubric
 * (total mass, Hubble type, stellar mass fraction and redshift), with a name
 * attribute added for easier identification and a vector that can contain 
 * satellite galaxies within the class definition.
 * 
 * I have overloaded some of the stream operators to allow for native insertion and
 * extraction of the hubble types, which I have defined as an enum class.
 * 
 * The main() function acts to demonstrate class interactions and features.
 **/

#include <iostream>
#include <iomanip>
#include <cmath>
#include <vector>
#include <string>
#include <algorithm> //Required for std::find()

//Defined the hubble_classes as an enum class to limit their possible values and to learn how to use enums
//However, this means that I've had to define a complementary string vector to allow this to work properly
enum class hubble_class {E0, E1, E2, E3, E4, E5, E6, E7, S0, Sa, Sb, Sc, SBa, SBb, SBc, Irr};
std::vector<std::string> hubble_types{"E0", "E1", "E2", "E3", "E4", "E5", "E6", "E7", "S0", "Sa", "Sb", "Sc",
                                      "SBa", "SBb", "SBc", "Irr"};

void clear_iostream();

std::istream& operator>>(std::istream& input, hubble_class& hubble_type)
{
  /* An overload of >> to allow std::cin >> to be used directly a variable of the enum class
  hubble_class, because it's far more convenient to encase that behaviour in an object and
  operator designed for extractions rather than including that in the class itself. Besides,
  this makes the method more general throughout a program. */
  std::string input_string{""};
  input >> input_string;
  std::vector<std::string>::iterator position{std::find(hubble_types.begin(), hubble_types.end(), input_string)};
  if((position < hubble_types.end()) && (position >= hubble_types.begin())){
    long long int index{position - hubble_types.begin()};
    hubble_type = hubble_class{index};
  } else {
    std::cin.setstate(std::ios_base::failbit);
  }
  return input;
}

std::ostream& operator<<(std::ostream& output, const hubble_class& hubble_type)
{
  /* As above, but this allows for easier and more general outputs to the terminal
  for galaxy objects. It also avoids having to store the Hubble type as a string
  inside a galaxy object alongside the enum hubble_class form. */
  std::string hubble_type_string{hubble_types[int(hubble_type)]};
  output << hubble_type_string;
  return output;
}

//Classes used in program - attempted a definition like the functions above, but that didn't work
class galaxy
{
  private:
    /* All the attributes as required by the rubric. The galaxy's name is also included because
    it's helpful, especially for differentiating different galaxy objects when output to the terminal */
    std::string name;
	  double redshift;
    double total_mass;
    double stellar_mass_fraction;
    hubble_class hubble_type;
    std::vector<galaxy> satellite_galaxies;

  public:
	  galaxy()
    {
      /* This is the constructor used when no parameters are specified at the creation of a galaxy.
      It is especially useful for galaxies created at runtime, for instance, satellite galaxies created
      by .add_satellite() */
      std::cout << "Please input the name of the galaxy: ";
      std::getline(std::cin, name);
      //clear_iostream();

      std::cout << "Please input the redshift of the galaxy (between 0 and 10): ";
      std::cin >> redshift;
      while(std::cin.fail() || (redshift < 0 || redshift > 10)){
        clear_iostream();
        std::cout << "Please type a valid redshift value: ";
        std::cin >> redshift;
      }
      clear_iostream();

      std::cout << "Please input total mass of the galaxy (in 10^7 solar masses, up to 10^13 solar masses): ";
      std::cin >> total_mass;
      while(std::cin.fail() || (total_mass < 1 || total_mass > 100000)){
        clear_iostream();
        std::cout << "Please type a valid mass value: ";
        std::cin >> total_mass;
      }
      total_mass *= std::pow(10, 7);
      clear_iostream();

      std::cout << "Please input the stellar mass fraction of the galaxy (up to 0.05): ";
      std::cin >> stellar_mass_fraction;
      while(std::cin.fail() || (stellar_mass_fraction < 0 || stellar_mass_fraction > 0.05)){
        clear_iostream();
        std::cout << "Please type a valid mass fraction: ";
        std::cin >> stellar_mass_fraction;
      }
      clear_iostream();

      std::cout << "Please input the Hubble type of the galaxy: ";
      std::cin >> hubble_type;
      while(std::cin.fail()){
        clear_iostream();
        std::cout << "Please enter a valid Hubble type: ";
        std::cin >> hubble_type;
      }
      clear_iostream();
    }

    galaxy(std::string galaxy_name, double rshift, double m_tot, double m_frac, hubble_class h_type)
    {
      /* This is the parameterised constructor. It is useful for galaxy objects created at compile time. */
      name = galaxy_name;
      redshift = rshift;
      total_mass = m_tot * std::pow(10, 7);
      stellar_mass_fraction = m_frac;
      hubble_type = h_type;
    }

		~galaxy()
		{
			std::cout << "Galaxy object '" << this->name << "' removed from memory at " << this << std::endl;
		}

    void change_h_type()
    {
      /* Gets a new Hubble type as an input from the user and changes the Hubble type
      of the galaxy instance once validated */
      std::string temp;
      std::cout << "Current Hubble type of " << name << ": " << this->hubble_type << std::endl;
      std::cout << "Please enter the new Hubble type of the galaxy: ";
      std::cin >> this->hubble_type;
      while(std::cin.fail()){
        clear_iostream();
        std::cout << "Invalid input. Please input a valid Hubble type: ";
        std::cin >> this->hubble_type;
      }
      std::cout << "Hubble type of " << name << " changed to: " << hubble_type << std::endl;
    }

    void change_h_type(hubble_class h_type)
    {
      /* Overload of above for a predefined Hubble type */
      std::cout << "Hubble type of " << name << " changed from " << hubble_type << " to ";
      hubble_type = h_type;
      std::cout << hubble_type << std::endl;
    }
    
    void add_satellite()
    {
      /* This function adds a satellite galaxy to the vector that contains them inside a galaxy instance.
      This form will call the standard constructor. */
      galaxy satellite;
      satellite_galaxies.push_back(satellite);
    }

    void add_satellite(galaxy satellite)
    {
      /* This overload of add_satellite is defined for use with the << operator into a galaxy instance */
      satellite_galaxies.push_back(satellite);
    }

    void add_satellite(std::string galaxy_name, double rshift, double m_tot, double m_frac, hubble_class h_type)
    {
      /* This overload is used when the parameters are defined in advance and hence calls the parameterised
      constructor rather than the standard constructor. */
      galaxy satellite(galaxy_name, rshift, m_tot, m_frac, h_type);
      satellite_galaxies.push_back(satellite);
    }

    double get_stellar_mass()
    {
      /* Returns the stellar mass of a galaxy object. */
      return double{this->stellar_mass_fraction * this->total_mass};
    }

  //Allows the function get_properties() to access the private members of a galaxy object 
  friend void get_properties(galaxy& input_galaxy);
};

galaxy& operator>>(galaxy& satellite, galaxy& parent)
{
  parent.add_satellite(satellite);
}

int main()
{
  galaxy galaxy_1("Galaxy 1", 0, 100000, 0.05, hubble_class::Irr);
  galaxy galaxy_2;
  galaxy galaxy_satellite("Galaxy 1a", 0 , 13000, 0.05, hubble_class::E7);
  galaxy_satellite >> galaxy_1;
  galaxy_1.add_satellite("Galaxy 1b", 0, 10000, 0.025, hubble_class::E6);
  galaxy_1.add_satellite("Galaxy 1c", 0, 30000, 0.015, hubble_class::SBa);
  galaxy_1.change_h_type(hubble_class::Sc);
  galaxy_2.add_satellite("Galaxy 2a", 3, 100, 0.0235, hubble_class::Irr);
  galaxy galaxy_3("Galaxy 3", 5, 10, 0.01, hubble_class::E0);

  std::vector<galaxy> galaxies{galaxy_1, galaxy_2, galaxy_3};
  std::vector<galaxy>::iterator galaxies_begin{galaxies.begin()};
  std::vector<galaxy>::iterator galaxies_end{galaxies.end()};
  std::vector<galaxy>::iterator galaxies_current;
  for(galaxies_current = galaxies_begin; galaxies_current < galaxies_end; ++galaxies_current){
    get_properties(*galaxies_current);
  }
	return 0;
}

void clear_iostream()
{
  /* Clears the IOStream objects after inputs, as a part of validation and preventing an overflowing stream.
  It's more convenient than writing these two lines every time I need to check an input and I really like
  working with modular code. */
  std::cin.clear();
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void get_properties(galaxy& input_galaxy)
{
  /* Outputs all properties of a galaxy to the terminal, including the number of satellite galaxies
  and the satellite galaxies' properties if applicable. */
  std::cout << "Name: " << input_galaxy.name << std::endl;
  std::cout << "Redshift: " << input_galaxy.redshift << std::endl;
  std::cout << "Mass: " << input_galaxy.total_mass << " solar masses"<< std::endl;
  std::cout << "Stellar Mass Fraction: " << input_galaxy.stellar_mass_fraction << std::endl;
  std::cout << "Stellar Mass: " << input_galaxy.get_stellar_mass() << " solar masses" << std::endl;
  std::cout << "Hubble Type: " << input_galaxy.hubble_type << std::endl;
  std::cout << input_galaxy.satellite_galaxies.size() << " satellite galaxies found" << std::endl;
  if(input_galaxy.satellite_galaxies.size() > 0){
    if(input_galaxy.satellite_galaxies.size() == 1){
      std::cout << "Satellite galaxy of " << input_galaxy.name << ":" << std::endl;
    } else{
      std::cout << "Satellite Galaxies of " << input_galaxy.name << ":" << std::endl;
    }
    std::cout << "---------------------------" << std::endl;
    std::vector<galaxy>::iterator satellite_begin{input_galaxy.satellite_galaxies.begin()};
    std::vector<galaxy>::iterator satellite_end{input_galaxy.satellite_galaxies.end()};
    std::vector<galaxy>::iterator satellite_current;
    for(satellite_current = satellite_begin; satellite_current < satellite_end; ++satellite_current){
      get_properties(*satellite_current);
      std::cout << "Satellite of " << input_galaxy.name << std::endl;
      std::cout << "---------------------------" << std::endl;      
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
}