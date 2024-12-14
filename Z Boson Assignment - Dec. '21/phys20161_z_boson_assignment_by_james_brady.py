# -*- coding: utf-8 -*-
"""
PHYS20161: Introduction to Programming - Assignment 2 - Z Boson Data Analysis

This program is used to fit data from several particle detectors in order to
test theoretical models of particle decay, in this case the program has been
used to fit for the Z_0 boson.

Several sets of data are taken in by the program and generated using inbuilt
functions. These data sets are then filtered to remove obvious outliers (such
as data points with vastly different oders of magnitude) and any non-numerical
values. If a file cannot be read, the program moves on to the next one.
All the generated sets are then combined into one array and sorted
by energy in ascending order to allow for the plotting to be correct. This
data is then plotted to a graph as 'outliers' as any non-outlier points will
be plotted above the data points such that only the outliers of this set are
labeled as such.

From this set, an initial fit to the cross section function is performed using
fmin on the chi squared regimen and given guess values for the coefficients.
The optimal chi squared and coefficient values are returned, along with the
coefficient uncertainties and more outliers are removed if necessary.
If outliers are removed, this process iterates until no more outliers can be
removed.

After this, value of the lifetime is calculated along with its uncertainty
(which is found via a standard propagation of error) and all values with
their relevant uncertainties are printed to the console. The data and
fit function are plotted on the same axes object as the outliers and a table
of the printed values is added to the right before the figure is saved as a
.png file. LaTeX has been used to format the text and Times New Roman has been
used as the standard font for the figure.

The user is then asked whether they would like to generate a contour plot.
If so, a contour plot of the chi squared values dependent on the coefficients
is generated on a new figure and the uncertainty level is shown along with
the minimum point. This figure also uses Times New Roman and is then saved as
another, separate .png file.

ASIDE: I consider myself to have implemented the following extras:
    - Good formatting of graphs other than the default
    - Presenting additional information along with any graphs
    - Generating other graphs than the representation of the data set and fit
    function
    - Asking the user whether they want other graphs to be generated (however,
    I feel that this could be tenuous as it is a minimal addition)
    - Generalising the pre-filter condtions so that they are general to other
    data (usage of medians over data set rather than a specific condtion) and
    easy to modify using global variables (could also be tenuous as only
    one condition is treated this way, however the other conditions are very
    general and are likely to be applicable to any other data set)
    - Allowing other decay products that obey the Breit-Wigner relation to be
    used by defining a simple and easily modified Breit-Wigner function (this
    may also be tenuous as it is needed for the program to work, however it
    also could have been part of my fitting function and so would be harder
    to modify as needed).

Created on Tue Nov 23 10:21:02 2021

@author: t10997jb (James Michael Patrick Brady)
"""

# Imports
import numpy as np
from scipy.optimize import fmin
from scipy import constants
import matplotlib.pyplot as plt
import matplotlib.font_manager as font_manager

# Constants
MASS_Z_INITIAL = 90 # in GeV/c^2
PARTIAL_WIDTH_Z = 3 # in GeV
PARTIAL_WIDTH_EE = 0.08391  # in GeV
H_BAR_GEV = constants.hbar / (constants.e * (10**9)) # in GeV/Hz
CONVERSION_COEFFICIENT = 0.3894 # in millibarn
MEDIAN_LEVEL = 10 # Used to filter data
FONT = "Times New Roman" # Used to set fonts for MatPlotLib graphs

# Functions
def cross_section(energy, coefficients, partial_width_ee=\
                  PARTIAL_WIDTH_EE):
    """
    Finds the cross-section of the Z-boson conversion in nanobarns at several
    energies, using the partial width and mass of the Z-boson and the
    partial width of electron-electron conversion using a
    Breit-Wigner relation.

    Parameters
    ----------
    energy : float
    mass_z : float
    partial_width_z : float
    partial_width_ee : float, optional. The default is PARTIAL_WIDTH_EE.

    Returns
    -------
    cross_section: float
    """
    mass_z = coefficients[0]
    partial_width_z = coefficients[1]
    cross_section_calc = (12 * np.pi * partial_width_ee**2 / (mass_z**2)) *\
        (energy**2 / ((energy**2 - mass_z**2)**2 + mass_z**2 * \
                      partial_width_z**2))
    return cross_section_calc * (CONVERSION_COEFFICIENT * 10**6)

def chi_square_fitting(input_data, part_width_z=PARTIAL_WIDTH_Z, \
                       mass_z=MASS_Z_INITIAL):
    """
    Performs a least-square (chi squared) fit on the data input [independent,
    dependent,uncertainity] using the fmin function. The results [minimum chi
    squared, mass, path length] and the uncertainties [mass, path length] are
    returned.

    Parameters
    ----------
    input_data : numpy array of float values
    part_width_z : float, optional. The default is PARTIAL_WIDTH_Z.
    mass_z : float, optional. The default is MASS_Z_INITIAL.

    Returns
    -------
    results : numpy array of float values
    sigma_coeff : numpy array of float values

    """
    fit_function = lambda coefficients: chi_squared(input_data[:, 1], \
                    input_data[:, 2], cross_section(input_data[:, 0], \
                                                    coefficients))

    fit_results = fmin(fit_function, (mass_z, part_width_z), \
                   full_output=True, xtol=0.001, disp=0)
    fitted_mass_z = float(fit_results[0][0])
    fitted_part_width_z = float(fit_results[0][1])
    chi_square_min = fit_results[1]
    results_array = np.array([chi_square_min, fitted_mass_z, \
                              fitted_part_width_z])

    uncert_function = lambda coefficients: abs(chi_squared(input_data[:, 1], \
            input_data[:, 2], cross_section(input_data[:, 0], coefficients))\
            - chi_square_min - 1)
    uncert_upper_coeff = fmin(uncert_function, (1.3 * fitted_mass_z,\
            1.2 * fitted_part_width_z), full_output=True, xtol=0.0001, \
            disp=0)[0]

    uncert_lower_coeff = fmin(uncert_function, (0.7 * fitted_mass_z, \
            0.8 * fitted_part_width_z), full_output=True, xtol=0.0001, \
            disp=0)[0]

    uncertainty_coeff = np.array((uncert_upper_coeff - uncert_lower_coeff)/2)
    return results_array, uncertainty_coeff

def filter_outliers(observed_data, model_data):
    """
    Compares a set of observed data [energy,cross-section,uncertainty] to
    data [cross-section] generated by a fit function and discards any
    observations that fall outside of a 3 * uncertainty deviation from
    the fit. If an outlier is dectected, a boolean check returns True so that
    the function can be iterated in the main code until no more
    can be removed.

    Parameters
    ----------
    observed_data : numpy array of floats
    model_data : numpy array of floats

    Returns
    -------
    reduced : numpy array of floats
    check : boolean
    """
    reduced = np.zeros((0, 3))
    check = False
    for index in range(len(observed_data)):
        model_cross_section = model_data[index]
        observed_cross_section = observed_data[index, 1]
        uncertainty = observed_data[index, 2]
        if abs(observed_cross_section - model_cross_section) < 3 * uncertainty:
            temp = np.array(observed_data[index, :])
            reduced = np.vstack((reduced, temp))
        else:
            check = True
    return reduced, check

def lifetime(partial_width):
    """
    Returns the lifetime of a particle with a defined partial width. This uses
    the reduced Planck's constant in GeV.

    Parameters
    ----------
    partial_width : float

    Returns
    -------
    lifetime: float
    """
    lifetime_calc = H_BAR_GEV/partial_width
    return lifetime_calc

def chi_squared(result, uncert, prediction):
    """
    Calculates the value of the chi squared test between recorded data with
    an uncertainties and predicted data generated by a model.

    Parameters
    ----------
    result : numpy array of float values
    uncert : numpy array of float values
    prediction : numpy array of float values

    Returns
    -------
    chi_squared : float value
    """
    chi_squared_calc = np.sum(((prediction - result)**2) / (uncert**2))
    return chi_squared_calc

def read_and_filter_data(data_file_name):
    """
    Takes the name of a file and attempts to generate a numpy array using
    a comma delimiter. If the file is not found, the function halts and
    returns an empty zeros array instead. Otherwise, the data is filtered to
    remove all errant entries returning an array of form
    [energy,cross-section,uncertainty].

    The lines in the data array are filtered such that:
        - if a NaN or non-numerical value is detected in the line,
        it is rejected
        - if the cross-section is more than an order MEDIAN_LEVEL larger than
        the median cross-section, the line is rejected. MEDIAN_LEVEL can easily
        be redefined in the global variable area.
        - if a negative cross-section is recorded, the line is rejected
        - if a negative energy is recorded, the line is rejected
        - if a line has no uncertainty, it is rejected
        - if the uncertainty of a line is regative, the absolute value is taken
        instead (this could be due to a calculation order error, unlike the
                 other quantities as they are more likely to be wrong due to
                 malfunction).

    The median was chosen due to it being relatively unbiased compared to
    the mean and so is less likely to be affected by major outliers. This
    also makes the filtering more general to other sorts of data with similar
    validation issues.

    Parameters
    ----------
    data_file_name : string

    Returns
    -------
    filtered_data = 2D array of floats
    """
    filtered_data = np.zeros((0, 3))
    try:
        arr = np.genfromtxt("{}".format(data_file_name), delimiter=',', \
                dtype='float', skip_header=1)
        for line in arr:
            try:
                temp = np.array([float(line[0]), float(line[1]), \
                                 float(line[2])])
                nan_check = np.isnan(temp)
                if True in nan_check or temp[2] == 0 or temp[1] > MEDIAN_LEVEL\
                    * np.median(arr[:, 1]) or temp[1] < 0 or temp[0] < 0:
                    temp = [0, 0, 0]
                elif temp[2] < 0:
                    temp[2] = abs(temp[2])
                    filtered_data = np.vstack((filtered_data, temp))
                else:
                    filtered_data = np.vstack((filtered_data, temp))
            except ValueError:
                continue
    except FileNotFoundError:
        print("Input file not found, please check name. Returning an empty"\
              "array and executing next step.")
        filtered_data = np.zeros((0, 3))
    return filtered_data

# Will need to allow this to work for many data sets down the line
def combine_data(data_array):
    """
    Takes in a list of arrays of equal width, stacks them vertically and then
    sorts the combination with respect to the arguments of the first column.
    This method has been made to be scalable for many data sets, not just
    the two provided for the assignment.

    Parameters
    ----------
    data_array: List of 2D arrays of floats

    Returns
    -------
    combined_data : 2D array of floats
    """
    temp = np.zeros((0, 3))
    for array in data_array:
        temp = np.vstack((temp, array))
    temp_sort = temp[temp[:, 0].argsort()]
    return temp_sort

def plotting_and_values(obs_data, fit_data, fit_results, fit_uncert):
    """
    Plots the fitted data alongside the fitted data from the cross section
    function (using the values at minimum) and then prints the values of the
    measurements and coefficients required with their uncertainties.
    A table of the parameters with their values and uncertainties is then
    appended to the figure. This figure is then saved as a .png file.

    Parameters
    ----------
    obs_data : numpy array of floats
    fit_data: numpy array of floats
    fit_results : numpy array of floats
    fit_uncert : numpy array of floats

    Returns
    -------
    0 - indicates success, as output data isn't needed
    """

    ax.errorbar(obs_data[:, 0], obs_data[:, 1], obs_data[:, 2], \
    label="Filtered Data from Final Fit", linestyle="none", color="green")
    ax.plot(obs_data[:, 0], fit_data, alpha=1, color="grey",\
            linestyle="dashed", label="Fit Function")
    min_chi_square = fit_results[0]
    min_chi_square_red = min_chi_square/(len(obs_data)-2)
    fit_mass = fit_results[1]
    fit_mass_uncert = abs(fit_uncert[0])
    fit_part_width = fit_results[2]
    fit_part_width_uncert = abs(fit_uncert[1])
    fit_lifetime = lifetime(results[2])
    lifetime_uncert = abs((fit_part_width_uncert / fit_part_width)\
                          * fit_lifetime)
    print("Minimum Reduced Chi Squared: {0:.3g}, Fitted Mass: {1:.4g} +-"\
          " {2:.4g} GeV/c^2, Fitted Path Length: {3:.4g} +- {4:.4g} GeV,"\
        " Lifetime: {5:.3g} +- {6:.3g} s".format(min_chi_square_red, fit_mass,\
        fit_mass_uncert, fit_part_width, fit_part_width_uncert, fit_lifetime,\
            lifetime_uncert))
    ax.set_title('Cross-Sections of Z-Boson Conversions to Electron-Positron'\
                 ' Pairs', fontfamily="Times New Roman")
    ax.set_xlabel('Energy (GeV)', fontfamily="Times New Roman")
    ax.set_ylabel('Cross-Section (nano barns)', fontfamily="Times New Roman")
    for tick in ax.get_xticklabels():
        tick.set_fontname(FONT)
    for tick in ax.get_yticklabels():
        tick.set_fontname(FONT)
    plt.tight_layout()
    ax.legend(loc="center left", bbox_to_anchor=(1, 0.86), prop=font_manager.\
            FontProperties(family=FONT))
    plt.rcParams["font.family"] = FONT
    columns = ["Parameters", "Optimal Value", "Uncertainty"]
    table_data = [[r"$\chi^2_{{\mathrm{{red}}}}$",\
             "{:.3g}".format(min_chi_square_red), "N/A"], \
            [r"$\Gamma$, (GeV)", "{:.4g}".format(fit_part_width),\
              "{:.4g}".format(fit_part_width_uncert)],\
            [r"$m_{{\mathrm{{Z^{{0}}}}}}$ (GeV/$\mathrm{{c^2}})$", \
             "{:.4g}".format(fit_mass), "{:.4g}".format(fit_mass_uncert)],\
            [r"$\tau$ (s)", "{:.3g}".format(fit_lifetime), \
             "{:.3g}".format(lifetime_uncert)]]
    table = ax.table(cellText=table_data, colLabels=columns, \
                     bbox=(1.02, 0, 0.8, 0.7), rowLoc="center")
    table.auto_set_font_size(False)
    table.set_fontsize(12)
    plt.savefig("Cross Section from Z-Boson Data.png", dpi=600, \
                bbox_inches="tight")
    plt.show()
    return 0

def contour_plotting(input_data, fit_results):
    """
    Creates a contour plot of the chi-squared fitting regime around the
    minimum value calculated previously in the program. This figure is
    then saved as a .png file.

    Parameters
    ----------
    input_data : numpy array of floats
    fit_results : numpy array of floats

    Returns
    -------
    0 - indicates success, as output data isn't needed
    """
    mass_range = 0.1
    path_length_range = 0.1
    masses_z = np.linspace(fit_results[1] - mass_range, \
                           fit_results[1] + mass_range, 100)
    path_lengths_z = np.linspace(fit_results[2] - path_length_range,\
                    fit_results[2] + path_length_range, 100)
    mass_mesh = np.empty((0, 100))
    path_length_mesh = np.empty((0, 100))

    for i in enumerate(masses_z):
        path_length_mesh = np.vstack((path_length_mesh, path_lengths_z))

    for i in enumerate(path_lengths_z):
        mass_mesh = np.vstack((mass_mesh, masses_z))

    mass_mesh = np.transpose(mass_mesh)
    chi_squared_mesh = np.zeros((0, len(mass_mesh[:, 0])))
    for j in range(len(mass_mesh[:, 0])):
        temp = np.zeros(0)
        for i in range(len(mass_mesh[0, :])):
            chi_value = chi_squared(input_data[:, 1], input_data[:, 2],\
            cross_section(input_data[:, 0], ((mass_mesh[i, j], \
                                                path_length_mesh[i, j]))))
            temp = np.hstack((temp, chi_value))
        chi_squared_mesh = np.vstack((chi_squared_mesh, temp))

    fig_2 = plt.figure(1)
    ax_2 = fig_2.add_subplot(111)
    chi_min = results[0]
    contour = ax_2.contour(path_length_mesh, mass_mesh, chi_squared_mesh, 10)
    contour.clabel()
    uncert_contour = ax_2.contour(path_length_mesh, mass_mesh, \
                chi_squared_mesh, levels=[chi_min + 1],\
                linestyles="dashed", colors="black")
    uncert_contour.collections[0].set_label(\
                    r" $\chi^2_{{\mathrm{{min}}}}$ + 1")
    ax_2.scatter(results[2], results[1], label=\
                 r"$\chi^2_{{\mathrm{{min}}}}$ : {:.3g}".format(results[0]))
    ax_2.set_xlabel(r"Path Length, $\Gamma$, (GeV)", \
                    fontfamily=FONT)
    ax_2.set_ylabel(r"Mass, $m_{{\mathrm{{Z^{{0}}}}}}$ (GeV/$\mathrm{{c^2}})$"\
                    , fontfamily=FONT)
    ax_2.set_title(r"Contour Plot of $\chi^2_{{\mathrm{{min}}}}$"\
                   , fontfamily=FONT)
    for tick in ax_2.get_xticklabels():
        tick.set_fontname(FONT)
    for tick in ax_2.get_yticklabels():
        tick.set_fontname(FONT)
    ax_2.legend(loc="center left", bbox_to_anchor=(1, 0.89), \
                prop=font_manager.FontProperties(family=FONT))
    plt.tight_layout()
    plt.savefig("Z-Boson Data Fit Contour.png", dpi=600)
    plt.show()
    return 0

# Main Code
data_1 = read_and_filter_data("z_boson_data_1.csv")
data_2 = read_and_filter_data("z_boson_data_2.csv")
data_sets = [data_1, data_2]
combined_data = combine_data(data_sets)

results, uncert_coeff = chi_square_fitting(combined_data)
fitted_coeff = np.array([results[1], results[2]])

fig = plt.figure(0)
ax = fig.add_subplot(111)
energies = combined_data[:, 0]
cross_sections = combined_data[:, 1]
uncerts = combined_data[:, 2]

# Any points that are not outliers will later be obscured by another plot
ax.errorbar(energies, cross_sections, uncerts, \
            label="Outliers", linestyle="none",\
                color="red")

results, uncert_coeff = chi_square_fitting(combined_data)
fitted_coeff = np.array([results[1], results[2]])
cross_section_fit = cross_section(combined_data[:, 0], fitted_coeff)
reduced_data, outlier_check = filter_outliers(combined_data, cross_section_fit)

while outlier_check:
    results, uncert_coeff = chi_square_fitting(reduced_data, \
                            part_width_z=results[2], mass_z=results[1])
    fitted_coeff = np.array([results[1], results[2]])
    cross_section_fit = cross_section(reduced_data[:, 0], fitted_coeff)
    reduced_data, outlier_check = filter_outliers(reduced_data, \
                                                  cross_section_fit)
plotting_and_values(reduced_data, cross_section_fit, results, uncert_coeff)

contour_ask = input("Would you like to generate a contour plot (Y/N)?   ")
if contour_ask in ("Y", "y"):
    contour_plotting(reduced_data, results)
