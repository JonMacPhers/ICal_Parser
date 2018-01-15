import mysql.connector
import sys

#### Open database connection ####

connectionFlag = 0

if(len(sys.argv) < 2 ):
    dbName = "jmacph04"
    uName = "jmacph04"
    passwd = "0878619"

    try:
        conn = mysql.connector.connect(host="dursley.socs.uoguelph.ca",database=dbName,user=uName, password=passwd)
        print("Connected Successfully")
    except mysql.connector.Error as err:
        print("Something went wrong: {}".format(err))
        exit()
else:
    dbName = sys.argv[1]
    if(dbName):
        password = input("input password: ")
        uName = dbName
        passwd = password
        try:
            conn = mysql.connector.connect(host="dursley.socs.uoguelph.ca",database=dbName,user=uName, password=passwd)
            print("Connected Successfully")
            connectionFlag = 1
        except mysql.connector.Error as err:
            connectionFlag = 0
            print("\nInvalid User Information, please try again\n")
            username = input("input username: ")
            password = input("input password: ")
            dbName = username
            uName = username
            passwd = password
            try:
                conn = mysql.connector.connect(host="dursley.socs.uoguelph.ca",database=dbName,user=uName, password=passwd)
                print("Connected Successfully")
                connectionFlag = 1
            except mysql.connector.Error as err:
                connectionFlag = 0
                print("\nInvalid User Information, please try again\n")
                username = input("input username: ")
                password = input("input password: ")
                dbName = username
                uName = username
                passwd = password
                try:
                    conn = mysql.connector.connect(host="dursley.socs.uoguelph.ca",database=dbName,user=uName, password=passwd)
                    print("Connected Successfully")
                    connectionFlag = 1
                except mysql.connector.Error as err:
                    print("Double check your username and password and run the program again\ngoodbye for now :)")
                    exit()
                    connectionFlag = 0

# prepare a cursor object using cursor() method
cursor = conn.cursor(buffered=True, dictionary=True)

########################################################
#####                Table ORGANIZER               #####
########################################################
createQuery="CREATE TABLE IF NOT EXISTS ORGANIZER (org_id int auto_increment,  name varchar(60) NOT NULL,  contact varchar(60) NOT NULL, primary key(org_id) )"
try:
    cursor.execute(createQuery)
    print("Created ORGANIZER Successfully\n")
    conn.commit()
except mysql.connector.Error as err:
    print("Something went wrong: {}".format(err))


####################### PRINT #######################
def printO():

    print("\nDatabase - Organizer Table contents sorted by org_id:\n")

    try:
        cursor.execute("SELECT * FROM ORGANIZER ORDER BY org_id")
        conn.commit()
    except mysql.connector.Error as err:
        print("Something went wrong: {}".format(err))
      
    for r in cursor:
        # r is a tuple of column values
        # this means that r has constant length
        print(r)
        
    print("\n\n")


#####################################################
#####                Table EVENT                #####
#####################################################
createQuery="CREATE TABLE IF NOT EXISTS EVENT (event_id int not null auto_increment,  summary varchar(60) not null,  start_time datetime not null, location varchar(60), organizer int, num_alarms int, primary key(event_id), foreign key(organizer) references ORGANIZER(org_id) )"
try:
    cursor.execute(createQuery)
    print("Created EVENT Successfully\n")
    conn.commit()
except mysql.connector.Error as err:
    print("Something went wrong: {}".format(err))


############### PRINT ################
def printE():
    print("Database - Event Table contents sorted by event_id:\n")

    try:
        cursor.execute("SELECT * FROM EVENT ORDER BY event_id")
        conn.commit()
    except mysql.connector.Error as err:
        print("Something went wrong: {}".format(err))
      
    for r in cursor:
        # r is a tuple of column values
        # this means that r has constant length
        print(r)
        
    print("\n\n")


''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''