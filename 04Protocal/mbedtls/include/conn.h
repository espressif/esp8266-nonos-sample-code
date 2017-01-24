#ifndef __CONN_H__
#define __CONN_H__

void user_conn_init(void);

/*
 * SSL Error codes
 */
#define ESPCONN_ERR_SSL_FEATURE_UNAVAILABLE                0x10  /**< The requested feature is not available. */
#define ESPCONN_ERR_SSL_BAD_INPUT_DATA                    -0x71  /**< Bad input parameters to function. */
#define ESPCONN_ERR_SSL_INVALID_MAC                        0x0F  /**< Verification of the message MAC failed. */
#define ESPCONN_ERR_SSL_INVALID_RECORD                    -0x72  /**< An invalid SSL record was received. */
#define ESPCONN_ERR_SSL_CONN_EOF                           0x0E  /**< The connection indicated an EOF. */
#define ESPCONN_ERR_SSL_UNKNOWN_CIPHER                    -0x73  /**< An unknown cipher was received. */
#define ESPCONN_ERR_SSL_NO_CIPHER_CHOSEN                   0x0D  /**< The server has no ciphersuites in common with the client. */
#define ESPCONN_ERR_SSL_NO_RNG                            -0x74  /**< No RNG was provided to the SSL module. */
#define ESPCONN_ERR_SSL_NO_CLIENT_CERTIFICATE              0x0C  /**< No client certification received from the client, but required by the authentication mode. */
#define ESPCONN_ERR_SSL_CERTIFICATE_TOO_LARGE             -0x75  /**< Our own certificate(s) is/are too large to send in an SSL message. */
#define ESPCONN_ERR_SSL_CERTIFICATE_REQUIRED               0x0B  /**< The own certificate is not set, but needed by the server. */
#define ESPCONN_ERR_SSL_PRIVATE_KEY_REQUIRED              -0x76  /**< The own private key or pre-shared key is not set, but needed. */
#define ESPCONN_ERR_SSL_CA_CHAIN_REQUIRED                  0x0A  /**< No CA Chain is set, but required to operate. */
#define ESPCONN_ERR_SSL_UNEXPECTED_MESSAGE                -0x77  /**< An unexpected message was received from our peer. */
#define ESPCONN_ERR_SSL_FATAL_ALERT_MESSAGE                0x09  /**< A fatal alert message was received from our peer. */
#define ESPCONN_ERR_SSL_PEER_VERIFY_FAILED                -0x78  /**< Verification of our peer failed. */
#define ESPCONN_ERR_SSL_PEER_CLOSE_NOTIFY                  0x08  /**< The peer notified us that the connection is going to be closed. */
#define ESPCONN_ERR_SSL_BAD_HS_CLIENT_HELLO               -0x79  /**< Processing of the ClientHello handshake message failed. */
#define ESPCONN_ERR_SSL_BAD_HS_SERVER_HELLO                0x07  /**< Processing of the ServerHello handshake message failed. */
#define ESPCONN_ERR_SSL_BAD_HS_CERTIFICATE                -0x7A  /**< Processing of the Certificate handshake message failed. */
#define ESPCONN_ERR_SSL_BAD_HS_CERTIFICATE_REQUEST         0x06  /**< Processing of the CertificateRequest handshake message failed. */
#define ESPCONN_ERR_SSL_BAD_HS_SERVER_KEY_EXCHANGE        -0x7B  /**< Processing of the ServerKeyExchange handshake message failed. */
#define ESPCONN_ERR_SSL_BAD_HS_SERVER_HELLO_DONE           0x05  /**< Processing of the ServerHelloDone handshake message failed. */
#define ESPCONN_ERR_SSL_BAD_HS_CLIENT_KEY_EXCHANGE        -0x7C  /**< Processing of the ClientKeyExchange handshake message failed. */
#define ESPCONN_ERR_SSL_BAD_HS_CLIENT_KEY_EXCHANGE_RP      0x04  /**< Processing of the ClientKeyExchange handshake message failed in DHM / ECDH Read Public. */
#define ESPCONN_ERR_SSL_BAD_HS_CLIENT_KEY_EXCHANGE_CS     -0x7D  /**< Processing of the ClientKeyExchange handshake message failed in DHM / ECDH Calculate Secret. */
#define ESPCONN_ERR_SSL_BAD_HS_CERTIFICATE_VERIFY          0x03  /**< Processing of the CertificateVerify handshake message failed. */
#define ESPCONN_ERR_SSL_BAD_HS_CHANGE_CIPHER_SPEC         -0x7E  /**< Processing of the ChangeCipherSpec handshake message failed. */
#define ESPCONN_ERR_SSL_BAD_HS_FINISHED                    0x02  /**< Processing of the Finished handshake message failed. */
#define ESPCONN_ERR_SSL_ALLOC_FAILED                      -0x7F  /**< Memory allocation failed */
#define ESPCONN_ERR_SSL_HW_ACCEL_FAILED                    0x01  /**< Hardware acceleration function returned with error */
#define ESPCONN_ERR_SSL_HW_ACCEL_FALLTHROUGH               0x11  /**< Hardware acceleration function skipped / left alone data */
#define ESPCONN_ERR_SSL_COMPRESSION_FAILED                -0x6F  /**< Processing of the compression / decompression failed */
#define ESPCONN_ERR_SSL_BAD_HS_PROTOCOL_VERSION            0x12  /**< Handshake protocol not within min/max boundaries */
#define ESPCONN_ERR_SSL_BAD_HS_NEW_SESSION_TICKET         -0x6E  /**< Processing of the NewSessionTicket handshake message failed. */
#define ESPCONN_ERR_SSL_SESSION_TICKET_EXPIRED             0x13  /**< Session ticket has expired. */
#define ESPCONN_ERR_SSL_PK_TYPE_MISMATCH                  -0x6D  /**< Public key type mismatch (eg, asked for RSA key exchange and presented EC key) */
#define ESPCONN_ERR_SSL_UNKNOWN_IDENTITY                   0x14  /**< Unknown identity received (eg, PSK identity) */
#define ESPCONN_ERR_SSL_INTERNAL_ERROR                    -0x6C  /**< Internal error (eg, unexpected failure in lower-level module) */
#define ESPCONN_ERR_SSL_COUNTER_WRAPPING                   0x15  /**< A counter would wrap (eg, too many messages exchanged). */
#define ESPCONN_ERR_SSL_WAITING_SERVER_HELLO_RENEGO       -0x6B  /**< Unexpected message at ServerHello in renegotiation. */
#define ESPCONN_ERR_SSL_HELLO_VERIFY_REQUIRED              0x16  /**< DTLS client must retry for hello verification */
#define ESPCONN_ERR_SSL_BUFFER_TOO_SMALL                  -0x6A  /**< A buffer is too small to receive or write a message */
#define ESPCONN_ERR_SSL_NO_USABLE_CIPHERSUITE              0x17  /**< None of the common ciphersuites is usable (eg, no suitable certificate, see debug messages). */
#define ESPCONN_ERR_SSL_WANT_READ                         -0x69  /**< Connection requires a read call. */
#define ESPCONN_ERR_SSL_WANT_WRITE                         0x18  /**< Connection requires a write call. */
#define ESPCONN_ERR_SSL_TIMEOUT                           -0x68  /**< The operation timed out. */

/**
 * \name X509 Error codes
 * \{
 */
#define ESPCONN_ERR_X509_FEATURE_UNAVAILABLE               0x60  /**< Unavailable feature, e.g. RSA hashing/encryption combination. */
#define ESPCONN_ERR_X509_UNKNOWN_OID                      -0x21  /**< Requested OID is unknown. */
#define ESPCONN_ERR_X509_INVALID_FORMAT                    0x59  /**< The CRT/CRL/CSR format is invalid, e.g. different type expected. */
#define ESPCONN_ERR_X509_INVALID_VERSION                  -0x22  /**< The CRT/CRL/CSR version element is invalid. */
#define ESPCONN_ERR_X509_INVALID_SERIAL                    0x58  /**< The serial tag or value is invalid. */
#define ESPCONN_ERR_X509_INVALID_ALG                      -0x23  /**< The algorithm tag or value is invalid. */
#define ESPCONN_ERR_X509_INVALID_NAME                      0x57  /**< The name tag or value is invalid. */
#define ESPCONN_ERR_X509_INVALID_DATE                     -0x24  /**< The date tag or value is invalid. */
#define ESPCONN_ERR_X509_INVALID_SIGNATURE                 0x56  /**< The signature tag or value invalid. */
#define ESPCONN_ERR_X509_INVALID_EXTENSIONS               -0x25  /**< The extension tag or value is invalid. */
#define ESPCONN_ERR_X509_UNKNOWN_VERSION                   0x55  /**< CRT/CRL/CSR has an unsupported version number. */
#define ESPCONN_ERR_X509_UNKNOWN_SIG_ALG                  -0x26  /**< Signature algorithm (oid) is unsupported. */
#define ESPCONN_ERR_X509_SIG_MISMATCH                      0x54  /**< Signature algorithms do not match. (see \c ::mbedtls_x509_crt sig_oid) */
#define ESPCONN_ERR_X509_CERT_VERIFY_FAILED               -0x27  /**< Certificate verification failed, e.g. CRL, CA or signature check failed. */
#define ESPCONN_ERR_X509_CERT_UNKNOWN_FORMAT               0x53  /**< Format not recognized as DER or PEM. */
#define ESPCONN_ERR_X509_BAD_INPUT_DATA                   -0x28  /**< Input invalid. */
#define ESPCONN_ERR_X509_ALLOC_FAILED                      0x52  /**< Allocation of memory failed. */
#define ESPCONN_ERR_X509_FILE_IO_ERROR                    -0x29  /**< Read/write of file failed. */
#define ESPCONN_ERR_X509_BUFFER_TOO_SMALL                  0x51  /**< Destination buffer is too small. */
/* \} name */

#endif
