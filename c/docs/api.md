# Enigma V300 Pure C - API Reference

## Core Functions

### Machine Initialization

```c
// Initialize an Enigma machine with default configuration
enigma_machine_t* enigma_init(void);

// Destroy an Enigma machine and free resources
void enigma_destroy(enigma_machine_t* machine);
```

### Configuration

```c
// Set rotor configuration
void enigma_set_rotors(enigma_machine_t* machine, uint8_t rotor1, uint8_t rotor2, uint8_t rotor3);

// Configure the reflector
void enigma_set_reflector(enigma_machine_t* machine, uint8_t reflector_type);

// Set plugboard wiring
void enigma_set_plugboard(enigma_machine_t* machine, const char* wiring);

// Set initial rotor positions
void enigma_set_positions(enigma_machine_t* machine, char pos1, char pos2, char pos3);
```

### Message Processing

```c
// Encrypt or decrypt a single character
char enigma_process_char(enigma_machine_t* machine, char input);

// Process a complete message
char* enigma_process_message(enigma_machine_t* machine, const char* message);

// Process message in-place (modifies input buffer)
void enigma_process_message_inplace(enigma_machine_t* machine, char* message);
```

### State Management

```c
// Get current rotor positions
void enigma_get_positions(enigma_machine_t* machine, char* pos1, char* pos2, char* pos3);

// Reset machine to initial state
void enigma_reset(enigma_machine_t* machine);
```

## Data Types

### enigma_machine_t
Main structure representing an Enigma machine instance, containing all rotor, reflector, and plugboard configurations.

## Return Values

Functions return appropriate status codes or pointers. Check individual function documentation for specific return value handling.
