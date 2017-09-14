/*
 * db.c
 *
 *  Created on: 10 wrz 2017
 *      Author: Pawe³
 */


#include "db.h"
#include "string.h"

#include "drv/EEPROM/EEPROM.h"

#include "drv/LCD/HD44780.h"
#include "avr/pgmspace.h"

#define _MEMORY_FIELD_NULL 0xFF
/******************
 * Buffer data cell:
 * [
 *   ID: 1, max ID 254, 255 means ID_NULL
 *   SIZE: 1, max size of single entry=254
 *   DATA: SIZE
 * ]
 */

#define DB_BUFFER_SIZE 500

#define CURRENT_VERSION 1

struct dbEntry {
	enum DBDataField id;
	uint16_t address;
	uint8_t size;
};

struct db {
//	uint8_t buffer[DB_BUFFER_SIZE];
	struct dbEntry info[DB_FIELDS_COUNT];
	uint8_t varsion;
	uint16_t currentSize;
	bool initialized;
	bool loaded;
};

struct db DB = {.initialized = false};

static uint8_t _getIdxById(DBInstance *db, enum DBDataField id) {
	uint8_t idx = DB_FIELD_ID_NULL;

	for (uint8_t i=0; i<DB_FIELDS_COUNT; ++i) {
		if (db->info[i].id == id) {
			idx = i;
			break;
		}
	}

	return idx;
}

Error DB_Initialize () {
	Error ret = NO_ERROR;

	memset(&DB, 0, sizeof(struct db));

	for (uint8_t i = 1; i<DB_FIELDS_COUNT; ++i) {
		DB.info[i].id = DB_FIELD_ID_NULL;
	}

	DB.currentSize = 0;

	DB.initialized = true;
	return ret;
}

Error DB_Load (DBInstance **db) {
	Error ret = NO_ERROR;
	uint16_t address = 0;
	uint8_t id = 0;
	uint8_t size = 0;
	uint8_t idx = 0;//current index to write

	do {
		id = EEPROM_read(address);
		size = EEPROM_read(address+1);

		if (size != _MEMORY_FIELD_NULL) {
			DB.info[idx].address = address+2;
			DB.info[idx].id = id;
			DB.info[idx].size = size;

			if (id == DB_VERSION) {
				DB.varsion = EEPROM_read(DB.info[idx].address);
			}
			//TODO: check version and maintain if there was any upgrade.
			//Implement:
			//upgrade means some entry was deleted, remove obsolete entries and fill
			// all gaps by copy.

			++idx;
			address += 2 + (uint16_t)size;
		}
	} while (size != _MEMORY_FIELD_NULL);

	if (DB.varsion == 0) { //version info was not found, EEPROM is clear;
		EEPROM_write(address, DB_VERSION);
		EEPROM_write(address+1, 1);
		EEPROM_write(address+2, CURRENT_VERSION);

		address += 3;
		++idx;
	}

	DB.currentSize = address;

	*db = &DB;

	DB.loaded = true;

	return ret;
}

Error DB_GetDB (DBInstance **db) {
	Error ret = NO_ERROR;

	if ((*db)->loaded != true) {
		DB_Load(db);
	} else {
		*db=&DB;
	}

	return ret;
}

Error DB_GetEntry (DBInstance *db, enum DBDataField ID, uint8_t *data, uint8_t *size) {
	Error ret = NO_ERROR;

	uint8_t idx = _getIdxById(db, ID);

	if (idx == DB_FIELD_ID_NULL) {
		*size = 0;
	} else {
		if (db->info[idx].size == *size) {
			for (uint8_t i=0; i<*size; ++i) {
				data[i] = EEPROM_read(db->info[idx].address+i);
			}
		} else {
			ret = ERROR_INVALID_PARAMETER;
			*size = 0;
		}
	}

	return ret;
}

Error DB_PutEntry (DBInstance *db, enum DBDataField ID, uint8_t *data, uint8_t size) {
	Error ret = NO_ERROR;
	uint8_t idx = _getIdxById(db, ID);

	if (idx != DB_FIELD_ID_NULL) {//there is no entry in DB yet
		if (db->info[idx].size == size) {
			for (uint8_t i=0; i<size; ++i) {
				EEPROM_write(db->info[idx].address+i, data[i]);
			}
		} else {
			ret = ERROR_INVALID_PARAMETER;
		}
	} else {
		EEPROM_write(db->currentSize, ID);
		EEPROM_write(db->currentSize+1, size);
		for (uint8_t i=0; i<size; ++i) {
			EEPROM_write(db->currentSize+i+2, data[i]);
		}
		idx = _getIdxById(db, DB_FIELD_ID_NULL);
		db->info[idx].id = ID;
		db->info[idx].address = db->currentSize + 2;
		db->info[idx].size = size;
		db->currentSize+= (uint16_t) size+2;
	}


	return ret;
}

Error DB_FactoryReset (DBInstance *db) {
	Error ret = NO_ERROR;

	EEPROM_clear();

	DB_Load(&db);

	return ret;
}

