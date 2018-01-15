from ICalWrapper import *
from dbTest import * 
from tkinter import *
from tkinter import ttk
from tkinter.filedialog import askopenfilename, asksaveasfilename

################################################
######                 SQL                 #####
################################################


def createQuery():

    def closeWindow():
        qWindow.destroy()

    def clearQLog():
        qlogTB.config(state=NORMAL)
        qlogTB.delete('1.0', END)
        qlogTB.config(state=DISABLED)

    def displayAllEvents():
        insertQLog(qlogTB, "=========================================\nEvent Table contents sorted by StartTime:\n=========================================\n\n")

        try:
            cursor.execute("SELECT * FROM EVENT ORDER BY start_time")
            conn.commit()
        except mysql.connector.Error as err:
            print("Something went wrong: {}".format(err))
          
        for r in cursor:
            # r is a tuple of column values
            # this means that r has constant length
            insertQLog(qlogTB, "\n")
            insertQLog(qlogTB, r)
            insertQLog(qlogTB, "\n")
            
        insertQLog(qlogTB, "\n\n")

    def displayAllEventsWOrg():

        e22Buffer = str(e22.get())

        ## PARSE organizer
        if( len(e22Buffer) > 0 ):
            insertQLog(qlogTB, "============================================\nEvent Table contents with org_id = " + e22Buffer + " sorted\nby StartDate:\n============================================\n\n")
            
            try:
                cursor.execute("SELECT * FROM EVENT WHERE organizer = \'"+ e22Buffer +"\' ORDER BY event_id")
                conn.commit()
            except mysql.connector.Error as err:
                print("Something went wrong: {}".format(err))
            
            for r in cursor:
                # r is a tuple of column values
                # this means that r has constant length
                insertQLog(qlogTB, "\n")
                insertQLog(qlogTB, r)
                insertQLog(qlogTB, "\n")
                
            insertQLog(qlogTB, "\n\n")
        else:
            insertQLog(qlogTB, "Please enter org_id number\n\n")

    def displayLocation():
        # display location in specific region
        insertQLog(qlogTB, "============================================\nOrganizer Table contents matching\nevents with 'jon' in their location:\n============================================\n\n")
            
        try:
            cursor.execute("SELECT organizer FROM EVENT WHERE location LIKE '%Jon%'")
            conn.commit()
            x = cursor.fetchone()
            try:
                cursor.execute("SELECT * FROM ORGANIZER WHERE org_id = "+ str(x['organizer']) +" ORDER BY name")
                conn.commit()
                for r in cursor:
                    # r is a tuple of column values
                    # this means that r has constant length
                    insertQLog(qlogTB, "\n")
                    insertQLog(qlogTB, r)
                    insertQLog(qlogTB, "\n")
                insertQLog(qlogTB, "\n\n")
            except mysql.connector.Error as err:
                print("Something went wrong: {}".format(err))
        except mysql.connector.Error as err:
            print("Something went wrong: {}".format(err))


    def displayContact():
        # display Contacts with specific @ type (gmail/hotmail/etc)
        insertQLog(qlogTB, "============================================\nOrganizer Table contacts with\n'.ca' email extensions:\n============================================\n\n")
            
        try:
            cursor.execute("SELECT * FROM ORGANIZER WHERE contact LIKE '%.ca%' ORDER BY org_id")
            conn.commit()
        except mysql.connector.Error as err:
            print("Something went wrong: {}".format(err))
        
        for r in cursor:
            # r is a tuple of column values
            # this means that r has constant length
            insertQLog(qlogTB, "\n")
            insertQLog(qlogTB, r)
            insertQLog(qlogTB, "\n")
        insertQLog(qlogTB, "\n\n")

    def displayAlarm():
        # display all events with 3-7 alarms
        insertQLog(qlogTB, "============================================\nEvent Table contents containing 3-7 alarms:\n============================================\n\n")
            
        try:
            cursor.execute("SELECT * FROM EVENT WHERE num_alarms BETWEEN 3 and 7 ORDER BY num_alarms")
            conn.commit()
        except mysql.connector.Error as err:
            print("Something went wrong: {}".format(err))
        
        for r in cursor:
            # r is a tuple of column values
            # this means that r has constant length
            insertQLog(qlogTB, "\n")
            insertQLog(qlogTB, r)
            insertQLog(qlogTB, "\n")
        insertQLog(qlogTB, "\n\n")

    def executeQuery():
        string = str(e6.get())

        try:
            insertQLog(qlogTB, "============================================\n\t\tCustom Query:\t\t\n============================================\n\n")
            cursor.execute(string)
            conn.commit()
            for r in cursor:
                # r is a tuple of column values
                # this means that r has constant length
                insertQLog(qlogTB, "\n")
                insertQLog(qlogTB, r)
                insertQLog(qlogTB, "\n")
        except mysql.connector.Error:
            insertQLog(qlogTB, "Invalid Query Attempt\n")

    qWindow = Toplevel()

    w = 750  # width for the Tk root
    h = 500  # height for the Tk root

    # get screen width and height
    ws = qWindow.winfo_screenwidth()  # width of the screen
    hs = qWindow.winfo_screenheight()  # height of the screen

    # calculate x and y coordinates for the Tk root window
    x = (ws / 2) - (w / 2)
    y = (hs / 2) - (h / 2)

    # set the dimensions of the screen
    # and where it is placed
    qWindow.geometry('%dx%d+%d+%d' % (w, h, x, y))

    qWindow.title("Query Window")
    qWindow.minsize(750, 500)

    qWindow.maxsize(750, 500)
    qWindow.config(background='cyan')

    ## FRAMES

    topQueryDiv = Frame(qWindow, bg="cyan", height=100)
    topQueryDiv.pack(anchor=N, fill=X, side=TOP, expand=False)

    topBotDiv = Frame(qWindow, bg="black", height=1)
    topBotDiv.pack(anchor=N, fill=X, side=TOP, expand=False)

    botQueryDiv = Frame(qWindow, bg="cyan")
    botQueryDiv.pack(anchor=S, fill=BOTH, side=BOTTOM, expand=True)

    leftQueryDiv = Frame(botQueryDiv, bg="blue", relief=SUNKEN, bd=3, width=375)
    leftQueryDiv.pack(anchor=W, fill=Y, side=LEFT, expand=True)
    leftQueryDiv.pack_propagate(0)

    rightQueryDiv = Frame(botQueryDiv, bg="cyan", width=375)
    rightQueryDiv.pack(anchor=W, fill=BOTH, side=LEFT, expand=1)

    ## LABELS AND ENTRIES
    title = Label(topQueryDiv, text="Query Window", bg='cyan')
    title.config(font=("Courier", 22, "bold"))
    title.pack(anchor=N, side=TOP, fill=BOTH, pady=20)

    ## TEXT BOX
    def insertQLog(qlogTB, str):
        qlogTB.config(state=NORMAL)
        qlogTB.insert(END, str)
        qlogTB.config(state=DISABLED)

    qlogTB = Text(leftQueryDiv, relief=SUNKEN)
    qlogTB.pack(anchor=N, fill=BOTH, expand=True, side=TOP)
    qlogTB.config(state=DISABLED)

    botFill = Frame(leftQueryDiv, bg="#b3ffff", height=30)
    botFill.pack(anchor=S, fill=X, side=BOTTOM)

    clearLog = Button(botFill, text="Clear Query Log", bg='blue', fg='white', command=clearQLog)
    clearLog.pack(anchor=N, padx=50, pady=3)

    # Log TB
    qlogTB_scrollbar = Scrollbar(qlogTB)
    qlogTB_scrollbar.pack(side=RIGHT, fill=Y)
    qlogTB.config(yscrollcommand=qlogTB_scrollbar.set)
    qlogTB_scrollbar.config(command=qlogTB.yview)

    # LEFT SIDE Widgets
    Label(rightQueryDiv, text="Display All Events\nby StartDate", bg='cyan').grid(sticky=W, row=1, padx=15)
    Label(rightQueryDiv, text="Display All Events\nwith the selected \norganizer id", bg='cyan').grid(sticky=W, row=2, padx=15)
    Label(rightQueryDiv, text="Display All Organizers in\nevents with 'jon' in their location", bg='cyan').grid(sticky=W, row=3, padx=15)
    Label(rightQueryDiv, text="Display all Organizers\nwith '.ca' email extensions", bg='cyan').grid(sticky=W, row=4, padx=15)
    Label(rightQueryDiv, text="Display All Events\ncontaining 3-7 alarms", bg='cyan').grid(sticky=W, row=5, padx=15)
    e6 = Entry(rightQueryDiv)
    e6.insert(END, 'SELECT')
    e6.grid(sticky=W, row=7, padx=22)

    # Middle Widgets
    e22 = Entry(rightQueryDiv, width=3)
    e22.grid(sticky=W, row=2, column=1)

    b6 = Button(rightQueryDiv, text="Submit", bg='black', fg='white', command=executeQuery)
    b6.grid(sticky=W, row=7, column=2, pady=0)

    Frame(rightQueryDiv, bg="black", width=375).grid(sticky=N, row=6, columnspan = 3, pady=10)
    Label(rightQueryDiv, text="Query Submit Entry Field", bg='cyan', fg='blue', bd=2).grid(sticky=N, row=6, columnspan = 3, pady=20)
    #e33 = Entry(rightQueryDiv, width=3).grid(sticky=W, row=2, column=1)
    #e44 = Entry(rightQueryDiv, width=3).grid(sticky=W, row=2, column=1)

    # RIGHT SIDE Widgets
    def help():
        def closeWindow():
            aboutWindow.destroy()

        aboutWindow = Toplevel()

        w = 800  # width for the Tk root
        h = 200  # height for the Tk root

        # get screen width and height
        ws = aboutWindow.winfo_screenwidth()  # width of the screen
        hs = aboutWindow.winfo_screenheight()  # height of the screen

        # calculate x and y coordinates for the Tk root window
        x = (ws / 2) - (w / 2)
        y = (hs / 2) - (h / 2)

        # set the dimensions of the screen
        # and where it is placed
        aboutWindow.geometry('%dx%d+%d+%d' % (w, h, x, y))

        aboutWindow.title("Help")
        aboutWindow.minsize(800, 200)

        aboutWindow.maxsize(800, 200)
        aboutWindow.config(background='cyan')

        ## LABELS AND ENTRIES

        try:
            cursor.execute("DESCRIBE ORGANIZER")
            print("Search Successful\n")
            desORG = cursor.fetchone()
        except mysql.connector.Error as err:
            print("Something went wrong: {}".format(err))

        try:
            cursor.execute("DESCRIBE EVENT")
            print("Search Successful\n")
            desEVE = cursor.fetchone()
        except mysql.connector.Error as err:
            print("Something went wrong: {}".format(err))


        Label(aboutWindow, bg='cyan', width=5).grid(rowspan=7)
        Label(aboutWindow, text="ORGANIZER => ", bg='cyan').grid(sticky=W, row=1, column=1)
        Label(aboutWindow, text="EVENT => ", bg='cyan').grid(sticky=W, row=2, column=1)
        Label(aboutWindow, text=desORG, bg='cyan', pady=50).grid(sticky=W, row=1, column=2)
        Label(aboutWindow, text=desEVE, bg='cyan', pady=10).grid(sticky=W, row=2, column=2)


        ## Set the focus on dialog window (needed on Windows)
        aboutWindow.focus_set()
        ## Make sure events only go to our dialog
        aboutWindow.grab_set()
        ## Make sure dialog stays on top of its parent window (if needed)
        aboutWindow.transient(qWindow)

        # Close Window Event
        aboutWindow.bind("<Escape>", lambda _: closeWindow())

        ## Display the window and wait for it to close
        aboutWindow.wait_window(aboutWindow)

    b1 = Button(rightQueryDiv, text="Display", bg='blue', fg='white', command=displayAllEvents).grid(sticky=W, row=1, column=2, pady=5)
    b2 = Button(rightQueryDiv, text="Display", bg='blue', fg='white', command=displayAllEventsWOrg).grid(sticky=W, row=2, column=2, pady=35)
    b3 = Button(rightQueryDiv, text="Display", bg='blue', fg='white', command=displayLocation).grid(sticky=W, row=3, column=2)
    b4 = Button(rightQueryDiv, text="Display", bg='blue', fg='white', command=displayContact).grid(sticky=W, row=4, column=2, pady=40)
    b5 = Button(rightQueryDiv, text="Display", bg='blue', fg='white', command=displayAlarm).grid(sticky=W, row=5, column=2)
    b6 = Button(rightQueryDiv, text="Help", bg='red', fg='white', command=help).grid(sticky=W, row=6, column=2, pady=0)

    # Close Window Event
    qWindow.bind("<Escape>", lambda _: closeWindow())

def addAllEvents():
    global selectedEventNo, myClass

    x = numEvents(myClass.calPtr)

    for i in range(x):
        selectedEventNo = (i+1)
        addCurEvent()

def addCurEvent():
    global selectedEventNo, myClass

    string = getSOL(selectedEventNo, myClass)

    #token event

    event = string.split('|&&&|', 5)
    '''
    CRUD App
    Create
    read - (SELECT)
    Update
    Delete

    SELECT statements - reads in rows from Database
    SELECT * FROM table_name;
    SELECT * FROM organizer;
    '''
    # 1. If organizer is NULL, do nothing
    # 2. Otherwise, you can use this command:
    # SELECT * FROM organizer
    # WHERE name = event[2] AND   ## Select argument returned on true
    #       contact = event[3];

    datetime = event[3]

    #token datetime

    sdt = datetime.split('|DT|', 2)

    date = sdt[0]

    year = date[0:4]

    month = date[4:6]

    day = date[6:8]

    time = sdt[1]

    hour = time[0:2]

    minute = time[2:4]

    second = time[4:6]

    summaryBUF = event[0]

    if( len(summaryBUF) > 60 ):
        summary = summaryBUF[:60]
    else:
        summary = summaryBUF

    organizer = event[1]

    location = event[2]

    if(len(summary) == 0):
        summary = 'NULL'
    if(len(organizer) == 0):
        organizer == 'NULL'
    if(len(location) == 0):
        location = 'NULL'

    if(len(year) == 0 or len(month) == 0 or len(day) == 0 or len(hour) == 0 or len(minute) == 0 or len(second) == 0):
        datetimestring = 'NULL'
    else:
        datetimestring = year + "-" + month + "-" + day + " " + hour + ":" + minute + ":" + second

    ## PARSE organizer

    org = organizer.split(':MAILTO:', 2)

    name0 = org[0]
    name = name0[3:]
    contact = org[1]

    ####### Check if organizer is in ORGANIZER table #######
    #'''
    O_ID = checkOrgID(name, contact)
    #'''

    if(O_ID is None):
        orgObject =  "INSERT INTO ORGANIZER (name, contact) VALUES (\'" + name + "\', \'" + contact + "\')"

        # execute SQL query using execute() method.
        try:
            cursor.execute(orgObject)
            print("Added Successfully\n")
            conn.commit()
            insertLog("Event No "+ str(selectedEventNo) + "'s Organizer Added to Database\n")
            ####### Insert event into EVENT table #######
            EventCheck = checkEvent(summary, datetimestring)
            if(EventCheck is None):
                try:
                    EventCheck = checkEvent(summary, datetimestring)
                    cursor.execute("SELECT LAST_INSERT_ID()")
                    print("Successfully retrieved last Primary Key\n")
                    conn.commit()
                    ID = cursor.fetchone()
                    eventObject =  "INSERT INTO EVENT (summary, start_time, location, organizer, num_alarms) VALUES (\'" + summary + "\', \'" + datetimestring + "\', \'" + location + "\', \'"+ str(ID['LAST_INSERT_ID()']) +"\', " + event[4] + ")"
                    try:
                        cursor.execute(eventObject)
                        print("Added Successfully\n")
                        conn.commit()
                        insertLog("Event No "+ str(selectedEventNo) + " Added to Database\n\n")
                        displayDB()
                    except mysql.connector.Error as err:
                        print("Something went wrong: {}".format(err))
                except mysql.connector.Error as err:
                    print("Something went wrong: {}".format(err))
            else:
                insertLog("Event No "+ str(selectedEventNo) + " already exists in Database\n\n")
        except mysql.connector.Error as err:
            print("Something went wrong: {}".format(err))
    else:
        ####### Insert event into EVENT table #######
        EventCheck = checkEvent(summary, datetimestring)
        if(EventCheck is None):
            eventObject =  "INSERT INTO EVENT (summary, start_time, location, organizer, num_alarms) VALUES (\'" + summary + "\', \'" + datetimestring + "\', \'" + location + "\', \'"+ str(O_ID['org_id']) +"\', " + event[4] + ")"
            try:
                cursor.execute(eventObject)
                print("Added Successfully\n")
                conn.commit()
                insertLog("Event No "+ str(selectedEventNo) + " Added to Database\n\n")
                displayDB()
            except mysql.connector.Error as err:
                print("Something went wrong: {}".format(err))
        else:
            insertLog("Event No "+ str(selectedEventNo) + " already exists in Database\n\n")

def checkOrgID(nameStr, contactStr):
    search = "SELECT org_id\nFROM ORGANIZER\n\tWHERE name = \'" + nameStr + "\' AND\n\tcontact = \'" + contactStr + "\';"

    try:
        cursor.execute(search)
        print("Search Successful\n")
        result = cursor.fetchone()
        return result
    except mysql.connector.Error as err:
        return None

def checkEvent(summary, datetime):
    search = "SELECT *\nFROM EVENT\n\tWHERE summary = \'" + summary + "\' AND\n\tstart_time = \'" + datetime + "\';"

    try:
        cursor.execute(search)
        print("Search Successful\n")
        result = cursor.fetchone()
        return result
    except mysql.connector.Error as err:
        return None

def delData():
    #clear data from event table first
    try:
        cursor.execute("DELETE FROM EVENT")
        print("Deleted Successfully\n")
        conn.commit()
    except mysql.connector.Error as err:
        print("Something went wrong: {}".format(err))
    #clear data from organizer table second
    try:
        cursor.execute("DELETE FROM ORGANIZER")
        print("Deleted Successfully\n")
        conn.commit()
    except mysql.connector.Error as err:
        print("Something went wrong: {}".format(err)) 

    displayDB()


def endSQL():
    #clear data from event table first
    try:
        cursor.execute("DELETE FROM EVENT")
        print("Deleted Successfully\n")
        conn.commit()
    except mysql.connector.Error as err:
        print("Something went wrong: {}".format(err)) 
    # drop EVENT first
    '''
    try:
        cursor.execute("DROP TABLE event")
        print("Dropped Successfully\n")
        conn.commit()
    except mysql.connector.Error as err:
        print("Something went wrong: {}".format(err)) 
    '''
    #clear data from organizer table second
    try:
        cursor.execute("DELETE FROM ORGANIZER")
        print("Deleted Successfully\n")
        conn.commit()
    except mysql.connector.Error as err:
        print("Something went wrong: {}".format(err))
    # drop second
    '''
    try:
        cursor.execute("DROP TABLE organizer")
        print("Dropped Successfully\n")
        conn.commit()
    except mysql.connector.Error as err:
        print("Something went wrong: {}".format(err))
    '''

    displayDB()

    cursor.close()    
    conn.close()

def displayDB():
    printO()
    printE()
    try:
        cursor.execute("SELECT COUNT(org_id) FROM ORGANIZER")
        print("Displayed Successfully\n")
        conn.commit()
        r = cursor.fetchone()
        insertLog("Database has ")
        insertLog(r['COUNT(org_id)'])
        insertLog(" organizers")
    except mysql.connector.Error as err:
        print("Something went wrong: {}".format(err))

    try:
        cursor.execute("SELECT COUNT(event_id) FROM EVENT")
        print("Displayed Successfully\n")
        conn.commit()
        x = cursor.fetchone()
        insertLog(" and ")
        insertLog(x['COUNT(event_id)'])
        insertLog(" events\n\n")
    except mysql.connector.Error as err:
        print("Something went wrong: {}".format(err))


'''
def CreateQuery():
    queryWindow = Toplevel()

'''


#######################################################################

# Initialize Global Variables
calFlag = 0

fileName = ""

class CalClass:
    calPtr = POINTER(Calendar)()

class EventClass():
    UID = ""
    create = ""
    start = ""
    alarm = ""
    propertyE = "" 

class DateTimeClass():
    date = ""
    time = ""
    UTC = ""

class PropertyClass():
    propName = ""
    propDescr = "" 

class AlarmClass():
    action = ""
    trigger = ""
    propertyA = ""

class GlobEventClass():
    event = EventClass()

class GlobalDTClass():
    dt = DateTimeClass()

class GlobalAlarmClass():
    alarm = AlarmClass()

class GlobalPropClass():
    prop = PropertyClass()

calStr = ""

selectedEventNo = 1

errorCode = 0

# Simple Cal Variables

prodID = ""

# GLOBAL CAL POINTER Class protected from evil python memory management
myClass = CalClass()

globalEvent = GlobEventClass()

globalDT = GlobalDTClass()

globalAlarm = GlobalAlarmClass()

globalProp = GlobalPropClass()

calFlag = 0

def createSimpleCalendar(prodID):
    global logTB, myClass, calFlag
    clearErrorLog()

    if(calFlag == 1):
        CurCal()
        deleteC(myClass)
        calFlag = 0

    if(prodID):
        createUICalendar(prodID, myClass)
        calFlag = 1
        printErrorLog()
        #printCalScreen(myClass)
        printCalFileTV(myClass)

def addEventToCal(globalEvent):
    global myClass
    clearErrorLog()

    if(globalEvent.UID):
        createUIEvent(globalEvent, myClass)
        #printCalScreen(myClass)
        printCalFileTV(myClass)

def addAlarmToCal(globalAlarm):
    global myClass, selectedEventNo
    clearErrorLog()

    if(globalAlarm.action and globalAlarm.trigger):
        createUIAlarm(selectedEventNo, myClass, globalAlarm)
        #printCalScreen(myClass)
        printCalFileTV(myClass)

def createCalUI():
    global errorCode, myClass, calFlag

    clearErrorLog()

    if(calFlag == 1):
        CurCal()
        deleteC(myClass)
        calFlag = 0
        
    errorCode = createCalendar(fileName, myClass)
    if(errorCode == 0):
        calFlag = 1
        printCalFileTV(myClass)
    else:
        insertLog("Failed to read file.\n")

## Event Functions 
def deleteEventUI():
    global selectedEventNo, myClass

    deleteEvent(selectedEventNo, myClass)
    printCalFileTV(myClass)
    clearErrorLog()

## FILE MANIPULATION
def setUIWindowTitleToFilename():
    file = fileName[(fileName.rfind('/')) + 1:]
    mainWindow.title(file)

def printFilename():
    file = fileName[(fileName.rfind('/')) + 1:]
    insertLog('File [%s] loaded\n' % file)

def fileOpen(event=None):
    global fileName, errorCode

    fileName = askopenfilename(title="Select file",
                               filetypes=[("ICal files", "*.ics")])
    if (fileName):
        createCalUI()
        if(errorCode == 0):
            setUIWindowTitleToFilename()
            calendarExists()
            printErrorLog()
            printFilename()

def fileSave(event=None):
    global myClass, logTB
    if(myClass.calPtr):
        if(fileName):
            errorCode = saveToFile(fileName, myClass)
            printErrorLog()
        else:
            fileSaveAs()
    else:
        insertLog("No Calendar,\nplease open a calendar file or create a new calendar.\n")

def fileSaveAs(event=None):
    global fileName, myClass, logTB
    if(myClass.calPtr):
        fileName = asksaveasfilename(title="Select file",
                                     filetypes=[("ICal files", "*.ics")])
        if(fileName):
            errorCode = saveToFile(fileName, myClass)
            printErrorLog()
    else:
        insertLog("No Calendar,\nplease open a calendar file or create a new calendar.\n")

## PRINT FUNCTIONS
def printCalScreen(myClass):
    if(myClass.calPtr):
        print(printCalendar(myClass))


def printAlarmToLog():  
    global logTB, myClass, selectedEventNo

    if(myClass.calPtr):

        string = printEventAlarms(selectedEventNo, myClass.calPtr)

        insertLog(string)

def printPropToLog():  
    global logTB, myClass

    if(myClass.calPtr):

        string = printEventProps(selectedEventNo, myClass.calPtr)

        insertLog(string)


def printCalFileTV(myClass):
    global fileTV

    fileTV.delete(*fileTV.get_children())
    
    x = numEvents(myClass.calPtr)


    if(x > 0):
        EventExists()
    if(x == 0):
        EventDoesntExist()

    if(myClass.calPtr):

        for i in range(x):
            string = eventStats(i, myClass)
            EventState = string.split('|&&&|', 3)

            totalProperties = int(EventState[0]) + 3

            fileTV.insert('', 'end', text=i+1, values=(str(totalProperties), EventState[1], EventState[2]))


def printErrorLog():

    errStr = printError(errorCode)

    errStr += "\n"

    insertLog(errStr)

## CLEAR FUNCTIONS
def clearErrorLog():
    global logTB
    logTB.config(state=NORMAL)
    logTB.delete('1.0', END)
    logTB.config(state=DISABLED)

def clearCalPtr():
    myClass.calPtr = POINTER(Calendar)

# FLAG FUNCTIONS
def calendarExists():
    subMenu2.entryconfig(1, state=NORMAL)
    subMenu1.entryconfig(1, state=NORMAL)
    subMenu1.entryconfig(2, state=NORMAL)

def EventExists():
    subMenu3.entryconfig(1, state=NORMAL)
    subMenu3.entryconfig(0, state=NORMAL)
    subMenu2.entryconfig(1, state=NORMAL)
    subMenu2.entryconfig(2, state=NORMAL)
    showAlarmsButton.config(state=NORMAL)
    showPropsButton.config(state=NORMAL)
    delEventButton.config(state=NORMAL)

def EventDoesntExist():
    subMenu2.entryconfig(2, state=DISABLED)
    showAlarmsButton.config(state=DISABLED)
    showPropsButton.config(state=DISABLED)
    delEventButton.config(state=DISABLED)

##################
# GUI Appearance #
##################

# create the application
mainWindow = Tk()

w = 900 # width for the Tk root
h = 650 # height for the Tk root

# get screen width and height
ws = mainWindow.winfo_screenwidth() # w
# Width of the screen
hs = mainWindow.winfo_screenheight() # height of the screen

# calculate x and y coordinates for the Tk root window
x = (ws/2) - (w/2)
y = (hs/2) - (h/2)

# set the dimensions of the screen
# and where it is placed
mainWindow.geometry('%dx%d+%d+%d' % (w, h, x, y))

def createCalendarWindow():
    global calFlag

    #if(calFlag):
        #CurCal()

    def closeWindow():
        createCalendarWindow.destroy()

    def addCalendar():
        global prodID, myClass

        if(myClass.calPtr):
            CurCal()

        x = e1.get()
        createCalendarWindow.destroy()
        if(x):
            global prodID
            try:
                prodID = x
                createSimpleCalendar(prodID)
                calendarExists()
                calFlag = 1
            except ValueError:
                insertLog("Error\n")
        else:
            insertLog("No prodID entered, please try again\n")

    createCalendarWindow = Toplevel()

    w = 320  # width for the Tk root
    h = 300  # height for the Tk root

    # get screen width and height
    ws = createCalendarWindow.winfo_screenwidth()  # width of the screen
    hs = createCalendarWindow.winfo_screenheight()  # height of the screen

    # calculate x and y coordinates for the Tk root window
    x = (ws / 2) - (w / 2)
    y = (hs / 2) - (h / 2)

    # set the dimensions of the screen
    # and where it is placed
    createCalendarWindow.geometry('%dx%d+%d+%d' % (w, h, x, y))

    createCalendarWindow.title("Create Calendar UI")
    createCalendarWindow.minsize(320, 250)
    createCalendarWindow.maxsize(320, 250)
    createCalendarWindow.config(background='cyan')

    ## LABELS AND ENTRIES
    title = Label(createCalendarWindow, text="New Calendar", bg='cyan')
    title.config(font=("Courier", 22, "bold"))
    title.grid(columnspan=4, padx=40, pady=20)

    Label(createCalendarWindow, bg='cyan', width=2).grid(rowspan=7)
    Label(createCalendarWindow, text="Please enter the calendar information below.\n\n", bg='cyan').grid(sticky=W, row=1, column=1, columnspan=2)
    Label(createCalendarWindow, text="Version Automatically set to '2.0'", bg='cyan').grid(sticky=W, row=2, column=1, columnspan=2, pady=3)
    Label(createCalendarWindow, text="Product ID", bg='cyan').grid(sticky=W, row=3, column=1, pady=3)

    e1 = Entry(createCalendarWindow)
    
    e1.grid(row=3, column=2)

    # Add Button
    button = Button(createCalendarWindow, bg='black', fg='white', text='OK', relief=RAISED, command=addCalendar)
    button.config(font=1)
    button.grid(columnspan=5, padx=60, pady=20)

    ## Set the focus on dialog window (needed on Windows)
    createCalendarWindow.focus_set()
    ## Make sure events only go to our dialog
    createCalendarWindow.grab_set()
    ## Make sure dialog stays on top of its parent window (if needed)
    createCalendarWindow.transient(mainWindow)
    # Close Window Event
    createCalendarWindow.bind('<Escape>', lambda _: closeWindow())
    ## Display the window and wait for it to close
    createCalendarWindow.wait_window(createCalendarWindow)


# COMMENTED OUT EXTRA FUNCTIONALITY (Create Full Calendar, Events/Alarms within UI's only)

'''
    # Property UI
    def createPropertyCal():
    
        def addProp():
            print("Prop Name " + e1.get())
            print("Prop Descr " + e2.get())
            propertyWindow.destroy()

        propertyWindow = Toplevel()

        w = 350  # width for the Tk root
        h = 300  # height for the Tk root

        # get screen width and height
        ws = propertyWindow.winfo_screenwidth()  # width of the screen
        hs = propertyWindow.winfo_screenheight()  # height of the screen

        # calculate x and y coordinates for the Tk root window
        x = (ws / 2) - (w / 2)
        y = (hs / 2) - (h / 2)

        # set the dimensions of the screen
        # and where it is placed
        propertyWindow.geometry('%dx%d+%d+%d' % (w, h, x, y))

        propertyWindow.title("Property UI")
        propertyWindow.minsize(350, 300)
        propertyWindow.maxsize(350, 300)
        propertyWindow.config(background='cyan')

        ## LABELS AND ENTRIES
        title = Label(propertyWindow, text="Create Property", bg='cyan')
        title.config(font=("Courier", 22, "bold"))
        title.grid(column=1, sticky=W, columnspan=4, pady=20)

        Label(propertyWindow, bg='cyan', width=2).grid(rowspan=7)
        Label(propertyWindow, text="Please enter the property information below.\n\n", bg='cyan').grid(sticky=W, row=1, column=1, columnspan=2)
        Label(propertyWindow, text="Property Name", bg='cyan').grid(sticky=W, row=3, column=1, pady=3)
        Label(propertyWindow, text="Property Description", bg='cyan').grid(sticky=W, row=4, column=1, pady=3)

        e1 = Entry(propertyWindow)
        e2 = Entry(propertyWindow)

        e1.grid(row=3, column=2)
        e2.grid(row=4, column=2)

        # Add Button
        button = Button(propertyWindow, bg='black', fg='white', text='ADD', relief=RAISED, command=addProp)
        button.config(font=1)
        button.grid(column=1, row=6, columnspan=6, padx=50, pady=30)

        ## Set the focus on dialog window (needed on Windows)
        propertyWindow.focus_set()
        ## Make sure events only go to our dialog
        propertyWindow.grab_set()
        ## Make sure dialog stays on top of its parent window (if needed)
        propertyWindow.transient(mainWindow)
        propertyWindow.transient(createCalendarWindow)
        ## Display the window and wait for it to close
        propertyWindow.wait_window(propertyWindow)

    def createEventWindowCal():
    
        def addEvent():
            print("UID " + e1.get())
            createEventWindow.destroy()
    
        createEventWindow = Toplevel()

        # Create Date Time UI
        def CDT():
        
            def addCDT():
                print("Date " + e1.get())
                print("Time " + e2.get())
                CDTWindow.destroy()
        
            CDTWindow = Toplevel()

            w = 340  # width for the Tk root
            h = 300  # height for the Tk root

            # get screen width and height
            ws = CDTWindow.winfo_screenwidth()  # width of the screen
            hs = CDTWindow.winfo_screenheight()  # height of the screen

            # calculate x and y coordinates for the Tk root window
            x = (ws / 2) - (w / 2)
            y = (hs / 2) - (h / 2)

            # set the dimensions of the screen
            # and where it is placed
            CDTWindow.geometry('%dx%d+%d+%d' % (w, h, x, y))

            CDTWindow.title("CreateDateTime UI")
            CDTWindow.minsize(340, 300)
            CDTWindow.maxsize(340, 300)
            CDTWindow.config(background='cyan')

            ## LABELS AND ENTRIES
            title = Label(CDTWindow, text="Create Date/Time", bg='cyan')
            title.config(font=("Courier", 22, "bold"))
            title.grid(sticky=N, columnspan=4, pady=20, padx=20)

            Label(CDTWindow, bg='cyan', width=2).grid(rowspan=7)
            Label(CDTWindow, text="Please enter the date/time information below.\n\n", bg='cyan').grid(sticky=N, row=1, columnspan=3)
            Label(CDTWindow, text="Date", bg='cyan').grid(sticky=N, row=3, column=1, pady=3)
            Label(CDTWindow, text="Time", bg='cyan').grid(sticky=N, row=4, column=1, pady=3)
            Label(CDTWindow, text="UTC Time?", bg='cyan').grid(sticky=N, row=5, column=1, pady=3)

            var = IntVar()

            e1 = Entry(CDTWindow)
            e2 = Entry(CDTWindow)
            c = Checkbutton(CDTWindow, bg='cyan', variable=var)

            e1.grid(row=3, column=2)
            e2.grid(row=4, column=2)
            c.grid(sticky=N, row=5, column=2, pady=3)

            # Add Button
            button = Button(CDTWindow, bg='black', fg='white', text='ADD', relief=RAISED, command=addCDT)
            button.config(font=1)
            button.grid(row=6, columnspan=6, padx=50, pady=30)

            ## Set the focus on dialog window (needed on Windows)
            CDTWindow.focus_set()
            ## Make sure events only go to our dialog
            CDTWindow.grab_set()
            ## Make sure dialog stays on top of its parent window (if needed)
            CDTWindow.transient(mainWindow)
            CDTWindow.transient(createEventWindow)
            ## Display the window and wait for it to close
            CDTWindow.wait_window(CDTWindow)

        # Start Date Time UI
        def SDT():
        
            def addSDT():
                print("Date " + e1.get())
                print("Time " + e2.get())
                SDTWindow.destroy()
        
            SDTWindow = Toplevel()

            w = 340  # width for the Tk root
            h = 300  # height for the Tk root

            # get screen width and height
            ws = SDTWindow.winfo_screenwidth()  # width of the screen
            hs = SDTWindow.winfo_screenheight()  # height of the screen

            # calculate x and y coordinates for the Tk root window
            x = (ws / 2) - (w / 2)
            y = (hs / 2) - (h / 2)

            # set the dimensions of the screen
            # and where it is placed
            SDTWindow.geometry('%dx%d+%d+%d' % (w, h, x, y))

            SDTWindow.title("CreateDateTime UI")
            SDTWindow.minsize(340, 300)
            SDTWindow.maxsize(340, 300)
            SDTWindow.config(background='cyan')

            ## LABELS AND ENTRIES
            title = Label(SDTWindow, text="Start Date/Time", bg='cyan')
            title.config(font=("Courier", 22, "bold"))
            title.grid(sticky=N, columnspan=4, pady=20, padx=20)

            Label(SDTWindow, bg='cyan', width=2).grid(rowspan=7)
            Label(SDTWindow, text="Please enter the date/time information below.\n\n", bg='cyan').grid(sticky=N, row=1, columnspan=3)
            Label(SDTWindow, text="Date", bg='cyan').grid(sticky=N, row=3, column=1, pady=3)
            Label(SDTWindow, text="Time", bg='cyan').grid(sticky=N, row=4, column=1, pady=3)
            Label(SDTWindow, text="UTC Time?", bg='cyan').grid(sticky=N, row=5, column=1, pady=3)

            var = IntVar()

            e1 = Entry(SDTWindow)
            e2 = Entry(SDTWindow)
            c = Checkbutton(SDTWindow, bg='cyan', variable=var)

            e1.grid(row=3, column=2)
            e2.grid(row=4, column=2)
            c.grid(sticky=N, row=5, column=2, pady=3)

            # Add Button
            button = Button(SDTWindow, bg='black', fg='white', text='ADD', relief=RAISED, command=addSDT)
            button.config(font=1)
            button.grid(row=6, columnspan=6, padx=50, pady=30)

            ## Set the focus on dialog window (needed on Windows)
            SDTWindow.focus_set()
            ## Make sure events only go to our dialog
            SDTWindow.grab_set()
            ## Make sure dialog stays on top of its parent window (if needed)
            SDTWindow.transient(mainWindow)
            SDTWindow.transient(createEventWindow)
            ## Display the window and wait for it to close
            SDTWindow.wait_window(SDTWindow)

        # Property UI
        def createPropertyEvent():
        
            def addProp():
                print("Prop Name " + e1.get())
                print("Prop Descr " + e2.get())
                propertyWindow.destroy()
        
            propertyWindow = Toplevel()

            w = 350  # width for the Tk root
            h = 300  # height for the Tk root

            # get screen width and height
            ws = propertyWindow.winfo_screenwidth()  # width of the screen
            hs = propertyWindow.winfo_screenheight()  # height of the screen

            # calculate x and y coordinates for the Tk root window
            x = (ws / 2) - (w / 2)
            y = (hs / 2) - (h / 2)

            # set the dimensions of the screen
            # and where it is placed
            propertyWindow.geometry('%dx%d+%d+%d' % (w, h, x, y))

            propertyWindow.title("Property UI")
            propertyWindow.minsize(350, 300)
            propertyWindow.maxsize(350, 300)
            propertyWindow.config(background='cyan')

            ## LABELS AND ENTRIES
            title = Label(propertyWindow, text="Create Property", bg='cyan')
            title.config(font=("Courier", 22, "bold"))
            title.grid(column=1, sticky=W, columnspan=4, pady=20)

            Label(propertyWindow, bg='cyan', width=2).grid(rowspan=7)
            Label(propertyWindow, text="Please enter the property information below.\n\n", bg='cyan').grid(sticky=W, row=1, column=1, columnspan=2)
            Label(propertyWindow, text="Property Name", bg='cyan').grid(sticky=W, row=3, column=1, pady=3)
            Label(propertyWindow, text="Property Description", bg='cyan').grid(sticky=W, row=4, column=1, pady=3)

            e1 = Entry(propertyWindow)
            e2 = Entry(propertyWindow)

            e1.grid(row=3, column=2)
            e2.grid(row=4, column=2)

            # Add Button
            button = Button(propertyWindow, bg='black', fg='white', text='ADD', relief=RAISED, command=addProp)
            button.config(font=1)
            button.grid(column=1, row=6, columnspan=6, padx=50, pady=30)

            ## Set the focus on dialog window (needed on Windows)
            propertyWindow.focus_set()
            ## Make sure events only go to our dialog
            propertyWindow.grab_set()
            ## Make sure dialog stays on top of its parent window (if needed)
            propertyWindow.transient(mainWindow)
            propertyWindow.transient(createEventWindow)
            ## Display the window and wait for it to close
            propertyWindow.wait_window(propertyWindow)

        def createAlarmWindowEvent():
        
            def addAlarm():
                print("Action " + e1.get())
                print("Trigger " + e2.get())
                createAlarmWindow.destroy()
        
            createAlarmWindow = Toplevel()

            # Property UI
            def createPropertyAlarm():
            
                def addProp():
                    print("Prop Name " + e1.get())
                    print("Prop Descr " + e2.get())
                    propertyWindow.destroy()
            
                propertyWindow = Toplevel()

                w = 350  # width for the Tk root
                h = 300  # height for the Tk root

                # get screen width and height
                ws = propertyWindow.winfo_screenwidth()  # width of the screen
                hs = propertyWindow.winfo_screenheight()  # height of the screen

                # calculate x and y coordinates for the Tk root window
                x = (ws / 2) - (w / 2)
                y = (hs / 2) - (h / 2)

                # set the dimensions of the screen
                # and where it is placed
                propertyWindow.geometry('%dx%d+%d+%d' % (w, h, x, y))

                propertyWindow.title("Property UI")
                propertyWindow.minsize(350, 300)
                propertyWindow.maxsize(350, 300)
                propertyWindow.config(background='cyan')

                ## LABELS AND ENTRIES
                title = Label(propertyWindow, text="Create Property", bg='cyan')
                title.config(font=("Courier", 22, "bold"))
                title.grid(column=1, sticky=W, columnspan=4, pady=20)

                Label(propertyWindow, bg='cyan', width=2).grid(rowspan=7)
                Label(propertyWindow, text="Please enter the property information below.\n\n", bg='cyan').grid(sticky=W, row=1, column=1, columnspan=2)
                Label(propertyWindow, text="Property Name", bg='cyan').grid(sticky=W, row=3, column=1, pady=3)
                Label(propertyWindow, text="Property Description", bg='cyan').grid(sticky=W, row=4, column=1, pady=3)

                e1 = Entry(propertyWindow)
                e2 = Entry(propertyWindow)

                e1.grid(row=3, column=2)
                e2.grid(row=4, column=2)

                # Add Button
                button = Button(propertyWindow, bg='black', fg='white', text='ADD', relief=RAISED, command=addProp)
                button.config(font=1)
                button.grid(column=1, row=6, columnspan=6, padx=50, pady=30)

                ## Set the focus on dialog window (needed on Windows)
                propertyWindow.focus_set()
                ## Make sure events only go to our dialog
                propertyWindow.grab_set()
                ## Make sure dialog stays on top of its parent window (if needed)
                propertyWindow.transient(mainWindow)
                propertyWindow.transient(createAlarmWindow)
                ## Display the window and wait for it to close
                propertyWindow.wait_window(propertyWindow)

            w = 350  # width for the Tk root
            h = 300  # height for the Tk root

            # get screen width and height
            ws = createAlarmWindow.winfo_screenwidth()  # width of the screen
            hs = createAlarmWindow.winfo_screenheight()  # height of the screen

            # calculate x and y coordinates for the Tk root window
            x = (ws / 2) - (w / 2)
            y = (hs / 2) - (h / 2)

            # set the dimensions of the screen
            # and where it is placed
            createAlarmWindow.geometry('%dx%d+%d+%d' % (w, h, x, y))

            createAlarmWindow.title("Create Alarm UI")
            createAlarmWindow.minsize(350, 300)

            createAlarmWindow.maxsize(350, 300)
            createAlarmWindow.config(background='cyan')

            ## LABELS AND ENTRIES
            title = Label(createAlarmWindow, text="New Alarm", bg='cyan')
            title.config(font=("Courier", 22, "bold"))
            title.grid(column=1, columnspan=4, padx=40, pady=20)

            Label(createAlarmWindow, bg='cyan', width=2).grid(rowspan=7)
            Label(createAlarmWindow, text="Please enter the alarm information below.\n\n", bg='cyan').grid(sticky=W, row=1, column=1, columnspan=2)
            Label(createAlarmWindow, text="Action", bg='cyan').grid(sticky=W, row=2, column=1, pady=3)
            Label(createAlarmWindow, text="Trigger", bg='cyan').grid(sticky=W, row=3, column=1, pady=3)
            Label(createAlarmWindow, text="Add Alarm Property?", bg='cyan').grid(sticky=W, row=4, column=1, pady=3)

            e1 = Entry(createAlarmWindow)
            e2 = Entry(createAlarmWindow)
            b3 = Button(createAlarmWindow, bg='blue', fg='white', text='ADD', relief=RAISED, command=createPropertyAlarm)

            b3.config()

            e1.grid(row=2, column=2)
            e2.grid(row=3, column=2)
            b3.grid(row=4, column=2)

            # Add Button
            button = Button(createAlarmWindow, bg='black', fg='white', text='ADD', relief=RAISED, command=addAlarm)
            button.config(font=1)
            button.grid(sticky=N, column=1, columnspan=5, padx=90, pady=20)

            ## Set the focus on dialog window (needed on Windows)
            createAlarmWindow.focus_set()
            ## Make sure events only go to our dialog
            createAlarmWindow.grab_set()
            ## Make sure dialog stays on top of its parent window (if needed)
            createAlarmWindow.transient(mainWindow)
            createAlarmWindow.transient(createEventWindow)
            ## Display the window and wait for it to close
            createAlarmWindow.wait_window(createAlarmWindow)

        w = 360  # width for the Tk root
        h = 300  # height for the Tk root

        # get screen width and height
        ws = createEventWindow.winfo_screenwidth()  # width of the screen
        hs = createEventWindow.winfo_screenheight()  # height of the screen

        # calculate x and y coordinates for the Tk root window
        x = (ws / 2) - (w / 2)
        y = (hs / 2) - (h / 2)

        # set the dimensions of the screen
        # and where it is placed
        createEventWindow.geometry('%dx%d+%d+%d' % (w, h, x, y))

        createEventWindow.title("Create Event UI")
        createEventWindow.minsize(360, 350)
        createEventWindow.maxsize(360, 350)
        createEventWindow.config(background='cyan')

        ## LABELS AND ENTRIES
        title = Label(createEventWindow, text="New Event", bg='cyan')
        title.config(font=("Courier", 22, "bold"))
        title.grid(columnspan=4, padx=70, pady=20)

        Label(createEventWindow, bg='cyan', width=2).grid(rowspan=7)
        Label(createEventWindow, text="Please enter the event information below.\n\n", bg='cyan').grid(sticky=W, row=1, column=1, columnspan=2)
        Label(createEventWindow, text="UID", bg='cyan').grid(sticky=W, row=2, column=1, pady=3)
        Label(createEventWindow, text="Event Creation Date/Time", bg='cyan').grid(sticky=W, row=3, column=1, pady=3)
        Label(createEventWindow, text="Event Start Date/Time", bg='cyan').grid(sticky=W, row=4, column=1, pady=3)
        Label(createEventWindow, text="Add Event Alarm?", bg='cyan').grid(sticky=W, row=5, column=1, pady=3)
        Label(createEventWindow, text="Add Event Property?", bg='cyan').grid(sticky=W, row=6, column=1, pady=3)

        e1 = Entry(createEventWindow)
        b2 = Button(createEventWindow, bg='blue', fg='white', text='ADD', relief=RAISED, command=CDT)
        b3 = Button(createEventWindow, bg='blue', fg='white', text='ADD', relief=RAISED, command=SDT)
        b4 = Button(createEventWindow, bg='blue', fg='white', text='ADD', relief=RAISED, command=createAlarmWindowEvent)
        b5 = Button(createEventWindow, bg='blue', fg='white', text='ADD', relief=RAISED, command=createPropertyEvent)

        b2.config()
        b3.config()
        b4.config()
        b5.config()

        e1.grid(row=2, column=2)
        b2.grid(row=3, column=2)
        b3.grid(row=4, column=2)
        b4.grid(row=5, column=2)
        b5.grid(row=6, column=2)

        # Add Button
        button = Button(createEventWindow, bg='black', fg='white', text='ADD', relief=RAISED, command=addEvent)
        button.config(font=1)
        button.grid(columnspan=4, padx=40, pady=20)

        ## Set the focus on dialog window (needed on Windows)
        createEventWindow.focus_set()
        ## Make sure events only go to our dialog
        createEventWindow.grab_set()
        ## Make sure dialog stays on top of its parent window (if needed)
        createEventWindow.transient(mainWindow)
        createEventWindow.transient(createCalendarWindow)
        ## Display the window and wait for it to close
        createEventWindow.wait_window(createEventWindow)



    w = 350  # width for the Tk root
    h = 300  # height for the Tk root

    # get screen width and height
    ws = createCalendarWindow.winfo_screenwidth()  # width of the screen
    hs = createCalendarWindow.winfo_screenheight()  # height of the screen

    # calculate x and y coordinates for the Tk root window
    x = (ws / 2) - (w / 2)
    y = (hs / 2) - (h / 2)

    # set the dimensions of the screen
    # and where it is placed
    createCalendarWindow.geometry('%dx%d+%d+%d' % (w, h, x, y))

    createCalendarWindow.title("Create Calendar UI")
    createCalendarWindow.minsize(350, 300)
    createCalendarWindow.maxsize(350, 300)
    createCalendarWindow.config(background='cyan')

    ## LABELS AND ENTRIES
    title = Label(createCalendarWindow, text="New Calendar", bg='cyan')
    title.config(font=("Courier", 22, "bold"))
    title.grid(columnspan=4, padx=40, pady=20)

    Label(createCalendarWindow, bg='cyan', width=2).grid(rowspan=7)
    Label(createCalendarWindow, text="Please enter the calendar information below.\n\n", bg='cyan').grid(sticky=W, row=1, column=1, columnspan=2)
    Label(createCalendarWindow, text="Version Automatically set to '2.0'", bg='cyan').grid(sticky=W, row=2, column=1, columnspan=2, pady=3)
    Label(createCalendarWindow, text="Product ID", bg='cyan').grid(sticky=W, row=3, column=1, pady=3)
    Label(createCalendarWindow, text="Add Calendar Event?", bg='cyan').grid(sticky=W, row=4, column=1, pady=3)
    Label(createCalendarWindow, text="Add Calendar Property?", bg='cyan').grid(sticky=W, row=5, column=1, pady=3)

    e1 = Entry(createCalendarWindow)
    b2 = Button(createCalendarWindow, bg='blue', fg='white', text='ADD', relief=RAISED, command=createEventWindowCal)
    b3 = Button(createCalendarWindow, bg='blue', fg='white', text='ADD', relief=RAISED, command=createPropertyCal)

    b2.config()
    b3.config()

    e1.grid(row=3, column=2)
    b2.grid(row=4, column=2)
    b3.grid(row=5, column=2)

    # Add Button
    button = Button(createCalendarWindow, bg='black', fg='white', text='SAVE', relief=RAISED, command=addCalendar)
    button.config(font=1)
    button.grid(columnspan=4, padx=40, pady=20)

    ## Set the focus on dialog window (needed on Windows)
    createCalendarWindow.focus_set()
    ## Make sure events only go to our dialog
    createCalendarWindow.grab_set()
    ## Make sure dialog stays on top of its parent window (if needed)
    createCalendarWindow.transient(mainWindow)
    ## Display the window and wait for it to close
    createCalendarWindow.wait_window(createCalendarWindow)

'''

def createEventWindow():
    global globalEvent

    globalEvent.event = EventClass()

    def closeWindow():
        createEventWindow.destroy()

    def addEvent():
        global myClass, globalEvent

        clearErrorLog()
        globalEvent.event.UID = e1.get()
        if(len(globalEvent.event.UID) < 1):
            insertLog("No UID entered, please try again\n")
        elif(len(globalEvent.event.create) < 16):
            insertLog("No Creation Date/Time entered, please try again\n")
        elif(len(globalEvent.event.start) < 16):
            insertLog("No Start Date/Time entered, please try again\n")
        else:
            try:
                addEventToCal(globalEvent.event)
                EventExists()
                createEventWindow.destroy()
            except ValueError:
                insertLog("Error adding event to calendar\n")
            

    createEventWindow = Toplevel()

    # Create Date Time UI
    def CDTAdded():
        b2.config(state=DISABLED)
    def CDT():

        def closeWindow():
            CDTWindow.destroy()
    
        def addCDT():
            global globalEvent

            clearErrorLog()
            dt = DateTimeClass()

            dt.date = e1.get()
            dt.time = e2.get()
            if(var.get() == 1):
                dt.UTC = str(1)
            else: 
                dt.UTC = str(0)
            if(len(dt.date) != 8 or not(dt.date.isdigit()) ):
                insertLog("Invalid date, please try again\n")
            elif(len(dt.time) != 6 or not(dt.time.isdigit()) ):
                insertLog("Invalid time, please try again\n")
            else:
                globalEvent.event.create = dt.date + "~" + dt.time + "~" + dt.UTC + "~"
                CDTWindow.destroy()
                CDTAdded()
    
        CDTWindow = Toplevel()

        w = 340  # width for the Tk root
        h = 300  # height for the Tk root

        # get screen width and height
        ws = CDTWindow.winfo_screenwidth()  # width of the screen
        hs = CDTWindow.winfo_screenheight()  # height of the screen

        # calculate x and y coordinates for the Tk root window
        x = (ws / 2) - (w / 2)
        y = (hs / 2) - (h / 2)

        # set the dimensions of the screen
        # and where it is placed
        CDTWindow.geometry('%dx%d+%d+%d' % (w, h, x, y))

        CDTWindow.title("CreateDateTime UI")
        CDTWindow.minsize(340, 300)
        CDTWindow.maxsize(340, 300)
        CDTWindow.config(background='cyan')

        ## LABELS AND ENTRIES
        title = Label(CDTWindow, text="Create Date/Time", bg='cyan')
        title.config(font=("Courier", 22, "bold"))
        title.grid(sticky=N, columnspan=4, pady=20, padx=20)

        Label(CDTWindow, bg='cyan', width=2).grid(rowspan=7)
        Label(CDTWindow, text="Please enter the date/time information below.\n\n", bg='cyan').grid(sticky=N, row=1,
                                                                                                   columnspan=3)
        Label(CDTWindow, text="Date (YYYYMMDD)", bg='cyan').grid(sticky=N, row=3, column=1, pady=3)
        Label(CDTWindow, text="Time (hhmmss)", bg='cyan').grid(sticky=N, row=4, column=1, pady=3)
        Label(CDTWindow, text="UTC Time?", bg='cyan').grid(sticky=N, row=5, column=1, pady=3)

        var = IntVar()

        e1 = Entry(CDTWindow)
        e2 = Entry(CDTWindow)
        c = Checkbutton(CDTWindow, bg='cyan', variable=var)

        e1.grid(row=3, column=2)
        e2.grid(row=4, column=2)
        c.grid(sticky=N, row=5, column=2, pady=3)

        # Add Button
        button = Button(CDTWindow, bg='black', fg='white', text='ADD', relief=RAISED, command=addCDT)
        button.config(font=1)
        button.grid(row=6, columnspan=6, padx=50, pady=30)

        ## Set the focus on dialog window (needed on Windows)
        CDTWindow.focus_set()
        ## Make sure events only go to our dialog
        CDTWindow.grab_set()
        ## Make sure dialog stays on top of its parent window (if needed)
        CDTWindow.transient(mainWindow)
        CDTWindow.transient(createEventWindow)

        # Close Window Event
        CDTWindow.bind("<Escape>", lambda _: closeWindow())

        ## Display the window and wait for it to close
        CDTWindow.wait_window(CDTWindow)


    # Start Date Time UI
    def SDTAdded():
        b3.config(state=DISABLED)

    def SDT():

        def closeWindow():
            SDTWindow.destroy()

        def addSDT():
            global globalEvent

            clearErrorLog()
            dt = DateTimeClass()

            dt.date = e1.get()
            dt.time = e2.get()
            if(var.get() == 1):
                dt.UTC = str(1)
            else: 
                dt.UTC = str(0)
            if(len(dt.date) != 8 or not(dt.date.isdigit()) ):
                insertLog("Invalid date, please try again\n")
            elif(len(dt.time) != 6 or not(dt.time.isdigit()) ):
                insertLog("Invalid time, please try again\n")
            else:
                globalEvent.event.start = dt.date + "~" + dt.time + "~" + dt.UTC + "~"
                SDTWindow.destroy()
                SDTAdded()
    
        SDTWindow = Toplevel()

        w = 340  # width for the Tk root
        h = 300  # height for the Tk root

        # get screen width and height
        ws = SDTWindow.winfo_screenwidth()  # width of the screen
        hs = SDTWindow.winfo_screenheight()  # height of the screen

        # calculate x and y coordinates for the Tk root window
        x = (ws / 2) - (w / 2)
        y = (hs / 2) - (h / 2)

        # set the dimensions of the screen
        # and where it is placed
        SDTWindow.geometry('%dx%d+%d+%d' % (w, h, x, y))

        SDTWindow.title("CreateDateTime UI")
        SDTWindow.minsize(340, 300)
        SDTWindow.maxsize(340, 300)
        SDTWindow.config(background='cyan')

        ## LABELS AND ENTRIES
        title = Label(SDTWindow, text="Start Date/Time", bg='cyan')
        title.config(font=("Courier", 22, "bold"))
        title.grid(sticky=N, columnspan=4, pady=20, padx=20)

        Label(SDTWindow, bg='cyan', width=2).grid(rowspan=7)
        Label(SDTWindow, text="Please enter the date/time information below.\n\n", bg='cyan').grid(sticky=N, row=1, columnspan=3)
        Label(SDTWindow, text="Date (YYYYMMDD)", bg='cyan').grid(sticky=N, row=3, column=1, pady=3)
        Label(SDTWindow, text="Time (hhmmss)", bg='cyan').grid(sticky=N, row=4, column=1, pady=3)
        Label(SDTWindow, text="UTC Time?", bg='cyan').grid(sticky=N, row=5, column=1, pady=3)

        var = IntVar()

        e1 = Entry(SDTWindow)
        e2 = Entry(SDTWindow)
        c = Checkbutton(SDTWindow, bg='cyan', variable=var)

        e1.grid(row=3, column=2)
        e2.grid(row=4, column=2)
        c.grid(sticky=N, row=5, column=2, pady=3)

        # Add Button
        button = Button(SDTWindow, bg='black', fg='white', text='ADD', relief=RAISED, command=addSDT)
        button.config(font=1)
        button.grid(row=6, columnspan=6, padx=50, pady=30)

        ## Set the focus on dialog window (needed on Windows)
        SDTWindow.focus_set()
        ## Make sure events only go to our dialog
        SDTWindow.grab_set()
        ## Make sure dialog stays on top of its parent window (if needed)
        SDTWindow.transient(mainWindow)
        SDTWindow.transient(createEventWindow)

        # Close Window Event
        SDTWindow.bind("<Escape>", lambda _: closeWindow())

        ## Display the window and wait for it to close
        SDTWindow.wait_window(SDTWindow)

    # Property UI
    '''
    def createPropertyEvent():

        def addProp():
            global globalEvent

            clearErrorLog()
            prop = PropertyClass()

            prop.propName = e1.get()
            prop.propDescr = e2.get()
            if(len(prop.propName) < 1):
                insertLog("Invalid property name, please try again\n")
            if(len(prop.propDescr) < 1):
                insertLog("Invalid property description, please try again\n")
            else:
                globalEvent.event.propertyE = prop.propName + "|&&&|" + prop.propDescr + "|&&&|"
                print("Prop Name " + prop.propName)
                print("Prop Descr " + prop.propDescr)
                propertyWindow.destroy()
    
        propertyWindow = Toplevel()

        w = 340  # width for the Tk root
        h = 300  # height for the Tk root

        # get screen width and height
        ws = propertyWindow.winfo_screenwidth()  # width of the screen
        hs = propertyWindow.winfo_screenheight()  # height of the screen

        # calculate x and y coordinates for the Tk root window
        x = (ws / 2) - (w / 2)
        y = (hs / 2) - (h / 2)

        # set the dimensions of the screen
        # and where it is placed
        propertyWindow.geometry('%dx%d+%d+%d' % (w, h, x, y))

        propertyWindow.title("Property UI")
        propertyWindow.minsize(340, 300)
        propertyWindow.maxsize(340, 300)
        propertyWindow.config(background='cyan')

        ## LABELS AND ENTRIES
        title = Label(propertyWindow, text="Create Property", bg='cyan')
        title.config(font=("Courier", 22, "bold"))
        title.grid(column=1, sticky=W, columnspan=4, pady=20)

        Label(propertyWindow, bg='cyan', width=2).grid(rowspan=7)
        Label(propertyWindow, text="Please enter the property information below.\n\n", bg='cyan').grid(sticky=W, row=1, column=1, columnspan=2)
        Label(propertyWindow, text="Property Name", bg='cyan').grid(sticky=W, row=3, column=1, pady=3)
        Label(propertyWindow, text="Property Description", bg='cyan').grid(sticky=W, row=4, column=1, pady=3)

        e1 = Entry(propertyWindow)
        e2 = Entry(propertyWindow)

        e1.grid(row=3, column=2)
        e2.grid(row=4, column=2)

        # Add Button
        button = Button(propertyWindow, bg='black', fg='white', text='ADD', relief=RAISED, command=addProp)
        button.config(font=1)
        button.grid(column=1, row=6, columnspan=6, padx=50, pady=30)

        ## Set the focus on dialog window (needed on Windows)
        propertyWindow.focus_set()
        ## Make sure events only go to our dialog
        propertyWindow.grab_set()
        ## Make sure dialog stays on top of its parent window (if needed)
        propertyWindow.transient(mainWindow)
        propertyWindow.transient(createEventWindow)
        ## Display the window and wait for it to close
        propertyWindow.wait_window(propertyWindow)
    '''
    
    def createAlarmWindow():  

        def closeWindow():
            createAlarmWindow.destroy()

        def addAlarm():
            global globalEvent

            clearErrorLog()
            alarm = AlarmClass()

            alarm.action = "AUDIO"
            alarm.trigger = e2.get()
            if(len(alarm.action) < 1):
                insertLog("Invalid action, please try again\n")
            elif(len(alarm.trigger) < 1):
                insertLog("Invalid trigger, please try again\n")
            else:
                globalEvent.event.alarm += alarm.action + "~" + alarm.trigger + "~"# + alarm.propertyA + "~"
                globalEvent.event.alarm += "|"

                createAlarmWindow.destroy()
    
        createAlarmWindow = Toplevel()

        '''
        # Property UI
        def createPropertyAlarm():
        
            def addProp():
                global globalAlarm

                clearErrorLog()
                prop = PropertyClass()

                prop.propName = e1.get()
                prop.propDescr = e2.get()
                if(len(prop.propName) < 1):
                    insertLog("Invalid property name, please try again\n")
                if(len(prop.propDescr) < 1):
                    insertLog("Invalid property description, please try again\n")
                else:
                    globalAlarm.alarm.propertyA += prop.propName + "|&&&|" + prop.propDescr + "|&&&|"
                    print("Prop Name " + prop.propName)
                    print("Prop Descr " + prop.propDescr)
                    propertyWindow.destroy()
        
            propertyWindow = Toplevel()

            w = 350  # width for the Tk root
            h = 300  # height for the Tk root

            # get screen width and height
            ws = propertyWindow.winfo_screenwidth()  # width of the screen
            hs = propertyWindow.winfo_screenheight()  # height of the screen

            # calculate x and y coordinates for the Tk root window
            x = (ws / 2) - (w / 2)
            y = (hs / 2) - (h / 2)

            # set the dimensions of the screen
            # and where it is placed
            propertyWindow.geometry('%dx%d+%d+%d' % (w, h, x, y))

            propertyWindow.title("Property UI")
            propertyWindow.minsize(350, 300)
            propertyWindow.maxsize(350, 300)
            propertyWindow.config(background='cyan')

            ## LABELS AND ENTRIES
            title = Label(propertyWindow, text="Create Property", bg='cyan')
            title.config(font=("Courier", 22, "bold"))
            title.grid(column=1, sticky=W, columnspan=4, pady=20)

            Label(propertyWindow, bg='cyan', width=2).grid(rowspan=7)
            Label(propertyWindow, text="Please enter the property information below.\n\n", bg='cyan').grid(sticky=W,
                                                                                                           row=1,
                                                                                                           column=1,
                                                                                                           columnspan=2)
            Label(propertyWindow, text="Property Name", bg='cyan').grid(sticky=W, row=3, column=1, pady=3)
            Label(propertyWindow, text="Property Description", bg='cyan').grid(sticky=W, row=4, column=1, pady=3)

            e1 = Entry(propertyWindow)
            e2 = Entry(propertyWindow)

            e1.grid(row=3, column=2)
            e2.grid(row=4, column=2)

            # Add Button
            button = Button(propertyWindow, bg='black', fg='white', text='ADD', relief=RAISED, command=addProp)
            button.config(font=1)
            button.grid(column=1, row=6, columnspan=6, padx=50, pady=30)

            ## Set the focus on dialog window (needed on Windows)
            propertyWindow.focus_set()
            ## Make sure events only go to our dialog
            propertyWindow.grab_set()
            ## Make sure dialog stays on top of its parent window (if needed)
            propertyWindow.transient(mainWindow)
            propertyWindow.transient(createEventWindow)
            ## Display the window and wait for it to close
            propertyWindow.wait_window(propertyWindow)
        '''

        w = 350  # width for the Tk root
        h = 300  # height for the Tk root

        # get screen width and height
        ws = createAlarmWindow.winfo_screenwidth()  # width of the screen
        hs = createAlarmWindow.winfo_screenheight()  # height of the screen

        # calculate x and y coordinates for the Tk root window
        x = (ws / 2) - (w / 2)
        y = (hs / 2) - (h / 2)

        # set the dimensions of the screen
        # and where it is placed
        createAlarmWindow.geometry('%dx%d+%d+%d' % (w, h, x, y))

        createAlarmWindow.title("Create Alarm UI")
        createAlarmWindow.minsize(350, 300)
        createAlarmWindow.maxsize(350, 300)
        
        createAlarmWindow.config(background='cyan')

        ## LABELS AND ENTRIES
        title = Label(createAlarmWindow, text="New Alarm", bg='cyan')
        title.config(font=("Courier", 22, "bold"))
        title.grid(sticky=E, column=1, columnspan=4, padx=40, pady=20)

        Label(createAlarmWindow, bg='cyan', width=2).grid(rowspan=7)
        Label(createAlarmWindow, text="Please enter the alarm information below.\n\n", bg='cyan').grid(sticky=W, row=1, column=1, columnspan=2)
        Label(createAlarmWindow, text="Action set to 'AUDIO'", bg='cyan').grid(sticky=W, row=2, column=1, columnspan=2, pady=3)
        Label(createAlarmWindow, text="Trigger", bg='cyan').grid(sticky=W, row=3, column=1, pady=3)
        #Label(createAlarmWindow, text="Add Alarm Property?", bg='cyan').grid(sticky=W, row=4, column=1, pady=3)

        #e1 = Entry(createAlarmWindow)
        e2 = Entry(createAlarmWindow)
        #b3 = Button(createAlarmWindow, bg='blue', fg='white', text='ADD', relief=RAISED, command=createPropertyAlarm)

        #b3.config()

        #e1.grid(sticky=E, row=2, column=2)
        e2.grid(sticky=E, row=3, column=2)
        #b3.grid(row=4, column=2)

        # Add Button
        button = Button(createAlarmWindow, bg='black', fg='white', text='ADD', relief=RAISED, command=addAlarm)
        button.config(font=1)
        button.grid(sticky=N, column=2, columnspan=5, padx=80, pady=20)

        ## Set the focus on dialog window (needed on Windows)
        createAlarmWindow.focus_set()
        ## Make sure events only go to our dialog
        createAlarmWindow.grab_set()
        ## Make sure dialog stays on top of its parent window (if needed)
        createAlarmWindow.transient(mainWindow)
        createAlarmWindow.transient(createEventWindow)

        # Close Window Event
        createAlarmWindow.bind("<Escape>", lambda _: closeWindow())

        ## Display the window and wait for it to close
        createAlarmWindow.wait_window(createAlarmWindow)

    w = 360  # width for the Tk root
    h = 350  # height for the Tk root

    # get screen width and height
    ws = createEventWindow.winfo_screenwidth()  # width of the screen
    hs = createEventWindow.winfo_screenheight()  # height of the screen

    # calculate x and y coordinates for the Tk root window
    x = (ws / 2) - (w / 2)
    y = (hs / 2) - (h / 2)

    # set the dimensions of the screen
    # and where it is placed
    createEventWindow.geometry('%dx%d+%d+%d' % (w, h, x, y))

    createEventWindow.title("Create Event UI")
    createEventWindow.minsize(360, 350)
    createEventWindow.maxsize(360, 350)
    createEventWindow.config(background='cyan')

    ## LABELS AND ENTRIES
    title = Label(createEventWindow, text="New Event", bg='cyan')
    title.config(font=("Courier", 22, "bold"))
    title.grid(columnspan=4, padx=70, pady=20)

    Label(createEventWindow, bg='cyan', width=2).grid(rowspan=7)
    Label(createEventWindow, text="Please enter the event information below.\n\n", bg='cyan').grid(sticky=W, row=1, column=1, columnspan=2)
    Label(createEventWindow, text="UID", bg='cyan').grid(sticky=W, row=2, column=1, pady=3)
    Label(createEventWindow, text="Event Creation Date/Time", bg='cyan').grid(sticky=W, row=3, column=1, pady=3)
    Label(createEventWindow, text="Event Start Date/Time", bg='cyan').grid(sticky=W, row=4, column=1, pady=3)
    Label(createEventWindow, text="Add Event Alarm?", bg='cyan').grid(sticky=W, row=5, column=1, pady=3)
    #Label(createEventWindow, text="Add Event Property?", bg='cyan').grid(sticky=W, row=6, column=1, pady=3)

    e1 = Entry(createEventWindow)
    b2 = Button(createEventWindow, bg='blue', fg='white', text='ADD', relief=RAISED, command=CDT)
    b3 = Button(createEventWindow, bg='blue', fg='white', text='ADD', relief=RAISED, command=SDT)
    b4 = Button(createEventWindow, bg='blue', fg='white', text='ADD', relief=RAISED, command=createAlarmWindow)
    #b5 = Button(createEventWindow, bg='blue', fg='white', text='ADD', relief=RAISED, command=createPropertyEvent)

    b2.config()
    b3.config()
    b4.config()
    #b5.config()

    e1.grid(row=2, column=2)
    b2.grid(row=3, column=2)
    b3.grid(row=4, column=2)
    b4.grid(row=5, column=2)
    #b5.grid(row=6, column=2)

    # Add Button
    button = Button(createEventWindow, bg='black', fg='white', text='ADD', relief=RAISED, command=addEvent)
    button.config(font=1)
    button.grid(columnspan=4, padx=40, pady=20)

    ## Set the focus on dialog window (needed on Windows)
    createEventWindow.focus_set()
    ## Make sure events only go to our dialog
    createEventWindow.grab_set()
    ## Make sure dialog stays on top of its parent window (if needed)
    createEventWindow.transient(mainWindow)

    # Close Window Event
    createEventWindow.bind("<Escape>", lambda _: closeWindow())

    ## Display the window and wait for it to close
    createEventWindow.wait_window(createEventWindow)

def createAlarmWindow():

    def closeWindow():
        createAlarmWindow.destroy()

    def addAlarm():
        global globalEvent, globalAlarm

        clearErrorLog()
        globalAlarm.alarm = AlarmClass()

        action = "AUDIO"
        trigger = e2.get()
        if(len(action) < 1):
            insertLog("Invalid action, please try again\n")
        elif(len(trigger) < 1):
            insertLog("Invalid trigger, please try again\n")
        else:
            globalAlarm.alarm.action = action
            globalAlarm.alarm.trigger = e2.get()
            addAlarmToCal(globalAlarm.alarm)
            createAlarmWindow.destroy()

    createAlarmWindow = Toplevel()
    '''
    # Property UI
    def createPropertyAlarm():
    
        def addProp():
            global globalAlarm

            clearErrorLog()
            prop = PropertyClass()

            prop.propName = e1.get()
            prop.propDescr = e2.get()
            if(len(prop.propName) < 1):
                insertLog("Invalid property name, please try again\n")
            if(len(prop.propDescr) < 1):
                insertLog("Invalid property description, please try again\n")
            else:
                globalAlarm.alarm.propertyA += prop.propName + "|&&&|" + prop.propDescr + "|&&&|"
                print("Prop Name " + prop.propName)
                print("Prop Descr " + prop.propDescr)
                propertyWindow.destroy()
    
        propertyWindow = Toplevel()

        w = 340  # width for the Tk root
        h = 300  # height for the Tk root

        # get screen width and height
        ws = propertyWindow.winfo_screenwidth()  # width of the screen
        hs = propertyWindow.winfo_screenheight()  # height of the screen

        # calculate x and y coordinates for the Tk root window
        x = (ws / 2) - (w / 2)
        y = (hs / 2) - (h / 2)

        # set the dimensions of the screen
        # and where it is placed
        propertyWindow.geometry('%dx%d+%d+%d' % (w, h, x, y))

        propertyWindow.title("Property UI")
        propertyWindow.minsize(340, 300)
        propertyWindow.maxsize(340, 300)
        propertyWindow.config(background='cyan')

        ## LABELS AND ENTRIES
        title = Label(propertyWindow, text="Create Property", bg='cyan')
        title.config(font=("Courier", 22, "bold"))
        title.grid(column=1, sticky=W, columnspan=4, pady=20)

        Label(propertyWindow, bg='cyan', width=2).grid(rowspan=7)
        Label(propertyWindow, text="Please enter the property information below.\n\n", bg='cyan').grid(sticky=W,
                                                                                                       row=1,
                                                                                                       column=1,
                                                                                                       columnspan=2)
        Label(propertyWindow, text="Property Name", bg='cyan').grid(sticky=W, row=3, column=1, pady=3)
        Label(propertyWindow, text="Property Description", bg='cyan').grid(sticky=W, row=4, column=1, pady=3)

        e1 = Entry(propertyWindow)
        e2 = Entry(propertyWindow)

        e1.grid(row=3, column=2)
        e2.grid(row=4, column=2)

        # Add Button
        button = Button(propertyWindow, bg='black', fg='white', text='ADD', relief=RAISED, command=addProp)
        button.config(font=1)
        button.grid(column=1, row=6, columnspan=6, padx=50, pady=30)

        ## Set the focus on dialog window (needed on Windows)
        propertyWindow.focus_set()
        ## Make sure events only go to our dialog
        propertyWindow.grab_set()
        ## Make sure dialog stays on top of its parent window (if needed)
        propertyWindow.transient(mainWindow)
        ## Display the window and wait for it to close
        propertyWindow.wait_window(propertyWindow)
    '''

    w = 350  # width for the Tk root
    h = 300  # height for the Tk root

    # get screen width and height
    ws = createAlarmWindow.winfo_screenwidth()  # width of the screen
    hs = createAlarmWindow.winfo_screenheight()  # height of the screen

    # calculate x and y coordinates for the Tk root window
    x = (ws / 2) - (w / 2)
    y = (hs / 2) - (h / 2)

    # set the dimensions of the screen
    # and where it is placed
    createAlarmWindow.geometry('%dx%d+%d+%d' % (w, h, x, y))

    createAlarmWindow.title("Create Alarm UI")
    createAlarmWindow.minsize(350, 300)

    createAlarmWindow.maxsize(350, 300)
    createAlarmWindow.config(background='cyan')

    ## LABELS AND ENTRIES
    title = Label(createAlarmWindow, text="New Alarm", bg='cyan')
    title.config(font=("Courier", 22, "bold"))
    title.grid(sticky=E, column=1, columnspan=4, padx=40, pady=20)

    Label(createAlarmWindow, bg='cyan', width=2).grid(rowspan=7)
    Label(createAlarmWindow, text="Please enter the alarm information below.\n\n", bg='cyan').grid(sticky=W, row=1, column=1, columnspan=2)
    Label(createAlarmWindow, text="Action set to 'AUDIO'", bg='cyan').grid(sticky=W, row=2, column=1, columnspan=2, pady=3)
    Label(createAlarmWindow, text="Trigger", bg='cyan').grid(sticky=W, row=3, column=1, pady=3)
    #Label(createAlarmWindow, text="Add Alarm Property?", bg='cyan').grid(sticky=W, row=4, column=1, pady=3)

    #e1 = Entry(createAlarmWindow)
    e2 = Entry(createAlarmWindow)
    #b3 = Button(createAlarmWindow, bg='blue', fg='white', text='ADD', relief=RAISED, command=createPropertyAlarm)

    #b3.config()

    #e1.grid(sticky=E, row=2, column=2)
    e2.grid(sticky=E, row=3, column=2)
    #b3.grid(row=4, column=2)

    # Add Button
    button = Button(createAlarmWindow, bg='black', fg='white', text='ADD', relief=RAISED, command=addAlarm)
    button.config(font=1)
    button.grid(sticky=N, column=2, columnspan=5, padx=80, pady=20)

    ## Set the focus on dialog window (needed on Windows)
    createAlarmWindow.focus_set()
    ## Make sure events only go to our dialog
    createAlarmWindow.grab_set()
    ## Make sure dialog stays on top of its parent window (if needed)
    createAlarmWindow.transient(mainWindow)

    # Close Window Event
    createAlarmWindow.bind("<Escape>", lambda _: closeWindow())

    ## Display the window and wait for it to close
    createAlarmWindow.wait_window(createAlarmWindow)

# Delete Current Cal? UI
def CurCal():
    def closeWindow():
        window.destroy()

    def okCurCal():

        if(saveVar.get() == 1):
            fileSaveAs()
            closeWindow()

        else:
            closeWindow()

    window = Toplevel()

    w = 300  # width for the Tk root
    h = 200  # height for the Tk root

    # get screen width and height
    ws = window.winfo_screenwidth()  # width of the screen
    hs = window.winfo_screenheight()  # height of the screen

    # calculate x and y coordinates for the Tk root window
    x = (ws / 2) - (w / 2)
    y = (hs / 2) - (h / 2)

    # set the dimensions of the screen
    # and where it is placed
    window.geometry('%dx%d+%d+%d' % (w, h, x, y))

    window.minsize(300, 200)

    window.maxsize(300, 200)
    window.config(background='cyan')

    ## LABELS AND ENTRIES
    title = Label(window, text="Calendar Already Exists", bg='cyan')
    title.config(font=("Courier", 11, "bold"))
    title.grid(column=1, sticky=W, columnspan=4, padx=30, pady=40)

    Label(window, text="Save Current Calendar?", bg='cyan').grid(sticky=N, row=1, column=1, pady=10)

    saveVar = IntVar()

    c = Checkbutton(window, bg='cyan', variable=saveVar)

    c.grid(sticky=E, row=1, column=2, pady=3)

    # Add Button
    button = Button(window, bg='black', fg='white', text='OK', relief=RAISED, command=okCurCal)
    button.config(font=1)
    button.grid(row=2, columnspan=6, padx=50, pady=10)
    
    ## Set the focus on dialog window (needed on Windows)
    window.focus_set()
    ## Make sure events only go to our dialog
    window.grab_set()
    ## Make sure dialog stays on top of its parent window (if needed)
    window.transient(mainWindow)

    # Close Window Event
    window.bind("<Escape>", lambda _: closeWindow())

    ## Display the window and wait for it to close
    window.wait_window(window)

# ABOUT
def aboutICal():
    def closeWindow():
        aboutWindow.destroy()

    aboutWindow = Toplevel()

    w = 300  # width for the Tk root
    h = 200  # height for the Tk root

    # get screen width and height
    ws = aboutWindow.winfo_screenwidth()  # width of the screen
    hs = aboutWindow.winfo_screenheight()  # height of the screen

    # calculate x and y coordinates for the Tk root window
    x = (ws / 2) - (w / 2)
    y = (hs / 2) - (h / 2)

    # set the dimensions of the screen
    # and where it is placed
    aboutWindow.geometry('%dx%d+%d+%d' % (w, h, x, y))

    aboutWindow.title("About")
    aboutWindow.minsize(300, 200)

    aboutWindow.maxsize(300, 200)
    aboutWindow.config(background='cyan')

    ## LABELS AND ENTRIES
    title = Label(aboutWindow, text="About", bg='cyan')
    title.config(font=("Courier", 22, "bold"))
    title.grid(column=1, sticky=W, columnspan=4, padx=85, pady=20)

    Label(aboutWindow, bg='cyan', width=2).grid(rowspan=7)
    Label(aboutWindow, text="Application Name:", bg='cyan').grid(sticky=E, row=1, column=1)
    Label(aboutWindow, text="Creator:", bg='cyan').grid(sticky=E, row=2, column=1)
    Label(aboutWindow, text="ICalendar Parser", bg='cyan').grid(sticky=W, row=1, column=3)
    Label(aboutWindow, text="Jon MacPherson", bg='cyan').grid(sticky=W, row=2, column=3)
    compatible = Label(aboutWindow, text="\nCompatible with iCalendar V2.0", bg='cyan')
    compatible.config(font=("Times New Roman", 9, "bold", "italic"))
    compatible.grid(sticky=N, row=3, column=1, columnspan=3, padx=40)

    ## Set the focus on dialog window (needed on Windows)
    aboutWindow.focus_set()
    ## Make sure events only go to our dialog
    aboutWindow.grab_set()
    ## Make sure dialog stays on top of its parent window (if needed)
    aboutWindow.transient(mainWindow)

    # Close Window Event
    aboutWindow.bind("<Escape>", lambda _: closeWindow())

    ## Display the window and wait for it to close
    aboutWindow.wait_window(aboutWindow)

# Delete Current Cal? UI
def confirmClose():

    def closeWindow():
        window.destroy()

    def okConfirm():

        if(closeVar.get() == 1):
            closeWindow()
            quit()

        else:
            closeWindow()

    window = Toplevel()

    w = 300  # width for the Tk root
    h = 200  # height for the Tk root

    # get screen width and height
    ws = window.winfo_screenwidth()  # width of the screen
    hs = window.winfo_screenheight()  # height of the screen

    # calculate x and y coordinates for the Tk root window
    x = (ws / 2) - (w / 2)
    y = (hs / 2) - (h / 2)

    # set the dimensions of the screen
    # and where it is placed
    window.geometry('%dx%d+%d+%d' % (w, h, x, y))

    window.minsize(300, 200)

    window.maxsize(300, 200)
    window.config(background='cyan')

    ## LABELS AND ENTRIES
    title = Label(window, text="The program is about to close", bg='cyan')
    title.config(font=("Courier", 11, "bold"))
    title.grid(column=1, sticky=W, columnspan=4, padx=20, pady=40)

    Label(window, text="Are you sure you would like to exit?", bg='cyan').grid(sticky=N, row=1, column=1, pady=10, padx=10)

    closeVar = IntVar()

    c = Checkbutton(window, bg='cyan', variable=closeVar)

    c.grid(sticky=W, row=1, column=2, pady=3)

    # Add Button
    button = Button(window, bg='black', fg='white', text='OK', relief=RAISED, command=okConfirm)
    button.config(font=1)
    button.grid(row=2, column=1, columnspan=6, padx=70, pady=10)
    
    ## Set the focus on dialog window (needed on Windows)
    window.focus_set()
    ## Make sure events only go to our dialog
    window.grab_set()
    ## Make sure dialog stays on top of its parent window (if needed)
    window.transient(mainWindow)

    # Close Window Event
    window.bind("<Escape>", lambda _: closeWindow())

    ## Display the window and wait for it to close
    window.wait_window(window)

#############################################################           MAIN WINDOW            ###################################################################


#
# Main GUI attributes
#
mainWindow.title("ICalParser UI")
mainWindow.minsize(900, 650)

##########
# FRAMES #
##########

# Top Border Frame
topSideBar = Frame(mainWindow, bg="cyan", height=50)
topSideBar.pack(anchor=N, fill=X, expand=False, side=TOP)

Title = Label(topSideBar, text="ICalendar Parser", fg='black', bg='cyan')
Title.config(font=("courier", 33, "bold"))
Title.pack(anchor=N, fill=Y, expand=True, side=TOP)

# Bottom Border Frame
topSideBar = Frame(mainWindow, bg="cyan", height=20)
topSideBar.pack(anchor=S, fill=X, expand=False, side=BOTTOM)

# Left Border Frame
rightSideBar = Frame(mainWindow, bg="cyan", width=20)
rightSideBar.pack(anchor=E, fill=Y, expand=False, side=RIGHT)

# Left Border Frame
leftSideBar = Frame(mainWindow, bg="cyan", width=50)
leftSideBar.pack(anchor=W, fill=Y, expand=False, side=LEFT)

# Right NavBar Frame
rightNavBar = Frame(mainWindow, bg="cyan", width=150)
rightNavBar.pack(anchor=E, fill=Y, expand=False, side=RIGHT)

file_Display_NavBar = Frame(rightNavBar, bg="#b3ffff", width=150, bd=3, relief=SUNKEN)
file_Display_NavBar.pack(anchor=N, fill=Y, expand=True, side=TOP, pady=2)

file_Display_NavBar.grid_propagate(0)

display_Divider = Frame(rightNavBar, bg="cyan", width=150, height=20)
display_Divider.pack(anchor=N, expand=False, side=TOP)

log_Display_NavBar = Frame(rightNavBar, bg="#b3ffff", height=170, width=150, bd=3, relief=SUNKEN)
log_Display_NavBar.pack(anchor=S, expand=False, side=BOTTOM, padx=10)

log_Display_NavBar.grid_propagate(0)

#######################
# File Navbar WIDGETS #
#######################

title = Label(file_Display_NavBar, text="File Display", bg='#b3ffff')
title.config(font=('courier', 14, "bold"))
title.grid(sticky=W, padx=3, pady=20)

line = Label(file_Display_NavBar, text="--------------------------------------------", bg='#b3ffff')
line.config(font=('courier', 4, "bold"))
line.grid(sticky=W, padx=3)

showAlarmsButton = Button(file_Display_NavBar, width=10, bg='blue', fg='white', text='Show Alarms', relief=RAISED, state=DISABLED, command=printAlarmToLog)
showPropsButton = Button(file_Display_NavBar, width=10, bg='blue', fg='white', text='Extract Optional\nProperties', relief=RAISED, state=DISABLED, command=printPropToLog)
delEventButton = Button(file_Display_NavBar, width=10, bg='blue', fg='white', text='Delete Event', relief=RAISED, state=DISABLED, command=deleteEventUI)

showAlarmsButton.config()
showPropsButton.config()
delEventButton.config()

showAlarmsButton.grid(sticky=N, pady=25)
showPropsButton.grid(sticky=N)
delEventButton.grid(sticky=N, pady=25)

######################
# Log Navbar WIDGETS #
######################

title = Label(log_Display_NavBar, text="Log Display", bg='#b3ffff')
title.config(font=('courier', 14, "bold"))
title.grid(sticky=N, padx=3, pady=20)

line = Label(log_Display_NavBar, text="--------------------------------------------", bg='#b3ffff')
line.config(font=('courier', 4, "bold"))
line.grid(sticky=W, padx=3)

clearLogButton = Button(log_Display_NavBar, width=10, bg='blue', fg='white', text='Clear Log', relief=RAISED, command=clearErrorLog)

clearLogButton.config()

clearLogButton.grid(sticky=N, padx=3, pady=25)

# Content Frame, containing both ListBox Displays

content_frame = Frame(mainWindow, bg="black", bd=1, relief=SUNKEN)
content_frame.pack(anchor=N, fill=BOTH, expand=True, side=LEFT)

file_Display = Frame(content_frame, bg="blue", bd=3, relief=RIDGE)
file_Display.pack(anchor=N, fill=BOTH, expand=True, side=TOP)

log_Display = Frame(content_frame, bg="blue", height=180, bd=3, relief=RIDGE)
log_Display.pack(anchor=S, fill=X, expand=False, side=BOTTOM)
log_Display.pack_propagate(0)

#############
# File Tree #
#############

def selectItem(a):
    global selectedEventNo
    curItem = fileTV.focus()
    fileTV.item(curItem)
    x = fileTV.item(curItem)
    try:
        selectedEventNo = int(x['text'])
        insertLog("Event No " + str(int(x['text'])) + " selected\n")
    except ValueError:
        selectedEventNo = 1

fileTV =  ttk.Treeview(file_Display)
fileTV['columns'] = ('props', 'alarms', 'summary')
fileTV.heading("#0", text='Event No', anchor='w')
fileTV.column("#0", anchor="w", width=50, stretch=1)
fileTV.heading('props', text='# of Props')
fileTV.column('props', anchor='center', width=50, stretch=1)
fileTV.heading('alarms', text='# of Alarms')
fileTV.column('alarms', anchor='center', width=50, stretch=1)
fileTV.heading('summary', text='Summary')
fileTV.column('summary', anchor='center', width=300, stretch=1)
fileTV.pack(anchor=N+W, fill=BOTH, expand=True, side=LEFT)

fileTV.bind('<Button-1>', selectItem)

############
# Text Box #
############

def insertLog(str):
    global logTB
    logTB.config(state=NORMAL)
    logTB.insert(END, str)
    logTB.config(state=DISABLED)

logTB = Text(log_Display, relief=SUNKEN)
logTB.pack(anchor=N+W, fill=BOTH, expand=True, side=LEFT)
logTB.config(state=DISABLED)

##############
# Scroll Bar #
##############

# File TV

fileTV_scrollbar = Scrollbar(file_Display)
fileTV_scrollbar.pack(side=RIGHT, fill=Y)
fileTV.config(yscrollcommand=fileTV_scrollbar.set)
fileTV_scrollbar.config(command=fileTV.yview)

# horizontal scroll
'''
fileTV_scrollbarX = Scrollbar(fileTV)
fileTV_scrollbarX.pack(side=BOTTOM, fill=X)
fileTV.config(xscrollcommand=fileTV_scrollbarX.set)
fileTV_scrollbarX.config(command=fileTV.xview, orient=HORIZONTAL)
'''
# Log TB
logTB_scrollbar = Scrollbar(log_Display)
logTB_scrollbar.pack(side=RIGHT, fill=Y)
logTB.config(yscrollcommand=logTB_scrollbar.set)
logTB_scrollbar.config(command=logTB.yview)

############
# Menu Bar #
############

menu = Menu(mainWindow)
mainWindow.config(menu=menu)
subMenu1 = Menu(menu, tearoff=0)
subMenu2 = Menu(menu, tearoff=0)
subMenu3 = Menu(menu, tearoff=0)
subMenu4 = Menu(menu, tearoff=0)

# File Menu Option
menu.add_cascade(label="File", menu=subMenu1)

subMenu1.add_command(label="Open   (Ctrl-O)", command=fileOpen)

subMenu1.add_command(label="Save   (Ctrl-S)", state=DISABLED, command=fileSave)

subMenu1.add_command(label="Save as...", state=DISABLED, command=fileSaveAs)

subMenu1.add_command(label="Exit   (Ctrl-X)", command=quit)

# Create Menu Option
menu.add_cascade(label="Create", menu=subMenu2)

subMenu2.add_command(label="Create calendar", command=createCalendarWindow)
subMenu2.add_command(label="Create event", command=createEventWindow, state=DISABLED)
subMenu2.add_command(label="Create alarm", command=createAlarmWindow, state=DISABLED)

# Database Menu Option
menu.add_cascade(label="Database", menu=subMenu3)
subMenu3.add_command(label="Store All Events", state=DISABLED, command=addAllEvents)
subMenu3.add_command(label="Store Current Event", state=DISABLED, command=addCurEvent)
subMenu3.add_command(label="Clear All Data", command=delData)
subMenu3.add_command(label="Display DB Status", command=displayDB)
subMenu3.add_command(label="Execute Query", command=createQuery)

# Help Menu Option
menu.add_cascade(label="Help", menu=subMenu4)
subMenu4.add_command(label="About iCalGUI...", command=aboutICal)


'''
    KEY EVENTS
'''

# File Load Key Event
mainWindow.bind("<Control-O>", fileOpen)
mainWindow.bind("<Control-o>", fileOpen)

# File Save Key Event
mainWindow.bind("<Control-S>", fileSave)
mainWindow.bind("<Control-s>", fileSave)

# Close Window Event
mainWindow.bind("<Control-X>", lambda _: confirmClose())
mainWindow.bind("<Control-x>", lambda _: confirmClose())
mainWindow.protocol('WM_DELETE_WINDOW', confirmClose)

# start the program
mainWindow.mainloop()
