from ctypes import *

#Create a Calentar type that matches our type in CalendarParser.h

class Calendar(Structure):
    _fields_ = [
        ("version", c_float),
        ("prodID", c_byte * 1000),
        ("events", c_void_p),
        ("properties", c_void_p)]

############################
# FUNCTIONS FOR FILE PANEL #
############################

def eventStats(index, myClass):

    #save the path
    calLibPath = 'bin/libcparse.so'

    #load the module
    callib = CDLL(calLibPath)

    eventStatsIndex = callib.eventStatsAtIndex

    eventStatsIndex.argtypes = [c_int, POINTER(Calendar)]
    eventStatsIndex.restype = c_char_p

    ESstr = eventStatsIndex(index, myClass.calPtr)

    return ESstr.decode("utf-8")

def numEvents(calPtr):
    #save the path
    calLibPath = 'bin/libcparse.so'

    #load the module
    callib = CDLL(calLibPath)

    getEventsLen = callib.numberOfEvents

    getEventsLen.argtypes = [POINTER(Calendar)]
    getEventsLen.restype = c_int

    num = getEventsLen(calPtr)

    return num

###########################
# FUNCTIONS FOR LOG PANEL #
###########################

def printEventAlarms(eventNo, calPtr):

    #save the path
    calLibPath = 'bin/libcparse.so'

    #load the module
    callib = CDLL(calLibPath)

    printA = callib.printEA

    printA.argtypes = [c_int, POINTER(Calendar)]
    printA.restype = c_char_p  

    AStr = printA(eventNo, calPtr)

    return AStr.decode("utf-8")

def printEventProps(eventNo, calPtr):

    #save the path
    calLibPath = 'bin/libcparse.so'

    #load the module
    callib = CDLL(calLibPath)

    printP = callib.printEP

    printP.argtypes = [c_int, POINTER(Calendar)]
    printP.restype = c_char_p  

    PStr = printP(eventNo, calPtr)

    return PStr.decode("utf-8")

######################
# CALENDAR FUNCTIONS #
######################

def createCalendar(fileName, myClass):

    #save the path
    calLibPath = 'bin/libcparse.so'

    #load the module
    callib = CDLL(calLibPath)

    #create a sample file argument
    fStr = fileName.encode('utf-8')

    #create an alias for callib.createCalendar() 
    createCal = callib.createCalendar

    # Help the Python compiler figure out the types for the function  
    #********** these can be commented out - Python can infer them **********
    createCal.argtypes = [c_char_p, POINTER(POINTER(Calendar))]
    createCal.restype = c_int

    # create a variable that will store the pointer to the calendar
    # if we don't do this, Python will be unhappy

    errorCode = createCal(fStr, byref(myClass.calPtr))

    return errorCode


def deleteC(myClass):

    #save the path
    calLibPath = 'bin/libcparse.so'

    #load the module
    callib = CDLL(calLibPath)

    #create an alias for callib.createCalendar() 
    deleteCal = callib.deleteCalendar

    deleteCal.argtypes = [POINTER(Calendar)]

    deleteCal(myClass.calPtr)

#**********************************************************************

def printCalendar(myClass):

    #save the path
    calLibPath = 'bin/libcparse.so'

    #load the module
    callib = CDLL(calLibPath)

    #create an alias for callib.printCalendar() 
    printCal = callib.printCalendar

    # Help the Python compiler figure out the types for the function  
    printCal.argtypes = [POINTER(Calendar)] #this can also be commented out
    printCal.restype = c_char_p             #this CANNOT be commented out! Otherwise Python will decide that printCal returns an int!

    #call printCal
    calStr = printCal(myClass.calPtr)

    return calStr.decode("utf-8")

def printError(errorCode):

    #save the path
    calLibPath = 'bin/libcparse.so'

    #load the module
    callib = CDLL(calLibPath)

    #create an alias for callib.printCalendar() 
    printErr = callib.printError

    # Help the Python compiler figure out the types for the function  
    printErr.argtypes = [c_int] #this can also be commented out
    printErr.restype = c_char_p             #this CANNOT be commented out! Otherwise Python will decide that printCal returns an int!

    errorStr = printErr(errorCode)

    return errorStr.decode("utf-8")

#**********************************************************************

def saveToFile(fileName, myClass):

    #save the path
    calLibPath = 'bin/libcparse.so'

    #load the module
    callib = CDLL(calLibPath)

    #create an alias for callib.printCalendar() 
    writeCal = callib.writeCalendar

    # Help the Python compiler figure out the types for the function  
    writeCal.argtypes = [c_char_p, POINTER(Calendar)] #this can also be commented out
    writeCal.restype = c_int            #this CANNOT be commented out! Otherwise Python will decide that printCal returns an int!

    errorCode = writeCal(fileName.encode('utf-8'), myClass.calPtr)

    return errorCode

#####################################################################################

##############################
# FILL UI CALENDAR FUNCTIONS #
##############################

def createUICalendar(prodID, myClass):

    #save the path
    calLibPath = 'bin/libcparse.so'

    #load the module
    callib = CDLL(calLibPath)

    #create an alias for callib.printCalendar() 
    fillCalendar = callib.fillCal

    # Help the Python compiler figure out the types for the function  
    fillCalendar.argtypes = [c_char_p, POINTER(POINTER(Calendar))] #this can also be commented out
    fillCalendar.restype = None             #this CANNOT be commented out! Otherwise Python will decide that printCal returns an int!

    fillCalendar(prodID.encode('utf-8'), byref(myClass.calPtr))

def createUIEvent(event, myClass):

    #save the path
    calLibPath = 'bin/libcparse.so'

    #load the module
    callib = CDLL(calLibPath)

    #create an alias for callib.printCalendar() 
    fillEvent = callib.fillCalEvent

    # Help the Python compiler figure out the types for the function  
    fillEvent.argtypes = [c_char_p, POINTER(POINTER(Calendar))] #this can also be commented out
    fillEvent.restype = None             #this CANNOT be commented out! Otherwise Python will decide that printCal returns an int!

    string = event.UID + "^" + event.create + "^" + event.start + "^" + event.alarm + "^"# + event.propertyE + "^"

    fillEvent(string.encode('utf-8'), byref(myClass.calPtr))

def createUIAlarm(index, myClass, alarm):

    #save the path
    calLibPath = 'bin/libcparse.so'

    #load the module
    callib = CDLL(calLibPath)

    #create an alias for callib.printCalendar() 
    fillAlarm = callib.addAlarmToEventAtIndex

    # Help the Python compiler figure out the types for the function  
    fillAlarm.argtypes = [c_int, POINTER(POINTER(Calendar)), c_char_p, c_char_p] #this can also be commented out
    fillAlarm.restype = None             #this CANNOT be commented out! Otherwise Python will decide that printCal returns an int!

    fillAlarm(index, byref(myClass.calPtr), alarm.action.encode('utf-8'), alarm.trigger.encode('utf-8'))

def deleteEvent(index, myClass):
    #save the path
    calLibPath = 'bin/libcparse.so'

    #load the module
    callib = CDLL(calLibPath)

    #create an alias for callib.printCalendar() 
    delEvent = callib.eventDelAtIndex

    # Help the Python compiler figure out the types for the function  
    delEvent.argtypes = [c_int, POINTER(POINTER(Calendar))] #this can also be commented out
    delEvent.restype = None             #this CANNOT be commented out! Otherwise Python will decide that printCal returns an int!

    delEvent(index, byref(myClass.calPtr))


################################# SQL Functions ###############################

def getSOL(eventNo, myClass):

    #save the path
    calLibPath = 'bin/libcparse.so'

    #load the module
    callib = CDLL(calLibPath)

    printEvDb = callib.getSOLSdtNa

    printEvDb.argtypes = [c_int, POINTER(Calendar)]
    printEvDb.restype = c_char_p  

    EDbStr = printEvDb(eventNo, myClass.calPtr)

    return EDbStr.decode("utf-8")