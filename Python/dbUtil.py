
import sqlite3
import sys
import traceback

mConn	= None 
mDbFile = None
mCursor = None


def openConnection(pathToDbFile):

	global mConn
	global mDbFile
	global mCursor 

	retval = False

	mDbFile = pathToDbFile

	if mConn is None:
		try:
			mConn = sqlite3.connect(mDbFile)
			mCursor = mConn.cursor()
			retval = True
			#print("Connected to database! "+mDbFile)
		except:
			print("Could not connect to database at "+pathToDbFile)
			retval = False
	else:
		print("DB connection already open")

	return retval


def closeConnection():

		global mConn
		global mCursor
		global mDbFile

		if mConn is not None:
			mConn.commit()
			mConn.close()
		else:
			print("Connection already closed")

		mConn = None
		mCursor = None
		mDbFile = None

class Table():
	
	mTableName = None
	mColumnInfo = None
	mAddRowSql = None
	mUpdateRowSql = None



	#tableName = String 
	#columnInfo = list of tuples (columnName, columnType)
	def __init__(self, tableName, columnInfo):

		self.mTableName = ""
		self.mColumnInfo = ()
		self.mAddRowSql = ""
		self.mUpdateRowSql = ""

		if mConn is not None and mCursor is not None:

			self.mTableName = tableName
			self.mColumnInfo = columnInfo

			if len(columnInfo) < 1:
				print("Table must have at least 1 column.")
				return

			# Creating a new SQLite table with column 1 as its PRIMARY KEY
			# PRIMARY KEY column must consist of unique values
			createTableSql = "CREATE TABLE IF NOT EXISTS "+self.mTableName+"("
			i=0
			for column in columnInfo:
				if i == 0:
					createTableSql += column[0] + " "+column[1]+" PRIMARY KEY,"
				elif i<len(columnInfo)-1:
					createTableSql += column[0] + " "+column[1]+","
				else:
					createTableSql += column[0] + " "+column[1]
				i += 1
			createTableSql += ");"

			try:
				mCursor.execute(createTableSql)
				#print("Table "+self.mTableName+" Created! SQL: "+createTableSql)
				sys.stdout.flush()
			except:
				print("Could not create table. SQL: "+createTableSql)
				traceback.print_exc()
				sys.stdout.flush()
				return

			self.mAddRowSql = "INSERT INTO "+tableName+"("
			i=0
			for column in self.mColumnInfo:
				if i==0:
					self.mAddRowSql += ""+column[0]
				else:
					self.mAddRowSql += ","+column[0]
				i += 1
			self.mAddRowSql += ") VALUES("
			i=0
			for column in self.mColumnInfo:
				if i == len(self.mColumnInfo)-1:
					self.mAddRowSql +="?"
				else:
					self.mAddRowSql += "?,"
				i += 1
			self.mAddRowSql += ")"

			self.mUpdateRowSql = "UPDATE "+tableName+" SET "
			i=0
			for column in self.mColumnInfo:
				if i==0:
					i += 1
					continue
				elif i == len(self.mColumnInfo)-1:
					self.mUpdateRowSql += column[0] + "=? "
				else:
					self.mUpdateRowSql += column[0] + "=?, "
				i += 1
			self.mUpdateRowSql += "WHERE "+self.mColumnInfo[0][0]+" = ?"

			mConn.commit()

			
			#print("Add Row SQL: "+self.mAddRowSql)
			#print("Update Row SQL: "+self.mUpdateRowSql)
			sys.stdout.flush()
			

		else:
			print("Could not create table. DB not connected.")



	def addColumn(self, columnName, columnType):

		global mConn
		global mCursor
		global mColumnInfo

		if mConn is not None and mCursor is not None:
			# Adding a new column without a row value
			mCursor.execute("ALTER TABLE {tn} ADD COLUMN '{cn}' {ct}"\
		        .format(tn=self.mTableName, cn=columnName, ct=columnType))

			mConn.commit()
			#print("Adding Column "+columnName+" to "+self.mTableName)
		else:
			print("Could not add column. DB not connected.")


	def getValue(self, columnName, rowId):

		global mCursor

		sql = "SELECT "+str(columnName)+" FROM "+self.mTableName+" WHERE "+str(self.mColumnInfo[0][0])+'="'+str(rowId)+'"'
		try:
			mCursor.execute(sql)
		except:
			print("DB ERROR: Could not get Row "+str(rowId)+" Column"+str(columnName)+" SQL:"+sql)
			traceback.print_exc()
			sys.stdout.flush()
			return None

		val = mCursor.fetchone()
		return val


	def getRow(self, rowId):

		global mCursor

		sql = "SELECT * FROM "+self.mTableName+" WHERE "+str(self.mColumnInfo[0][0])+'="'+str(rowId)+'"'
		try:
			mCursor.execute(sql)
		except:
			print("DB ERROR: Could not get row "+str(rowId)+" SQL:"+sql)
			traceback.print_exc()
			sys.stdout.flush()

		row = mCursor.fetchone()
		if row is not None:
			#print("Got Row! on table "+self.mTableName+" "+str(row))
			return row
		else:
			print("DB ERROR: Row "+str(rowId)+" does not exist. SQL:"+sql)
			return None

	def addRow(self, entry):

		global mCursor
		global mConn
		

		if len(entry) == len(self.mColumnInfo):
			try:
				mCursor.execute(self.mAddRowSql, entry)
				mConn.commit()
				#print("Added Row! "+str(entry))
				sys.stdout.flush()
			except:
				print("DB ERROR: Could not add row "+str(entry[0])+" SQL: "+str(self.mAddRowSql)+" Row: "+str(entry))
				traceback.print_exc()
				sys.stdout.flush()
		else:
			print("Could not add row. Entry is wrong size. #Columns = "+str(len(self.mColumnInfo))+" #Entry Colums = "+str(len(entry)))
			print("COLS: "+str(self.mColumnInfo))
			print("ENTRY: "+str(entry))
			sys.stdout.flush()

	def popTuple(self, tuplex, index):
		listx = list(tuplex)
		retVal = listx.pop(index)
		retTuple = tuple(listx)
		return [retTuple, retVal]


	def updateRow(self, entry):
		global mCursor
		global mConn

		tup = self.popTuple(entry, 0)
		rowId = tup[1]
		entry = tup[0]
		entry += (rowId,);
		if len(entry) == len(self.mColumnInfo):
			try:
				mCursor.execute(self.mUpdateRowSql, entry)
				mConn.commit()
				#print("Updated Row! on table "+self.mTableName+" "+str(entry))
				sys.stdout.flush()
			except:
				print("DB ERROR: Could not update row")
				print("UPDATE SQL: "+self.mUpdateRowSql)
				print("ENTRY: "+str(entry))
				traceback.print_exc()
				sys.stdout.flush()
		else:
			print("Could not update row. Entry is wrong size. #Columns = "+str(len(self.mColumnInfo))+" #Entry Colums = "+str(len(entry)))
			print("COLS: "+str(self.mColumnInfo))
			print("ENTRY: "+str(entry))
			sys.stdout.flush()

	
	def updateValue(self, columnName, rowId, value):
		global mCursor
		global mConn

		sql = "UPDATE "+self.mTableName+" SET "+columnName+"="+str(value)+" WHERE "+str(self.mColumnInfo[0][0])+'="'+str(rowId)+'"'
		try:
			mCursor.execute(sql)
			mConn.commit()
			#print("Updated Value! on table "+self.mTableName+" column "+str(columnName)+" row "+str(rowId)+" value "+str(value))
			#sys.stdout.flush()
		except:
			print("DB ERROR: Could not update value on row "+str(rowId)+" column "+str(columnName))
			print("SQL: "+sql)
			traceback.print_exc()
			sys.stdout.flush()


	def deleteRow(self, rowId):
		
		global mCursor
		
		sql = "DELETE FROM "+self.mTableName+" WHERE "+str(self.mColumnInfo[0][0])+"="+str(rowId)
		try:
			mCursor.execute(sql)
		except:
			print("DB ERROR: Could not delete row "+str(rowId))
			traceback.print_exc()
			sys.stdout.flush()


	def clear(self):
		
		global mCursor

		sql = "DELETE FROM "+self.mTableName
		print("Clearing Table "+self.mTableName+" SQL: "+sql)
		mCursor.execute(sql)


	