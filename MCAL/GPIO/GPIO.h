
#ifndef GPIO_H_
#define GPIO_H_
#include "std_types.h"
#include "common.h"

/**
 * @enum en_GPIO_port_t
 * Specifies the number of the port.
 */
typedef enum {
    PORT_A = 0, PORT_B = 1, PORT_C = 2, PORT_D = 3, PORT_E = 4, PORT_F = 5
}en_GPIO_port_t;

/**
 * @enum en_GPIO_pin_t
 * Specifies the number of the pin.
 */
typedef enum {
    PIN0 = 0, PIN1 = 1, PIN2 = 2, PIN3 = 3, PIN4 = 4, PIN5 = 5, PIN6 = 6, PIN7 = 7
}en_GPIO_pin_t;

/**
 * @enum en_GPIO_pinDir_t
 * Specifies the mode of operation of the pin.
 */
typedef enum {
    INPUT = 0, OUTPUT = 1
}en_GPIO_pinDir_t;

/**
 * @enum en_GPIO_driveCurrent_t
 * Specifies the drive current of the pin.
 */
typedef enum {
    DRIVE_2mA = 0, DRIVE_4mA = 1, DRIVE_8mA = 2
}en_GPIO_driveCurrent_t;

/**
 * @enum en_GPIO_pull_t
 * Specifies the pull state of the pin.
 */
typedef enum {
    PULL_UP = 0, PULL_DOWN = 1, OPEN_DRAIN = 2
}en_GPIO_pull_t;

typedef enum {
    GPIO_STATUS_SUCCESS                     =   100,
    GPIO_STATUS_CLOCK_FAILED                =   101,
    GPIO_STATUS_SET_DIRECTION_FAILED        =   102,
    GPIO_STATUS_SET_DRIVE_CURRENT_FAILED    =   103,
    GPIO_STATUS_SET_PULL_FAILED             =   104,
    GPIO_STATUS_INVALID_PIN_DIR             =   105,
    GPIO_STATUS_INVALID_PORT_NUM            =   106,
    GPIO_STATUS_INVALID_CONFIG_ARRAY        =   107
}en_GPIO_error_t;

/**
 * @struct st_GPIO_config_t
 * @brief Holds the configuration of a specific pin of a port.
 * @var en_GPIO_port
 * Member 'en_GPIO_port' sets the port to be configured.
 * @var en_GPIO_pin_t
 * Member 'en_GPIO_pin_t' sets the pin to be configured.
 * @var en_GPIO_pinDir
 * Member 'en_GPIO_pinDir' sets the direction of the pin.
 * @var en_GPIO_pull
 * Member 'en_GPIO_pull; sets the pull state of the pin.
 */
typedef struct {
    en_GPIO_port_t          en_GPIO_port;
    en_GPIO_pin_t           en_GPIO_pin;
    en_GPIO_pinDir_t        en_GPIO_pinDir;
    en_GPIO_driveCurrent_t  en_GPIO_driveCurrent;
    en_GPIO_pull_t          en_GPIO_pull;
}st_GPIO_config_t;

/**
 * @brief Initializes a set of pins with the specified configuration.
 * @param[in] ptr_st_GPIO_config    Address of the array of the specified pins.
 * @return en_GPIO_error_t
 */
en_GPIO_error_t GPIO_Init(const st_GPIO_config_t *ptr_st_GPIO_config);


en_GPIO_error_t GPIO_DeInit(const st_GPIO_config_t *ptr_st_GPIO_config);

/**
 * @brief Reads the state of a single pin.
 * @param[in] ptr_st_GPIO_config    Address of the specified pin configuration strcuture.
 * @param[in] ptr_pinValue          Address of the value where we stores the state of the pin.
 * @return en_GPIO_error_t
 */
en_GPIO_error_t GPIO_ReadPin(const st_GPIO_config_t *ptr_st_GPIO_config, uint8_t *ptr_pinValue);

/**
 * @brief Reads the state of a single pin.
 * @param[in] ptr_st_GPIO_config    Address of the specified pin configuration strcuture.
 * @param[in] pinValue              The value to be written on the specified pin.
 * @return en_GPIO_error_t
 */
en_GPIO_error_t GPIO_WritePin(const st_GPIO_config_t *ptr_st_GPIO_config, uint8 pinValue);

/* TODO Implement the GPIO toggle function.*/
// void GPIO_TogglePin(const st_GPIO_config_t *ptr_st_GPIO_config);

#endif
