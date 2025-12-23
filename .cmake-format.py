# -------------------------------
# Options affecting file encoding
# -------------------------------
with section("encode"):

  # If true, emit the unicode byte-order mark (BOM) at the start of the file
  emit_byteorder_mark = True

  # Specify the encoding of the input file. Defaults to utf-8
  input_encoding = 'utf-8'

  # Specify the encoding of the output file. Defaults to utf-8. Note that cmake
  # only claims to support utf-8 so be careful when using anything else
  output_encoding = 'utf-8'

# -------------------------------------
# Miscellaneous configurations options.
# -------------------------------------
with section("misc"):

  # A dictionary containing any per-command configuration overrides. Currently
  # only `command_case` is supported.
  per_command = {}

