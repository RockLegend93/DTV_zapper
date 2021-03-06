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
#include "table_parser.h"
#include "remote.h"
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

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
void parsePatServiceInfoArray(uint8_t *buffer, PatServiceInfo patServiceInfoArray[], uint16_t section_length)
{
    int brojPidova = (section_length - 10) / 4;
    int kraj;
    int i = 0;
    if (brojPidova < MAX_NUM_OF_PIDS)
    {
        kraj = brojPidova;
    }
    else
    {
        kraj = MAX_NUM_OF_PIDS;
    }

    for (i = 0; i < kraj; i++)
    {
        patServiceInfoArray[i].program_number = (uint16_t) ((*(buffer + i * 4 + 8) << 8) + *(buffer + i * 4 + 9));
        patServiceInfoArray[i].pid = (uint16_t) (((*(buffer + i * 4 + 10) << 8) + *(buffer + i * 4 + 11)) & 0x1FFF);
    }
}

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
void parsePatHeader(uint8_t *buffer, PatHeader* patHeader)
{
    (*patHeader).table_id = (uint8_t) (*(buffer + 0));
    (*patHeader).section_syntax_indicator = (uint8_t) (*(buffer + 1) << 7);
    (*patHeader).section_length = (uint16_t) (((*(buffer + 1) << 8) + *(buffer + 2)) & 0x0FFF);
    (*patHeader).transport_stream_id = (uint16_t) ((*(buffer + 3) << 8) + *(buffer + 4));
    (*patHeader).version_number = (uint8_t) ((*(buffer + 5) >> 1) & 0x1F);
    (*patHeader).current_next_indicator = (uint8_t) (*(buffer + 5) & 0x01);
    (*patHeader).section_number = (uint8_t) (*(buffer + 6));
    (*patHeader).last_section_number = (uint8_t) (*(buffer + 7));
}

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
void dumpPatHeader(PatHeader* patHeader)
{
    printf("\n<<<<<<<<<<<<<<<< Pat header >>>>>>>>>>>>>\n");
    printf("Table id:%d\n", (*patHeader).table_id);
    printf("Section syntax: %d\n", (*patHeader).section_syntax_indicator);
    printf("Section length: %d\n", (*patHeader).section_length);
    printf("Transport stream ID: %d\n", (*patHeader).transport_stream_id);
    printf("Version number: %d\n", (*patHeader).version_number);
    printf("Current next indicator: %d\n", (*patHeader).current_next_indicator);
    printf("Section number: %d\n", (*patHeader).section_number);
    printf("Last section number: %d", (*patHeader).last_section_number);
}

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
void parsePatTable(uint8_t *buffer, PatTable* table)
{
    printf("%s started\n", __FUNCTION__);
    parsePatHeader(buffer, table->patHeader);
    printf("%s header parsed\n", __FUNCTION__);
    parsePatServiceInfoArray(buffer, table->patServiceInfoArray, table->patHeader->section_length);
    printf("%s service info parsed\n", __FUNCTION__);
    (*table).serviceInfoCount = (uint8_t) ((*(*table).patHeader).section_length - 10) / 4;
}

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
void dumpPatServiceInfo(PatServiceInfo* patServiceInfo)
{
    printf("Program number: %d,pid: %d\n", patServiceInfo->program_number, patServiceInfo->pid);
}

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
void dumpPatTable(PatTable* table)
{
    int i = 0;
    dumpPatHeader((table->patHeader));
    printf("\n<<<<<<<<<<<<<<<< Pat service info >>>>>>>>>>>>>\n");
    for (i = 0; i < table->serviceInfoCount; i++)
        dumpPatServiceInfo(&(table->patServiceInfoArray[i]));
}

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
void parsePmt(uint8_t *buffer, PmtTable* table)
{
    parsePmtHeader(buffer, table->pmtHeader);
    parsePmtServiceInfoArray(buffer, table->pmtServiceInfoArray, &((*table).streamCount), &(table->teletekst));
}

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
void parsePmtHeader(uint8_t *buffer, PmtHeader* pmtHeader)
{
    (*pmtHeader).table_id = (uint8_t) (*(buffer + 0));
    (*pmtHeader).section_syntax_indicator = (uint8_t) (*(buffer + 1) >> 7);
    (*pmtHeader).section_length = (uint16_t) (((*(buffer + 1) << 8) + *(buffer + 2)) & 0x0FFF);
    (*pmtHeader).program_number = (uint16_t) ((*(buffer + 3) << 8) + *(buffer + 4));
    (*pmtHeader).version_number = (uint8_t) ((*(buffer + 5) >> 1) & 0x1F);
    (*pmtHeader).current_next_indicator = (uint8_t) (*(buffer + 5) & 0x01);
    (*pmtHeader).section_number = (uint8_t) (*(buffer + 6));
    (*pmtHeader).last_section_number = (uint8_t) (*(buffer + 7));
    (*pmtHeader).pcr_pid = (uint16_t) (((*(buffer + 8) << 8) + *(buffer + 9)) & 0x1FFF);
    (*pmtHeader).program_info_length = (uint16_t) (((*(buffer + 10) << 8) + *(buffer + 11)) & 0x0FFF);
}

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
void parsePmtServiceInfoArray(uint8_t *buffer, PmtServiceInfo pmtServiceInfoArray[], uint8_t* broj, uint8_t* teletekst)
{
    uint8_t section_length = (uint16_t) (((*(buffer + 1) << 8) + *(buffer + 2)) & 0x0FFF);
    uint16_t program_info_length = (uint16_t) (((*(buffer + 10) << 8) + *(buffer + 11)) & 0x0FFF);
    int kraj = section_length - 1;
    int poc = program_info_length + 3 + 9;
    int i = 0;
    *teletekst = 0;
    for (i = 0; poc < kraj; i++)
    {
        pmtServiceInfoArray[i].stream_type = (uint8_t) (*(buffer + poc));
        poc++;
        pmtServiceInfoArray[i].el_pid = (uint16_t) (((*(buffer + poc) << 8) + *(buffer + poc + 1)) & 0x1FFF);
        poc += 2;
        pmtServiceInfoArray[i].es_info_length = (uint16_t) (((*(buffer + poc) << 8) + *(buffer + poc + 1)) & 0x0FFF);
        poc += 2;
        if (pmtServiceInfoArray[i].es_info_length > 0)
        {
            if (*teletekst == 0)
                *teletekst = (*(buffer + poc) == 0x56);
        }
        poc += pmtServiceInfoArray[i].es_info_length;
    }
    *broj = i;
}

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
void dumpPmtTable(PmtTable* pmtTable)
{
    int i = 0;
    printf("<<<<<<<<<<<<<<<<<<<<<<<<<<<< PMT TABLE >>>>>>>>>>>>>>>>>>>>>>>>>>\n");
    printf("Table id: %d\n", pmtTable->pmtHeader->table_id);
    printf("Service info count: %d\n", pmtTable->streamCount);
    printf("Section syntax indicator id: %d\n", pmtTable->pmtHeader->section_syntax_indicator);
    printf("section_length: %d\n", pmtTable->pmtHeader->section_length);
    printf("program_number: %d\n", pmtTable->pmtHeader->program_number);
    printf("version_number: %d\n", pmtTable->pmtHeader->version_number);
    printf("current_next_indicator: %d\n", pmtTable->pmtHeader->current_next_indicator);
    printf("section_number: %d\n", pmtTable->pmtHeader->section_number);
    printf("last_section_number: %d\n", pmtTable->pmtHeader->last_section_number);
    printf("pcr_pid: %d\n", pmtTable->pmtHeader->pcr_pid);
    printf("program_info_length: %d\n", pmtTable->pmtHeader->program_info_length);
    for (i = 0; i < pmtTable->streamCount; i++)
    {
        printf("Service Type: %d el_pid: %d es_info_length %d\n", pmtTable->pmtServiceInfoArray[i].stream_type, pmtTable->pmtServiceInfoArray[i].el_pid, pmtTable->pmtServiceInfoArray[i].es_info_length);
    }
}

/****************************************************************************
 *
 * @brief
Fukcija koja se koristi za ispis vrijednosti buffera u heksadecimalnom sistemu standarni izlaz,
 * na taj nacin ova funkcija olaksava manipulaciju novim tabelama koje u skopu ovog projekta nisu parsirane
 *
 * @param
table - [in]  PAT tabela koja ce biti ispisana na standardni izlaz
 *
 *
 *
 *****************************************************************************/
void dumpBuffer(uint8_t* buffer)
{
    int i = 0;
    printf("\n<<<<<<<<<<<<<<<<<Buffer>>>>>>>>>>>>>>>>>>>>>>>>\n");
    for (i = 0; i < 26; i++)
    {
        printf("%x ", buffer[i]);
    }
    printf("\n<<<<<<<<<<<<<<<<<Buffer>>>>>>>>>>>>>>>>>>>>>>>>\n");
}

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
void parseEitTable(uint8_t* buffer, EitTable* table)
{
    parseEitHeader(buffer, &(table->header));
    dumpEitHeader(&(table->header));
    parseEitEvent(buffer + 13, &(table->events[0]));
    dumpEitEvent(&(table->events[0]));
    dumpBuffer(buffer);
}

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
void parseEitHeader(uint8_t* buffer, EitHeader* header)
{
    header->table_id = buffer[0];
    header->section_syntax_indicator = buffer[1] >> 7;
    header->section_length = (uint16_t) (((*(buffer + 1) << 8) + *(buffer + 2)) & 0x0FFF);
    header->service_id = (uint16_t) ((*(buffer + 3) << 8) + *(buffer + 4));
    header->version_number = (buffer[5]&0x3E) >> 1;
    header->current_next_indicator = buffer[5]&0x01;
    header->section_number = buffer[6];
    header->last_section_number = buffer[7];
    header->transport_stream_id = buffer[8] << 8 + buffer[9];
    header->original_network_id = buffer[10] << 8 + buffer[11];
    header->segment_last_section_number = buffer[12];
    header->last_table_id = buffer[13];
}

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
void parseEitEvent(uint8_t* buffer, EitEvents* event)
{
    event->event_id = buffer[0] << 8 + buffer[1];
    event->start_time[0] = buffer[2];
    event->start_time[1] = buffer[3];
    event->start_time[2] = buffer[4];
    event->start_time[3] = buffer[5];
    event->start_time[4] = buffer[6];
    event->durration[0] = buffer[7];
    event->durration[1] = buffer[8];
    event->durration[2] = buffer[9];
    event->descriptor_loop_length = (buffer[10]&0xF0) << 8 + buffer[11];
}

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
void dumpEitEvent(EitEvents *event)
{
    printf("\tEvent id: %d", event->event_id);
    printf("\tStart time %x%x%x%x%x\n", event->start_time[0], event->start_time[1], event->start_time[2], event->start_time[3], event->start_time[4]);
    printf("\tDuration %x:%x:%x\n", event->durration[0], event->durration[1], event->durration[2]);
    printf("\tDescriptors loop length %d\n", event->descriptor_loop_length);
}

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
void dumpEitHeader(EitHeader* table)
{
    printf("\n<<<<<<<<<<<<<<<<EIT table>>>>>>>>>>>>>>>>>>\n");
    printf("Table id: %d\n", table->table_id);
    printf("Section syntax indicator %d\n", table->section_syntax_indicator);
    printf("Section length %d\n", table->section_length);
    printf("Service id %d\n", table->service_id);
    printf("Version number %d\n", table->version_number);
    printf("current_next_indicator %d\n", table->current_next_indicator);
    printf("section_number %d\n", table->section_number);
    printf("last_section_number %d\n", table->last_section_number);
    printf("transport_stream_id %d\n", table->transport_stream_id);
    printf("original_network_id %d\n", table->original_network_id);
    printf("segment_last_section_number %d", table->segment_last_section_number);
    printf("last_table_id %d\n", table->last_table_id);
    printf("<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>\n");
}
