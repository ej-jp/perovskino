def generate_dac_lookup_lists(master_start, master_end):
    # Generate forward and backward fast voltage scan
    forward_fast = list(range(master_start, master_end - 1, -5))
    backward_fast = forward_fast[::-1]



    return {
        "forward_fast": forward_fast,
        "backward_fast": backward_fast,
    }

def write_to_file(filename, lookup_lists):
    # File header content
    header_content = """#ifndef DACLOOKUP_H
#define DACLOOKUP_H
#include <avr/pgmspace.h>
"""
    footer_content = """#endif
"""

    with open(filename, 'w') as f:
        f.write(header_content)
        for i, (key, values) in enumerate(lookup_lists.items(), start=1):
            steps = len(values)
            values_str = ', '.join(map(str, values))
            f.write(f"// {key.replace('_', ' ').title()}\n")
            f.write(f"const int dac_steps{i} = {steps};\n")
            f.write(f"const PROGMEM int DACLookup_list{i}[dac_steps{i}] = {{\n")
            f.write(f"  {values_str}\n")
            f.write("};\n\n")
        f.write(footer_content)

if __name__ == "__main__":
    import sys
    if len(sys.argv) != 3:
        print("Usage: python generate_dac_lists.py <master_start> <master_end>")
        sys.exit(1)

    master_start = int(sys.argv[1])
    master_end = int(sys.argv[2])
    lookup_lists = generate_dac_lookup_lists(master_start, master_end)
    write_to_file("dac_lookup_lists4PO.h", lookup_lists)
