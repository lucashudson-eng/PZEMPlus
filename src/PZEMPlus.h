/**
 * @file PZEMPlus.h
 * @brief Unified header for PZEMPlus library - selects appropriate PZEM model based on defines
 * @author Lucas Hudson
 * @date 2025
 * 
 * @details
 * This header file provides a unified interface to different PZEM models.
 * Define one of the following before including this header:
 * - PZEM_004T: For PZEM-004T single-phase module
 * - PZEM_014: For PZEM-014 single-phase module (same functionality as PZEM-004T)
 * - PZEM_016: For PZEM-016 single-phase module (same functionality as PZEM-004T)
 * - PZEM_003: For PZEM-003 DC module
 * - PZEM_017: For PZEM-017 DC module (extends PZEM-003 with current range)
 * - PZEM_6L24: For PZEM-6L24 three-phase module
 * - PZIOT_E02: For PZIOT-E02 module (pending implementation)
 * 
 * @example
 * @code
 * #define PZEM_004T
 * #include "PZEMPlus.h"
 * 
 * PZEMPlus pzem(Serial, 0xF8);
 * @endcode
 */

#ifndef PZEMPLUS_H
#define PZEMPLUS_H

#if defined(PZEM_004T)
#include "PZEM004T.h"
/** @typedef PZEMPlus
 *  @brief Type alias for PZEM004T when PZEM_004T is defined
 */
typedef PZEM004T PZEMPlus;

#elif defined(PZEM_014)
#include "PZEM014.h"
/** @typedef PZEMPlus
 *  @brief Type alias for PZEM014 when PZEM_014 is defined
 */
typedef PZEM014 PZEMPlus;

#elif defined(PZEM_016)
#include "PZEM016.h"
/** @typedef PZEMPlus
 *  @brief Type alias for PZEM016 when PZEM_016 is defined
 */
typedef PZEM016 PZEMPlus;

#elif defined(PZEM_003)
#include "PZEM003.h"
/** @typedef PZEMPlus
 *  @brief Type alias for PZEM003 when PZEM_003 is defined
 */
typedef PZEM003 PZEMPlus;

#elif defined(PZEM_017)
#include "PZEM017.h"
/** @typedef PZEMPlus
 *  @brief Type alias for PZEM017 when PZEM_017 is defined
 */
typedef PZEM017 PZEMPlus;

#elif defined(PZEM_6L24)
#include "PZEM6L24.h"
/** @typedef PZEMPlus
 *  @brief Type alias for PZEM6L24 when PZEM_6L24 is defined
 */
typedef PZEM6L24 PZEMPlus;

#elif defined(PZIOT_E02)
#include "PZIOTE02.h"
/** @typedef PZEMPlus
 *  @brief Type alias for PZIOTE02 when PZIOT_E02 is defined
 */
typedef PZIOTE02 PZEMPlus;

#else
#error "Please define Peacefair PZEM model: PZEM_004T, PZEM_014, PZEM_016, PZEM_003, PZEM_017, PZEM_6L24, or PZIOT_E02"
#endif

#endif // PZEMPLUS_H
