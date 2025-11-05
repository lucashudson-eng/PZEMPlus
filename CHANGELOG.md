# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.7.1] - 2025-11-04

### Fixed
- **RS485 Enable Pin Initialization**: Fixed `_rs485_en` initialization error by changing sentinel value from `-1` to `255` (0xFF) to properly work with `uint8_t` type
- **PZIOT-E02 Naming**: Corrected device name consistency across documentation and code

### Changed
- **Default Parameters**: Added default parameter values to setter methods for improved API usability:
  - `setAddress()`: Default address values (0x01 for PZEM003/004T/017, 0x00 for PZEM6L24)
  - `setPowerAlarm()`: Default threshold 2300.0 W
  - `setHighVoltageAlarm()`: Default threshold 300.0 V
  - `setLowVoltageAlarm()`: Default threshold 7.0 V
  - `setFrequency()`: Default frequency 50 Hz
  - `setBaudrateAndConnectionType()`: Default baudrate 9600 and connection type
  - `setCurrentRange()`: Default range 100 A

### Improved
- **API Usability**: Setter methods now have sensible defaults, making configuration easier for common use cases
- **Type Safety**: Fixed type compatibility issue with RS485 enable pin handling

## [0.7.0] - 2025-01-27

### Added
- **PZEM-014 Support**: Added PZEM-014 single-phase AC energy monitor class (inherits from PZEM-004T)
- **PZEM-016 Support**: Added PZEM-016 single-phase AC energy monitor class (inherits from PZEM-004T)
- **Complete Doxygen Documentation**: Comprehensive API documentation for all classes and methods
- **Documentation Standardization**: All functions now include @brief, @param, and @return tags
- **Class Documentation**: Added @class, @defgroup, and @name tags for better code organization
- **File Headers**: Doxygen file headers with @file, @brief, @author, and @date for all source files
- **PZEM-6L24 Combined Reading**: Added `readVoltageCurrent()` method to read voltage and current for all three phases simultaneously in a single Modbus request

### Changed
- **PZEM003017 Separation**: Separated PZEM003017 into two distinct classes:
  - **PZEM003**: Base class for DC energy monitoring (10A range, built-in shunt)
  - **PZEM017**: Derived class extending PZEM003 with current range functionality (50A-300A range, external shunt)
- **Error Return Values**: Changed error return values from `0` to `NAN` for floating-point measurement functions in PZEM003, PZEM017, and PZEM6L24
- **Return Type Optimization**: Updated return types to use appropriate integer types with sentinel error values:
  - `getAddress()`: `uint8_t` (returns 0xFF on error)
  - `getBaudrate()`: `uint32_t` (returns 0 on error)
  - `getConnectionType()`: `uint8_t` (returns 0xFF on error)
  - `getFrequency()`: `uint8_t` (returns 50 or 60 Hz, or 0 on error)
- **Parameter Simplification**: Updated parameter methods to accept direct values instead of internal constants:
  - `setFrequency()`: Now accepts `50` or `60` directly (Hz) instead of `PZEM_FREQUENCY_*` constants
  - `setBaudrateAndConnectionType()`: Now accepts baudrate value directly (2400, 4800, 9600, etc.) with automatic validation
- **Documentation Structure**: Simplified documentation in .cpp files to only @brief, keeping full documentation (with @param and @return) in .h files
- **Access Specifiers**: Changed `_slaveAddr`, `_rxPin`, and `_txPin` from `private` to `protected` in PZEM003 to allow PZEM017 inheritance
- **Library Metadata**: Updated library.json and library.properties to include PZEM-014 and PZEM-016
- **Keywords File**: Updated keywords.txt with PZEM003, PZEM017, PZEM014, and PZEM016, removed PZEM003017

### Fixed
- **Inheritance Access**: Fixed PZEM017 access to PZEM003 protected members for proper inheritance
- **Documentation Completeness**: Ensured all public methods have complete Doxygen documentation
- **File Headers**: Corrected file documentation headers to match actual class implementations

### Improved
- **Code Documentation**: Professional Doxygen documentation throughout the entire codebase
- **API Clarity**: Clear parameter and return value documentation for all public methods
- **API Usability**: More intuitive API with direct value parameters (50/60 Hz, baudrate values) instead of internal constants
- **Type Safety**: Appropriate return types for integer values (uint8_t, uint32_t) instead of float, improving type safety
- **Parameter Validation**: Automatic validation of baudrate and frequency values in setter methods
- **Library Organization**: Better structured class hierarchy with proper inheritance relationships
- **Error Handling**: Consistent error handling using NAN for floating-point measurements and sentinel values for integer parameters
- **README Documentation**: Comprehensive updates to README.md including all new devices and architecture changes
- **Developer Experience**: Improved IDE support with complete keywords.txt and syntax highlighting

## [0.6.3] - 2025-01-27

### Added
- **Unified combineRegisters Function**: Combined signed and unsigned versions of combineRegisters into a single, more efficient function
- **Enhanced Baudrate Change**: Improved changeBaudrate functionality with better error handling and validation

### Changed
- **Code Optimization**: Streamlined register combination logic by merging signed and unsigned implementations
- **Baudrate Management**: Enhanced baudrate change process with improved reliability and error detection

### Improved
- **Function Efficiency**: Reduced code duplication by combining similar register combination functions
- **Communication Stability**: Better baudrate change handling for more reliable device configuration
- **Code Maintainability**: Simplified codebase with unified register handling functions

## [0.6.2] - 2025-01-27

### Added
- **PZEM-6L24 Parameter Management**: Complete implementation of parameter reading and configuration functions
- **Configuration Functions**: Added methods to read and set address, baudrate, connection type, and frequency settings
- **Parameter Reading Methods**: `getAddress()`, `getBaudrate()`, `getConnectionType()`, `getFrequency()`, `getSoftwareHardwareSettings()`
- **Parameter Setting Methods**: `setAddress()`, `setBaudrate()`, `setBaudrateAndConnectionType()`, `setFrequency()`

### Changed
- **Error Handling Standardization**: Changed all error return values from -1 to NAN across all PZEM models for consistency
- **Code Standardization**: Improved code consistency and standardization across all device implementations
- **PZEM-6L24 Measurements**: Enhanced and corrected measurement reading functions for better accuracy and reliability

### Fixed
- **PZEM-6L24 Reading Corrections**: Fixed various measurement reading issues and improved data processing accuracy
- **Error Value Consistency**: Standardized error handling to use NAN instead of -1 for better floating-point compatibility
- **Parameter Configuration**: Improved parameter setting and reading reliability for PZEM-6L24 device

### Improved
- **Library Consistency**: Enhanced overall code consistency and maintainability across all PZEM device classes
- **Error Detection**: Better error detection and handling throughout the library
- **Documentation**: Updated internal documentation and code comments for better clarity

## [0.6.1] - 2025-01-27

### Added
- **Multi-Device Example**: Complete example demonstrating how to use multiple PZEM devices simultaneously
- **Array Management**: Example showing how to create and manage arrays of PZEMPlus objects
- **Batch Reading**: Demonstration of efficient readAll() method across multiple devices
- **Table Format Output**: Professional table formatting for multi-device measurements display

### Improved
- **Documentation**: Enhanced examples section with practical multi-device usage scenarios
- **Code Examples**: Added comprehensive multi-device example with proper error handling

## [0.6.0] - 2025-01-27

### Added
- **PZEM-6L24 Implementation**: Complete implementation for three-phase energy monitoring device
- **Three-Phase Measurements**: Support for individual phase readings (A, B, C) and combined measurements
- **Advanced Measurements**: Voltage, current, frequency, power (active, reactive, apparent), energy, and power factor
- **Phase Angle Monitoring**: Voltage and current phase angle measurements for power quality analysis
- **Multi-Phase Reading Methods**: Efficient batch reading methods for all three phases simultaneously
- **Combined Measurements**: Total power and energy calculations across all phases
- **Energy Reset Functionality**: Support for resetting energy counters by phase or all phases
- **Comprehensive Example**: Complete example demonstrating all PZEM-6L24 capabilities
- **Modular Design**: Seamless integration with existing library architecture

### Changed
- **Library Structure**: Enhanced to support three-phase monitoring devices alongside existing single-phase devices
- **Documentation**: Updated with PZEM-6L24 usage examples and configuration information

### Fixed
- **Device Compatibility**: Improved support for advanced three-phase energy monitoring applications
- **Communication Layer**: Optimized for three-phase device protocols and register mapping

## [0.5.3] - 2025-01-27

### Improved
- **Communication Optimization**: Enhanced RS485 communication with intelligent response detection
- **Timeout Management**: Optimized timeout handling for better performance and reliability
- **Response Detection**: Improved logic to detect response completion based on inactivity time
- **Code Cleanup**: Removed unused code and optimized memory usage
- **Example Enhancement**: Updated examples with better performance metrics and timeout configuration

### Fixed
- **Response Reading**: Enhanced reading logic to always wait for slaveAddr byte at the beginning of response
- **Communication Stability**: Improved RS485 communication reliability with better error handling
- **Timeout Issues**: Fixed timeout management for more consistent communication

## [0.5.2] - 2025-01-27

### Fixed
- **Byte count in writeSingle**: Fixed incorrect byte count in writeSingleRegister and resetEnergy function
- **Response reading**: Improved reading logic to always wait for slaveAddr byte at the beginning of response
- **Communication stability**: Enhanced RS485 communication reliability

## [0.5.1] - 2025-10-05

### Fixed
- Bug fixes and improvements for better stability and performance
- Enhanced error handling and communication reliability
- Improved parameter validation and data processing

### Improved
- **Examples Enhancement**: Comprehensive improvements to all example files
- **PZEM-004T Example**: Updated with better timeout configuration (100ms default)
- **PZEM-003/017 Example**: Enhanced with complete parameter testing and performance metrics
- **Address Change Tool**: Improved error handling and device compatibility
- **Documentation**: Updated README with realistic configuration examples
- **Library Properties**: Streamlined description for better clarity

## [0.5.0] - 2025-10-04

### Added
- **PZEM-003/017 Implementation**: Complete implementation for DC energy monitoring devices
- **DC Energy Monitoring**: Support for PZEM-003 (10A range, built-in shunt) and PZEM-017 (50A-300A range, external shunt)
- **Voltage Alarm System**: High and low voltage alarm configuration and monitoring
- **Current Range Configuration**: Configurable current ranges for PZEM-017 (50A, 100A, 200A, 300A)
- **DC Measurement Functions**: Voltage, current, power, and energy reading for DC applications
- **Alarm Status Reading**: Real-time alarm status monitoring for voltage thresholds
- **Parameter Management**: Complete parameter setting and reading for DC devices
- **Example Implementation**: Working example for PZEM-003/017 usage

### Changed
- **Library Structure**: Enhanced modular design to support both AC and DC monitoring devices
- **Communication Layer**: Optimized RS485 communication for DC device protocols
- **Documentation**: Updated README with DC device information and usage examples

### Fixed
- **Device Compatibility**: Improved compatibility with DC energy monitoring devices
- **Parameter Handling**: Enhanced parameter validation and error handling for DC devices

## [0.4.0] - 2025-10-04

### Added
- **RS485 Communication Optimization**: Intelligent timeout implementation with automatic response end detection
- **Data Caching System**: Cache system for optimized readings with sampling time control
- **Smart Reading Methods**: Methods that alternate between direct reading and cache based on sampling time
- **Performance Improvements**: Significant reduction in Modbus communication response time
- **Practical Examples**: New usage examples including address change and multiple devices

### Changed
- **Reception Algorithm**: Implementation of automatic response end detection based on inactivity time
- **Timeout Management**: Optimization of timeout system for better efficiency
- **Cache Architecture**: More robust cache system with data validation
- **Reading Methods**: All reading methods now support both direct and cache modes

### Fixed
- **Communication Stability**: Fixed timeout issues and data loss problems
- **Data Processing**: Improved validation and processing of Modbus responses
- **Memory Management**: Optimized memory usage for communication operations

## [0.3.0] - 2025-10-03

### Added
- **PZEM-004T Implementation**: Complete class implementation with all measurement functions
- **RS485 Base Class**: Robust Modbus RTU communication layer with CRC verification
- **Measurement Functions**: Voltage, current, power, energy, frequency, and power factor reading
- **Device Configuration**: Alarm threshold setting, address configuration, and energy reset
- **Batch Reading**: Efficient method to read all measurements in a single call
- **Error Handling**: Comprehensive error detection and timeout management

### Changed
- **Library Architecture**: Modular design with separate classes for each PZEM model
- **Communication Layer**: Enhanced RS485 implementation with optimized response handling
- **Code Organization**: Better separation of concerns between communication and device-specific logic

### Fixed
- **Communication Stability**: Improved timeout handling and response validation
- **Data Processing**: Correct handling of 32-bit values from 16-bit registers
- **Error Recovery**: Better error detection and graceful failure handling

## [0.2.0] - 2025-10-03

### Added
- Support for multiple PZEM models (PZEM-004T, PZEM-6L24, PZIOT-E02, PZEM-003, PZEM-017)
- Automatic model selection based on compilation defines
- Base RS485 communication class
- Individual classes for each PZEM model
- Improved library structure with modular design

### Changed
- Library architecture to support multiple device types
- Header file structure for better organization

## [0.1.0] - 2025-10-02

### Added
- Initial release
- Basic PZEM library structure
- Support for Peacefair energy monitoring devices
