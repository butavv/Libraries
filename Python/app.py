
#####################################################################################
#   Haven Innovation HM1000 PT Table Controller:
#
#   BeagleBone UART Bridge
#
#   Description: Handles packets from the Embedded controller and updates database's
#                Reads data from database's and send packets back to embedded controller
#
#   Written for Python 2.7, Angstrom Linux on BeagleBone Back 
#
#
#   2017 Phase 1 Engineering, LLC
#####################################################################################



from __future__ import print_function
from flask import Flask, render_template, Response
import sys
from flask import request
import dbUtil as db
import json
import logging
from flask_socketio import SocketIO
from flask_socketio import send, emit

import eventlet
#patch for eventlet
eventlet.monkey_patch()
import os
import subprocess
import serialUtil as ser
import time
import Adafruit_BBIO.UART as UART
from threading import Thread

DEBUG = False



#Packet Type Definitions
PACKET_STANDARD             = 0
PACKET_CONFIG               = 1
PACKET_CALIBRATE            = 2
PACKET_TABLEINFO            = 3

#Table State Definitions
TABLESTATE_IDLE             = 0
TABLESTATE_RUN_MANUAL       = 1
TABLESTATE_RUN_AUTO         = 2
TABLESTATE_TOLEANCE_TESTING = 3
TABLESTATE_CALIBRATING      = 4
TABLESTATE_PAUSE_MANUAL     = 5
TABLESTATE_PAUSE_AUTO       = 6
TABLESTATE_FLEXION_AUTO     = 7
TABLESTATE_STOP_MANUAL      = 8
TABLESTATE_STOP_AUTO        = 9

#System Globals
mActualTableState           = TABLESTATE_IDLE
mActualDwellTime            = 0
mActualForce                = 0
mActualFlexionAngle         = 0
mActualSecondsRemaining     = 0
mActualForceApplied         = 0
mActualLateralFlexion       = 0
mActualLatFlexAngle         = 0
mActualTension              = 0
mActualYTravel              = 0
mActualDistance             = 0
mActualColumnHeight         = 0
mActualSpeedSetting         = 0
mActualMaxDistance          = 0
mActualPretension           = 0
mActualTableHeight          = 0
mActualStatusCode           = 0
mActualTimeSetting          = 0

mTargetTableState           = TABLESTATE_IDLE
mTargetDwellTime            = 0
mTargetForce                = 0
mTargetFlexionAngle         = 0
mTargetSecondsRemaining     = 0
mTargetSpeedSetting         = 0
mTargetMaxDistance          = 0
mTargetPretension           = 0
mTargetTableHeight          = 0
mTargetTimeSetting          = 0

mControlDataTable           = None
mRecordDataTable            = None

engineInitialized           = False
#Database Structure Definitions.
#Note:  These must agree with definitions in "app.py"

CONTROL_DB_UPDATE_MS        = 100
RECORD_DB_UPDATE_SEC        = 1

PATH_TO_DB_FILE             = "hm1000db.sqlite"
DB_CONTROL_TABLE_NAME       = "HM1000Control"
DB_RECORD_TABLE_NAME        = "HM1000Record"

DB_CONTROL_COLUMNS = [
        ("Control_Type",        "TEXT"),
        ("Table_State",         "INTEGER"),
        ("Dwell_Time",          "INTEGER"),
        ("Force",               "INTEGER"),
        ("Flexion_Angle",       "INTEGER"),
        ("Seconds_Remaining",   "INTEGER"),
        ("Distance",            "INTEGER"),
        ("Force_Applied",       "INTEGER"),
        ("Lat_Flex_Angle",      "INTEGER"),
        ("Tension",             "INTEGER"),
        ("Y_Travel",            "INTEGER"),
        ("Column_Height",       "INTEGER"),
        ("Speed_Setting",       "INTEGER"),
        ("Max_Distance",        "INTEGER"),
        ("Pretension",          "INTEGER"),
        ("Table_Height",        "INTEGER"),
        ("Time_Setting",        "INTEGER"),
        ("Status_Code",         "INTEGER")
]

DB_RECORD_COLUMNS = [
        ("Time",                    "INTEGER"),
        ("Actual_Table_State",      "INTEGER"),
        ("Actual_Dwell_Time",       "INTEGER"),
        ("Actual_Force",            "INTEGER"),
        ("Actual_Flexion_Angle",    "INTEGER")
]


#Call to initialize
def initializeEngine():
    global engineInitialized
    print("Initializing Engine")
    initControlDb();
    initUART()
    print("Engine Initialized")
    engineInitialized = True
    return True


def pullEmbedded():
    while ser.isChatting():
        for packet in ser.serialReadPackets():
            handlePacket(packet)

        #MUST SLEEP HERE TO FREE UP PROCESSOR    
        time.sleep(0.01)

    print("PULL EMBEDDED EXITED")
    sys.stdout.flush()
    app.logger.debug("Pull Embedded Exited")

#Init UART communication with angstrum linux on BeagleBone Black
def initUART():
    print("Initializing UART")

    subprocess.call(["sudo", "config-pin", "P9.24", "uart"])
    subprocess.call(["sudo", "config-pin", "P9.26", "uart"])

    ser.serialOpenConnection("/dev/ttyO1", 115200, 0)
    Thread(target=pullEmbedded, args=()).start()
    print("UART Initialized")
    


#Initialize the control database
def initControlDb():

    global mControlDataTable
    global mTargetTableState
    global mTargetDwellTime
    global mTargetForce
    global mTargetFlexionAngle
    global mTargetTableHeight
    global mTargetTableHeight
    global mTargetSpeedSetting
    global mTargetPretension
    global mTargetTimeSetting
    global mActualTableState
    global mActualDwellTime
    global mActualForce
    global mActualFlexionAngle
    global mActualForceApplied
    global mActualTableHeight
    global mActualSpeedSetting
    global mActualPretension
    global mActualLateralFlexion
    global mActualLatFlexAngle
    global mActualTension
    global mActualYTravel
    global mActualColumnHeight
    global mActualStatusCode
    global mActualDistance
    global mActualSecondsRemaining
    global mActualTimeSetting

    print("Initializing DB")
    db.openConnection(PATH_TO_DB_FILE)
    print("Connection Opened")
    mControlDataTable = db.Table(DB_CONTROL_TABLE_NAME, DB_CONTROL_COLUMNS)
    print("Control Table Found")
    mControlDataTable.clear()
    print("Control table cleared")

    #Add first row to DB
    dbEntry = ("Target", 
        mTargetTableState, 
        mTargetDwellTime, 
        mTargetForce,
        mTargetFlexionAngle,
        mTargetSecondsRemaining,
        0,                          #Distance
        0,                          #Force applied
        0,                          #Lat Flex Angle
        0,                          #Tension
        0,                          #Y Travel
        0,                          #Column Height
        mTargetSpeedSetting,
        mTargetMaxDistance,
        mTargetPretension,
        mTargetTableHeight,
        mTargetTimeSetting,
        0)                          #StatusCode

    mControlDataTable.addRow(dbEntry)
    print("Added target entry")

    time.sleep(1)

    dbEntry = ("Actual",
        mActualTableState,
        mActualDwellTime,
        mActualForce,
        mActualFlexionAngle,
        mActualSecondsRemaining,
        mActualDistance,
        mActualForceApplied,
        mActualLatFlexAngle,
        mActualTension,
        mActualYTravel,
        mActualColumnHeight,
        mActualSpeedSetting,
        mActualMaxDistance,
        mActualPretension,
        mActualTableHeight,
        mActualTimeSetting,
        mActualStatusCode                     
        )

    print("Prepairing to add Actual Entry "+str(dbEntry))
    mControlDataTable.addRow(dbEntry)
    print("Added Actual Entry")


    print("DB Initialized")


    


def getControlColumns():
    return DB_CONTROL_COLUMNS



def intToByteArray(i):
    i = i % 4294967296
    n4 = i % 256
    i = i / 256
    n3 = i % 256
    i = i / 256
    n2 = i % 256
    n1 = i / 256
    return [int(n1),int(n2),int(n3),int(n4)]


def byteArrayToInt(byteArray, index, bigEndian):
    result = 0
    if bigEndian == True:
        for i in range(index, index+4):
            result = result * 256 + int(byteArray[i])
    else:
        for i in range(index+3, index-1, -1):
            result = result * 256 + int(byteArray[i])
    return result

def byteArrayToShort(byteArray, index, bigEndian):
    result = 0
    #app.logger.debug("Byte Arr to Short Called. Index = "+str(index))
    if bigEndian == True:
         for i in range(index, index+2):
            result = result * 256 + int(byteArray[i])
    else:
        for i in range(index+1, index-1, -1):
            result = result * 256 + int(byteArray[i])
        #app.logger.debug("Result = "+str(result)+" i="+str(i))
    return result

def shortToByteArray(i):
    uint32 = intToByteArray(i)
    return [uint32[2], uint32[3]]


#Parses a full packet and sets the appropriate variables
def handlePacket(packetIn):

    global mActualTableState
    global mActualDwellTime
    global mActualForce
    global mActualFlexionAngle
    global mActualForceApplied
    global mActualTableHeight
    global mActualSpeedSetting
    global mActualPretension
    global mActualLateralFlexion
    global mActualLatFlexAngle
    global mActualTension
    global mActualYTravel
    global mActualColumnHeight
    global mActualStatusCode
    global mActualSecondsRemaining
    global mActualMaxDistance
    global mActualTimeSetting

    app.logger.debug("Received embedded packet "+str(packetIn))

    #parse packet type
    if packetIn[0] == PACKET_STANDARD:
       
        mActualTableState           = packetIn[1]
        mActualDwellTime            = packetIn[2]
        mActualForce                = packetIn[3]
        mActualFlexionAngle         = packetIn[4]
        #app.logger.debug("Prepairing to call byte array to short")
        mActualSecondsRemaining     = byteArrayToShort(packetIn, 5, False)
        mActualDistance             = packetIn[7]
        mActualForceApplied         = packetIn[8]
        mActualLatFlexAngle         = packetIn[9]
        mActualTension              = packetIn[10]
        mActualYTravel              = packetIn[11]
        mActualColumnHeight         = packetIn[12]
        mActualSpeedSetting         = packetIn[13]
        mActualMaxDistance          = packetIn[14]
        mActualPretension           = packetIn[15]
        mActualTableHeight          = packetIn[16]
        mActualTimeSetting          = byteArrayToShort(packetIn, 17, False)
        mActualStatusCode           = packetIn[19]

        dataToUi = ("Actual",
                mActualTableState,
                mActualDwellTime,
                mActualForce,
                mActualFlexionAngle,
                mActualSecondsRemaining,
                mActualDistance,
                mActualForceApplied,
                mActualLatFlexAngle,
                mActualTension,
                mActualYTravel,
                mActualColumnHeight,
                mActualSpeedSetting,
                mActualMaxDistance,
                mActualPretension,
                mActualTableHeight,
                mActualTimeSetting,
                mActualStatusCode
                )

        #app.logger.debug("TEST "+dbEntry[2])
        try:
            app.logger.debug("Pushing to UI")
            socketPush(dataToUi)
            app.logger.debug("UI push success")
        except:
            app.logger.debug("Could not push socket")





def handleUiData(data):
    
    global engineInitialized
    global mTargetTableState
    global mTargetDwellTime
    global mTargetForce
    global mTargetFlexionAngle
    global mTargetSecondsRemaining
    global mTargetSpeedSetting
    global mTargetMaxDistance
    global mTargetPretension
    global mTargetTableHeight
    global mTargetTimeSetting
    global mControlDataTable


    app.logger.debug("Received UI Data"+str(data))

    '''if mControlDataTable is None:
        print("Control Data Table nonexistent")
        app.logger.debug("Control table not ready")
        return'''


    tableName = data['Table_Name']
    rowId = data['Row_ID']
    values = data['Values']
    
    dataSet = []
    for item in values:
        columnName = item[0]
        value =  item[1]
        if columnName == "Table_State":
            mTargetTableState = value
        if columnName == "Dwell_Time":
            mTargetDwellTime = value
        if columnName == "Force":
            mTargetForce = value
        if columnName == "Flexion_Angle":
            mTargetFlexionAngle = value
        if columnName == "Speed_Setting":
            mTargetSpeedSetting = value
        if columnName == "Max_Distance":
            mTargetMaxDistance = value
        if columnName == "Pretension":
            mTargetPretension = value
        if columnName == "Table_Height":
            mTargetTableHeight = value
        if columnName == "Time_Setting":
            mTargetTimeSetting = value
        if columnName == "Seconds_Remaining":
            mTargetSecondsRemaining = value

        data = [PACKET_STANDARD,
            mTargetTableState, 
            mTargetDwellTime, 
            mTargetForce, 
            mTargetFlexionAngle,
            ]

        data.extend(shortToByteArray(mTargetSecondsRemaining))

        data.extend([
            mTargetSpeedSetting,
            mTargetMaxDistance,
            mTargetPretension,
            mTargetTableHeight
        ])

        data.extend(shortToByteArray(mTargetTimeSetting))
        

    try:
        app.logger.debug("Sending Packet to Embedded "+str(data))  
        ser.serialWritePacket(data)
        app.logger.debug("Serial Write Complete!")
    except:
        app.logger.debug("Serial Write Failed")

    



#Updates control database with "actual" data harvested from the embedded processor
def dbPush(): 

    global mTargetTableState
    global mTargetDwellTime
    global mTargetForce
    global mTargetFlexionAngle
    global mTargetSecondsRemaining
    global mTargetSpeedSetting
    global mTargetMaxDistance
    global mTargetPretension
    global mTargetTableHeight
    global mTargetTimeSetting
    
    global mActualTableState
    global mActualDwellTime
    global mActualForce
    global mActualFlexionAngle
    global mActualForceApplied
    global mActualTableHeight
    global mActualSpeedSetting
    global mActualPretension
    global mActualLateralFlexion
    global mActualLatFlexAngle
    global mActualTension
    global mActualYTravel
    global mActualColumnHeight
    global mActualStatusCode
    global mActualDistance
    global mActualSecondsRemaining
    global mActualTimeSetting

    global mControlDataTable

    app.logger.debug("Updating DB")

    dbEntry = ("Actual",
                mActualTableState,
                mActualDwellTime,
                mActualForce,
                mActualFlexionAngle,
                mActualSecondsRemaining,
                mActualDistance,
                mActualForceApplied,
                mActualLatFlexAngle,
                mActualTension,
                mActualYTravel,
                mActualColumnHeight,
                mActualSpeedSetting,
                mActualMaxDistance,
                mActualPretension,
                mActualTableHeight,
                mActualTimeSetting,
                mActualStatusCode
                )

    mControlDataTable.updateRow(dbEntry)

    app.logger.debug("Actual Values Updated")

    for item in values:
        app.logger.debug("Updating value in database "+str(item)+" on table "+str(tableName))
        columnName = item[0]
        value =  item[1]
        if tableName == DB_CONTROL_TABLE_NAME:
            mControlDataTable.updateValue(columnName, rowId, value)
    
    #time.sleep(0.02)

    app.logger.debug("Database Updated")

#Reads the "target" data from the control database
def dbPull():

    global mTargetTableState
    global mTargetDwellTime
    global mTargetForce
    global mTargetFlexionAngle
    global mTargetSecondsRemaining
    global mTargetSpeedSetting
    global mTargetMaxDistance
    global mTargetPretension
    global mTargetTableHeight
    global mTargetTimeSetting
    
    global mActualTableState
    global mActualDwellTime
    global mActualForce
    global mActualFlexionAngle
    global mActualForceApplied
    global mActualTableHeight
    global mActualSpeedSetting
    global mActualPretension
    global mActualLateralFlexion
    global mActualLatFlexAngle
    global mActualTension
    global mActualYTravel
    global mActualColumnHeight
    global mActualStatusCode
    global mActualDistance
    global mActualSecondsRemaining
    global mActualTimeSetting

    global mControlDataTable

    

    if mControlDataTable is None:
        app.logger.debug("Tried to pull when control data table not initialized")
        return

    dbRow = mControlDataTable.getRow("Target")

    if dbRow == None:
        app.logger.debug("ERROR: Could not get UI Data")
        sys.stdout.flush()
        return

    mTargetTableState       = dbRow[1]
    mTargetDwellTime        = dbRow[2]
    mTargetForce            = dbRow[3]
    mTargetFlexionAngle     = dbRow[4]
    mTargetSecondsRemaining = dbRow[5]
    mTargetSpeedSetting     = dbRow[12]
    mTargetMaxDistance      = dbRow[13]
    mTargetPretension       = dbRow[14]
    mTargetTableHeight      = dbRow[15]
    mTargetTimeSetting      = dbRow[16]

    app.logger.debug("Pull Success! "+str(dbRow))

    '''try:
        pushEmbeddedData()
    except:
        app.logger.debug("Could not push Embedded data")'''


#Sends data packet to embedded processor
def pushEmbeddedData():

    global mTargetTableState
    global mTargetDwellTime
    global mTargetForce
    global mTargetFlexionAngle
    global mTargetSecondsRemaining
    global mTargetSpeedSetting
    global mTargetMaxDistance
    global mTargetPretension
    global mTargetTableHeight
    global mTargetTimeSetting
    global packetOut

    #print("Sending Packet")

    




#Prints Variables
def printData():
    global mActualTableState
    global mTargetSecondsRemaining
    global mActualDwellTime
    global mActualForce
    global mActualFlexionAngle
    global mTargetTableState
    global mTargetDwellTime
    global mTargetFlexionAngle
    global mTargetForce
    global mActualSecondsRemaining
    global mRecordDataTable

    print("\n")
    print("TARGET: SecondsRemaining:"+str(mTargetSecondsRemaining)+" TableState:"+str(mTargetTableState)+" DwellTime:"+str(mTargetDwellTime)+" Force:"+str(mTargetForce)+" FlexionAngle:"+str(mTargetFlexionAngle))
    print("ACTUAL: SecondsRemaining:"+str(mActualSecondsRemaining)+" TableState:"+str(mActualTableState)+" DwellTime:"+str(mActualDwellTime)+" Force:"+str(mActualForce)+" FlexionAngle:"+str(mActualFlexionAngle))
    print("\n")
    sys.stdout.flush()


#Adds a row to the record database containing the current info at the specific timeslot
def pushRecordData():

    global mActualTableState
    global mTargetSecondsRemaining
    global mActualDwellTime
    global mActualForce
    global mActualFlexionAngle
    global mTargetTableState
    global mTargetDwellTime
    global mTargetFlexionAngle
    global mTargetForce
    global mActualSecondsRemaining
    global mRecordDataTable


#Gets the current time in seconds
def getTimeSeconds():
    t = time.time()
    return int(t)

#Gets the current time in milliseconds
def getTimeMillis():
    millis = int(round(time.time() * 1000))
    return millis



#Initialize all
initializeEngine()
#eventlet.monkey_patch()
app = Flask(__name__)
socketio = SocketIO(app)
gunicorn_error_logger = logging.getLogger('gunicorn.error')
app.logger.handlers.extend(gunicorn_error_logger.handlers)
app.logger.setLevel(logging.DEBUG)




@app.route('/')
def index():
    
    return  render_template("/local/html/index.html")


@app.route('/rest', methods=['POST', 'GET', 'PUT'])
def restRx():

    #app.logger.debug("REST "+request.method+" Data = "+str(request.json))

    global mTargetDataTable
    
    #print("Received REST request with method "+str(request.method)+" and data "+str(request.json) )
    sys.stdout.flush()

    if request.method == 'POST':
        data = request.json
        
        mControlDataTable.updateRow(dbEntry)
        sys.stdout.flush()
        responseData = '{"success" : true}'
        #app.logger.debug("REST POST: Response = "+str(responseData))
        status = '200 OK'
        responseHeaders = [
            ('Content-Type', 'application/json'),
            ('Content-Length', str(len(responseData)))
        ]

        return responseData, status, responseHeaders

    elif request.method == 'PUT':
        data = request.json
        tableName = data['Table_Name']
        rowId = data['Row_ID']
        values = data['Values']
        for item in values:
            columnName = item[0]
            value =  item[1]
            #app.logger.debug("Table "+str(tableName)+" Row "+str(rowId)+" Column "+str(columnName)+" Value "+str(value))
            if tableName == DB_CONTROL_TABLE_NAME:
                mControlDataTable.updateValue(columnName, rowId, value)

        responseData = '{"success" : true}'
        #app.logger.debug("REST PUT: Response = "+str(responseData))
        status = '200 OK'
        responseHeaders = [
            ('Content-Type', 'application/json'),
            ('Content-Length', str(len(responseData)))
        ]
        #start_response(status, response_headers)
        return responseData, status, responseHeaders 
        
    elif request.method == 'GET':
        rowData = mControlDataTable.getRow("Actual")
        #app.logger.debug("REST GET: Response = "+str(rowData))
        i = 0
        responseData = "{"
        for column in DB_CONTROL_COLUMNS:
            if column[1] == "INTEGER":
                responseData += '"'+column[0]+'" : '+str(rowData[i])
            elif column[1] == "TEXT":
                responseData += '"'+column[0]+'" : "'+str(rowData[i])+'"'
            #app.logger.debug(responseData)
            if i < len(DB_CONTROL_COLUMNS)-1:
                responseData += ", "
            i += 1
        responseData += "}"
        status = '200 OK'
        responseHeaders = [
            ('Content-Type', 'application/json'),
            ('Content-Length', str(len(responseData)))
        ]
        return json.dumps(responseData), status, responseHeaders

    elif request.method == 'DELETE':
        print("DELETE")
        return json.dumps({'success':True}), 200, {'Content-Type':'application/json'}


@socketio.on('json')
def handle_json(jsonObj):
    data = json.loads(jsonObj)
    #app.logger.debug("Received JSON "+str(data))
    handleUiData(data)
    #app.logger.debug("Socket RX Complete")



@socketio.on('connectEvent')
def handleConnection(data):
    app.logger.debug("Socket Connected"+str(data))




def socketPush(rowData):
    
    i = 0
    pushData = "{"
    for column in DB_CONTROL_COLUMNS:
        if column[1] == "INTEGER":
            pushData += '"'+column[0]+'" : '+str(rowData[i])
        elif column[1] == "TEXT":
            pushData += '"'+column[0]+'" : "'+str(rowData[i])+'"'
        #app.logger.debug(responseData)
        if i < len(DB_CONTROL_COLUMNS)-1:
            pushData += ", "
        i += 1
    pushData += "}"
    app.logger.debug("Pushing Data "+str(pushData))
    socketio.emit('serverEvent', pushData)
    app.logger.debug("Socket Push Complete")

    



if __name__ == '__main__':
    print("Starting Server", file=sys.stderr)
    socketio.run(app)