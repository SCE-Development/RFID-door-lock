#pragma once

#include <WiFi.h>

/**
 * @brief Simple http response parsing methods 
 * 
 * All http parsing methods must be called in order.
 * get_version(...)
 * get_status_code(...)
 * get_status_reason(...)
 * 
 * get_next_header(...) // Until empty header
 */
namespace http {

  enum error {
    ok=0,
    timeout=-1
  };

  /**
   * @brief Read http version from response and stores in output buffer.
   * When the output buffer is full, the function will continue reading, but the output will be truncated.
   * 
   * Returns http::timeout on timeout,
   * http::ok otherwise.
   * 
   * @param output_buffer 
   * @param size size of output buffer
   * @param timeout Timeout in milliseconds
   * @param client Connection
   * @return http::error 
   */
  http::error get_version(char * output_buffer, size_t size, unsigned long timeout, WiFiClient& client);
  /**
   * @brief Get the http status code and stores in result and output buffer.
   * When the output buffer is full, the function will continue reading, but the output will be truncated. This should never happen.
   * 
   * Returns http::timeout on timeout,
   * http::ok otherwise.
   * 
   * @param result the parsed int of the output buffer
   * @param output_buffer 
   * @param size size of output buffer
   * @param timeout Timeout in milliseconds
   * @param client Connection
   * @return http::error 
   */
  http::error get_status_code(int& result, char * output_buffer, size_t size, unsigned long timeout, WiFiClient& client);
  /**
   * @brief Get the status reason and store in output buffer.
   * When the output buffer is full, the function will continue reading, but the output will be truncated.
   * 
   * Returns http::timeout on timeout,
   * http::ok otherwise.
   * 
   * @param output_buffer 
   * @param size size of output buffer.
   * @param timeout Timeout in millisecond
   * @param client Connection
   * @return http::error 
   */
  http::error get_status_reason(char * output_buffer, size_t size, unsigned long timeout, WiFiClient& client);

  /**
   * @brief Get the next header and stores the key in the key buffer and value in the value buffer.
   * When either buffer is full, the function will continue reading, but the output will be truncated.
   * If the last header was read, this call should leave the key and value buffer empty (key[0] = value[0] = '\0')
   * 
   * Returns http::timeout on timeout,
   * http::ok otherwise.
   * 
   * @param key 
   * @param key_size size of the header buffer.
   * @param value 
   * @param value_size size of the value buffer.
   * @param timeout Timeout in millisecond
   * @param client Connection
   * @return http::error 
   */
  http::error get_next_header(char * key, size_t key_size, char * value, size_t value_size, unsigned long timeout, WiFiClient& client);

  namespace internal {
    /**
     * @brief Read from connection until a given character is reached or the timeout expires. Fills the output buffer.
     * When the output buffer is full, the function will continue reading, but the output will be truncated.
     * 
     * Returns http::timeout on timeout,
     * http::ok otherwise.
     * 
     * @param buf Output buffer
     * @param size Output buffer size
     * @param timeout Timeout in milliseconds
     * @param client Connection
     * @param c Character to end at
     * @return http::error 
     */
    http::error read_until_character(char * buf, size_t size, unsigned long timeout, WiFiClient& client, char c);
    /**
     * @brief Read from connection until a CRLF is reached or the timeout expires. Fills the output buffer.
     * When the output buffer is full, the function will continue reading, but the output will be truncated.
     * 
     * Returns http::timeout on timeout,
     * http::ok otherwise.
     * 
     * @param buf Output buffer
     * @param size Output buffer size
     * @param timeout Timeout in milliseconds
     * @param client Connection
     * @param c Character to end at
     * @return http::error 
     */
    http::error read_until_crlf(char * buf, size_t size, unsigned long timeout, WiFiClient& client);
  };
};




http::error http::internal::read_until_character(char * buf, size_t size, unsigned long timeout, WiFiClient& client, char c) {
  int i = 0;
  char curr = 0;

  unsigned long start = millis();

// SD1306
  while(curr != c){
    // Keep reading even when buffer is full. This will truncate the output, but if the method would end early, other http::get_* will be in the wrong position to parse.
    if(i < (size - 1) && curr != 0) {
      buf[i] = curr;
      i++;
    }

    // Wait for a byte to read. Break if it times out.
    while(!client.available()) {
      if(millis() - start > timeout) {
        return http::timeout;
      }
    }

    curr = client.read();
  }

  buf[i] = '\0';

  return http::ok;
}

http::error http::internal::read_until_crlf(char * buf, size_t size, unsigned long timeout, WiFiClient& client) {
  int i = 0;
  char curr = 0, second_curr = 0;

  unsigned long start = millis();


  while(!(second_curr == '\r' && curr == '\n')){
    // Keep reading even when buffer is full. This will truncate the output, but if the method would end early, other http::get_* will be in the wrong position to parse.
    if(i < (size - 1) && second_curr != 0) {
      buf[i] = second_curr;
      i++;
    }

    // Wait for a byte to read. Break if it times out.
    while(!client.available()) {
      if(millis() - start > timeout) {
        return http::timeout;
      }
    }

    second_curr = curr;
    curr = client.read();
  }

  buf[i] = '\0';

  return http::ok;
}


// HTTP/1.1 200 OK
// [version] [status code] [status reason]


http::error http::get_version(char * output_buffer, size_t size, unsigned long timeout, WiFiClient& client) {
  return http::internal::read_until_character(output_buffer, size, timeout, client, ' ');
}

http::error http::get_status_code(int& result, char * output_buffer, size_t size, unsigned long timeout, WiFiClient& client) {
  http::error error = http::internal::read_until_character(output_buffer, size, timeout, client, ' ');
  if(error != http::ok) {
    return error;
  }
  
  result = atoi(output_buffer);

  return http::ok;
}

http::error http::get_status_reason(char * output_buffer, size_t size, unsigned long timeout, WiFiClient& client) {
  return http::internal::read_until_crlf(output_buffer, size, timeout, client);
}



http::error http::get_next_header(char * key, size_t key_size, char * value, size_t value_size, unsigned long timeout, WiFiClient& client) {
  int i = 0;
  char curr = 0, second_curr = 0;

  unsigned long start = millis();


  while(!(second_curr == '\r' && curr == '\n') || !(second_curr == ':')){
    // Keep reading even when buffer is full. This will truncate the output, but if the method would end early, other http::get_* will be in the wrong position to parse.
    if(i < (key_size - 1) && second_curr != 0) {
      key[i] = second_curr;
      i++;
    }

    // Wait for a byte to read. Break if it times out.
    while(!client.available()) {
      if(millis() - start > timeout) {
        return http::timeout;
      }
    }

    second_curr = curr;
    curr = client.read();
  }

  key[i] = '\0';

  while(!(second_curr == '\r' && curr == '\n')){
    // Keep reading even when buffer is full. This will truncate the output, but if the method would end early, other http::get_* will be in the wrong position to parse.
    if(i < (value_size - 1) && second_curr != 0) {
      value[i] = second_curr;
      i++;
    }

    // Wait for a byte to read. Break if it times out.
    while(!client.available()) {
      if(millis() - start > timeout) {
        return http::timeout;
      }
    }

    second_curr = curr;
    curr = client.read();
  }

  value[i] = '\0';

  return http::ok;
}