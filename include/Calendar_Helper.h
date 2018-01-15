#ifndef CALENDARHELPER_H
#define CALENDARHELPER_H

#include <time.h>
#include <ctype.h>

////////////////////////
//	Print Functions   //
////////////////////////

char * toStringEvent(List list);

char * toStringAlarm(List list);

char * toStringProp(List list);

////////////////////////
// Validate Functions //
////////////////////////

ICalErrorCode validateEvent(List list);

bool validateAlarm(List list);

bool checkDigit(char * string);

char * toLower(char * string);

bool checkPropertyElse(char * name, int eaFlag);

bool checkPropertyCal(char * string);

/* My Functions */

char* printCalendarFile(const Calendar* obj);

const char * checkExtension(const char *filename);

char * cleanString( char * string );

void freeIndices(char ** fileSave, int length);

void UI();

ICalErrorCode createCalendarCustom(char* fileName, Calendar** obj);

//~~~~~~~~~~~~~~~~~ List Functions ~~~~~~~~~~~~~~~~~~

char* printProps(void *toBePrinted);

int compareProps(const void *first, const void *second);

void deleteProps(void *toBeDeleted);

char* printAlarms(void *toBePrinted);

int compareAlarms(const void *first, const void *second);

void deleteAlarms(void *toBeDeleted);

char* printEvents(void *toBePrinted);

int compareEvents(const void *first, const void *second);

void deleteEvents(void *toBeDeleted);

//~~~~~~~~~~~~~~~~~ Tkinter Structs ~~~~~~~~~~~~~~~~~~

//Represents iCalendar Date-time
typedef struct es {
	// # of Properties
	int numProps; 
	// # of Alarms
	int numAlarms; 
	// Summary
	char summary[4000];  
} EventStats;

//~~~~~~~~~~~~~~~~~ Tkinter Functions ~~~~~~~~~~~~~~~~~~

char * findSummary(List list);

char * printAlarmUI(void *toBePrinted);

char * printListProp(List list);

char * printAlarmListProp(List list);

char * printProp(void *toBePrinted);

char * printEA(int index, Calendar * cal);

char * printEP(int index, Calendar * cal);

void addAlarmToEventAtIndex(int index, Calendar ** cal, char * action, char * trigger);

//~~~~~~~~~~~~~~~~~~~~~ SQL Functions ~~~~~~~~~~~~~~~~~~~~

char * getSOLSdtNa(int index, Calendar * cal);

char * findOrganizer(List list);

char * findLocation(List list);

#endif
