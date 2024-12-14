#include <iostream>
#include <iomanip>
#include <cmath>

const double ev_to_J{1.60217663 * std::pow(10,-19)};

/* Functions are pre-defined here in the same order as they defined after main().
This makes main() the first fully-defined function, whilst allowing the program's
scope to be known in advance. */
double delta_E_calc(int quantum_initial, int quantum_final, int atomic_no, bool convert);
int quantum_int_gather();
bool confirm_yes_no_funct();

int main()
{
    /* Preparing the cin variables before input, as the example codes do.
    Confirm is reused as a bool to check conditions throughout*/
    int quantum_initial;
    int quantum_final;
    int atomic_no;
    bool confirm{false};
    double delta_E;    

    //Asking the user for the required inputs and validating them in turn. 
    std::cout << "Please enter the atomic number of the atom: ";
    atomic_no = quantum_int_gather();

    std::cout << "Please enter the inital quantum number of the electron: ";
    quantum_initial = quantum_int_gather();

    std::cout << "Please enter the final quantum number of the electron: ";
    quantum_final = quantum_int_gather();
    while (quantum_final > quantum_initial)
    {
        //Removes the absorbtion regime, as per specification
        std::cout << "Input is not physical for emissions, please enter an integer <= than the inital quantum number: ";
        quantum_final = quantum_int_gather();
    }
   
    std::cout << "Would you like to output the result in J instead of eV (default)? (Y/N) ";
    confirm = confirm_yes_no_funct();
    std::cout << std::endl;

    if (confirm){
        delta_E = delta_E_calc(quantum_initial, quantum_final, atomic_no, true);
        std::cout << "The energy of the emitted photon is " << std::setprecision(3) << delta_E << " J." << std::endl;
        std::cout << std::endl;
    } else {
        delta_E = delta_E_calc(quantum_initial, quantum_final, atomic_no, false);
        std::cout << "The energy of the emitted photon is " << std::setprecision(3) << delta_E << " eV." << std::endl;
        std::cout << std::endl;
    }

    std::cout << "Would you like to make another calculation? (Y/N) ";
    confirm = confirm_yes_no_funct();
    if (confirm){
        std::cout << "-------------------------------------------------" << std::endl;
        main();
    }

    return 0;
}

double delta_E_calc(int quantum_initial, int quantum_final, int atomic_no, bool convert = false)
{
    /* Takes in the parameters from main(), calculates the change in energy,
    converts to J if wanted and returns the value calculated. Defaults to 
    calculating in eV. */ 
    double delta_E;
    delta_E = 13.6 * std::pow(atomic_no, 2) * (std::pow(quantum_final , -2) - std::pow(quantum_initial, -2));
    if (convert){
        delta_E *= ev_to_J;
    }
    return delta_E;
}

int quantum_int_gather()
{
    /* Used to obtain the quantum integers and atomic number from the user
    and performs validation over the inputs to remove errant values. Any errant
    characters will stop the rest of the buffer from being read, hence preventing
    other types from being passed to the variable. */
    int quantum_int;
    std::cin >> quantum_int;
    while ((std::cin.fail()) || (quantum_int < 1)){
        if (std::cin.fail()){
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input, please enter an integer value: ";
            std::cin >> quantum_int;
            std::cout << std::endl;
        } else {
            std::cout << "Invalid, please enter a value greater than or equal to 1: ";
            std::cin >> quantum_int;
            std::cout << std::endl;
        }
    }
    //Prevents additional parts of the buffer from being read by other cin objects
    std::cout << "Input read as: " << quantum_int << std::endl;
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << std::endl;
    return quantum_int;
}

bool confirm_yes_no_funct()
{
    /* Checks whether the first letter of an input is "Y" or "N" (not case specific) and true/false
    respectively. Validation is also performed in a similar way to the quantum integers. */
    char ask;
    std::cin >> ask;
    while ((std::cin.fail()) || !(ask == 'Y' || ask == 'y' || ask == 'N' || ask == 'n')){
        if (std::cin.fail()){
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input, please enter a single character and press ENTER: ";
            std::cin >> ask;
            std::cout << std::endl;
        } else if (!(ask == 'Y' || ask == 'y' || ask == 'N' || ask == 'n')){
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input, please enter either 'Y' or 'N' only: ";
            std::cin >> ask;
            std::cout << std::endl;
        }
    }  
    //Prevents additional parts of the buffer from being read by other cin objects
    std::cout << "Input read as: " << ask << std::endl;
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (ask == 'Y' || ask == 'y'){
        return true;
    } else if (ask == 'N' || ask == 'n'){
        return false;
    } else {
        //Extra fool-proofing/default case
        return false;
    }
}
