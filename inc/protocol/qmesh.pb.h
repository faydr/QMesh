/* Automatically generated nanopb header */
/* Generated by nanopb-0.4.1 */

#ifndef PB_QMESH_PB_H_INCLUDED
#define PB_QMESH_PB_H_INCLUDED
#include <pb.h>

#if PB_PROTO_HEADER_VERSION != 40
#error Regenerate this file with the current version of nanopb generator.
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Enum definitions */
typedef enum _FECCfg_Type {
    FECCfg_Type_NONE = 0,
    FECCfg_Type_INTERLEAVE = 1,
    FECCfg_Type_CONV = 2,
    FECCfg_Type_RSV = 3,
    FECCfg_Type_RSVGOLAY = 4
} FECCfg_Type;

typedef enum _RadioCfg_Type {
    RadioCfg_Type_LORA = 0,
    RadioCfg_Type_FSK = 1
} RadioCfg_Type;

typedef enum _SysCfgMsg_Mode {
    SysCfgMsg_Mode_NORMAL = 0,
    SysCfgMsg_Mode_MODE_RESET = 1,
    SysCfgMsg_Mode_MODE_SILENT = 2,
    SysCfgMsg_Mode_TESTING = 3
} SysCfgMsg_Mode;

typedef enum _StatusMsg_Status {
    StatusMsg_Status_BOOTING = 0,
    StatusMsg_Status_MANAGEMENT = 1,
    StatusMsg_Status_RUNNING = 2
} StatusMsg_Status;

typedef enum _SerialMsg_Type {
    SerialMsg_Type_GET_CONFIG = 0,
    SerialMsg_Type_SET_CONFIG = 1,
    SerialMsg_Type_CONFIG = 2,
    SerialMsg_Type_DATA = 3,
    SerialMsg_Type_CLOCK_SET = 4,
    SerialMsg_Type_STATUS = 5,
    SerialMsg_Type_GET_STATUS = 6,
    SerialMsg_Type_STAY_IN_MGT = 7,
    SerialMsg_Type_DEBUG_MSG = 8,
    SerialMsg_Type_REBOOT = 9,
    SerialMsg_Type_ERASE_LOGS = 10,
    SerialMsg_Type_ERASE_BOOT_LOGS = 11,
    SerialMsg_Type_ERASE_CFG = 12,
    SerialMsg_Type_READ_LOG = 13,
    SerialMsg_Type_READ_LOG_RETRY = 14,
    SerialMsg_Type_REPLY_LOG = 15,
    SerialMsg_Type_READ_BOOT_LOG = 16,
    SerialMsg_Type_READ_BOOT_LOG_RETRY = 17,
    SerialMsg_Type_REPLY_BOOT_LOG = 18,
    SerialMsg_Type_CRC_ERR = 19,
    SerialMsg_Type_SET_TIME = 20,
    SerialMsg_Type_ACK = 21,
    SerialMsg_Type_ERR = 22,
    SerialMsg_Type_ENTER_KISS_MODE = 23
} SerialMsg_Type;

typedef enum _DataMsg_Type {
    DataMsg_Type_TX = 0,
    DataMsg_Type_RX = 1,
    DataMsg_Type_KISSTX = 2,
    DataMsg_Type_KISSRX = 3
} DataMsg_Type;

/* Struct definitions */
typedef struct _BootLogMsg {
    bool valid;
    uint32_t boot_time;
    uint32_t count;
} BootLogMsg;

typedef struct _ClockSetMsg {
    uint32_t time;
} ClockSetMsg;

typedef PB_BYTES_ARRAY_T(256) DataMsg_payload_t;
typedef struct _DataMsg {
    DataMsg_Type type;
    uint32_t stream_id;
    uint32_t ttl;
    uint32_t sender;
    uint32_t sym_offset;
    DataMsg_payload_t payload;
    uint32_t crc;
} DataMsg;

typedef struct _DbgMsg {
    char msg[256];
} DbgMsg;

typedef struct _ErrorMsg {
    char msg[256];
} ErrorMsg;

typedef struct _FECCfg {
    FECCfg_Type type;
    int32_t conv_rate;
    int32_t conv_order;
    int32_t rs_num_roots;
} FECCfg;

typedef struct _GPSMsg {
    bool valid;
    float lat;
    float lon;
} GPSMsg;

typedef struct _LoraCfg {
    uint32_t bw;
    uint32_t cr;
    uint32_t sf;
    uint32_t preamble_length;
    uint32_t fhss_pre_len;
} LoraCfg;

typedef struct _NetCfg {
    char beacon_msg[256];
    uint32_t beacon_interval;
    uint32_t num_offsets;
    uint32_t pld_len;
    uint32_t full_pkt_len;
} NetCfg;

typedef struct _POCSAGCfg {
    bool enabled;
    uint32_t frequency;
    uint32_t beacon_interval;
} POCSAGCfg;

typedef struct _SerialCRCMsg {
    uint32_t crc32;
} SerialCRCMsg;

typedef struct _StatusMsg {
    StatusMsg_Status status;
    bool tx_full;
    uint32_t time;
} StatusMsg;

typedef struct _TestCfg {
    bool cw_test_mode;
    bool preamble_test_mode;
    bool test_fec;
} TestCfg;

typedef struct _TimeMsg {
    uint32_t time;
} TimeMsg;

typedef struct _LogMsg {
    bool valid;
    uint32_t count;
    uint32_t timestamp;
    uint32_t sender;
    uint32_t ttl;
    uint32_t stream_id;
    int32_t rssi;
    int32_t snr;
    int32_t rx_size;
    uint32_t comp_crc;
    uint32_t crc;
    uint32_t uptime;
    bool has_gps_msg;
    GPSMsg gps_msg;
} LogMsg;

typedef struct _RadioCfg {
    RadioCfg_Type type;
    int32_t frequency;
    pb_size_t frequencies_count;
    int32_t frequencies[16];
    int32_t tx_power;
    bool has_lora_cfg;
    LoraCfg lora_cfg;
} RadioCfg;

typedef struct _SysCfgMsg {
    SysCfgMsg_Mode mode;
    uint32_t address;
    bool has_radio_cfg;
    RadioCfg radio_cfg;
    bool has_test_cfg;
    TestCfg test_cfg;
    bool has_fec_cfg;
    FECCfg fec_cfg;
    bool has_net_cfg;
    NetCfg net_cfg;
    bool has_pocsag_cfg;
    POCSAGCfg pocsag_cfg;
    bool gps_en;
    bool log_packets_en;
    bool boot_log_en;
} SysCfgMsg;

typedef struct _SerialMsg {
    SerialMsg_Type type;
    bool retry;
    bool has_sys_cfg;
    SysCfgMsg sys_cfg;
    bool has_clock_set;
    ClockSetMsg clock_set;
    bool has_status;
    StatusMsg status;
    bool has_dbg_msg;
    DbgMsg dbg_msg;
    bool has_log_msg;
    LogMsg log_msg;
    bool has_boot_log_msg;
    BootLogMsg boot_log_msg;
    bool has_data_msg;
    DataMsg data_msg;
    bool has_error_msg;
    ErrorMsg error_msg;
    bool has_time_msg;
    TimeMsg time_msg;
} SerialMsg;


/* Helper constants for enums */
#define _FECCfg_Type_MIN FECCfg_Type_NONE
#define _FECCfg_Type_MAX FECCfg_Type_RSVGOLAY
#define _FECCfg_Type_ARRAYSIZE ((FECCfg_Type)(FECCfg_Type_RSVGOLAY+1))

#define _RadioCfg_Type_MIN RadioCfg_Type_LORA
#define _RadioCfg_Type_MAX RadioCfg_Type_FSK
#define _RadioCfg_Type_ARRAYSIZE ((RadioCfg_Type)(RadioCfg_Type_FSK+1))

#define _SysCfgMsg_Mode_MIN SysCfgMsg_Mode_NORMAL
#define _SysCfgMsg_Mode_MAX SysCfgMsg_Mode_TESTING
#define _SysCfgMsg_Mode_ARRAYSIZE ((SysCfgMsg_Mode)(SysCfgMsg_Mode_TESTING+1))

#define _StatusMsg_Status_MIN StatusMsg_Status_BOOTING
#define _StatusMsg_Status_MAX StatusMsg_Status_RUNNING
#define _StatusMsg_Status_ARRAYSIZE ((StatusMsg_Status)(StatusMsg_Status_RUNNING+1))

#define _SerialMsg_Type_MIN SerialMsg_Type_GET_CONFIG
#define _SerialMsg_Type_MAX SerialMsg_Type_ENTER_KISS_MODE
#define _SerialMsg_Type_ARRAYSIZE ((SerialMsg_Type)(SerialMsg_Type_ENTER_KISS_MODE+1))

#define _DataMsg_Type_MIN DataMsg_Type_TX
#define _DataMsg_Type_MAX DataMsg_Type_KISSRX
#define _DataMsg_Type_ARRAYSIZE ((DataMsg_Type)(DataMsg_Type_KISSRX+1))


/* Initializer values for message structs */
#define LoraCfg_init_default                     {0, 0, 0, 0, 0}
#define TestCfg_init_default                     {0, 0, 0}
#define FECCfg_init_default                      {_FECCfg_Type_MIN, 0, 0, 0}
#define RadioCfg_init_default                    {_RadioCfg_Type_MIN, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 0, false, LoraCfg_init_default}
#define NetCfg_init_default                      {"", 0, 0, 0, 0}
#define POCSAGCfg_init_default                   {0, 0, 0}
#define SysCfgMsg_init_default                   {_SysCfgMsg_Mode_MIN, 0, false, RadioCfg_init_default, false, TestCfg_init_default, false, FECCfg_init_default, false, NetCfg_init_default, false, POCSAGCfg_init_default, 0, 0, 0}
#define ClockSetMsg_init_default                 {0}
#define StatusMsg_init_default                   {_StatusMsg_Status_MIN, 0, 0}
#define DbgMsg_init_default                      {""}
#define SerialCRCMsg_init_default                {0}
#define BootLogMsg_init_default                  {0, 0, 0}
#define GPSMsg_init_default                      {0, 0, 0}
#define LogMsg_init_default                      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, GPSMsg_init_default}
#define TimeMsg_init_default                     {0}
#define SerialMsg_init_default                   {_SerialMsg_Type_MIN, 0, false, SysCfgMsg_init_default, false, ClockSetMsg_init_default, false, StatusMsg_init_default, false, DbgMsg_init_default, false, LogMsg_init_default, false, BootLogMsg_init_default, false, DataMsg_init_default, false, ErrorMsg_init_default, false, TimeMsg_init_default}
#define ErrorMsg_init_default                    {""}
#define DataMsg_init_default                     {_DataMsg_Type_MIN, 0, 0, 0, 0, {0, {0}}, 0}
#define LoraCfg_init_zero                        {0, 0, 0, 0, 0}
#define TestCfg_init_zero                        {0, 0, 0}
#define FECCfg_init_zero                         {_FECCfg_Type_MIN, 0, 0, 0}
#define RadioCfg_init_zero                       {_RadioCfg_Type_MIN, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 0, false, LoraCfg_init_zero}
#define NetCfg_init_zero                         {"", 0, 0, 0, 0}
#define POCSAGCfg_init_zero                      {0, 0, 0}
#define SysCfgMsg_init_zero                      {_SysCfgMsg_Mode_MIN, 0, false, RadioCfg_init_zero, false, TestCfg_init_zero, false, FECCfg_init_zero, false, NetCfg_init_zero, false, POCSAGCfg_init_zero, 0, 0, 0}
#define ClockSetMsg_init_zero                    {0}
#define StatusMsg_init_zero                      {_StatusMsg_Status_MIN, 0, 0}
#define DbgMsg_init_zero                         {""}
#define SerialCRCMsg_init_zero                   {0}
#define BootLogMsg_init_zero                     {0, 0, 0}
#define GPSMsg_init_zero                         {0, 0, 0}
#define LogMsg_init_zero                         {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, GPSMsg_init_zero}
#define TimeMsg_init_zero                        {0}
#define SerialMsg_init_zero                      {_SerialMsg_Type_MIN, 0, false, SysCfgMsg_init_zero, false, ClockSetMsg_init_zero, false, StatusMsg_init_zero, false, DbgMsg_init_zero, false, LogMsg_init_zero, false, BootLogMsg_init_zero, false, DataMsg_init_zero, false, ErrorMsg_init_zero, false, TimeMsg_init_zero}
#define ErrorMsg_init_zero                       {""}
#define DataMsg_init_zero                        {_DataMsg_Type_MIN, 0, 0, 0, 0, {0, {0}}, 0}

/* Field tags (for use in manual encoding/decoding) */
#define BootLogMsg_valid_tag                     1
#define BootLogMsg_boot_time_tag                 2
#define BootLogMsg_count_tag                     3
#define ClockSetMsg_time_tag                     1
#define DataMsg_type_tag                         1
#define DataMsg_stream_id_tag                    2
#define DataMsg_ttl_tag                          3
#define DataMsg_sender_tag                       4
#define DataMsg_sym_offset_tag                   5
#define DataMsg_payload_tag                      6
#define DataMsg_crc_tag                          7
#define DbgMsg_msg_tag                           1
#define ErrorMsg_msg_tag                         1
#define FECCfg_type_tag                          1
#define FECCfg_conv_rate_tag                     2
#define FECCfg_conv_order_tag                    3
#define FECCfg_rs_num_roots_tag                  4
#define GPSMsg_valid_tag                         1
#define GPSMsg_lat_tag                           2
#define GPSMsg_lon_tag                           3
#define LoraCfg_bw_tag                           1
#define LoraCfg_cr_tag                           2
#define LoraCfg_sf_tag                           3
#define LoraCfg_preamble_length_tag              4
#define LoraCfg_fhss_pre_len_tag                 5
#define NetCfg_beacon_msg_tag                    1
#define NetCfg_beacon_interval_tag               2
#define NetCfg_num_offsets_tag                   3
#define NetCfg_pld_len_tag                       4
#define NetCfg_full_pkt_len_tag                  5
#define POCSAGCfg_enabled_tag                    1
#define POCSAGCfg_frequency_tag                  2
#define POCSAGCfg_beacon_interval_tag            3
#define SerialCRCMsg_crc32_tag                   1
#define StatusMsg_status_tag                     1
#define StatusMsg_tx_full_tag                    2
#define StatusMsg_time_tag                       3
#define TestCfg_cw_test_mode_tag                 1
#define TestCfg_preamble_test_mode_tag           2
#define TestCfg_test_fec_tag                     3
#define TimeMsg_time_tag                         1
#define LogMsg_valid_tag                         1
#define LogMsg_count_tag                         2
#define LogMsg_timestamp_tag                     3
#define LogMsg_sender_tag                        4
#define LogMsg_ttl_tag                           5
#define LogMsg_stream_id_tag                     6
#define LogMsg_rssi_tag                          7
#define LogMsg_snr_tag                           8
#define LogMsg_rx_size_tag                       9
#define LogMsg_comp_crc_tag                      10
#define LogMsg_crc_tag                           11
#define LogMsg_uptime_tag                        12
#define LogMsg_gps_msg_tag                       13
#define RadioCfg_type_tag                        1
#define RadioCfg_frequency_tag                   2
#define RadioCfg_frequencies_tag                 3
#define RadioCfg_tx_power_tag                    4
#define RadioCfg_lora_cfg_tag                    5
#define SysCfgMsg_mode_tag                       1
#define SysCfgMsg_address_tag                    2
#define SysCfgMsg_radio_cfg_tag                  3
#define SysCfgMsg_test_cfg_tag                   4
#define SysCfgMsg_fec_cfg_tag                    5
#define SysCfgMsg_net_cfg_tag                    6
#define SysCfgMsg_pocsag_cfg_tag                 7
#define SysCfgMsg_gps_en_tag                     8
#define SysCfgMsg_log_packets_en_tag             9
#define SysCfgMsg_boot_log_en_tag                10
#define SerialMsg_type_tag                       1
#define SerialMsg_retry_tag                      2
#define SerialMsg_sys_cfg_tag                    3
#define SerialMsg_clock_set_tag                  4
#define SerialMsg_status_tag                     5
#define SerialMsg_dbg_msg_tag                    6
#define SerialMsg_log_msg_tag                    7
#define SerialMsg_boot_log_msg_tag               8
#define SerialMsg_data_msg_tag                   9
#define SerialMsg_error_msg_tag                  10
#define SerialMsg_time_msg_tag                   11

/* Struct field encoding specification for nanopb */
#define LoraCfg_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, UINT32,   bw,                1) \
X(a, STATIC,   SINGULAR, UINT32,   cr,                2) \
X(a, STATIC,   SINGULAR, UINT32,   sf,                3) \
X(a, STATIC,   SINGULAR, UINT32,   preamble_length,   4) \
X(a, STATIC,   SINGULAR, UINT32,   fhss_pre_len,      5)
#define LoraCfg_CALLBACK NULL
#define LoraCfg_DEFAULT NULL

#define TestCfg_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, BOOL,     cw_test_mode,      1) \
X(a, STATIC,   SINGULAR, BOOL,     preamble_test_mode,   2) \
X(a, STATIC,   SINGULAR, BOOL,     test_fec,          3)
#define TestCfg_CALLBACK NULL
#define TestCfg_DEFAULT NULL

#define FECCfg_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, UENUM,    type,              1) \
X(a, STATIC,   SINGULAR, INT32,    conv_rate,         2) \
X(a, STATIC,   SINGULAR, INT32,    conv_order,        3) \
X(a, STATIC,   SINGULAR, INT32,    rs_num_roots,      4)
#define FECCfg_CALLBACK NULL
#define FECCfg_DEFAULT NULL

#define RadioCfg_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, UENUM,    type,              1) \
X(a, STATIC,   SINGULAR, INT32,    frequency,         2) \
X(a, STATIC,   REPEATED, INT32,    frequencies,       3) \
X(a, STATIC,   SINGULAR, INT32,    tx_power,          4) \
X(a, STATIC,   OPTIONAL, MESSAGE,  lora_cfg,          5)
#define RadioCfg_CALLBACK NULL
#define RadioCfg_DEFAULT NULL
#define RadioCfg_lora_cfg_MSGTYPE LoraCfg

#define NetCfg_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, STRING,   beacon_msg,        1) \
X(a, STATIC,   SINGULAR, UINT32,   beacon_interval,   2) \
X(a, STATIC,   SINGULAR, UINT32,   num_offsets,       3) \
X(a, STATIC,   SINGULAR, UINT32,   pld_len,           4) \
X(a, STATIC,   SINGULAR, UINT32,   full_pkt_len,      5)
#define NetCfg_CALLBACK NULL
#define NetCfg_DEFAULT NULL

#define POCSAGCfg_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, BOOL,     enabled,           1) \
X(a, STATIC,   SINGULAR, UINT32,   frequency,         2) \
X(a, STATIC,   SINGULAR, UINT32,   beacon_interval,   3)
#define POCSAGCfg_CALLBACK NULL
#define POCSAGCfg_DEFAULT NULL

#define SysCfgMsg_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, UENUM,    mode,              1) \
X(a, STATIC,   SINGULAR, UINT32,   address,           2) \
X(a, STATIC,   OPTIONAL, MESSAGE,  radio_cfg,         3) \
X(a, STATIC,   OPTIONAL, MESSAGE,  test_cfg,          4) \
X(a, STATIC,   OPTIONAL, MESSAGE,  fec_cfg,           5) \
X(a, STATIC,   OPTIONAL, MESSAGE,  net_cfg,           6) \
X(a, STATIC,   OPTIONAL, MESSAGE,  pocsag_cfg,        7) \
X(a, STATIC,   SINGULAR, BOOL,     gps_en,            8) \
X(a, STATIC,   SINGULAR, BOOL,     log_packets_en,    9) \
X(a, STATIC,   SINGULAR, BOOL,     boot_log_en,      10)
#define SysCfgMsg_CALLBACK NULL
#define SysCfgMsg_DEFAULT NULL
#define SysCfgMsg_radio_cfg_MSGTYPE RadioCfg
#define SysCfgMsg_test_cfg_MSGTYPE TestCfg
#define SysCfgMsg_fec_cfg_MSGTYPE FECCfg
#define SysCfgMsg_net_cfg_MSGTYPE NetCfg
#define SysCfgMsg_pocsag_cfg_MSGTYPE POCSAGCfg

#define ClockSetMsg_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, UINT32,   time,              1)
#define ClockSetMsg_CALLBACK NULL
#define ClockSetMsg_DEFAULT NULL

#define StatusMsg_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, UENUM,    status,            1) \
X(a, STATIC,   SINGULAR, BOOL,     tx_full,           2) \
X(a, STATIC,   SINGULAR, UINT32,   time,              3)
#define StatusMsg_CALLBACK NULL
#define StatusMsg_DEFAULT NULL

#define DbgMsg_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, STRING,   msg,               1)
#define DbgMsg_CALLBACK NULL
#define DbgMsg_DEFAULT NULL

#define SerialCRCMsg_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, UINT32,   crc32,             1)
#define SerialCRCMsg_CALLBACK NULL
#define SerialCRCMsg_DEFAULT NULL

#define BootLogMsg_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, BOOL,     valid,             1) \
X(a, STATIC,   SINGULAR, UINT32,   boot_time,         2) \
X(a, STATIC,   SINGULAR, UINT32,   count,             3)
#define BootLogMsg_CALLBACK NULL
#define BootLogMsg_DEFAULT NULL

#define GPSMsg_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, BOOL,     valid,             1) \
X(a, STATIC,   SINGULAR, FLOAT,    lat,               2) \
X(a, STATIC,   SINGULAR, FLOAT,    lon,               3)
#define GPSMsg_CALLBACK NULL
#define GPSMsg_DEFAULT NULL

#define LogMsg_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, BOOL,     valid,             1) \
X(a, STATIC,   SINGULAR, UINT32,   count,             2) \
X(a, STATIC,   SINGULAR, UINT32,   timestamp,         3) \
X(a, STATIC,   SINGULAR, UINT32,   sender,            4) \
X(a, STATIC,   SINGULAR, UINT32,   ttl,               5) \
X(a, STATIC,   SINGULAR, UINT32,   stream_id,         6) \
X(a, STATIC,   SINGULAR, SINT32,   rssi,              7) \
X(a, STATIC,   SINGULAR, SINT32,   snr,               8) \
X(a, STATIC,   SINGULAR, INT32,    rx_size,           9) \
X(a, STATIC,   SINGULAR, UINT32,   comp_crc,         10) \
X(a, STATIC,   SINGULAR, UINT32,   crc,              11) \
X(a, STATIC,   SINGULAR, UINT32,   uptime,           12) \
X(a, STATIC,   OPTIONAL, MESSAGE,  gps_msg,          13)
#define LogMsg_CALLBACK NULL
#define LogMsg_DEFAULT NULL
#define LogMsg_gps_msg_MSGTYPE GPSMsg

#define TimeMsg_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, UINT32,   time,              1)
#define TimeMsg_CALLBACK NULL
#define TimeMsg_DEFAULT NULL

#define SerialMsg_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, UENUM,    type,              1) \
X(a, STATIC,   SINGULAR, BOOL,     retry,             2) \
X(a, STATIC,   OPTIONAL, MESSAGE,  sys_cfg,           3) \
X(a, STATIC,   OPTIONAL, MESSAGE,  clock_set,         4) \
X(a, STATIC,   OPTIONAL, MESSAGE,  status,            5) \
X(a, STATIC,   OPTIONAL, MESSAGE,  dbg_msg,           6) \
X(a, STATIC,   OPTIONAL, MESSAGE,  log_msg,           7) \
X(a, STATIC,   OPTIONAL, MESSAGE,  boot_log_msg,      8) \
X(a, STATIC,   OPTIONAL, MESSAGE,  data_msg,          9) \
X(a, STATIC,   OPTIONAL, MESSAGE,  error_msg,        10) \
X(a, STATIC,   OPTIONAL, MESSAGE,  time_msg,         11)
#define SerialMsg_CALLBACK NULL
#define SerialMsg_DEFAULT NULL
#define SerialMsg_sys_cfg_MSGTYPE SysCfgMsg
#define SerialMsg_clock_set_MSGTYPE ClockSetMsg
#define SerialMsg_status_MSGTYPE StatusMsg
#define SerialMsg_dbg_msg_MSGTYPE DbgMsg
#define SerialMsg_log_msg_MSGTYPE LogMsg
#define SerialMsg_boot_log_msg_MSGTYPE BootLogMsg
#define SerialMsg_data_msg_MSGTYPE DataMsg
#define SerialMsg_error_msg_MSGTYPE ErrorMsg
#define SerialMsg_time_msg_MSGTYPE TimeMsg

#define ErrorMsg_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, STRING,   msg,               1)
#define ErrorMsg_CALLBACK NULL
#define ErrorMsg_DEFAULT NULL

#define DataMsg_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, UENUM,    type,              1) \
X(a, STATIC,   SINGULAR, UINT32,   stream_id,         2) \
X(a, STATIC,   SINGULAR, UINT32,   ttl,               3) \
X(a, STATIC,   SINGULAR, UINT32,   sender,            4) \
X(a, STATIC,   SINGULAR, UINT32,   sym_offset,        5) \
X(a, STATIC,   SINGULAR, BYTES,    payload,           6) \
X(a, STATIC,   SINGULAR, UINT32,   crc,               7)
#define DataMsg_CALLBACK NULL
#define DataMsg_DEFAULT NULL

extern const pb_msgdesc_t LoraCfg_msg;
extern const pb_msgdesc_t TestCfg_msg;
extern const pb_msgdesc_t FECCfg_msg;
extern const pb_msgdesc_t RadioCfg_msg;
extern const pb_msgdesc_t NetCfg_msg;
extern const pb_msgdesc_t POCSAGCfg_msg;
extern const pb_msgdesc_t SysCfgMsg_msg;
extern const pb_msgdesc_t ClockSetMsg_msg;
extern const pb_msgdesc_t StatusMsg_msg;
extern const pb_msgdesc_t DbgMsg_msg;
extern const pb_msgdesc_t SerialCRCMsg_msg;
extern const pb_msgdesc_t BootLogMsg_msg;
extern const pb_msgdesc_t GPSMsg_msg;
extern const pb_msgdesc_t LogMsg_msg;
extern const pb_msgdesc_t TimeMsg_msg;
extern const pb_msgdesc_t SerialMsg_msg;
extern const pb_msgdesc_t ErrorMsg_msg;
extern const pb_msgdesc_t DataMsg_msg;

/* Defines for backwards compatibility with code written before nanopb-0.4.0 */
#define LoraCfg_fields &LoraCfg_msg
#define TestCfg_fields &TestCfg_msg
#define FECCfg_fields &FECCfg_msg
#define RadioCfg_fields &RadioCfg_msg
#define NetCfg_fields &NetCfg_msg
#define POCSAGCfg_fields &POCSAGCfg_msg
#define SysCfgMsg_fields &SysCfgMsg_msg
#define ClockSetMsg_fields &ClockSetMsg_msg
#define StatusMsg_fields &StatusMsg_msg
#define DbgMsg_fields &DbgMsg_msg
#define SerialCRCMsg_fields &SerialCRCMsg_msg
#define BootLogMsg_fields &BootLogMsg_msg
#define GPSMsg_fields &GPSMsg_msg
#define LogMsg_fields &LogMsg_msg
#define TimeMsg_fields &TimeMsg_msg
#define SerialMsg_fields &SerialMsg_msg
#define ErrorMsg_fields &ErrorMsg_msg
#define DataMsg_fields &DataMsg_msg

/* Maximum encoded size of messages (where known) */
#define LoraCfg_size                             30
#define TestCfg_size                             6
#define FECCfg_size                              35
#define RadioCfg_size                            232
#define NetCfg_size                              282
#define POCSAGCfg_size                           14
#define SysCfgMsg_size                           595
#define ClockSetMsg_size                         6
#define StatusMsg_size                           10
#define DbgMsg_size                              258
#define SerialCRCMsg_size                        6
#define BootLogMsg_size                          14
#define GPSMsg_size                              12
#define LogMsg_size                              87
#define TimeMsg_size                             6
#define SerialMsg_size                           1551
#define ErrorMsg_size                            258
#define DataMsg_size                             291

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
