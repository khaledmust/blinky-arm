
#include "./GPIO.h"
#include "GPIO_private.h"
#include <assert.h>

#define	PORT_LOCK_VALUE     (uint32_t)(0x4C4F434B)
#define BIT_MASKING_OFFSET  (uint8_t)(2)
#define PCTL_PIN_MASK       (uint32_t)(0x0000000F)
#define PCTL_PIN_BIT_NUM    (uint8_t)(4)
#define MIN_PCTL_VAL        (uint8_t)(1)
#define MAX_PCTL_VAL        (uint8_t)(15)

/**
 * @brief Enables the clock source to the GPIO module.
 * @param[in] en_GPIO_port      Port number to be configured.
 * @return en_GPIO_error_t
 */
static en_GPIO_error_t GPIO_EnableClock(en_GPIO_port_t en_GPIO_port) {
    SET_BIT(RCGCGPIO, en_GPIO_port);
    return GPIO_STATUS_SUCCESS;
}

/**
 * @brief Sets the direction of the specified pin.
 * @param[in] en_GPIO_port      Port number to be configured.
 * @param[in] en_GPIO_pin       Pin number to be configured.
 * @param[in] en_GPIO_pinDir    Direction of the specified pin.
 * @return en_GPIO_error_t
 */
static en_GPIO_error_t GPIO_SetDirection(en_GPIO_port_t en_GPIO_port, en_GPIO_pin_t en_GPIO_pin, en_GPIO_pinDir_t en_GPIO_pinDir) {
    if (en_GPIO_pinDir == OUTPUT) {
        SET_BIT(GPIODIR(en_GPIO_port), en_GPIO_pin);
        return GPIO_STATUS_SUCCESS;
    } else if (en_GPIO_pinDir == INPUT) {
        CLR_BIT(GPIODIR(en_GPIO_port), en_GPIO_pin);
        return GPIO_STATUS_SUCCESS;
    } else {
        return GPIO_STATUS_SET_DIRECTION_FAILED;
    }
}

/**
 * @brief Sets the drive current of the specified pin.
 * @param[in] en_GPIO_port          Port number to be configured.
 * @param[in] en_GPIO_pin           Pin number to be configured.
 * @param[in] en_GPIO_driveCurrent  The drive current.
 * @return en_GPIO_error_t
 */
static en_GPIO_error_t GPIO_SetDriveCurrent(en_GPIO_port_t en_GPIO_port, en_GPIO_pin_t en_GPIO_pin, en_GPIO_driveCurrent_t en_GPIO_driveCurrent) {
    assert(en_GPIO_driveCurrent == DRIVE_2mA || en_GPIO_driveCurrent  == DRIVE_4mA || en_GPIO_driveCurrent == DRIVE_8mA);
    switch (en_GPIO_driveCurrent) {
        case DRIVE_2mA:
            SET_BIT(GPIODR2R(en_GPIO_port), en_GPIO_pin);
            CLR_BIT(GPIODR4R(en_GPIO_port), en_GPIO_pin);
            CLR_BIT(GPIODR8R(en_GPIO_port), en_GPIO_pin);
            break;
        case DRIVE_4mA:
            CLR_BIT(GPIODR2R(en_GPIO_port), en_GPIO_pin);
            SET_BIT(GPIODR4R(en_GPIO_port), en_GPIO_pin);
            CLR_BIT(GPIODR8R(en_GPIO_port), en_GPIO_pin);
            break;
        case DRIVE_8mA:
            CLR_BIT(GPIODR2R(en_GPIO_port), en_GPIO_pin);
            CLR_BIT(GPIODR4R(en_GPIO_port), en_GPIO_pin);
            SET_BIT(GPIODR8R(en_GPIO_port), en_GPIO_pin);
            break;
    }
    return GPIO_STATUS_SUCCESS;
}

/**
 * @brief Sets the pull state of the specified pin.
 * @param[in] en_GPIO_port          Port number to be configured.
 * @param[in] en_GPIO_pin           Pin number to be configured.
 * @param[in] en_GPIO_pull          The pull state of the pin.
 * @return en_GPIO_error_t
 */
static en_GPIO_error_t GPIO_SetPull(en_GPIO_port_t en_GPIO_port, en_GPIO_pin_t en_GPIO_pin, en_GPIO_pull_t en_GPIO_pull) {
    assert(en_GPIO_pull == PULL_UP || en_GPIO_pull == PULL_DOWN || en_GPIO_pull == OPEN_DRAIN);
    switch (en_GPIO_pull) {
        case PULL_UP:
            SET_BIT(GPIOPUR(en_GPIO_port), en_GPIO_pin);
            CLR_BIT(GPIOPDR(en_GPIO_port), en_GPIO_pin);
            CLR_BIT(GPIOODR(en_GPIO_port), en_GPIO_pin);
			CLR_BIT(GPIOSLR(en_GPIO_port), en_GPIO_pin);
            break;
        case PULL_DOWN:
            CLR_BIT(GPIOPUR(en_GPIO_port), en_GPIO_pin);
			SET_BIT(GPIOPDR(en_GPIO_port), en_GPIO_pin);
			CLR_BIT(GPIOODR(en_GPIO_port), en_GPIO_pin);
			CLR_BIT(GPIOSLR(en_GPIO_port), en_GPIO_pin);
            break;
        case OPEN_DRAIN:
            CLR_BIT(GPIOPUR(en_GPIO_port), en_GPIO_pin);
			CLR_BIT(GPIOPDR(en_GPIO_port), en_GPIO_pin);
			SET_BIT(GPIOODR(en_GPIO_port), en_GPIO_pin);
			CLR_BIT(GPIOSLR(en_GPIO_port), en_GPIO_pin);
            break;
    }
    return GPIO_STATUS_SUCCESS;
}

/**
 * @brief Configure the specified pin as digital.
 * @param[in] en_GPIO_port          Port number to be configured.
 * @param[in] en_GPIO_pin           Pin number to be configured.
 * @return en_GPIO_error_t
 */
static en_GPIO_error_t GPIO_DigitalEnable(en_GPIO_port_t en_GPIO_port, en_GPIO_pin_t en_GPIO_pin) {
    SET_BIT(GPIODEN(en_GPIO_port), en_GPIO_pin);
    return GPIO_STATUS_SUCCESS;
}

/**
 * @brief Initializes a set of pins with the specified configuration.
 * @param[in] ptr_st_GPIO_config    Address of the array of the specified pins.
 * @return en_GPIO_error_t
 */
en_GPIO_error_t GPIO_Init(const st_GPIO_config_t *ptr_st_GPIO_config) {
    
    /* Variable for the current API status */
    en_GPIO_error_t en_GPIO_error_CurrentError;
    
    if (ptr_st_GPIO_config != NULL) {
        
        /* 1. Enable the clodk to the port */
        en_GPIO_error_CurrentError = GPIO_EnableClock(ptr_st_GPIO_config->en_GPIO_port);
        if (en_GPIO_error_CurrentError != GPIO_STATUS_SUCCESS) {
            return GPIO_STATUS_CLOCK_FAILED;
        }
        
        /* 2. Set the direction of the GPIO port pins */
        en_GPIO_error_CurrentError = GPIO_SetDirection(ptr_st_GPIO_config->en_GPIO_port, ptr_st_GPIO_config->en_GPIO_pin, ptr_st_GPIO_config->en_GPIO_pinDir);
        if (en_GPIO_error_CurrentError != GPIO_STATUS_SUCCESS) {
            return en_GPIO_error_CurrentError;
        }
        
        /* 3. Set the drive strenght of the pin */
        en_GPIO_error_CurrentError = GPIO_SetDriveCurrent(ptr_st_GPIO_config->en_GPIO_port, ptr_st_GPIO_config->en_GPIO_pin, ptr_st_GPIO_config->en_GPIO_driveCurrent);
        if (en_GPIO_error_CurrentError != GPIO_STATUS_SUCCESS) {
            return en_GPIO_error_CurrentError;
        }
        
        /* 4. Set the pull state for the pin */
        en_GPIO_error_CurrentError = GPIO_SetPull(ptr_st_GPIO_config->en_GPIO_port, ptr_st_GPIO_config->en_GPIO_pin, ptr_st_GPIO_config->en_GPIO_pull);
        if (en_GPIO_error_CurrentError != GPIO_STATUS_SUCCESS) {
            return en_GPIO_error_CurrentError;
        }
        
        /* 5. Enable the GPIO pin as digital */
        en_GPIO_error_CurrentError = GPIO_DigitalEnable(ptr_st_GPIO_config->en_GPIO_port, ptr_st_GPIO_config->en_GPIO_pin);
        if (en_GPIO_error_CurrentError != GPIO_STATUS_SUCCESS) {
            return en_GPIO_error_CurrentError;
        }
        return GPIO_STATUS_SUCCESS;
    } else {
        /* return Failed */
    }
    
}

/**
 * @brief Reads the state of a single pin.
 * @param[in] ptr_st_GPIO_config    Address of the specified pin configuration strcuture.
 * @param[in] ptr_pinValue          Address of the value where we stores the state of the pin.
 * @return en_GPIO_error_t
 */
en_GPIO_error_t GPIO_ReadPin(const st_GPIO_config_t *ptr_st_GPIO_config, uint8_t *ptr_pinValue) {
    
    /* Variable for holding the port number. */
    uint8 portNum = ptr_st_GPIO_config->en_GPIO_port;
    uint8 pinNum = ptr_st_GPIO_config->en_GPIO_pin;
    
    if (ptr_st_GPIO_config->en_GPIO_pinDir == INPUT || ptr_st_GPIO_config->en_GPIO_pinDir == OUTPUT) {
        assert(portNum == PORT_A || portNum == PORT_B || portNum == PORT_C || portNum == PORT_D || portNum == PORT_E || portNum == PORT_F);
  
        switch (portNum) {
            case PORT_A:
				*ptr_pinValue = GET_BIT_STATUS(GPIODATA(PORT_A), pinNum);
                break;
            case PORT_B:
                *ptr_pinValue = GET_BIT_STATUS(GPIODATA(PORT_B), pinNum);
                break;
            case PORT_C:
                *ptr_pinValue = GET_BIT_STATUS(GPIODATA(PORT_C), pinNum);
                break;
            case PORT_D:
                *ptr_pinValue = GET_BIT_STATUS(GPIODATA(PORT_D), pinNum);
                break;
            case PORT_E:
                *ptr_pinValue = GET_BIT_STATUS(GPIODATA(PORT_E), pinNum);
                break;
            case PORT_F:
                *ptr_pinValue = GET_BIT_STATUS(GPIODATA(PORT_F), pinNum);
                break;
        }
    } else {
        return GPIO_STATUS_INVALID_PIN_DIR;
    }
    return GPIO_STATUS_SUCCESS;
}

/**
 * @brief Reads the state of a single pin.
 * @param[in] ptr_st_GPIO_config    Address of the specified pin configuration strcuture.
 * @param[in] pinValue              The value to be written on the specified pin.
 * @return en_GPIO_error_t
 */
en_GPIO_error_t GPIO_WritePin(const st_GPIO_config_t *ptr_st_GPIO_config, uint8 pinValue) {
    
    /* Variable for holding the port number. */
    uint8 portNum = ptr_st_GPIO_config->en_GPIO_port;
    
    /* Varualbe for holding the pin number. */
    uint8 pinNum = ptr_st_GPIO_config->en_GPIO_pin;
    
    if (ptr_st_GPIO_config->en_GPIO_pinDir == OUTPUT) {
        assert(portNum == PORT_A || portNum == PORT_B || portNum == PORT_C || portNum == PORT_D || portNum == PORT_E || portNum == PORT_F);
        switch (portNum) {
            case PORT_A:
                if(pinValue == HIGH) {
                    SET_BIT(GPIODATA(PORT_A), pinNum);
                } else if(pinValue == LOW) {
                    CLR_BIT(GPIODATA(PORT_A), pinNum);
                } else {
                    //error handling
                    }
                break;
            case PORT_B:
                if(pinValue == HIGH) {
                    SET_BIT(GPIODATA(PORT_B), pinNum);
                } else if(pinValue == LOW) {
                    CLR_BIT(GPIODATA(PORT_B), pinNum);
                } else {
                    //error handling
                    }
                break;
            case PORT_C:
                if(pinValue == HIGH) {
                    SET_BIT(GPIODATA(PORT_C), pinNum);
                } else if(pinValue == LOW) {
                    CLR_BIT(GPIODATA(PORT_C), pinNum);
                } else {
                    //error handling
                    }
                break;
            case PORT_D:
                if(pinValue == HIGH) {
                    SET_BIT(GPIODATA(PORT_D), pinNum);
                } else if(pinValue == LOW) {
                    CLR_BIT(GPIODATA(PORT_D), pinNum);
                } else {
                    //error handling
                    }
                break;
            case PORT_E:
                if(pinValue == HIGH) {
                    SET_BIT(GPIODATA(PORT_E), pinNum);
                } else if(pinValue == LOW) {
                    CLR_BIT(GPIODATA(PORT_E), pinNum);
                } else {
                    //error handling
                    }
                break;
            case PORT_F:
                if(pinValue == HIGH) {
                    SET_BIT(GPIODATA(PORT_F), pinNum);
                } else if(pinValue == LOW) {
                    CLR_BIT(GPIODATA(PORT_F), pinNum);
                } else {
                    //error handling
                    }
                break;
        }
    } else {
        return GPIO_STATUS_INVALID_PIN_DIR;
    }
    return GPIO_STATUS_SUCCESS;
}
