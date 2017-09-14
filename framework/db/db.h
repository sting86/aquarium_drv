/*
 * db.h
 *
 *  Created on: 10 wrz 2017
 *      Author: Pawe³
 */

#ifndef FRAMEWORK_DB_DB_H_
#define FRAMEWORK_DB_DB_H_

#include "framework/typedefs.h"
#include "framework/error.h"

enum DBDataField {
	DB_VERSION,
	DB_FIELD_ID_LIGHT_ON,
	DB_FIELDS_COUNT,
	DB_FIELD_ID_NULL = 0xFF
};

typedef struct db DBInstance;

Error DB_Initialize ();
Error DB_Load (DBInstance **db);
Error DB_GetDB (DBInstance **db);

Error DB_GetEntry (DBInstance *db, enum DBDataField ID, uint8_t *data, uint8_t *size);
Error DB_PutEntry (DBInstance *db, enum DBDataField ID, uint8_t *data, uint8_t size);
Error DB_FactoryReset (DBInstance *db);


#endif /* FRAMEWORK_DB_DB_H_ */
