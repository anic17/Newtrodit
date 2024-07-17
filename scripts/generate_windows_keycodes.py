# Define the keys with their codes
keys = {
    "F1": 0x70, "F2": 0x71, "F3": 0x72, "F4": 0x73, "F5": 0x74, "F6": 0x75,
    "F7": 0x76, "F8": 0x77, "F9": 0x78, "F10": 0x79, "F11": 0x7A, "F12": 0x7B,
    "UP": 0x26, "LEFT": 0x25, "RIGHT": 0x27, "DOWN": 0x28,
    "INSERT": 0x2D, "HOME": 0x24, "PGUP": 0x21, "PGDW": 0x22, "END": 0x23, "DEL": 0x2E
}

# Generate the enum
enum_entries = []

# Function to generate enum entry
def generate_entry(modifiers, key_name):
    name_parts = modifiers[:]
    if key_name:
        name_parts.append(key_name)
    name = "".join(name_parts)

    value_parts = []
    if key_name:
        value_parts.append(f"0x{keys[key_name]:02X}")
    if "CTRL" in modifiers:
        value_parts.append("CTRL_BITMASK")
    if "ALT" in modifiers:
        value_parts.append("ALT_BITMASK")
    if "SHIFT" in modifiers:
        value_parts.append("SHIFT_BITMASK")
    
    value = " | ".join(value_parts)
    enum_entries.append(f"    {name} = {value}")

# Generate all combinations
modifiers_combinations = [
    ["CTRL"], ["ALT"], ["SHIFT"],
    ["CTRL", "ALT"], ["CTRL", "SHIFT"], ["ALT", "SHIFT"],
    ["CTRL", "ALT", "SHIFT"]
]

for modifiers in modifiers_combinations:
    for key in keys:
        generate_entry(modifiers, key)

# Print the enum
print("enum KeyCodes {")
print(",\n".join(enum_entries))
print("};")
