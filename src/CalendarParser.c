#include "CalendarParser.h"
#include "Calendar_Helper.h"

#define BUFF 765678735 

//                                                                      //
//                ICalendar Parsing Static/Dynamic Library                  //
//                         Jmacph04  -   0878619                        //

/* Assume properties less than 75 characters are on 1 line */

/* 

    ***ICalendar class notes***

    Program scope:  ~variables are a bad idea to give external scope to (just like global variables are a bad idea)
                    ~Narrow Scope is always the goal
                    ~functions having larger scope isn't a bad thing (helper functions)
                    ~for functions that you don't want the user to use, make them static
                    ~C doesn't allow variable shadowing
*/

/*

Here are some of the rules:

~The first word on each line is the property name
~The property name will be followed by a colon (:) or a semicolon (;)
~If it is a colon then the property value will be directly to the right of the content to the
 end of the line
~A further layer of complexity is added here as a comma separated list of values are allowed
 that would then be stored in an array. So the CATEGORIES one for example would have 3
 elements in an array for the values
~If after the property name a semi colon is there, then there are optional parameters that
 follow
~The optional parameter format is ParamName=ParamValue. Again a comma separated list is
 supported here.
~There can be more than one optional parameter as seen on the ORGANIZER line. There would
 just be another semicolon followed by the next parameter and value.
~And to throw in yet another wrench, quotations are allowed in the values. If something is
 in quotes for the value it would need to be treated as part of the value instead of being
 part of the syntax. So a semicolon in a quotation would not mean that there is another
 parameter it would be part of the value.

*/

bool predicate(const void* first, const void* second){
    
    Property * tmpProp1;
    Property * tmpProp2;
    
    if (first == NULL || second == NULL){
        return false;
    }
    
    tmpProp1 = (Property *)first;
    tmpProp2 = (Property *)second;
    
    if(strcmp((char*)tmpProp1->propName, (char*)tmpProp2->propName) == 0){
        return true;
    }
    return false;
}

////////////////////////
// Database Functions //
////////////////////////

char * getSOLSdtNa(int index, Calendar * cal)
{
    ListIterator iter = createIterator(cal->events);
    
    Event * event;

    int i = 0;
    while( i < index ){
        event = (Event*)nextElement(&iter);
        i++;
    }
    char * summary = findSummary(event->properties);
    char * organizer = findOrganizer(event->properties);
    char * location = findLocation(event->properties);
    char sdt[26];
    sdt[0] = '\0';
    sprintf(sdt, "%s|DT|%s|DT|", event->startDateTime.date, event->startDateTime.time);
    char * EventStats = malloc(sizeof(char)*strlen(summary)+100+strlen(organizer)+strlen(location)+strlen(sdt));
    EventStats[0] = '\0';
    int numAlarms = 0;
    numAlarms = getLength(event->alarms);
    sprintf(EventStats, "%s|&&&|%s|&&&|%s|&&&|%s|&&&|%d|&&&|", summary, organizer, location, sdt, numAlarms);

    return EventStats;
}

char * findOrganizer(List list){
    char * string;
    int listLen = getLength(list);
    Property * search = malloc(sizeof(Property));
    strcpy(search->propName, "ORGANIZER");
    if( listLen > 0 )
    {
        Property * prop = findElement(list, &predicate, search);
        if(prop != NULL){
            string = malloc(sizeof(char)*strlen(prop->propDescr)+1);
            string = prop->propDescr;
        }
        else{
            string = malloc(10);
            string = "";
        }
    }
    else{
        string = malloc(10);
        string = "";
    }
    free(search);
    return string;
}

char * findLocation(List list){
    char * string;
    int listLen = getLength(list);
    Property * search = malloc(sizeof(Property));
    strcpy(search->propName, "LOCATION");
    if( listLen > 0 )
    {
        Property * prop = findElement(list, &predicate, search);
        if(prop != NULL){
            string = malloc(sizeof(char)*strlen(prop->propDescr)+1);
            string = prop->propDescr;
        }
        else{
            string = malloc(10);
            string = "";
        }
    }
    else{
        string = malloc(10);
        string = "";
    }
    free(search);
    return string;
}

///////////////////////
// Tkinter Functions //
///////////////////////

int numberOfEvents(Calendar * cal){
    return getLength(cal->events);
}

void addAlarmToEventAtIndex(int index, Calendar ** cal, char * action, char * trigger)
{
    ListIterator iter = createIterator((*cal)->events);
    
    Event * event;
    int i = 0;
    while( i < index ){
        event = (Event*)nextElement(&iter);
        i++;
    }
    Alarm * alarm = malloc(sizeof(Alarm));
    strcpy(alarm->action, action);
    alarm->trigger = malloc(strlen(trigger) + 1);
    strcpy(alarm->trigger, trigger);

    alarm->properties = initializeList(&printProps, &deleteProps, &compareProps);

    insertBack(&(event->alarms), alarm);
}

// Delete Event at Index
void eventDelAtIndex(int index, Calendar ** cal)
{

    ListIterator iter = createIterator((*cal)->events);
    
    Event * event;

    int i = 0;
    while( i < index ){
        event = (Event*)nextElement(&iter);
        i++;
    }
    Event * toDel;
    toDel = deleteDataFromList(&((*cal)->events), event);
    deleteEvents(toDel);
}

// Return Event Stats at Index
char * eventStatsAtIndex(int index, Calendar * cal)
{
    ListIterator iter = createIterator(cal->events);
    
    Event * event;

    int i = 0;
    while( i <= index ){
        event = (Event*)nextElement(&iter);
        i++;
    }
    char * summary = findSummary(event->properties);
    char * EventStats = malloc(sizeof(char)*strlen(summary)+100);
    int numProps = 0;
    int numAlarms = 0;
    numProps = getLength(event->properties);
    numAlarms = getLength(event->alarms);
    sprintf(EventStats, "%d|&&&|%d|&&&|%s|&&&|", numProps, numAlarms, summary);

    return EventStats;
}

char * findSummary(List list){
    char * string;
    int listLen = getLength(list);
    Property * search = malloc(sizeof(Property));
    strcpy(search->propName, "SUMMARY");
    if( listLen > 0 )
    {
        Property * prop = findElement(list, &predicate, search);
        if(prop != NULL){
            string = malloc(sizeof(char)*strlen(prop->propDescr)+1);
            string = prop->propDescr;
        }
        else{
            string = malloc(10);
            string = "";
        }
    }
    else{
        string = malloc(10);
        string = "";
    }
    free(search);
    return string;
}

// Fill Created Calendar Object
void fillCal(char * prodID, Calendar ** cal)
{
    *cal = malloc(sizeof(Calendar));
    (*cal)->version = 2.0;
    strcpy((*cal)->prodID, prodID);
    (*cal)->events = initializeList(&printEvents, &deleteEvents, &compareEvents);
    (*cal)->properties = initializeList(&printProps, &deleteProps, &compareProps);
}

// Fill Created Event Object
void fillCalEvent(char * event, Calendar ** cal)
{
    char UID[1000];
    char startString[20];
    char createString[20];
    char alarmString[1500];
    char properties[1500];

    char * token;

    /* get the first token */
    char delim[] = "^";
    token = strtok(event, delim);
    // Get UID
    if(token != NULL){
        strcpy(UID, token);
    }
    token = strtok(NULL, delim);

    // Get create
    if(token != NULL){
        strcpy(createString, token);
    }
    token = strtok(NULL, delim);
    // Get start
    if(token != NULL){
        strcpy(startString, token);
    }
    token = strtok(NULL, delim);
    // Get alarm
    if(token != NULL){
        strcpy(alarmString, token);
    }
    token = strtok(NULL, delim);
    // Get properties
    if(token != NULL){
        strcpy(properties, token);
    }

    Event * eventObj = malloc(sizeof(Event));
    strcpy(eventObj->UID, UID);
    /////////////////////////////////////
    DateTime create;
    // strtok create string
    char * createToken;
    /* get the create date token */
    createToken = strtok(createString, "~");
    strcpy(create.date, createToken);
    // token time
    if(createToken != NULL){
        createToken = strtok(NULL, "~");
        strcpy(create.time, createToken);
    }
    // token UTC
    if(createToken != NULL){
        createToken = strtok(NULL, "~");
        create.UTC = atoi(createToken);
    }
    eventObj->creationDateTime = create;
    ////////////////////////////////////// 
    DateTime start;
    // strtok alarm string
    char * startToken;
    /* get the alarm date token */
    startToken = strtok(startString, "~");
    strcpy(start.date, startToken);
    // token time
    if(startToken != NULL){
        startToken = strtok(NULL, "~");
        strcpy(start.time, startToken);
    }
    // token UTC
    if(startToken != NULL){
        startToken = strtok(NULL, "~");
        start.UTC = atoi(startToken);
    }
    eventObj->startDateTime = start;
    //////////////////////////////////////
    eventObj->alarms = initializeList(&printAlarms, &deleteAlarms, &compareAlarms);
    Alarm * alarm;
    // strtok alarm list string
    char * alarmListToken;

    char * data = alarmString;
    alarmListToken  = strchr(data, '|');
    while(alarmListToken != NULL){
        alarm = malloc(sizeof(Alarm));

        *alarmListToken++ = '\0';

        char * alarmToken= strtok(data, "~");

        strcpy(alarm->action, alarmToken);
        
        alarmToken = strtok(NULL, "~");
        
        alarm->trigger = malloc(sizeof(alarmToken)+1);
        strcpy(alarm->trigger, alarmToken);
        
        data = alarmListToken;
        alarmListToken = strchr(data, '|');
        alarm->properties = initializeList(&printProps, &deleteProps, &compareProps);

        insertBack(&(eventObj->alarms), alarm);
    }
    eventObj->properties = initializeList(&printProps, &deleteProps, &compareProps);
    insertBack(&((*cal)->events), eventObj);
}

// Fill Created Alarm Object
void fillEventAlarm(char * action, char * trigger, Calendar ** cal)
{
    Alarm * alarmObject = malloc(sizeof(Alarm));
    strcpy(alarmObject->action, action);
    alarmObject->trigger = malloc(strlen(trigger) + 1);
    strcpy(alarmObject->trigger, trigger);
    alarmObject->properties = initializeList(&printProps, &deleteProps, &compareProps);
}

// Fill Created Property Object
Property * fillProperty(char * name, char * description, Calendar ** cal)
{
    Property * property = malloc(sizeof(Property) + strlen(description) + 1);
    strcpy(property->propName, name);
    strcpy(property->propDescr, description);
    return property;   
}
/*
###########################
# FUNCTIONS FOR LOG PANEL #
###########################
*/
// PROPS

char * printPropUI(void *toBePrinted){

    char* tmpStr;
    Property * tmpProp;
    int len;
    
    if (toBePrinted == NULL){
        return NULL;
    }
    
    tmpProp = (Property *)toBePrinted;
    
    len = strlen(tmpProp->propName)+strlen(tmpProp->propDescr)+100;
    tmpStr = malloc(sizeof(char)*len);
    sprintf(tmpStr, "\t\t%s:%s\n", tmpProp->propName, tmpProp->propDescr);
    return tmpStr;
}

char * printListProp(List list)
{
    int length = 0;
    Node * tmp = list.head;
    char * string;
    while( tmp != NULL )
    {
        string = printPropUI(tmp->data);
        length = length + strlen(string) + 1;
        free(string);
        tmp = tmp->next;
    } 
    char * listString = malloc(sizeof(char)*length+15);
    strcpy(listString, "Properties:\n\0");
    Node * tmp2 = list.head;
    char * tPtr;
    while( tmp2 != NULL )
    {
        tPtr = printPropUI(tmp2->data);
        length = length + strlen(tPtr) + 1;
        listString = realloc(listString, length);
        strcat(listString, tPtr);
        tmp2 = tmp2->next;
        free(tPtr);
    }
    return listString;
}

// ALARMS

char * printAlarmListProp(List list)
{
    int length = 0;
    Node * tmp = list.head;
    char * string;
    while( tmp != NULL )
    {
        string = printPropUI(tmp->data);
        length = length + strlen(string) + 1;
        free(string);
        tmp = tmp->next;
    } 
    char * listString = malloc(sizeof(char)*length+30);
    strcpy(listString, "Alarm Properties:\n\0");
    Node * tmp2 = list.head;
    char * tPtr;
    while( tmp2 != NULL )
    {
        tPtr = printPropUI(tmp2->data);
        length = length + strlen(tPtr) + 1;
        listString = realloc(listString, length);
        strcat(listString, tPtr);
        tmp2 = tmp2->next;
        free(tPtr);
    }
    return listString;
}

char* printAlarmUI(void *toBePrinted){
    
    char* tmpStr;
    Alarm * tmpAlarm;
    char * lenList;
    int len;
    
    if (toBePrinted == NULL){
        return NULL;
    }
    
    tmpAlarm = (Alarm *)toBePrinted;

    len = strlen(tmpAlarm->action) + strlen(tmpAlarm->trigger) + 100;
    if( getLength(tmpAlarm->properties) > 0 ){
        lenList = printAlarmListProp(tmpAlarm->properties);
        len = len + strlen(lenList) + 1;
    }
    tmpStr = malloc(sizeof(char)*len);
    
    sprintf(tmpStr, "Alarm:\n\t\tACTION:%s\n\t\tTRIGGER:%s\n", tmpAlarm->action, tmpAlarm->trigger);

    if( getLength(tmpAlarm->properties) > 0 ){
        strcat(tmpStr, lenList);
        free(lenList);
    }

    return tmpStr;
}

char * printListAlarm(List list)
{
    int length = 0;
    Node * tmp = list.head;
    char * string;
    while( tmp != NULL )
    {
        string = printAlarmUI(tmp->data);
        length = length + strlen(string) + 1;
        free(string);
        tmp = tmp->next;
    } 
    char * listString = malloc(sizeof(char)*length);
    listString[0] = '\0';
    Node * tmp2 = list.head;
    int flag = 0;
    char * tPtr;
    while( tmp2 != NULL )
    {
        tPtr = printAlarmUI(tmp2->data);
        length = length + strlen(tPtr) + 1;
        listString = realloc(listString, length);
        if(flag == 0){
            strcpy(listString, tPtr);
        }
        else{
            strcat(listString, tPtr);
        }
        tmp2 = tmp2->next;
        free(tPtr);
        flag = 1;
    }
    return listString;
}

char * printEA(int index, Calendar * cal){
    if(getLength(cal->events) < 1){
        return "";
    }
    ListIterator iter = createIterator(cal->events);
    Event * event = (Event*)nextElement(&iter);
    int i = 0;
    while(event != NULL && i != index-1){
        event = (Event*)nextElement(&iter);
        i++;
    }
    if(getLength(event->alarms) < 1){
        return "";
    }
    char * string = printListAlarm(event->alarms);
    return string;
}

char * printEP(int index, Calendar * cal){
    if(getLength(cal->events) < 1){
        return "";
    }
    ListIterator iter = createIterator(cal->events);
    Event * event = (Event*)nextElement(&iter);
    int i = 0;
    while(event != NULL && i != index-1){
        event = (Event*)nextElement(&iter);
        i++;
    }
    if(getLength(event->properties) < 1){
        return "";
    }
    char * string = printListProp(event->properties);
    return string;
}

/*
char * returnAlarmAtIndex(int index, Event * event)
{
    Alarm * alarm = event->alarms.head;
    int i;
    for(i = 0; i < index; i++){
        alarm = alarm->next
    }
    char * string = printEvent(alarm);
    return string;
}
*/


// ~~~~~~~~~~~~~~~~~ UI ~~~~~~~~~~~~~~~~~~

void UI(){

    printf("-----------------------\n");
    printf("Read Calendar File (r)\n");
    printf("Display Calendar File (p)\n");
    printf("Create Calendar Object (c)\n");
    printf("Write Calendar to File (w)\n");
    printf("Quit (q)\n");
    
    char selection;
    selection = getchar();
    
    // Declare Cal
    Calendar * ICal = NULL;
    char file[200];
    bool uiFlag = true;
    bool ICalFlag = false;
    // Command Loop
    while(uiFlag){

        if( selection == 'r' || selection == 'p' || selection == 'c' || selection == 'w' || selection == 'q'){

            // Declare Cal
          //  Calendar * ICal;
          //  Calendar * ICalCustom;

            if( selection == 'r' ){
                while(ICalFlag){
                    printf("---------------------------------\n");
                    printf("~~~~~~~ICalendar Object already read~~~~~~~\n~~Would you like to read a new file?~~\n");
                    printf("                 Yes (y)\n");
                    printf("                 No (n)\n");             
                    char propSelect;
                    getchar();
                    propSelect = getchar();
                    while( propSelect != 'y' && propSelect != 'n'){
                        printf("---------------------------------\n");
                        printf("Invalid input\nPlease try again\n");
                        getchar();
                        propSelect = getchar();
                    }
                    if( propSelect == 'y' ){
                        deleteCalendar(ICal);
                        ICal = NULL;
                        ICalFlag = false;
                    }
                    else{
                        printf("---------------------------------\n");
                        printf("Returning you to Main Menu :)\n");
                    }
                }
                if(!ICalFlag){
                    // Get File Name
                    printf("-----------------------\n");
                    printf("~Input File Name Below~\n");
                    printf("-> ");

                    scanf("%s" , file);

                    char * errorCodePrint;

                    errorCodePrint = printError( createCalendar(file, &ICal) );

                    printf("%s\n", errorCodePrint);

                    if( strcmp(errorCodePrint,"OK") == 0 ){
                        ICalFlag = true;
                        free(errorCodePrint);
                    }
                    else if( strcmp(errorCodePrint,"OK") != 0 ){
                        free(errorCodePrint);
                        ICal = NULL;
                        while( strcmp(errorCodePrint,"OK") != 0 ){
                            printf("---------------------------------\n");
                            printf("~~Would you like to try reading a new file?~~\n");
                            printf("                 Yes (y)\n");
                            printf("                 No (n)\n");
                            char propSelect;
                            getchar();
                            propSelect = getchar();
                            while( propSelect != 'y' && propSelect != 'n'){
                                printf("---------------------------------\n");
                                printf("Invalid input\nPlease try again\n");
                                getchar();
                                propSelect = getchar();
                            }
                            if( propSelect == 'y' ){
                                // Get File Name
                                printf("-----------------------\n");
                                printf("~Input File Name Below~\n");
                                printf("-> ");

                                scanf("%s" , file);

                                errorCodePrint = printError( createCalendar(file, &ICal) );

                                printf("%s\n", errorCodePrint);

                                free(errorCodePrint);
                            }
                            else{
                                printf("---------------------------------\n");
                                printf("Returning you to Main Menu :)\n");
                                break;
                            }
                        }
                    }
                    printf("-----------------------\n");
                }
            }
            else if( selection == 'p' ){
                ///// PRINT /////
                if( ICal != NULL ){
                    char * string = printCalendar(ICal);
                    printf("%s\n", string);
                    free(string);

                    //deleteCalendar(ICal);
                }
                else{
                    printf("-----------------------\n");
                    printf("No Calendar File Read\n");
                    printf("-----------------------\n");
                }
            }
            else if( selection == 'c' ){
                // Check for ICAL Object
                while(ICalFlag){
                    printf("---------------------------------\n");
                    printf("~~~~~~~ICalendar Object already read~~~~~~~\n~~Would you like to read a new file?~~\n");
                    printf("                 Yes (y)\n");
                    printf("                 No (n)\n");             
                    char propSelect;
                    getchar();
                    propSelect = getchar();
                    while( propSelect != 'y' && propSelect != 'n'){
                        printf("---------------------------------\n");
                        printf("Invalid input\nPlease try again\n");
                        getchar();
                        propSelect = getchar();
                    }
                    if( propSelect == 'y' ){
                        deleteCalendar(ICal);
                        ICal = NULL;
                        ICalFlag = false;
                    }
                    else{
                        printf("---------------------------------\n");
                        printf("Returning you to Main Menu :)\n");
                    }
                }
                if(!ICalFlag){
                    ///// Create Calendar Object /////

                    // Get File Name
                    printf("---------------------------------\n");
                    printf("~Input Desired File Name Below~\n");
                    printf("-> ");

                    scanf("%s" , file);

                    //Check for Invalid File
                    const char * tok = checkExtension(file);
                    while( strcmp(tok, "ics") != 0)
                    {
                        printf("---------------------------------\n");
                        printf("~Invalid File Name, Please Try Again~\n");
                        printf("-> ");
                        scanf("%s" , file);
                        tok = checkExtension(file);
                    }

                    // Valid File
                    printf("---------------------------------\n");
                    printf(" Filename will be %s \n", file);
                    printf("---------------------------------\n");

                    ICal = malloc(sizeof(Calendar));

                    // Set version to version 2
                    printf("\n");
                    printf("~Calendar Version Automatically set to '2.0'~\n");
                    printf("\n");

                    ICal->version = 2;

                    // Get Product ID
                    printf("---------------------------------\n");
                    printf("~Input Product ID Below~\n");
                    printf("-> ");

                    char calProdid[1000];

                    scanf("%s" , calProdid);

                    strcpy(ICal->prodID, calProdid);

                    printf("---------------------------------\n");
                    printf(" ProdID set to: %s  \n", calProdid);

                    // Initialize Calendar Properties List
                    List customCalProperties = initializeList(&printProps, &deleteProps, &compareProps);

                    bool PropUIFlag = true;

                    while(PropUIFlag){
                        printf("---------------------------------\n");
                        printf("~~Create a new Calendar Property?~~\n");
                        printf("           Yes (y)\n");
                        printf("           No (n)\n");
                        char propSelect;
                        getchar();
                        propSelect = getchar();
                        while( propSelect != 'y' && propSelect != 'n'){
                            printf("---------------------------------\n");
                            printf("Invalid input\nPlease try again\n");
                            getchar();
                            propSelect = getchar();
                        }
                        if( propSelect == 'y' ){

                            // Get Event Property
                            printf("---------------------------------\n");
                            printf("~Input Property Name Below~\n");
                            printf("-> ");

                            char propName[50];

                            scanf("%s" , propName);

                            printf("---------------------------------\n");
                            printf("~Input Property Description Below~\n");
                            printf("-> ");

                            char propDescr[100];

                            scanf("%s" , propDescr);

                            int propDescrLen = strlen(propDescr) + 1;

                            Property * Prop = malloc(sizeof(Property)+(sizeof(char)*propDescrLen));

                            strcpy(Prop->propName, propName);

                            strcpy(Prop->propDescr, propDescr);

                            insertBack(&customCalProperties, Prop);

                            printf("---------------------------------\n");
                            printf(" Property set to: %s:%s  \n", propName, propDescr);
                        }
                        else{
                            PropUIFlag = false;
                            ICal->properties = customCalProperties;
                        }
                    }

                    // Calendar Creation Option
                    bool eventUIFlag = true;
                    bool firstEvent = true;
                    List eventsList;
                    while(eventUIFlag){
                        if(firstEvent){
                            eventsList = initializeList(&printEvents, &deleteEvents, &compareEvents);
                            firstEvent = false;

                            // Create EVENT
                            // Get UID
                            printf("---------------------------------\n");
                            printf("~Input First Event UID Below~\n");
                            printf("-> ");

                            char UID[200];

                            scanf("%s" , UID);

                            Event * event = malloc(sizeof(Event));

                            strcpy(event->UID, UID);

                            printf("---------------------------------\n");
                            printf(" First Event UID set to: %s  \n", UID);

                            // Get DateTime
                            DateTime DT;

                            // Get Date
                            time_t t = time(NULL);
                            struct tm tm = *localtime(&t);

                            sprintf(DT.date, "%d", tm.tm_year + 1900);

                            char buf[3];
                            if( tm.tm_mon + 1 < 10 ){
                                sprintf( buf, "0%d", tm.tm_mon + 1);
                                strcat(DT.date, buf);
                            }
                            else{
                                sprintf( buf, "%d", tm.tm_mon + 1);
                                strcat(DT.date, buf);
                            }
                            if( tm.tm_mday < 10 ){
                                sprintf( buf, "0%d", tm.tm_mday);
                                strcat(DT.date, buf);
                            }
                            else{
                                sprintf( buf, "%d", tm.tm_mday);
                                strcat(DT.date, buf);
                            }

                            // Get UTC
                            printf("---------------------------------\n");
                            printf("         ~ UTC Time? ~\n");
                            printf("            Yes (y)\n");
                            printf("            No (n)\n");

                            char utcSelect;
                            getchar();
                            utcSelect = getchar();
                            while( utcSelect != 'y' && utcSelect != 'n'){
                                printf("---------------------------------\n");
                                printf("Invalid input\nPlease try again\n");
                                getchar();
                                utcSelect = getchar();
                            }
                            if( utcSelect == 'y' ){
                                tm = *gmtime(&t);
                                if( tm.tm_hour < 10 ){
                                sprintf( buf, "0%d", tm.tm_hour);
                                strcpy(DT.time, buf);
                                }
                                else{
                                    sprintf( buf, "%d", tm.tm_hour);
                                    strcpy(DT.time, buf);
                                }
                                if( tm.tm_min < 10 ){
                                sprintf( buf, "0%d", tm.tm_min);
                                strcat(DT.time, buf);
                                }
                                else{
                                    sprintf( buf, "%d", tm.tm_min);
                                    strcat(DT.time, buf);
                                }
                                if( tm.tm_sec < 10 ){
                                sprintf( buf, "0%d", tm.tm_sec);
                                strcat(DT.time, buf);
                                }
                                else{
                                    sprintf( buf, "%d", tm.tm_sec);
                                    strcat(DT.time, buf);
                                }
                                DT.UTC = 1;
                                printf("---------------------------------\n");
                                printf(" DateTime set to: %sT%sZ \n", DT.date, DT.time);
                            }else{
                                sprintf(DT.time, "%d%d%d", tm.tm_hour, tm.tm_min, tm.tm_sec);
                                if( tm.tm_hour < 10 ){
                                sprintf( buf, "0%d", tm.tm_hour);
                                strcpy(DT.time, buf);
                                }
                                else{
                                    sprintf( buf, "%d", tm.tm_hour);
                                    strcpy(DT.time, buf);
                                }
                                if( tm.tm_min < 10 ){
                                sprintf( buf, "0%d", tm.tm_min);
                                strcat(DT.time, buf);
                                }
                                else{
                                    sprintf( buf, "%d", tm.tm_min);
                                    strcat(DT.time, buf);
                                }
                                if( tm.tm_sec < 10 ){
                                sprintf( buf, "0%d", tm.tm_sec);
                                strcat(DT.time, buf);
                                }
                                else{
                                    sprintf( buf, "%d", tm.tm_sec);
                                    strcat(DT.time, buf);
                                }
                                DT.UTC = 0;
                                printf("---------------------------------\n");
                                printf(" DateTime set to: %sT%s \n", DT.date, DT.time);
                            }

                            event->creationDateTime = DT;

                            // Event Property Creation Option
                            bool eventPropUIFlag = true;
                            bool firstEventProp = true;
                            List eventPropsList;
                            eventPropsList = initializeList(&printProps, &deleteProps, &compareProps);
                            while(eventPropUIFlag){
                                printf("---------------------------------\n");
                                printf("~~Create a new Event Property?~~\n");
                                printf("           Yes (y)\n");
                                printf("           No (n)\n");
                                char propSelect;
                                getchar();
                                propSelect = getchar();
                                while( propSelect != 'y' && propSelect != 'n'){
                                    printf("---------------------------------\n");
                                    printf("Invalid input\nPlease try again\n");
                                    getchar();
                                    propSelect = getchar();
                                }
                                if( propSelect == 'y' ){

                                    if(firstEventProp){
                                        //eventPropsList = initializeList(&printProps, &deleteProps, &compareProps);
                                        firstEventProp = false;
                                    }

                                    // Get Event Property
                                    printf("---------------------------------\n");
                                    printf("~Input Property Name Below~\n");
                                    printf("-> ");

                                    char propName[50];

                                    scanf("%s" , propName);

                                    printf("---------------------------------\n");
                                    printf("~Input Property Description Below~\n");
                                    printf("-> ");

                                    char propDescr[100];

                                    scanf("%s" , propDescr);

                                    int propDescrLen = strlen(propDescr) + 1;

                                    Property * Prop = malloc(sizeof(Property)+(sizeof(char)*propDescrLen));

                                    strcpy(Prop->propName, propName);

                                    strcpy(Prop->propDescr, propDescr);

                                    insertBack(&eventPropsList, Prop);

                                    printf("---------------------------------\n");
                                    printf(" Property set to: %s:%s  \n", propName, propDescr);

                                }
                                else{
                                    eventPropUIFlag = false;
                                    event->properties = eventPropsList;
                                }
                            }

                            // Event Alarm Creation Option
                            bool eventAlarmUIFlag = true;
                            bool firstEventAlarm = true;
                            List eventAlarmsList;
                            eventAlarmsList = initializeList(&printAlarms, &deleteAlarms, &compareAlarms);
                            while(eventAlarmUIFlag){
                                printf("---------------------------------\n");
                                printf("~~Create a new Event Alarm?~~\n");
                                printf("          Yes (y)\n");
                                printf("          No (n)\n");
                                char alarmSelect;
                                getchar();
                                alarmSelect = getchar();
                                while( alarmSelect != 'y' && alarmSelect != 'n'){
                                    printf("---------------------------------\n");
                                    printf("Invalid input\nPlease try again\n");
                                    getchar();
                                    alarmSelect = getchar();
                                }
                                if( alarmSelect == 'y' ){

                                    if(firstEventAlarm){
                                        //eventAlarmsList = initializeList(&printAlarms, &deleteAlarms, &compareAlarms);
                                        firstEventAlarm = false;
                                    }

                                    // Get Event Alarm
                                    Alarm * alarm = malloc(sizeof(Alarm));

                                    printf("---------------------------------\n");
                                    printf("~Input Alarm Action Below~\n");
                                    printf("-> ");

                                    char alarmAction[100];

                                    scanf("%s" , alarmAction);

                                    strcpy(alarm->action, alarmAction);

                                    printf("---------------------------------\n");
                                    printf(" Alarm Action set to: %s  \n", alarmAction);

                                    printf("---------------------------------\n");
                                    printf("~Input Alarm Trigger Below~\n");
                                    printf("-> ");

                                    char alarmTrigger[100];

                                    scanf("%s" , alarmTrigger);

                                    int triggerLen = strlen(alarmTrigger) + 1;

                                    alarm->trigger = malloc(sizeof(char)*triggerLen);

                                    strcpy(alarm->trigger, alarmTrigger);

                                    insertBack(&eventAlarmsList, alarm);

                                    printf("---------------------------------\n");
                                    printf(" Alarm Trigger set to: %s  \n", alarmTrigger);

                                    // Alarm Property Creation Option
                                    bool alarmPropUIFlag = true;
                                    bool firstAlarmProp = true;
                                    List alarmPropsList;
                                    alarmPropsList = initializeList(&printProps, &deleteProps, &compareProps);
                                    while(alarmPropUIFlag){
                                        printf("---------------------------------\n");
                                        printf("~~Create a new Alarm Property?~~\n");
                                        printf("           Yes (y)\n");
                                        printf("           No (n)\n");
                                        char propSelect;
                                        getchar();
                                        propSelect = getchar();
                                        while( propSelect != 'y' && propSelect != 'n'){
                                            printf("---------------------------------\n");
                                            printf("Invalid input\nPlease try again\n");
                                            getchar();
                                            propSelect = getchar();
                                        }
                                        if( propSelect == 'y' ){

                                            if(firstAlarmProp){
                                                //alarmPropsList = initializeList(&printProps, &deleteProps, &compareProps);
                                                firstAlarmProp = false;
                                            }

                                            // Get Event Property
                                            printf("---------------------------------\n");
                                            printf("~Input Property Name Below~\n");
                                            printf("-> ");

                                            char propName[50];

                                            scanf("%s" , propName);

                                            printf("---------------------------------\n");
                                            printf("~Input Property Description Below~\n");
                                            printf("-> ");

                                            char propDescr[100];

                                            scanf("%s" , propDescr);

                                            int propDescrLen = strlen(propDescr) + 1;

                                            Property * Prop = malloc(sizeof(Property)+(sizeof(char)*propDescrLen));

                                            strcpy(Prop->propName, propName);

                                            strcpy(Prop->propDescr, propDescr);

                                            insertBack(&alarmPropsList, Prop);

                                            printf("---------------------------------\n");
                                            printf(" Property set to: %s:%s  \n", propName, propDescr);

                                        }
                                        else{
                                            alarmPropUIFlag = false;
                                            alarm->properties = alarmPropsList;
                                        }
                                    }
                                }
                                else{
                                    eventAlarmUIFlag = false;
                                    event->alarms = eventAlarmsList;
                                }
                            }
                            insertBack(&eventsList, event);
                        }
                        else{
                            printf("---------------------------------\n");
                            printf("~~Create a new Event?~~\n");
                            printf("       Yes (y)\n");
                            printf("       No (n)\n");

                            char eventSelect;
                            getchar();
                            eventSelect = getchar();
                            while( eventSelect != 'y' && eventSelect != 'n'){
                                printf("---------------------------------\n");
                                printf("Invalid input\nPlease try again\n");
                                getchar();
                                eventSelect = getchar();
                            }
                            if( eventSelect == 'y' ){

                                // Create EVENT
                                Event * event = malloc(sizeof(Event));

                                // Get DateTime
                                DateTime DT;

                                // Get Date
                                time_t t = time(NULL);
                                struct tm tm = *localtime(&t);

                                sprintf(DT.date, "%d", tm.tm_year + 1900);

                                char buf[3];
                                if( tm.tm_mon + 1 < 10 ){
                                    sprintf( buf, "0%d", tm.tm_mon + 1);
                                    strcat(DT.date, buf);
                                }
                                else{
                                    sprintf( buf, "%d", tm.tm_mon + 1);
                                    strcat(DT.date, buf);
                                }
                                if( tm.tm_mday < 10 ){
                                    sprintf( buf, "0%d", tm.tm_mday);
                                    strcat(DT.date, buf);
                                }
                                else{
                                    sprintf( buf, "%d", tm.tm_mday);
                                    strcat(DT.date, buf);
                                }

                                // Get UTC
                                printf("---------------------------------\n");
                                printf("         ~ UTC Time? ~\n");
                                printf("            Yes (y)\n");
                                printf("            No (n)\n");

                                char utcSelect;
                                getchar();
                                utcSelect = getchar();
                                while( utcSelect != 'y' && utcSelect != 'n'){
                                    printf("---------------------------------\n");
                                    printf("Invalid input\nPlease try again\n");
                                    getchar();
                                    utcSelect = getchar();
                                }
                                if( utcSelect == 'y' ){
                                    tm = *gmtime(&t);
                                    if( tm.tm_hour < 10 ){
                                    sprintf( buf, "0%d", tm.tm_hour);
                                    strcpy(DT.time, buf);
                                    }
                                    else{
                                        sprintf( buf, "%d", tm.tm_hour);
                                        strcpy(DT.time, buf);
                                    }
                                    if( tm.tm_min < 10 ){
                                    sprintf( buf, "0%d", tm.tm_min);
                                    strcat(DT.time, buf);
                                    }
                                    else{
                                        sprintf( buf, "%d", tm.tm_min);
                                        strcat(DT.time, buf);
                                    }
                                    if( tm.tm_sec < 10 ){
                                    sprintf( buf, "0%d", tm.tm_sec);
                                    strcat(DT.time, buf);
                                    }
                                    else{
                                        sprintf( buf, "%d", tm.tm_sec);
                                        strcat(DT.time, buf);
                                    }
                                    DT.UTC = 1;
                                    printf("---------------------------------\n");
                                    printf(" DateTime set to: %sT%sZ \n", DT.date, DT.time);
                                }else{
                                    if( tm.tm_hour < 10 ){
                                    sprintf( buf, "0%d", tm.tm_hour);
                                    strcpy(DT.time, buf);
                                    }
                                    else{
                                        sprintf( buf, "%d", tm.tm_hour);
                                        strcpy(DT.time, buf);
                                    }
                                    if( tm.tm_min < 10 ){
                                    sprintf( buf, "0%d", tm.tm_min);
                                    strcat(DT.time, buf);
                                    }
                                    else{
                                        sprintf( buf, "%d", tm.tm_min);
                                        strcat(DT.time, buf);
                                    }
                                    if( tm.tm_sec < 10 ){
                                    sprintf( buf, "0%d", tm.tm_sec);
                                    strcat(DT.time, buf);
                                    }
                                    else{
                                        sprintf( buf, "%d", tm.tm_sec);
                                        strcat(DT.time, buf);
                                    }
                                    DT.UTC = 0;
                                    printf("---------------------------------\n");
                                    printf(" DateTime set to: %sT%s \n", DT.date, DT.time);
                                }

                                event->creationDateTime = DT;

                                // Event Property Creation Option
                                bool eventPropUIFlag = true;
                                bool firstEventProp = true;
                                List eventPropsList;
                                eventPropsList = initializeList(&printProps, &deleteProps, &compareProps);
                                while(eventPropUIFlag){
                                    printf("---------------------------------\n");
                                    printf("~~Create a new Event Property?~~\n");
                                    printf("           Yes (y)\n");
                                    printf("           No (n)\n");
                                    char propSelect;
                                    getchar();
                                    propSelect = getchar();
                                    while( propSelect != 'y' && propSelect != 'n'){
                                        printf("---------------------------------\n");
                                        printf("Invalid input\nPlease try again\n");
                                        getchar();
                                        propSelect = getchar();
                                    }
                                    if( propSelect == 'y' ){

                                        if(firstEventProp){
                                            //eventPropsList = initializeList(&printProps, &deleteProps, &compareProps);
                                            firstEventProp = false;
                                        }

                                        // Get Event Property
                                        printf("---------------------------------\n");
                                        printf("~Input Property Name Below~\n");
                                        printf("-> ");

                                        char propName[50];

                                        scanf("%s" , propName);

                                        printf("---------------------------------\n");
                                        printf("~Input Property Description Below~\n");
                                        printf("-> ");

                                        char propDescr[100];

                                        scanf("%s" , propDescr);

                                        int propDescrLen = strlen(propDescr) + 1;

                                        Property * Prop = malloc(sizeof(Property)+(sizeof(char)*propDescrLen));

                                        strcpy(Prop->propName, propName);

                                        strcpy(Prop->propDescr, propDescr);

                                        insertBack(&eventPropsList, Prop);

                                        printf("---------------------------------\n");
                                        printf(" Property set to: %s:%s  \n", propName, propDescr);

                                    }
                                    else{
                                        eventPropUIFlag = false;
                                        event->properties = eventPropsList;
                                    }
                                }

                                // Event Alarm Creation Option
                                bool eventAlarmUIFlag = true;
                                bool firstEventAlarm = true;
                                List eventAlarmsList;
                                eventAlarmsList = initializeList(&printAlarms, &deleteAlarms, &compareAlarms);
                                while(eventAlarmUIFlag){
                                    printf("---------------------------------\n");
                                    printf("~~Create a new Event Alarm?~~\n");
                                    printf("          Yes (y)\n");
                                    printf("          No (n)\n");
                                    char alarmSelect;
                                    getchar();
                                    alarmSelect = getchar();
                                    while( alarmSelect != 'y' && alarmSelect != 'n'){
                                        printf("---------------------------------\n");
                                        printf("Invalid input\nPlease try again\n");
                                        getchar();
                                        alarmSelect = getchar();
                                    }
                                    if( alarmSelect == 'y' ){

                                        if(firstEventAlarm){
                                            //eventAlarmsList = initializeList(&printAlarms, &deleteAlarms, &compareAlarms);
                                            firstEventAlarm = false;
                                        }

                                        // Get Event Alarm
                                        Alarm * alarm = malloc(sizeof(Alarm));

                                        printf("---------------------------------\n");
                                        printf("~Input Alarm Action Below~\n");
                                        printf("-> ");

                                        char alarmAction[100];

                                        scanf("%s" , alarmAction);

                                        strcpy(alarm->action, alarmAction);

                                        printf("---------------------------------\n");
                                        printf(" Alarm Action set to: %s  \n", alarmAction);

                                        printf("---------------------------------\n");
                                        printf("~Input Alarm Trigger Below~\n");
                                        printf("-> ");

                                        char alarmTrigger[100];

                                        scanf("%s" , alarmTrigger);

                                        int triggerLen = strlen(alarmTrigger) + 1;

                                        alarm->trigger = malloc(sizeof(char)*triggerLen);

                                        strcpy(alarm->trigger, alarmTrigger);

                                        insertBack(&eventAlarmsList, alarm);

                                        printf("---------------------------------\n");
                                        printf(" Alarm Trigger set to: %s  \n", alarmTrigger);

                                        // Alarm Property Creation Option
                                        bool alarmPropUIFlag = true;
                                        bool firstAlarmProp = true;
                                        List alarmPropsList;
                                        alarmPropsList = initializeList(&printProps, &deleteProps, &compareProps);
                                        while(alarmPropUIFlag){
                                            printf("---------------------------------\n");
                                            printf("~~Create a new Alarm Property?~~\n");
                                            printf("           Yes (y)\n");
                                            printf("           No (n)\n");
                                            char propSelect;
                                            getchar();
                                            propSelect = getchar();
                                            while( propSelect != 'y' && propSelect != 'n'){
                                                printf("---------------------------------\n");
                                                printf("Invalid input\nPlease try again\n");
                                                getchar();
                                                propSelect = getchar();
                                            }
                                            if( propSelect == 'y' ){

                                                if(firstAlarmProp){
                                                    //alarmPropsList = initializeList(&printProps, &deleteProps, &compareProps);
                                                    firstAlarmProp = false;
                                                }

                                                // Get Event Property
                                                printf("---------------------------------\n");
                                                printf("~Input Property Name Below~\n");
                                                printf("-> ");

                                                char propName[50];

                                                scanf("%s" , propName);

                                                printf("---------------------------------\n");
                                                printf("~Input Property Description Below~\n");
                                                printf("-> ");

                                                char propDescr[100];

                                                scanf("%s" , propDescr);

                                                int propDescrLen = strlen(propDescr) + 1;

                                                Property * Prop = malloc(sizeof(Property)+(sizeof(char)*propDescrLen));

                                                strcpy(Prop->propName, propName);

                                                strcpy(Prop->propDescr, propDescr);

                                                insertBack(&alarmPropsList, Prop);

                                                printf("---------------------------------\n");
                                                printf(" Property set to: %s:%s  \n", propName, propDescr);

                                            }
                                            else{
                                                alarmPropUIFlag = false;
                                                alarm->properties = alarmPropsList;
                                            }
                                        }
                                    }
                                    else{
                                        eventAlarmUIFlag = false;
                                        event->alarms = eventAlarmsList;
                                    }
                                }
                                insertBack(&eventsList, event);
                            }
                            else{
                                eventUIFlag = false;
                                ICal->events = eventsList;
                            }
                        }
                    }

                    printf("~~~~~~~~~~~~~~~~~\n");

                    const char * errorCodePrint;

                    errorCodePrint = printError( createCalendarCustom(file, &ICal) );
                    if( strcmp(errorCodePrint,"OK") == 0 ){
                        printf("       %s\n", errorCodePrint);
                    }
                    else{
                        printf(" %s\n", errorCodePrint);
                    }
                    if( strcmp(errorCodePrint,"OK") == 0 ){
                        ICalFlag = true;
                    }

                    free((void*)errorCodePrint);

                    printf("~~~~~~~~~~~~~~~~~\n");
                }
            }
            else if( selection == 'w' ){

                if( ICal == NULL || strlen(file) == 0 ){
                    printf("-----------------------\n");
                    printf("  No Current Calendar\n");
                    printf("-----------------------\n");
                }
                else{

                    char fileWrite[200];
                    strcpy(fileWrite, file);

                    char * errorCodePrint = printError( writeCalendar(fileWrite, ICal) );

                    ///// Write Calendar Object /////
                    printf("-----------------------\n");
                    printf("   %s\n", errorCodePrint);
                    printf("-----------------------\n");

                    free(errorCodePrint);
                }
            }
            else if( selection == 'q' ){
                ///// Quit /////
                printf("-----------------------\n");
                printf("    Bye for now :)\n");
                printf("-----------------------\n");
                if( ICalFlag == true )
                {
                    ///// DELETE /////
                    deleteCalendar(ICal);
                }
                uiFlag = false;
            }
        }
        else{
            printf("---------------------------------\n");
            printf("Invalid input\nPlease try again\n");
            getchar();
            selection = getchar();
        }
        if(uiFlag){
            printf("Read Calendar File (r)\n");
            printf("Display Calendar File (p)\n");
            printf("Create Calendar Object (c)\n");
            printf("Write Calendar to File (w)\n");
            printf("Quit (q)\n");
            getchar();
            selection = getchar();
        }
    }

}  

ICalErrorCode createCalendarCustom(char* fileName, Calendar** obj)
{   
    if(fileName == NULL || strcmp(fileName,"") == 0)
        return INV_FILE;
    if(obj == NULL)
        return INV_CAL;
    else
    {
        const char * tok = checkExtension(fileName);
        if( strcmp(tok, "ics") != 0 )
        {
            return INV_FILE;
        }
        return OK;
    }
}

/* Header specific Functions */

//printFunc will return a string that contains a humanly readable copy of the list contents
char* printProps(void *toBePrinted){

    char* tmpStr;
    Property * tmpProp;
    int len;
    
    if (toBePrinted == NULL){
        return NULL;
    }
    
    tmpProp = (Property *)toBePrinted;
    
    len = strlen(tmpProp->propName)+strlen(tmpProp->propDescr)+10;
    tmpStr = malloc(sizeof(char)*len);
    sprintf(tmpStr, "%s:%s\r\n", tmpProp->propName, tmpProp->propDescr);
    return tmpStr;
}

//We compare names by last name
int compareProps(const void *first, const void *second){
    
    Property * tmpProp1;
    Property * tmpProp2;
    
    if (first == NULL || second == NULL){
        return 0;
    }
    
    tmpProp1 = (Property *)first;
    tmpProp2 = (Property *)second;
    
    return strcmp((char*)tmpProp1->propName, (char*)tmpProp2->propName);
}

//We need to free the first and last names, and then the Name struct itself
void deleteProps(void *toBeDeleted){
    
    Property * tmpProp;
    
    if (toBeDeleted == NULL){
        return;
    }
    
    tmpProp = (Property *)toBeDeleted;
    
    free(tmpProp);
}

//printFunc will return a string that contains a humanly readable copy of the list contents
char* printAlarms(void *toBePrinted){
    
    char* tmpStr;
    Alarm * tmpAlarm;
    char * lenList;
    int len;
    
    if (toBePrinted == NULL){
        return NULL;
    }
    
    tmpAlarm = (Alarm *)toBePrinted;

    len = strlen(tmpAlarm->action) + strlen(tmpAlarm->trigger) + 100;
    if( getLength(tmpAlarm->properties) > 0 ){
        lenList = toStringProp(tmpAlarm->properties);
        len = len + strlen(lenList) + 1;
    }
    tmpStr = malloc(sizeof(char)*len);
    
    sprintf(tmpStr, "BEGIN:VALARM\r\nACTION:%s\r\nTRIGGER;%s\r\n", tmpAlarm->action, tmpAlarm->trigger);

    if( getLength(tmpAlarm->properties) > 0 ){
        strcat(tmpStr, lenList);
        free(lenList);
    }

    strcat(tmpStr, "END:VALARM\r\n");

    return tmpStr;
}

//We compare names by last name
int compareAlarms(const void *first, const void *second){
    
    Alarm * tmpAlarm1;
    Alarm * tmpAlarm2;
    
    if (first == NULL || second == NULL){
        return 0;
    }
    
    tmpAlarm1 = (Alarm *)first;
    tmpAlarm2 = (Alarm *)second;
    
    return strcmp((char*)tmpAlarm1->trigger, (char*)tmpAlarm2->trigger);
}

//We need to free the first and last names, and then the Name struct itself
void deleteAlarms(void *toBeDeleted){
    
    Alarm * tmpAlarm;
    
    if (toBeDeleted == NULL){
        return;
    }
    
    tmpAlarm = (Alarm *)toBeDeleted;
    if(tmpAlarm->trigger != NULL){
        free(tmpAlarm->trigger);
    }
    if( &(tmpAlarm->properties) != NULL )
    {
        clearList(&(tmpAlarm->properties));
    }
    free(tmpAlarm);
}

//Event List Functions
char* printEvents(void *toBePrinted){
    
    char* tmpStr;
    Event * tmpEvent;
    int len;
    
    if (toBePrinted == NULL){
        return NULL;
    }
    
    tmpEvent = (Event *)toBePrinted;
    len = strlen(tmpEvent->UID) + 200;
    char * lenList;
    char * lenList2;
    if( getLength(tmpEvent->properties) > 1 ){
        lenList = toStringProp(tmpEvent->properties);
        len = len + strlen(lenList) +1;
    }
    if( getLength(tmpEvent->alarms) > 1 ){
        lenList2 = toStringAlarm(tmpEvent->alarms);
        len = len + strlen(lenList2) +1;
    }
    tmpStr = malloc(sizeof(char)*len);

    char buf[4000];

    sprintf(buf, "BEGIN:VEVENT\r\nUID:%s\r\nDTSTAMP:%sT%s", tmpEvent->UID, tmpEvent->creationDateTime.date, tmpEvent->creationDateTime.time);
    strcpy(tmpStr, buf);
    if(tmpEvent->creationDateTime.UTC){
        strcat(tmpStr, "Z\r\n");
    }
    else if(!tmpEvent->creationDateTime.UTC){
        strcat(tmpStr, "\r\n");
    }
    char buf2[500];
    sprintf(buf2, "DTSTART:%sT%s", tmpEvent->startDateTime.date, tmpEvent->startDateTime.time);
    strcat(tmpStr, buf2);
    if(tmpEvent->startDateTime.UTC){
        strcat(tmpStr, "Z\r\n");
    }
    else if(!tmpEvent->startDateTime.UTC){
        strcat(tmpStr, "\r\n");
    }
    if( getLength(tmpEvent->properties) > 1 ){
        strcat(tmpStr, lenList);
        free(lenList);
    }
    if( getLength(tmpEvent->alarms) > 1 ){
        strcat(tmpStr, lenList2);
        free(lenList2);
    }
    strcat(tmpStr, "END:VEVENT\r\n");
    return tmpStr;
}

//We compare names by last name
int compareEvents(const void *first, const void *second){
    
    Event * tmpEvent1;
    Event * tmpEvent2;
    
    if (first == NULL || second == NULL){
        return 0;
    }
    
    tmpEvent1 = (Event *)first;
    tmpEvent2 = (Event *)second;
    
    return strcmp((char*)tmpEvent1->UID, (char*)tmpEvent2->UID);
}

//We need to free the first and last names, and then the Name struct itself
void deleteEvents(void *toBeDeleted){
    
    Event * tmpEvent;
    
    if (toBeDeleted == NULL){
        return;
    }
    
    tmpEvent = (Event *)toBeDeleted;
    
    if( getLength(tmpEvent->alarms) > 0 )
    {
        clearList(&(tmpEvent->alarms));
    }
    if( getLength(tmpEvent->properties) > 0 )
    {
        clearList(&(tmpEvent->properties));
    }
    free(tmpEvent);
}

const char * checkExtension(const char *filename){
    const char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    else
    return dot + 1;
}

ICalErrorCode parseAlarm( Alarm * alarmObject, char ** fileSave, Calendar ** obj, int * index, int newLineCount, List alarmsPropsList, List * alarms ){
    (*index)++;

    // Recreate buffers & variables
    char * subTok;
    char commentCheck[10000];
    bool actionFlag = false, triggerFlag = false;
    while( true ) // break at END:VALARM
    {
        subTok = strtok(fileSave[*index], ":;");
        strcpy(commentCheck, fileSave[*index]);
        //printf("Alarm commentCheck: %s\n", commentCheck);
        if( commentCheck[0] != ';' ){

            if(commentCheck[0] == ':'){
                deleteCalendar(*obj);
                freeIndices(fileSave, newLineCount);
                *obj = NULL;
                return INV_ALARM;
            }

            if( strcmp("BEGIN", subTok) == 0)
            {
                subTok = strtok(NULL, "\0");
                if( subTok != NULL )
                {
                    Property * property;
                    property = malloc( sizeof(Property) + strlen(subTok) + 1);
                    strcpy(property->propName, "BEGIN");
                    strcpy(property->propDescr, subTok);
                    insertBack( &alarmsPropsList, property );
                }
            }

            ///////////////// ALARM OBJECTS /////////////////
            //case "TRIGGER"
            if( strcmp("TRIGGER", subTok) == 0)
            {
                subTok = strtok(NULL, "\n");
                if( subTok != NULL )
                {
                    alarmObject->trigger = malloc(sizeof(char)*(strlen(subTok) + 2));
                    strcpy(alarmObject->trigger, subTok);
                    triggerFlag = true;
                }
                else{
                    deleteCalendar(*obj);
                    freeIndices(fileSave, newLineCount);
                    *obj = NULL;
                    return INV_ALARM;
                }
            }

            //case "ACTION"
            else if( strcmp("ACTION", subTok) == 0)
            {
                subTok = strtok(NULL, "\n");
                if( subTok != NULL )
                {
                    strcpy(alarmObject->action, subTok);
                    actionFlag = true;
                }
                else{
                    deleteCalendar(*obj);
                    freeIndices(fileSave, newLineCount);
                    *obj = NULL;
                    return INV_ALARM;
                }
            }

            //case "END:":
            else if( strcmp("END", subTok) == 0 )
            {
                subTok = strtok(NULL, ":");
                if( subTok != NULL )
                {
                    if( strcmp("VALARM", subTok) == 0)
                    {
                        if( actionFlag == false || triggerFlag == false ){
                            deleteCalendar(*obj);
                            freeIndices(fileSave, newLineCount);
                            *obj = NULL;
                            return INV_ALARM;
                        }
                        alarmObject->properties = alarmsPropsList;
                        insertBack(alarms, alarmObject);  
                        return OK;
                    }
                    else{
                        Property * property;
                        property = malloc( sizeof(Property) + strlen(subTok) + 1);
                        strcpy(property->propName, "END");
                        strcpy(property->propDescr, subTok);
                        insertBack( &alarmsPropsList, property );
                    }
                }
                else{
                    deleteCalendar(*obj);
                    freeIndices(fileSave, newLineCount);
                    *obj = NULL;
                    return INV_ALARM;
                }
            }
            // CHECK FOR INVALID PROPERTY NAMES
            else if(!checkPropertyElse(subTok, 0)){
                deleteCalendar(*obj);
                freeIndices(fileSave, newLineCount);
                *obj = NULL;
                return INV_CAL;
            }
            else if(checkPropertyElse(subTok, 0)){
                Property * eventProperty = malloc(sizeof(Property));
                strcpy(eventProperty->propName, subTok);
                subTok = strtok(NULL, "\n");
                if( subTok != NULL )
                {
                    eventProperty = realloc( eventProperty, sizeof(Property) + strlen(subTok) + 1);
                    strcpy(eventProperty->propDescr, subTok);
                    insertBack( &alarmsPropsList, eventProperty );
                }
                else{
                    free(eventProperty);
                    deleteCalendar(*obj);
                    freeIndices(fileSave, newLineCount);
                    *obj = NULL;
                    return INV_ALARM;
                }
            }
        }
        (*index)++;
    }
    return INV_ALARM;
}

ICalErrorCode parseEvent( Event * event, char ** fileSave, Calendar ** obj, int * index, int newLineCount, List alarms, List eventProperties, List * eventsList ){
    (*index)++;

    // Recreate buffers & variables
    char * subTok;
    char commentCheck[10000];
    char * dataObject;
    List alarmsPropsList;
    Alarm * alarmObject;
    //bool dtStartFlag = false;
    bool durationFLag = false, dtendFlag = false;
    bool uidFlag = false, dtStampFlag = false;
    while( true ) // break at END:VEVENT
    {
        strcpy(commentCheck, fileSave[*index]);
        subTok = strtok(fileSave[*index], ":;");
        //printf("Event commentCheck: %s\n", commentCheck);
        //printf("subTok: %s\n", subTok);
        if( commentCheck[0] != ';' ){

            if(commentCheck[0] == ':'){
                deleteCalendar(*obj);
                freeIndices(fileSave, newLineCount);
                *obj = NULL;
                return INV_EVENT;
            }

            if(strcmp("DTEND", subTok) == 0){
            	dtendFlag = true;
            	if(durationFLag){
            		deleteCalendar(*obj);
	                freeIndices(fileSave, newLineCount);
	                *obj = NULL;
	                return INV_EVENT;
            	}
            }
            if(strcmp("DURATION", subTok) == 0){
            	durationFLag = true;
            	if(dtendFlag){
            		deleteCalendar(*obj);
	                freeIndices(fileSave, newLineCount);
	                *obj = NULL;
	                return INV_EVENT;
            	}
            }
            ////////////////  Event Object Properties  //////////////////

            //case "UID:":
            if( strcmp("UID", subTok) == 0)
            {
                subTok = strtok(NULL, "\n");
                if( subTok != NULL )
                {
                    dataObject = malloc(sizeof(char)*(strlen(subTok) + 2));
                    strcpy(dataObject, subTok);
                    strcpy(event->UID, dataObject);
                    free(dataObject);
                    uidFlag = true;
                }
                else{
                    deleteCalendar(*obj);
                    freeIndices(fileSave, newLineCount);
                    *obj = NULL;
                    return INV_EVENT;
                }
            }

            //////////////// DATE TIME //////////////////

            //case "DTSTART:":
            else if( strcmp("DTSTART", subTok) == 0)
            {   
                //Check for Date & Time
                subTok = strtok(NULL, "\n");
                if( subTok != NULL )
                {
                    dataObject = malloc(sizeof(char)*(strlen(subTok) + 2));
                    int i, j = 0, strLen;
                    strLen = strlen(subTok);
                    for(i = 0; i < strLen; i++)
                    {
                        if( subTok[i] == '\0' ){
                            break;
                        }
                        if( subTok[i] != 'Z' )
                        {
                            dataObject[j] = subTok[i];
                        }
                        j++;
                    }
                    dataObject[i-1] = '\0';
                    if( strstr(dataObject, "T") == NULL ){
                        deleteCalendar(*obj);
                        freeIndices(fileSave, newLineCount);
                        *obj = NULL;
                        return INV_CREATEDT;
                    }
                    if( subTok[strLen-1] == 'Z' )
                    {
                        if(event != NULL){
                            event->startDateTime.UTC = true;
                        }
                    }
                    else{
                        if(event != NULL){
                            event->startDateTime.UTC = false;
                        }
                    }
                    //Parse Date and Time
                    if(event != NULL){
                        subTok = strtok(dataObject, "T");
                        if( strlen(subTok) > 8 || strlen(subTok) < 8 ){
                            deleteCalendar(*obj);
                            freeIndices(fileSave, newLineCount);
                            *obj = NULL;
                            return INV_CREATEDT;
                        }
                        strcpy(event->startDateTime.date, subTok);
                        subTok = strtok(NULL, "T");
                        if( strlen(subTok) > 7 || strlen(subTok) < 5 ){
                            deleteCalendar(*obj);
                            freeIndices(fileSave, newLineCount);
                            *obj = NULL;
                            return INV_CREATEDT;
                        }
                        strcpy(event->startDateTime.time, subTok);
                        free(dataObject);
                    }
                }
                else{
                    deleteCalendar(*obj);
                    freeIndices(fileSave, newLineCount);
                    *obj = NULL;
                    return INV_CREATEDT;
                }
            }

            //case "DTSTAMP:":
            else if( strcmp("DTSTAMP", subTok) == 0)
            {   
                //Check for Date & Time
                subTok = strtok(NULL, "\n");
                if( subTok != NULL )
                {
                    dataObject = malloc(sizeof(char)*(strlen(subTok) + 2));
                    int i, j = 0, strLen;
                    strLen = strlen(subTok);
                    for(i = 0; i < strLen; i++)
                    {
                        if( subTok[i] == '\0' ){
                            break;
                        }
                        if( subTok[i] != 'Z' )
                        {
                            dataObject[j] = subTok[i];
                        }
                        j++;
                    }
                    dataObject[i-1] = '\0';
                    if( strstr(dataObject, "T") == NULL ){
                        deleteCalendar(*obj);
                        freeIndices(fileSave, newLineCount);
                        *obj = NULL;
                        return INV_CREATEDT;
                    }
                    if( subTok[strLen-1] == 'Z' )
                    {
                        if(event != NULL){
                            event->creationDateTime.UTC = true;
                        }
                    }
                    else{
                        if(event != NULL){
                            event->creationDateTime.UTC = false;
                        }
                    }

                    //Parse Date and Time
                    if(event != NULL){
                        subTok = strtok(dataObject, "T");
                        if( strlen(subTok) > 8 || strlen(subTok) < 8 ){
                            deleteCalendar(*obj);
                            freeIndices(fileSave, newLineCount);
                            *obj = NULL;
                            return INV_CREATEDT;
                        }
                        strcpy(event->creationDateTime.date, subTok);
                        subTok = strtok(NULL, "T");
                        if( strlen(subTok) > 6 || strlen(subTok) < 6 ){
                            deleteCalendar(*obj);
                            freeIndices(fileSave, newLineCount);
                            *obj = NULL;
                            return INV_CREATEDT;
                        }
                        strcpy(event->creationDateTime.time, subTok);
                        free(dataObject);
                        dtStampFlag = true;
                    }
                }
                else{
                    deleteCalendar(*obj);
                    freeIndices(fileSave, newLineCount);
                    *obj = NULL;
                    return INV_CREATEDT;
                }
            }

            //case "BEGIN:":  // Beginning object (calendar, event)
            else if( strcmp("BEGIN", subTok) == 0)
            {
                subTok = strtok(NULL, "\0");
                if( subTok != NULL )
                {
                    if( strcmp("VALARM", subTok) == 0)
                    {
                        alarmObject = malloc(sizeof(Alarm));
                        alarmObject->trigger = NULL;
                        alarmsPropsList = initializeList(&printProps, &deleteProps, &compareProps);
                        ICalErrorCode alarmErrorCode;
                        alarmErrorCode = parseAlarm( alarmObject, fileSave, obj, index, newLineCount, alarmsPropsList, &alarms  );
                        if( alarmErrorCode != OK ){
                            return alarmErrorCode;
                        }
                    }
                    else if( strcmp("VEVENT", subTok) == 0)
                    {
                        deleteCalendar(*obj);
                        freeIndices(fileSave, newLineCount);
                        *obj = NULL;
                        return INV_EVENT;
                    }
                    else{
                        Property * property;
                        property = malloc( sizeof(Property) + strlen(subTok) + 1);
                        strcpy(property->propName, "BEGIN");
                        strcpy(property->propDescr, subTok);
                        insertBack( &eventProperties, property );
                    }
                }
            }

            //case "END:":
            else if( strcmp("END", subTok) == 0 )
            {
                subTok = strtok(NULL, ":");
                if( subTok != NULL )
                {
                    if(strcmp(subTok, "VEVENT") == 0)
                    {
                        // Check for parsed UID
                        if( !uidFlag ){
                            deleteCalendar(*obj);
                            freeIndices(fileSave, newLineCount);
                            *obj = NULL;
                            return INV_EVENT;
                        }
                        else if( !dtStampFlag ){
                            deleteCalendar(*obj);
                            freeIndices(fileSave, newLineCount);
                            *obj = NULL;
                            return INV_CREATEDT;
                        }
                        else if(event != NULL){
                            event->properties = eventProperties;
                            event->alarms = alarms;
                            insertBack(eventsList, event);
                            return OK;
                        }
                    }
                    else if( strcmp("VALARM", subTok) == 0)
                    {
                        deleteCalendar(*obj);
                        freeIndices(fileSave, newLineCount);
                        *obj = NULL;
                        return INV_EVENT;
                    }
                    else{
                        Property * property;
                        property = malloc( sizeof(Property) + strlen(subTok) + 1);
                        strcpy(property->propName, "END");
                        strcpy(property->propDescr, subTok);
                        insertBack( &eventProperties, property );
                    }
                }
                else{
                    deleteCalendar(*obj);
                    freeIndices(fileSave, newLineCount);
                    *obj = NULL;
                    return INV_EVENT;
                }
            }

            // CHECK FOR INVALID PROPERTY NAMES
            else if(!checkPropertyElse(subTok, 1)){
                deleteCalendar(*obj);
                freeIndices(fileSave, newLineCount);
                *obj = NULL;
                return INV_CAL;
            }
            else if(checkPropertyElse(subTok, 1)){
                Property * eventProperty = malloc(sizeof(Property));
                strcpy(eventProperty->propName, subTok);
                subTok = strtok(NULL, "\n");
                if( subTok != NULL )
                {
                    eventProperty = realloc( eventProperty, sizeof(Property) + strlen(subTok) + 100);
                    strcpy(eventProperty->propDescr, subTok);
                    insertBack( &eventProperties, eventProperty );
                }
                else{
                    free(eventProperty);
                    deleteCalendar(*obj);
                    freeIndices(fileSave, newLineCount);
                    *obj = NULL;
                    return INV_EVENT;
                }
            }
        }
        (*index)++;
    }
    return INV_EVENT;
}

ICalErrorCode createCalendar(char* fileName, Calendar** obj)
{   
    if(fileName == NULL || strcmp(fileName,"") == 0)
        return INV_FILE;
    if(obj == NULL )
        return INV_CAL;
    else
    {
        const char * tok = checkExtension(fileName);
        if( strcmp(tok, "ics") != 0 )
        {
            return INV_FILE;
        }
        /* Create Calendar */

        // Parse Text Data
        FILE* fp;
        fp = fopen(fileName, "r");

        if(fp == NULL)
        {
            return INV_FILE;
        }
        else
        {
            /*      PARSING     */
            int length = 0;
            fseek(fp, 0, SEEK_END);
            length = ftell(fp);
            rewind(fp);

            char * sample = malloc(sizeof(char *)*(length)+50);
            sample[0] = '\0';
            fread(sample, sizeof(char), length, fp);
            strcat(sample, "\0");

            rewind(fp);

            char lineCheck[4000];
            int newLineCount = 0;
            // USE FGETS TO CHECK FOR INVALID LINES \r\n
            while( fgets(lineCheck, 4000, fp) != NULL ){
                if( strstr(lineCheck, "\r\n") == NULL ){
                    free(sample);
                    fclose(fp);
                    return INV_FILE;
                }
                newLineCount++;
            }

            // ALSO GET LINE COUNT FROM THIS

            fclose(fp);

            char * newSample = cleanString(sample);

            // Store Parsed File String into String Array Line by Line
            char ** fileSave = malloc(sizeof(char *)*newLineCount);

            char * buffer;
            buffer = strtok(newSample, "\r\n");   // Grab File Line from File String
            int i = 0;
            while(buffer != NULL && i < newLineCount)
            {
                fileSave[i] = malloc(sizeof(char)*(strlen(buffer)+1));
                strcpy(fileSave[i], buffer);
                buffer = strtok(NULL, "\r\n");
                i++;
            }
            free(newSample);

            // Main Parsing Command Loop Variables
            char * dataObject;
            char * subTok;
            List eventsList;
            List calPropertiesList;
            List propertiesList;
            List alarmsList;
            List alarmsPropsList;
            Alarm * alarmObject;
            Event * event;
            char commentCheck[4000];
            //int ERRORCOUNTER = 1;
            int index = 0, verFlag = 0, prodidFlag = 0, begVCalFlag = 0, firstEventFlag = 0, endEventFlag = 0, uidFlag = 0, calscaleFlag = 0, methFlag = 0;
            while( index < newLineCount )
            {   
                //printf("ERRORCOUNTER: %d\n", ERRORCOUNTER);
                //ERRORCOUNTER++;
                if( fileSave[index] == NULL ){
                    break;
                }
                //strcpy(commentCheck, fileSave[index]);
                //printf("commentCheck: %s\n", commentCheck);
                subTok = strtok(fileSave[index], ":;");  // strtok Line at ': or ;'
                if( commentCheck[0] != ';' ){

                    if( commentCheck[0] == ':' && begVCalFlag == 0 ){
                        freeIndices(fileSave, newLineCount);
                        *obj = NULL;
                        return INV_CAL;
                    }
                    else if(commentCheck[0] == ':'){
                        deleteCalendar(*obj);
                        freeIndices(fileSave, newLineCount);
                        *obj = NULL;
                        return INV_CAL;
                    }
                    //case "BEGIN:":  // Beginning object (calendar, event)
                    if( strcmp("BEGIN", subTok) == 0)
                    {
                        subTok = strtok(NULL, "\r");
                        if( subTok != NULL )
                        {
                            //Check for VCALENDAR
                            if( strcmp("VCALENDAR", subTok) == 0)
                            {
                                *obj = malloc(sizeof(Calendar));
                                eventsList = initializeList(&printEvents, &deleteEvents, &compareEvents);
                                calPropertiesList = initializeList(&printProps, &deleteProps, &compareProps);
                                begVCalFlag = 1;
                            }
                            //Check for VEVENT
                            else if( strcmp("VEVENT", subTok) == 0)
                            {
                                if(verFlag == 0){   // IF NO VERSION BEFORE EVENT
                                    deleteCalendar(*obj);
                                    freeIndices(fileSave, newLineCount);
                                    *obj = NULL;
                                    return INV_CAL;
                                }
                                if(prodidFlag == 0){    // IF NO PRODID BEFORE EVENT
                                    deleteCalendar(*obj);
                                    freeIndices(fileSave, newLineCount);
                                    *obj = NULL;
                                    return INV_CAL;
                                }
                                else if(begVCalFlag == 0){
                                    deleteCalendar(*obj);
                                    freeIndices(fileSave, newLineCount);
                                    *obj = NULL;
                                    return INV_CAL;
                                }
                                event = malloc(sizeof(Event));
                                strcpy(event->UID, "Empty");
                                propertiesList = initializeList(&printProps, &deleteProps, &compareProps);
                                alarmsList = initializeList(&printAlarms, &deleteAlarms, &compareAlarms);
                                ICalErrorCode eventErrorCode;
                                eventErrorCode = parseEvent( event, fileSave, obj, &index, newLineCount, alarmsList, propertiesList, &eventsList );
                                if( eventErrorCode != OK ){
                                    return eventErrorCode;
                                }
                                firstEventFlag = 1;
                            }
                            else if( strcmp("VALARM", subTok) == 0)
                            {
                                alarmObject = malloc(sizeof(Alarm));
                                alarmObject->trigger = NULL;
                                alarmsPropsList = initializeList(&printProps, &deleteProps, &compareProps);
                            }
                            else if(checkPropertyCal(subTok)){
                                Property * property;
                                property = malloc( sizeof(Property) + strlen(subTok) + 1);
                                strcpy(property->propName, "BEGIN");
                                strcpy(property->propDescr, subTok);
                                insertBack( &calPropertiesList, property );
                            }
                        }
                        else if(begVCalFlag == 0){
                            deleteCalendar(*obj);
                            freeIndices(fileSave, newLineCount);
                            *obj = NULL;
                            return INV_CAL;
                        }
                        else if(begVCalFlag !=0 && firstEventFlag == 0){
                            deleteCalendar(*obj);
                            freeIndices(fileSave, newLineCount);
                            *obj = NULL;
                            return INV_EVENT;
                        }else{
                            deleteCalendar(*obj);
                            freeIndices(fileSave, newLineCount);
                            *obj = NULL;
                            return INV_CAL;
                        }
                    }
                    //case "VERSION:":
                    else if( strcmp("VERSION", subTok) == 0)
                    {
                        if(begVCalFlag == 0)
                        {
                            deleteCalendar(*obj);
                            freeIndices(fileSave, newLineCount);
                            *obj = NULL;
                            return INV_CAL;
                        }
                        if(verFlag == 1){   // IF DUP VERSION
                            deleteCalendar(*obj);
                            freeIndices(fileSave, newLineCount);
                            *obj = NULL;
                            return DUP_VER;
                        }
                        //Check for VERSION:2.0 
                        subTok = strtok(NULL, "\n");
                        if( subTok != NULL )
                        {
                            dataObject = malloc(sizeof(char)*(strlen(subTok) + 2));
                            strcpy(dataObject, subTok);
                            float ver = atof(dataObject);
                            if(ver < 1)
                            {
                                deleteCalendar(*obj);
                                free(dataObject);
                                freeIndices(fileSave, newLineCount);
                                *obj = NULL;
                                return INV_VER;
                            }
                            else{
                                verFlag = 1;
                                (*obj)->version = ver;
                                free(dataObject);
                            }
                        }
                        else{
                            deleteCalendar(*obj);
                            freeIndices(fileSave, newLineCount);
                            *obj = NULL;
                            return INV_VER;
                        }
                    }
                    //case "PRODID:":
                    else if( strcmp("PRODID", subTok) == 0)
                    {
                        subTok = strtok(NULL, ":");
                        if(prodidFlag == 1){    // IF DUP PRODID
                            deleteCalendar(*obj);
                            freeIndices(fileSave, newLineCount);
                            *obj = NULL;
                            return DUP_PRODID;
                        }
                        if( subTok != NULL )
                        {
                            if( prodidFlag != 0 ){
                                deleteCalendar(*obj);
                                freeIndices(fileSave, newLineCount);
                                *obj = NULL;
                                return DUP_PRODID;
                            }
                            dataObject = malloc(sizeof(char)*(strlen(subTok) + 2));
                            strcpy(dataObject, subTok);
                            strcpy((*obj)->prodID, dataObject);
                            prodidFlag = 1;
                        }
                        else{ 
                            deleteCalendar(*obj);
                            freeIndices(fileSave, newLineCount);
                            *obj = NULL;
                            return INV_PRODID;
                        }
                    }

                    //case "CALSCALE:":
                    else if( strcmp("CALSCALE", subTok) == 0)
                    {
                        char * propName = malloc(sizeof(char)*20);
                        strcpy(propName, subTok);
                        subTok = strtok(NULL, "\n");
                        if( subTok != NULL )
                        {
                            if( calscaleFlag )
                            {
                                free(propName);
                                deleteCalendar(*obj);
                                freeIndices(fileSave, newLineCount);
                                return INV_CAL;
                            }  
                            Property * propertyData = malloc(sizeof(Property)+(sizeof(char)*strlen(subTok)+1)); // NEEDS TO BE FREE IN DELETE FUNCTION
                            strcpy(propertyData->propName, propName);
                            strcpy(propertyData->propDescr, subTok);
                            insertBack(&calPropertiesList, propertyData);
                            calscaleFlag = 1;
                        }else{
                            free(propName);
                            deleteCalendar(*obj);
                            freeIndices(fileSave, newLineCount);
                            *obj = NULL;
                            return INV_EVENT;
                        }
                    }

                    //case "METHOD:":
                    else if( strcmp("METHOD", subTok) == 0)
                    {
                        char * propName = malloc(sizeof(char)*20);
                        strcpy(propName, subTok);
                        subTok = strtok(NULL, "\n");
                        if( subTok != NULL )
                        {
                            if( methFlag )
                            {
                                free(propName);
                                deleteCalendar(*obj);
                                freeIndices(fileSave, newLineCount);
                                return INV_CAL;
                            }
                            Property * propertyData = malloc(sizeof(Property)+(sizeof(char)*strlen(subTok)+1)); // NEEDS TO BE FREE IN DELETE FUNCTION
                            strcpy(propertyData->propName, propName);
                            strcpy(propertyData->propDescr, subTok);
                            insertBack(&calPropertiesList, propertyData);
                            methFlag = 1;
                        }else{
                            free(propName);
                            deleteCalendar(*obj);
                            freeIndices(fileSave, newLineCount);
                            *obj = NULL;
                            return INV_EVENT;
                        }
                    }

                    //case "END:":
                    else if( strcmp("END", subTok) == 0 )
                    {
                        subTok = strtok(NULL, ":");
                        if( subTok != NULL )
                        {
                            dataObject = malloc(sizeof(char)*(strlen(subTok) + 2));
                            strcpy(dataObject, subTok);
                            if(strcmp(subTok, "VCALENDAR") == 0)
                            {
                                if( firstEventFlag == 0){
                                    deleteCalendar(*obj);
                                    freeIndices(fileSave, newLineCount);
                                    *obj = NULL;
                                    return INV_CAL;
                                }
                                free(dataObject);
                                (*obj)->events = eventsList;
                                (*obj)->properties = calPropertiesList;
                                return OK;
                            }
                            else if(strcmp(subTok, "VEVENT") == 0)
                            {
                                if( uidFlag != 1 ){
                                    free(dataObject);
                                    deleteCalendar(*obj);
                                    freeIndices(fileSave, newLineCount);
                                    *obj = NULL;
                                    return INV_EVENT;
                                }
                                if(event == NULL){
                                    free(dataObject);
                                    deleteCalendar(*obj);
                                    freeIndices(fileSave, newLineCount);
                                    *obj = NULL;
                                    return INV_EVENT;
                                }
                                else if(event != NULL){
                                    event->properties = propertiesList;
                                    event->alarms = alarmsList;
                                    insertBack(&eventsList, event);
                                    endEventFlag++;
                                }
                            }
                            else if( strcmp("VALARM", subTok) == 0)
                            {
                                alarmObject->properties = alarmsPropsList;
                                insertBack(&alarmsList, alarmObject);  
                            }
                            else if(!checkPropertyCal(subTok)){
                                if(begVCalFlag == 0){
                                    freeIndices(fileSave, newLineCount);
                                    *obj = NULL;
                                    return INV_CAL;
                                }
                                else{
                                    deleteCalendar(*obj);
                                    freeIndices(fileSave, newLineCount);
                                    *obj = NULL;
                                    return INV_CAL;
                                }
                            }
                            else if(checkPropertyCal(subTok)){
                                Property * property;
                                property = malloc( sizeof(Property) + strlen(subTok) + 1);
                                strcpy(property->propName, "END");
                                strcpy(property->propDescr, subTok);
                                insertBack( &calPropertiesList, property );
                            }
                            free(dataObject);
                        }
                        else if(begVCalFlag == 0 || endEventFlag == 1){
                            deleteCalendar(*obj);
                            freeIndices(fileSave, newLineCount);
                            *obj = NULL;
                            return INV_CAL;
                        }
                        else{
                            deleteCalendar(*obj);
                            freeIndices(fileSave, newLineCount);
                            *obj = NULL;
                            return OTHER_ERROR;
                        }
                    }
                    else{
                        if(begVCalFlag == 0){
                            freeIndices(fileSave, newLineCount);
                            *obj = NULL;
                            return INV_CAL;
                        }
                    }
                }
                index++;
            } 
            freeIndices(fileSave, newLineCount);  
            return INV_CAL;
        }
    }
}

void freeIndices(char ** fileSave, int length){
    if(fileSave != NULL){
        int index = 0;
        while(index < length )
        {
            if( strlen(fileSave[index]) > 3 ){
                if(fileSave[index] != NULL){
                    free(fileSave[index]);    
                }
            }
            else{
                break;
            }
            index++;
        }
        free(fileSave);
    }
}

void deleteCalendar(Calendar* obj)
{
    if(obj == NULL)
    {
        return;
    }
    if( obj != NULL ){
        clearList(&(obj->events));
        clearList(&(obj->properties));
        free(obj);
        obj = NULL;
    }
}

char* printCalendar(const Calendar* obj)
{
    if(obj == NULL)
    {
        return NULL;
    }
    char * len1;
    char * len2;
    int length = (strlen(obj->prodID) + 1) + 200;
    if( getLength(obj->properties) > 0 ){
        len1 = toStringProp(obj->properties);
        length = length + strlen(len1) + 1;
    }
    if( getLength(obj->events) > 0 ){
        len2 = toStringEvent(obj->events);
        length = length + strlen(len2) + 1;
    }
    char * string = malloc( sizeof(char)*length );
    string[0] = '\0';
    char x[60];
    x[0] = '\0';
    sprintf(x, "BEGIN:VCALENDAR\r\nVERSION:%0.1f\r\n", obj->version);
    strcat(string, x);
    strcat(string, "PRODID:");
    strcat(string, obj->prodID);
    strcat(string, "\r\n\0");
    if( getLength(obj->properties) > 1 ){
        strcat(string, len1);
        free(len1);
    }
    strcat(string, "\0");
    if( getLength(obj->events) > 0 ){
        strcat(string, len2);
        free(len2);
    }
    strcat(string, "END:VCALENDAR\r\n"); 
    if(strlen(string) > length){
        printf("Storing Over Allocated Memory\n");
    }
    return string;
}

char* printError(ICalErrorCode err)
{
    char * str = NULL; 
    switch (err)
    {
        case OK:
        str = malloc(strlen("Calendar Operation Successful")+1);
        strcpy(str, "Calendar Operation Successful");
        break;

        case INV_FILE:
        str = malloc(strlen("Invalid File")+1);
        strcpy(str, "Invalid File");
        break;

        case INV_CAL:
        str = malloc(strlen("Invalid Calendar")+1);
        strcpy(str, "Invalid Calendar");
        break;

        case INV_VER:
        str = malloc(strlen("Invalid Version")+1);
        strcpy(str, "Invalid Version");
        break;

        case DUP_VER:
        str = malloc(strlen("Duplicate Version")+1);
        strcpy(str, "Duplicate Version");
        break;

        case INV_PRODID:
        str = malloc(strlen("Invalid Product ID")+1);
        strcpy(str, "Invalid Product ID");
        break;

        case DUP_PRODID:
        str = malloc(strlen("Duplicate Product ID")+1);
        strcpy(str, "Duplicate Product ID");
        break;

        case INV_EVENT:
        str = malloc(strlen("Invalid Event")+1);
        strcpy(str, "Invalid Event");
        break;

        case INV_ALARM:
        str = malloc(strlen("Invalid Alarm")+1);
        strcpy(str, "Invalid Alarm");
        break;

        case INV_CREATEDT:
        str = malloc(strlen("Invalid Time")+1);
        strcpy(str, "Invalid Time");
        break;

        case OTHER_ERROR:
        str = malloc(strlen("ERROR")+1);
        strcpy(str, "ERROR");
        break;

        default:
        str = malloc(strlen("INVALID_err_VALUE")+1);
        strcpy(str, "INVALID_err_VALUE");
        break;
    }
    return str;
}

char * cleanString( char * string )
{
    char * fixedString = malloc(sizeof(char)*(strlen(string)+1));
    fixedString[0] = '\0';
    int i, j = 0;
    for( i = 0; i < strlen(string)+1; i++ )
    {
        if((string[i] == ' ' || string[i] == '\n' || string[i] == '\r' || string[i] == '\t')
        && (string[i + 1] == ' ' || string[i + 1] == '\n' || string[i + 1] == '\r' 
        || string[i + 1] == '\t'))
        {
            while( string[i+1] == ' ' || string[i+1] == '\r' || string[i+1] == '\t' || string[i+1] == '\n' )
            {
                i++;
            }
        }
        if(string[i] == ' ' && string[i+1] != '\n' && string[i-1] == '\n')
        {
            i++;
        }
        if((int)string[i] == 92 && string[i+1] == ',')
        {
            i++;
        }
        if(string[i] == '\n' 
        && (((int)string[i+1] >= 48 && (int)string[i+1] <= 57) || ((int)string[i+1] >= 65 && (int)string[i+1] <= 90) || ((int)string[i+1] >= 97 && (int)string[i+1] <= 122))
        && (((int)string[i-1] >= 48 && (int)string[i-1] <= 57) || ((int)string[i-1] >= 65 && (int)string[i-1] <= 90) || ((int)string[i-1] >= 97 && (int)string[i-1] <= 122)) 
        && string[i+2] == '\n')
        {
            i++;
        }
        if(string[i] != '\r' && string[i] != '\t' )
        {
            fixedString[j] = string[i];
            j++;
        }
    } 
    strcat(fixedString, "\0");
    free(string);
    return fixedString;
}

char * toStringEvent(List list)
{
    int length = 0;
    Node * tmp = list.head;
    char * string;
    while( tmp != NULL )
    {
        string = printEvents(tmp->data);
        length = length + strlen(string) + 1;
        free(string);
        tmp = tmp->next;
    } 
    char * listString = malloc(sizeof(char)*length);
    listString[0] = '\0';
    Node * tmp2 = list.head;
    int flag = 0;
    char * tPtr;
    while( tmp2 != NULL )
    {
        tPtr = printEvents(tmp2->data);
        length = length + strlen(tPtr) + 1;
        listString = realloc(listString, length);
        if(flag == 0){
            strcpy(listString, tPtr);
        }
        else{
            strcat(listString, tPtr);
        }
        tmp2 = tmp2->next;
        free(tPtr);
        flag = 1;
    }
    return listString;
}

char * toStringAlarm(List list)
{
    int length = 0;
    Node * tmp = list.head;
    char * string;
    while( tmp != NULL )
    {
        string = printAlarms(tmp->data);
        length = length + strlen(string) + 1;
        free(string);
        tmp = tmp->next;
    } 
    char * listString = malloc(sizeof(char)*length);
    listString[0] = '\0';
    Node * tmp2 = list.head;
    int flag = 0;
    char * tPtr;
    while( tmp2 != NULL )
    {
        tPtr = printAlarms(tmp2->data);
        length = length + strlen(tPtr) + 1;
        listString = realloc(listString, length);
        if(flag == 0){
            strcpy(listString, tPtr);
        }
        else{
            strcat(listString, tPtr);
        }
        tmp2 = tmp2->next;
        free(tPtr);
        flag = 1;
    }
    return listString;
}

char * toStringProp(List list)
{
    int length = 0;
    Node * tmp = list.head;
    char * string;
    while( tmp != NULL )
    {
        string = printProps(tmp->data);
        length = length + strlen(string) + 1;
        free(string);
        tmp = tmp->next;
    } 
    char * listString = malloc(sizeof(char)*length);
    listString[0] = '\0';
    Node * tmp2 = list.head;
    int flag = 0;
    char * tPtr;
    while( tmp2 != NULL )
    {
        tPtr = printProps(tmp2->data);
        length = length + strlen(tPtr) + 1;
        listString = realloc(listString, length);
        if(flag == 0){
            strcpy(listString, tPtr);
        }
        else{
            strcat(listString, tPtr);
        }
        tmp2 = tmp2->next;
        free(tPtr);
        flag = 1;
    }
    return listString;
}

ICalErrorCode writeCalendar(char * fileName, const Calendar* obj){
    if(obj == NULL){
        return WRITE_ERROR;
    }
    else if( fileName == NULL || strcmp(fileName,"") == 0 ){
        return WRITE_ERROR;
    }
    // If object is invalid return WRITE_ERROR
    ICalErrorCode errorCode;
    errorCode = validateCalendar(obj);
    if( errorCode == INV_CAL ){
        return WRITE_ERROR;
    }
    else{
        // Print Calendar Object to String
        char * calendarString = printCalendar(obj);

        // Create File
        // Parse Text Data
        FILE* fp;
        
       /* int len = strlen(fileName);
        int i; // Rename File, 'fileName'NEW.ics
        for(i = (len + 1); i >= (len - 4); i-- ){
            fileName[i] = '\0';
        }
        strcat(fileName, "NEW.ics");*/
        printf("Calendar saved as %s\n", fileName);
        fp = fopen(fileName, "w+");

        fseek(fp, 0, SEEK_SET);

        if(fp == NULL)
        {
            fclose(fp);
            free(calendarString);
            return INV_FILE;
        }

        // Write Calendar to File

        fputs(calendarString, fp);

        fclose(fp);
        free(calendarString);
        return OK;
    }

}

ICalErrorCode validateCalendar(const Calendar* obj){
    if( obj == NULL ){
        return INV_CAL;
    }
    else if( obj->version < 1 ){
        return INV_CAL;
    }
    else if( strlen(obj->prodID) == 0 ){
        return INV_CAL;
    }
    else if( getLength(obj->events) < 0 ){
        return INV_CAL;        
    }
    else if( getLength(obj->properties) > 0 ){
        ListIterator propIter = createIterator(obj->properties);
        void * temp = NULL;
        bool calscale = false, method = false;
        while((temp = nextElement(&propIter)) != NULL){
            Property * property = (Property*)temp;
            if( strcmp(property->propName,"CALSCALE") == 0 ){
                if(calscale){
                    return INV_CAL;
                }
                calscale = true;
            }
            else if( strcmp(property->propName,"METHOD") == 0 ){
                if(method){
                    return INV_CAL;
                }
                method = true;
            }
            else if( strcmp(property->propName,"PRODID") == 0 ){
                if(strlen(obj->prodID) > 1){
                    return INV_CAL;
                }
            }
            else if( strcmp(property->propName,"VERSION") == 0 ){
                if(obj->version > 0){
                    return INV_CAL;
                }
            }
        }
    }
    if( getLength(obj->events) > 0 ){
        // Check each element of events list
        ICalErrorCode validCheck = validateEvent(obj->events);
        if( validCheck == INV_EVENT ){
            return INV_EVENT;
        }
        else if( validCheck == INV_ALARM ){
            return INV_ALARM;   
        }
    }
    return OK;
}

ICalErrorCode validateEvent(List list){
    Node * tPtr = list.head;
    Event * eventCheck;
    DateTime dateTimeCheck;
    while(tPtr != NULL){
        eventCheck = tPtr->data;
        dateTimeCheck = eventCheck->creationDateTime;
        if(strlen(eventCheck->UID) == 0){
            return INV_EVENT;
        }
        else if(strlen(dateTimeCheck.date) == 0){
            return INV_EVENT;
        }
        else if(strlen(dateTimeCheck.time) == 0){
            return INV_EVENT;
        }
        else if( getLength(eventCheck->alarms) > 0 ){
            bool validCheck = validateAlarm(eventCheck->alarms);
            if( validCheck == false ){
                return INV_ALARM;
            }
        }
        if( getLength(eventCheck->properties) > 0 ){
            ListIterator propIter = createIterator(eventCheck->properties);
            void * temp = NULL;
            bool organizer = false, dtstart = false, dtend = false, class = false, summary = false;
            while((temp = nextElement(&propIter)) != NULL){
                Property * property = (Property*)temp;

                if( strcmp(property->propName,"ORGANIZER") == 0 ){
                    if(organizer){
                        return INV_EVENT;
                    }
                    organizer = true;
                }
                else if( strcmp(property->propName,"DTSTART") == 0 ){
                    if(dtstart){
                        return INV_EVENT;
                    }
                    dtstart = true;
                }
                else if( strcmp(property->propName,"DTEND") == 0 ){
                    if(dtend){
                        return INV_EVENT;
                    }
                    dtend = true;
                }
                else if( strcmp(property->propName,"CLASS") == 0 ){
                    if(class){
                        return INV_EVENT;
                    }
                    class = true;
                }
                else if( strcmp(property->propName,"SUMMARY") == 0 ){
                    if(summary){
                        return INV_EVENT;
                    }
                    summary = true;
                }
            }
        }
        tPtr = tPtr->next;
    }
    return true;
}

bool validateAlarm(List list){
    Node * tPtr = list.head;
    Alarm * alarmCheck;
    while(tPtr != NULL){
        alarmCheck = tPtr->data;
        if(strlen(alarmCheck->action) == 0){
            return false;
        }
        else if(strlen(alarmCheck->trigger) == 0){
            return false;
        }
        tPtr = tPtr->next;
    }
    return true;
}

bool checkDigit(char * string){
    int strLen = strlen(string);
    if( string == NULL || strLen == 0){
        return false;
    }
    int i;
    for( i = 0; i < strLen; i++ ){
        if(!isdigit(string[i])){
            return false;
        }
    }
    return true;
}

char * toLower(char * string){
    int strLen = strlen(string);
    if( string == NULL || strLen == 0){
        return false;
    }
    int i;
    for( i = 0; i < strLen; i++ ){
        putchar(tolower(string[i]));
    }
    return string;
}

bool checkPropertyCal(char * string){
    if(strcmp(string, "VTIMEZONE") == 0 ||
       strcmp(string, "VJOURNAL") == 0 ||
       strcmp(string, "VTODO") == 0 ||
       strcmp(string, "VFREEBUSY") == 0 )
    {
        return true;
    }
    return false;
}

bool checkPropertyElse(char * name, int eaFlag){
    if( eaFlag == 1 ){
        if(strcmp(name, "CLASS") == 0 || strcmp(name, "CREATED") == 0 ||
           strcmp(name, "DESCRIPTION") == 0 || strcmp(name, "DTSTAMP") == 0 ||
           strcmp(name, "DTSTART") == 0 || strcmp(name, "DTEND") == 0 ||
           strcmp(name, "DURATION") == 0 || strcmp(name, "GEO") == 0 ||
           strcmp(name, "LAST-MODIFIED") == 0 || strcmp(name, "LOCATION") == 0 ||
           strcmp(name, "ORGANIZER") == 0 || strcmp(name, "PRIORITY") == 0 ||
           strcmp(name, "SEQUENCE") == 0 || strcmp(name, "STATUS") == 0 ||
           strcmp(name, "SUMMARY") == 0 || strcmp(name, "TRANSP") == 0 ||
           strcmp(name, "URL") == 0 || strcmp(name, "RESOURCES") == 0 )
        {
            return true;
        }
    }
    else if( eaFlag == 0 ){
        if(strcmp(name, "ATTACH") == 0 || strcmp(name, "ATTENDEE") == 0 ||
           strcmp(name, "CATEGORIES") == 0 || strcmp(name, "COMMENT") == 0 ||
           strcmp(name, "CONTACT") == 0 || strcmp(name, "EXDATE") == 0 ||
           strcmp(name, "EXRULE") == 0 || strcmp(name, "RELATED-TO") == 0 ||
           strcmp(name, "REQUEST-STATUS") == 0 || strcmp(name, "RESOURCES") == 0 ||
           strcmp(name, "RDATE") == 0 || strcmp(name, "RRULE") == 0 || 
           strcmp(name, "REPEAT") == 0 || strcmp(name, "DURATION") == 0 ||
           strcmp(name, "DESCRIPTION") == 0)
        {
            return true;
        }
    }
    return false;
}
