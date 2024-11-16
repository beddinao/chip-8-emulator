Here’s a deep dive into the Chip-8 instructions, including what they do, what components they modify, and the conditions they rely on:



---

### **System Instructions**
#### `00E0` - Clear the display
- **What it does:** Clears the screen by setting all pixels in the display array to 0.
- **Modifies:** The display array.
- **Conditions:** None, it’s unconditional.

#### `00EE` - Return from a subroutine
- **What it does:** Pops the top address from the stack and sets the PC to that address.
- **Modifies:** The PC and the SP.
- **Conditions:** The stack must not be empty; relies on the SP for proper stack operation.

---

### **Jump and Call Instructions**
#### `1nnn` - Jump to address `nnn`
- **What it does:** Sets the PC to `nnn`, effectively jumping to that memory location.
- **Modifies:** The PC.
- **Conditions:** None, it’s unconditional.

#### `2nnn` - Call subroutine at `nnn`
- **What it does:** Pushes the current PC onto the stack, increments the SP, and sets the PC to `nnn`.
- **Modifies:** The PC, SP, and stack.
- **Conditions:** The stack must not overflow.

#### `Bnnn` - Jump to address `nnn + V0`
- **What it does:** Sets the PC to `nnn` plus the value in `V0`.
- **Modifies:** The PC.
- **Conditions:** Relies on `V0` to determine the target address.

---

### **Conditional Instructions**
#### `3xkk` - Skip next instruction if `Vx == kk`
- **What it does:** Compares the value in `Vx` with `kk`. If they’re equal, increments the PC by 2 (skipping the next instruction).
- **Modifies:** The PC.
- **Conditions:** Relies on the value in `Vx`.

#### `4xkk` - Skip next instruction if `Vx != kk`
- **What it does:** Compares `Vx` with `kk`. If they’re not equal, increments the PC by 2.
- **Modifies:** The PC.
- **Conditions:** Relies on the value in `Vx`.

#### `5xy0` - Skip next instruction if `Vx == Vy`
- **What it does:** Compares `Vx` and `Vy`. If they’re equal, increments the PC by 2.
- **Modifies:** The PC.
- **Conditions:** Relies on the values in `Vx` and `Vy`.

#### `9xy0` - Skip next instruction if `Vx != Vy`
- **What it does:** Compares `Vx` and `Vy`. If they’re not equal, increments the PC by 2.
- **Modifies:** The PC.
- **Conditions:** Relies on the values in `Vx` and `Vy`.

---
- (0-4)   opcode & 0xF000 |
- (4-8)   opcode & 0x0F00 >> 8 |
- (8-12)  opcode & 0x00F0 >> 4 |
- (12-16) opcode & 0x000F

### **Data Manipulation Instructions**
#### `6xkk` - Load `kk` into `Vx`
- **What it does:** Sets `Vx` to `kk`.
- **Modifies:** `Vx`.
- **Conditions:** None, it’s unconditional.

#### `7xkk` - Add `kk` to `Vx`
- **What it does:** Adds `kk` to the current value of `Vx`. (No carry flag is used here.)
- **Modifies:** `Vx`.
- **Conditions:** None, it’s unconditional.

#### `8xy0` - Load `Vy` into `Vx`
- **What it does:** Copies the value of `Vy` into `Vx`.
- **Modifies:** `Vx`.
- **Conditions:** None, it’s unconditional.

#### `8xy1` - Bitwise OR `Vx` with `Vy`
- **What it does:** Performs a bitwise OR between `Vx` and `Vy`, stores the result in `Vx`.
- **Modifies:** `Vx`.
- **Conditions:** None.

#### `8xy2` - Bitwise AND `Vx` with `Vy`
- **What it does:** Performs a bitwise AND between `Vx` and `Vy`, stores the result in `Vx`.
- **Modifies:** `Vx`.
- **Conditions:** None.

#### `8xy3` - Bitwise XOR `Vx` with `Vy`
- **What it does:** Performs a bitwise XOR between `Vx` and `Vy`, stores the result in `Vx`.
- **Modifies:** `Vx`.
- **Conditions:** None.

#### `8xy4` - Add `Vy` to `Vx`, set `VF` on carry
- **What it does:** Adds `Vy` to `Vx`. If the result exceeds 8 bits, sets `VF` to 1; otherwise, sets `VF` to 0. Keeps only the least significant 8 bits in `Vx`.
- **Modifies:** `Vx` and `VF`.
- **Conditions:** Relies on the sum of `Vx` and `Vy`.

#### `8xy5` - Subtract `Vy` from `Vx`, set `VF` on no borrow
- **What it does:** Subtracts `Vy` from `Vx`. If `Vx >= Vy`, sets `VF` to 1; otherwise, sets `VF` to 0.
- **Modifies:** `Vx` and `VF`.
- **Conditions:** Relies on the values in `Vx` and `Vy`.

#### `8xy6` - Right-shift `Vx` by 1, set `VF` to least significant bit
- **What it does:** Sets `VF` to the least significant bit of `Vx`, then divides `Vx` by 2.
- **Modifies:** `Vx` and `VF`.
- **Conditions:** Relies on the value in `Vx`.

#### `8xyE` - Left-shift `Vx` by 1, set `VF` to most significant bit
- **What it does:** Sets `VF` to the most significant bit of `Vx`, then multiplies `Vx` by 2.
- **Modifies:** `Vx` and `VF`.
- **Conditions:** Relies on the value in `Vx`.

---

### **Memory Instructions**
#### `Annn` - Set IR to `nnn`
- **What it does:** Sets the IR to `nnn`.
- **Modifies:** IR.
- **Conditions:** None, it’s unconditional.

#### `Fx1E` - Add `Vx` to IR
- **What it does:** Adds the value of `Vx` to the IR. If the result overflows 0xFFF, behavior depends on the implementation.
- **Modifies:** IR.
- **Conditions:** Relies on the value in `Vx`.

#### `Fx55` - Store registers `V0` to `Vx` in memory starting at `IR`
- **What it does:** Copies the values of registers `V0` to `Vx` into memory starting at the address stored in `IR`.
- **Modifies:** Memory starting at `IR`.
- **Conditions:** Relies on the value of `IR`.

#### `Fx65` - Load registers `V0` to `Vx` from memory starting at `IR`
- **What it does:** Reads values from memory starting at `IR` into registers `V0` to `Vx`.
- **Modifies:** Registers `V0` to `Vx`.
- **Conditions:** Relies on the value of `IR`.

---

### **Timers and Input Instructions**
#### `Fx07` - Load `DT` into `Vx`
- **What it does:** Copies the value of the delay timer into `Vx`.
- **Modifies:** `Vx`.
- **Conditions:** Relies on the value of `DT`.

#### `Fx15` - Set `DT` to `Vx`
- **What it does:** Sets the delay timer to the value of `Vx`.
- **Modifies:** `DT`.
- **Conditions:** None.

#### `Fx18` - Set `ST` to `Vx`
- **What it does:** Sets the sound timer to the value of `Vx`.
- **Modifies:** `ST`.
- **Conditions:** None.

#### `Ex9E` - Skip next instruction if key `Vx` is pressed
- **What it does:** Checks if the key corresponding to the value in `Vx` is pressed. If yes, increments the PC by 2.
- **Modifies:** The PC.
- **Conditions:** Relies on the key state and `Vx`.

#### `ExA1` - Skip next instruction if key `Vx` is not pressed
- **What it does:** Checks if the key corresponding to the value in `Vx` is not pressed. If not, increments the PC by 2.
- **Modifies:** The PC.
- **Conditions:** Relies on the key state and `Vx`.

---

Let me know if you need clarification on any specific instruction!
