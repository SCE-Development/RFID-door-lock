/**
 * @file configure.h
 * @author andrew lin
 * @brief A file to hold most configurable parameters
 * @version 0.1
 * @date 2023-10-12
 * 
 */


/**
 * @brief This should be the template request string.
 * Should be a format string.
 * 
 */
#define REQUEST_STRING "GET / HTTP/1.1\r\nHost:google.com\r\n\r\n"

/**
 * @brief WiFi SSID (WiFi network name)
 * 
 */
#define WIFI_SSID "no"
/**
 * @brief WiFi Password.
 * 
 */
#define WIFI_PASSWORD "123456789"


/**
 * @brief This should be the ip to the backend server
 * 
 */
#define SERVER_IP IPAddress(142,251,46,238) // This points to google at the moment.


/**
 * @brief This is the set buffer size for each response string.
 * 
 */
#define RESPONSE_BUFFER_SIZE 64