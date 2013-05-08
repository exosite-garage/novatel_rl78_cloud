/*-------------------------------------------------------------------------*
* File:  novatel_cdma.h
*-------------------------------------------------------------------------*
* Description:
*      The GainSpan AT Command Library (AtCmdLib) provides the functions
*      that send AT commands to a GainSpan node and looks for a response.
*      Parse commands are provided to interpret the response data.
*-------------------------------------------------------------------------*/
#ifndef _MODEM_ATCMDLIB_H_
#define _MODEM_ATCMDLIB_H_

#include <stdint.h>
#include <stdbool.h>

/* Parsing related defines */
#define  ATMODEM_UDP_CLIENT_CID_OFFSET_BYTE        (8)  /* CID parameter offset in UDP connection response */
#define  ATMODEM_RAW_DATA_STRING_SIZE_MAX          (4)  /* Number of octets representing the data length field in raw data transfer message*/
#define  ATMODEM_HTTP_RESP_DATA_LEN_STRING_SIZE    (4)  /* Number of octets representing the data length field in HTTP data transfer message*/
#define  ATMODEM_GSLINK_DATA_LEN_STRING_SIZE       (4)  /* Number of octets representing the data lenght field in GSLink data transfer message */

#define  ATMODEM_INVALID_CID                    (0xFF) /* invalid CID */

#define ATMODEM_BSSID_MAX_LENGTH    20
#define ATMODEM_SSID_MAX_LENGTH     32
#define ATMODEM_MAC_MAX_LENGTH      18
#define ATMODEM_PASSWORD_MAX_LENGTH     32      // TODO: Is this correct?

#define NOVATEL_UART_BAUD_RATE  115200
typedef enum
{
    ATMODEM_RET_STATUS_ERROR = 0,
    ATMODEM_RET_STATUS_OK = 1,
    ATMODEM_RET_STATUS_VALID_DATA = 2,
    ATMODEM_RET_STATUS_CONTROL_CODE = 3,
    ATMODEM_RET_STATUS_MAX
} ATMODEM_RET_STATUS_E;

// AT RESPONSES
enum AT_RESPONSES
{
    AT_REPLY_NONE,
    AT_REPLY_OK,
    AT_REPLY_SGACT_NOT_SET,
    AT_REPLY_ERROR,
    AT_REPLY_TIMED_OUT
};

// FUNCTION PROTOS
int modem_SetParams(void);
int get_AT_CREG(void);;
int send_AT_PADS(void);
int AT_response_check(unsigned int checkTime, char *needle, char *termStr);
int ReadHTTPResponse(unsigned char isGet);

void readHTTPStatus(char *status, char bufsize);
void readHTTPDate(char *date, char bufsize);
void readHTTPBody(char *body, char bufsize);

typedef enum
{
    ATMODEM_DISABLE = 0, ATMODEM_ENABLE = 1
} ATMODEM_STATUS_E;

typedef enum
{
    ATMODEM_CON_TCP_SERVER,
    ATMODEM_CON_UDP_SERVER,
    ATMODEM_CON_UDP_CLIENT,
    ATMODEM_CON_TCP_CLIENT

} ATMODEM_CON_TYPE;

typedef enum
{
    ATMODEM_RX_STATE_START = 0,
    ATMODEM_RX_STATE_CMD_RESP,
    ATMODEM_RX_STATE_ESCAPE_START,
    ATMODEM_RX_STATE_DATA_HANDLE,
    ATMODEM_RX_STATE_BULK_DATA_HANDLE,
    ATMODEM_RX_STATE_HTTP_RESPONSE_DATA_HANDLE,
    ATMODEM_RX_STATE_RAW_DATA_HANDLE,

    ATMODEM_RX_STATE_DATA_CID,
    ATMODEM_RX_STATE_GLINK_DATA_LEN,
    ATMODEM_RX_STATE_GLINK_DATA_TYPE,
    ATMODEM_RX_STATE_GSLINK_DATA_HANDLE,

    ATMODEM_RX_STATE_MAX
} ATMODEM_RX_STATE_E;

typedef enum
{
    ATMODEM_MSG_ID_NONE = 0,
    ATMODEM_MSG_ID_OK = 1,
    ATMODEM_MSG_ID_INVALID_INPUT,
    ATMODEM_MSG_ID_ERROR,
    ATMODEM_MSG_ID_ERROR_IP_CONFIG_FAIL,
    ATMODEM_MSG_ID_ERROR_SOCKET_FAIL,
    ATMODEM_MSG_ID_DISCONNECT,
    ATMODEM_MSG_ID_DISASSOCIATION_EVENT,
    ATMODEM_MSG_ID_APP_RESET,
    ATMODEM_MSG_ID_OUT_OF_STBY_ALARM,
    ATMODEM_MSG_ID_OUT_OF_STBY_TIMER,
    ATMODEM_MSG_ID_UNEXPECTED_WARM_BOOT,
    ATMODEM_MSG_ID_OUT_OF_DEEP_SLEEP,
    ATMODEM_MSG_ID_WELCOME_MSG,
    ATMODEM_MSG_ID_STBY_CMD_ECHO,
    ATMODEM_MSG_ID_TCP_CON_DONE,
    ATMODEM_MSG_ID_RESPONSE_TIMEOUT,
    ATMODEM_MSG_ID_BULK_DATA_RX,
    ATMODEM_MSG_ID_DATA_RX,
    ATMODEM_MSG_ID_RAW_DATA_RX,
    ATMODEM_MSG_ID_ESC_CMD_OK,
    ATMODEM_MSG_ID_ESC_CMD_FAIL,
    ATMODEM_MSG_ID_HTTP_RESPONSE_DATA_RX,
    ATMODEM_MSG_ID_MAX,
    ATMODEM_MSG_ID_TCP_SERVER_CONNECT,
    ATMODEM_MSG_ID_GENERAL_MESSAGE
} ATMODEM_MSG_ID_E;

typedef enum
{
    ATMODEM_ID_INFO_OEM = 0,
    ATMODEM_ID_INFO_HARDWARE_VERSION = 1,
    ATMODEM_ID_INFO_SOFTWARE_VERSION = 2
} ATMODEM_ID_INFO_E;

typedef enum
{
    ATMODEM_PROFILE_ACTIVE = 0,
    ATMODEM_PROFILE_STORED_0 = 1,
    ATMODEM_PROFILE_STORED_1 = 2,
} ATMODEM_PROFILE_E;

typedef enum
{
    ATMODEM_REGDOMAIN_FCC = 0,
    ATMODEM_REGDOMAIN_ETSI = 1,
    ATMODEM_REGDOMAIN_TELEC = 2,
    ATMODEM_REGDOMAIN_UNKNOWN = -1
} ATMODEM_REGDOMAIN_E;

typedef enum
{
    ATMODEM_AUTHMODE_NONE_WPA = 0,
    ATMODEM_AUTHMODE_OPEN_WEP = 1,
    ATMODEM_AUTHMODE_SHARED_WEP = 2,
    ATMODEM_AUTHMODE_UNKNOWN = -1
} ATMODEM_AUTHMODE_E;

typedef enum
{
    ATMODEM_WEPKEY1 = 1,
    ATMODEM_WEPKEY2 = 2,
    ATMODEM_WEPKEY3 = 3,
    ATMODEM_WEPKEY4 = 4,
    ATMODEM_WEPKEY_UNKNOWN = -1
} ATMODEM_WEPKEY_E;

typedef enum
{
    ATMODEM_OAEAPFAST = 43,
    ATMODEM_OAEAPTLS = 13,
    ATMODEM_OAEAPTTLS = 21,
    ATMODEM_OAEAPPEAP = 25,
    ATMODEM_IAEAPMSCHAP = 26,
    ATMODEM_IAEAPGTC = 6,
    ATMODEM_AU_UNKNOWN = -1
} ATMODEM_INOUAUTH_E;

typedef enum
{
    ATMODEM_SMAUTO = 0,
    ATMODEM_SMOPEN = 1,
    ATMODEM_SMWEP = 2,
    ATMODEM_SMWPAPSK = 4,
    ATMODEM_SMWPA2PSK = 8,
    ATMODEM_SMWPAE = 16,
    ATMODEM_SMWPA2E = 32,
    ATMODEM_SM_UNKNOWN = -1
} ATMODEM_SECURITYMODE_E;

typedef enum
{
    ATMODEM_PROVSECU_OPEN = 1,
    ATMODEM_PROVSECU_WEP = 2,
    ATMODEM_PROVSECU_WPA_PER = 3,
    ATMODEM_PROVSECU_WPA_ENT = 4
} ATMODEM_PROVSECURITY_E;

typedef enum
{
    ATMODEM_HTTP_HE_AUTH = 2,
    ATMODEM_HTTP_HE_CONN = 3,
    ATMODEM_HTTP_HE_CON_ENC = 4,
    ATMODEM_HTTP_HE_CON_LENGTH = 5,
    ATMODEM_HTTP_HE_CON_RANGE = 6,
    ATMODEM_HTTP_HE_CON_TYPE = 7,
    ATMODEM_HTTP_HE_DATE = 8,
    ATMODEM_HTTP_HE_EXPIRES = 9,
    ATMODEM_HTTP_HE_FROM = 10,
    ATMODEM_HTTP_HE_HOST = 11,
    ATMODEM_HTTP_HE_IF_MODIF_SIN = 12,
    ATMODEM_HTTP_HE_LAST_MODIF = 13,
    ATMODEM_HTTP_HE_LOCATION = 14,
    ATMODEM_HTTP_HE_PRAGMA = 15,
    ATMODEM_HTTP_HE_RANGE = 16,
    ATMODEM_HTTP_HE_REF = 17,
    ATMODEM_HTTP_HE_SERVER = 18,
    ATMODEM_HTTP_HE_TRANS_ENC = 19,
    ATMODEM_HTTP_HE_USER_A = 20,
    ATMODEM_HTTP_HE_WWW_AUTH = 21,
    ATMODEM_HTTP_REQ_URL = 23,
    ATMODEM_HTTP_UNKNOWN = -1
} ATMODEM_HTTPCLIENT_E;

typedef enum
{
    ATMODEM_RAW_ETH_DIS = 0,
    ATMODEM_RAW_ETH_NONSNAP = 1,
    ATMODEM_RAW_ETH_ENABLEALL = 2,
    ATMODEM_RAW_ETH_UNKNOWN = -1
} ATMODEM_RAW_ETH_E;

typedef enum
{
    ATMODEM_ASPWMD_ACTIVE_ON = 0,
    ATMODEM_ASPWMD_MODE_1 = 1,
    ATMODEM_ASPWMD_MODE_2 = 2,
    ATMODEM_ASPWMD_MODE_3 = 3,
    ATMODEM_ASPWMD_UNKNOWN = -1
} ATMODEM_ASSOCPOWMODE_E;

typedef enum
{
    ATMODEM_SPI_PHASE = 0, ATMODEM_SPI_PHASE_180 = 1,
} ATMODEM_SPI_PHASE_E;

typedef enum
{
    ATMODEM_SPI_POLARITY_LOW = 0, ATMODEM_SPI_POLARITY_HIGH = 1,
} ATMODEM_SPI_POLARITY_E;

typedef enum
{
    ATMODEM_APP0_BIN = 1,
    ATMODEM_APP1_BIN = 2,
    ATMODEM_APP01_BINS = 3,
    ATMODEM_WLAN_BIN = 4,
    ATMODEM_ALL_BINS = 7,
    ATMODEM_FWBIN_UNKNOWN = -1
} ATMODEM_FIRMWARE_BINARIES_E;

typedef enum
{
    ATMODEM_GPIO10 = 10,
    ATMODEM_GPIO11 = 11,
    ATMODEM_GPIO30 = 30,
    ATMODEM_GPIO31 = 31,
} ATMODEM_GPIO_PIN_E;

typedef enum
{
    ATMODEM_LOW = 0, ATMODEM_HIGH = 1,
} ATMODEM_GPIO_STATE_E;

typedef enum
{
    ATMODEM_FWUPPARAM_SERVERIP = 0,
    ATMODEM_FWUPPARAM_SERVERPORT = 1,
    ATMODEM_FWUPPARAM_PROXYPRESENT = 2,
    ATMODEM_FWUPPARAM_PROXYIP = 3,
    ATMODEM_FWUPPARAM_PROXYPORT = 4,
    ATMODEM_FWUPPARAM_SSLENABLE = 5,
    ATMODEM_FWUPPARAM_CACERT = 6,
    ATMODEM_FWUPPARAM_WLAN = 7,
    ATMODEM_FWUPPARAM_APP0 = 8,
    ATMODEM_FWUPPARAM_APP1 = 9,
    ATMODEM_FWUPPARAM_UNKNOWN = -1
} ATMODEM_FWUPGRADEPARAM_E;

typedef enum
{
    ATMODEM_EAPTLS_CAC = 0,
    ATMODEM_EAPTLS_CLIC = 1,
    ATMODEM_EAPTLS_PRK = 2,
    ATMODEM_EAPTLS_BIN = 0,
    ATMODEM_EAPTLS_HEX = 1,
    ATMODEM_EAPTLS_FLASH = 0,
    ATMODEM_EAPTLS_RAM = 1,
    ATMODEM_EAPTLS_UNKNOWN = -1
} ATMODEM_EAPTLS_E;

typedef enum
{
    ATMODEM_SOCKET_OPTION_TYPE_SOCK = 65535,
    ATMODEM_SOCKET_OPTION_TYPE_IP = 0,
    ATMODEM_SOCKET_OPTION_TYPE_TCP = 6,
    ATMODEM_SOCKET_OPTION_TYPE_UNKNOWN = -1
} ATMODEM_SOCKET_OPTION_TYPE_E;

typedef enum
{
    ATMODEM_SOCKET_OPTION_PARAM_TCP_MAXRT = 0x10,
    ATMODEM_SOCKET_OPTION_PARAM_TCP_KEEPALIVE = 0x4001,
    ATMODEM_SOCKET_OPTION_PARAM_SO_KEEPALIVE = 0x8,
    ATMODEM_SOCKET_OPTION_PARAM_TCP_KEEPALIVE_COUNT = 0x4005,
    ATMODEM_SOCKET_OPTION_PARAM_UNKNOWN = -1
} ATMODEM_SOCKET_OPTION_PARAM_E;

typedef enum
{
    ATMODEM_HTTPSEND_GET = 1,
    ATMODEM_HTTPSEND_POST = 3,
    ATMODEM_HTTPSEND_UNKNOWN = -1
} ATMODEM_HTTPSEND_E;

typedef enum
{
    ATMODEM_TRAUTO = 0,
    ATMODEM_TR1MBPS = 2,
    ATMODEM_TR2MBPS = 4,
    ATMODEM_TR5p5MBPS = 11,
    ATMODEM_TR11MBPS = 22,
    ATMODEM_TR_UNKNOWN = -1
} ATMODEM_TRANSRATE_E;

typedef enum
{
    ATMODEM_UNSOLICITEDTX_RT1 = 130,
    ATMODEM_UNSOLICITEDTX_RT2 = 132,
    ATMODEM_UNSOLICITEDTX_RT55 = 139,
    ATMODEM_UNSOLICITEDTX_RT11 = 150,
    ATMODEM_UNSOLICITEDTX_UNKNOWN = -1
} ATMODEM_UNSOLICITEDTX_E;

typedef enum
{
    ATMODEM_RFWAVETXSTART_RATE1 = 0,
    ATMODEM_RFWAVETXSTART_RATE2 = 1,
    ATMODEM_RFWAVETXSTART_RATE55 = 2,
    ATMODEM_RFWAVETXSTART_RATE11 = 3,
    ATMODEM_RFWAVETXSTART_UNKNOWN = -1
} ATMODEM_RFWAVETXSTART_E;

typedef enum
{
    ATMODEM_RFFRAMETXSTART_RATE1 = 2,
    ATMODEM_RFFRAMETXSTART_RATE2 = 4,
    ATMODEM_RFFRAMETXSTART_RATE55 = 11,
    ATMODEM_RFFRAMETXSTART_RATE11 = 22,
    ATMODEM_RFFRAMETXSTART_UNKNOWN = -1
} ATMODEM_RFFRAMETXSTART_E;

typedef enum
{
    ATMODEM_STATIONMODE_INFRASTRUCTURE = 0,
    ATMODEM_STATIONMODE_AD_HOC = 1,
    ATMODEM_STATIONMODE_LIMITED_AP = 2,
    ATMODEM_STATIONMODE_UNKNOWN = -1
} ATMODEM_STATIONMODE_E;

typedef uint8_t ATMODEM_IPv4[4];

typedef union
{
    ATMODEM_IPv4 ipv4;
//  ATMODEM_IPv6 ipv6; // placeholder
} ATMODEM_IP;

typedef struct
{
    char bssid[ATMODEM_BSSID_MAX_LENGTH + 1];
    char ssid[ATMODEM_SSID_MAX_LENGTH + 1];
    uint8_t channel;
    ATMODEM_STATIONMODE_E station;
    int8_t signal;
    ATMODEM_SECURITYMODE_E security;
} ATMODEM_NetworkScanEntry;

typedef struct
{
    char mac[ATMODEM_MAC_MAX_LENGTH + 1];
    uint8_t connected;
    ATMODEM_STATIONMODE_E mode;
    char bssid[ATMODEM_BSSID_MAX_LENGTH + 1];
    char ssid[ATMODEM_SSID_MAX_LENGTH + 1];
    uint8_t channel;
    ATMODEM_SECURITYMODE_E security;
    char signal;
    ATMODEM_IP addr;
    ATMODEM_IP subnet;
    ATMODEM_IP gateway;
    ATMODEM_IP dns1;
    ATMODEM_IP dns2;
    uint32_t rxCount;
    uint32_t txCount;
} ATMODEM_NetworkStatus;

typedef struct
{
    char ssid[ATMODEM_SSID_MAX_LENGTH + 1];
    uint8_t channel;
    uint8_t conn_type;
    char password[ATMODEM_PASSWORD_MAX_LENGTH + 1];
    ATMODEM_STATIONMODE_E station;
    ATMODEM_PROVSECURITY_E security;
    uint8_t dhcp_enable;
    uint8_t auto_dns_enable;
    ATMODEM_IPv4 ip;
    ATMODEM_IPv4 subnet;
    ATMODEM_IPv4 gateway;
    ATMODEM_IPv4 dns1;
    ATMODEM_IPv4 dns2;
    uint8_t tcpIPClientHostIP;
} ATMODEM_WEB_PROV_SETTINGS;

#define  ATMODEM_CR_CHAR          0x0D     /* octet value in hex representing Carriage return    */
#define  ATMODEM_LF_CHAR          0x0A     /* octet value in hex representing Line feed             */
#define  ATMODEM_ESC_CHAR         0x1B     /* octet value in hex representing application level ESCAPE sequence */

/* Special characters used for data mode handling */
#define  ATMODEM_DATA_MODE_NORMAL_START_CHAR_S      'S'
#define  ATMODEM_DATA_MODE_NORMAL_END_CHAR_E        'E'
#define  ATMODEM_DATA_MODE_BULK_START_CHAR_Z        'Z'
#define  ATMODEM_DATA_MODE_BULK_START_CHAR_H        'H'
#define  ATMODEM_DATA_MODE_RAW_INDICATION_CHAR_COL  ':'
#define  ATMODEM_DATA_MODE_RAW_INDICATION_CHAR_R    'R'
#define  ATMODEM_DATA_MODE_ESC_OK_CHAR_O            'O'
#define  ATMODEM_DATA_MODE_ESC_FAIL_CHAR_F          'F'
#define  ATMODEM_DATA_MODE_UDP_START_CHAR_u         'u'

#define ATMODEM_DATA_MODE_GSLINK_START_CHAR_K       'K'
#define ATMODEM_DATA_MODE_GSLINK_START_CHAR_G       'G'

#define  GSLINK_GET_XML      1
#define  GSLINK_GET_RAW      2
#define  GSLINK_POST_XML     3
#define  GSLINK_SET_RAW      4
#define  GSLINK_GET_RESP     6
#define  GSLINK_POST_RESP    7

#define  ESC_G_VALUE_INIT    0
#define  ESC_G_VALUE_ENABLE  1
#define  ESC_G_VALUE_END     2

/************* Following control octets are  used by SPI driver layer *****************************/
#define  ATMODEM_SPI_ESC_CHAR               (0xFB)    /* Start transmission indication */
#define  ATMODEM_SPI_IDLE_CHAR              (0xF5)    /* synchronous IDLE */
#define  ATMODEM_SPI_XOFF_CHAR              (0xFA)    /* Stop transmission indication */
#define  ATMODEM_SPI_XON_CHAR               (0xFD)    /* Start transmission indication */
#define  ATMODEM_SPI_INVALID_CHAR_ALL_ONE   (0xFF)    /* Invalid character possibly recieved during reboot */
#define  ATMODEM_SPI_INVALID_CHAR_ALL_ZERO  (0x00)    /* Invalid character possibly recieved during reboot */
#define  ATMODEM_SPI_READY_CHECK            (0xF3)    /* SPI link ready check */
#define  ATMODEM_SPI_READY_ACK              (0xF3)    /* SPI link ready response */
#define  ATMODEM_SPI_CTRL_BYTE_MASK         (0x80)    /* Control byte mask */

/************* End of  SPI driver layer  specific defines*****************************/
#define ATMODEM_RX_MAX_LINES                20

/* this should be tied to a timer, faster processor larger the number */
//#define  ATMODEM_RESPONSE_TIMEOUT_COUNT    (0xFFFFFFFF) /* Time out value for reponse message */
#define  ATMODEM_RESPONSE_TIMEOUT_COUNT    (0x2FFFFFF) // OK for SPI
/*************<Function prototye forward delarations >***************/

ATMODEM_MSG_ID_E AtModem_Check(void);
ATMODEM_MSG_ID_E AtModem_SetEcho(uint8_t mode);
ATMODEM_MSG_ID_E AtModem_SetMAC(char *pAddr);
ATMODEM_MSG_ID_E AtModem_SetMAC2(char *pAddr);
ATMODEM_MSG_ID_E AtModem_GetMAC(char *mac);
ATMODEM_MSG_ID_E AtModem_GetMAC2(char *mac);
ATMODEM_MSG_ID_E AtModem_CalcNStorePSK(char *pSsid, char *pPsk);
//ATMODEM_MSG_ID_E AtModem_WlanConnStat( ATMODEM_N);
ATMODEM_MSG_ID_E AtModem_GetNetworkStatus(ATMODEM_NetworkStatus *pStatus);
ATMODEM_MSG_ID_E AtModem_DHCPSet(uint8_t mode);
ATMODEM_MSG_ID_E AtModem_Assoc(char *pSsid, char *pBssid, uint8_t channel);
ATMODEM_MSG_ID_E AtModem_TCPClientStart(
    char *pRemoteTcpSrvIp,
    int16_t pRemoteTcpSrvPort,
    uint8_t *cid);
ATMODEM_MSG_ID_E AtModem_UDPClientStart(
    char *pRemoteUdpSrvIp,
    char *pRemoteUdpSrvPort,
    char *pUdpLocalPort);

ATMODEM_MSG_ID_E AtModem_CloseAll(void);

ATMODEM_MSG_ID_E AtModem_BatteryCheckStart(uint16_t interval);
ATMODEM_MSG_ID_E AtModem_GotoSTNDBy(
    char *nsec,
    uint32_t dealy,
    uint8_t alarm1_Pol,
    uint8_t alarm2_Pol);
ATMODEM_MSG_ID_E AtModem_PSK(char *pPsk);
ATMODEM_MSG_ID_E AtModem_EnableDeepSleep(void);
ATMODEM_MSG_ID_E AtModem_StoreNwConn(void);
ATMODEM_MSG_ID_E AtModem_ReStoreNwConn(void);
ATMODEM_MSG_ID_E AtModem_IPSet(char *pIpAddr, char *pSubnet, char *pGateway);

ATMODEM_MSG_ID_E AtModem_SaveProfile(uint8_t profile);
ATMODEM_MSG_ID_E AtModem_LoadProfile(uint8_t profile);
ATMODEM_MSG_ID_E AtModem_ResetFactoryDefaults(void);
ATMODEM_MSG_ID_E AtModem_GetRssi(void);
ATMODEM_MSG_ID_E AtModem_DisAssoc(void);
ATMODEM_MSG_ID_E AtModem_FWUpgrade(
    char *pSrvip,
    uint16_t srvport,
    uint16_t srcPort,
    char *pSrcIP);
ATMODEM_MSG_ID_E AtModem_BatteryCheckStop(void);
ATMODEM_MSG_ID_E AtModem_MultiCast(uint8_t mode);
ATMODEM_MSG_ID_E AtModem_Version(void);
ATMODEM_MSG_ID_E AtModem_Mode(ATMODEM_STATIONMODE_E mode);
ATMODEM_MSG_ID_E AtModem_UDPServer_Start(uint16_t pUdpSrvPort, uint8_t *cid);
ATMODEM_MSG_ID_E AtModem_TCPServer_Start(uint16_t pTcpSrvPort, uint8_t *cid);
ATMODEM_MSG_ID_E AtModem_DNSLookup(char* pUrl);
ATMODEM_MSG_ID_E AtModem_Close(uint8_t cid);
ATMODEM_MSG_ID_E AtModem_SetWRetryCount(uint16_t count);
ATMODEM_MSG_ID_E AtModem_GetErrCount(uint32_t *errorCount);
ATMODEM_MSG_ID_E AtModem_EnableRadio(uint8_t mode);
ATMODEM_MSG_ID_E AtModem_EnablePwSave(uint8_t mode);
ATMODEM_MSG_ID_E AtModem_SetTime(char* pDate, char *pTime);
ATMODEM_MSG_ID_E AtModem_EnableExternalPA(uint8_t mode);
ATMODEM_MSG_ID_E AtModem_SyncLossInterval(uint16_t interval);
ATMODEM_MSG_ID_E AtModem_PSPollInterval(uint16_t interval);
ATMODEM_MSG_ID_E AtModem_SetTxPower(uint8_t power);
ATMODEM_MSG_ID_E AtModem_SetDNSServerIP(char *pDNS1, char *pDNS2);
ATMODEM_MSG_ID_E AtModem_EnableAutoConnect(uint8_t mode);
void AtModem_SwitchFromAutoToCmd(void);
ATMODEM_MSG_ID_E AtModem_StoreWAutoConn(char * pSsid, uint8_t channel);
ATMODEM_MSG_ID_E AtModem_StoreNAutoConn(char* pIpAddr, int16_t pRmtPort);
ATMODEM_MSG_ID_E AtModem_StoreATS(uint8_t param, uint8_t value);
ATMODEM_MSG_ID_E AtModem_BData(uint8_t mode);
typedef struct
{
    char ssid[ATMODEM_SSID_MAX_LENGTH + 1];
    char password[ATMODEM_PASSWORD_MAX_LENGTH + 1];
    uint16_t channel;
} AtModem_WPSResult;

ATMODEM_MSG_ID_E AtModem_StartWPSPUSH(AtModem_WPSResult *result);
ATMODEM_MSG_ID_E AtModem_StartWPSPIN(char* pin, AtModem_WPSResult *result);
ATMODEM_MSG_ID_E AtModem_GetInfo(
    ATMODEM_ID_INFO_E info,
    char* infotext,
    int16_t maxchars);
ATMODEM_MSG_ID_E AtModem_GetScanTimes(uint16_t *min, uint16_t *max);
ATMODEM_MSG_ID_E AtModem_SetScanTimes(uint16_t min, uint16_t max);
ATMODEM_MSG_ID_E AtModem_SetDefaultProfile(uint8_t profile);
ATMODEM_MSG_ID_E AtModem_GetProfile(
    ATMODEM_PROFILE_E profile,
    char *text,
    uint16_t maxChars);
ATMODEM_MSG_ID_E AtModem_GetRegulatoryDomain(ATMODEM_REGDOMAIN_E *regDomain);
ATMODEM_MSG_ID_E AtModem_SetRegulatoryDomain(ATMODEM_REGDOMAIN_E regDomain);
ATMODEM_MSG_ID_E AtModem_NetworkScan(
    const char *SSID,
    uint8_t channel,
    uint16_t scantime,
    ATMODEM_NetworkScanEntry *entries,
    uint8_t maxEntries,
    uint8_t *numEntries);

typedef struct
{
    // IP address string of unit on other side
    char ip[17];

    // Local port id
    uint16_t port;

    // Connection id of the current
    uint8_t cid;

    // Pointer to buffer to receive message
    uint8_t *message;

    // Returned length of current message
    uint16_t numBytes;
} ATMODEM_UDPMessage;

typedef struct
{
    // Connection id of the current
    uint8_t cid;

    // Pointer to buffer to receive message
    const uint8_t *message;

    // Returned length of current message
    uint16_t numBytes;
} ATMODEM_TCPMessage;

typedef struct
{
    // Which server created this connection?
    uint8_t server_cid;

    // Newly created cid for communication
    uint8_t cid;

    // IP number of incoming connection
    char ip[16];

    // Connection port (local allocated port number)
    uint16_t port;
} ATMODEM_TCPConnection;

ATMODEM_MSG_ID_E AtModem_WaitForTCPMessage(uint32_t timeout);
void AtModem_ParseTCPData(
    const uint8_t *received,
    uint16_t length,
    ATMODEM_TCPMessage *rxm);
ATMODEM_MSG_ID_E AtModem_WaitForTCPConnection(
    ATMODEM_TCPConnection *connection,
    uint32_t timeout);

typedef struct
{
    // Connection id of the current
    uint8_t cid;

    // Pointer to buffer to receive message
    const uint8_t *message;

    // Returned length of current message
    uint16_t numBytes;
} ATMODEM_HTTPMessage;

ATMODEM_MSG_ID_E AtModem_WaitForHTTPMessage(uint32_t timeout);
void AtModem_ParseHTTPData(
    const uint8_t *received,
    uint8_t length,
    ATMODEM_HTTPMessage *rxm);

ATMODEM_MSG_ID_E AtModem_WaitForUDPMessage(uint32_t timeout);
void AtModem_ParseUDPData(
    const uint8_t *received,
    uint8_t length,
    ATMODEM_UDPMessage *msg);

// TODO: Put all functions in same order/grouping as in command reference list
ATMODEM_MSG_ID_E AtModem_DisAssoc2(void);
ATMODEM_MSG_ID_E AtModem_GetTransmitRate(void);
ATMODEM_MSG_ID_E AtModem_GetClientInfo(void);
ATMODEM_MSG_ID_E AtModem_SetAuthentictionMode(ATMODEM_AUTHMODE_E authmode);
ATMODEM_MSG_ID_E AtModem_SetWEPKey(ATMODEM_WEPKEY_E keynum, char key[]);
ATMODEM_MSG_ID_E AtModem_SetWPAKey(char key[]);
ATMODEM_MSG_ID_E
AtModem_SetListenBeaconInterval(bool enable, uint16_t interval);
ATMODEM_MSG_ID_E AtModem_SetTransmitRate(ATMODEM_TRANSRATE_E transrate);
ATMODEM_MSG_ID_E AtModem_EnableDHCPServer(void);
ATMODEM_MSG_ID_E AtModem_DisableDHCPServer(void);
ATMODEM_MSG_ID_E AtModem_EnableDNSServer(char server[]);
ATMODEM_MSG_ID_E AtModem_DisableDNSServer(void);
ATMODEM_MSG_ID_E AtModem_SetEAPConfiguration(
    ATMODEM_INOUAUTH_E oa,
    ATMODEM_INOUAUTH_E ia,
    char user[],
    char pass[]);
ATMODEM_MSG_ID_E AtModem_AddSSLCertificate(
    char name[],
    bool hex,
    uint16_t size,
    bool ram);
ATMODEM_MSG_ID_E AtModem_DeleteSSLCertificate(char name[]);
ATMODEM_MSG_ID_E AtModem_SetSecurity(ATMODEM_SECURITYMODE_E security);
ATMODEM_MSG_ID_E AtModem_SSLOpen(uint8_t cid, char name[]);
ATMODEM_MSG_ID_E AtModem_SSLClose(uint8_t cid);
ATMODEM_MSG_ID_E AtModem_HTTPConf(ATMODEM_HTTPCLIENT_E param, char value[]);
ATMODEM_MSG_ID_E AtModem_HTTPConfDel(ATMODEM_HTTPCLIENT_E param);
ATMODEM_MSG_ID_E AtModem_HTTPClose(uint8_t cid);
ATMODEM_MSG_ID_E AtModem_RawETHFrameConf(ATMODEM_RAW_ETH_E enable);
ATMODEM_MSG_ID_E AtModem_BulkDataTrans(bool enable);
ATMODEM_MSG_ID_E AtModem_SetBatteryCheckFreq(uint16_t freq);
ATMODEM_MSG_ID_E AtModem_GetBatteryCheckFreq(uint16_t *freq);
ATMODEM_MSG_ID_E AtModem_GetBatteryValue(void);
ATMODEM_MSG_ID_E AtModem_EnterDeepSleep(uint32_t milliseconds);
ATMODEM_MSG_ID_E AtModem_AssocPowerSaveMode(ATMODEM_ASSOCPOWMODE_E mode);
ATMODEM_MSG_ID_E AtModem_SetTransmitPower(uint8_t power);
ATMODEM_MSG_ID_E AtModem_StartAutoConnect(void);
ATMODEM_MSG_ID_E AtModem_ConfigAPAutoConnect(void);
ATMODEM_MSG_ID_E AtModem_ReturnAutoConnect(void);
ATMODEM_MSG_ID_E AtModem_WebProv(char username[], char password[]);
ATMODEM_MSG_ID_E AtModem_WebLogoAdd(uint16_t size);
ATMODEM_MSG_ID_E AtModem_RFAsyncFrameRX(uint16_t channel);
ATMODEM_MSG_ID_E AtModem_RFStop(void);
ATMODEM_MSG_ID_E AtModem_SetPassPhrase(char *pPhrase);
ATMODEM_MSG_ID_E AtModem_SPIConf(
    ATMODEM_SPI_POLARITY_E polarity,
    ATMODEM_SPI_PHASE_E phase);
ATMODEM_MSG_ID_E AtModem_Reset(void);
ATMODEM_MSG_ID_E AtModem_MemoryTrace(void);
ATMODEM_MSG_ID_E AtModem_StartFirmwareUpdate(ATMODEM_FIRMWARE_BINARIES_E bins);
ATMODEM_MSG_ID_E AtModem_GetTime(void);
ATMODEM_MSG_ID_E AtModem_SetGPIO(
    ATMODEM_GPIO_PIN_E GPIO,
    ATMODEM_GPIO_STATE_E state);
ATMODEM_MSG_ID_E AtModem_EnableEnhancedAsync(bool on);
ATMODEM_MSG_ID_E AtModem_EnableVerbose(bool on);
ATMODEM_MSG_ID_E AtModem_GetNetworkConfiguration(void);
ATMODEM_MSG_ID_E AtModem_FWUpgradeConfig(
    ATMODEM_FWUPGRADEPARAM_E param,
    char value[]);
ATMODEM_MSG_ID_E AtModem_GetCIDInfo(void);
ATMODEM_MSG_ID_E AtModem_BatterySetWarnLevel(
    uint16_t level,
    uint16_t freq,
    uint16_t standby);
ATMODEM_MSG_ID_E AtModem_SetEAPCertificate(
    ATMODEM_EAPTLS_E cert,
    ATMODEM_EAPTLS_E binhex,
    uint16_t size,
    ATMODEM_EAPTLS_E flashram);
ATMODEM_MSG_ID_E AtModem_Ping(
    char ip[],
    uint16_t trails,
    uint16_t timeout,
    uint16_t len,
    uint8_t tos,
    uint8_t ttl,
    char payload[]);
ATMODEM_MSG_ID_E AtModem_TraceRoute(
    char ip[],
    uint16_t interval,
    uint8_t maxhops,
    uint8_t minhops,
    uint8_t tos);
ATMODEM_MSG_ID_E AtModem_SNTPsync(
    bool enable,
    char ip[],
    uint8_t timeout,
    bool periodic,
    uint32_t frequency);
ATMODEM_MSG_ID_E AtModem_SetSocketOptions(
    uint8_t cid,
    ATMODEM_SOCKET_OPTION_TYPE_E type,
    ATMODEM_SOCKET_OPTION_PARAM_E param,
    uint32_t value);
ATMODEM_MSG_ID_E AtModem_HTTPOpen(
    char host[],
    uint16_t port,
    bool sslenabled,
    char certname[],
    char proxy[],
    uint16_t timeout,
    uint8_t *cid);
ATMODEM_MSG_ID_E AtModem_HTTPSend(
    uint8_t cid,
    ATMODEM_HTTPSEND_E type,
    uint16_t timeout,
    char page[],
    uint16_t size,
    const void *txBuf);
ATMODEM_MSG_ID_E AtModem_UnsolicitedTXRate(
    uint16_t frame,
    uint16_t seq,
    uint8_t chan,
    ATMODEM_UNSOLICITEDTX_E rate,
    uint32_t wmm,
    char mac[],
    char bssid[],
    uint16_t len);
ATMODEM_MSG_ID_E AtModem_RFAsyncFrameTX(
    uint8_t channel,
    uint8_t power,
    ATMODEM_RFFRAMETXSTART_E rate,
    uint16_t times,
    uint16_t interval,
    uint8_t frameControl,
    uint16_t durationId,
    uint16_t sequenceControl,
    uint16_t frameLen,
    bool shortPreamble,
    bool scramblerOff,
    char dstMac[],
    char srcMac[]);
ATMODEM_MSG_ID_E AtModem_RFWaveTXStart(
    bool isModulated,
    uint16_t channel,
    ATMODEM_RFWAVETXSTART_E rate,
    bool longPreamble,
    bool scramblerOn,
    bool continousWaveRX,
    uint8_t power,
    char ssid[]);

uint8_t AtModem_ParseUDPClientCid(void);
uint8_t AtModem_ParseWlanConnStat(void);
uint8_t AtModem_ParseNodeIPv4Address(ATMODEM_IPv4 *ip);
void AtModem_ParseIPv4Address(const char *line, ATMODEM_IPv4 *ip);
void AtModem_IPv4AddressToString(ATMODEM_IPv4 *ip, char *string);
uint8_t AtModem_ParseRssiResponse(int16_t *pRssi);
uint8_t AtModem_ParseUDPServerStartResponse(uint8_t *pConnId);
uint8_t AtModem_ParseTCPServerStartResponse(uint8_t *pConnId);
uint8_t AtModem_ParseDNSLookupResponse(char *ipAddr);
uint16_t AtModem_ParseIntoLines(char *text, char *pLines[], uint16_t maxLines);
uint8_t AtModem_ParseIntoTokens(
    char *line,
    char deliminator,
    char *tokens[],
    uint8_t maxTokens);
ATMODEM_SECURITYMODE_E AtModem_ParseSecurityMode(const char *string);
void AtModem_ParseIPAddress(const char *string, ATMODEM_IP *ip);

ATMODEM_MSG_ID_E AtModem_CommandSend(void);
ATMODEM_MSG_ID_E AtModem_CommandSendString(char *aString);
void AtModem_DataSend(const void *pTxData, uint16_t dataLen);
ATMODEM_MSG_ID_E AtModem_SendTCPData(
    uint8_t cid,
    const void *txBuf,
    uint16_t dataLen);
ATMODEM_MSG_ID_E AtModem_SendUDPData(
    uint8_t cid,
    const void *txBuf,
    uint16_t dataLen,
    ATMODEM_CON_TYPE conType,
    const char *pUdpClientIP,
    uint16_t udpClientPort);

void AtModem_BulkDataTransfer(uint8_t cid, const void *pData, uint16_t dataLen);
ATMODEM_MSG_ID_E AtModem_checkEOFMessage(const char *pBuffer);
ATMODEM_MSG_ID_E AtModem_ReceiveDataHandle(uint32_t timeout);
ATMODEM_MSG_ID_E AtModem_ReceiveDataProcess(uint8_t rxData);
ATMODEM_MSG_ID_E AtModem_ResponseHandle(void);
ATMODEM_MSG_ID_E AtModem_ProcessRxChunk(const void *rxBuf, uint16_t bufLen);
void AtModem_LinkCheck(void);
void AtModem_FlushIncomingMessage(void);
uint8_t AtModem_IsNodeResetDetected(void);
void AtModem_SetNodeResetFlag(void);
void AtModem_ClearNodeResetFlag(void);
uint8_t AtModem_IsNodeAssociated(void);
void AtModem_SetNodeAssociationFlag(void);
void AtModem_ClearNodeAssociationFlag(void);

void AtModem_FlushRxBuffer(void);
void AtModem_ConvertNumberTo4DigitASCII(uint16_t myNum, char *pStr);
ATMODEM_MSG_ID_E AtModem_GetWebProvSettings(
    ATMODEM_WEB_PROV_SETTINGS *wp,
    uint32_t timeout);

ATMODEM_MSG_ID_E AtModem_SetXMLParse(uint8_t mode);
ATMODEM_MSG_ID_E AtModem_WebServer(uint8_t mode, char username[], char password[], char SSL[], char idleTimeOut[]);
ATMODEM_MSG_ID_E AtModem_StartMDNS(void);
ATMODEM_MSG_ID_E AtModem_AnnounceMDNS(void);
ATMODEM_MSG_ID_E AtModem_RegisterMDNSHost(char hostname[], char domain[]);
ATMODEM_MSG_ID_E AtModem_RegisterMDNSService(char *pServerName, char *pServerSubType, char *pServiceType, \
                                             char *pProtocol,  char *pDomain, char *pPort,  char *pKey1 );
ATMODEM_MSG_ID_E AtModem_SetWEP1( int8_t *pWEP);


/*-------------------------------------------------------------------------*
* Function Prototypes:
*-------------------------------------------------------------------------*/
void AtModem_FlushRxBuffer(void);

int AtModem_Init(void);
int AtModem_NetworkCheck(void);
int AtModem_OnCheck(void);
int ATModem_CellActivate(void);

int32_t AtModem_SocketOpen(char * ip, uint8_t port, uint8_t * cid);
int AtModem_SocketClose(uint8_t cid);
int AtModem_ReadID(unsigned char * UUID_buf, unsigned char buf_len);
void AtModem_ExitDataMode(void);
int AtModem_Connect(unsigned char * server, unsigned char bufsize);
int Modem_Socket_Send(uint8_t sock, char * buffer, long length);
int Modem_Scoket_Recv(char * buffer, long length);


char *strnloc(const char * haystack, const char * needle, unsigned char limit);

void AtModem_Write(const void *txData, uint16_t dataLength);
bool AtModem_Read(uint8_t *rxData, uint16_t dataLength, uint8_t blockFlag);
int AtModem_ReadLine(unsigned char * fullLine, unsigned int checkTime, char *buffer, unsigned int bufSize, char *termStr);
int AtModem_ReadLineTimeOut(unsigned int checkTime,
                            char *buffer,
                            unsigned int bufSize);


uint8_t AtModem_GetMeid(char *buffer);

// User supplied routines
extern void App_ProcessIncomingData(uint8_t rxData);
void App_DelayMS(uint32_t cnt);
void App_Write(const void *txData, uint16_t dataLength);
bool App_Read(uint8_t *rxData, uint16_t dataLength, uint8_t blockFlag);
ATMODEM_MSG_ID_E AtModem_ConfigAntenna(uint8_t mode);
uint8_t AtModem_ParseGetMacResponse(char *pMAC);
void NovatelModemInit();
uint8_t Novatel_Setup();
uint8_t Novatel_GetMeid(char *buffer);
uint8_t Novatel_isModemActivate();
#endif /* _MODEM_ATCMDLIB_H_ */

/*-------------------------------------------------------------------------*
* End of File:  novatel_cdma.h
*-------------------------------------------------------------------------*/
