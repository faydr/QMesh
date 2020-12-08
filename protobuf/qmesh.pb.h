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

typedef enum _StatusMsg_Status {
    StatusMsg_Status_BOOTING = 0,
    StatusMsg_Status_MANAGEMENT = 1,
    StatusMsg_Status_RUNNING = 2
} StatusMsg_Status;

typedef enum _SerialMsg_Type {
    SerialMsg_Type_CONFIG = 0,
    SerialMsg_Type_DATA = 1,
    SerialMsg_Type_CLOCK_SET = 2,
    SerialMsg_Type_STATUS = 3
} SerialMsg_Type;

/* Struct definitions */
typedef struct _ClockSetMsg {
    uint32_t time;
} ClockSetMsg;

typedef PB_BYTES_ARRAY_T(256) DataMsg_payload_t;
typedef struct _DataMsg {
    uint32_t type;
    uint32_t stream_id;
    uint32_t ttl;
    uint32_t sender;
    uint32_t sym_offset;
    DataMsg_payload_t payload;
} DataMsg;

typedef struct _FECCfg {
    int32_t conv_rate;
    int32_t conv_order;
    int32_t rs_num_roots;
} FECCfg;

typedef struct _LoraCfg {
    int32_t bw;
    int32_t cr;
    int32_t sf;
    int32_t preamble_length;
} LoraCfg;

typedef struct _StatusMsg {
    bool tx_full;
    uint32_t time;
} StatusMsg;

typedef struct _Testing {
    bool cw_test_mode;
    bool preamble_test_mode;
} Testing;

typedef struct _SysCfgMsg {
    int32_t frequency;
    pb_size_t frequencies_count;
    int32_t frequencies[16];
    bool has_lora_cfg;
    LoraCfg lora_cfg;
    bool has_testing;
    Testing testing;
    bool has_fec_cfg;
    FECCfg fec_cfg;
} SysCfgMsg;

typedef struct _SerialMsg {
    bool has_sys_cfg;
    SysCfgMsg sys_cfg;
    bool has_clock_set;
    ClockSetMsg clock_set;
    bool has_status;
    StatusMsg status;
} SerialMsg;


/* Helper constants for enums */
#define _FECCfg_Type_MIN FECCfg_Type_NONE
#define _FECCfg_Type_MAX FECCfg_Type_RSVGOLAY
#define _FECCfg_Type_ARRAYSIZE ((FECCfg_Type)(FECCfg_Type_RSVGOLAY+1))

#define _StatusMsg_Status_MIN StatusMsg_Status_BOOTING
#define _StatusMsg_Status_MAX StatusMsg_Status_RUNNING
#define _StatusMsg_Status_ARRAYSIZE ((StatusMsg_Status)(StatusMsg_Status_RUNNING+1))

#define _SerialMsg_Type_MIN SerialMsg_Type_CONFIG
#define _SerialMsg_Type_MAX SerialMsg_Type_STATUS
#define _SerialMsg_Type_ARRAYSIZE ((SerialMsg_Type)(SerialMsg_Type_STATUS+1))


/* Initializer values for message structs */
#define LoraCfg_init_default                     {0, 0, 0, 0}
#define Testing_init_default                     {0, 0}
#define FECCfg_init_default                      {0, 0, 0}
#define SysCfgMsg_init_default                   {0, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, false, LoraCfg_init_default, false, Testing_init_default, false, FECCfg_init_default}
#define ClockSetMsg_init_default                 {0}
#define StatusMsg_init_default                   {0, 0}
#define SerialMsg_init_default                   {false, SysCfgMsg_init_default, false, ClockSetMsg_init_default, false, StatusMsg_init_default}
#define DataMsg_init_default                     {0, 0, 0, 0, 0, {0, {0}}}
#define LoraCfg_init_zero                        {0, 0, 0, 0}
#define Testing_init_zero                        {0, 0}
#define FECCfg_init_zero                         {0, 0, 0}
#define SysCfgMsg_init_zero                      {0, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, false, LoraCfg_init_zero, false, Testing_init_zero, false, FECCfg_init_zero}
#define ClockSetMsg_init_zero                    {0}
#define StatusMsg_init_zero                      {0, 0}
#define SerialMsg_init_zero                      {false, SysCfgMsg_init_zero, false, ClockSetMsg_init_zero, false, StatusMsg_init_zero}
#define DataMsg_init_zero                        {0, 0, 0, 0, 0, {0, {0}}}

/* Field tags (for use in manual encoding/decoding) */
#define ClockSetMsg_time_tag                     1
#define DataMsg_type_tag                         1
#define DataMsg_stream_id_tag                    2
#define DataMsg_ttl_tag                          3
#define DataMsg_sender_tag                       4
#define DataMsg_sym_offset_tag                   5
#define DataMsg_payload_tag                      6
#define FECCfg_conv_rate_tag                     1
#define FECCfg_conv_order_tag                    2
#define FECCfg_rs_num_roots_tag                  3
#define LoraCfg_bw_tag                           1
#define LoraCfg_cr_tag                           2
#define LoraCfg_sf_tag                           3
#define LoraCfg_preamble_length_tag              4
#define StatusMsg_tx_full_tag                    1
#define StatusMsg_time_tag                       2
#define Testing_cw_test_mode_tag                 1
#define Testing_preamble_test_mode_tag           2
#define SysCfgMsg_frequency_tag                  1
#define SysCfgMsg_frequencies_tag                2
#define SysCfgMsg_lora_cfg_tag                   3
#define SysCfgMsg_testing_tag                    4
#define SysCfgMsg_fec_cfg_tag                    5
#define SerialMsg_sys_cfg_tag                    1
#define SerialMsg_clock_set_tag                  2
#define SerialMsg_status_tag                     3

/* Struct field encoding specification for nanopb */
#define LoraCfg_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, INT32,    bw,                1) \
X(a, STATIC,   SINGULAR, INT32,    cr,                2) \
X(a, STATIC,   SINGULAR, INT32,    sf,                3) \
X(a, STATIC,   SINGULAR, INT32,    preamble_length,   4)
#define LoraCfg_CALLBACK NULL
#define LoraCfg_DEFAULT NULL

#define Testing_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, BOOL,     cw_test_mode,      1) \
X(a, STATIC,   SINGULAR, BOOL,     preamble_test_mode,   2)
#define Testing_CALLBACK NULL
#define Testing_DEFAULT NULL

#define FECCfg_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, INT32,    conv_rate,         1) \
X(a, STATIC,   SINGULAR, INT32,    conv_order,        2) \
X(a, STATIC,   SINGULAR, INT32,    rs_num_roots,      3)
#define FECCfg_CALLBACK NULL
#define FECCfg_DEFAULT NULL

#define SysCfgMsg_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, INT32,    frequency,         1) \
X(a, STATIC,   REPEATED, INT32,    frequencies,       2) \
X(a, STATIC,   OPTIONAL, MESSAGE,  lora_cfg,          3) \
X(a, STATIC,   OPTIONAL, MESSAGE,  testing,           4) \
X(a, STATIC,   OPTIONAL, MESSAGE,  fec_cfg,           5)
#define SysCfgMsg_CALLBACK NULL
#define SysCfgMsg_DEFAULT NULL
#define SysCfgMsg_lora_cfg_MSGTYPE LoraCfg
#define SysCfgMsg_testing_MSGTYPE Testing
#define SysCfgMsg_fec_cfg_MSGTYPE FECCfg

#define ClockSetMsg_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, UINT32,   time,              1)
#define ClockSetMsg_CALLBACK NULL
#define ClockSetMsg_DEFAULT NULL

#define StatusMsg_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, BOOL,     tx_full,           1) \
X(a, STATIC,   SINGULAR, UINT32,   time,              2)
#define StatusMsg_CALLBACK NULL
#define StatusMsg_DEFAULT NULL

#define SerialMsg_FIELDLIST(X, a) \
X(a, STATIC,   OPTIONAL, MESSAGE,  sys_cfg,           1) \
X(a, STATIC,   OPTIONAL, MESSAGE,  clock_set,         2) \
X(a, STATIC,   OPTIONAL, MESSAGE,  status,            3)
#define SerialMsg_CALLBACK NULL
#define SerialMsg_DEFAULT NULL
#define SerialMsg_sys_cfg_MSGTYPE SysCfgMsg
#define SerialMsg_clock_set_MSGTYPE ClockSetMsg
#define SerialMsg_status_MSGTYPE StatusMsg

#define DataMsg_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, UINT32,   type,              1) \
X(a, STATIC,   SINGULAR, UINT32,   stream_id,         2) \
X(a, STATIC,   SINGULAR, UINT32,   ttl,               3) \
X(a, STATIC,   SINGULAR, UINT32,   sender,            4) \
X(a, STATIC,   SINGULAR, UINT32,   sym_offset,        5) \
X(a, STATIC,   SINGULAR, BYTES,    payload,           6)
#define DataMsg_CALLBACK NULL
#define DataMsg_DEFAULT NULL

extern const pb_msgdesc_t LoraCfg_msg;
extern const pb_msgdesc_t Testing_msg;
extern const pb_msgdesc_t FECCfg_msg;
extern const pb_msgdesc_t SysCfgMsg_msg;
extern const pb_msgdesc_t ClockSetMsg_msg;
extern const pb_msgdesc_t StatusMsg_msg;
extern const pb_msgdesc_t SerialMsg_msg;
extern const pb_msgdesc_t DataMsg_msg;

/* Defines for backwards compatibility with code written before nanopb-0.4.0 */
#define LoraCfg_fields &LoraCfg_msg
#define Testing_fields &Testing_msg
#define FECCfg_fields &FECCfg_msg
#define SysCfgMsg_fields &SysCfgMsg_msg
#define ClockSetMsg_fields &ClockSetMsg_msg
#define StatusMsg_fields &StatusMsg_msg
#define SerialMsg_fields &SerialMsg_msg
#define DataMsg_fields &DataMsg_msg

/* Maximum encoded size of messages (where known) */
#define LoraCfg_size                             44
#define Testing_size                             4
#define FECCfg_size                              33
#define SysCfgMsg_size                           274
#define ClockSetMsg_size                         6
#define StatusMsg_size                           8
#define SerialMsg_size                           295
#define DataMsg_size                             289

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
