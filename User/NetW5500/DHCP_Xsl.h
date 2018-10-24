//-------------------------------------------------------------------------------//
//                    ��������: RealView MDK-ARM Version 5.10                    //
//                    �� �� ��: RealView MDK-ARM Version 5.10                    //
//                    оƬ�ͺ�: STM32F103ZET6                                    //
//                    ��Ŀ����: HH-SPS ������Ŀ                                  //
//                    �ļ�����: DNS_Xsl.h                                        //
//                    ��    ��: ������                                           //
//                    ʱ    ��: 2014-08-07    �汾:  0.0                         //
//                    ע    ��: �ļ�δ��ʽӦ��,����������                        //
//-------------------------------------------------------------------------------//
/*************************************˵��*****************************************
**********************************************************************************/
#ifndef _DHCP_H_
#define _DHCP_H_

//���Ľṹ��
typedef struct DHCPXSL_S_MSG
{
   uint8_t  op;        //1-client���� 0-serverӦ��
   uint8_t  htype;     //����Ӳ����ַ����(1-10M��̫��)
   uint8_t  hlen;      //MAC��ַ�ֽ���(һ��Ϊ6)
   uint8_t  hops;      //����(��ʼ��0)
   uint32_t xid;       //����ID
   uint16_t secs;      //����
   uint16_t flags;     //��־
   uint8_t  ciaddr[4]; //�ͻ���IP
   uint8_t  yiaddr[4]; //������������ͻ��˵�IP
   uint8_t  siaddr[4]; //������IP
   uint8_t  giaddr[4]; //�м̴���IP
   uint8_t  chaddr[16];//�ͻ���Ӳ����ַ
   uint8_t  sname[64]; //��������������
   uint8_t  file[128]; //�����ļ���
   uint8_t  OPT[312];  //ѡ��(������)
}DHCPXSL_S_MSG;
//ʵ������
#define  STATE_DHCP_READY        0  //
#define  STATE_DHCP_DISCOVER     1  //
#define  STATE_DHCP_REQUEST      2  //
#define  STATE_DHCP_LEASED       3  //
#define  STATE_DHCP_REREQUEST    4  //
#define  STATE_DHCP_RELEASE      5  //












#define EXTERN_DHCP_MAC    ConfigMsg.mac
#define EXTERN_DHCP_SN     ConfigMsg.sub
#define EXTERN_DHCP_GW     ConfigMsg.gw
#define EXTERN_DHCP_DNS    ConfigMsg.dns
#define EXTERN_DHCP_NAME   "iweb"//ConfigMsg.domain
#define EXTERN_DHCP_SIP    ConfigMsg.lip



#define DHCP_RET_NONE      0
#define DHCP_RET_ERR       1
#define DHCP_RET_TIMEOUT   2
#define DHCP_RET_UPDATE    3
#define DHCP_RET_CONFLICT  4


         
#define  MAX_DHCP_RETRY          3
#define  DHCP_WAIT_TIME          5
         
#define  DHCP_FLAGSBROADCAST     0x8000

/* UDP port numbers for DHCP */
#define  DHCP_SERVER_PORT        67	/* from server to client */
#define  DHCP_CLIENT_PORT        68	/* from client to server */

/* DHCP message OP code */
#define  DHCP_BOOTREQUEST        1
#define  DHCP_BOOTREPLY          2

/* DHCP message type */
#define  DHCP_DISCOVER           1
#define  DHCP_OFFER              2
#define  DHCP_REQUEST            3
#define  DHCP_DECLINE            4
#define  DHCP_ACK                5
#define  DHCP_NAK                6
#define  DHCP_RELEASE            7
#define  DHCP_INFORM             8

#define DHCP_HTYPE10MB           1
#define DHCP_HTYPE100MB          2

#define DHCP_HLENETHERNET        6
#define DHCP_HOPS                0
#define DHCP_SECS                0

#define MAGIC_COOKIE		         0x63825363
#define DEFAULT_XID              0x12345678

#define DEFAULT_LEASETIME        0xffffffff	/* infinite lease time */

/* DHCP option and value (cf. RFC1533) */
enum
{
   padOption               = 0,
   subnetMask              = 1,
   timerOffset             = 2,
   routersOnSubnet         = 3,
   timeServer              = 4,
   nameServer              = 5,
   dns                     = 6,
   logServer               = 7,
   cookieServer            = 8,
   lprServer               = 9,
   impressServer           = 10,
   resourceLocationServer  = 11,
   hostName                = 12,
   bootFileSize            = 13,
   meritDumpFile           = 14,
   domainName              = 15,
   swapServer              = 16,
   rootPath                = 17,
   extentionsPath          = 18,
   IPforwarding            = 19,
   nonLocalSourceRouting   = 20,
   policyFilter            = 21,
   maxDgramReasmSize       = 22,
   defaultIPTTL            = 23,
   pathMTUagingTimeout     = 24,
   pathMTUplateauTable     = 25,
   ifMTU                   = 26,
   allSubnetsLocal         = 27,
   broadcastAddr           = 28,
   performMaskDiscovery    = 29,
   maskSupplier            = 30,
   performRouterDiscovery  = 31,
   routerSolicitationAddr  = 32,
   staticRoute             = 33,
   trailerEncapsulation    = 34,
   arpCacheTimeout         = 35,
   ethernetEncapsulation   = 36,
   tcpDefaultTTL           = 37,
   tcpKeepaliveInterval    = 38,
   tcpKeepaliveGarbage     = 39,
   nisDomainName           = 40,
   nisServers              = 41,
   ntpServers              = 42,
   vendorSpecificInfo      = 43,
   netBIOSnameServer       = 44,
   netBIOSdgramDistServer  = 45,
   netBIOSnodeType         = 46,
   netBIOSscope            = 47,
   xFontServer             = 48,
   xDisplayManager         = 49,
   dhcpRequestedIPaddr     = 50,
   dhcpIPaddrLeaseTime     = 51,
   dhcpOptionOverload      = 52,
   dhcpMessageType         = 53,
   dhcpServerIdentifier    = 54,
   dhcpParamRequest        = 55,
   dhcpMsg                 = 56,
   dhcpMaxMsgSize          = 57,
   dhcpT1value             = 58,
   dhcpT2value             = 59,
   dhcpClassIdentifier     = 60,
   dhcpClientIdentifier    = 61,
   endOption               = 255
};



#define MAX_DHCP_OPT	16


void  init_dhcp_client(void);
//uint8 getIP_DHCPS(void);      // Get the network configuration from the DHCP server
uint8 check_DHCP_state(SOCKET s); // Check the DHCP state

#endif	/* _DHCP_H_ */