#ifndef LOGS_H

#define LOGS_H

/**
 * @brief Displays a message at DEBUG level. The message will be in the format:
 * 
 * <timestamp> [DEBUG] format
 * 
 * If LOG_LEVEL is defined as less then LOG_LEVEL_DEBUG, then nothing will be displayed on call.
 * 
 * @param format Format for the message to be displayed.
 * @param ... Values to be injected inside the function.
 */
void DEBUG(char *format, ...);

/**
 * @brief Displays a message at INFO level. The message will be in the format:
 * 
 * <timestamp> [INFO] format
 * 
 * If LOG_LEVEL is defined as less then LOG_LEVEL_INFO, then nothing will be displayed on call.
 * 
 * @param format Format for the message to be displayed.
 * @param ... Values to be injected inside the function.
 */
void INFO(char *format, ...);

/**
 * @brief Displays a message at ERROR level. The message will be in the format:
 * 
 * <timestamp> [ERROR] format Error: <value of error, if existing>
 * 
 * If LOG_LEVEL is defined as less then LOG_LEVEL_ERROR, then nothing will be displayed on call.
 * 
 * @param format Format for the message to be displayed.
 * @param ... Values to be injected inside the function.
 */
void ERROR(char *format, ...);

/**
 * @brief Displays a message at FATAL level. The message will be in the format:
 * 
 * <timestamp> [FATAL] format Error: <value of error, if existing>
 * 
 * If LOG_LEVEL is defined as less then LOG_LEVEL_FATAL, then nothing will be displayed on call.
 * 
 * @param format Format for the message to be displayed.
 * @param ... Values to be injected inside the function.
 */
void FATAL(char *format, ...);

#define LOG_LEVEL_FATAL 0
#define LOG_LEVEL_ERROR 1
#define LOG_LEVEL_INFO 2
#define LOG_LEVEL_DEBUG 3

#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_INFO
#endif

#endif
