/*! 
 * \file rout.h
 * \brief Handles output
 *  
 * Has several functions called by the main program to assist in writting 
 * output both to the screen and to file.
 */

/*!
 * Prints a line of rocekt state to a file
 * \param outfile The file to write to
 * \param Mjd The time in Modified Julian Date
 * \param t The current time in seconds
 * \param r The current rocket state to write
 * \param t The current time in seconds
 */
void PrintLine(FILE *outfile, double Mjd, double t, state r);

/*!
 * Prints the header line of an output file
 * \param outfile The file to write to
 */
void PrintHeader(FILE *outfile);

/*!
 * Prints the results of an entire simulation to the screen.
 * It gets it's initial state and time information from orbit.h
 * \param burnout A rocket State at time of burnout.
 * \param apogee A rocket State at time of apogee.
 * \param t_bo The time at burnout.
 * \param t_apogee The time at apogee.
 */
void PrintResult(state burnout, state apogee, double t_bo, double t_apogee);
void PrintKmlHeader(FILE *outfile);
void PrintKmlFooter(FILE *outfile);
void PrintKmlLine(FILE *outfile, state r);
void PrintHtmlResult(state burnout, state apogee, double t_bo, double t_apogee);
