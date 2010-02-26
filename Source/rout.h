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
 * \param jd The current time in Julian Date
 * \param r The current rocket state to write
 */
void PrintStateLine(FILE *outfile, double jd, state r);

/*!
 * Prints the header line of an output file
 * \param outfile The file to write to
 */
void PrintHeader(FILE *outfile);

/*!
 * Prints a line in the output file that tracks forces
 * \param outfile The file to write to
 * \param t The current time in mission elapsed time in seconds
 * \param Jd The time in Julian Date
 * \param rocket The current state
 */
void PrintForceLine(FILE *outfile, double jd, state r);

/*!
 * Prints the results of an entire simulation to the screen.
 * It gets it's initial state and time information from orbit.h
 * \param burnout A rocket State at time of burnout.
 * \param apogee A rocket State at time of apogee.
 * \param t_bo The time at burnout.
 * \param t_apogee The time at apogee.
 */
void PrintSimResult();
void PrintKmlHeader(FILE *outfile);
void PrintKmlFooter(FILE *outfile);
void PrintKmlLine(FILE *outfile, state r);
void PrintHtmlResult(Rocket_Stage *stages);
void MakePltFiles(Rocket_Stage finalStage);
void DumpState(state dump);
void DumpDescription(stageDesc desc);

