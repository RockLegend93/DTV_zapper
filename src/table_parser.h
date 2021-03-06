/****************************************************************************
 *
 * Univerzitet u Banjoj Luci, Elektrotehnicki fakultet
 *
 * -----------------------------------------------------
 * Ispitni zadatak iz predmeta:
 *
 * MULTIMEDIJALNI SISTEMI
 * -----------------------------------------------------
 * DTV Zapper
 * -----------------------------------------------------
 *
 * \file table_parser.c
 * \brief
 * Ovaj modul realizuje parsiranje PMT,PAT i EIT tabela, uz postojanje fukcija za
 * ispis sadrzaja na standardni izlaz.
 * 
 * @Author Milan Maric
 *
 *****************************************************************************/

#ifndef TABLES_H_
#define TABLES_H_

#include <stdint.h>
#include "config_parser.h"

#define MAX_NUM_OF_PIDS 20
#define PARSING_ERROR -1
#define INIT_ERROR -1
#define MAX_NUM_OF_EVENTS 5

typedef struct _PatHeader
{
    uint8_t table_id;
    uint8_t section_syntax_indicator;
    uint16_t section_length;
    uint16_t transport_stream_id;
    uint8_t version_number;
    uint8_t current_next_indicator;
    uint8_t section_number;
    uint8_t last_section_number;
} PatHeader;

typedef struct _PatServiceInfo
{
    uint16_t program_number;
    uint16_t pid;
} PatServiceInfo;

typedef struct _PatTable
{
    PatServiceInfo patServiceInfoArray[MAX_NUM_OF_PIDS];
    uint8_t serviceInfoCount;
    PatHeader* patHeader;
} PatTable;

typedef struct _PmtHeader
{
    uint8_t table_id;
    uint8_t section_syntax_indicator;
    uint16_t section_length;
    uint16_t program_number;
    uint8_t version_number;
    uint8_t current_next_indicator;
    uint8_t section_number;
    uint8_t last_section_number;
    uint16_t pcr_pid;
    uint16_t program_info_length;
} PmtHeader;

typedef struct _PmtServiceInfo
{
    uint8_t stream_type;
    uint16_t el_pid;
    uint16_t es_info_length;
} PmtServiceInfo;

typedef struct _PmtTable
{
    PmtHeader* pmtHeader;
    PmtServiceInfo pmtServiceInfoArray[MAX_NUM_OF_PIDS];
    uint8_t streamCount;
    uint8_t teletekst;
} PmtTable;

typedef struct _ShortEventDesriptor
{
    uint8_t dvb_DescriptorTag;
    uint8_t descriptor_length;
    uint8_t languageCode;
    uint8_t event_name_length;
    char* event_name;
} ShortEventDescriptor;

typedef struct _EitEvents
{
    uint16_t event_id; //	16	This 16 bit field indicates the event id of the event for which information is given. Within a service this id must be unique.
    uint8_t start_time[5]; //start time	40	This 40 bit field gives the start time and date in UTC and MJD of the event. The first 16 bits represent the 16 bits MJD,then the 24-bit UTC as 6 digits in 4-bit BCD
    uint8_t durration[3]; //	24	This 24 bit field indicates the length of the event in hours, minutes, seconds as 4 bits BCD. for instance 02:25:30 is encoded as 0x022530
    uint8_t running_status; //	3	This field gives information about the status of the event, 000 = undefined, 001 = not running, 010 = start in a few seconds, 011 = pause, 100 = running, 101 - 111 reserved for future use. In the case of an NVOD reference event, the running status will be put to '0'
    uint16_t descriptor_loop_length; //12	The length of the descriptor loop.
    ShortEventDescriptor descriptors[MAX_NUM_OF_PIDS];
} EitEvents;

typedef struct _EitHeader
{
    uint8_t table_id; //8 	Indicates to which table this section belongs, in this case EIT.
    uint8_t section_syntax_indicator; //1 Indicates whether a sub-table structure including CRC check is used.
    uint16_t section_length; // 12	The length of the section in bytes. This length starts immediately after this field and includes the CRC
    uint16_t service_id; //16	Indicates on which service this EIT table informs.
    uint8_t version_number; //	5	Value between 0 to 31. A higher value indicates that the information has changed.
    uint8_t current_next_indicator; //	1	This is to indicate whether a section is 'valid now' or 'valid in future'
    uint8_t section_number; //	8	Value between 0x00 - 0xFF. Used to indicate the sections of a table. Up to 256 sections
    uint8_t last_section_number; //	8	The number of the last section, so the receiver know when the table is completely received.
    uint16_t transport_stream_id; //	16	Indicates on what TS this EIT table information provides.
    uint16_t original_network_id; //	12	The ID of the original network where this transport stream originated.
    uint8_t segment_last_section_number; //	8	This 8 bit field gives the number of the last section of this segment of the sub-table. If the sub-table is not segmented, then this field must have the same value as the last section number field.
    uint8_t last_table_id; //	8	This 8 bit field indicates the last used table_id.
} EitHeader;

typedef struct _EitTable
{
    EitHeader header;
    EitEvents events[MAX_NUM_OF_EVENTS];
} EitTable;

/****************************************************************************
 *
 * @brief
Fukcija koja se koristi za parsiranje EIT tabele
 *
 * @param
buff - [in] Ulazni bafer sa odmercima
 *
table - [out] tabela u koju je potrebno upisati vrijednosti
 *
 *
 *
 *****************************************************************************/
void parseEitTable(uint8_t* buffer, EitTable* table);

/****************************************************************************
 *
 * @brief
 * Fukcija koja se koristi za ispis zaglavlja EIT tabele na standardni izlaz
 *
 * @param
 * table - [in] tabela cije ce zaglavlje biti ispisano
 *
 *
 *
 *****************************************************************************/
void dumpEitHeader(EitHeader* table);

/****************************************************************************
 *
 * @brief
Fukcija koja se koristi za parsiranje zaglavlja EIT tabele
 *
 * @param
buff - [in] Ulazni bafer sa odmercima
 *
header - [out] zaglavlje u koje je potrebno upisati vrijednosti
 *
 *
 *
 *****************************************************************************/
void parseEitHeader(uint8_t* buffer, EitHeader* header);

/****************************************************************************
 *
 * @brief
Fukcija koja se koristi za parsiranje EIT dogadjaja (event)
 *
 * @param
buff - [in] Ulazni bafer sa odmercima
 *
event - [out] zaglavlje u koje je potrebno upisati vrijednosti
 *
 *
 *
 *****************************************************************************/
void parseEitEvent(uint8_t* buffer, EitEvents* event);

/****************************************************************************
 *
 * @brief
 * Fukcija koja se koristi za ispis EIT evenata na standardni izlaz
 *
 * @param
buff - [in] Ulazni bafer sa odmercima
 *
patHeader - [out] zaglavlje u koje je potrebno upisati vrijednosti
 *
 *
 *
 *****************************************************************************/
void dumpEitEvent(EitEvents* event);

/****************************************************************************
 *
 * @brief
Fukcija koja se koristi za parsiranje niza servisnih informacija iz PAT tabele
 *
 * @param
buff - [in] Ulazni bafer sa odmercima
 *
patServiceInfoArray - [out] niz servisnih informacija u koje je potrebno upisati parsirane vrijednosti
 *
section_length - [in] vrijednost koja predstavlja duzinu sekcije(polje iz PAT header - a)
 *
 *
 *
 *****************************************************************************/
void parsePatServiceInfoArray(uint8_t *buffer, PatServiceInfo patServiceInfoArray[], uint16_t section_length);

/****************************************************************************
 *
 * @brief
Fukcija koja se koristi za parsiranje zaglavlja PAT tabele
 *
 * @param
buff - [in] Ulazni bafer sa odmercima
 *
patHeader - [out] zaglavlje u koje je potrebno upisati vrijednosti
 *
 *
 *
 *****************************************************************************/
void parsePatHeader(uint8_t *buffer, PatHeader* patHeader);

/****************************************************************************
 *
 * @brief
Fukcija koja se koristi za parsiranje PAT tabele
 *
 * @param
buff - [in] Ulazni bafer sa odmercima
 *
table - [out] tabela u koju je potrebno upisati vrijednosti
 *
 *
 *
 *****************************************************************************/
void parsePatTable(uint8_t *buffer, PatTable* table);

/****************************************************************************
 *
 * @brief
Fukcija koja se koristi za parsiranje PMT tabele
 *
 * @param
buff - [in] Ulazni bafer sa odmercima
 *
table - [out] tabela u koju je potrebno upisati vrijednosti
 *
 *
 *
 *****************************************************************************/
void parsePmt(uint8_t *buffer, PmtTable* table);

/****************************************************************************
 *
 * @brief
Fukcija koja se koristi za parsiranje zaglavlja PMT tabele
 *
 * @param
buff - [in] Ulazni bafer sa odmercima
 *
patHeader - [out] zaglavlje u koje je potrebno upisati vrijednosti
 *
 *
 *
 *****************************************************************************/
void parsePmtHeader(uint8_t *buffer, PmtHeader* pmtHeader);

/****************************************************************************
 *
 * @brief
Fukcija koja se koristi za parsiranje niza servisnih informacija iz PMT tabele
 *
 * @param
buff - [in] Ulazni bafer sa odmercima
 *
pmtServiceInfoArray - [out] niz servisnih informacija u koje je potrebno upisati parsirane vrijednosti
 *
broj - [out] broj sekcija koje ce biti parsirane
 *
 *
 *
 *****************************************************************************/
void parsePmtServiceInfoArray(uint8_t *buffer, PmtServiceInfo pmtServiceInfoArray[], uint8_t* broj, uint8_t* teletekst);

/****************************************************************************
 *
 * @brief
Fukcija koja se koristi za ispis PAT tabele na standarni izlaz
 *
 * @param
table - [in]  PAT tabela koja ce biti ispisana na standardni izlaz
 *
 *
 *
 *****************************************************************************/
void dumpPatTable(PatTable* table);

/****************************************************************************
 *
 * @brief
Fukcija koja se koristi za ispis PMT tabele na standarni izlaz
 *
 * @param
table - [in]  PAT tabela koja ce biti ispisana na standardni izlaz
 *
 *
 *
 *****************************************************************************/
void dumpPmtTable(PmtTable* pmtTable);

/****************************************************************************
 *
 * @brief
Fukcija koja se koristi za ispis zaglavlja PAT tabele na standarni izlaz
 *
 * @param
patHeader - [in] zaglavlje PAT tabele
 *
 *
 *
 *****************************************************************************/
void dumpPatHeader(PatHeader* patHeader);

/****************************************************************************
 *
 * @brief
Fukcija koja se koristi za ispis niza servisnih informacija PAT tabele na standarni izlaz
 *
 * @param
patServiceInfo - [in] niz servisnih informacija PAT tabele
 *
 *
 *
 *****************************************************************************/
void dumpPatServiceInfo(PatServiceInfo* patServiceInfo);




#endif