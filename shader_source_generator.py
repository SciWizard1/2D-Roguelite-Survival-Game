#!/usr/bin/env python3
import os, sys

if len(sys.argv) != 3:
    print("Usage: shader_source_generator.py <input_dir> <output_header>")
    sys.exit(1)

input_dir = sys.argv[1]
output_file = sys.argv[2]

with open(output_file, "w", encoding="utf-8") as out:
    out.write("// Auto-generated shader sources\n\n")
    for filename in os.listdir(input_dir):
        if filename.endswith(".glsl"):
            path = os.path.join(input_dir, filename)
            var_name = os.path.splitext(filename)[0]
            with open(path, "r", encoding="utf-8") as f:
                code = f.read()
            escaped = code.replace("\\", "\\\\").replace("\"", "\\\"").replace("\n", "\\n\"\n\"")
            out.write(f"const char* {var_name}_src = \"{escaped}\";\n\n")

print(f"[Embed] Shaders written to {output_file}")
