/**
 * Assignment 2: Simple Course Database
 * Written by: James Michael Patrick Brady
 * 
 * Program Overview:
 * This program takes a .dat file containing data about the Physics Department's
 * modules in the form {grade, course code, name} and manipulates it.
 * 
 * Firstly, the user is asked for the name of the file containing the data (or path,
 * if not in the same local folder) and performs validation to confirm that it exists.
 * An fstream object is then returned and interpreted to copy the data stored in
 * the file into a series of vectors (grade, years, course title), using a stringstream
 * object to combine the course code and name into one course title string.
 * 
 * Next, the user is asked for a selection of years they would like to view the modules
 * for. This is done by getting an int input from the user, validating and comparing
 * to the year vector created at interpretation to return a vector of indices. These
 * indices correspond to the relevant positions in the other vectors and are used to
 * limit the selection to vectors of those courses and grades only.
 * 
 * The user is then asked how they would like their selection to be sorted: by course code
 * (using the "PHYSxxxxx" part of the title) or by course name (using the name part of the
 * title). Validation is done for the input and then an index argsort is performed using the
 * data, lambdas and custom comparison functions.
 * 
 * Finally, this sorted selection is output to the user as a command-line table and statistical
 * analysis is performed on the grades in the selection (finding the mean, standard deviation
 * and standard error) and returned to the user in the terminal.
 * 
 * Most functions used in this program are void-type and pass variables by reference to both
 * improve efficiency but also to allow for multiple variables to be modified at once without
 * having to deal with single returns and reassignments.
 **/

#include <iostream>
#include <iomanip> //Required for clearing iostreams
#include <fstream> //Required to open and interpret files
#include <string>
#include <cmath>
#include <vector>
#include <algorithm> //Required for sorting

//Input and file processing functions
void get_file(std::fstream& file_data);
void interpret_file(std::fstream& file_data, std::vector<double>& grades, std::vector<std::string>& course_titles, std::vector<int>& years);
void selection_menu(std::vector<int>& years, std::vector<int>& selection_indices);

//Data processing functions
void sorting(std::vector<double>& grades, std::vector<std::string>& course_titles, std::vector<int>& indices);
bool course_code_sort(std::string& a, std::string& b);
bool course_name_sort(std::string& a, std::string& b);
void sort_from_indices(std::vector<double>& grades, std::vector<int>& indices);
void sort_from_indices(std::vector<std::string>& course_titles, std::vector<int>& indices);

//Output and data analysis functions
void output(std::vector<double>& selection_grades, std::vector<std::string>& selection_titles);
void grade_statistics(std::vector<double>& grades);
double grade_mean(std::vector<double>& grades, int& length);
double grade_stand_dev(std::vector<double>& grades, double& mean, int& length);
double grade_stand_err(double& stand_dev, int&length);

int main()
{
  //Initialising storage and getting the file
  std::fstream data_stream;
  get_file(data_stream);

  //Interpreting the file and packing into vectors for analysis and output
  std::vector<double> grades(0);
  std::vector<std::string> course_titles(0);
  std::vector<int> years(0);
  interpret_file(data_stream, grades, course_titles, years);  
  std::cout << grades.size() << " courses found." << std::endl;
  std::cout << std::endl;

  /* Selecting modules according to year and returning postions in above 
  vectors as a vector<int> of indices */
  std::vector<int> selection_indices(0);
  selection_menu(years, selection_indices);
  std::cout << std::endl;
  
  std::vector<double> selection_grades(0);
  std::vector<std::string> selection_titles(0);

  //Reduces selection to requested entries only and corrects indices to the shorter vectors
  if(selection_indices.size() != years.size()){
    std::vector<int>::iterator index_first{selection_indices.begin()};
    std::vector<int>::iterator index_last{selection_indices.end()};
    std::vector<int>::iterator index_current;
    std::cout << "Courses selected from Year " << years[0] << "." << std::endl;
    int first_index{*index_first};
    /* Iterates over each index in the selection, pushes back the relevant grades and
    titles to new vectors and then corrects the indices to positions in the new vectors */
    for(index_current = index_first; index_current < index_last; ++index_current){
      std::cout << grades[*index_current] << " " << course_titles[*index_current] << std::endl;
      selection_grades.push_back(grades[*index_current]);
      selection_titles.push_back(course_titles[*index_current]);
      *index_current -= first_index;
    }
  } else{
    //Clearly, no iteration or corrections are needed here as everything is selected
    std::cout << "Courses selected from all years." << std::endl;
    selection_grades = grades;
    selection_titles = course_titles;
  }

  std::cout << selection_grades.size() << " courses found in selection." << std::endl;
  std::cout << std::endl;
  //Performs an argsort on the grade and course name vectors
  sorting(selection_grades, selection_titles, selection_indices);
  std::cout << std::endl;
  //Outputs the grades and courses as a table and performs the required statistical analysis.
  output(selection_grades, selection_titles);
  return 0;
}

void get_file(std::fstream& file_data)
{
  /* Declaring an fstream in advance allows me to not have to deal with an awkward return */
  bool file_read_success{false};
  std::string file_name{""};
  while (!file_read_success){
    std::cout << "Enter the filename or path of your .dat file: ";
    std::getline(std::cin, file_name);
    file_data.open(file_name);
    if (!file_data.good()){
      std::cout << "File or file directory '" << file_name << "' does not exist." << std::endl;
      std::cout << std::endl;
    } else{
      std::cout << "File found successfully!" << std::endl;
      file_read_success = true;
    }
  }
}

void interpret_file(std::fstream& file_data, std::vector<double>& grades, std::vector<std::string>& course_titles, std::vector<int>& years)
{
  /* I deliberately create the vectors before calling the function and then pass them by reference so
  that I can store both the grades and the course titles using the same function rather than having
  to specify two functions with unique return types. */
  double grade{0.0};
  int course_code{0};
  std::string course_name{""};
  std::ostringstream string_convert("");

  while(!file_data.eof()){
    file_data >> grade;
    grades.push_back(grade);
    file_data >> course_code;
    int year{course_code/10000};
    years.push_back(year);
    getline(file_data, course_name);
    //Required to remove stray spaces left over from reading
    course_name.erase(0,2);
    //Using a stringstream to combine code & name, as specified in the brief
    string_convert.str(std::string(""));
    string_convert << "PHYS" << course_code << course_name;
    std::string course_title{string_convert.str()};
    course_titles.push_back(course_title);
  }
  //Closing the filestream as soon as I'm finished with it, because I passed by reference
  file_data.close();

  //This is a workaround for the problems the newline causes, because trying to ignore the newline didn't work
  grades.pop_back();
  years.pop_back();
  course_titles.pop_back();

}

void selection_menu(std::vector<int>& years, std::vector<int>& selection_indices)
{
  /* Selection menu for chhosing which years' courses you want to limit analysis to.
  The benefit of using ints is that I can use the years as previously found as the
  selection parameters and hence comparison is slightly simpler. */
  int selection{0};
  std::cout << "Please select the year you would like to view: " << std::endl;
  std::cout << "0 - All Years " << std::endl;
  std::cout << "1 - First Year " << std::endl;
  std::cout << "2 - Second Year " << std::endl;
  std::cout << "3 - Third Year " << std::endl;
  std::cout << "4 - Fourth Year " << std::endl;
  std::cout << "Enter the number corresponding to your selection: ";
  std::cin >> selection;

  //Basic validation for input
  while ((std::cin.fail()) || (selection < 0) || (selection > 4)){
    if (std::cin.fail()){
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      std::cout << "Invalid input, please enter an integer value: ";
      std::cin >> selection;
      std::cout << std::endl;
    } else {
        std::cout << "Invalid, please enter a value between (and including) 0 and 4: ";
        std::cin >> selection;
        std::cout << std::endl;
    }
  }
  //Clears iostreams
  std::cin.clear();
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

  //Extracting positions of selection in data vectors for a later argsort implementation
  std::vector<int>::iterator selection_begin{years.begin()};
  std::vector<int>::iterator selection_end{years.end()};
  std::vector<int>::iterator selection_iterator;
  for(selection_iterator = selection_begin; selection_iterator < selection_end; ++selection_iterator){
    int index = distance(selection_begin, selection_iterator);
    if(selection != 0){
      if(*selection_iterator == selection){
        selection_indices.push_back(index);
      } else {
        continue;
      }
    } else{
      selection_indices.push_back(index);
    }
  }
}

void sorting(std::vector<double>& grades, std::vector<std::string>& course_titles, std::vector<int>& indices)
{
  /* Because my data is set up into separate vectors of different types (because C++ does not allow multi-dimensional
  vectors of more than one type and pair objects seem a bit awkward to use), I instead perform an argsort over the
  indices of the vectors according to either the course title or course name and pass elements being sorted via lambdas
  to custom sort functions. It may not be simpler than using pairs, but at least I know how to do it this way.*/
  std::string selection{""};
  std::cout << "Your selection can be sorted by typing in the following words: " << std::endl;
  std::cout << "- Type 'code' (without quotes) to sort by course code" << std::endl;
  std::cout << "- Type 'name' (without quotes) to sort by course name" << std::endl;
  std::cout << "Enter the word corresponding to your selection: ";
  std::cin >> selection;
  for(int i = 0; i < selection.size(); i++){
    selection[i] = std::tolower(selection[i]);
  }

  while ((std::cin.fail()) || !(selection == "code" || selection == "name")){
    if (std::cin.fail()){
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      std::cout << "Invalid input, please enter a valid string ";
      std::cin >> selection;
      for(int i = 0; i < selection.size(); i++){
        selection[i] = std::tolower(selection[i]);
      }
      std::cout << std::endl;
    } else {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid, please enter either 'code' or 'title': ";
        std::cin >> selection;
        for(int i = 0; i < selection.size(); i++){
          selection[i] = std::tolower(selection[i]);
        }
        std::cout << std::endl;
    }
  }
  std::cin.clear();
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

  //Argsorts the indices via a sort on the data based on the data selection above 
  if(selection == "code"){
    //Sorts by course code
    std::sort(indices.begin(), indices.end(), [&](const size_t i, const size_t j){return course_code_sort(course_titles[i], course_titles[j]);});
  } else{
    //Sorts by course title (the non-'PHYSxxxxx' part)
    std::sort(indices.begin(), indices.end(), [&](const size_t i, const size_t j){return course_name_sort(course_titles[i], course_titles[j]);});
  }
  /* The data is then sorted using indices so I can sort both vectors the same way rather than having
   a single sorted vector and a unpaired, unsorted vector BECAUSE I'm also outputting the grades. */
  sort_from_indices(grades, indices);
  sort_from_indices(course_titles, indices);
}

bool course_code_sort(std::string& a, std::string& b)
{
  /* Gets the course codes from the vector being used in the sort */
  std::string code_a{a.substr(4,8)};
  std::string code_b{b.substr(4,8)};
  return code_a < code_b;
}

bool course_name_sort(std::string& a, std::string& b)
{
  /* Gets the course names from the vector being used in the sort */
  std::string name_a{a.substr(10)};
  std::string name_b{b.substr(10)};
  /* Somebody pointed out to me that capitals rank lower in ASCII than lowercase,
   hence names with multiple capitals might be out of order. These loops prevent that
   from occuring by making comparisons in lowercase only. */
  for(int i = 0; i < name_a.size(); i++){
    name_a[i] = std::tolower(name_a[i]);
  } for(int i = 0; i < name_b.size(); i++){
    name_b[i] = std::tolower(name_b[i]);
  }
  return name_a < name_b;
}

void sort_from_indices(std::vector<double>& grades, std::vector<int>& indices)
{
  /* Uses the result of the argsort to sort the grade vector using iterators */
  std::vector<double> grades_copy{grades};
  std::vector<int>::iterator copy_begin{indices.begin()};
  std::vector<int>::iterator copy_end{indices.end()};
  std::vector<int>::iterator copy_iterator;
  std::vector<double>::iterator temp_iterator{grades.begin()};
  for(copy_iterator = copy_begin; copy_iterator < copy_end; copy_iterator++){
    *temp_iterator = grades_copy[*copy_iterator];
    temp_iterator++;
  }
}

void sort_from_indices(std::vector<std::string>& course_titles, std::vector<int>& indices)
{
  /* Overloaded to sort the course title vector, rather than defining a new named function */
  std::vector<std::string> titles_copy{course_titles};
  std::vector<int>::iterator copy_begin{indices.begin()};
  std::vector<int>::iterator copy_end{indices.end()};
  std::vector<int>::iterator copy_iterator;
  std::vector<std::string>::iterator temp_iterator{course_titles.begin()};
  for(copy_iterator = copy_begin; copy_iterator < copy_end; copy_iterator++){
    *temp_iterator = titles_copy[*copy_iterator];
    temp_iterator++;
  }
}

void output(std::vector<double>& selection_grades, std::vector<std::string>& selection_titles)
{
  /* Outputs the sorted selection of courses and then performs the required statistics on the
  selected courses' grades */
  std::cout << "Grade   |" << "Course" << std::endl;
  std::cout << "---------------" << std::endl;
  std::vector<double>::iterator grades_begin{selection_grades.begin()};
  std::vector<double>::iterator grades_end{selection_grades.end()};
  std::vector<double>::iterator grades_iterator;
  std::vector<std::string>::iterator titles_iterator{selection_titles.begin()};
  //Iterates over the data vectors to output the table in a pleasing way
  for(grades_iterator = grades_begin; grades_iterator < grades_end; grades_iterator++){
    double modulo{std::fmod(*grades_iterator, 1)};
    //Workaround for when the double grade does not have a decimal point - this keeps presentation neat
    if(modulo == 0){
      std::cout << std::setprecision(3) << *grades_iterator << "      |" << *titles_iterator << std::endl;
      titles_iterator++;
    } else{
      std::cout << std::setprecision(3) << *grades_iterator << "    |" << *titles_iterator << std::endl;
      titles_iterator++;
    }
  }
  std::cout << "---------------" << std::endl;
  std::cout << std::endl;
  grade_statistics(selection_grades);
}

void grade_statistics(std::vector<double>& grades)
{
  /* Performs and outputs the statistical analysis of the selected grades */
  int length{grades.size()};
  double mean_grade = grade_mean(grades, length);
  double grade_deviation = grade_stand_dev(grades, mean_grade, length);
  double mean_grade_error = grade_stand_err(grade_deviation, length);

  std::cout << "The mean grade is " << std::setprecision(3) << mean_grade << " +- " << mean_grade_error << " %." << std::endl;
  std::cout << "The standard deviation of the grades is " << grade_deviation << " %." << std::endl;
}

double grade_mean(std::vector<double>& grades, int& length)
{
  /* Calculates the mean of the grades using iterators */
  double sum{0};
  std::vector<double>::iterator grade_begin{grades.begin()};
  std::vector<double>::iterator grade_end{grades.end()};
  std::vector<double>::iterator grade_iterator;
  for(grade_iterator = grade_begin; grade_iterator < grade_end; grade_iterator++){
    sum += *grade_iterator;
  }
  return double{sum/length};
}

double grade_stand_dev(std::vector<double>& grades, double& mean, int& length)
{
  /* Calculates the standard deviation of the grades using iterators */
  double difference_sum{0};
  std::vector<double>::iterator grade_begin{grades.begin()};
  std::vector<double>::iterator grade_end{grades.end()};
  std::vector<double>::iterator grade_iterator;
  for(grade_iterator = grade_begin; grade_iterator < grade_end; grade_iterator++){
    difference_sum += pow((*grade_iterator - mean), 2);
  }
  return double{pow((difference_sum/(length - 1)), 0.5)};
}

double grade_stand_err(double& stand_dev, int&length)
{
  /* Returns the standard error, STD/sqrt(N) of the mean grade */
  return double{stand_dev * pow(length, -0.5)};
}