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
 * - PZEM_003 or PZEM_017: For PZEM-003 or PZEM-017 modules
 * - PZEM_6L24: For PZEM-6L24 three-phase module
 * - PZEM_IOT_E02: For PZEM IoT-E02 module (pending implementation)
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

#elif defined(PZEM_003) || defined(PZEM_017)
#include "PZEM003017.h"
/** @typedef PZEMPlus
 *  @brief Type alias for PZEM003017 when PZEM_003 or PZEM_017 is defined
 */
typedef PZEM003017 PZEMPlus;

#elif defined(PZEM_6L24)
#include "PZEM6L24.h"
/** @typedef PZEMPlus
 *  @brief Type alias for PZEM6L24 when PZEM_6L24 is defined
 */
typedef PZEM6L24 PZEMPlus;

#elif defined(PZEM_IOT_E02)
#include "PZEMIOTE02.h"
/** @typedef PZEMPlus
 *  @brief Type alias for PZEMIOTE02 when PZEM_IOT_E02 is defined
 */
typedef PZEMIOTE02 PZEMPlus;

#else
#error "Please define Peacefair PZEM model: PZEM_004T, PZEM_003, PZEM_017, PZEM_6L24, or PZEM_IOT_E02"
#endif

#endif // PZEMPLUS_H
